
## libclangPlgVar_runtime_c_TmPnt_ThrLcl.a

### libclangPlgVar_runtime_c_TmPnt_ThrLcl.a 是 frida_js 和  'runtime_cpp__vars_fn|runtime_c__vars_fn' 之间的桥梁

- frida_js 写 ThreadLocal_TmPnt 到  libclangPlgVar_runtime_c_TmPnt_ThrLcl.a  

- 'runtime_cpp__vars_fn|runtime_c__vars_fn' 从 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a   读 ThreadLocal_TmPnt


### 术语

TmPnt == TimePoint == 时间点

参考 http://giteaz:3000/frida_analyze_app_src/frida_js/src/tag/tag_release__qemu_v8.2.2__linux_v5.11/InterceptFnSym.ts
