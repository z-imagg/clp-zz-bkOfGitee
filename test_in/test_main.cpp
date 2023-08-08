//file: /pubx/llvm-project/llvm/utils/TableGen/AsmWriterInst.cpp





// 以下为简化后的 AsmWriterInst.cpp



#include "AsmWriterInst.h"
#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"

using namespace llvm;

static bool isIdentChar(char C) { return isAlnum(C) || C == '_'; }

std::string AsmWriterOperand::getCode(bool PassSubtarget) const {
  return  "";
}


AsmWriterInst::AsmWriterInst(const CodeGenInstruction &CGI, unsigned CGIIndex,
                             unsigned Variant)
    : CGI(&CGI), CGIIndex(CGIIndex) {

  std::string AsmString = CGI.FlattenAsmStringVariants(CGI.AsmString, Variant);
  std::string::size_type LastEmitted = 0;
  while (LastEmitted != AsmString.size()) {
    std::string::size_type DollarPos =
      AsmString.find_first_of("$\\", LastEmitted);

    // Emit a constant string fragment.
    if (DollarPos != LastEmitted) {
      for (; LastEmitted != DollarPos; ++LastEmitted)//此for下的switch不需要加花括号。此即 for下有一个无分号单体语句 即switch其结尾是没有分号的，因此找switch结尾的分号会找到siwtch后续无关语句分号，这很错且走远了。 解决办法是面对for时，若其下是switch 直接跳过该switch。 已经有此逻辑，即在 isAloneContainerStmt 是否 独立且容器 语句。 其实 独立且容器  就是指这里的 无分号单体语句。
        switch (AsmString[LastEmitted]) {
          case '\n':
            AddLiteralString("\\n");
            if(LastEmitted>9)//此if下的return应该加花括号
              return;
            break;
          default:
            AddLiteralString(std::string(1, AsmString[LastEmitted]));
            break;
        }
    } else if (AsmString[DollarPos] == '\\') {

    }   
  }

  Operands.emplace_back("return;", AsmWriterOperand::isLiteralStatementOperand);
}


unsigned AsmWriterInst::MatchesAllButOneOp(const AsmWriterInst &Other)const{

  return 0;
}
