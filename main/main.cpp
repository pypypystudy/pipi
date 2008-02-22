#include <iostream>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>   
#include <arpa/inet.h>

#include <fstream>

#include "../base64/base64.h"
#include "../md5/md5.h"
#include "../aes/kaes.h"
#include "pipi.h"


using namespace std;

PP_Downloader::PP_Downloader()
{
    set_recv_constant();
}
PP_Downloader::~PP_Downloader()
{
}

/********************************************/
//1.ADDRESS PROCESS
/********************************************/
int PP_Downloader::parse_address()
{
    int rc = 0;
    
    //input download addr
    unsigned char addr_input[128] = {0};
    unsigned char *addr_in;
    cout<<"Please input the download addr:"<<endl;
    cin>>addr_input;
    
    //check length
    int len = strlen((const char *)addr_input);
    if (ADDR_MAX_LENGTH < len)
    {
        cout<<"address length beyond the maxium length."<<endl;
        return RC_ERROR;
    }
    
    //check addr head is  'thunder://', if begin with it need decode by BASE64
    addr_in = addr_input;
    unsigned char addr_output[128] = {0};
    unsigned char *addr_out;
    if (0 == strncmp((const char *)addr_input, (const char *)ADDR_BASE64_HEAD, \
                                ADDR_BASE64_HEAD_LENGTH))
    {
        addr_in += ADDR_BASE64_HEAD_LENGTH;        
        Base64 base64;
        rc = base64.decode(addr_in, addr_output);
        if (RC_SUCCESS != rc)
        {
            cout<<"base64.decode error."<<endl;
            return RC_ERROR;
        }
        //check addr head is "AA", and tail is "ZZ"
        int len = strlen((const char *)addr_output);
        addr_out = addr_output + (len - 2);
        if (0 == strncmp((const char *)addr_out, (const char *)"ZZ", 2))
        {
            *addr_out = '\0';
        }
        addr_out = addr_output;
        if (0 == strncmp((const char *)addr_out, (const char *)"AA", 2))
        {
            addr_out += 2;
        }
        cout<<addr_out<<endl;
    }
    
    //should process other type addr here


    //save in private variable
    rc = set_addr(addr_out);
    if (RC_SUCCESS != rc)
    {
        cout<<"set addr error."<<endl;
        return RC_ERROR;
    }
    
    return RC_SUCCESS;
}

