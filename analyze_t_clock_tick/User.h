#ifndef User_H
#define User_H

#define TRUE  1
#define FALSE 0
#define USER_CAPACITY 2
// #define USER_CAPACITY 32

#define NAME_LIMIT 24
#define PASS_LIMIT 16

struct User{
    int id;
    char name[NAME_LIMIT];
    short sex;
    char pass[PASS_LIMIT];
};

short cpy_char_arr(char* src, int src_length, char* dst, int dst_length){
    int k;
    int limit=src_length<dst_length?src_length:dst_length;
    for(k=0; k < limit; k++){
        dst[k]=src[k];
    }
    return TRUE;
}

struct User user_tab[USER_CAPACITY];
int next_user_id=0;

short register_user(char* name, int name_len, char* pass, int pass_len, int* user_id){
    if(next_user_id>=USER_CAPACITY){
        return FALSE;
    }
    // struct User * user=&(user_tab[next_user_id]);
    struct User * user=user_tab+next_user_id;
    cpy_char_arr(name,name_len,user->name,NAME_LIMIT);
    cpy_char_arr(pass,pass_len,user->pass,PASS_LIMIT);
    
    (*user_id)=next_user_id;
    
    next_user_id++;

    return TRUE;
}




#endif