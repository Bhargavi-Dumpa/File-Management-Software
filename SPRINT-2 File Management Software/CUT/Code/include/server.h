/*
*
*File name : server.h
*Description : Header file containing all includes of the modules and commands as MACROS 
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <pthread.h>

/*Commands*/
#define END_CONNECTION "End_Connection"
#define LIST_DIR "List_Directory"
#define PUSH_DOWNLOAD_FILE "Download"
#define PUSH_UPLOAD_FILE "Upload"
#define DELETE "Delete"
#define AUTH_REQUEST "Auth"
#define CMD_SUCCESS "done"
#define CMD_NOT_SUCCESS "notdone"


#define FILE_SIZE 100
#define IP_ADDRESS "127.0.0.1"
#define IP_PROTOCOL 0
#define PORT_NO 4444
#define NET_BUF_SIZE 40960
#define cipherKey 'S'
#define sendrecvflag 0
#define FILE_NAME_SIZE 100
#define nofile "File_Not_Found"
#define EOL "\n"


extern void push_download_file(int sockfd,char *net_buf);

extern void list_all(int sockfd,char *net_buf);

typedef struct connection_data
{
    int clientSocket;
    char *net_buf;
    struct sockaddr_in serverAddr;
    int addr_size;

}connection_data_t;


extern void *handle_connection(void *new_socket_details);


extern void get_user_file(int sockfd,char *net_buf);


extern void delete_file(int sockfd,char *net_buf);


#include <stdio.h>
extern void clearBuf(char *b);
extern char Cipher(char ch);
extern int send_file(FILE *fp,char *buf,int s);
extern int recv_file_save(char *buf,int s,char *file_name);

extern void authentication(int sockfd,char *net_buf);