int PP_Downloader::parse_listfile()
{
    int rc = 0;
    //create socket
    int sockfd; 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
        cout<<"create socket error!"<<endl;
        return RC_ERROR;
    }

    //connect socket
    struct sockaddr_in serv_addr;  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVPORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVIP);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        cout<<"connect socket error!"<<endl;
        return RC_ERROR;
    }

    //repare for parse protocol: 
    //open pipi.resrc for save resource list
    //claculate md5 as aes key
    //ofstream recv_file;
    //recv_file.open(RECV_TEMP_FILE, ios::binary);

    unsigned char md5_key[8] = {0x34, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00};
    unsigned char key[16] = {0};
    MD5_CTX md5;
    md5.MD5Update(md5_key, 8);
    md5.MD5Final(key);
    set_aeskey(key);

    //send a fake request packet, should be instead of compose_requestpakcet
    unsigned char msg[282] = {
      0x50, 0x4F, 0x53, 0x54, 0x20, 0x2F, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2F, 0x31, 0x2E, 0x31, 0x0D,
      0x0A, 0x48, 0x6F, 0x73, 0x74, 0x3A, 0x20, 0x35, 0x38, 0x2E, 0x32, 0x35, 0x34, 0x2E, 0x33, 0x39,
      0x2E, 0x36, 0x3A, 0x38, 0x30, 0x0D, 0x0A, 0x43, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2D, 0x74,
      0x79, 0x70, 0x65, 0x3A, 0x20, 0x61, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E,
      0x2F, 0x6F, 0x63, 0x74, 0x65, 0x74, 0x2D, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6D, 0x0D, 0x0A, 0x43,
      0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2D, 0x4C, 0x65, 0x6E, 0x67, 0x74, 0x68, 0x3A, 0x20, 0x31,
      0x35, 0x36, 0x0D, 0x0A, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x20,
      0x4B, 0x65, 0x65, 0x70, 0x2D, 0x41, 0x6C, 0x69, 0x76, 0x65, 0x0D, 0x0A, 0x0D, 0x0A, 0x34, 0x00,
      0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x7F, 0xAA, 0x9D, 0x15, 0x94, 0x35,
      0xE4, 0x42, 0xA7, 0xE0, 0xDF, 0xDC, 0x41, 0x24, 0xA2, 0xB5, 0x17, 0x07, 0x55, 0xC8, 0x16, 0x00,
      0xE5, 0x5F, 0xB5, 0x95, 0x2A, 0x7B, 0x4C, 0x19, 0x46, 0x09, 0x73, 0x77, 0x55, 0xF5, 0x71, 0xCD,
      0x9D, 0xCD, 0x17, 0xF6, 0xAD, 0x25, 0xA2, 0x65, 0x3F, 0xB1, 0xBD, 0xCE, 0x2D, 0x62, 0x34, 0x2E,
      0x68, 0xEB, 0xF6, 0x86, 0x28, 0x2D, 0x45, 0xC2, 0x41, 0x9A, 0x09, 0x48, 0x7A, 0xFC, 0xB8, 0x90,
      0x24, 0x94, 0xF9, 0xB0, 0x63, 0x63, 0xA1, 0x3E, 0xB2, 0xE3, 0x60, 0x66, 0x51, 0xF0, 0x58, 0x41,
      0xAB, 0x0D, 0xDE, 0x99, 0x33, 0x61, 0xAF, 0xC1, 0x84, 0x92, 0x7C, 0x02, 0xF2, 0x85, 0x2B, 0x28,
      0x38, 0x94, 0x2C, 0xB7, 0x3C, 0xD2, 0x46, 0xE0, 0x98, 0xC2, 0x13, 0x54, 0x14, 0xBC, 0x64, 0xCA,
      0x43, 0xF2, 0xB6, 0x70, 0xAF, 0x5A, 0xF1, 0x08, 0xCF, 0x8E, 0xA8, 0x95, 0xF5, 0xCE, 0xFA, 0x8D,
      0xC5, 0xA1, 0x5C, 0xA3, 0x53, 0x98, 0x87, 0xC0, 0x05, 0x15};
    int sent_count = 0;
    sent_count = send(sockfd, msg, 282, 0);
    if (282 != sent_count)
    {
        cout<<"send msg error!"<<endl;
        close(sockfd);
        //recv_file.close();
        return RC_ERROR;
    }

    int recved_count = 0;
    unsigned char recv_buffer[RECV_BUFFER_SIZE] = {0};
    unsigned char recv_resp[1] = {0};
    int resp_count = 0;
    int recv_flag = 0;
    int pkt_size = 0;
    unsigned char *recv_ptr;
    while(1)
    {
        break;//for test
        
        if ((recved_count = recv(sockfd, recv_buffer, RECV_BUFFER_SIZE, 0)) == -1)
        {
            cout<<"recv msg error!"<<endl;
            close(sockfd);
            //recv_file.close();
            return RC_ERROR;
        }

        recv_ptr = recv_buffer;
        
        //check received message
        //if recv_buffer not equal to recv_flag, continue recv from server
        if (0 == (memcmp(recv_buffer, recv_constant, RECV_CONSTANT_LENGTH)))
        {
            //get packet size
            unsigned char *buffer_ptr = recv_buffer;
            buffer_ptr += RECV_CONSTANT_LENGTH;
            pkt_size = *((int *)buffer_ptr);
            cout<<"pkt_size="<<pkt_size<<endl;

            recv_ptr += FIRST_PACKET_HEAD_SIZE;
            recved_count  -= FIRST_PACKET_HEAD_SIZE;
            recv_flag = RECV_FIRST_PACKET_FLAG;//start recv packet
        }

        //if recv finish, exit while(1)
        if ((RECV_FIRST_PACKET_FLAG == recv_flag) && (pkt_size - recved_count < 0))
        {
            cout<<pkt_size - recved_count<<endl;
            break;
        }
        
        //process packet and save result in file
        if((RECV_FIRST_PACKET_FLAG == recv_flag) && (pkt_size - recved_count >= 0))
        {
            //recv_file.write((const char *)recv_ptr, recved_count);
            pkt_size -= recved_count;
        }

        //one recv, one response
        resp_count = send(sockfd, recv_resp, 0, 0);
        if (-1 == sent_count)
        {
            cout<<"send resp error!"<<endl;
            close(sockfd);
            //recv_file.close();
            return RC_ERROR;
        }    
    }

    //save resource list finish
    close(sockfd);
    //recv_file.close();

    //You are here, means the coded resource list file had been save in temp.resrc
    //now let's decode the file
    rc = decode_listfile();
    if (RC_SUCCESS != rc)
    {
        cout<<"decode_listfile error."<<endl;
	 return RC_ERROR;
    }

    return RC_SUCCESS;
}

