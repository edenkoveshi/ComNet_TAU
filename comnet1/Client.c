/*
 * client.c
 *
 *  Created on: Nov 19, 2017
 *      Author: oriy
 */

#include "Client.h"

#define STDIN 0

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


int MessageSend(int sockfd,char* ui)
{
	if (sendall(sockfd,ui)== 1)
		return 1;
	else
		return -1;
}

int MessageRecv(int sockfd,char* si){
	if (recvall(sockfd,si) == 1)
		return 1;
	else
		return -1;
}

int main(int argc, char** argv){
	if(argc>3)
	{
		printf("Error: invalid number of parameters");fflush(stdout);
		return 0;
	}
	char* hostname = (char*) malloc (16);//size of ip address
	int port = 1337;
	if(argc>1)							// set not default port and host name
	{
		if(strlen(argv[1])>16)
		{
			printf("Invalid hostname,please try again\n");fflush(stdout);
			return 0;
		}	
		if(argv[1]) strcpy(hostname,argv[1]);
		if(argv[2] && !argv[1]) 
		{
			printf("Invalid parameters. please try again \n");fflush(stdout);
			return 0;
		}
		if(argv[2]) port=atoi(argv[2]);
		if(port==0)
		{
			printf("Invalid port. please try again \n");fflush(stdout);
			return 0;	
		}
	}
	if(argc==1)
	{
		hostname="127.0.0.1";
		port=1337;
	}
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)  {
		printf("Error in system call: %s\n",strerror(errno));fflush(stdout);
		return 0;
	}
	sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	int x=inet_pton(AF_INET,hostname,&(server_addr.sin_addr));
	if(x<=0)
	{
		if(x==0) printf("inet_pton error: Invalid hostname\n");
		if(x==1) printf("ient_pton error\n");
	}
	server_addr.sin_port=htons(port);
	memset(&server_addr.sin_zero,0,sizeof(server_addr.sin_zero));
	char* si = (char*) malloc(sizeof(char)*(message_length)); //success char + info file
	memset(si,0,1+message_length);
	socklen_t addr_size=sizeof(server_addr);
	int Connection = connect(sockfd, (struct sockaddr*) &server_addr, addr_size);
	if (Connection == -1){
		printf("connection Error in system call: %s\n",strerror(errno));fflush(stdout);
		close(sockfd);
		free(si);
		return 0;
	}
	MessageRecv(sockfd,si);
	switch(si[0]){
		case '1':														//connection have made
			printf("Welcome! Please log in.\n");fflush(stdout);
			break;
		case '0':
		case '2':
		default:
			printf("Server error\n");fflush(stdout);
			close(sockfd);
			free(si);
			return 0;
			break;
	}
	//struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	char strUserName[NAME_MAX];
	char strPassword[NAME_MAX];
	char* pos;
	char* ui = (char*) malloc(sizeof(char)*(message_length));   // type + user_name + password + file + path + file_name
	memset(ui,0,message_length);	
	while (1){				//connection
		fgets(strUserName, sizeof(strUserName), stdin);
		if (strUserName[0] == '\n'){						// e legal input
			strncpy(ui_type,"6",1);
			printf("invalid command. please enter user name and password\n");fflush(stdout);
			MessageSend(sockfd,ui);
			//free(ui);
			//close(sockfd);
			//return 1;
			continue;
		}
		if ((pos=strchr(strUserName, '\n')) != NULL)
			*pos = '\0';
		fgets(strPassword, sizeof(strPassword), stdin);
		if (strPassword[0] == '\n'){						// e legal input
			strncpy(ui_type,"6",1);
			printf("invalid command. please enter user name and password\n");fflush(stdout);
			MessageSend(sockfd,ui);
			//free(ui);
			//close(sockfd);
			//return 1;
			continue;
		}
		if ((pos=strchr(strPassword, '\n')) != NULL)
			*pos = '\0';
		ParserConnection(ui,strUserName,strPassword);
		if (strcmp(ui_type,"0")!=0){						// e legal input
			strncpy(ui_type,"6",1);
			printf("invalid command. please enter user name and password\n");fflush(stdout);
			//MessageSend(sockfd,ui);
			//free(ui);
			//free(si);
			//close(sockfd);
			//return 1;
			continue;
		}
		if (MessageSend(sockfd,ui) == -1){
			printf("send message error. program crashed. please reconnect\n");fflush(stdout);
			free(ui);
			free(si);
			close(sockfd);
			return 1;
			//continue;
		}
		//break;
	//}
	//while(1){
		while (MessageRecv(sockfd,si) == -1){
		};
		switch(si[0]){
			case '1':													// connection success
				printf("%s", si+1);fflush(stdout);
				break;
			case '0':													// invalid input
				printf("invalid user name or password. please enter user name and password\n");fflush(stdout);
				//close(sockfd);
				//free(ui);
				//free(si);
				//return 1;
				//continue;
				break;
			case '2':													// error
			default:
				printf("Server error.program crashed. please reconnect\n");fflush(stdout);
				close(sockfd);
				free(ui);
				free(si);
				return 1;
				break;
			}
		if (si[0]=='1')
			break;										// connection success. out of while
		else
			continue;									// connection refused. try again
	}//end of while(1) connection
	char str[PATH_MAX];
	char* path; //path to open
	int fd; //file descriptor
	int bytes_written; //checking write success
	//char* dir_path;	


