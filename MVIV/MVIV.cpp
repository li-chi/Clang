
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

std::string varName;


class VarDeclHandler : public MatchFinder::MatchCallback {
public:
    VarDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) {
      const VarDecl *vd = Result.Nodes.getNodeAs<clang::VarDecl>("vd");
      SourceLocation sl = vd->getSourceRange().getBegin();
        
        SourceLocation des = sl;
        while (Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true).isInvalid()) {
          //des = Lexer::getLocForEndOfToken(sl, 0, Rewrite.getSourceMgr(), Rewrite.getLangOpts());
          des = des.getLocWithOffset(-1);
        }
        des = Lexer::findLocationAfterToken(des, tok::semi, Rewrite.getSourceMgr(), 
                                    Rewrite.getLangOpts(), true); 
      std::string str = vd->getType().getAsString() + " " + vd->getNameAsString() + "; // init var \n"
        + "// Missing init var " + vd->getNameAsString() + ": ";

      //std::cout << "fun: " <<str<< std::endl;

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

    
        Matcher.addMatcher(varDecl(hasName(varName)).bind("vd"),&VD);
        //ignoringImpCasts(declRefExpr(to(functionDecl().bind("fun2"))));
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
    std::cout << "Enter variable name: " << std::endl;
    std::getline (std::cin,varName);
    CommonOptionsParser op(argc, argv, MatcherSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());
    
    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
