set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MVIV
  MVIV.cpp
  )
target_link_libraries(MVIV
  clangTooling
  clangBasic
  clangASTMatchers
  )