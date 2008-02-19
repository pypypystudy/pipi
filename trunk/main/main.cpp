#include <iostream>
#include <stdlib.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include "../base64/base64.h"
#include "pipi.h"

using namespace std;

int main(int argc, char *argv[])
{
  int rc;

  /********************************************/
  //1.ADDRESS PROCESS
  /********************************************/
  //input download addr
  char addr_input[128], *addr_in;
  cout<<"Please input the download addr:"<<endl;
  cin>>addr_input;
  addr_in = addr_input;
  //check addr head is  'thunder://'
  if (0 == strncmp(addr_input, "thunder://", 10))
  {
      addr_in += 10;
  }
   
  //translate addr
  char addr_output[128], *addr_out;
  Base64 base64;
  rc = base64.decode(addr_in, addr_output);
  if (RC_SUCCESS != rc)
  {
    cout<<"error occur!"<<endl;
	return RC_ERROR;
  }

  //check addr head is "AA", and tail is "ZZ"
  {
  	int len = strlen(addr_output);
	addr_out = addr_output + (len - 2);
	if (0 == strncmp(addr_out, "ZZ", 2))
	{
	  *addr_out = '\0';
	}
	addr_out = addr_output;
	if (0 == strncmp(addr_out, "AA", 2))
	{
	  addr_out += 2;
	}

	cout<<addr_out<<endl;
  }

  //NEED TO DO: check addr_out is a correct addr

  //NEED TO DO: the input address may be not encode by BASE64, maybe is normal address,
  //                    these address should be support later.


  /********************************************/
  //2.CREATE CONNECTION
  /********************************************/
  //create socket
  
  int sockfd;
  struct sockaddr_in serv_addr;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  { 
    cout<<"create socket error!"<<endl;
    return RC_ERROR;
  }
  
  system("PAUSE");	
  return 0;
}