int PP_Downloader::download_file()
{
    return RC_SUCCESS;
}

int PP_Downloader::compose_requestpacket(unsigned char *buffer)
{
    return RC_SUCCESS;
}

int PP_Downloader::set_addr(unsigned char *addr_input)  
{
    return RC_SUCCESS;
}

unsigned char * PP_Downloader::get_addr()
{
    return addr;
}

int PP_Downloader::set_recv_constant()
{
    unsigned char temp[] = {0x34, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00};
    memcpy(recv_constant, temp, RECV_CONSTANT_LENGTH);
    return RC_SUCCESS;
}

unsigned char * PP_Downloader::get_recv_constant()
{
    return recv_constant;
}

int PP_Downloader::set_aeskey(unsigned char * key)
{
    memcpy(aes_key, key, AES_KEY_LENGTH);
    return RC_SUCCESS;
}

unsigned char * PP_Downloader::get_aeskey()
{
    return aes_key;
}

int PP_Downloader::decode_listfile()
{
    ifstream input_file;
    ofstream output_file;
    ofstream temp_file;
    int file_size;
    
    input_file.open(RECV_TEMP_FILE, ios::binary);
    output_file.open(RESOURCE_FILE);
    temp_file.open("temp", ios::binary);

    //get file size
    input_file.seekg(0, ios::end);
    file_size = input_file.tellg();
    input_file.seekg(0, ios::beg);
    file_size += FILE_SIZE_PADDING;

    //malloc mem save decode file
    unsigned char *decode_mem = NULL;
    decode_mem = (unsigned char *)malloc(file_size);
    if (NULL == decode_mem)
    {
        cout<<"malloc memory error."<<endl;
        input_file.close();
        output_file.close();
        return RC_ERROR;
    }
    memset(decode_mem, 0, file_size);
    
    //decode file by AES into decode_mem
    KAES aes(AES_KEY_LENGTH, get_aeskey());
    unsigned char input[AES_DECODE_LENGTH] = {0};
    unsigned char output[AES_DECODE_LENGTH] = {0};
    unsigned char *mem_ptr = decode_mem;

    int a = 0;
    while(!input_file.eof())
    {
        input_file.read((char *)input, AES_DECODE_LENGTH);
        aes.InvCipher(input, output);
        temp_file.write((char *)output, AES_DECODE_LENGTH);
        memcpy(mem_ptr, output, AES_DECODE_LENGTH);
        mem_ptr += AES_DECODE_LENGTH;
        a += 16;
        cout<<a<<endl;
    }
    temp_file.close();
    mem_ptr -= AES_DECODE_LENGTH;

    //find useful link from decode_mem then save in pipi.resrc
    //now mem_ptr is the end of mem
    unsigned char *curr_ptr = NULL;
    int link_length = 0;

    curr_ptr = decode_mem;
    
    printf("start=%p\n", decode_mem);
    printf("end=%p\n", mem_ptr);

    while(curr_ptr < mem_ptr)
    {
        if ((0 == strncmp((const char *)curr_ptr, USEFUL_LINK_HTTP, USEFUL_LINK_HTTP_LENGTH))
            || (0 == strncmp((const char *)curr_ptr, USEFUL_LINK_FTP, USEFUL_LINK_FTP_LENGTH)))
        {
            link_length = strlen((const char *)curr_ptr);
            output_file<<curr_ptr<<endl;
            cout<<curr_ptr<<endl;
            curr_ptr += link_length;
            break;
        }
        curr_ptr ++;
    }
    
    free(decode_mem);    
    input_file.close();
    output_file.close();
    
    return RC_SUCCESS;
}

int main(int argc, char *argv[])
{
    int rc = 0;
    PP_Downloader pipi;
    
    rc = pipi.parse_address();
    if (RC_SUCCESS != rc)
    {
        cout<<"parse_address error."<<endl;
        return RC_ERROR;
    }
    
    rc = pipi.parse_listfile();
    if (RC_SUCCESS != rc)
    {
        cout<<"parse_listfile error."<<endl;
        return RC_ERROR;
    }

    rc = pipi.download_file();
    if (RC_SUCCESS != rc)
    {
        cout<<"download_file error."<<endl;
        return RC_ERROR;
    }
     
    return 0;
}
