// LatePipe: clang 5.0.1 plugin that adds LLVM 6.0's late GlobalOpt step to -O3.
//
// What it does. Stock clang 5.0.1 runs GlobalOpt (and GlobalDCE) only before the
// inliner. LLVM 6.0.0 added a second GlobalOpt + GlobalDCE right after "Deduce
// function attributes in RPO", followed by a fresh GlobalsAA, in
// PassManagerBuilder::populateModulePassManager. This plugin registers those three
// passes at EP_VectorizerStart, which on LLVM 5.0.1 is the same point (only
// Float2Int runs between the two), so the pass order emulates LLVM 6's placement on
// the pinned LLVM 5.0.1 compiler. Nothing else in the pipeline changes.
//
// Why. The original objects ("Rynda 1.4.9 based on LLVM 5.0.1" in their DWARF) look
// as if GlobalOpt also ran after inlining: file statics whose last load was inlined
// away are deleted, statics are split into the original's `.N.M` names, empty
// implicit destructors registered with __cxa_atexit vanish. Over all 1,492 measured
// sources this step makes 38 more functions match and loses none outside three
// files whose source was tuned to the stock pipeline. The vendor change itself is
// not verified (no fork sources). See docs/late_globalopt.md.
//
// Loaded by CMakeLists.txt with -fplugin=<build/latepipe/<hash>/LatePipe.so>.
// Build and pin: toolchain/latepipe/latepipe.py (the binary's SHA-256 is pinned in
// toolchain/latepipe/latepipe.lock.json; any change to this file needs a new pin).
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

static void addLateGlobalOpt(const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
  // LLVM 6.0 guards this with OptLevel > 1 inside the !PrepareForLTO path; the
  // vectorizer extension point is only reached by the -O2/-O3 module pipeline.
  if (Builder.OptLevel <= 1)
    return;
  PM.add(createGlobalOptimizerPass());
  PM.add(createGlobalDCEPass());
  PM.add(createGlobalsAAWrapperPass());
}

static RegisterStandardPasses LateGlobalOpt(PassManagerBuilder::EP_VectorizerStart, addLateGlobalOpt);
