#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

    int sockfd, n;
    struct sockaddr_in servaddr;
    char str[1000];
    char buff[1024];
    char *domain_name;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        perror("cannot create socket");
    //erases the data in sizeof(servaddr) bytes starting from servaddr
    bzero(&servaddr, sizeof(servaddr));
    //uses the internet address family
    servaddr.sin_family = AF_INET;
    //connects to the IP address passed as argument
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(atoi(argv[1])); //connects sd to port 7727

    while (1)
    {
        printf("\n\nEnter the domain name: ");
        scanf("%s", domain_name);
        strcpy(buff,domain_name);

        sendto(sockfd, buff, sizeof(buff), MSG_, (struct sockaddr *)&servaddr, sizeof(servaddr));
        recvfrom(sockfd, buff, sizeof(buff), MSG_WAITALL, (struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("The IP address is: %s", buff);
    }
    close(sockfd);
}

