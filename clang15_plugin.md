> clang 15插件，输入是c++源码。
>> clang插件并非LLVM插件。  
>>     LLVM插件的输入是中间表示 。
>>     c++源码被转成中间表示，源码中的各种可读符号此时难以获取。
>>     虽然理论上LLVM插件面对中间表示时，可以从dwarf等调试信息库中拿到符号，但不容易不直接。


#   clang15插件(c++源码修改插件)小结：
## 1.  共用Rewriter，导致后写入覆盖前写入，前写入丢失
>-  因此正确写法是，本插件全局 只能有一个Rewriter对象。
## 2. 单纯Rewriter对象，早期(Act中)不会崩溃，晚期(Visitor)会崩溃，应该用所谓智能指针const std::shared_ptr<Rewrite>
>- 单纯Rewriter对象```Rewriter rewriter;```作为Act成员字段， 早期(Act中CreateASTConsumer方法内)不会Segmentation fault，晚期(Visitor)会Segmentation fault。
>- 单纯对象 改为 ```const std::shared_ptr<Rewriter> mRewriter_ptr```后，正常，不再Segmentation fault。

## 3. 插件独立运行与插件被clang加载运行并不等同
> -  单纯Rewriter对象```Rewriter rewriter;```在插件独立运行中 并不会 Segmentation fault，而在插件被clang加载运行时 会 Segmentation fault。

##  由此可见 Rewriter 正确写法如下：
```cpp
/////Act中（早期）的Rewriter样子：（Act是Rewriter源头、是Rewriter的创建者。）
class XxxAstAct : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &CI,
                      llvm::StringRef inFile) override {
      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();
      mRewriter_ptr->setSourceMgr(SM, langOptions);//Rewriter对象设置
      return std::make_unique<XxxAstConsumer>(CI,mRewriter_ptr, &astContext, SM, langOptions);//Consumer进一步把Rewriter
    }

    PluginASTAction::ActionType getActionType() override {
      //本插件自动运行:  在MainAction后运行本插件
      return AddAfterMainAction;
    }

private:
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//Rewriter源头。 智能指针样式构造Rewriter对象
};

//晚期(Visitor)的Rewriter样子：(仅仅使用Act中的Rewriter）
class XxxVisitor
        : public RecursiveASTVisitor<XxxVisitor> {
public:
    enum LifeStep{
        Alloc,
        Free
    };
public:
    explicit CTkVst(const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, CompilerInstance &CI, SourceManager& SM)
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;




};


```

## 4. 在clang15插件视角 ，clang15 api并没有提供 直接修改AST树 的机会。
>- 因此只能应用Rewriter在期望位置插入文本，结尾处Rewriter.overwriteChangedFiles写磁盘。
>- 这就是说 修改后的源码，无法参与到本次编译过程中。只能再次编译，才会编译到修改后的源码
>-  网上搜索出来的 大部分是老版本api  ， 老版本clang的api是可以直接修改AST树的。

## 5. 欲 在 遍历c++源码的每个语法节点过程中执行自定义处理，最方便办法是  在 TraverseXxxStmt、TraverseXxxDecl中表达。
> - 被 override重写TraverseXxxStmt 应该具有的内容 : 自定义部分、递归链条粘结部分。
> - TraverseXxxDecl: 同上。
> - 不需要 override重写 TraverseStmt ，要被 override重写的是 TraverseXxxStmt。
> - TraverseDecl、TraverseXxxDecl: 同上。
> - Visit*系列，只能访问本节点自身，子节点递归部分都要自己实现，那样还不如自己做个clang……。因此Visit*系列不合适。

