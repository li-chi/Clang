
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


using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

bool counting = true;
int count = 0;
int choose;

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");


class FunctionCallHandler : public MatchFinder::MatchCallback {
public:
    FunctionCallHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) {
      
      const BinaryOperator *bo = Result.Nodes.getNodeAs<BinaryOperator>("bo");
      if(bo->isMultiplicativeOp() || bo->isAdditiveOp()){

      } else {
        return;
      }

      if(counting) {
        count++;
        return;
      } else {
        count++;
        if(count != choose) {
          return;
        }
      }
  
      const CallExpr *call = Result.Nodes.getNodeAs<CallExpr>("call");
      
      std::string str,str_;

      if(bo->getOpcode() == clang::BinaryOperatorKind::BO_Add) {
        str = "//change + to - :\n";
        str_ = "-";
      } else if(bo->getOpcode() == clang::BinaryOperatorKind::BO_Sub) {
        str = "//change - to + :\n";
        str_ = "+";
      } else if(bo->getOpcode() == clang::BinaryOperatorKind::BO_Mul) {
        str = "//change * to / :\n";
        str_ = "/";
      } else if(bo->getOpcode() == clang::BinaryOperatorKind::BO_Div) {
        str = "//change / to * :\n";
        str_ = "*";
      }

      
      SourceLocation des = call->getLocStart();
      SourceLocation des_ = bo->getOperatorLoc();
      /*
      while (Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true).isInvalid()) {
        //des = Lexer::getLocForEndOfToken(sl, 0, Rewrite.getSourceMgr(), Rewrite.getLangOpts());
        des = des.getLocWithOffset(1);
      }
      des = Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true); 
      des = des.getLocWithOffset(-1);
      std::string str = "// replace " + varName + " with " + newName;

      //std::cout << "fun: " <<str<< std::endl;
      
      */

      Rewrite.InsertTextAfter(des, str);
      Rewrite.ReplaceText(des_,1,str_);
      
    }
    
private:
    Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(Rewriter &R) : Fun(R) {

    
        Matcher.addMatcher(callExpr(hasAnyArgument(binaryOperator().bind("bo"))).bind("call"),&Fun);
    }
    
    void HandleTranslationUnit(ASTContext &Context) override {
        // Run the matchers when we have the whole TU parsed.
        Matcher.matchAST(Context);
    }
    
private:
    MatchFinder Matcher;
    FunctionCallHandler Fun;

};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
    MyFrontendAction() {}
    void EndSourceFileAction() override {
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
