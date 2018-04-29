/*
 * Client.h
 *
 *  Created on: Nov 20, 2017
 *      Author: oriy
 */

#ifndef CLIENT_H_
#define CLIENT_H_
#include "ClientParser.h"

int MessageSend(int sockfd,char* ui);
int MessageRecv(int sockfd,char* si);
#endif /* CLIENT_H_ */
