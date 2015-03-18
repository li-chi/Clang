set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MVAE
  MVAE.cpp
  )
target_link_libraries(MVAE
  clangTooling
  clangBasic
  clangASTMatchers
  )