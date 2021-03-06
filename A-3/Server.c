#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define PORT 8080

int main(int argc, char **argv)
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
    printf("Waiting for client\n");
    listen(sfd, 2);

    //accept from client
    len = sizeof(caddr);
    cfd = accept(sfd, (struct sockaddr*)&caddr, &len); //accept the first client in the queue
    n = read(cfd, buffer, sizeof(buffer));

    //printing message from client
    printf("File to be transferred: %s", buffer);
    printf("\n");

    //sending the file contents back to client
    FILE *fp;
    fp = fopen(buffer, "r");
    fscanf(fp, "%[^\0]", content);
    fclose(fp);
    n = write(cfd, content, sizeof(content));

    printf("File Transferred");
    printf("\n");

    close(sfd);
    close(cfd);
    return 0;
}

/*
Output:
(base) Veeraraghavans-MacBook-Pro-2:A-3 veeraraghavan$ ./s
Waiting for client
File to be transferred: Direc/source.txt
File Transferred
*/
