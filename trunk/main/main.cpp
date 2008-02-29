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
    unsigned char *addr_out = addr_input;
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

    cout<<addr<<endl;
    
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
    ofstream recv_file;
    recv_file.open(RECV_TEMP_FILE, ios::binary);

    unsigned char md5_key[8] = {0x34, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00};
    unsigned char key[16] = {0};
    MD5_CTX md5;
    md5.MD5Update(md5_key, 8);
    md5.MD5Final(key);
    set_aeskey(key);

    //send a request packet
    int send_length;
    unsigned char msg[COMPOSE_MAX_BUFFER_SIZE] = {0};
    rc = compose_requestpacket(msg, &send_length);
    if (RC_SUCCESS != rc)
    {
        cout<<"compose_requestpacket error."<<endl;\
        close(sockfd);
        recv_file.close();
        return RC_ERROR;
    }
    
    int sent_count = 0;
    sent_count = send(sockfd, msg, send_length, 0);
    if (send_length != sent_count)
    {
        cout<<"send msg error!"<<endl;
        close(sockfd);
        recv_file.close();
        return RC_ERROR;
    }

    int recved_count = 0;
    unsigned char recv_buffer[RECV_BUFFER_SIZE] = {0};
    unsigned char recv_resp[1] = {0};
    int resp_count = 0;
    int recv_flag = 0;
    int pkt_size = 0;
    unsigned char *recv_ptr;
    unsigned char *recv_end;
    while(1)
    {
        if ((recved_count = recv(sockfd, recv_buffer, RECV_BUFFER_SIZE, 0)) == -1)
        {
            cout<<"recv msg error!"<<endl;
            close(sockfd);
            recv_file.close();
            return RC_ERROR;
        }

        recv_ptr = recv_buffer;
        recv_end = recv_buffer + recved_count;
        
        //check received message
        //if recv_buffer not equal to recv_flag, continue recv from server
        while((recv_ptr < recv_end) && (RECV_FIRST_PACKET_FLAG != recv_flag))
        {
            if (0 == (memcmp(recv_ptr, recv_constant, RECV_CONSTANT_LENGTH)))
            {
                //get packet size
                unsigned char *buffer_ptr = recv_ptr;
                buffer_ptr += RECV_CONSTANT_LENGTH;
                pkt_size = *((int *)buffer_ptr);
                cout<<pkt_size<<endl;

                recv_ptr += FIRST_PACKET_HEAD_SIZE;
                recved_count  -= recv_ptr - recv_buffer;
                recv_flag = RECV_FIRST_PACKET_FLAG;//start recv packet
                break;
            }
            recv_ptr++;
        }
       
        //process packet and save result in file
        if((RECV_FIRST_PACKET_FLAG == recv_flag) && (pkt_size - recved_count >= 0))
        {
            recv_file.write((const char *)recv_ptr, recved_count);
            pkt_size -= recved_count;
        }

        //if recv finish, exit while(1)
        if ((RECV_FIRST_PACKET_FLAG == recv_flag) && (pkt_size <= 0))
        {
            break;
        }

        //one recv, one response
        resp_count = send(sockfd, recv_resp, 0, 0);
        if (-1 == sent_count)
        {
            cout<<"send resp error!"<<endl;
            close(sockfd);
            recv_file.close();
            return RC_ERROR;
        }    
    }

    //save resource list finish
    close(sockfd);
    recv_file.close();

    return RC_SUCCESS;
}

int PP_Downloader::download_file()
{
    return RC_SUCCESS;
}

