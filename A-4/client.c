//client 
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>    //def of struct for socket 
#include<netinet/in.h>    //for internet domain address
#include<string.h>  
#include<unistd.h>

#define PORT 8080
int main(int argc,char **argv)
{
	int len, sfd, cfd, n;
    struct sockaddr_in saddr, caddr;
    char buffer[1024], rbuffer[1024], dest[1024];

    //create a socket and get the socket fd
    sfd = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain,int type,int protocol)
    if(sfd<0)
        perror("Cannot create socket");

    //create server sockaddr
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);
    
    //connecting to the server
    connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
	
	//sending messages
    while(1)
	{
		printf("\nEnter the Message: ");
		fgets(buffer, 1024, stdin);

		n=write(sfd, buffer, sizeof(buffer));

		//break if bye is entered
		if(strcmp(buffer,"bye\n")==0)
			break;
		
		bzero(buffer, 1024);
		n=read(sfd, buffer, sizeof(buffer));
		if(strcmp(buffer, "bye\n")==0)
			break;
		printf("Server:%s", buffer);

	}
	//write
	
	close(sfd);
	return 0;
}

	