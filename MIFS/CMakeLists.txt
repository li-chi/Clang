set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MIFS
  MIFS.cpp
  )
target_link_libraries(MIFS
  clangTooling
  clangBasic
  clangASTMatchers
  )