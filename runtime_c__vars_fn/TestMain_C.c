#include "runtime_c__vars_fn.h"

struct T_User{};
struct T_Book{};
struct T_Auth{};
#define TRUE 1

void func1(){_VarDeclLs * _vdLs= _init_varLs_inFn__RtC00("runtime_c__vars_fn/test_main__runtime_c__vars_fn.c", "func1",
                                                         7, 14);
    int a, b, c, *d;
    struct T_User user1, user2, *ptr_user3;
  createVar__RtC00(_vdLs, "T_User", 2);

    if(TRUE){
        for(int i=1; i <= 2; i++){
            struct T_Book book1, book2, *ptr_book3,book4;
          createVar__RtC00(_vdLs, "T_Book", 3);
            {
                struct T_Auth auth1,auth2,*ptr_auth3;
              createVar__RtC00(_vdLs, "T_Auth", 2);
            }
        }
    }

  destroyVarLs_inFn__RtC00(_vdLs);}


int main(int argc, char** argv){
func1();
}