/*
 * socket_list.c
 *
 *  Created on: 11 בינו׳ 2018
 *      Author: Eden
 */
#include "socket_list.h"

#include "ClientParser.h"


user_list_node* create_list(int socket_fd,char* user_name)
{
	user_list_node* head=malloc(sizeof(user_list_node));
	head->socket_fd=socket_fd;
	head->user_name=malloc(NAME_MAX);
	head->user_name=user_name;
	head->next=NULL;
	head->msgbuffer=NULL;
	return head;
}

user_list_node* create_empty_list()
{
	user_list_node* head=malloc(sizeof(user_list_node));
	return head;
}

user_list_node* add_user(user_list_node* head,int socket_fd,char* user_name)
{
	user_list_node* temp=head;
	while(temp->next!=NULL)
	{
		temp=temp->next;
	}
	temp->next=malloc(sizeof(user_list_node));
	temp->next->socket_fd=socket_fd;
	//temp->next->user_name=malloc(NAME_MAX);
	temp->next->user_name=user_name;
	temp->next->next=NULL;
	temp->next->msgbuffer=NULL;
	return temp;
}

//user_list_node* set_user_name(user_list_node* head)

int delete_user(user_list_node* head,int socket_fd)
{
	if(head==NULL)
		return -1;
	user_list_node* temp=head;
	while(temp->next!=NULL && temp->next->socket_fd!=socket_fd)
	{
		temp=temp->next;
	}
	user_list_node* user_to_remove=temp->next;
	temp->next=user_to_remove->next;
	free(user_to_remove->user_name);
	free(user_to_remove);
	return 0;
}



user_list_node* find_user_by_sockfd(user_list_node* head,int sockfd)
{
	user_list_node* temp=head;
	while(temp!=NULL && temp->socket_fd!=sockfd)
	{
		temp=temp->next;
	}
	return head;
}


user_list_node* find_user_by_username(user_list_node* head,char* username)
{
	user_list_node* temp=head;
	while(temp!=NULL)
	{
		printf("temp->username=%s\n",temp->user_name);
		if(!strcmp(temp->user_name,username))
			break;
		temp=temp->next;
	}
	return temp;
}

user_list_node* get_ith_user(user_list_node* head,int i)
{
	user_list_node* temp=head;
	int j=0;
	while(temp!=NULL && j<i)
	{
		temp=temp->next;
		j++;
	}
	return temp;
}


user_list_node* copy_node(user_list_node* node)
{
	user_list_node* copy=malloc(sizeof(user_list_node));
	if(node==NULL)
		return NULL;
	copy->next=node->next;
	copy->socket_fd=node->socket_fd;
	copy->user_name=node->user_name;
	return copy;
}


int send_msg(user_list_node* reciever,char* msg)
{
	printf("reciever is %s\n",reciever->user_name);
	if(reciever->msgbuffer!=NULL)
	{
		printf("msg is %s\n",reciever->msgbuffer);
		return -1;
	}
	printf("msgbuffer is null\n");
	reciever->msgbuffer=malloc(message_length);
	printf("msgbuffer malloc'd\n");
	strcpy(reciever->msgbuffer,msg);
	printf("strcpy success in send_msg\n");
	return 0;
}

void read_msg(user_list_node* reciever,char* msgbuffer)
{
	strcpy(msgbuffer,reciever->msgbuffer);
	printf("strcpy sucess in read_msg\n");
	free(reciever->msgbuffer);
	printf("free success!\n");
	reciever->msgbuffer=NULL;
	printf("NULL'd\n");
}
