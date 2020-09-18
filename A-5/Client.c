#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char **argv)
{
    int len;
    int sfd, n;
    struct sockaddr_in saddr, caddr;
    char str[1000];
    char buffer[1024];
    char msg[100];
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd < 0)
        perror("cannot create socket");
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(argv[1]);
    saddr.sin_port = htons(PORT);

    int opt = 0;
    char con;

    while (opt != -1)
    {
        printf("1. Date\n2. Day\n3. Month\n4. Year\n5. Time\n6. Toronto\nEnter the choice:");
        scanf("%d", &opt);
        snprintf(buffer, 10, "%d", opt);
        sendto(sfd, buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr *)&saddr, sizeof(saddr));
        recvfrom(sfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *)&saddr, sizeof(saddr));
        switch(opt)
        {
	        case 1:
	            printf("Date: %s", buffer);
	            break;
	        case 2:
	            printf("Day: %s", buffer);
	            break;
	        case 3:
	            printf("Month: %s", buffer);
	            break;
	        case 4:
	            printf("Year: %s", buffer);
	            break;
	        case 5:
	            printf("Time: %s", buffer);
	            break;
	        case 6:
	            printf("Toronto Date and Time: %s", buffer);
	            break;
	        default:
	            printf("%s", buffer);
    	}

        printf("\nEnter y to continue and n to exit: ");
        scanf(" %c", &con);
        if (tolower(con) == 'n')
            opt = -1;
    }
    close(sfd);
}