//	printf("%s",ui_user_name);fflush(stdout);
	while (1){
		FD_SET(STDIN, &readfds);
		FD_SET(sockfd, &readfds);
		select(sockfd+1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(sockfd, &readfds)){
			MessageRecv(sockfd,si);
			switch(si[0]){
				case '1':
					printf("%s", si+1);fflush(stdout);
					break;
				case '0':
					printf("oops something went wrong\n");fflush(stdout);
					break;
				case '2':
				default:
					printf("Server error\n");fflush(stdout);
					close(sockfd);
					free(si);
					free(ui);
					return 0;
					break;
			}
		}
		if (FD_ISSET(STDIN, &readfds)){
			fgets(str, sizeof(str), stdin);
			if (str[0] == '\n'){
				printf("invalid command. please type again\n");fflush(stdout);
			continue;
			}
			if ((pos=strchr(str, '\n')) != NULL)
					*pos = '\0';
			ParserMassage(ui,str);
			switch (atoi(ui_type)){
				case 1:											// list of files
				case 7:											// users_online
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
//					printf("message recieved\n");fflush(stdout);
//					printf("original si_success: %s\n",si_success);fflush(stdout);
//					printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							printf("%s", si+1); fflush(stdout);
							break;
						case '0':
							printf("oops something went wrong. please send message again\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(si);
							free(ui);
							return 0;
							break;
					}
					break;
				case 9:											// read_msgs
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
//					printf("message recieved\n");fflush(stdout);
//					printf("original si_success: %s\n",si_success);fflush(stdout);
//					printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							printf("%s\n", si+1); fflush(stdout);
							break;
						case '0':
							printf("oops something went wrong. please send message again\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(si);
							free(ui);
							return 0;
							break;
					}
					break;
				case 2:												// delete file
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
					//printf("message recieved\n");fflush(stdout);
					//printf("original si_success: %s\n",si_success);fflush(stdout);
					//printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							printf("File removed.\n");fflush(stdout);
							break;
						case '0':
							printf("No such file exists!\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(si);
							free(ui);
							return 0;
							break;
					}
					break;
				case 3:													// add file
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
					//printf("message recieved\n");fflush(stdout);
					//printf("original si_success: %s\n",si_success);fflush(stdout);
					//printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							printf("File added.\n");fflush(stdout);
							//printf("file content: %s", ui_file);
							break;
						case '0':
							printf("oops something went wrong. please send message again\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(si);
							free(ui);
							return 0;
							break;
					}
					break;
				case 4:											// get file
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
					//printf("message recieved\n");fflush(stdout);
					//printf("original si_success: %s\n",si_success);fflush(stdout);
					//printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							path=(char*) malloc(PATH_MAX+NAME_MAX*2);
							sprintf(path,"%s/%s",ui_path,ui_file_name);
							if(!path)
							{
								printf("some error occured. please enter command again\n");fflush(stdout);
								free(path);
								break;
							}
							fd=open(path,O_CREAT|O_WRONLY,0777);
							if(fd==-1)
							{
								perror("file creating error:\n");
								close(sockfd);
								free(ui);
								free(si);
								free(path);
								return 0;
								break;
							}
							bytes_written=write(fd,si_info,strlen(si_info));
							close(fd);
							if(bytes_written==-1)
							{
								perror("Write Error");
								close(sockfd);
								free(si);
								free(ui);
								free(path);
								return 0;
								break;
							}
							chmod(path,S_IRUSR);
							break;
						case '0':
							printf("file doesn't exist\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(ui);
							free(si);
							return 0;
							break;
					}
					break;
				case 5:											// quit
					MessageSend(sockfd,ui);
					free(ui);
					free(si);
					close(sockfd);
					return 1;
					break;
				case 8:													// msg
					MessageSend(sockfd,ui);
					MessageRecv(sockfd,si);
					//printf("message recieved\n");fflush(stdout);
					//printf("original si_success: %s\n",si_success);fflush(stdout);
					//printf("original si_info: %s\n",si_info);fflush(stdout);
					switch(si[0]){
						case '1':
							//printf("DEBUG | msg recieved in the server\n");fflush(stdout);
							//printf("file content: %s", ui_file);fflush(stdout);
							printf("\n");fflush(stdout);
							break;
						case '0':
							printf("oops something went wrong. please send message again\n");fflush(stdout);
							break;
						case '2':
						default:
							printf("Server error\n");fflush(stdout);
							close(sockfd);
							free(si);
							free(ui);
							return 0;
							break;
					}
					break;
				default:
					printf("Invalid message. please send again\n"); fflush (stdout);
					break;
			}
		}//end of if stdin
	}
	return 0;
}



