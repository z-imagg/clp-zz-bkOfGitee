#include <clang/Rewrite/Core/Rewriter.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CodeStyleCheckerASTConsumer(clang::Rewriter &R, clang::ASTContext *Context,
                                         clang::SourceManager &SM)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            : Visitor(R, Context),
            SM(SM)  {}

    void insertInclude(clang::ASTContext &Context, clang::Rewriter& rewriter)   {
      clang::SourceManager &SM = Context.getSourceManager();
      clang::FileID MainFileID = SM.getMainFileID();
      clang::SourceLocation Loc = SM.getLocForStartOfFile(MainFileID);

      // Get the rewritten buffer for the main file
      const clang::RewriteBuffer *RewriteBuf = rewriter.getRewriteBufferFor(MainFileID);
      if (!RewriteBuf)
        return;

      // Find the location to insert the include directive
//      unsigned Offset = Lexer::MeasureIndentation(SM.getBufferData(MainFileID), Loc, SM);
      std::string IncludeCode = "#include \"t_clock_tick.h\"\n";

      // Insert the include directive
      rewriter.InsertText(Loc, IncludeCode, true, true);
    }

    virtual void HandleTranslationUnit(clang::ASTContext &Ctx) override{
      clang::TranslationUnitDecl* translationUnitDecl=Ctx.getTranslationUnitDecl();
      Visitor.TraverseDecl(translationUnitDecl);

      insertInclude(Ctx, Visitor.mRewriter);

//      Visitor.mRewriter.getEditBuffer(SM.getMainFileID())
//              .write(llvm::outs());

        //不在这里写出修改，而是到 函数 EndSourceFileAction 中去 写出修改
      Visitor.mRewriter.overwriteChangedFiles();//修改会影响原始文件


    }

private:
    CodeStyleCheckerVisitor Visitor;
    clang::SourceManager &SM;
    // Should this plugin be only run on the main translation unit?
};
