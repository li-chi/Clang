set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MIEB
  MIEB.cpp
  )
target_link_libraries(MIEB
  clangTooling
  clangBasic
  clangASTMatchers
  )