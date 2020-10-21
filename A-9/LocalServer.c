#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct ResourceRecord
{
    char domain_name[500];
    char matching_ip[500];
} ResourceRecord;

typedef struct DNSTable
{
    int noRecords;
    ResourceRecord entries[100];
} DNSTable;

void displayDNS(DNSTable *D)
{
    int i = 0;
    printf("SERVER NAME\t\tIP ADDRESS\n");
    for (i = 0; i < D->noRecords; i++)
    {
        printf("%s%20s\n", D->entries[i].domain_name, D->entries[i].matching_ip);
    }
}

void addDNS(char *domain_name, char *new_ip, DNSTable *D)
{
    strcpy(D->entries[D->noRecords].domain_name, domain_name);
    strcpy(D->entries[D->noRecords].matching_ip, new_ip);
    D->noRecords++;
}

void initDNS(DNSTable *D)
{
    D->noRecords = 3;
    strcpy(D->entries[0].domain_name, "www.yahoo.com");
    strcpy(D->entries[1].domain_name, "www.annauniv.edu");
    strcpy(D->entries[2].domain_name, "www.google.com");

    strcpy(D->entries[0].matching_ip, "10.2.45.67");
    strcpy(D->entries[1].matching_ip, "197.34.53.122");
    strcpy(D->entries[2].matching_ip, "142.89.78.66");
}

char *findIP(char *server, DNSTable *D)
{
    char error_message[100];
    strcpy(error_message, "The server name was not found!");
    for (int i = 0; i < D->noRecords; i++)
    {
        if (strcmp(server, D->entries[i].domain_name) == 0)
        {
            // printf("Found %s", DNS_TABLE[i].matching_ip);
            return (D->entries[i].matching_ip);
        }
    }
    return "The server name was not found!";
}

int check_valid_ip(char *new_ip)
{
    char *copy_ip = (char *)calloc(100, sizeof(char));
    strcpy(copy_ip, new_ip);
    char *token = strtok(copy_ip, ".");
    int count = 0;
    while (token != NULL)
    {
        if (atoi(token) < 0 || atoi(token) > 255)
        {
            return 0;
        }
        count++;
        token = strtok(NULL, ".");
    }
    if (count != 4)
    {
        return 0;
    }
    return 1;
}

void modify_table(DNSTable *D, char *new_domain_name, char *new_ip)
{
    int found = 0;
    for (int i = 0; i < D->noRecords; i++)
    {
        if (strcmp(new_domain_name, D->entries[i].domain_name) == 0) //domain name already exists
        {
            found = 1;
            if (strcmp(new_ip, D->entries[i].matching_ip) == 0) //entry in table has the same ip already
            {
                printf("IP address already exists\n");
                return;
            }
            else //new ip
            {
                strcpy(D->entries[i].matching_ip, new_ip);
                displayDNS(D);
                return;
            }
        }
    }
    if (found == 0) //new domain name
    {
        strcpy(D->entries[D->noRecords].domain_name, new_domain_name);
        strcpy(D->entries[D->noRecords].matching_ip, new_ip);
        D->noRecords++;
        displayDNS(D);
        return;
    }
}

int main(int argc, char **argv)
{
    int sockfd, ret;
    struct sockaddr_in servaddr, servaddr2;
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
    servaddr.sin_port = htons(atoi(argv[1])); //connects sd to port 7727
    servaddr.sin_addr.s_addr = INADDR_ANY;

    //initializing the DNS Table
    DNSTable *DNS_TABLE;
    DNS_TABLE = malloc(sizeof(DNSTable));
    DNS_TABLE->noRecords = 0;

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error");
        exit(1);
    }

    len = sizeof(cliaddr);
    char modify;
    char *matched_ip;
    char *new_ip = (char *)calloc(100, sizeof(char));
    char *new_domain_name = (char *)calloc(100, sizeof(char));

    char error_message[100];
    strcpy(error_message, "The server name was not found!");

    char *local = (char *)calloc(100, sizeof(char));
    char *auth = (char *)calloc(100, sizeof(char));
    while (1)
    {
        recvfrom(sockfd, buff, sizeof(buff), MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        printf("%s", buff);
        strcpy(local, findIP(buff, DNS_TABLE));
        if (strcmp(local, error_message) == 0)
        {
            printf("\nNot Found in Local DNS server\n");
            char buff2[1024];
            for (int i = 2; i < 5; i++)
            {
                char serverName[100];
                switch (i)
                {
                case 1:
                    strcpy(serverName, "Root");
                    break;
                case 2:
                    strcpy(serverName, "TLD");
                    break;
                case 3:
                    strcpy(serverName, "Authoritative");
                    break;
                }
                printf("Contacting the %s Server", serverName);
                int sockfd2;
                struct sockaddr_in servaddr2;
                sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
                if (sockfd2 < 0)
                    perror("cannot create socket");
                bzero(&servaddr2, sizeof(servaddr2));
                servaddr2.sin_family = AF_INET;
                servaddr2.sin_addr.s_addr = INADDR_ANY;
                servaddr2.sin_port = htons(atoi(argv[i])); //connects sd to port 7727
                strcpy(buff2, buff);
                sendto(sockfd2, buff2, sizeof(buff2), MSG_CONFIRM, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
                printf("waiting");
                recvfrom(sockfd2, buff2, sizeof(buff2), MSG_WAITALL, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
                printf("%s", buff2);
            }
            strcpy(buff, buff2);
        }
        else
        {
            printf("Found in local DNS\n");
            strcpy(buff, local);
        }
        sendto(sockfd, buff, sizeof(buff), MSG_CONFIRM, (struct sockaddr *)&cliaddr, len);
    }
}
