// Export Ghidra's decompiled C for every function in functions.csv, grouped
// into one file per original source file (from function_files.csv).
// Run through tools/ghidra_export.py. Script args: <data dir> <output dir>
//@category D3Decomp

import java.io.BufferedReader;
import java.io.File;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.decompiler.parallel.DecompileConfigurer;
import ghidra.app.decompiler.parallel.DecompilerCallback;
import ghidra.app.decompiler.parallel.ParallelDecompiler;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.util.task.TaskMonitor;

public class ExportDecomp extends GhidraScript {
    private static final String NO_FILE = "_no_dwarf.c";

    private List<String[]> readCsv(File f) throws Exception {
        List<String[]> rows = new ArrayList<>();
        try (BufferedReader r = Files.newBufferedReader(f.toPath(), StandardCharsets.UTF_8)) {
            r.readLine();
            for (String line = r.readLine(); line != null; line = r.readLine())
                rows.add(line.split(",", 4));
        }
        return rows;
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File dataDir = new File(args[0]);
        File outDir = new File(args[1]);

        Map<Long, String> fileOf = new HashMap<>();
        for (String[] row : readCsv(new File(dataDir, "function_files.csv")))
            fileOf.put(Long.decode(row[0]), row[1].isEmpty() ? NO_FILE : row[1]);

        Map<Long, String> header = new HashMap<>();
        List<Function> funcs = new ArrayList<>();
        List<String> missing = new ArrayList<>();
        for (String[] row : readCsv(new File(dataDir, "functions.csv"))) {
            long addr = Long.decode(row[0]);
            header.put(addr, "// " + row[0] + "  size " + Integer.parseInt(row[2]) + "  " + row[3]);
            Function fn = getFunctionAt(toAddr(addr));
            if (fn == null)
                missing.add(row[0] + "," + row[3]);
            else
                funcs.add(fn);
        }
        println("decompiling " + funcs.size() + " functions (" + missing.size() + " not defined in Ghidra)");

        ConcurrentHashMap<Long, String> code = new ConcurrentHashMap<>();
        AtomicInteger done = new AtomicInteger();
        AtomicInteger failed = new AtomicInteger();
        DecompilerCallback<Void> callback = new DecompilerCallback<Void>(currentProgram, new DecompileConfigurer() {
            @Override
            public void configure(DecompInterface decompiler) {
                DecompileOptions options = new DecompileOptions();
                options.grabFromProgram(currentProgram);
                decompiler.setOptions(options);
                decompiler.toggleCCode(true);
                decompiler.toggleSyntaxTree(false);
                decompiler.setSimplificationStyle("decompile");
            }
        }) {
            @Override
            public Void process(DecompileResults results, TaskMonitor m) {
                long addr = results.getFunction().getEntryPoint().getOffset();
                if (results.decompileCompleted()) {
                    code.put(addr, results.getDecompiledFunction().getC());
                } else {
                    failed.incrementAndGet();
                    code.put(addr, "// decompile failed: " + results.getErrorMessage().trim() + "\n");
                }
                int n = done.incrementAndGet();
                if (n % 2000 == 0)
                    println("  " + n + " done");
                return null;
            }
        };
        callback.setTimeout(180);
        try {
            ParallelDecompiler.decompileFunctions(callback, currentProgram, funcs.iterator(), r -> {}, monitor);
        } finally {
            callback.dispose();
        }

        Map<String, TreeMap<Long, String>> byFile = new TreeMap<>();
        for (Map.Entry<Long, String> e : code.entrySet())
            byFile.computeIfAbsent(fileOf.getOrDefault(e.getKey(), NO_FILE), k -> new TreeMap<>())
                  .put(e.getKey(), e.getValue());
        for (Map.Entry<String, TreeMap<Long, String>> f : byFile.entrySet()) {
            File out = new File(outDir, f.getKey());
            out.getParentFile().mkdirs();
            try (PrintWriter w = new PrintWriter(out, "UTF-8")) {
                for (Map.Entry<Long, String> e : f.getValue().entrySet()) {
                    w.println(header.get(e.getKey()));
                    w.println(e.getValue().trim());
                    w.println();
                }
            }
        }
        try (PrintWriter w = new PrintWriter(new File(outDir, "_missing.csv"), "UTF-8")) {
            w.println("Address,Name");
            for (String m : missing)
                w.println(m);
        }
        println("wrote " + code.size() + " functions to " + byFile.size() + " files in " + outDir
                + " (" + failed.get() + " failed, " + missing.size() + " missing)");
    }
}