> - 覆盖TraverseXxxStmt中有错误写法  导致 死递归举例、以及正确的 覆盖TraverseXxxStmt样子：
```cpp
/**  推测的结论：
在clang源码内部 ：
   在Stmt递归过程中， TraverseStmt 位于 顶层， 各个 TraverseXxxStmt 位于 下层， 即   TraverseStmt 调用 各个 TraverseXxxStmt
在clang源码内部  Stmt递归过程 伪码如下：*/
TraverseStmt(第k层节点x){
	for (Xxx in [If, While, For, Try, ... ] ){
		TraverseXxxStmt(x);
	}
//因此 TraverseXxxStmt(x) 中 不能含有 TraverseStmt(x) , 否则 形成  环 且 是无条件环  即死递归
//  clang并没有预期允许能发生这种事情 即没有代码能检测这种事情   即 是 无条件
}

// 也可以看出 自定义 TraverseXxxStmt(x) 应该具有如下形式：
//写法1. 正确的 自定义 TraverseXxxStmt(x) 写法 ：
TraverseXxxStmt(x) {
//  对当前节点x做想要的自定义处理， 比如 插入 某种语句

//  将递归链条正确的接好:
  for(auto child: x){
    TraverseStmt(child); //进入下一层节点的递归. 注意 此时 调用栈上层有 TraverseStmt(x),  本行有 TraverseStmt(child) ， 但 x 和 child 是 不相等的 ，所以不存在环 即不会死递归。
    // 注：  x 是 语法树 中的 第k层节点  ，而 child是第k+1层节点，因此 x 和 child 不相等。
  }
}

//写法2. 错误的自定义写法： 引起死递归
TraverseXxxStmt(x){
    //对当前节点x做想要的自定义处理

    TraverseStmt(x);// 这里想图省事，直接用本层节点. 注意 此时 调用栈上层有 TraverseStmt(x),  本行又有  TraverseStmt(x) ，二者 函数+参数 完全相同  ，所以存在环 且 没有条件斩断环 即 产生死递归。
}

// TraverseDecl 同 TraverseStmt
//TraverseXxxDecl 同 TraverseXxxStmt

// 由此可见 clang15源码内部，对待 TraverseStmt 是更高级别的对待， 而TraverseXxxStmt是更次要的对待。 
// TraverseDecl、TraverseXxxDecl同理。

// 同时可见，不要轻易 override重写  TraverseStmt和TraverseDecl，因为这两者相当于是中心枢纽。实际上也根本不需要重写这两者。

```


## 6.  clang15插件独立运行main函数大致样子
>-  网上找不到clang15插件独立运行代码样子，找到的都是clang老版本api的样子，在clang15下各种编译错误。
```cpp


int main(int Argc, const char **Argv) {
  Expected<tooling::CommonOptionsParser> eOptParser =
          tooling::CommonOptionsParser::create(Argc, Argv, CTkAloneCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }

  // 获取编译配置
  clang::tooling::CompilationDatabase &Compilations = eOptParser->getCompilations();


  // 创建 CompilerInstance 对象
  clang::CompilerInstance CI;
  // 创建源管理器
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
/* 确保在创建编译器实例（`clang::CompilerInstance`）后，立即创建源管理器（`clang::SourceManager`）并将其设置到编译器实例中
否则 ，运行时 断言失败 报错 ：   CompilerInstance.h:423: clang::SourceManager& clang::CompilerInstance::getSourceManager() const: Assertion `SourceMgr && "Compiler instance has no source manager!"' failed.
 */

  // 设置语言选项
  CI.getLangOpts().CPlusPlus = true;


  // 设置文件名
  const char* FileName = "/xxx.cpp";
  if(Argc>=2 && Argv[1]){
    //如果命令行 有指定源文件路径 则用命令行的
    FileName=Argv[1];
  }
  clang::FileID MainFileID = CI.getSourceManager().getOrCreateFileID(
          CI.getFileManager().getVirtualFile(FileName, /*Size=*/0, /*ModificationTime=*/0),
          clang::SrcMgr::C_User);

  // 将文件 ID 设置为主文件
  CI.getSourceManager().setMainFileID(MainFileID);

  // 创建 ClangTool 对象
  std::vector<std::string> Files{FileName};
  clang::tooling::ClangTool Tool(Compilations, Files);//报错： No member named 'getCompilations' in 'clang::CompilerInstance'


  CI.getDiagnosticOpts().ShowColors = true;

  Tool.appendArgumentsAdjuster( clang::tooling::getInsertArgumentAdjuster("--verbose"));
  //解决： 找不到 stddef.h
  Tool.appendArgumentsAdjuster( clang::tooling::getInsertArgumentAdjuster({"-resource-dir","/pubx/build-llvm15/lib/clang/15.0.0"},tooling::ArgumentInsertPosition::END));
  /* stddef.h位于:
/pubx/build-llvm15/lib/clang/15.0.0/include/stddef.h
   */

  // 运行 ClangTool
  int Result = Tool.run(clang::tooling::newFrontendActionFactory<CTkAloneAct>().get());

  return Result;
}

```


## 7.  独立运行下 HandleTranslationUnit 会被多次调用，被clang加载运行下HandleTranslationUnit只会被调用一次。
>- 不要想着 用  FrontendOptions.ProgramAction、ASTContext.TUKind 等表示不同编译阶段的标志来判断区分
>- 就是在同一阶段下HandleTranslationUnit被调用了多次
>- 想要HandleTranslationUnit只被调用一次，没有更好的办法，只能自己加标记字段。
>- 而且 每次HandleTranslationUnit被调用，Visitor对象居然还是不同的。因此加的标记字段只能是类级别static成员字段，虽然这样很丑。


## 8.  粗略感觉 clang11~clang15 api差异不是特别大

