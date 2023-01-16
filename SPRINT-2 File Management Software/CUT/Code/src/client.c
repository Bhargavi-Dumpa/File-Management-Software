
#include "client.h"

char permission[30];
int main()
{
    int choice;

    // char array to store incoming and outgoing message
    char net_buf[NET_BUF_SIZE]; /* Used for Data and Command Transfer*/
    char file_name[FILE_SIZE];/* Requested File Name */
    socklen_t addrlen;// = sizeof(addr_con);
                      // Socket id
    int clientSocket, ret;

    // Client socket structure
    struct sockaddr_in cliAddr;

    struct sockaddr_in serverAddr;


    // Creating socket id
    clientSocket = socket(AF_INET,SOCK_STREAM, 0);

    if (clientSocket < 0) {
        perror("[error] Socket");
        exit(EXIT_FAILURE);
    }

    printf("\t\t\tSocket is created.\n");

    // Initializing socket structure with NULL
    memset(&cliAddr, '\0', sizeof(cliAddr));


    // Assigning port number and IP address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT_NO);

    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // connect() to connect to the server
    ret = connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    if (ret < 0) {
        perror("[error] Connection");
        exit(EXIT_FAILURE);
    }

    
    printf(" Connected to server.\n");
    printf(" Please login to continue ... \n");

    int auth_check = authentication_request(clientSocket,net_buf);

    if(auth_check != 0)
    {
        printf("multiple login attempt and failed!!! \n");
        end_connection(clientSocket,net_buf);
        return -1;
    }

    user_choice(clientSocket,net_buf,file_name);

    return EXIT_SUCCESS;
}

//authentication_request 

int authentication_request(int sockfd,char *net_buf)
{
    char username[20];
    char password[20];
    int failed_count_username = 0;
    int failed_count_password = 0;
    int ch;
    printf("=========================================\n");
    printf("\n                   MAIN MENU             \n");
    printf("\n=========================================\n");   
    printf("\n1.login\n\n2.logout\n");
    printf("\n=========================================\n");   
    printf("enter the choice: ");
    scanf("%d",&ch);

    while(1)
    {
     switch(ch){
	case 1:
        	if(failed_count_username >= 3 || failed_count_password >=3)
        	{
            		break;
        	}

        	printf("Enter username: ");
        	scanf("%s",username);
        
        	/*Sending Request to Server*/
        	//clearBuf(net_buf);
        	strcpy(net_buf,AUTH_REQUEST);
       		send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        	clearBuf(net_buf);

        	/*Sending Username*/
        	strcpy(net_buf,username);
        	send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        	clearBuf(net_buf);
        	/*Receiving Acknowlegment of User in Database*/
       		 recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

       		 /*If user is not  present send again for authentication*/
        	if(strcmp(net_buf,CMD_NOT_SUCCESS)==0)
        	{
            	failed_count_username++;
           	 printf("No such user found: %d attempts left!!!\n",(3 - failed_count_username));
            	continue;
        	}
        
        	printf("Password for %s: ", username);
        	scanf("%s",password);
        	/*Sending Password*/
        	strcpy(net_buf,password);
        	send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        	clearBuf(net_buf);
        
        	recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        	/*If wrong password entered send again for authentication*/
        	if(strcmp(net_buf,CMD_NOT_SUCCESS)==0)
        	{

            	failed_count_password++;
            	printf("Wrong password: %d attempts left!!!\n",(3-failed_count_password));
            	continue;
        	}
		/*permissions*/
   		clearBuf(net_buf);
	
        	recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
		strcpy(permission,net_buf);	 
        	printf("Logged in successfully....... \n");   
       		 return 0;
		break;

	case 2: printf("Successfully loggedout!!! \n"); 
                end_connection(sockfd,net_buf);
                break;
      

		
     }

    return -1;
}
}



// common


/*
Description : Set all memory values to ‘\0’ of the char pointer.
*
*/


void clearBuf(char *b)
{
  int i;
  for(i = 0;i<NET_BUF_SIZE;i++)
  {
    b[i] = '\0';
  }
}

/*
Description : Used to encrypt and decrypt the file content character by character.
*/


char Cipher(char ch)
{
  return ch ^ cipherKey; //Encryption using XOR

}

