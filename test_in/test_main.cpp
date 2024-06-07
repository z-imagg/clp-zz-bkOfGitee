#pragma message("VFIR_inserted")
//来源 ： /fridaAnlzAp/sleuthkit-4.12.1/tsk/fs/apfs.cpp

#include <stdexcept>

class APFSPool{};
class apfs_block_num{};
class APFSObject{
public:
    APFSObject(const APFSPool& pool,
                    const apfs_block_num block_num);

};
class APFSSpacemanCIB :APFSObject{
public:
    APFSSpacemanCIB(const APFSPool& pool,
                                     const apfs_block_num block_num);
};

int obj_type(){
    return 0;
}
APFSSpacemanCIB::APFSSpacemanCIB(const APFSPool& pool,
                                 const apfs_block_num block_num)
        : APFSObject(pool, block_num) {
    if (obj_type() != 0) {
        throw std::runtime_error("APFSSpacemanCIB: invalid object type");
    }
//这里不该插入 销毁变量调用 destroyVarLs_inFn__RtC**
return; /* voidFnEndInsertRet: */}
int main(int argc, char** argv){
    return 0;
}