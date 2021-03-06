#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define PORT 8080

typedef struct
{
    char s_ip[50];
    char s_mac[50];
    char d_ip[50];
    char d_mac[50];
    char data[50];
}arp_packet;

void print_arp_packet(arp_packet *p, int pdmac, int pdata)
{
    printf("%s|%s|%s", p->s_ip, p->s_mac, p->d_ip);
    if(pdmac == 1)  printf("|%s", p->d_mac);
    if(pdata == 1)  printf("|%s", p->data);
}

void pactostring(arp_packet *p, char *str, int pdmac, int pdata)
{
    sprintf(str, 150, "%s|%s|%s|%s", p->s_ip, p->s_mac, p->d_ip);
    if(pdmac == 1)  sprintf(str, 50, "|%s", p->d_mac);
    if(pdata == 1)  sprintf(str, 50, "|%s", p->data);
}

void stringtopac(arp_packet *p, char *str, int pdmac, int pdata)
{
    sscanf(str, 150, "%s|%s|%s", p->s_ip, p->s_mac, p->d_ip);
    if(pdmac == 1)  sscanf(str, 50, "|%s", p->d_mac);
    if(pdata == 1)  sscanf(str, 50, "|%s", p->data);
}

int main(int argc, char **argv)
{
    int len, sfd, cfd, n;
    struct sockaddr_in saddr, caddr;
    char buffer[1024], content[1024];
    char *c_ip, *c_mac, *sent_pac;
    pid_t child;
    arp_packet *p = malloc(sizeof(arp_packet));

    //create a socket and get the socket fd
    sfd = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain,int type,int protocol)
    if(sfd<0)
        perror("Cannot create socket");

    //create server sockaddr and bind
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);

    connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr));
    printf("Enter the IP Address: ");
    scanf("%s", c_ip);
    printf("\nEnter the MAC Address: ");
    scanf(" %s", c_mac);

    recv(sfd, buffer, sizeof(buffer), 0);
    printf("\nARP Request Received: ");
    stringtopac(p, buffer, 0, 0);
    print_arp_packet(p, 0, 0);

    if(strcmp(p->d_ip, c_ip) == 0)
    {
        printf("\nIP Address Match");
        strcpy(p->d_mac, c_mac);
        pactostring(p, buffer, 1, 0);
        strcpy(sent_pac, buffer);
        send(sfd, buffer, sizeof(buffer), 0);
        printf("\nARP Packet Sent:");
        print_arp_packet(p, 1, 0);
        while(strcmp(buffer, sent_pac) == 0) read(sfd, buffer, sizeof(buffer));
        printf("\nRecieved Packet is: ");
        pactostring(p, buffer, 1, 1);
        print_arp_packet(p, 1, 1);
    }

    else printf("\nIP doesn't match!");
    close(sfd);
    return 0;
}

