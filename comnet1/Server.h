/*
 * Server.h
 *
 *  Created on: 8 бреб„ 2017
 *      Author: Eden
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "ClientParser.h"
#include "socket_list.h"


typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

/*
 * Server initialization. returns 1 on success, 0 on failure
 */
int ServerInit(int sockfd,sockaddr_in server_addr,int port);


/*
 * This function handles message receiving - communication with the user
 * is done through this function. It uses HandleUserAction function
 * and handles INVALID,QUIT,ADD-type messages.  returns 1 on success, 0 on failure.
 */
int MessageRecieve(int sockfd,char* users_file,char* dir_path,char* user_name,user_list_node* user);

/*
 * This function handles a user action (except for ADD - which is handled in MessageRecieve function)
 * The result, if needed, is saved in 'buffer'. dir_path is the server directory which contains
 * user information., ui is a struct containing user info.
 * returns 1 on success, 0 on failure.
 */
int	HandleUserAction(char* path,char* buffer,char* ui,char* user_name);

//Assuming 'buffer' is a string containing a list of files, returns the number of files
int CountFiles(char* buffer);


#endif /* SERVER_H_ */
