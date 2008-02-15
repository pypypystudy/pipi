#include <cstdlib>
#include <iostream>
#include "Base64.h"
using namespace std;

//define structure
#define RC_SUCCESS 0
#define RC_ERROR 1

//class implement

Base64::Base64()
{
    char beta[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
                     'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
                     'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
                     'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', 
                     '4', '5', '6', '7', '8', '9', '+', '/', 
                     '='};
    memcpy(alpha, beta, 65);
};

Base64::~Base64()
{};

int Base64::encode(char *src, char *dest)
{
    //check dest_len
    int src_len = strlen(src);
    int pad_num = src_len % 3;
    if (0 != pad_num)
    {
        pad_num = (pad_num==1?2:1);
    }

    //decode regular
    int loop_num = src_len / 3;
    int src_cur = 0;
    int dest_cur = 0;
    for (int i=0; i<loop_num; i++)
    {
        if(RC_SUCCESS != encode_process(src+src_cur, dest+dest_cur, 0))
        {
            cout<<"error occur!"<<endl;
            return RC_ERROR;
        }
        src_cur += 3;
        dest_cur += 4;
    }
    
    //decode tail
    if (0 == pad_num)
    {
        return RC_SUCCESS;
    }
    
    if(RC_SUCCESS != encode_process(src+src_cur, dest+dest_cur, pad_num))
    {
        cout<<"error occur!"<<endl;
        return RC_ERROR;
    }
    
    return RC_SUCCESS;
};

int Base64::decode(char *src, char *dest)
{
    //check source length
    int src_len = strlen(src);
    if (0 != src_len % 4)
    {
        cout<<"error srouce length!"<<endl;
        return RC_ERROR;
    }

    int src_cur = 0;
    int dest_cur = 0;
    int loop_num = src_len / 4;
    for (int i=0; i<loop_num; i++)
    {
        if (RC_SUCCESS != decode_process(src+src_cur, dest+dest_cur))
        {
            cout<<"error occur!"<<endl;
            return RC_ERROR;
        }
        src_cur += 4;
        dest_cur += 3;
    }
    
    return RC_SUCCESS;
};

int Base64::encode_process(char *src, char *dest, int pad_num)
{
    //construct 
    

    if (0 == pad_num)
    {
        unsigned int src_tmp[3];
        src_tmp[0] = src[0];
        src_tmp[1] = src[1];
        src_tmp[2] = src[2];
        
        unsigned int dest_tmp[4];
        
        dest_tmp[0] = (src_tmp[0] & 0xfc) >> 2;
        dest_tmp[1] = ((src_tmp[0] & 0x03) << 4) | ((src_tmp[1] & 0xf0) >> 4);
        dest_tmp[2] = ((src_tmp[1] & 0x0f) << 2) | ((src_tmp[2] & 0xc0) >> 6);
        dest_tmp[3] = (src_tmp[2] & 0x3f);
    
        dest[0] = alpha[dest_tmp[0]];
        dest[1] = alpha[dest_tmp[1]];
        dest[2] = alpha[dest_tmp[2]];
        dest[3] = alpha[dest_tmp[3]];
        dest[4] = '\0';
    }
    if (1 == pad_num)
    {
        unsigned int src_tmp[2];
        src_tmp[0] = src[0];
        src_tmp[1] = src[1];
        
        unsigned int dest_tmp[3];
        
        dest_tmp[0] = (src_tmp[0] & 0xfc) >> 2;
        dest_tmp[1] = ((src_tmp[0] & 0x03) << 4) | ((src_tmp[1] & 0xf0) >> 4);
        dest_tmp[2] = (src_tmp[1] & 0x0f) << 2;
    
        dest[0] = alpha[dest_tmp[0]];
        dest[1] = alpha[dest_tmp[1]];
        dest[2] = alpha[dest_tmp[2]];
        dest[3] = alpha[64];
        dest[4] = '\0';
    }
    if (2 == pad_num)
    {
        unsigned int src_tmp[1];
        src_tmp[0] = src[0];
        
        unsigned int dest_tmp[2];
        
        dest_tmp[0] = (src_tmp[0] & 0xfc) >> 2;
        dest_tmp[1] = (src_tmp[0] & 0x03) << 4;
    
        dest[0] = alpha[dest_tmp[0]];
        dest[1] = alpha[dest_tmp[1]];
        dest[2] = alpha[64];
        dest[3] = alpha[64];
        dest[4] = '\0';
    }

    return RC_SUCCESS;
}

int Base64::alpha_revert(char src)
{
    int i = 0;
    while (i < 65 && src != alpha[i])
    {
        i++;
    }
    return i;
}

int Base64::decode_process(char *src, char *dest)
{
    int src_tmp[4];
    for (int i=0; i<4; i++)
    {
        src_tmp[i] = alpha_revert(src[i]);
        if (65 == src_tmp[i])
        {
            cout<<"revert error!"<<endl;
            return RC_SUCCESS;
        }
    }
    
    int dest_tmp[3];
    if (('=' != src[2]) && ('=' != src[3]))
    {
        dest[0] = ((src_tmp[0] & 0x3f) << 2) | ((src_tmp[1] & 0x30) >> 4);
        dest[1] = ((src_tmp[1] & 0x0f) << 4) | ((src_tmp[2] & 0x3c) >> 2);
        dest[2] = ((src_tmp[2] & 0x03) << 6) | (src_tmp[3] & 0x3f);
    }
    else if (('=' == src[2]) && ('=' == src[3]))
    {
        dest[0] = ((src_tmp[0] & 0x3f) << 2) | ((src_tmp[1] & 0x30) >> 4);
        dest[1] = '\0';
        dest[2] = '\0';
    }
    else if(('=' != src[2]) && ('=' == src[3]))
    {
        dest[0] = ((src_tmp[0] & 0x3f) << 2) | ((src_tmp[1] & 0x30) >> 4);
        dest[1] = ((src_tmp[1] & 0x0f) << 4) | ((src_tmp[2] & 0x3c) >> 2);
        dest[2] = '\0';
    }
    else
    {
        cout<<"source format error!"<<endl;
        return RC_ERROR;
    }
    return RC_SUCCESS;
}
