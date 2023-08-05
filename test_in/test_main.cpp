
bool  nonMicrosoftDemangle(  char *MangledName, char * &Result) {
  char *Demangled = nullptr;
  if (MangledName+1)
    Demangled = MangledName+1;
  else if (MangledName+2)
    Demangled = MangledName+3;

  if (!Demangled)//bug: 此if的then不会被插入花括号
    return false;

  Result = Demangled;
  return true;
}