set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(WAEP
  WAEP.cpp
  )
target_link_libraries(WAEP
  clangTooling
  clangBasic
  clangASTMatchers
  )