/*

Description : Used to save the file on clienthe t side with the specified filename by server.

*/


int save_file(char *buf,int s,char *file_name)
{
  FILE *fptr;
  int i;
  char ch;
  char file_error[] = "File_Not_Found";
  char base_address[FILE_SIZE] = "./users-drive-download-files/";
  strcat(base_address,file_name);
  /*    if(strcmp(buf,file_error)==0)
        {
        return;
        }
        */
  fptr = fopen(base_address,"w+");

if(fptr == NULL)
{
	printf("File not Found \n");
return -1;
}

  for(i = 3;i<s;i++)
  {
    ch = buf[i];
    ch = Cipher(ch);
    if(ch == EOF)
    {
      return 1;
    }
    else
    {
      fputc(ch,fptr);
      printf("%c",ch);
      //sleep(2);
    }
  }

  fclose(fptr);
  return 0;
}

/*
Description : Used to decrypt the received file content 
*/


int recv_file(char *buf,int s)
{
  int i, cnt = 1;
  char ch;
  //printf("");
  for(i = 0;i<s;i++)
  {
    ch = buf[i];
    ch = Cipher(ch);
    if(ch == EOF)
    {

      printf("\n%d items found \n", cnt);
      return 1;
    }
    else
    {
      printf("%c",ch);
      if(ch == '\n')
      {
        cnt++;
       printf("%d)",i+1);
      }
    }
   
  }

  return 0;
}

/*

Description : Checks if the file present on client side in download folder 

*/

int file_found(char *buf,int s)
{
  char file_error[] = "File_Not_Found";
  char string[strlen(buf) - 1];
  int i = 0;
  for(i=0;i<strlen(buf)-1;i++)
  {
    string[i] = Cipher(buf[i]);
  }
  printf("%s\n",string);
  
  if(strcmp(string,file_error)==0)
  {
    return -1;
  }

  return 0;
}

/*

Description : Used to encrypt the contents of the file to be sent.
*/


int send_file(FILE *fp,char *buf,int s)
{
  int i, len;
  if(fp==NULL)
  {
    strcpy(buf,nofile);
    len = strlen(nofile);
    buf[len] = EOF;
    for(i = 0;i<=len;i++)
    {
      buf[i] = Cipher(buf[i]);
    }
    return 1;
  }
  char ch,ch2;
  for(i =0;i<s;i++)
  {
    ch = fgetc(fp);
    ch2 = Cipher(ch);
    buf[i] = ch2;
    if(ch == EOF)
      return 1;
  }

  return 0;
}

/*
Description : Used to end connection from client side works as logout
*/



void end_connection(int sockfd,char *net_buf)
{
  strcpy(net_buf,END_CONNECTION);
  printf("Client is disconnected...........\n");
  send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
}



// delete

void delete(int sockfd,char *net_buf)
{

    int nBytes;
    FILE *fp;
    char *file_name;
    clearBuf(net_buf);
    strcpy(net_buf,DELETE);
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    clearBuf(net_buf);
    printf("\n[+] Please Enter file name to be deleted....\n");
    scanf("%s",net_buf);
    if(strcmp(net_buf,EXIT)==0)
    {
        printf("you can't use %s as filename \n", EXIT);
        return;
    }
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    clearBuf(net_buf);

    recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

    if(strcmp(net_buf,CMD_SUCCESS)==0)
    {
        printf("File Deleted Sucessfully\n");
    }
    else
    {
        printf("%s\n",nofile);
    }
}

//download 

void download(int sockfd,char *net_buf,char *file_name)
{
    int nBytes;
    clearBuf(net_buf);
    strcpy(net_buf,PUSH_DOWNLOAD_FILE);
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    clearBuf(net_buf);
    printf("\n[+] Please Enter file name....\n");
    scanf("%s",net_buf);
    strcpy(file_name,net_buf);
    if(strcmp(net_buf,EXIT)==0)
    {
        return;
    }
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

    printf("\n************* Downloaded Content *************\n");

    while(1)
    {
        clearBuf(net_buf);
        nBytes = recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
	
        /*saving file at client side*/
        if(file_found(net_buf,NET_BUF_SIZE)==-1)
        {
            break;
        }

        if(save_file(net_buf,NET_BUF_SIZE,file_name))
        {
            break;
        }
    }
    printf("\n**********************************************\n"); 
}

