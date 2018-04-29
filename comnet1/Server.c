/*
 * Server.c
 *
 *  Created on: 8 ����� 2017
 *      Author: Eden
 */
#include "Server.h"

user_list_node* head; //We know this is STDIN. it will not be used anyway.

int main(int argc,char** argv)
{
	//-------------INTIALIZATION---------------
	if(argc<3 || argc>4) //only 2/3 arguments (excluding executable's name) will be accepted.
	{
		printf("Error: invalid number of parameters\n");
		return 0;
	}
	int sockfd=socket(AF_INET,SOCK_STREAM,0); //socket opening
	if(sockfd<0)  {
		printf("Error in system call:scoket fail\n");
			return 0;
	}
	sockaddr_in server_addr,client_addr;
	int port;
	if(argv[3]) port=atoi(argv[3]); //if port was supplied it is set to desired port
	else port=1337; //else it is set to default port
	if(ServerInit(sockfd,server_addr,port)==0)
	{
		printf("Error in system call:server init fail\n");
		return 0;
	}
	int users_file_fd=open(argv[1],O_RDONLY);
	char* dir_path=argv[2];
	DIR* dir=opendir(argv[2]);
	if(users_file_fd<1 || !dir) //validating args
	{
		perror("failed to open file");
		return 0;
	}
	char* username=malloc(NAME_MAX);
	char* user_dir_path=malloc(sizeof(dir_path)+2+NAME_MAX);
	char* user_msg_path=malloc(PATH_MAX+30);//reserved for message_recieved_offline.txt
	int i=0;
	//------------ CREATE DIRECTORY & MESSAGE FILE FOR EACH USER ------------------
	int file_length = lseek(users_file_fd,0,SEEK_END); //find length
	lseek(users_file_fd,-file_length,SEEK_END); //go back to the start
	char* users_file_info=malloc(MAX_CLIENTS*NAME_MAX*2);
	read(users_file_fd,users_file_info,file_length);
	username=strtok(users_file_info,"\t\n");
	while(username!=NULL)
	{
		if(i%2==0) //string split should return username and password interchangeably
		{
			sprintf(user_dir_path,"%s/%s",dir_path,username);
			if(mkdir(user_dir_path,0777)<0)
			{
						perror("Making dir failed.Error:");
						free(username);
						free(user_dir_path);
						free(user_msg_path);
						return 0;
			}
			sprintf(user_msg_path,"%s/%s",user_dir_path,"Messages_recieved_offline.txt");
			if(open(user_msg_path,O_CREAT|O_WRONLY|O_TRUNC,0777)<0)
			{
				free(username);
				free(user_dir_path);
				free(user_msg_path);
				perror("Making files failed. Error:");
				return 0;
			}
		}
		username=strtok(0,"\t\n");
		i++;

	}
	free(username);
	closedir(dir);
	close(users_file_fd);


	if (listen(sockfd,MAX_CLIENTS)==-1)
	{
		perror("listen fail. Error");
		return 0;
	}
	char* user_name=(char*) malloc(NAME_MAX);
	char* si = (char*) malloc(sizeof(char)*(message_length));   //success char + info file
	memset(si,0,message_length);
	socklen_t client_len;
	fd_set readfds;
	//fd_set writefds;
	head=create_list(0,"Server");
	FD_ZERO(&readfds);
	//FD_ZERO(&writefds);
	FD_SET(sockfd,&readfds);
	int maxfd=sockfd;


//---------------------SERVER-CLIENT COMMUNCATION---------------------
	user_list_node* user = create_empty_list();
	while(1)
	{
		//ACCEPT CLIENT
		int i;
		int client_sockfd;
		client_len=sizeof(client_addr);
		//printf("ok1\n");fflush(stdout);
		for(i=0;i<MAX_CLIENTS;i++)
		{
			user_list_node* temp=head;
			while(temp!=NULL)
			{
				if(temp->msgbuffer!=NULL)
				{
					printf("msg is:%s\n",temp->msgbuffer);
					strncpy(si,"1",1);
					char* msg=malloc(message_length-1);
					printf("before reading!\n");
					read_msg(temp,msg);
					printf("message read successfully!\n");
					strcpy(si_info,msg);
					printf("message copied!\n");
					free(msg);
					sendall(temp->socket_fd,si);
					printf("Message sent!\n");
				}
			temp=temp->next;
			}
			printf("before select!\n");
			//printf("ok2\n");fflush(stdout);
			select(maxfd+1,&readfds,NULL,NULL,NULL);
			//ACCEPT ALL INCOMING CALLS
			//printf("ok3\n");fflush(stdout);

			if(FD_ISSET(sockfd,&readfds))
			{
				client_sockfd= accept(sockfd,(sockaddr*) &client_addr,&client_len);
				if(client_sockfd==-1)
				{
					printf("%s","acception failed\n");
					break;
				}
				FD_SET(client_sockfd,&readfds);
				sprintf(si,"1");
				sendall(client_sockfd,si);
				add_user(head,client_sockfd,NULL);
				if(client_sockfd>maxfd)
					maxfd=client_sockfd;
			}
			//printf("ok4\n");fflush(stdout);
			//LOOK FOR USER REQUESTS
			if(head!=NULL)
				user=get_ith_user(head,i);
			//printf("ok5\n");fflush(stdout);
			if(user!=NULL)
			{
				int socket_fd=user->socket_fd;
				user_name=user->user_name;
				//printf("ok6\n");fflush(stdout);
				//printf("socket_fd=%d\n",socket_fd);
				//HANDLE USER REQUESTS
				if(FD_ISSET(socket_fd,&readfds))
				{
					//printf("ok7 serving user=%s\n", user_name);fflush(stdout);
					/*user=find_user_by_sockfd(head,socket_fd);
					if(user->msgbuffer!=NULL)
					{
						printf("HERE\n");
						strncpy(si,"1",1);
						strcpy(si,read_msg(user));
						sendall(socket_fd,si);
					}*/

					MessageRecieve(socket_fd,argv[1],dir_path,user_name,user);
					//printf("ok8\n");fflush(stdout);
				}
				/*if(FD_ISSET(socket_fd,&writefds))
				{
					user=find_user_by_sockfd(head,socket_fd);
					if(user->msgbuffer!=NULL)
					{
						printf("HERE\n");
						strncpy(si,"1",1);
						strcpy(si,read_msg(user));
						sendall(socket_fd,si);
					}
				}*/
			}
			//printf("ok9\n");
			FD_SET(sockfd,&readfds);
			for(int j=0;j<MAX_CLIENTS;j++){
			user=get_ith_user(head,j);
			if(user!=NULL)
			{
				FD_SET(user->socket_fd,&readfds);
				//FD_SET(user->socket_fd,&writefds);
				if(user->socket_fd>maxfd)
					maxfd=user->socket_fd;
			}
			//printf("ok10\n");fflush(stdout);
			}
		}
		free(user);
	}
	return 1;

}

