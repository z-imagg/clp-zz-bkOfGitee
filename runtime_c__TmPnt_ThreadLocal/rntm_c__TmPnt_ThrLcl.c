
#include "rntm_c__TmPnt_ThrLcl.h"

// __thread是gcc扩展 ,  
//    __thread == ThreadLocal
__thread int TL_TmPnt = -1;

void TL_TmPnt__update(int _TmPnt_new) {
    TL_TmPnt = _TmPnt_new;
}

int TL_TmPnt__get() {
    return TL_TmPnt;
}
