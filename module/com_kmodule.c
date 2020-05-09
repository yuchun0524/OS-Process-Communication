#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include "com_kmodule.h"

#define NETLINK_USER 31
char id[1000]= {0};
EXPORT_SYMBOL(id);
struct sock *nl_sk = NULL;
struct nlmsghdr *nlh;
struct nlmsghdr *nlh_send;
struct sk_buff *skb_out, *skb_send;
struct mailbox mail[1000];
int i, pid, pid_send, msg_size, res, ret, len;
long real_id;
char id_string[5];
char *type = "";
char *middle = "";
char *message = "";
char start[15];
void *data = "";
char msg[256];
char *end = "";
char *blank = " ";
char *fail = "Fail\0";
char *success = "Success\0";
char *id_location = "";
char *allocate = "allocate memory error\0";

void strchrn(char *dest, const char *src, const char b)
{
    int len;
    char *remaining = strchr(src, b)+1;
    len = strlen(src)-strlen(remaining);
    strncpy(dest, src, len-1);
    dest[len-1] = '\0';
}

void strchrrn(char *dest, const char *src, const char b, const char e)
{
    int len;
    char *m = strchr(src, b) +1;
    char *n = strchr(src, e);
    len = strlen(m) - strlen(n);
    strncpy(dest, m, len);
    dest[len] = '\0';
}