int ServerInit(int sockfd,sockaddr_in server_addr,int port)
{
	bzero((char*) &server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(port);
	//memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));
	if(bind(sockfd,(struct sockaddr*)  &server_addr,sizeof(server_addr))<0)
	{
		perror("bind fail.Error");
		return 0;
	}
	return 1;

}

int CountFiles(char* buffer)
{
	int i=0;
	int count=0;
	while(i<strlen(buffer))
	{
		if(buffer[i]=='\n') count++;
		i++;
	}
	return count;
}

int HandleUserAction(char* path,char* buffer,char* ui,char* username)
{
	message_type msg=atoi(ui_type);//user message
	DIR* fd; //directory variable
	struct dirent* dp; //directory variable
	if(!buffer) return 0;
	int removed=404; //file not found :)
	int file_exist=404; //file not found :)
	char* tmp_path; //temp. path
	char* user_dir_path=(char*) malloc(PATH_MAX); //command
	int fd_get; //file descriptor for GET command
	int file_length;
	//char* msgs = malloc(200);
	if(sprintf(user_dir_path,"%s/%s",path,username)<0)
	{
		free(user_dir_path);
		return 0;
	}
	fd =opendir(user_dir_path);
	char* str = malloc(10);
	if(fd) //open user directory
	{
		while ((dp=readdir(fd))!=NULL) //for all files in user directory
		{
			if(!strcmp(dp->d_name,".")||!strcmp(dp->d_name,".."))
				continue; //ignore current and parent directory
			switch(msg)
			{
			case CONNECTION:
			case LIST:
				strcat(buffer,dp->d_name); //concat. buffer and current file name
				strcat(buffer,"\n");
				break;

			case DELETE:
				if(!strcmp(dp->d_name,ui_file_name))
				{
					removed=0; //file found
					tmp_path=malloc(PATH_MAX);
					sprintf(tmp_path,"%s/%s/%s",path,username,ui_file_name);
					if(unlink(tmp_path)==0)
					{
						removed=1; //file removed
					}
				}
				break;

			case GET:
				if(!strcmp(dp->d_name,ui_file_name))
				{
					file_exist = 1;
					tmp_path=malloc(PATH_MAX);
					sprintf(tmp_path,"%s/%s/%s",path,username,ui_file_name);
					fd_get=open(tmp_path,O_RDONLY); //open and read file into buffer
					if(fd_get==-1)
					{
						free(user_dir_path);
						free(dp);
						close(fd_get);
						closedir(fd);
						return 0;
					}
					file_length = lseek(fd_get,0,SEEK_END); //find length
					lseek(fd_get,-file_length,SEEK_END); //go back to the start
					read(fd_get,buffer,file_length);
					close(fd_get);
					free(tmp_path);
				}
				break;

			case READ_MSGS:

				tmp_path=malloc(PATH_MAX);
				sprintf(tmp_path,"%s/%s",user_dir_path,"Messages_recieved_offline.txt");
				printf("%s\n",tmp_path );fflush(stdout);
				if((fd_get=open(tmp_path,O_RDONLY))<0)
				{
					free(user_dir_path);
					free(tmp_path);
					perror("Making files failed. Error:");
					return 0;
				}
				file_length = lseek(fd_get,0,SEEK_END); //find length
				printf("file_length=%d\n",file_length);fflush(stdout);
				//close(fd_get);
				//fd_get=open(tmp_path,O_RDONLY);
				lseek(fd_get,-file_length,SEEK_END); //go back to the start
				read(fd_get,buffer,file_length-1);
				printf("BUFFER=%s\n",buffer);fflush(stdout);
				fgets(str, 10, stdin);
				//printf("str=%s\n",str);fflush(stdout);
				close(fd_get);
				fd_get=open(tmp_path,O_TRUNC); //overwrite existing file
				close(fd_get);
				free(tmp_path);
				break;

			case MSG:

				tmp_path=malloc(PATH_MAX);
				sprintf(tmp_path,"%s/%s",user_dir_path,"Messages_recieved_offline.txt");
				if((fd_get=open(tmp_path,O_WRONLY))<0)
				{
					free(user_dir_path);
					free(tmp_path);
					perror("Error sending message to user. Error:");
					return 0;
				}
				printf("ui_file=%s\n",ui_file);
				fgets(str,10,stdin);
				file_length = lseek(fd_get,0,SEEK_END);
				sprintf(buffer,"Message from %s:%s\n",ui_user_name,ui_file);
				write(fd_get,buffer,strlen(buffer));
				close(fd_get);
				free(tmp_path);
				printf("Handle user action done!\n");
				break;

			default:
				break;
			}
		}
		free(user_dir_path);
		free(dp);
		closedir(fd);
	}
	/*else if (atoi(ui_type)==CONNECTION)
	{
		if(mkdir(user_dir_path,0700)<0)
		{
			free(user_dir_path);
			return 0;
		}
	}*/
	if(atoi(ui_type)==DELETE){
		printf("entered this if?\n");
	 return removed;
	}
	if(atoi(ui_type)==GET){		
		printf("entered this if?\n");
	 return file_exist;
	}
return 1;

}

int MessageRecieve(int sockfd,char* users_file,char* dir_path,char* user_name,user_list_node* user)
{
	char* ui= (char*) malloc(sizeof(char)*(message_length));// type + user_name + password + file + path + file_name;
	memset(ui,0,message_length);
	recvall(sockfd,ui); //message recieved from client
	char* users_file_info;
	char* buffer="\0"; //buffer in which result will be written
	char* user_info; //temp. buffer containing user info
	char* si=(char*) malloc(sizeof(char)*(message_length));   //success char + info file
	memset(si,0,message_length);
	int x; //temp.
	char* path; //path to open
	int fd; //file descriptor
	int bytes_written; //checking write success
	message_type msg=atoi(ui_type);
	int users_file_fd;
	int file_length;
	int i;// used for indexing
	user_list_node* temp;
	char* query;
	printf("%s\n",ui_type);
	switch(msg)
	{
		case(INVALID):
//			printf("%s","Invalid\n");
			sprintf(si,"0");
			sendall(sockfd,si);//keep sending while failing
			free(si);
			free(ui);
			return 0;
		case(CONNECTION):
			users_file_info=(char*) malloc(MAX_CLIENTS*(NAME_MAX*2+2));//\t and \n for each user
			user_info=(char*) malloc(strlen(ui_user_name)+strlen(ui_password)+2);
			users_file_fd=open(users_file,O_RDONLY);
			if(users_file_fd<1)
			{
				sprintf(si,"0");
				sendall(sockfd,si);//keep sending while failing
				free(si);
				free(user_info);
				free(ui);
				free(users_file_info);
				return 0;
			}
		    file_length = lseek(users_file_fd,0,SEEK_END); //find length
		    lseek(users_file_fd,-file_length,SEEK_END); //go back to the start
			read(users_file_fd,users_file_info,file_length);
			close(users_file_fd);
			if(!users_file_info)
			{
				sprintf(si,"0");
				sendall(sockfd,si);//keep sending while failing
				free(si);
				free(user_info);
				free(ui);
				free(users_file_info);
				return 0;
			}
			sprintf(user_info,"%s",ui_user_name);
			user_info[strlen(ui_user_name)]='\0';
			strcat(user_info,"\t");
			strcat(user_info,ui_password);
			query=malloc(2*NAME_MAX);
			query=strtok(users_file_info,"\n");
			while(query!=NULL)
			{
				if(!strcmp(query,user_info))
					break;
				query=strtok(NULL,"\n");
			}
			if(!query)
			{
				sprintf(si,"0");
				sendall(sockfd,si);//keep sending while failing
				free(si);
				free(user_info);
				free(ui);
				free(users_file_info);
				return 0;
			}
			else
			{
				free(users_file_info);
				free(user_info);
				buffer=(char*) malloc (NAME_MAX*MAX_FILES_PER_CLIENT);
				memset(buffer,0,NAME_MAX*MAX_FILES_PER_CLIENT);
				HandleUserAction(dir_path,buffer,ui,ui_user_name);
				if(!buffer)
				{
					sprintf(si,"2");
					sendall(sockfd,si);//keep sending while failing
					free(si);
					free(user_info);
					free(ui);
					free(users_file_info);
					return 0;
				}
				user->user_name=malloc(NAME_MAX);
				strcpy(user->user_name,ui_user_name);
				sprintf(si,"1");
				sprintf(si+1,"Hi %s,you have %d files stored\n",ui_user_name,\
						CountFiles(buffer));
				sendall(sockfd,si);
				//free(user_info);
				//free(buffer);
							
			}
			break;

		case(LIST):
			buffer=(char*) malloc (NAME_MAX*MAX_FILES_PER_CLIENT);
			memset(buffer,0,NAME_MAX*MAX_FILES_PER_CLIENT);
			HandleUserAction(dir_path,buffer,ui,user_name);
			if(!buffer)
			{
				sprintf(si,"2");
				sendall(sockfd,si);
				free(si);
				free(ui);
				return 0;
			}
			sprintf(si,"1");
			sprintf(si_info,"%s",buffer);
			sendall(sockfd,si);
			break;

		case(DELETE):
			x=HandleUserAction(dir_path,buffer,ui,user_name);
			if(x==404) //not found
			{
				sprintf(si,"0");
				sendall(sockfd,si);
			}
			if(x==0) //server error
			{
				sprintf(si,"2");
				sendall(sockfd,si);
				free(si);
				free(ui);
				free(buffer);
				return 0;			
			}
			if(x==1) //found
			{
				sprintf(si,"1");
				sendall(sockfd,si);
			}
			break;

		case(GET):
			buffer=malloc(FILE_MAX);
			memset(buffer,0,FILE_MAX);
			x=HandleUserAction(dir_path,buffer,ui,user_name);
			if(x==404) //not found
			{
				sprintf(si,"0");
				sendall(sockfd,si);
			}			
			if(x==0) //server error
			{
				sprintf(si,"2");
				sendall(sockfd,si);
				free(si);
				free(ui);
				free(buffer);
				return 0;
			}

			if(x==1) //found
			{				
				//printf("BUFFER=%s\n",buffer);
				sprintf(si,"1");
				strcpy(si_info,buffer);
				sendall(sockfd,si);
			}
			break;

		case(ADD):
			path=(char*) malloc(PATH_MAX+NAME_MAX*2);
			sprintf(path,"%s/%s/%s",dir_path,user_name,ui_file_name);
			if(!path)
			{
				printf("sprintf error\n");
				sprintf(si,"2");
				sendall(sockfd,si);
				free(si);
				free(ui);
				free(path);
				return 0;
			}
			fd=open(path,O_CREAT|O_WRONLY,0777);
			if(fd==-1)
			{
				printf("file opening error\n");
				strncpy(si,"2",1); //snprintf(si,2, "2");
				sendall(sockfd,si);
				free(ui);
				free(si);
				free(path);
				return 0;
			}
			bytes_written=write(fd,ui_file,strlen(ui_file));
			close(fd);
			if(bytes_written==-1)
			{
				printf("write error:%s\n",strerror(errno));
				strncpy(si,"2",1);//snprintf(si,2,"2");
				sendall(sockfd,si);
				free(si);
				free(ui);
				free(path);
				return 0;
			}
			sprintf(si,"1");
			sendall(sockfd,si);
			break;

		case(USERS_ONLINE):
			i=1; //pass head
			user=get_ith_user(head,i);
			buffer=malloc(MAX_CLIENTS*(NAME_MAX+1)+1);
			strcpy(buffer,"");
			temp=user;
			while(temp!=NULL)
			{
				strcat(buffer,temp->user_name);
				if(temp->next!=NULL)
					strcat(buffer,",");
				else
					strcat(buffer,"\n");
				temp=temp->next;
			}
			strncpy(si,"1",1);//snprintf(si,1,"1");
			strcpy(si_info,buffer);
			sendall(sockfd,si);
			free(temp);
			//free(buffer);
			break;

		case(READ_MSGS):
			buffer=malloc(FILE_MAX);
			//memset(si_info,0,message_length-1);
			if(HandleUserAction(dir_path,buffer,ui,user_name)==1)
			{
				strncpy(si,"1",1);
				if(buffer)
					sprintf(si_info,"%s",buffer);

				else
					sprintf(si_info,"%s","You do not have any unread messages\n");

			}
			else
			{
				strncpy(si,"2",1);//snprintf(si,1,"2");
			}
			sendall(sockfd,si);
			break;

		case(MSG):
			buffer=malloc(NAME_MAX+message_length);
			temp=find_user_by_username(head,ui_file_name);
			if(temp)
			{
				printf("%s\n",temp->user_name);
				if(send_msg(temp,ui_file)==-1)
					{
						printf("error in sending message.\n");
						strncpy(si,"2",1);
					}
				printf("send_msg success\n");
				strncpy(si,"1",1);
			}
			else
			{
				if(HandleUserAction(dir_path,buffer,ui,ui_file_name)==1)
				{
					strncpy(si,"1",1);//snprintf(si,1,"1");
					memset(si_info,0,message_length-1);

				}
				else
				{
					strncpy(si,"2",1);//snprintf(si,1,"2");
				}
			}
			sendall(sockfd,si);
			break;

		case(QUIT):
			//free(buffer);
			delete_user(head,sockfd);
			free(ui);
			free(si);
			return -1;
			break;
		default:
			break;
	}
	free(buffer);
	free(si);
	free(ui);
	return 1;
}


