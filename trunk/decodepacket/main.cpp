#include <iostream>
#include <stdlib.h>

#include <fstream>

#include "../base64/base64.h"
#include "../md5/md5.h"
#include "../aes/kaes.h"
#include "decodepacket.h"

using namespace std;

int main(int argc, char *argv[])
{
    int rc = 0;

    unsigned char md5_key[8] = {0x35, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    unsigned char aes_key[16] = {0};
    MD5_CTX md5;
    md5.MD5Update(md5_key, 8);
    md5.MD5Final(aes_key);

    KAES aes(16, aes_key);

    ifstream input_file;
    ofstream output_file;
    unsigned char input[16] = {0};
    unsigned char output[16] = {0};

    input_file.open("encodepacket.txt", ios::binary);
    input_file.open("decodepacket.txt", ios::binary);

    int read_count = 0;
    while (!input_file.eof())
    {
        input_file.read((char *)input, 16);
        read_count = input_file.gcount();
        cout<<read_count<<endl;
        if (0 == read_count)
        {
            break;
        }
        aes.InvCipher(input, output);
        output_file.write((char *)output, 16);
    }

    input_file.close();
    output_file.close();
    
    return 0;
}
