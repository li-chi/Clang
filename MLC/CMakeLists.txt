set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MLC
  MLC.cpp
  )
target_link_libraries(MLC
  clangTooling
  clangBasic
  clangASTMatchers
  )