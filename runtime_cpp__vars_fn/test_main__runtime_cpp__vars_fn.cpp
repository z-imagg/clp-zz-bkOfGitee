#include "runtime_cpp__vars_fn.h"

class T_User{};
class T_Book{};
class T_Auth{};

void func1(){std::vector<_VarDecl> * _vdVec=_init_varLs_inFn("runtime_cpp__vars_fn/test_main__runtime_cpp__vars_fn.cpp","func1",5,14);
    int a, b, c, *d;
    T_User user1, user2, *ptr_user3; createVar(_vdVec,"T_User", 2);

    if(true){
        for(int i=0; i < 4; i++){
            T_Book book1, book2, *ptr_book3,book4; createVar(_vdVec,"T_Book", 3);
            {
                T_Auth auth1; createVar(_vdVec,"T_Auth", 1);
            }
        }
    }

destroyVarLs_inFn(_vdVec);}


int main(int argc, char** argv){
func1();
}