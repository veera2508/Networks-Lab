#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define PORT 8081

int main(int argc, char **argv)
{
    int len, sfd, cfd, n;
    struct sockaddr_in saddr, caddr;
    char buffer[1024], str[1024];

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

    //accept from client
    len = sizeof(caddr);
    cfd = accept(sfd, (struct sockaddr*)&caddr, &len); //accept the first client in the queue
    n = read(cfd, buffer, sizeof(buffer));

    //printing message from client
    printf("Message from client: %s", buffer);
    printf("\n");

    //sending the same message back to client
    n = write(cfd, buffer, sizeof(buffer));

    //printing the sent message
    printf("Message sent: %s", buffer);
    printf("\n");

    close(sfd);
    close(cfd);
    return 0;
}

/*
Output:
(base) Veeraraghavans-MacBook-Pro-2:A-2 veeraraghavan$ ./s
Message from client: Hello from the Client
Message sent: Hello from the Client
*/