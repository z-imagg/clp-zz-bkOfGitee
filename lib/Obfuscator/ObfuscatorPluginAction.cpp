//==============================================================================
// FILE:
//    Obfuscator.cpp
//
// DESCRIPTION:
//  Obfuscates the integer addition (`+`) and subtraction (`-`) expressions as
//  follows:
//
//    * a + b --> (a ^ b) + 2 * (a & b)
//    * a - b --> (a + ~b) + 1
//
//  Either of the operands can be a literal or a variable declared earlier. The
//  modified file is printed to stdout. Additions and subtractions are modified
//  separately, i.e. the plugin runs twice over the input file. First time it
//  modifies the additions, second time it modifies the subtractions.
//
// USAGE:
//    * clang -cc1 -load <BUILD_DIR>/lib/libObfuscator.dylib `\`
//        -plugin Obfuscator test/MBA_add_int.cpp
//
// License: The Unlicense
//==============================================================================
#include "Obfuscator/ObfuscatorASTConsumer.h"

#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>

using namespace clang;
using namespace ast_matchers;





//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class ObfuscatorPluginAction : public PluginASTAction {
public:
    ObfuscatorPluginAction()
            : RewriterForObfuscator{std::make_shared<Rewriter>()} {}
    // Our plugin can alter behavior based on the command line options
    bool ParseArgs(const CompilerInstance &,
                   const std::vector<std::string> &) override {
      return true;
    }

    // Returns our ASTConsumer per translation unit.
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef file) override {
      RewriterForObfuscator->setSourceMgr(CI.getSourceManager(),
                                          CI.getLangOpts());
      return std::make_unique<ObfuscatorASTConsumer>(RewriterForObfuscator);
    }

    void EndSourceFileAction() override {
      // Output to stdout (via llvm::outs()
      RewriterForObfuscator
              ->getEditBuffer(RewriterForObfuscator->getSourceMgr().getMainFileID())
              .write(llvm::outs());
    }

private:
    std::shared_ptr<Rewriter> RewriterForObfuscator;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<ObfuscatorPluginAction>
        X(/*Name=*/"Obfuscator",
        /*Desc=*/"Mixed Boolean Algebra Transformations");