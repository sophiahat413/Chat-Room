//#include	"unp.h"
#define MAXLINE 4096
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
#include <sys/types.h> //bind
#include <unistd.h> //read
#include        "stdlib.h"
#include<stdio.h> //printf, fileno
#include<string.h> //memset,strcmp
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include <stdarg.h> //sprintf
#include<netdb.h> //hostent
#include<arpa/inet.h>
//host_name to ip
int 
hostname_to_ip(char *hostname , char *ip)
{
    int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;
 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
 
    if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
 
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        h = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip , inet_ntoa( h->sin_addr ) );
    }
     
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}
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
//max
int
max(int a, int b)
{
	if(a >= b)
		return a;
	else 
		return b;
}
//error_quit
void
err_quit(const char *message)
{
	printf("%s",message);
	exit(1);
}

void
str_cli(FILE *fp, int sockfd)
{
	int			maxfdp1, stdineof;
	fd_set		rset;
	char		buf[MAXLINE];
	char*           command;
	char c;
	int		n;
	int err,k = 0;
	stdineof = 0;
	FD_ZERO(&rset);
	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
			k=0;
			while(err = read(sockfd, &c,sizeof(c))){
                                buf[k] = c;
                                if(c == '\n'){
                                         buf[k+1] = 0;
					 break;
				}
                                k++;
                        }
			if (err == 0) {
				if (stdineof == 1)
					return;		/* normal termination */
				else
					err_quit("str_cli: server terminated prematurely");
			}
			else if(err < 0 )
				printf("read error.\n");
			else{
				printf("%s",buf);
				//Writen(fileno(stdout), buf,strlen(buf));
				fflush(stdout);
			}
		}
		//fp -> stdin
		if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
			k = 0;
			while(err = read(fileno(fp), &c,sizeof(c))){
                                buf[k] = c;
                                if(c == '\n'){
					buf[k+1] = 0;
                                        break;
				}		
                                k++;
                        }
			if ( err == 0) {
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);	/* send FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			else if(err < 0)
				printf("read error.\n");
			else{
				//printf("%s",buf);
				if(strcmp(buf,"exit\n") == 0)
					return ; //close the connection
				
				else
					writen(sockfd, buf, strlen(buf));
			}
		}
	}
 }
int  main(int argc, char **argv)
{
        int                                     sockfd;
        struct sockaddr_in      servaddr;

        if (argc != 3)
                err_quit("./client <SERVER IP> <SERVER PORT>");
		
	char *hostname = argv[1];
	char ip[100];
	hostname_to_ip(hostname,ip);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(atoi(argv[2]));
	//host name-> ip
        inet_pton(AF_INET, ip, &servaddr.sin_addr);

        connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

        str_cli(stdin, sockfd);         /* do it all */

        exit(0);
}

