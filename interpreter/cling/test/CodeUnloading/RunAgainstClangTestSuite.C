//RUN: cd %testexecdir/../../clang/test && CLANG=%p/clangTestUnloader.sh python %llvmsrcdir/utils/lit/lit.py -v -sv --param clang_site_config=%testexecdir/../../clang/test/lit.site.cfg %p/../../../clang/test/Sema/address-constant.c


// Current SemaCXX failures:
// Expected Passes    : 392
// Expected Failures  : 1
// Unexpected Failures: 184

// Current Sema failures:
// Expected Passes    : 420
// Unsupported Tests  : 2
// Unexpected Failures: 74
