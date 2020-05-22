/*
The MIT License (MIT)

Copyright (c) 2013-2020 xiaohui(2535418266@qq.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <stdio.h>
#include <sys/time.h>
#define MILLION 1000000L
typedef void (*CALLBACK_FUNC_T)(unsigned char *,unsigned char,unsigned char *,unsigned char);
#define MAX_CODE_SUM 4294967295
typedef struct{
	unsigned char *code_str;
	unsigned long int code_sum;//这里要注意是否会溢出
    CALLBACK_FUNC_T callback_func;
}instr_t;

unsigned long int global_data_cnt = 0;
unsigned long int max_instr_len = 0;
void get_v_callback_func(unsigned char *dev_addr_p,unsigned char dev_addr_len,unsigned char *dat_p,unsigned char dat_len);
void get_cur_callback_func(unsigned char *dev_addr_p,unsigned char dev_addr_len,unsigned char *dat_p,unsigned char dat_len);
instr_t instr_table[]={
		{.code_str = "get_v",.code_sum = 0,.callback_func = get_v_callback_func},
		{.code_str = "get_cur",.code_sum = 0,.callback_func = get_cur_callback_func}
	};
void get_v_callback_func(unsigned char *dev_addr_p,unsigned char dev_addr_len,unsigned char *dat_p,unsigned char dat_len){
    // printf("%d get_v_callback_func dev_addr_len: %d dev_addr:",global_data_cnt,dev_addr_len);print_char_table(dev_addr_p,dev_addr_len);
    // printf(" dat_len: %d data:",dat_len);print_char_table(dat_p,dat_len);
    // printf("\r\n");
}
void get_cur_callback_func(unsigned char *dev_addr_p,unsigned char dev_addr_len,unsigned char *dat_p,unsigned char dat_len){
    // printf("%d get_cur_callback_func dev_addr_len: %d dev_addr:",global_data_cnt,dev_addr_len);print_char_table(dev_addr_p,dev_addr_len);
    // printf(" dat_len: %d data:",dat_len);print_char_table(dat_p,dat_len);
    // printf("\r\n");
}
int init_instr_table(void){
	unsigned char i;
	unsigned char *tmp_str_p;
    unsigned long int tmp_instr_len;
	for(i=0;i<(sizeof(instr_table)/sizeof(instr_t));i++){
		tmp_str_p = (instr_t *)(instr_table+i)->code_str;
		instr_table[i].code_sum = 0;//防止编译器没有初始化此空间
        tmp_instr_len = 0;
		while(*tmp_str_p != '\0'){
            if(((unsigned long long int)(instr_table[i].code_sum) + (unsigned long long int)(*tmp_str_p)) > MAX_CODE_SUM){
                return -1;//溢出报错
            }
            tmp_instr_len ++;
			instr_table[i].code_sum += *tmp_str_p;
			tmp_str_p++;
		}
        if(tmp_instr_len > max_instr_len){
            max_instr_len = tmp_instr_len;//因为字符串长度必须大于0，所以才这么写
        }
	}
    return 0;
}
#define DEV_ADDR_FIX_LEN 3
#define DAT_MAX_LEN      100
typedef enum {
    wait_for_dev_addr = 0,
    wait_for_underline = 1,
    wait_for_equl = 2,
    wait_for_CR = 3,
    wait_for_LF = 4

}rev_state;
rev_state rev_global_state = wait_for_dev_addr;
void print_char_table(unsigned char *start,unsigned char len){
    unsigned char *start_p = start;
    while(len--){
        printf("%c",*start_p++);
    }
}
unsigned char dat_buf[];
int deal_func();
int main(){
    long timedif;
    struct timeval tpend;            //timeval结构体包含两个成员:time_t tv_sec;从Epoch开始的秒数  time_t tv_usec;从Epoch开始的微秒数
    struct timeval tpstart;
    
    if(gettimeofday(&tpstart, NULL)){
        printf(stderr, "Failed to get start time\n");
        return 1;
    }
    
    deal_func();
    
    if(gettimeofday(&tpend, NULL)){
        printf(stderr, "Failed to get end time\n");
        return 1;
    }
    printf("start deal test_data_len:%fKB\n",strlen(dat_buf)/1024.000);
    timedif = MILLION*(tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
    printf("The function_to_time took %ld microseconds data_cnt:%d\n",timedif,global_data_cnt);

}

int parse_data(unsigned char *dev_addr_p, unsigned char dev_addr_len,unsigned char *instr_p,unsigned char instr_len,unsigned char *dat_p,unsigned char dat_len,unsigned long int tmp_code_sum){
    global_data_cnt ++;
    // printf("%d dev_addr_len: %d dev_addr:",global_data_cnt,dev_addr_len);print_char_table(dev_addr_p,dev_addr_len);
    // printf(" instr_len: %d instr:",instr_len);print_char_table(instr_p,instr_len);
    // printf(" dat_len: %d data:",dat_len);print_char_table(dat_p,dat_len);
    // printf(" tmp_code_sum: %d",tmp_code_sum);
    // printf("\r\n");
    unsigned char i;
	unsigned char *tmp_str_p;
    unsigned char *tmp_instr_p;
    unsigned char tmp_instr_len;
	for(i=0;i<(sizeof(instr_table)/sizeof(instr_t));i++){
        // printf("i %d code_sum %ld tmp_code_sum: %d\r\n",i,instr_table[i].code_sum,tmp_code_sum);
        if(instr_table[i].code_sum != tmp_code_sum){
            continue;
        }
		tmp_str_p = (instr_t *)(instr_table+i)->code_str;
        tmp_instr_p = instr_p;
        tmp_instr_len = instr_len;
		while(*tmp_str_p != '\0' && tmp_instr_len-- && *tmp_str_p == *tmp_instr_p++ ){
			tmp_str_p++;
		}
        if(tmp_instr_len == 0){
            //指令存在
            // printf("instr exist %s\r\n",(instr_t *)(instr_table+i)->code_str);
            // void (*callback_func)(unsigned char *dev_addr_p,unsigned char dev_addr_len,unsigned char *dat_p,unsigned char dat_len)
            instr_table[i].callback_func(dev_addr_p, dev_addr_len,dat_p, dat_len);
        }
	}
    // printf("\r\n");
}
int deal_func()
{
    
    if(init_instr_table()==0){
        // printf("init_instr_table ok\r\n");
    }else{
        printf("init_instr_table failed\r\n");
        return -1;
    }



   //模拟处理ringbuf的过程(每次只处理一个字节)
    unsigned char *dat_buf_p = dat_buf;
	unsigned char *deal_pos_p = dat_buf;
    unsigned long int temp_cnter = 0;
    unsigned char *dev_addr_p;
    unsigned char dev_addr_len;
    unsigned char *instr_p;
    unsigned char instr_len;
    unsigned char *dat_p;
    unsigned char dat_len;
    unsigned long int tmp_code_sum;
	while(*deal_pos_p != '\0'){
        // printf("deal_pos_p:%c 0x%x rev_global_state:%d\r\n",*deal_pos_p,*deal_pos_p,rev_global_state);
	    switch(rev_global_state){
            case wait_for_dev_addr:
                // printf("wait_for_dev_addr deal_pos_p:%c temp_cnter %d\r\n",*deal_pos_p,temp_cnter);
                if(*deal_pos_p >= 48 && *deal_pos_p <= 57){//这里根据要求改，如果当前设备只有一个地址的话，可以直接判断是否为地址位
                    if(temp_cnter == 0){
                       dev_addr_p = deal_pos_p;//发现有可能是地址的开始，临时缓存 
                    }
                    if((temp_cnter+1) == DEV_ADDR_FIX_LEN){
                        // printf("wait_for_dev_addr end\r\n");
                        dev_addr_len = temp_cnter + 1;
                        rev_global_state = wait_for_underline;
                        temp_cnter = 0;
                    }else if((temp_cnter+1) > DEV_ADDR_FIX_LEN){
                        //数据非法，需要忽略第一个字节并重新接收解析
                        dat_buf_p ++;
                        deal_pos_p = dat_buf_p;
                        //init deal env var
                        temp_cnter = 0;
                        continue;
                    }
                    temp_cnter ++;
                }else{
                    //数据非法，需要忽略第一个字节并重新接收解析
                    dat_buf_p ++;
                    deal_pos_p = dat_buf_p;
                    //init deal env var
                    temp_cnter = 0;//方便下一次使用的时候判断是否首次进入
                    continue;
                }
            break;
            case wait_for_underline:
                // printf("wait_for_underline deal_pos_p:%c\r\n",*deal_pos_p);
                if(*deal_pos_p == '_'){
                    rev_global_state = wait_for_equl;
                    tmp_code_sum = 0;
                    // printf("wait_for_underline to wait_for_equl rev_global_state %d\r\n",rev_global_state);
                    temp_cnter = 0;
                }else{
                    if(*deal_pos_p >= 48 && *deal_pos_p <= 57){
                        //数据非法，需要忽略第一个字节并重新接收解析
                        rev_global_state = wait_for_dev_addr;
                        dat_buf_p ++;
                        deal_pos_p = dat_buf_p;
                        temp_cnter = 0;
                        continue;
                    }else{
                        //数据非法并且发现前面的数据完全不可用，需要忽略前面接收的数据并重新接收解析
                        rev_global_state = wait_for_dev_addr;
                        dat_buf_p = deal_pos_p + 1;
                        deal_pos_p = dat_buf_p;
                        temp_cnter = 0;
                        continue;
                    }
                    //init deal env var
                    temp_cnter = 0;//方便下一次使用的时候判断是否首次进入
                }
            break;
            case wait_for_equl:
                // printf("wait_for_equl deal_pos_p:%c temp_cnter:%d\r\n",*deal_pos_p,temp_cnter);
                if(temp_cnter == 0){
                    instr_p = deal_pos_p;
                }
                if(*deal_pos_p == '='){
                    rev_global_state = wait_for_CR;
                    instr_len = temp_cnter + 1 - 1;
                    temp_cnter = 0;
                }else{
                    tmp_code_sum += *deal_pos_p;
                    if((temp_cnter+1) > max_instr_len){
                        //数据非法，需要从_后开始接收并重新接收解析
                        rev_global_state = wait_for_dev_addr;
                        dat_buf_p = instr_p;//从下划线后
                        deal_pos_p = dat_buf_p;
                        temp_cnter = 0;
                        continue;
                    }
                }
                temp_cnter ++;
            break;
            case wait_for_CR:
                // printf("wait_for_CR deal_pos_p:%c temp_cnter:%d\r\n",*deal_pos_p,temp_cnter);
                if(temp_cnter == 1){
                    // printf("wait_for_CR dat_p:%c\r\n",*dat_p);
                    dat_p = deal_pos_p;
                }
                if(*deal_pos_p == '\r'){
                    rev_global_state = wait_for_LF;
                    // printf("wait_for_CR to wait_for_LF rev_global_state:%d\r\n",rev_global_state);
                }
                if((temp_cnter+2) > DAT_MAX_LEN){
                    //数据长度非法，需要从=后开始接收并重新接收解析
                    rev_global_state = wait_for_dev_addr;
                    dat_buf_p = dat_p;//从=后
                    deal_pos_p = dat_buf_p;
                    temp_cnter = 0;
                    continue;
                }
                temp_cnter ++;
            break;
            case wait_for_LF:
                // printf("wait_for_LF deal_pos_p:%c %x\r\n",*deal_pos_p,*deal_pos_p);
                if(*deal_pos_p == '\n'){
                    //接收完成,初始化环境准备下一包
                    rev_global_state = wait_for_dev_addr;
                    dat_buf_p = deal_pos_p + 1;
                    dat_len = temp_cnter - 2;
                    deal_pos_p = dat_buf_p;
                    temp_cnter = 0;
                    parse_data(dev_addr_p,dev_addr_len,instr_p,instr_len,dat_p,dat_len,tmp_code_sum);
                    tmp_code_sum = 0;
                    continue;
                }else{
                    rev_global_state = wait_for_CR;
                }
                if((temp_cnter+2-1) > DAT_MAX_LEN){
                    //数据长度非法，需要从=后开始接收并重新接收解析
                    rev_global_state = wait_for_dev_addr;
                    dat_buf_p = dat_p;//从=后
                    deal_pos_p = dat_buf_p;
                    temp_cnter = 0;
                    continue;
                }
                temp_cnter ++;
            break;
        }
		deal_pos_p++;
	}
   
   return 0;
}
unsigned char dat_buf[]={ \
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_v=test\r\n12345我是干扰你信不信？001_get_v=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_v=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_cur=test\r\n12345我是干扰你信不信？001_get_cur=test_data\r\n001_get_cur=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_cur=?\r\n_g再来干扰一下et_v=我就是干扰你，哈哈哈\r\n"
    "001_get_a=test\r\n12345我是干扰你信不信？001_get_a=test_data\r\n001_get_v=?\r\n0\r\n没错干扰还是我01_get_v=?001_g再来干扰一下et_v=?\r\n001001_get_a=?\r\n_g再来干扰一下et_a=我就是干扰你，哈哈哈\r\n"
    
};