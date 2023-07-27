
import pathlib
from pathlib import Path
import re
from typing import List


class File:
    def __init__(self,file_path:str):
        self.file_path:str=file_path
        self.栈生:int=0
        self.栈死:int=0
        self.堆生:int=0
        self.堆死:int=0
    def 栈分配(self, 变量个数):
        self.栈生+=变量个数
    def 栈释放(self, 变量个数):
        self.栈死+=变量个数
    def 堆分配(self, 变量个数):
        self.堆生+=变量个数
    def 堆释放(self, 变量个数):
        self.堆死+=变量个数
    def 平衡性(self)->bool:
        栈平衡:bool = self.栈生==self.栈死
        堆平衡:bool = self.堆生==self.堆死
        return 栈平衡 and 堆平衡

    def __str__(self):
        return f"{self.file_path},栈:{self.栈生},{self.栈死};堆:{self.堆生},{self.堆死};"
import glob
import regex as re
#  `regex`库：`regex`库是一个功能更强大且兼容性更好的正则表达式库，它提供了对PCRE（Perl兼容正则表达式）的支持。与标准库中的`re`模块相比，`regex`库提供了更多的功能和选项，例如更复杂的模式匹配、更多的元字符和修饰符等。您可以使用`pip install regex`命令安装该库。
from pathlib import Path

pattern = r'X__t_clock_tick\(/[*]栈生[*]/(\d+), /[*]栈死[*]/(\d+), /[*]堆生[*]/(\d+), /[*]堆死[*]/(\d+)\);'
def lineLsProcess(lines:List[str],fK:File):
    # 遍历每行
    for line in lines:
        # 匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);
        match = re.search(pattern, line)
        if match:
            # 提取数A、数B、数C、数D
            fK.栈分配(int(match.group(1)))
            fK.栈释放(int(match.group(2)))
            fK.堆分配(int(match.group(3)))
            fK.堆释放(int(match.group(4)))
            # print(f"数A: {栈生}, 数B: {栈死}, 数C: {堆生}, 数D: {堆死}")
    return

def process_files(directory):
    # 递归遍历目录下的所有.c和.cpp文件
    file_pattern = f"{directory}/**/*.c"  # 匹配.c文件
    files = glob.glob(file_pattern, recursive=True)

    file_pattern = f"{directory}/**/*.cpp"  # 匹配.cpp文件
    files += glob.glob(file_pattern, recursive=True)

    # 遍历每个源文件
    for file_path in files:
        fK:File=File(file_path)
        with open(file_path, 'r') as f:
            lines = f.readlines()
        lineLsProcess(lines,fK)
        if not fK.平衡性():
            print(f"失衡:{fK}")
        else:
            print(f"平衡:{fK}")
            pass
###############



# 使用示例
# directory = '/path/to/directory'  # 替换为您要递归遍历的目录路径
# process_files(directory)
# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    process_files("/pubx/llvm-project/llvm/")
# See PyCharm help at https://www.jetbrains.com/help/pycharm/

# python编程：请编写通用函数（不准使用for，只能使用map()、list()、列表解析等高级特性； 不准使用os.walk， 尽可能glob、pathlib；不准使用自带re，尽可能使用三方库re2 ），递归遍历给定目录下所有 .c、.cpp 源文件， 读取每个源文件，遍历该源文件中每行，  匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);   ， 若有匹配  打印 数A 、数B、 数C、 数D
