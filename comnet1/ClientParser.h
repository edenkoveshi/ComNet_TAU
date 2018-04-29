/*
 * ClientParser.h
 *
 *  Created on: 8 ����� 2017
 *      Author: Eden
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#ifndef CLIENTPARSER_H_
#define CLIENTPARSER_H_

#define MAX_CLIENTS 15
#define MAX_UID_PWD 25
#define FILE_MAX 512*4
#define NAME_MAX 25
#define PATH_MAX 512*4
#define MAX_FILES_PER_CLIENT 15
#define SOCK_LEN 3
#define MAX_MSG 1024*6

//these are our defines, a way to implement a struct as a string
#define ui_file ui														// in msg command this field will hold the The_message
#define ui_file_name ui+FILE_MAX+1										// in msg command this field will hold the user_name_we_send_to
#define ui_password ui+FILE_MAX+NAME_MAX+2
#define ui_path ui+FILE_MAX+2*NAME_MAX+3
#define ui_user_name ui+FILE_MAX+2*NAME_MAX+PATH_MAX+4					// in msg command this field will hold the user_name who sent the msg
#define ui_type ui+FILE_MAX+3*NAME_MAX+PATH_MAX+5

#define si_success si
#define si_info si+1

#define message_length 1+3*FILE_MAX+FILE_MAX+PATH_MAX+1+2+3+4+5

//these are defines for our own use, an implementation of a struct as a string

typedef enum message_type{
	CONNECTION,				//0
	LIST,
	DELETE,
	ADD,               		//3
	GET,
	QUIT,
	INVALID,					//6
	USERS_ONLINE,
	MSG,
	READ_MSGS					//9

} message_type;


void ParserConnection(char* ui,char* strUserName,char* strPassword);
void ParserMassage(char* ui, char* str);



/*
 * A modification of 'send' function, not allowing to read partial info
 */
int sendall(int sockfd,char* nessage);

/*
 * A modification of 'recv' function, not allowing to read partial info
 */
int recvall(int sockfd,char* message);
#endif /* CLIENTPARSER_H_ */
