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

    //getting path from user
    printf("Enter the path of file: ");
    scanf("%s", buffer);
    
    //sending path to server
    n = write(sfd, buffer, sizeof(buffer));

    //reading contents from server
    n = read(sfd, rbuffer, sizeof(rbuffer));

    //transferring to the location
    printf("File Transferred\n");
    printf("Save the file in path: ");
    scanf("%s", dest);

    //saving the file
    FILE *fp;
    fp = fopen(dest, "w");
    fprintf(fp, "%s", rbuffer);
    fclose(fp);

    close(sfd);
    return 0;
}