
#ifndef rntm_c__TmPnt_ThrLcl_H
#define rntm_c__TmPnt_ThrLcl_H

// 始终保持 为 C函数，而非C++函数
#ifdef __cplusplus
extern "C" {
#endif

void TL_TmPnt__update(int _TmPnt_new);

int TL_TmPnt__get() ;

//打印ThreadLocal中某变量指针
// ThreadLocal中某变量指针 不应该被获得 ，
// ThreadLocal 即  线程A只应该操纵线程A的A_TL_x1变量 线程B只应该操纵线程B的B_TL_x1  这是两个不同的变量
//  【术语】 TL == ThreadLocal
//   ThreadLocal的逻辑意图是  而线程A没有权限操纵B_TL_x1 ，从而保证了 无法错乱(即线程安全)
//       但若提供 B_TL_x1 的指针 ，则 当A线程获得此指针后 ， 此逻辑意图则被打破(即ThreadLocal失效）
//   因此 不应该提供 ThreadLocal中变量的指针， 故而 只打印 该指针 而不返回 该指针
void TL_TmPnt__printPtr( );
#ifdef __cplusplus
};
#endif




#endif //rntm_c__TmPnt_ThrLcl_H
