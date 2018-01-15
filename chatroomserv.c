/* include fig01 */
//#include	"unp.h"
#define MAXLINE 4096
#define FD_SETSIZE 1024
#define LISTENQ 1024
#define SA struct sockaddr
#include<sys/types.h> //Socket
#include<sys/socket.h>
#include <strings.h> //bzero
#include <arpa/inet.h> //htons
#include <arpa/inet.h> //inet_pton
#include <sys/types.h> //connect
#include <sys/socket.h>
#include <sys/select.h> //FD_
#include <sys/time.h>
#include <sys/types.h>  //bind
#include <unistd.h> //read
#include<stdio.h> //printf, fileno
#include <stdarg.h> //sprintf
#include<string.h> //memset,strcmp
#include<errno.h>
#include<stdlib.h>
//writen
ssize_t
writen(int fd, const void *vptr, size_t n)
{
        size_t          nleft;
        ssize_t         nwritten;
        const char      *ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
                if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
                        if (nwritten < 0 && errno == EINTR)
                                nwritten = 0;           /* and call write() again */
                        else
                                return(-1);                     /* error */
                }

                nleft -= nwritten;
                ptr   += nwritten;
        }
        return(n);
}
//err_quit
void
err_quit(const char *message)
{
        printf("%s",message);
        exit(1);
}
//max
int
max(int a, int b)
{
        if(a >= b)
                return a;
        else
                return b;
}

int main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char hello[60],come[60],offline[60],cli_ip[40];
	char name[FD_SETSIZE][20];//save the name of the client
	bzero(name,sizeof(name));
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(5566);

       //	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_quit("bind error.");
	//listen(listenfd, LISTENQ);
	if(listen(listenfd, LISTENQ) < 0)
		err_quit("listen error.");

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		if((nready = select(maxfd+1, &rset, NULL, NULL, NULL))<0)
			err_quit("select error.");
                int q;
		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (SA *) &cliaddr, &clilen))<0)
				err_quit("accept error.");
