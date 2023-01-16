/*
 * Description : Initializing and setting pararametrs of concurrent server implemented using threads 
 */

#include "server.h"

int main()
{
    char net_buf[NET_BUF_SIZE];
    FILE *fp;


    int sockfd, ret;

    // Server socket address structures
    struct sockaddr_in serverAddr;

    // Client socket id
    int clientSocket;

    // Client socket address structures
    struct sockaddr_in cliAddr;

    // Stores byte size of server socket address
    socklen_t addr_size;

    // Child process id
    pid_t childpid;

    // Creates a TCP socket id from IPV4 family
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error handling if socket id is not valid
    if (sockfd < 0) 
    {
        perror("error in Connection.........");
        exit(EXIT_FAILURE);
    }

    printf("Server socket is created...........\n");

    // Initializing address structure with NULL
    memset(&serverAddr, '\0', sizeof(serverAddr));

    // Assign port number and IP address
    // to the socket created
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT_NO);

    //IP address
    serverAddr.sin_addr.s_addr
        = inet_addr(IP_ADDRESS);

    // Binding the socket id with
    // the socket structure
    ret = bind(sockfd,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr));

    // Error handling
    if (ret < 0) {
        perror("error in Binding.......");
        exit(EXIT_FAILURE);
    }

    // Listening for connections (upto 10)
    if (listen(sockfd, 10) == 0) {
        printf("Server is listening...........\n\n");
    }

    int cnt = 0;


    while(1)
    {
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr,&addr_size);

        // Error handling
        if (clientSocket < 0) 
        {
            exit(1);
        }

        // Displaying information of
        // connected client
        printf("Connection accepted from %s:%d\n",
                inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port));

        // Print number of clients
        // connected till now
        printf("Clients connected: %d\n\n",
                ++cnt);

        pthread_t t;

        //arguments to be passed in pthread_create for client handler function 
        connection_data_t *parcel_ptr;
        connection_data_t parcel;
        parcel.clientSocket = clientSocket;
        parcel.net_buf = net_buf;

        parcel_ptr = &parcel;

        //HandleConnection(parcel_ptr);

        pthread_create(&t,NULL,handle_connection,parcel_ptr);

    }
    return EXIT_SUCCESS;
}

#include <netinet/in.h>


/*structure to hold user info and permissions*/
struct account 
{
    char id[20];
    char password[20];
    char read[10];
    char write[10];
    char delete[10];
};
static struct account accounts[10];

int read_file(struct account accounts[])
{
    FILE *fp;
    int i=0;   // count how many lines are in the file
    int c;
    fp=fopen("server-files/UserDB", "r");
    while(!feof(fp)) {
        c=fgetc(fp);
        if(c=='\n')
            ++i;
    }
    int j=0;
    rewind(fp); 
        // read each line and put into accounts
    while(j!=i-1) {
        fscanf(fp, "%s %s %s %s %s", accounts[j].id, accounts[j].password, accounts[j].read, accounts[j].write, accounts[j].delete);
        ++j;
    }
    return ((i/2) + 1);
}

/*Authentication of user*/
void authentication(int sockfd,char *net_buf)
{
    int total_entries = read_file(accounts);
    printf("Authentication process start from client sockfd %d \n", sockfd);
    char username[20];
    char password[20];
    char permission[30];
    int clientSocket;

       /*Attending Request of Username From Client*/
        clearBuf(net_buf);
        recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        for(int i = 0;i<total_entries;i++)
        {
            if(strcmp(accounts[i].id,net_buf)==0)
            {
                strcpy(username,net_buf);
                strcpy(password,accounts[i].password);
		strcpy(permission,accounts[i].read);
		strcat(permission,accounts[i].write);
		strcat(permission,accounts[i].delete);

            }
        }
        printf("username received : %s\n",net_buf);
        if(strcmp(username,net_buf) == 0)
        {
            strcpy(net_buf,CMD_SUCCESS);
            send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
            clearBuf(net_buf);
            recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
            printf("password received : %s\n",net_buf);

            /*Attending Request of Correct Password From Client*/
            if(strcmp(password,net_buf)==0)
            {
                clearBuf(net_buf);
                strcpy(net_buf,CMD_SUCCESS);
                send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

                 printf("Data sent sucessfully .......");  //%s\n",net_buf

		clearBuf(net_buf);
		strcpy(net_buf,permission);
                send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
            }
            else
            {
                clearBuf(net_buf);
                strcpy(net_buf,CMD_NOT_SUCCESS);
                send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
            }

        } 
         

		
        else
        {

            clearBuf(net_buf);
            strcpy(net_buf,CMD_NOT_SUCCESS);
            send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
        }

}






void clearBuf(char *b)
{
    int i;
    for(i = 0;i<NET_BUF_SIZE;i++)
    {
        b[i] = '\0';
    }
}
/*
Description : Used to encrypt and decrypt the file based on the XOR operation and cipher key.
*/

