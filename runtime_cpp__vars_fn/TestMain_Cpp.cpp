#include "runtime_cpp__vars_fn.h"

class T_User{};
class T_Book{};
class T_Auth{};

void func1(){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("runtime_cpp__vars_fn/test_main__runtime_cpp__vars_fn.cpp","func1",7,14);
    int a, b, c, *d;
    T_User user1, user2, *ptr_user3; createVar__RtCxx(_vdLs,"T_User", 2);

    if(true){
        for(int i=1; i <= 2; i++){
            T_Book book1, book2, *ptr_book3,book4; createVar__RtCxx(_vdLs,"T_Book", 3);
            {
                T_Auth auth1,auth2,*ptr_auth3; createVar__RtCxx(_vdLs,"T_Auth", 2);
            }
        }
    }

destroyVarLs_inFn__RtCxx(_vdLs);}


int main(int argc, char** argv){
func1();
}