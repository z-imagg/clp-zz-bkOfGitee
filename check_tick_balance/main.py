
import pathlib
from pathlib import Path
import re





import glob
import regex as re
#  `regex`库：`regex`库是一个功能更强大且兼容性更好的正则表达式库，它提供了对PCRE（Perl兼容正则表达式）的支持。与标准库中的`re`模块相比，`regex`库提供了更多的功能和选项，例如更复杂的模式匹配、更多的元字符和修饰符等。您可以使用`pip install regex`命令安装该库。
from pathlib import Path

def process_files(directory):
    # 递归遍历目录下的所有.c和.cpp文件
    file_pattern = f"{directory}/**/*.c"  # 匹配.c文件
    files = glob.glob(file_pattern, recursive=True)

    file_pattern = f"{directory}/**/*.cpp"  # 匹配.cpp文件
    files += glob.glob(file_pattern, recursive=True)

    # 遍历每个源文件
    for file_path in files:
        print(file_path)
        with open(file_path, 'r') as f:
            lines = f.readlines()

        # 遍历每行
        for line in lines:
            # 匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);
            re__=r'X__t_clock_tick\(/[*]栈生[*]/(\d+), /[*]栈死[*]/(\d+), /[*]堆生[*]/(\d+), /[*]堆死[*]/(\d+)\);'
            match = re.search(re__, line)
            if match:
                # 提取数A、数B、数C、数D
                栈生 = match.group(1)
                栈死 = match.group(2)
                堆生 = match.group(3)
                堆死 = match.group(4)

                print(f"数A: {栈生}, 数B: {栈死}, 数C: {堆生}, 数D: {堆死}")

# 使用示例
# directory = '/path/to/directory'  # 替换为您要递归遍历的目录路径
# process_files(directory)
# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    process_files("/pubx/llvm-project/llvm/")
# See PyCharm help at https://www.jetbrains.com/help/pycharm/

# python编程：请编写通用函数（不准使用for，只能使用map()、list()、列表解析等高级特性； 不准使用os.walk， 尽可能glob、pathlib；不准使用自带re，尽可能使用三方库re2 ），递归遍历给定目录下所有 .c、.cpp 源文件， 读取每个源文件，遍历该源文件中每行，  匹配行中的 X__t_clock_tick(/*栈生*/数A, /*栈死*/数B, /*堆生*/数C, /*堆死*/数D);   ， 若有匹配  打印 数A 、数B、 数C、 数D
