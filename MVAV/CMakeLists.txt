set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MVAV
  MVAV.cpp
  )
target_link_libraries(MVAV
  clangTooling
  clangBasic
  clangASTMatchers
  )