/*
*File name : server.h*
*
Description : Header file containing all includes of the modules and commands as MACROS 
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

/*Modules
#include "./modules/authentication-request/AuthenticationRequest.h"
#include "./modules/common/Common.h"
#include "./modules/delete/Delete.h"
#include "./modules/download/Download.h"
#include "./modules/list-all-downloaded/ListAllDownloaded.h"
#include "./modules/list-all-drive-files/ListAllDriveFiles.h"
#include "./modules/upload/Upload.h"
#include "./modules/user-interface/UserInterface.h"
*/

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
#define IP_PROTOCOL 0
#define PORT_NO     4444
#define NET_BUF_SIZE 40960
#define cipherKey 'S'
#define sendrecvflag 0
#define EXIT "exit"
#define IP_ADDRESS  "127.0.0.1"
#define nofile "File_Not_Found"



//authentication_request

extern char permission[30];
extern int authentication_request(int sockfd,char *net_buf);


// common 

#include <stdio.h>
extern void clearBuf(char *b);
extern char Cipher(char ch);
extern int save_file(char *buf,int s,char *file_name);
extern int recv_file(char *buf,int s);
extern int file_found(char *buf,int s);
extern int send_file(FILE *fp,char *buf,int s);
extern void end_connection(int sockfd,char *net_buf);


//delete

extern void delete(int sockfd,char *net_buf);

//download

extern void download(int sockfd,char *net_buf,char *file_name);

//list all downloads

extern void list_all_downloaded();

//list_all_drive_files

extern void list_all_drive_files(int sockfd,char *net_buf);
//upload
extern void upload(int sockfd,char *net_buf);


//user choice
extern int user_choice(int sockfd,char *net_buf,char *file_name);


