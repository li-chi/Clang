set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(WVAV
  WVAV.cpp
  )
target_link_libraries(WVAV
  clangTooling
  clangBasic
  clangASTMatchers
  )