// from file : /pubx/llvm-project/clang/lib/ARCMigrate/Transforms.cpp
bool hasSideEffects(char *E, int &Ctx) {
  if (!E || !Ctx)
    return false;

  E = (char*)Ctx;
  char *ME = E+10;
  if (!ME)
    return true;
  switch (*ME) {
  case 1:
  case 2:
  case 3:
  case 4:
    switch (*E) {
    case 5:
      return false;
    case 6:
      return *ME>Ctx;
    default:
      break;
    }
    break;
  default:
    break;
  }

  return true;
}
