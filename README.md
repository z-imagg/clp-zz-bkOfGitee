# 处理的语句

## case
### 术语约定或备忘
1. case起止范围： 从冒号到下一个'case'开头,  简称有： case内 、case内容
2. Ast: Abstract syntax tree: 抽象语法树


### 没插入花括号的case
> 若case内, 以下任一条成立，则 跳过该case 即 不会对该case内容用花括号包裹.
> - 有#define、
> - 有#include、
> - 有直属变量声明、
> - 空case、
> - 有宏调用 


###  详述
#### 预处理回调收集#include指令、宏定义 
> CollectIncMacro_PPCb：Collect Inlucde Macro PPCallbacks :  收集Inlucde和Macro的预处理回调
>>  收集 #include、 #define ，  以判断case起止范围内 有无 #include、 #define

#### 遍历switch内某case起止范围内每条语句
>RangeHasMacroAstVst: Range Has Macro Call Ast Vistor: 给定范围有无宏调用Ast遍历器
>>  名义上遍历整个switch下的Stmt，实际遍历   给定范围内( 即 case起止范围 内) 的语句，进行以下计算:
    1. hasMacro： case起止范围 有无宏调用，
      从而帮助过滤掉 有宏调用 的case
    2. caseKSubStmtCnt：    case起止范围 语句个数（即 case子语句个数）, 
      从而帮助 过滤掉 空case
    3. VarDeclDirectlyInCaseKCnt:   直接写在'case'内的变量声明语句个数，
      即 直属变量声明个数 
      从而 帮助过滤掉 有直属变量声明 的case。
      直接写在'case'内的变量声明语句个数，包括以下两种情况：
      3.1. 直接写在'case'内，其父亲是case语句的
      3.2. 直接写在'case'内, 但是其父亲是switch块的.  
        即 存在 在case内的语句 但却不属于该case  而是直接属于switch， 此现象，直接导致 case的子语句 是伪命题，
          才使得 RangeHasMacroAstVst 不可能 实现无遗漏地 遍历 case下的子语句 ， 
            只能扩大遍历范围到整个switch 并只关注case起止范围内的语句 才能 实现无遗漏地、精准地 遍历 case下的子语句。


# 实际运行花括号插件
> 到此 加花括号插件完工了，在llvm-project上正常运行：

```bash
sudo docker exec -it ubuntu2204_clang15Compile bash
```
> 弹出docker实例ubuntu2204_clang15Compile的bash命令行，以下命令都在此命令行下执行

```bash
cd /pubx/

git clone https://gitcode.net/pubz/llvm-project/-/commits/brc-dev-no_tick
#即 https://gitcode.net/pubz/llvm-project/-/commit/bee38a325d0957a28b4d06cb4be3c251d143cdf0
#克隆仓库llvm-project后目录结构如下: /pubx/llvm-project/.git/config
```

- 步骤1: 对每个被直接编译的源文件中单语句加花括号
>  对llvm-project的每个源文件的编译过程应用插件libBrcPlugin.so 以 对 该源文件中单语句加花括号
```bash
source /pubx/llvm-project/doc_clang15_build/brc_build1_plugin.sh
```
> [brc_build1_plugin.sh](https://gitcode.net/pubz/llvm-project/-/blob/brc-dev-no_tick/doc_clang15_build/brc_build1_plugin.sh)

- 步骤2: 对加了花括号后的llvm-project再次做正常的普通编译
```bash
source /pubx/llvm-project/doc_clang15_build/brc_build2_directly.sh
```
> [brc_build2_directly.sh](https://gitcode.net/pubz/llvm-project/-/blob/brc-dev-no_tick/doc_clang15_build/brc_build2_directly.sh)

- 步骤3: 验证
```cpp
//编写c语言源文件 hello.c，内容如下：
#include <stdio.h>
int main(int argc, char** argv){
  int a,b;
  printf("a,b:");
  scanf("%d,%d",&a,&b);
  int sum=a+b, diff=a-b, div=a/b, mod=a%b;
  printf("sum=%d,diff=%d,div=%d,mod=%d\n",sum,diff,div,mod);
  return 0;
}
```

```bash
/pubx/build-llvm15/bin/clang-15  hello.c  -o hello.app
./hello.app
a,b:45,21
sum=66,diff=24,div=2,mod=3
```
> 加完花括号的llvm-project源码编译出的编译器clang-15  对 hello.c 实施编译， 编译出二进制文件 hello.app，
>
> 而该二进制文件 hello.app 正常运行
>
> 由此说明 ，花括号加的位置基本正确。


#统计
```bash
find /pubx/llvm-project/ -not -path '*/.git/*' -type f  \( -name "*.cpp" -or -name "*.c"  \)   | xargs -I% grep -Hn    BrcXxx    % > /pubx/BrcXxx.log

#把上一条bash命令抽成bash函数
findBrcCommentThenSave() {
  set -x #bash启用显示执行的命令
  keyword=$1
  find /pubx/llvm-project/ -not -path '*/.git/*' -type f \( -name "*.cpp" -or -name "*.c" \) | xargs -I% grep -Hn "$keyword" % |tee  /pubx/"${keyword}.log"
  set +x #bash禁止显示执行的命令
}
```

```bash
findBrcCommentThenSave BrcThen
findBrcCommentThenSave BrcSw
findBrcCommentThenSave BrcElse
findBrcCommentThenSave BrcFor
findBrcCommentThenSave BrcForRange
findBrcCommentThenSave BrcWhl
findBrcCommentThenSave BrcSw
```

> 各种语句分别加了多少花括号
```bash
ls -S /pubx/Brc* | xargs -I% sh -c  'wc -l %; ' 

'''
93201 /pubx/BrcThen.log
29832 /pubx/BrcSw.log
5539 /pubx/BrcElse.log
3603 /pubx/BrcFor.log
2187 /pubx/BrcForRange.log
663 /pubx/BrcWhl.log
'''
```

> 各种语句加了花括号的，有多少含有return
>> 这些单语句return，由于没有被花括号包裹，才没有被t_clock_tick插入栈变量释放语句。
>> 而tick插件栈变量分配、释放不平衡，具体为  栈变量共24万、最终残留2万没释放。  此不平衡是 由于 这些大约5万个单return语句没释放栈变量 导致的吗？
>> 如下所示，被BrcPlugin插入花括号的语句中 大约5万个含有return. 
```bash
ls -S /pubx/Brc* | xargs -I% sh -c  'echo -n "%    "; grep return % |wc -l '

'''
/pubx/BrcThen.log    50438
/pubx/BrcSw.log    2681
/pubx/BrcElse.log    815
/pubx/BrcFor.log    6
/pubx/BrcForRange.log    4
/pubx/BrcWhl.log    2
'''
```