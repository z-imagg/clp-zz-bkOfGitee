
#ifndef CollectIncMacro_PPCb_H
#define CollectIncMacro_PPCb_H
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "base/LocId.h"


#include <unordered_set>

using namespace clang;

class CollectIncMacro_PPCb : public clang::PPCallbacks {
public:
    CompilerInstance &CI;

    static std::unordered_set<LocId,LocId> InclusionDirectiveLocSet;
    static std::unordered_set<LocId,LocId> MacroDefinedLocSet;

    explicit CollectIncMacro_PPCb(CompilerInstance &_CI) : CI(_CI) {

    }

    //预处理回调收集#includee 以判断case起止范围内 有无#i
    void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName, bool IsAngled,
                            CharSourceRange FilenameRange, Optional<FileEntryRef> File, StringRef SearchPath,
                            StringRef RelativePath, const Module *Imported,
                            SrcMgr::CharacteristicKind FileType) override;

    //预处理回调收集#define 以判断case起止范围内 有无#d
    virtual void MacroDefined(const clang::Token& MacroNameTok,
                              const clang::MacroDirective* MD) override;

    static bool hasInclusionDirective(SourceManager& SM, SourceRange range);
    static bool hasMacroDefined(SourceManager& SM, SourceRange range);
};


#endif //CollectIncMacro_PPCb_H