#ifdef	NOTDEF
			if(inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL)<0)
				err_quit("inet_ntop error.");
			printf("new client: %s, port %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
#endif    
			sprintf(hello,"Hello, anonymous! From: %s/%d\n",inet_ntop(AF_INET, &cliaddr.sin_addr,cli_ip,sizeof(cli_ip)),ntohs(cliaddr.sin_port)); 
                        sprintf(come, "Someone is coming!\n"); 
			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;
					strcpy(name[i],"anonymous"); 				
	/* save descriptor */
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */
                        for(q = 0; q<=maxi; q++){
				if(client[q] > 0){
					if(client[q] != connfd)
						writen(client[q],come,strlen(come));
					else
						writen(connfd,hello,strlen(hello));
				}
			}
			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			//client want to close the connection
			if (FD_ISSET(sockfd, &rset)) {//if: FD_INSET
				//It does not want to read, it wants to end
			char c;
			int k = 0;
			int err;
			char others[60];
			char me[60];
			char* message;
			char* friend;
			char* user;
			char* new_name;
			char content[10000];
			//save a line data
			while(err = read(sockfd, &c,sizeof(c))){
				buf[k] = c ;
				if(c == '\n'){
					buf[k] = 0;
					break;
				}
				k++;
			}
			//EOF
			if ( err == 0) {
					/*4connection closed by client */
					sprintf(offline,"[Server] <%s> is offline.\n",name[i]);
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					for(q = 0; q<=maxi; q++){
						if(client[q] >0)
							writen(client[q],offline,strlen(offline));
					}
			}
				else if ( err < 0)
					printf("read error."); 
				else{//1:err>0
					char* command = strtok(buf," \n");
					
					if(!command) ;
					/*if(!command){
						sprintf(content,"[Server] ERROR: Error command.\n");
                                                writen(sockfd,content,strlen(content));
					}*/
					else{//else:command not null
					if(strcmp(command,"who") == 0){//if:who
						char* check;
                                        	check = strtok(NULL," \n");
                                        	if(check != NULL){
                                                	sprintf(content,"[Server] ERROR: Error command.\n");
                                                	writen(sockfd,content,strlen(content));
                                        	}
						else{
						for(q = 0; q<=maxi; q++){
                                               		 if(client[q] >0){
								//check if the client is the sender
								if(client[q] == sockfd){
									getpeername(sockfd,(SA *) &cliaddr, &clilen);
									sprintf(me,"<%s> <%s>/<%d> ->me\n",name[q],inet_ntop(AF_INET, &cliaddr.sin_addr,cli_ip,sizeof(cli_ip)),ntohs(cliaddr.sin_port));
									writen(client[q],me,strlen(me));
				
								}					
			        				else{
									getpeername(client[q],(SA *) &cliaddr, &clilen);
                                                                        sprintf(others,"<%s> <%s>/<%d> \n",name[q],inet_ntop(AF_INET, &cliaddr.sin_addr,cli_ip,sizeof(cli_ip)),ntohs(cliaddr.sin_port));
                                                                        writen(sockfd,others,strlen(others));
								}
							  }
						 }
						 } //else: not error cmd
				/*	char* check;
					check = strtok(NULL," \n");
					if(check != NULL){
						sprintf(content,"[Server] ERROR: Error command.\n");
                                                writen(sockfd,content,strlen(content));
					}*/
						
					}//if:who		
                                 	else if(strcmp(command,"yell")==0){//else:yell
						message =strtok(NULL,"\n");
						if(!message)
							sprintf(content,"%s  yell <>\n",name[i]);
						else
							sprintf(content,"%s yell <%s>\n",name[i],message);
						for( q = 0; q<=maxi; q++){
							if(client[q] >0)
								writen(client[q],content,strlen(content));
							}
					}//else:yell
					else if(strcmp(command,"tell") == 0){//else: tell
						friend = strtok(NULL," \n");
						message = strtok(NULL,"\n");
						//sender name is anonymous
						if(strcmp(name[i],"anonymous")==0){//sender is anonymous
							sprintf(content,"[Server] ERROR: You are anonymous.\n");
							writen(sockfd,content,strlen(content));
						}//sender is anonymous
						else{
						if(!friend){
                                                        sprintf(content,"[Server] ERROR: The receiver doesn't exist.\n");
                                                        writen(sockfd,content,strlen(content));
                                                }
						else{//friend is not null
                                                if( *(friend-1) == ' '){
							sprintf(content,"[Server] ERROR: The receiver doesn't exist.\n");
                                                        writen(sockfd,content,strlen(content));
						}
						else{//friend does not have space
						int w, find = 0;
                                                for(w=0; w<=maxi; w++){
                                                        if(client[w]>0){
								//find the receiver in the array
                                                                if(strcmp(friend,name[w]) == 0){
                                                                        find = 1;
									break;
								}
                                                        }
                                                }
                                                if(find == 0){
                                                        sprintf(content,"[Server] ERROR: The receiver doesn't exist.\n");
                                                        writen(sockfd,content,strlen(content));
                                                }
						//can find
						else{//can find
						//receiver's name is anonymous
						if(strcmp(friend,"anonymous")==0){
                                                        sprintf(content,"[Server] ERROR: The client to which you sent is anonymous.\n");
                                                        writen(sockfd,content,strlen(content));
                                                }
						else{//friend has name
						int w, Find = 0;
						for(w=0; w<=maxi; w++){
							if(client[w]>0){
								if(strcmp(friend,name[w]) == 0){
									 Find = 1;	
									 sprintf(content,"[Server] SUCCESS: Your message has been sent.\n");
                                                        		 writen(sockfd,content,strlen(content));
									 if(!message)
										sprintf(content,"[Server] <%s> tell you <>\n",name[i]);
									 else
									 	sprintf(content,"[Server] <%s> tell you <%s>\n",name[i],message);
                                                                         writen(client[w],content,strlen(content));
							
								}
							}
						}
						}//friend has name
						}//can find
						}//friend does not have space
						}//friend is nt null
						}//sender is anonymous
						//sprintf(content,"%s yell %s\n",name[i],message);
	     				}//else:tell
					else if(strcmp(command,"name") == 0){//name
						int used = 0, range = 0,english = 1;
						new_name = strtok(NULL,"\n");
						//test
						//sprintf(content,"name\n");
                                                //writen(sockfd,content,strlen(content));
	 					//
						if(!new_name){//new name is nul
							sprintf(content,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
                                                        writen(sockfd,content,strlen(content));
						}//new name is null
						else{//new name is not null
						if(strlen(new_name)<2 || strlen(new_name) >12){
                                                	sprintf(content,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
                                                        writen(sockfd,content,strlen(content));
                                                                }
						else{// 2<name<12
						if(strcmp(new_name,"anonymous")==0){
							sprintf(content,"[Server] ERROR: Username cannot be anonymous.\n");
                                                        writen(sockfd,content,strlen(content));
						}
						else {//new name is not anonymous
							for(q=0; q<=maxi; q++){
								if(strcmp(new_name,name[q]) == 0){
									 if(q != i){
									 sprintf(content,"[Server] ERROR: <%s> has been used by others.\n",new_name);
                                                       			 writen(sockfd,content,strlen(content));
									 used = 1;
								         break;
									 }
								}
							}
							if(used == 0){//not used
							/*	if(strlen(new_name)<2 || strlen(new_name) >12){
									sprintf(content,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
                                                        		writen(sockfd,content,strlen(content));
								}*/
							//	else{// 2-12
									for(q = 0; q<strlen(new_name);q++){
										if((new_name[q]>='a' && new_name[q]<='z')||(new_name[q]>='A'&&new_name[q]<='Z'));
										else{//not englisg
											sprintf(content,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
                                                                        		writen(sockfd,content,strlen(content));
											english = 0;
											break;
										}//not english					
									}		
							//	}//2-12
								if(english == 1){//is english
									sprintf(content,"You're now known as <%s>.\n",new_name);
                                                        		writen(sockfd,content,strlen(content));
									for(q = 0; q<=maxi; q++){
										if(client[q] > 0){
											if(client[q] != sockfd){
												sprintf(content,"[Server] <%s> is now known as <%s>.\n",name[i],new_name);
                                                                       			        writen(client[q],content,strlen(content));
											}
										}
									}
									strcpy(name[i],new_name);
								}//is english
							}//not used
						}//new name is not anonymous
						}// 2<name<12
						}//new name is not null
						}//name
						else{
							sprintf(content,"[Server] ERROR: Error command.\n");
							//printf("%s",content);
							//fflush(stdout);
                                                        writen(sockfd,content,strlen(content));
						}
						}//else:command not null

					
				}//1:err>0		
				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}//if:FD_INSET
		}
					
         }
}
/* end fig02 */
