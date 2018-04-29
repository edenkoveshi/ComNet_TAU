/*
 * scoket_list.h
 *
 *  Created on: 11 בינו׳ 2018
 *      Author: Eden
 */

#ifndef SOCKET_LIST_H_
#define SOCKET_LIST_H_

typedef struct list_node
{
	int socket_fd;
	char* user_name;
	struct list_node* next;
	char* msgbuffer;
} user_list_node;

user_list_node* create_list(int socket_fd,char* user_name);

user_list_node* create_empty_list();

user_list_node* add_user(user_list_node* head,int socket_fd,char* user_name);

int delete_user(user_list_node* head,int socket_fd);

user_list_node* find_user_by_sockfd(user_list_node* head,int sockfd);

user_list_node* find_user_by_username(user_list_node* head,char* username);

user_list_node* get_ith_user(user_list_node* head,int i);

user_list_node* copy_node(user_list_node* node);

int send_msg(user_list_node*,char*);

void read_msg(user_list_node*,char*);

#endif /* SOCKET_LIST_H_ */
