
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

//std::string varName;
bool counting = true;
int count = 0;
int choose;

class VarDeclHandler : public MatchFinder::MatchCallback {
public:
    VarDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) {
      const BinaryOperator *bo = Result.Nodes.getNodeAs<clang::BinaryOperator>("bo");
      const VarDecl *vd = Result.Nodes.getNodeAs<clang::VarDecl>("vd");
      std::string str;

      if(vd->getType()->isIntegerType()) {
        if(counting) {
          count++;
          return;
        } else {
          count++;
          if(count != choose) {
            return;
          }
        }
        srand(time(NULL));
        int r = rand() % 1000;
        str = vd->getNameAsString() + " = " + std::to_string(r) + ";//wrong value\n//";

      } else if (vd->getType()->isFloatingType()) {
        if(counting) {
         count++;
         return;
       } else {
          count++;
          if(count != choose) {
           return;
          }
        }
        srand(time(NULL));
        int r = rand() & 1000;
        str = vd->getNameAsString() + " = " + std::to_string(r) + ".0;//wrong value\n//";
      } else {
        return;
      }

      SourceLocation sl = bo->getSourceRange().getBegin();
    
      SourceLocation des = sl;
      while (Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true).isInvalid()) {
          //des = Lexer::getLocForEndOfToken(sl, 0, Rewrite.getSourceMgr(), Rewrite.getLangOpts());
          des = des.getLocWithOffset(-1);
      }
      des = Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true); 
      

      Rewrite.InsertTextAfter(des, str);
    
    }
    
private:
    Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(Rewriter &R) : VD(R) {

    
        Matcher.addMatcher(binaryOperator(hasOperatorName("="),
                                          hasLHS(ignoringParenImpCasts(declRefExpr(to(
                                               varDecl().bind("vd")))))).bind("bo"),&VD); 
    }
    
    void HandleTranslationUnit(ASTContext &Context) override {
        // Run the matchers when we have the whole TU parsed.
        Matcher.matchAST(Context);
    }
    
private:
    MatchFinder Matcher;
    VarDeclHandler VD;

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
    /*
    std::cout << "Enter variable name: " << std::endl;
    std::getline (std::cin,varName);
    */
    CommonOptionsParser op(argc, argv, MatcherSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());
    Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
    
    counting = false;
    srand(time(NULL));
    choose = rand() % count + 1;
    count = 0;

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
