
#include <clang/Rewrite/Core/Rewriter.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CodeStyleCheckerVisitor
        : public clang::RecursiveASTVisitor<CodeStyleCheckerVisitor> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CodeStyleCheckerVisitor(clang::Rewriter &R, clang::ASTContext *Ctx)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter(R),
    Ctx(Ctx)
    {

    }

    //下面4个形如 bool VisitZzz(clang:Zzz *Decl)  的方法:, 其中Zzz的完整列表叙述 从 本文件第27行开始.
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
    bool VisitFunctionDecl(clang::FunctionDecl *Decl);
    bool VisitVarDecl(clang::VarDecl *Decl);
    bool VisitFieldDecl(clang::FieldDecl *Decl);
/*  类 RecursiveASTVisitor 中 有大量 形如 bool VisitZzz(clang:Zzz *Decl) 的方法, 其中Zzz即是可以被Visit的语法节点, Zzz列表完整如下:
grep DEF_TRAVERSE_DECL   '/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h'
DEF_TRAVERSE_DECL(AccessSpecDecl
DEF_TRAVERSE_DECL(BlockDecl
DEF_TRAVERSE_DECL(CapturedDecl
DEF_TRAVERSE_DECL(EmptyDecl
DEF_TRAVERSE_DECL(LifetimeExtendedTemporaryDecl
DEF_TRAVERSE_DECL(FileScopeAsmDecl
DEF_TRAVERSE_DECL(ImportDecl
DEF_TRAVERSE_DECL(FriendDecl
DEF_TRAVERSE_DECL(FriendTemplateDecl
DEF_TRAVERSE_DECL(ClassScopeFunctionSpecializationDecl
DEF_TRAVERSE_DECL(LinkageSpecDecl
DEF_TRAVERSE_DECL(ExportDecl
DEF_TRAVERSE_DECL(ObjCPropertyImplDecl
DEF_TRAVERSE_DECL(StaticAssertDecl
DEF_TRAVERSE_DECL(TranslationUnitDecl
DEF_TRAVERSE_DECL(PragmaCommentDecl
DEF_TRAVERSE_DECL(PragmaDetectMismatchDecl
DEF_TRAVERSE_DECL(ExternCContextDecl
DEF_TRAVERSE_DECL(NamespaceAliasDecl
DEF_TRAVERSE_DECL(LabelDecl
DEF_TRAVERSE_DECL(ObjCCompatibleAliasDecl
DEF_TRAVERSE_DECL(ObjCCategoryDecl
DEF_TRAVERSE_DECL(ObjCCategoryImplDecl
DEF_TRAVERSE_DECL(ObjCImplementationDecl
DEF_TRAVERSE_DECL(ObjCInterfaceDecl
DEF_TRAVERSE_DECL(ObjCProtocolDecl
DEF_TRAVERSE_DECL(ObjCMethodDecl
DEF_TRAVERSE_DECL(ObjCTypeParamDecl
DEF_TRAVERSE_DECL(ObjCPropertyDecl
DEF_TRAVERSE_DECL(UsingDecl
DEF_TRAVERSE_DECL(UsingEnumDecl
DEF_TRAVERSE_DECL(UsingPackDecl
DEF_TRAVERSE_DECL(UsingDirectiveDecl
DEF_TRAVERSE_DECL(UsingShadowDecl
DEF_TRAVERSE_DECL(ConstructorUsingShadowDecl
DEF_TRAVERSE_DECL(OMPThreadPrivateDecl
DEF_TRAVERSE_DECL(OMPRequiresDecl
DEF_TRAVERSE_DECL(OMPDeclareReductionDecl
DEF_TRAVERSE_DECL(OMPDeclareMapperDecl
DEF_TRAVERSE_DECL(OMPCapturedExprDecl
DEF_TRAVERSE_DECL(OMPAllocateDecl
DEF_TRAVERSE_DECL(TemplateTemplateParmDecl
DEF_TRAVERSE_DECL(BuiltinTemplateDecl
DEF_TRAVERSE_DECL(TemplateTypeParmDecl
DEF_TRAVERSE_DECL(TypedefDecl
DEF_TRAVERSE_DECL(TypeAliasDecl
DEF_TRAVERSE_DECL(TypeAliasTemplateDecl
DEF_TRAVERSE_DECL(ConceptDecl
DEF_TRAVERSE_DECL(UnresolvedUsingTypenameDecl
DEF_TRAVERSE_DECL(UnresolvedUsingIfExistsDecl
DEF_TRAVERSE_DECL(EnumDecl
DEF_TRAVERSE_DECL(RecordDecl
DEF_TRAVERSE_DECL(CXXRecordDecl
DEF_TRAVERSE_DECL(EnumConstantDecl
DEF_TRAVERSE_DECL(UnresolvedUsingValueDecl
DEF_TRAVERSE_DECL(IndirectFieldDecl
DEF_TRAVERSE_DECL(DecompositionDecl
DEF_TRAVERSE_DECL(BindingDecl
DEF_TRAVERSE_DECL(MSPropertyDecl
DEF_TRAVERSE_DECL(MSGuidDecl
DEF_TRAVERSE_DECL(UnnamedGlobalConstantDecl
DEF_TRAVERSE_DECL(TemplateParamObjectDecl
DEF_TRAVERSE_DECL(FieldDecl
DEF_TRAVERSE_DECL(ObjCAtDefsFieldDecl
DEF_TRAVERSE_DECL(ObjCIvarDecl
DEF_TRAVERSE_DECL(FunctionDecl
DEF_TRAVERSE_DECL(CXXDeductionGuideDecl
DEF_TRAVERSE_DECL(CXXMethodDecl
DEF_TRAVERSE_DECL(CXXConstructorDecl
DEF_TRAVERSE_DECL(CXXConversionDecl
DEF_TRAVERSE_DECL(CXXDestructorDecl
DEF_TRAVERSE_DECL(VarDecl
DEF_TRAVERSE_DECL(ImplicitParamDecl
DEF_TRAVERSE_DECL(NonTypeTemplateParmDecl
DEF_TRAVERSE_DECL(ParmVarDecl
DEF_TRAVERSE_DECL(RequiresExprBodyDecl
 */

public:
    clang::Rewriter mRewriter;

private:
    clang::ASTContext *Ctx;

    // Checks whether the name in Decl contains any `_`. Issues a warnning if it
    // does.
    void checkNoUnderscoreInName(clang::NamedDecl *Decl);
    // Checks whether the name in Decl starts with a lower case letter. Issues a
    // warning if not.
    void checkNameStartsWithLowerCase(clang::NamedDecl *Decl);
    // Checks whether the name in Decl starts with an upper case letter. Issues a
    // warning if not.
    void checkNameStartsWithUpperCase(clang::NamedDecl *Decl);
};