static void recv_msg(struct sk_buff *skb)
{
    nlh = (struct nlmsghdr *)skb->data;
    nlh_send = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;
    pid_send = nlh_send->nlmsg_pid;
    data = nlmsg_data(nlh);
    strchrn(start, (char *)data, ' ');
    if(strcmp(start,"Send")==0)
    {
        middle = "";
        message = "";
        middle = strchr((char *)data, ' ')+1;
        message = strchr(middle, ' ')+1;
        len = strlen(middle)-strlen(message);
        strncpy(id_string, middle, len-1);
        id_string[len-1] = '\0';
        real_id = simple_strtol(id_string, NULL, 10);
        ret = strncmp(&id[real_id-1],"",1);
        if(ret == 0)
        {
            memset(msg, 0, sizeof(msg));
            strncpy(msg, fail, strlen(fail));
            strcat(msg, blank);
            msg[strlen(fail)+1] = '\0';
        }
        else
        {
            if(strlen(message)>255)
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, fail, strlen(fail));
                strcat(msg, blank);
                msg[strlen(fail)+1] = '\0';
            }
            else
            {
                if((strncmp(&id[real_id-1],"q",1)==0)&& mail[real_id-1].msg_data_count==3)
                {
                    memset(msg, 0, sizeof(msg));
                    strncpy(msg, fail, strlen(fail));
                    strcat(msg, blank);
                    msg[strlen(fail)+1] = '\0';
                }
                if(strncmp(&id[real_id-1],"u",1)==0)
                {
                    memset(msg, 0, sizeof(msg));
                    strncpy(msg, success, strlen(success));
                    msg[strlen(success)] = '\0';
                    mail[real_id-1].msg_data_count = 1;
                    memset(mail[real_id-1].msg_data_head->buf, 0, sizeof(mail[real_id-1].msg_data_head->buf));
                    strncpy(mail[real_id-1].msg_data_head->buf, message, strlen(message));
                    mail[real_id-1].msg_data_head->buf[strlen(message)] = '\0';
                }
                else if(strncmp(&id[real_id-1],"q",1)==0)
                {
                    if(mail[real_id-1].msg_data_count == 0)
                    {
                        mail[real_id-1].msg_data_head->next = kmalloc(sizeof(struct msg_data), GFP_KERNEL);
                        mail[real_id-1].msg_data_tail =  mail[real_id-1].msg_data_head->next;
                        memset(mail[real_id-1].msg_data_tail->buf, 0, sizeof(mail[real_id-1].msg_data_tail->buf));
                        memset(mail[real_id-1].msg_data_head->buf, 0, sizeof(mail[real_id-1].msg_data_head->buf));
                        strncpy(mail[real_id-1].msg_data_head->buf, message, strlen(message));
                        mail[real_id-1].msg_data_head->buf[strlen(message)] = '\0';
                        ++mail[real_id-1].msg_data_count;
                        memset(msg, 0, sizeof(msg));
                        strncpy(msg, success, strlen(success));
                        msg[strlen(success)] = '\0';
                    }
                    else if(mail[real_id-1].msg_data_count == 1)
                    {
                        memset(mail[real_id-1].msg_data_tail->buf, 0, sizeof(mail[real_id-1].msg_data_tail->buf));
                        strncpy(mail[real_id-1].msg_data_tail->buf, message, strlen(message));
                        mail[real_id-1].msg_data_tail->buf[strlen(message)] = '\0';
                        ++mail[real_id-1].msg_data_count;
                        mail[real_id-1].msg_data_tail->next = kmalloc(sizeof(struct msg_data), GFP_KERNEL);
                        mail[real_id-1].msg_data_tail =  mail[real_id-1].msg_data_tail->next;
                        memset(mail[real_id-1].msg_data_tail->buf, 0, sizeof(mail[real_id-1].msg_data_tail->buf));
                        memset(msg, 0, sizeof(msg));
                        strncpy(msg, success, strlen(success));
                        msg[strlen(success)] = '\0';
                    }
                    else if(mail[real_id-1].msg_data_count == 2)
                    {
                        memset(mail[real_id-1].msg_data_tail->buf, 0, sizeof(mail[real_id-1].msg_data_tail->buf));
                        strncpy(mail[real_id-1].msg_data_tail->buf, message, strlen(message));
                        mail[real_id-1].msg_data_tail->buf[strlen(message)] = '\0';
                        ++mail[real_id-1].msg_data_count;
                        memset(msg, 0, sizeof(msg));
                        strncpy(msg, success, strlen(success));
                        msg[strlen(success)] = '\0';
                    }
                    else if(mail[real_id-1].msg_data_count == 3)
                    {
                        memset(msg, 0, sizeof(msg));
                        strncpy(msg, fail, strlen(fail));
                        strcat(msg, blank);
                        msg[strlen(fail)+1] = '\0';
                    }
                }
            }
        }
    }
    else if(strcmp(start,"Recv")==0)
    {
        id_location = strchr((char *)data, ' ')+1;
        len = strlen((char*)data)-5;
        strncpy(id_string, id_location, len);
        id_string[len] = '\0';
        real_id = simple_strtol(id_string, NULL, 10);
        ret = strncmp(&id[real_id-1],"",1);
        if(ret == 0)
        {
            memset(msg, 0, sizeof(msg));
            strncpy(msg, fail, strlen(fail));
            strcat(msg, blank);
            msg[strlen(fail)+1] = '\0';
        }
        else
        {
            if((strncmp(&id[real_id-1],"q",1)==0) && mail[real_id-1].msg_data_count == 0 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, fail, strlen(fail));
                strcat(msg, blank);
                msg[strlen(fail)+1] = '\0';
            }
            else if((strncmp(&id[real_id-1],"u",1)==0) && mail[real_id-1].msg_data_count == 0 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, fail, strlen(fail));
                strcat(msg, blank);
                msg[strlen(fail)+1] = '\0';
            }
            else if((strncmp(&id[real_id-1],"u",1)==0) && mail[real_id-1].msg_data_count == 1 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, mail[real_id-1].msg_data_head->buf, strlen(mail[real_id-1].msg_data_head->buf));
                msg[strlen(mail[real_id-1].msg_data_head->buf)] = '\0';
            }
            else if((strncmp(&id[real_id-1],"q",1)==0) && mail[real_id-1].msg_data_count == 1 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, mail[real_id-1].msg_data_head->buf, strlen(mail[real_id-1].msg_data_head->buf));
                msg[strlen(mail[real_id-1].msg_data_head->buf)] = '\0';
                kfree(mail[real_id-1].msg_data_head);
                mail[real_id-1].msg_data_head = mail[real_id-1].msg_data_tail;
                --mail[real_id-1].msg_data_count;
            }
            else if((strncmp(&id[real_id-1],"q",1)==0) && mail[real_id-1].msg_data_count == 2 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, mail[real_id-1].msg_data_head->buf, strlen(mail[real_id-1].msg_data_head->buf));
                msg[strlen(mail[real_id-1].msg_data_head->buf)] = '\0';
                mail[real_id-1].msg_data_tail = mail[real_id-1].msg_data_head->next;
                kfree(mail[real_id-1].msg_data_head);
                mail[real_id-1].msg_data_head = mail[real_id-1].msg_data_tail;
                mail[real_id-1].msg_data_tail = mail[real_id-1].msg_data_tail->next;
                --mail[real_id-1].msg_data_count;
            }
            else if((strncmp(&id[real_id-1],"q",1)==0) && mail[real_id-1].msg_data_count == 3 )
            {
                memset(msg, 0, sizeof(msg));
                strncpy(msg, mail[real_id-1].msg_data_head->buf, strlen(mail[real_id-1].msg_data_head->buf));
                msg[strlen(mail[real_id-1].msg_data_head->buf)] = '\0';
                mail[real_id-1].msg_data_tail = mail[real_id-1].msg_data_head->next;
                kfree(mail[real_id-1].msg_data_head);
                mail[real_id-1].msg_data_head = mail[real_id-1].msg_data_tail;
                mail[real_id-1].msg_data_tail = mail[real_id-1].msg_data_tail->next;
                --mail[real_id-1].msg_data_count;
                mail[real_id-1].msg_data_tail->next = kmalloc(sizeof(struct msg_data), GFP_KERNEL);
                mail[real_id-1].msg_data_tail =  mail[real_id-1].msg_data_tail->next;
                memset(mail[real_id-1].msg_data_tail->buf, 0, sizeof(mail[real_id-1].msg_data_tail->buf));
            }
        }
    }
    else
    {
        strchrrn(id_string, (char *)data,'=', ',');
        type = strrchr((char *)data, '=')+1;
        real_id = simple_strtol(id_string, NULL, 10);
        ret = strncmp(&id[real_id-1],"",1);
        if(ret == 0)
        {
            memset(msg, 0, sizeof(msg));
            strncpy(msg, success, strlen(success));
            msg[strlen(success)] = '\0';
            if(strcmp(type,"queued")==0)
            {
                id[real_id-1]='q';
                mail[real_id-1].msg_data_count = 0;
                mail[real_id-1].msg_data_head = kmalloc(sizeof(struct msg_data), GFP_KERNEL);
                if(!mail[real_id-1].msg_data_head)
                {
                    memset(msg, 0, sizeof(msg));
                    strncpy(msg, allocate, strlen(allocate));
                    msg[strlen(allocate)] = '\0';
                }
                else
                    memset(mail[real_id-1].msg_data_head->buf, 0, sizeof(mail[real_id-1].msg_data_head->buf));
            }
            else if(strcmp(type,"unqueued")==0)
            {
                id[real_id-1]='u';
                mail[real_id-1].msg_data_count = 0;
                mail[real_id-1].msg_data_head = kmalloc(sizeof(struct msg_data), GFP_KERNEL);
                if(!mail[real_id-1].msg_data_head)
                {
                    memset(msg, 0, sizeof(msg));
                    strncpy(msg, allocate, strlen(allocate));
                    msg[strlen(allocate)] = '\0';
                }
                else
                    memset(mail[real_id-1].msg_data_head->buf, 0, sizeof(mail[real_id-1].msg_data_head->buf));
            }
        }
        else
        {
            memset(msg, 0, sizeof(msg));
            strncpy(msg, fail, strlen(fail));
            strcat(msg, blank);
            strcat(msg, blank);
            msg[strlen(fail)+2] = '\0';
        }
    }
    msg_size = strlen(msg);
    skb_send = nlmsg_new(msg_size, 0);
    if(!skb_send)
        return;
    nlh_send = nlmsg_put(skb_send, 0, 0, NLMSG_DONE, msg_size+1, 0);
    NETLINK_CB(skb_send).dst_group = 0;
    memset(nlmsg_data(nlh_send), 0, sizeof(nlmsg_data(nlh_send)));
    //strncpy(nlmsg_data(nlh_send), msg, sizeof(msg));
    snprintf(nlmsg_data(nlh_send), msg_size+1, "%s", msg);
    res = nlmsg_unicast(nl_sk, skb_send, pid);
    if(res < 0)
        printk("error while sending back to user.\n");
}
struct netlink_kernel_cfg cfg =
{
    .groups = 1,
    .input = recv_msg,
};
static int __init com_kmodule_init(void)
{
    printk(KERN_INFO "Enter module. Hello world!\n");
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if(!nl_sk)
    {
        printk("error while creating socket.\n");
        return -10;
    }
    return 0;
}

static void __exit com_kmodule_exit(void)
{
    printk(KERN_INFO "Exit module. Bye~\n");
    for(i = 0; i < 1000; i++)
    {
        kfree(mail[i].msg_data_head);
    }
    netlink_kernel_release(nl_sk);
}

module_init(com_kmodule_init);
module_exit(com_kmodule_exit);

MODULE_LICENSE("GPL");
