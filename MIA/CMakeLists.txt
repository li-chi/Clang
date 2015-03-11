set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MIA
  MIA.cpp
  )
target_link_libraries(MIA
  clangTooling
  clangBasic
  clangASTMatchers
  )