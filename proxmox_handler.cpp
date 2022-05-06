/*
*/

#include "kmclib.h"

void readVMlist();
void sendVMlist();
extern int wsconnected;

// =========================================
#define NODE_IP     "192.168.10.90"
#define NODE_NAME   "proxmoxnuc"
#define USER_NAME   "root@pam"
#define USER_PASS   "ivibauul"
#define NODE_PORT   8006
#define PROXY_PORT  3128
// =========================================

int vmshow = -1;
int vmlistavail = 0;
char url_query[100];
char cookie[2000];
char token[100];

#define MAXVMS 50
char vm_name[MAXVMS][100];
int vm_id[MAXVMS];
int vm_status[MAXVMS];
int vidx;


void build_urls(char const *cmd, int vmid)
{
    sprintf(url_query,"https://%s:%d/api2/json/nodes/%s/qemu",NODE_IP,NODE_PORT,NODE_NAME);

    if(vmid != -1)
        sprintf(url_query+strlen(url_query),"/%d/%s",vmid,cmd);

    
}

char scmd[10000];
char *run_command(char const *cmd, int vmid = -1)
{
    build_urls(cmd,vmid);
    if(vmid == -1)
        sprintf(scmd,"curl -sS --insecure --cookie %s --header %s -X GET \"%s\"",cookie,token,url_query);
    else
        sprintf(scmd,"curl -sS --insecure --cookie %s --header %s -X POST \"%s\"",cookie,token,url_query);

    //printf("run cmd:<%s> len:%d\n",scmd,strlen(scmd));
    runProgram(scmd, sizeof(scmd)-1);

    return scmd;
}

int isVMrunning(int vmid)
{
    for(int i=0; i<vidx; i++)
    {
        if(vm_id[i] == vmid) 
            return vm_status[i];
    }
    return 0;
}

void startVM(int vmid)
{
    run_command("status/start", vmid);
    sleep(2);
    readVMlist();
    sendVMlist();
}

void showVM(int vmid)
{
    if(!isVMrunning(vmid))
    {
        startVM(vmid);
    }

    // start viewer
    char s[500];
    sprintf(s,"./cv4pve-pepper --host=%s --username=%s --password=%s --vmid %d --viewer /usr/bin/remote-viewer --proxy=http://%s:3128 &",NODE_IP,USER_NAME,USER_PASS,vmid,NODE_IP);
    printf("run prg:<%s>\n",s);
    //runProgram(s, sizeof(s)-1);
    int r = system(s);
    if(r<0) r=0; // avoid compiler warning
}

// a ticket is required to access the proxmox functions
// read it from the node and store in "ticket"
// and extract the cookie and the token
int read_ticket()
{
char url_ticket[100];
char ticket[10000];
char ticketid[100];

    // read the ticket
    sprintf(url_ticket,"https://%s:%d/api2/json/access/ticket",NODE_IP,NODE_PORT);
    sprintf(ticket,"curl -sS --insecure --data \"username=%s&password=%s\" %s",USER_NAME, USER_PASS,url_ticket);

    runProgram(ticket, sizeof(ticket)-1);

    //printf("Ticket:\n<%s>\n",ticket);

    // extract the cookie ID
    const char *tidef = "\"ticket\":\"";
    char *hp = strstr(ticket,tidef);
    if(!hp) return 0;
    hp += strlen(tidef);
    char *hp1 = strchr(hp,'\"');
    if(!hp1) return 0;
    char csave = *hp1;
    *hp1 = 0;

    // hp has the cookieid, build complete cookie
    //printf("cookieid:\n<%s>\n",hp);
    sprintf(cookie,"PVEAuthCookie=%s",hp);
    //printf("cookie:\n<%s>\n",cookie);
    *hp1 = csave;

    // read the ticket ID
    char *hp2 = strstr(cookie,":");
    if(!hp2) return 0;
    char *hp3 = strstr(hp2+1,":");
    if(!hp3) return 0;
    hp3++;
    char *hp4 = strstr(hp3,":");
    if(!hp4) return 0;
    csave = *hp4;
    *hp4=0;
    strcpy(ticketid,hp3);
    //printf("ticketid:\n<%s>\n",ticketid);
    *hp4=csave;

    // read the token
    const char *todef = "\"CSRFPreventionToken\":\"";
    char *hp5 = strstr(ticket,todef);
    if(!hp5) return 0;
    hp5 += strlen(todef);
    char *hp6 = strchr(hp5,'\"');
    if(!hp6) return 0;
    *hp6 = 0;
    sprintf(token,"CSRFPreventionToken:%s",hp5);
    //printf("token:\n<%s>\n",token);

    return 1;
}

void readVMlist()
{
    // read list of all VMs
    run_command("");

    // read vmids
    char *hp = scmd;
    vidx = 0;
    while(1)
    {
        char *pv = strstr(hp,"\"vmid\":");
        if(!pv) break;
        pv+=7;
        char *pve = strchr(pv,',');
        if(!pve) pve = strchr(pv,'}');
        *pve = 0;
        char s[20];
        strcpy(s,pv);
        vm_id[vidx++] = atoi(s);
        *pve = ',';
        hp = pve;
    }
    // read status
    hp = scmd;
    vidx = 0;
    while(1)
    {
        char *pv = strstr(hp,"\"status\":");
        if(!pv) break;
        pv+=7;
        char *pve = strchr(pv,',');
        if(!pve) pve = strchr(pv,'}');
        *pve = 0;
        int st=1;
        if(!strstr(pv,"running")) st=0;
        vm_status[vidx++] = st;
        *pve = ',';
        hp = pve;
    }
    // read vnames
    hp = scmd;
    vidx = 0;
    while(1)
    {
        char *pv = strstr(hp,"\"name\":");
        if(!pv) break;
        pv+=7;
        char *pve = strchr(pv,',');
        if(!pve) pve = strchr(pv,'}');
        *pve = 0;
        strcpy(vm_name[vidx],pv+1);
        char *ap = strchr(vm_name[vidx],'\"');
        if(ap) *ap=0;
        vidx++;
        *pve = ',';
        hp = pve;
    }

    //for(int i=0; i<vidx; i++) printf("%d <%s> %d %s\n",vm_id[i],vm_name[i],vm_status[i],vm_status[i]?"ON":"off");
}

// send list to browser
void sendVMlist()
{
char s[10000];

    strcpy(s,"L");  // L ... VM list follows
    for(int i=0; i<vidx; i++)
    {
        sprintf(s+strlen(s),"% 3d %15.15s %s",vm_id[i],vm_name[i],vm_status[i]?"ON":"off");
        if(i < (vidx-1)) strcat(s,"~");
    }
    ws_send((unsigned char *)s,strlen(s));
}

void prox_loop()
{
    if(wsconnected == 1 && vmlistavail == 0) 
    {
        // read list of VMs
        readVMlist();
        
        // send list to browser
        sendVMlist();

        vmlistavail = 1;
    }

    if(vmshow != -1)
    {
        showVM(vmshow);
        vmshow = -1;
    }
}