char Cipher(char ch)
{
    return ch ^ cipherKey; //Encryption using XOR
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
Description : Used to save the file on server with the specified filename by user.
*/


int recv_file_save(char *buf,int s,char *file_name)
{
    FILE *fp;
    int i;
    char ch;
    char base_address[FILE_SIZE] = "./shared-storage/";
    strcat(base_address,file_name);
    fp = fopen(base_address,"w+");
    if(fp == NULL)
    {
        printf("\nFile open error\n");
        return -1;
    }

    //character-by-character encryption and saving file
    for(i = 0;i<s;i++)
    {
        ch = buf[i];
        ch = Cipher(ch);
        if(ch == EOF)
        {
            return 1;
        }
        else
        {
            fputc(ch,fp);
            printf("%c",ch);
        }
    }

    fclose(fp);
    return 0;
}




void delete_file(int sockfd,char *net_buf)
{
    	char file_name[FILE_SIZE];
        printf("\n Waiting for file name to be deleted....\n");

        recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

       printf(" File Name Received: %s\n",net_buf);

       strcpy(file_name,net_buf);

       clearBuf(net_buf);
       char base_address[FILE_SIZE] = "./shared-storage/";
       strcat(base_address,file_name);

       if(remove(base_address) == 0)
       {
           strcpy(net_buf,CMD_SUCCESS);
       }
       else
       {
           strcpy(net_buf,CMD_NOT_SUCCESS);
       }
       send(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);
       clearBuf(net_buf);
       
}



void get_user_file(int sockfd,char *net_buf)
{
    FILE *fp;
    int nBytes;
    char file_name[FILE_SIZE];
        printf("\nWaiting for uploaded file name....\n");
        //clearBuf(net_buf);
        nBytes = recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

       printf("File Name Received: %s\n",net_buf);

       strcpy(file_name,net_buf);


       printf("********************File Content*******************\n");
       while(1)
       {
        clearBuf(net_buf);
        nBytes = recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);


        /*saving file at server side*/
        
        if(recv_file_save(net_buf,NET_BUF_SIZE,file_name))
        {

            break;
        }
    }
    printf("\n***************************************************\n");

}


//handle_connection

void *handle_connection(void *new_socket_details)
{
    connection_data_t *parcel_ptr = (connection_data_t *) new_socket_details;

    int clientSocket;
    char *net_buf;

    clientSocket = parcel_ptr->clientSocket;
    net_buf = parcel_ptr->net_buf ;


    /*Receiving command*/
        while(1)
        {
            recv(clientSocket,net_buf,NET_BUF_SIZE,sendrecvflag);

            printf("Command Received : %s\n",net_buf);

            /*if(strcmp(net_buf,CMD_SUCCESS) == 0 || strcmp(net_buf,CMD_NOT_SUCCESS) == 0 || (net_buf == NULL) )
            {
                clearBuf(net_buf);
                continue;
            }*/
            if(strcmp(net_buf,AUTH_REQUEST)==0)
            {
                authentication(clientSocket,net_buf);
            }

            if(strcmp(net_buf,LIST_DIR)==0)
            {
                list_all(clientSocket,net_buf);
            }
            else if(strcmp(net_buf,PUSH_DOWNLOAD_FILE)==0)
            {
                push_download_file(clientSocket,net_buf);
            }
            else if(strcmp(net_buf,PUSH_UPLOAD_FILE)==0)
            {
                get_user_file(clientSocket,net_buf);
            }
            else if(strcmp(net_buf,DELETE) == 0)
            {
                delete_file(clientSocket,net_buf);
            }
            else if(strcmp(net_buf,END_CONNECTION)==0)
            {
                close(clientSocket);
                break;
            }

        }
        return NULL;
}



//list_all

void list_all(int sockfd,char *net_buf)
{
    printf("[+] LIST_DIR command received %s \n",net_buf);
    FILE *fp1,*fp2;
    DIR *d;
    struct dirent *dir;
    char string[NET_BUF_SIZE];
    d = opendir("./shared-storage/");
    fp1 = fopen("./server-files/Directory_Index.txt","wb");
    if (d)
    {
        printf("*********************Listed All Files********************\n");
        while ((dir = readdir(d)) != NULL)
        {
            if(fp1 ==NULL)
            {
                printf("\n LIST_DIR command failed!!\n");
            }
            else
            {
                strcat(string,dir->d_name);
                strcat(string,EOL);
            }
        }

        string[strlen(string) - 1] = EOF;
        fwrite(string,sizeof(char),sizeof(string),fp1);
        closedir(d);

        fclose(fp1);
        printf("*********************************************************\n");
    }
    clearBuf(net_buf);
    fp2 = fopen("./server-files/Directory_Index.txt","rb");
    if(fp2 ==NULL)
    {
        printf("\n[failed] File open failed!!\n");
    }
    else
    {
        printf("\nFile Open Successfull.......\n");
    }
    while(1)
    {
        if(send_file(fp2,string,NET_BUF_SIZE)){
            send(sockfd,string,NET_BUF_SIZE,sendrecvflag);
            clearBuf(string);

            break;
        }
        if(fp2!=NULL)
        {
            fclose(fp2);
        }
    }

}

//push download

void push_download_file(int sockfd,char *net_buf)
{
    FILE *fp;
    int nBytes;
        printf("\nWaiting for file name....\n");
        clearBuf(net_buf);
        nBytes = recv(sockfd,net_buf,NET_BUF_SIZE,sendrecvflag);

        char base_address[100] = "./shared-storage/";
        //relative_address = net_buf;
        strcat(base_address,net_buf);
        //printf("Concatanated address : %s\n",base_address);
        fp = fopen(base_address,"r");
        printf("\nFile Name Received: %s\n",net_buf);

        if(fp ==NULL)
        {
            printf("\nFile open failed!!\n");
        }
        else
        {
            printf("\nFile Open Successfull!!\n");
        }

        while(1)
        {
            //count++;
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



