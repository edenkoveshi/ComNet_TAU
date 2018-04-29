/*
 * ClientParser.c
 *
 *  Created on: Nov 19, 2017
 *      Author: oriy
 */



#include "ClientParser.h"

#include <stdio.h>
#include <string.h>


/**
 *
 *
 * this function parser a client connection command to a stream of chars
 *
 */
void ParserConnection(char* ui,char* strUserName,char* strPassword){
	if (!strUserName || !strPassword){
		strncpy(ui_type,"6",1);
		return;
	}
	char* delimeter = " ";
	char* strUserNameChunk;
	char copyUserName[NAME_MAX];
	char* strPasswordChunk;
	char copyPassword[NAME_MAX];
	strcpy(copyUserName, strUserName);
	strUserNameChunk=strtok(copyUserName,delimeter);
	if (strcmp(strUserNameChunk,"User:") == 0){
		strUserNameChunk = strtok(NULL,delimeter);
		if (!strUserNameChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strncpy(ui_user_name,strUserNameChunk,strlen(strUserNameChunk));
			strUserNameChunk = strtok(NULL,delimeter);
			if (strUserNameChunk){
				strncpy(ui_type,"6",1);
				return;
			}
		}
	}else{
		strncpy(ui_type,"6",1);
		return;
	} 
	strcpy(copyPassword, strPassword);
	strPasswordChunk = strtok(copyPassword,delimeter);
	if (strcmp(strPasswordChunk,"Password:") == 0){
		strPasswordChunk = strtok(NULL,delimeter);
		if (!strPasswordChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strncpy(ui_password,strPasswordChunk,strlen(strPasswordChunk));
			strPasswordChunk = strtok(NULL,delimeter);
			if (strPasswordChunk){
				strncpy(ui_type,"6",1);
				return;
			}
		}
	}else{
		strncpy(ui_type,"6",1);
		return;
	}
//	printf("ui_user_name:%s\n",ui_user_name);
//	printf("ui_password:%s\n",ui_password);
	strncpy(ui_type,"0",1);
	return;
}


/**
 *
 *
 * this function parser a client command to a stream of chars
 *
 */


