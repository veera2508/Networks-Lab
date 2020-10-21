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
    time_t now;
    time(&now);

    //initializing the DNS Table
    DNSTable *DNS_TABLE;
    DNS_TABLE = malloc(sizeof(DNSTable));
    DNS_TABLE->noRecords = 0;

    if (strcmp(argv[2], "auth") == 0)
    {
        initDNS(DNS_TABLE);
        printf("Authoritative DNS Table\n");
        displayDNS(DNS_TABLE);
        char modify;
        char *matched_ip;
        char *new_ip = (char *)calloc(100, sizeof(char));
        char *new_domain_name = (char *)calloc(100, sizeof(char));

        while (1)
        {
            printf("Do you want to modify (y/n): ");
            scanf(" %c", &modify);
            if (modify == 'y')
            {

                printf("Domain Name: ");
                scanf("%s", new_domain_name);
                printf("Enter IP Address: ");
                scanf("%s", new_ip);
                while (check_valid_ip(new_ip) != 1)
                {
                    printf("Invalid IP\nEnter valid IP Address: ");
                    scanf("%s", new_ip);
                }
                modify_table(DNS_TABLE, new_domain_name, new_ip);
            }
            else
            {
                break;
            }
        }
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        perror("cannot create socket");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error");
        exit(1);
    }
    char error_message[100];
    strcpy(error_message, "The server name was not found!");
    while (1)
    {
        recvfrom(sockfd, buff, sizeof(buff), MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);

        if (strcmp(argv[2], "auth") == 0)
        {
            char *auth = (char *)calloc(100, sizeof(char));
            strcpy(auth, findIP(buff, DNS_TABLE));
            if (strcmp(auth, error_message) == 0)
            {
                strcpy(buff, error_message);
                printf("Not Found in Authoritative DNS server\n");
            }
            else
            {
                printf("\nFound in Authoritative DNS server\n");
            }
        }
        else if (strcmp(argv[2], "root") == 0)
        {
            printf("root yess");
            int len = strlen(buff);
            char new[3];
            new[0] = buff[len - 3];
            new[1] = buff[len - 2];
            new[2] = buff[len - 1];
            strcpy(buff, new);
            sendto(sockfd, buff, sizeof(buff), MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
            printf("%s", buff);
            break;
        }
        else if (strcmp(argv[2], "tld") == 0)
        {
            strcpy(buff, "Contact Authoritative Server");
            sendto(sockfd, buff, sizeof(buff), MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
        }
    }
}

