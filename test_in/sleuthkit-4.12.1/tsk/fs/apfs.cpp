/*
 问题: 此构造函数 只给出了销毁调用 却没有给出初始化调用
 复现步骤:
 cd /fridaAnlzAp/sleuthkit-4.12.1/tsk/fs
 /fridaAnlzAp/clang-var/cmake-build-debug/bin/VarAlone apfs.cpp

 * Brian Carrier [carrier <at> sleuthkit [dot] org]
 * Copyright (c) 2019-2020 Brian Carrier.  All Rights reserved
 * Copyright (c) 2018-2019 BlackBag Technologies.  All Rights reserved
 *
 * This software is distributed under the Common Public License 1.0
 */
#include "../util/crypto.hpp"
#include "apfs_fs.hpp"
#include "tsk_apfs.hpp"

#include <cstring>

APFSBlock::APFSBlock(const APFSPool& pool, const apfs_block_num block_num)
    : _storage{}, _pool{pool}, _block_num{block_num} { 
return; /* voidFnEndInsertRet: */}
