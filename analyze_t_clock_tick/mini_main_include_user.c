
#include "User.h"

// #include <stdio.h>
int main(int argc, char** argv){
    int user_id_tomcat=-1,user_id_kate=-1,user_id_john=-1;
    short reg_result_tomcat= register_user("tomcat", 6, "123",4,  &user_id_tomcat);
    short reg_result_kate= register_user("Kate", 5, "secret",7,  &user_id_kate);
    short reg_result_john= register_user("john", 5, "0000",5,  &user_id_john);
    
    // printf("user_id_tomcat=%d\n",user_id_tomcat);
    // printf("user_id_kate=%d\n",user_id_kate);
    // printf("user_id_john=%d\n",user_id_john);
    // printf("next_user_id=%d\n",next_user_id);
    return 0;
}

