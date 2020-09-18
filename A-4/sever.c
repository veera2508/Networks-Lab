//server
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
    char buffer[1024], content[1024];

    //create a socket and get the socket fd
    sfd = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain,int type,int protocol)
    if(sfd<0)
        perror("Cannot create socket");

    //create server sockaddr and bind
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);

    if(bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
        perror("Bind Error");
    
    //listen for client
    listen(sfd, 2);
	len = sizeof(caddr);

	while(1)
	{
		cfd = accept(sfd, (struct sockaddr*)&caddr, &len);
		int cp;
		if((cp = fork()) == 0)
		{
			int p_id=getpid();
			while((n=read(cfd, buffer, sizeof(buffer))>0))
			{
				if(strcmp(buffer, "bye\n")==0)
					break;
				if(n<0)
					break;
				printf("\nClient %d : ",getpid());
				printf("%s ",buffer);
				bzero(buffer,1024);
				printf("\nEnter the message: ");
				fgets(buffer, 1024, stdin);
		
				n=write(cfd, buffer, sizeof(buffer));
				if(strcmp(buffer, "bye\n")==0)
					break;
				printf("server: %s",buffer);
		
			}
			close(sfd);
			close(cfd);
			return 0;
		}
	}

}