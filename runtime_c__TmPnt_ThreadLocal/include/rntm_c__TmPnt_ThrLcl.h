
#ifndef rntm_c__TmPnt_ThrLcl_H
#define rntm_c__TmPnt_ThrLcl_H

// 始终保持 为 C函数，而非C++函数
#ifdef __cplusplus
extern "C" {
#endif

void TL_TmPnt__update(int _TmPnt_new);

int TL_TmPnt__get() ;

#ifdef __cplusplus
};
#endif




#endif //rntm_c__TmPnt_ThrLcl_H
