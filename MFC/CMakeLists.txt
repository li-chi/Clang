set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(MFC
  MFC.cpp
  )
target_link_libraries(MFC
  clangTooling
  clangBasic
  clangASTMatchers
  )