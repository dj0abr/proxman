/*
* Raspberry PI / Zero AddOn Board specially for Ham Radio Applications
* ====================================================================
* Author: DJ0ABR
*
*   (c) DJ0ABR
*   www.dj0abr.de
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
* 
main.cpp
===========

this is an exaple of the main file calling all other functions
do all initialisation here

*
*/

#include "kmclib.h"

void exit_program();
int read_ticket();
void prox_loop();

extern int keeprunning;

int wsconnected = 0;
extern int vmlistavail;
extern int vmshow;

void onmessage_func_handler(unsigned char *msg)
{
    wsconnected = 1;    // alive or other msg received
    //printf("websock msg rxed:<%s>\n",msg);
    // newly connected browser, send VM list
    if(strstr((char *)msg,(const char *)"newalive")) vmlistavail = 0;

    if(msg[0] == 'V' && msg[1] == 'M')
    {
        int vmid = atoi((const char *)(msg+2));
        //printf("vmid:%d\n",vmid);
        vmshow = vmid;
    }
}

void timerhandler()
{
    vmlistavail = 0;    // send new VM list to browser
}

int main(int argc, char *argv[])
{
    printf("proxman starting ...\npress Ctrl-C to exit\n");

    if(isRunning("proxman") == 1)
        exit(0);    // do not allow to run this program twice

    install_signal_handler(exit_program); // mainly used to catch Ctrl-C

    // init websocket connection to the browser
    ws_init(&keeprunning, onmessage_func_handler,40128);

    // read ticket from Proxmox and generate required cookies and tokens
    int r = read_ticket();
    if(r == 0)
    {
        printf("cannot read or process ticket\n");
        exit_program();
    }
    
    // timer to send VM list to browser every 2 seconds
    start_timer(2000, timerhandler);

    while(keeprunning)
    {
        prox_loop();
        usleep(100);    // make as long as possible for the application to reduce CPU load
    }
    
    exit_program();
    return 0;
}

void exit_program()
{
    printf("close program\n");
    // do cleanup
    keeprunning = 0;
    sleep(1);   // give threads a chance to exit
    //destroy_fifos();
    printf("exit program\n");
    exit(0);
}
