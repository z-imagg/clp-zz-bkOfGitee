//来源: https://gitee.com/repok/sleuthkit-4.12.1/blob/master/tsk/fs/yaffs.cpp
#include <map>
#include <string>
#include <set>


static int
yaffs_validate_integer_field(std::string numStr){
    return 0;
}

static int
yaffs_validate_config_file(std::map<std::string, std::string> & paramMap){
    int offset_field_count;

    // Make a list of all fields to test
    std::set<std::string> integerParams;
    integerParams.insert("YAFFS_CONFIG_OBJ_ID_STR");

    // If the parameter is set, verify that the value is an int
    for(std::set<std::string>::iterator it = integerParams.begin();it != integerParams.end();it++){
        if((paramMap.find(*it) != paramMap.end()) && 
            (0 != yaffs_validate_integer_field(paramMap[*it]))){
            return 1;
        }
    }
    return 0;
}
