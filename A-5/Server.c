#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 8080

int main()
{
    int sockfd, ret;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    socklen_t addr_size;
    char buff[1024];
    int len;
    int n;
    char *message;
    time_t now;
    time(&now);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        perror("cannot create socket");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7229);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error");
        exit(1);
    }

    len = sizeof(cliaddr);
    char days[7][] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    char months[12][] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    while (1)
    {
        recvfrom(sockfd, buff, sizeof(buff), MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        printf("\nRequest from Client %d", ntohs(cliaddr.sin_port) % 100);
        if (strcmp(buff, "1") == 0)
        {
            struct tm *local = localtime(&now);
            snprintf(buff, 100, "%d/%d/%d", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
        }
        else if (strcmp(buff, "2") == 0)
        {
            struct tm *local = localtime(&now);
            int dw = local->tm_wday;
            sprintf(buff, days[dw]);
        }
        else if (strcmp(buff, "3") == 0)
        {
            struct tm *local = localtime(&now);
            int monthNumber = local->tm_mon;
            sprintf(buff, months[monthNumber]);
        }
        else if (strcmp(buff, "4") == 0)
        {
            struct tm *local = localtime(&now);
            snprintf(buff, 100, "%d", local->tm_year + 1900);
        }
        else if (strcmp(buff, "5") == 0)
        {
            time_t rawtime;
            struct tm *timeinfo;

            time(&rawtime);
            timeinfo = localtime(&rawtime);
            snprintf(buff, 100, " %d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        }
        else if (strcmp(buff, "6") == 0)
        {
            time_t rawtime;
            struct tm *timeinfo;
            struct tm *gtime;
            time_t now;
            time_t toronto;
            time(&now);
            timeinfo = gmtime(&now);
            timeinfo->tm_hour -= 4;
            toronto = mktime(timeinfo);
            timeinfo = localtime(&toronto);
            snprintf(buff, 100, "%d/%d/%d  %d:%d:%d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        }
        else
        {
            strcpy(buff, "Please check the option!");
        }
        printf("\nMessage sent to Client\n");
        sendto(sockfd, buff, sizeof(buff), MSG_CONFIRM, (struct sockaddr *)&cliaddr, len);
    }
}

