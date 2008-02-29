#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

class ascii2hex{
    public:
        ascii2hex(const char input_filename[] = "input.txt", const char output_filename[] = "output.txt");
        void setinput(const char input_filename[]);
        void setoutput(const char output_filename[]);
        int execute();
    private:
        char input[64];
        char output[64];
};

ascii2hex::ascii2hex(const char input_filename[], const char output_filename[])
{
    strcpy(input, input_filename);
    strcpy(output, output_filename);
}

void ascii2hex::setinput(const char input_filename[])
{
    strcpy(input, input_filename);
}

void ascii2hex::setoutput(const char output_filename[])
{
    strcpy(output, output_filename);
}

int ascii2hex::execute()
{
    ifstream input_file;
    ofstream output_file;
    input_file.open(input);
    output_file.open(output, ios::binary);
    
    
    char ch = 0;
    int num = 0;
    int flag = 0;
    while (!input_file.eof())
    {
        input_file.get(ch);
        if (ch >= '0' && ch <= '9')//number0~9
        {
            num *= 16;
            num += ch - '0';
            flag = 1;
        }
        else if (ch >= 'A' && ch <= 'F')//number A~F
        {
            num *= 16;
            num += ch - '7';
            flag = 1;
        }
        else if (ch >= 'a' && ch <= 'f')//number a~f
        {
            num *= 16;
            num += ch - 'W';
            flag = 1;
        }
        else
        {
            if (1 == flag)
            {
                output_file.write((char *)&num, sizeof(char));
            }
            num = 0;
            flag = 0;
        }
    }
    
    input_file.close();
    output_file.close();
}

int main(int argc, char *argv[])
{
    ascii2hex a2h;
    a2h.execute();
    return EXIT_SUCCESS;
}
