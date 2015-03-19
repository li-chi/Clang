
#include <string>
#include <iostream>
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Lex/lexer.h"
#include "clang/Lex/Token.h"
#include <stdlib.h>  

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

bool counting = true;
int count = 0;
int choose;

class IfHandler : public MatchFinder::MatchCallback {
public:
    IfHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) {

      const BinaryOperator *bo = Result.Nodes.getNodeAs<BinaryOperator>("bo");
      printf("found\n\n");

      
      if(counting) {
        count++;
        return;
      } else {
        count++;
        if(count != choose) {
          return;
        }
      }

      SourceLocation sl = bo->getOperatorLoc();
      std::string str = "/* missing clause: ";
      Rewrite.InsertTextAfter(sl, str);

      sl = bo->getLocEnd();
      sl = sl.getLocWithOffset(1);
      str = " */";
      Rewrite.InsertTextAfter(sl, str);
        
      
    }
    
private:
    Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(Rewriter &R) : Handler(R) {

        
        Matcher.addMatcher(ifStmt(hasCondition(binaryOperator(
                                                               hasOperatorName("&&")).bind("bo"))),&Handler);
        Matcher.addMatcher(ifStmt(hasCondition(binaryOperator(
                                                               hasOperatorName("||")).bind("bo"))),&Handler);
        //ignoringImpCasts(declRefExpr(to(functionDecl().bind("fun2"))));
    }
    
    void HandleTranslationUnit(ASTContext &Context) override {
        // Run the matchers when we have the whole TU parsed.
        Matcher.matchAST(Context);
    }
    
private:
    MatchFinder Matcher;
    IfHandler Handler;

};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
    MyFrontendAction() {}
    void EndSourceFileAction() override {
        if(counting) return;
        TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
    }
    
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef file) override {
        TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return llvm::make_unique<MyASTConsumer>(TheRewriter);
    }
    
private:
    Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, MatcherSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());
    
    Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
    
    counting = false;
    srand(time(NULL));
    choose = rand() % count + 1;
    count = 0;

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