//you can get the request packet format in file requestpacketformat.txt
int PP_Downloader::compose_requestpacket(unsigned char *buffer, int *length)
{
    unsigned char http_head[126] = {
0x50, 0x4F, 0x53, 0x54, 0x20, 0x2F, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2F, 0x31, 0x2E, 0x31, 0x0D,
0x0A, 0x48, 0x6F, 0x73, 0x74, 0x3A, 0x20, 0x35, 0x38, 0x2E, 0x32, 0x35, 0x34, 0x2E, 0x33, 0x39,
0x2E, 0x36, 0x3A, 0x38, 0x30, 0x0D, 0x0A, 0x43, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2D, 0x74,
0x79, 0x70, 0x65, 0x3A, 0x20, 0x61, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E,
0x2F, 0x6F, 0x63, 0x74, 0x65, 0x74, 0x2D, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6D, 0x0D, 0x0A, 0x43,
0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2D, 0x4C, 0x65, 0x6E, 0x67, 0x74, 0x68, 0x3A, 0x20, 0x31,
0x35, 0x36, 0x0D, 0x0A, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x20,
0x4B, 0x65, 0x65, 0x70, 0x2D, 0x41, 0x6C, 0x69, 0x76, 0x65, 0x0D, 0x0A, 0x0D, 0x0A};

    unsigned char md5_key[8] = {0x34, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00};
    int decode_length = 0;
    unsigned char decode_head[9] = {0x94, 0x01, 0x05, 0x00, 0x00, 0x00, 0xD1, 0x07, 0x00};
    int address_length = 0;
    unsigned char decode_tail[55] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x30, 0x30, 0x31, 0x32, 0x33, 0x46, 0x37,
0x44, 0x43, 0x31, 0x44, 0x45, 0x30, 0x30, 0x30, 0x30, 0x2F, 0x9A, 0x38, 0x6E, 0x2A, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //calculate length
    int pad_num = 0;
    address_length = strlen((const char *)addr);
    decode_length = 9 + 4 + address_length + 55;
    pad_num = 16 - (decode_length % 16);
    decode_length += pad_num;

    //get length
    *length = decode_length + 126 + 8 + 4;

    unsigned char *buffer_ptr = buffer;
    //copy http part
    memcpy(buffer_ptr, http_head, 126);
    buffer_ptr += 126;

    //copy md5 hash key
    memcpy(buffer_ptr, md5_key, 8);
    buffer_ptr += 8;

    //copy decode length
    {
        int *p = (int *)buffer_ptr;
        *p = decode_length;
        buffer_ptr += 4;
    }

    //prepare decode packet memory
    unsigned char *mem_ptr = NULL;
    unsigned char *cur_ptr = NULL;

    mem_ptr = (unsigned char *)malloc(decode_length);
    if (NULL == mem_ptr)
    {
        cout<<"malloc error."<<endl;
        return RC_ERROR;
    }
    memset(mem_ptr, 0, decode_length);

    cur_ptr = mem_ptr;

    //copy decode head
    memcpy(cur_ptr, decode_head, 9);
    cur_ptr += 9;

    //copy address length
    {
        int *p = (int *)cur_ptr;
        *p = address_length;
        cur_ptr += 4;
    }

    //copy address
    memcpy(cur_ptr, addr, address_length);
    cur_ptr += address_length;

    //copy decode tail
    memcpy(cur_ptr, decode_tail, 55);
    cur_ptr += 55;

    //copy pad
    for (int i=0; i<pad_num; i++)
    {
        *cur_ptr = 0x0C;
        cur_ptr++;
    }

    //here, we composed the packet which need to be aes encode, let's do it now.
    //and copy decode packet
    cur_ptr = mem_ptr;
    KAES aes(AES_KEY_LENGTH, get_aeskey());
    while (decode_length > 0)
    {
        aes.Cipher(cur_ptr, buffer_ptr);
        cur_ptr += 16;
        buffer_ptr += 16;
        decode_length -= 16;
    }
    
    free(mem_ptr);

    return RC_SUCCESS;
}

int PP_Downloader::set_addr(unsigned char *addr_input)  
{
    strcpy((char *)addr, (char *)addr_input);
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
    int file_size;
    
    input_file.open(RECV_TEMP_FILE, ios::binary);
    output_file.open(RESOURCE_FILE);

    //get file size
    input_file.seekg(0, ios::end);
    file_size = input_file.tellg();
    input_file.seekg(0, ios::beg);

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

    while(!input_file.eof())
    {
        int read_count = 0;
        input_file.read((char *)input, AES_DECODE_LENGTH);
        read_count = input_file.gcount();
        if (0 == read_count)
        {
            break;
        }
        aes.InvCipher(input, output);
        memcpy(mem_ptr, output, AES_DECODE_LENGTH);
        mem_ptr += AES_DECODE_LENGTH;
    }

    //find useful link from decode_mem then save in pipi.resrc
    //now mem_ptr is the end of mem
    unsigned char *curr_ptr = NULL;
    int link_length = 0;

    curr_ptr = decode_mem;
    
    while(curr_ptr < mem_ptr)
    {
        if ((0 == strncmp((const char *)curr_ptr, USEFUL_LINK_HTTP, USEFUL_LINK_HTTP_LENGTH))
            || (0 == strncmp((const char *)curr_ptr, USEFUL_LINK_FTP, USEFUL_LINK_FTP_LENGTH)))
        {
            link_length = strlen((const char *)curr_ptr);
            output_file<<curr_ptr<<endl;
            curr_ptr += link_length;
            continue;
        }
        curr_ptr ++;
    }
    
    free(decode_mem);    
    input_file.close();
    output_file.close();
    //read resource file
    /*
    ifstream test;
    test.open(RESOURCE_FILE);
    unsigned char aaa[256] = {0};
    int read_count = 0;
    while(!test.eof())
    {
        read_count = test.gcount();
        if (0 == read_count)
        {
            break;
        }
        test>>aaa;
        cout<<aaa<<endl;
    }
    */
    
    return RC_SUCCESS;
}

void PP_Downloader::print_mem16(unsigned char *mem)
{
    for (int i=0; i<16; i++)
    {
        printf("%02x ", mem[i]);
    }
	printf("\n");
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

    rc = pipi.decode_listfile();
    if (RC_SUCCESS != rc)
    {
        cout<<"decode_listfile error."<<endl;
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
