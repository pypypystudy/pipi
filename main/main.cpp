#include <iostream>
#include <stdlib.h>
#include "../base64/base64.h"

using namespace std;

int main(int argc, char *argv[])
{
  //input download addr
  char addr_in[128];
  cout<<"Please input the download addr:"<<endl;
  cin>>addr_in;
  
  //translate addr
  cout<<addr_in<<endl;
  Base64 base64;
  char addr_out[256];
  base64.encode(addr_in, addr_out);
  cout<<addr_out<<endl;
  
  
  system("PAUSE");	
  return 0;
}