void ParserMassage(char* ui, char* str){
	char* delimeter = " ";
	char* strChunk;
	char copy[2*FILE_MAX];
	strcpy(copy, str);
	strChunk = strtok(copy,delimeter);
	strncpy(ui_type,"6",1);
				
	if (strcmp(strChunk,"list_of_files") == 0){
		strChunk = strtok(NULL,delimeter);
		if (strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else	{
			strncpy(ui_type,"1",1);
			return;
		}
	}
	if (strcmp(strChunk,"delete_file") == 0){
		strChunk = strtok(NULL,delimeter);
		if (!strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strcpy(ui_file_name,strChunk);
			strChunk = strtok(NULL,delimeter);
			if (strChunk || (strcmp(strChunk,"Messages_recieved_offline.txt")==0) ){
				strncpy(ui_type,"6",1);
				return;
			} else {
				strncpy(ui_type,"2",1);
				return;
			}
		}
	}
	//int i;
	char path[PATH_MAX] ;
	if (strcmp(strChunk,"add_file") == 0){
		strChunk = strtok(NULL,delimeter);
		if (!strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strcpy(path,strChunk);				
			strcpy(ui_path,strChunk);
			strChunk = strtok(NULL,delimeter);
			if (!strChunk){
				strncpy(ui_type,"6",1);
				return;
			} else {
				strcpy(ui_file_name,strChunk);
				strChunk = strtok(NULL,delimeter);
				if (strChunk){
					strncpy(ui_type,"6",1);
					return;
				} else {
			//		printf("ui_path=%s\n",ui_path);
					//strcpy(ui_path,"\0");
					//path[i] = '\0';
					FILE* fp = fopen(path, "r");
					//printf("PATH=%s\n",path);
					if(!fp){
						printf("Error: File doesn't exist or cannot be opened\n"); fflush (stdout);
						strncpy(ui_type,"6",1);
						return;
					}  					
					fseek(fp,0,SEEK_END);//find end of file
					int length = ftell(fp);
					//printf("LENGTH=%d\n",length);
					fseek(fp,0,SEEK_SET); //return to start
					//fclose(fp);
					//fp = fopen(path, "r");
					if(length>FILE_MAX)
					{
						printf("Error: FILE SIZE IS ILLEGAL\n");
						strncpy(ui_type,"6",1);
						return;
					}
					fread(ui_file, sizeof(char), length, fp);
					if (!ui_file){
						printf("ERROR: File read\n"); fflush (stdout);
						fclose(fp);
						strncpy(ui_type,"6",1);
						return;
					}
					strncpy(ui_type,"3",1);
					return;
				}
			}
		}
	}
	if (strcmp(strChunk,"get_file") == 0){
		strChunk = strtok(NULL,delimeter);
		if (!strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strcpy(ui_file_name,strChunk);
			strChunk = strtok(NULL,delimeter);
			if (!strChunk){
				strncpy(ui_type,"6",1);
				return;
			} else {
				strcpy(ui_path,strChunk);
				strChunk = strtok(NULL,delimeter);
				if (strChunk){
					strncpy(ui_type,"6",1);
					return;
				} else {
					strncpy(ui_type,"4",1);
					return;
				}
			}
		}
	}
	if (strcmp(strChunk,"quit") == 0){
		strChunk = strtok(NULL,delimeter);
		if (!strChunk){
			strncpy(ui_type,"5",1);
			return;
		} else {
			strncpy(ui_type,"6",1);
			return;
		}
	}
	if (strcmp(strChunk,"users_online") == 0){
		strChunk = strtok(NULL,delimeter);
		if (strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else	{
			strncpy(ui_type,"7",1);
			return;
		}
	}
	if (strcmp(strChunk,"msg") == 0){
		strChunk = strtok(NULL,delimeter);
		if (!strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else {
			strncpy(ui_file_name,strChunk,strlen(strChunk));
			strChunk = strtok(NULL,delimeter);
			if (!strChunk){
				strncpy(ui_type,"6",1);
				return;
			} else {
				//printf("ok1");fflush(stdout);
				char* tmp = malloc(100);
				char* tmpMsg = malloc(FILE_MAX);
				strncpy(tmp,strChunk,strlen(strChunk));
				strncpy(tmpMsg,"",0);
				strcat(tmpMsg, tmp);
				strChunk = strtok(NULL,delimeter);
				//if (strChunk)
					//strcat(ui_file, " ");
				//printf("ok2");fflush(stdout);
				while (strChunk){
					strcat(tmpMsg, " ");
					strncpy(tmp,strChunk,100);
					strcat(tmpMsg, tmp);
					strChunk = strtok(NULL,delimeter);
				}
				strcpy(ui_file, tmpMsg);
				printf("msg sent=%s\n", ui_file);fflush(stdout);
				strncpy(ui_type,"8",1);
				return;
			}
		}
	}
	if (strcmp(strChunk,"read_msgs") == 0){
		strChunk = strtok(NULL,delimeter);
		if (strChunk){
			strncpy(ui_type,"6",1);
			return;
		} else	{
			strncpy(ui_type,"9",1);
			return;
		}
	}
	return;
}

//This is not a parsing operation, but it's mutual to both server and client
int sendall(int sockfd,char* message)
{
	int bytes_send=0;
	int bytes_left=message_length;
	int n=0;
	while(bytes_send<message_length)
	{
		n=send(sockfd,message+bytes_send,bytes_left,0);
		if(n==-1)
		{
			printf("Error in system call: %s\n",strerror(errno));
			free(message);
			return -1;
		}
		bytes_send+=n;
		bytes_left-=n;
	}
	return 1;
}

int recvall(int sockfd,char* message)
{
	int bytes_read=0;
	int bytes_left=message_length;
	int n=0;
	while(bytes_read<message_length)
	{
		n=recv(sockfd,message+bytes_read,bytes_left,0);
		if(n==-1)
		{
			printf("Error in system call: %s\n",strerror(errno));
			free(message);
			return 0;
		}
		bytes_read+=n;
		bytes_left-=n;
	}
	return 1;
}
