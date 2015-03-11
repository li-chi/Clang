
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

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

std::string funName;


class IfHandler : public MatchFinder::MatchCallback {
public:
    IfHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) {
      const IfStmt *ifStmt = Result.Nodes.getNodeAs<IfStmt>("if");
      if (!ifStmt->getElse()) {
        SourceLocation sl = ifStmt->getLocStart();
        std::string str = "/* missing if construct:\n";
        Rewrite.InsertTextAfter(sl, str);
        const Expr *cond = ifStmt->getCond();
        sl = cond->getLocEnd();
        SourceLocation des = sl;
        while (Lexer::findLocationAfterToken(des, tok::r_paren, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true).isInvalid()) {
          //des = Lexer::getLocForEndOfToken(sl, 0, Rewrite.getSourceMgr(), Rewrite.getLangOpts());
          des = des.getLocWithOffset(-1);
        }
        des = Lexer::findLocationAfterToken(des, tok::r_paren, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true); 
        str = " end */";
        Rewrite.InsertTextAfter(des, str);
      }
    	
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

    
        Matcher.addMatcher(ifStmt().bind("if"),&Handler);
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
    
    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
