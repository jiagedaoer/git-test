/*
 * main.c

 *
 *  Created on: 2019年6月8日
 *      Author: sice
 */
#include "unp.h"
#include	<limits.h>

int
main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, filefd,client[FD_SETSIZE];
	char				readbuf[MAXLINE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr("118.25.218.223");
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(8888);
	bzero(&(servaddr.sin_zero),8);
	int opt=1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr))==-1)
		perror("bind error\n");
	else {
		printf("socket bind success\n");
	}

	listen(listenfd, 5);

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
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (SA *) &cliaddr, &clilen))==-1)
			{
				perror("accept failue\n");
			}
			else
				printf("accept success\n");
			for (i = 0; i < FD_SETSIZE; i++)
							if (client[i] < 0) {
								client[i] = connfd;	/* save descriptor */
								break;
							}
						if (i == FD_SETSIZE)
							perror("too many clients");

						FD_SET(connfd, &allset);	/* add new descriptor to set */
						if (connfd > maxfd)
							maxfd = connfd;			/* for select */
						if (i > maxi)
							maxi = i;				/* max index in client[] array */

						if (--nready <= 0)
							continue;
	}
		for(i=0;i<=maxi;i++)
		{
			if ( (sockfd = client[i]) < 0)
							continue;
			if(FD_ISSET(connfd,&rset))
			{
				if ( (n = read(sockfd, buf, MAXLINE)) == 0)
				{
										/*4connection closed by client */
				close(sockfd);
				printf("connect close\n");
				FD_CLR(sockfd, &allset);
				client[i] = -1;
				}
				if(n>0)
				{
					printf("client:%s\n",buf);
				if((filefd=open("music_kugou.txt",O_RDONLY))==-1)
				{
					perror("file open:");
				}
				else
					printf("file open success\n");
				read(filefd,readbuf,MAXLINE);
				write(sockfd,readbuf,MAXLINE);
				printf("send ok\n");
				}
				if (--nready <= 0)
					break;
			}
			}
		}
}