//list all downloaded files

void list_all_downloaded()
{
    printf("All downloaded files \n");

    DIR *d;
    struct dirent *dir;
    d = opendir("./users-drive-download-files/");
    if (d)
    {
        int cnt = 0;
        printf("\n*************** Data Received ****************\n");
        while ((dir = readdir(d)) != NULL)
        {
            printf("%d) %s\n",cnt+1, dir->d_name);
            cnt++;
        }

        printf("\n %d items found !!! \n", cnt);
        closedir(d);
        printf("\n**********************************************\n");
    }
    else
    {
        printf("No data found!!! \n");
    }
}

//list_all_drive_files

void list_all_drive_files(int sockfd,char *net_buf)
{
    printf("All files in server \n");

    int nBytes;
    clearBuf(net_buf);
    strcpy(net_buf,LIST_DIR);
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    clearBuf(net_buf);
    printf("\n************** User Drive Files ***************\n");
  
    while(1)
    {
        clearBuf(net_buf);
        nBytes = recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        if(recv_file(net_buf,NET_BUF_SIZE))
        {
            break;
        }
    }
    
    printf("\n***********************************************\n"); 
}


// upload file
void upload(int sockfd,char *net_buf)
{
    int nBytes;
    FILE *fp;
    char *file_name;
    clearBuf(net_buf);
    strcpy(net_buf,PUSH_UPLOAD_FILE);
    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    clearBuf(net_buf);
    printf("\nPlease enter file name to upload....\n");
    scanf("%s",net_buf);

    if(strcmp(net_buf,EXIT)==0)
    {
        printf("You can't use %s as file name \n", EXIT);
        return;
    }

    send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
    //printf("%s\n",net_buf);

    char base_address[100] = "./users-drive-download-files/";
    strcat(base_address,net_buf);
    fp = fopen(base_address,"rb");

    if(fp == NULL)
    {
        printf("\nFile open failed\n");
    }
    else
    {
        printf("\nFile open successfully\n");
    }

    //printf("\n***********Downloaded Content************\n");

    while(1)
    {
        if(send_file(fp,net_buf,NET_BUF_SIZE)){
            send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
            clearBuf(net_buf);

            break;
        }
        if(fp!=NULL)
        {
            fclose(fp);
        }
    }

}



void print_menu()
{
  printf("\n\n==============================================\n");
  printf("\n==================== MENU ====================\n");
  printf("\n===============================================\n"); //46 characters
  printf("--------------------SELECT--------------------\n\n");
  printf("1. List all downloaded files \n");
  printf("2. List all  files on server \n");
  printf("3. To download a file \n");
  printf("4. To upload a file \n");
  printf("5. To delete a file \n");
  printf("6. Logout \n\n");
  printf("===================================================\n");

  printf("\nEnter your choice :- ");


}


//user choice

int user_choice(int sockfd,char *net_buf,char *file_name)
{
  int clientSocket;
  while(1)
  {
    char input[50];
    char choice;
  
    print_menu();

    scanf("%s", input);

    if(strlen(input) > 1)
    {
      printf("[error] Invalid input!!! please choose a number from the menu \n");
      continue;
    }
    
    choice = *input;

    switch(choice)
    {
      case '1': list_all_downloaded();
              break;

      case '2': list_all_drive_files(sockfd,net_buf);
              break;


      case '3': if(permission[0] == 'r' && permission[1] == 'e')
                    {
                        download(sockfd,net_buf,file_name);
                    }
                    else
                    {
                    printf("Access Denied\n");
                    }
                    break;


      case '4': if(permission[2] == 'w' && permission[3] == 'r')
                    {
                        upload(sockfd,net_buf);
                    }
                    else
                    {
                        printf("Access Denied\n");
                    }
                    break;

      case '5': if(permission[4] == 'd' && permission[5] == 'e')
                    {
                        delete(sockfd,net_buf);
                    }
                    else
                    {
                        printf("Access Denied\n");
                    }
                    break;

      case '6':
              printf("Successfully loggedout!!! \n"); 
	      //authentication_request(clientSocket,net_buf);
              end_connection(sockfd,net_buf);
              return 0;

      default: printf("Invalid input!!! please choose a number from above menu \n");

    }

  }
}





