// Linked into build/<version>/d3 only. tools/check requires a .rela.dyn
// section in the output ELF; a tree that has no global-data relocations yet
// (early on) would otherwise lack one. This emits one R_AARCH64_RELATIVE.
// It adds no functions, so it never affects what tools/check compares.
static int d3_anchor_target;
extern "C" void* const d3_link_anchor = &d3_anchor_target;
