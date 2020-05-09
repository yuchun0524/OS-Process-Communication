#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include "com_app.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct nlmsghdr *nlh_recv = NULL;
struct iovec iov, iov_recv;
struct msghdr msg, msg_recv;
int netlink_socket;

int main(int argc, char *argv[])
{
    char request[40] = "Registration. id=";
    char type[20] = ", type=";
    char string[270];
    char *id = argv[1];
    char *blank = " ";
    size_t end;
    memset(string, 0, sizeof(string));
    strcat(request,argv[1]);
    strcat(type,argv[2]);
    strcat(request,type);
    netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(netlink_socket < 0)
    {
        printf("socket error\n");
        return -1;
    }
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    bind(netlink_socket, (struct sockaddr *)&src_addr,sizeof(src_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), request);
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name =(void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    //receive
    nlh_recv = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    iov_recv.iov_base = (void *)nlh_recv;
    iov_recv.iov_len = nlh->nlmsg_len;
    msg_recv.msg_iov = &iov_recv;
    msg_recv.msg_iovlen = 1;
    sendmsg(netlink_socket, &msg, 0);
    recvmsg(netlink_socket, &msg_recv, 0);
    printf("%s\n",(char *)NLMSG_DATA(nlh_recv));
    while(1)
    {
        if(strcmp((char *)NLMSG_DATA(nlh_recv), "Success")==0 || strcmp((char *)NLMSG_DATA(nlh_recv), "Fail ")==0)
        {
            fgets(string,sizeof(string),stdin);
            if(strcmp(string, "Recv\n")==0)
            {
                if(string[strlen(string)-1]=='\n')
                {
                    end = strlen(string);
                    --end;
                    string[end]='\0';
                    //string[strlen(string)-1]='\0';
                }
                strcat(string, blank);
                strcat(string, id);
            }
            else if(strchr(string, '\n')==0)
            {
                while(fgetc(stdin)!='\n')
                    string[sizeof(string)-1] = '\n';
            }
            if(string[strlen(string)-1]=='\n')
            {
                end = strlen(string);
                --end;
                string[end]='\0';
            }
            strncpy(NLMSG_DATA(nlh), string, sizeof(string));
            sendmsg(netlink_socket, &msg, 0);
            recvmsg(netlink_socket, &msg_recv, 0);
            printf("%s\n", (char *)NLMSG_DATA(nlh_recv));
        }
        else if(strcmp((char *)NLMSG_DATA(nlh_recv), "Fail  ")==0)
            break;
        else
        {
            fgets(string,sizeof(string),stdin);
            if(strcmp(string, "Recv\n")==0)
            {
                if(string[strlen(string)-1]=='\n')
                {
                    end = strlen(string);
                    --end;
                    string[end]='\0';
                }
                strcat(string, blank);
                strcat(string, id);
            }
            else if(strchr(string, '\n')==0)
            {
                while(fgetc(stdin)!='\n')
                    string[sizeof(string)-1] = '\n';
            }
            if(string[strlen(string)-1]=='\n')
            {
                end = strlen(string);
                --end;
                string[end]='\0';
            }
            //strncpy(NLMSG_DATA(nlh), string, sizeof(string));
            snprintf(NLMSG_DATA(nlh), strlen(string)+1, "%s", string);
            sendmsg(netlink_socket, &msg, 0);
            recvmsg(netlink_socket, &msg_recv, 0);
            printf("%s\n", (char *)NLMSG_DATA(nlh_recv));
        }
    }
    free(nlh);
    close(netlink_socket);
    return 0;
}
