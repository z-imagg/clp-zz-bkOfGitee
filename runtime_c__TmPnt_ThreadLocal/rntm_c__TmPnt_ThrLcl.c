
#include "rntm_c__TmPnt_ThrLcl.h"

// 始终保持 为 C函数，而非C++函数
#ifdef __cplusplus
extern "C" {
#endif


// __thread是gcc扩展 ,  
//    __thread == ThreadLocal
//  术语  TL_ == ThreadLocal_
__thread int TL_TmPnt = -1;

void TL_TmPnt__update(int _TmPnt_new) {
    TL_TmPnt = _TmPnt_new;
}

int TL_TmPnt__get() {
    return TL_TmPnt;
}


#ifdef __cplusplus
};
#endif