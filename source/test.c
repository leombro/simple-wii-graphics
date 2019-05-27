//
//  test.c
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 09/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#include <stdio.h>
#include <wiiuse/wpad.h>
#include "simple_graphics.h"

extern S_FIFO* sgfifo;

int main( int argc, char **argv )
{
    SG_Init();
    WPAD_Init();
    DrawLine(200, 200, 300, 300, 1, 0x00FF0000);
    DrawLine(200, 200, 200, 300, 1, 0x0000FF00);
    DrawLine(200, 200, 300, 200, 1, 0x000000FF);
    
    /*GXRModeObj* rmode = VIDEO_GetPreferredMode(NULL);
    void* framebufe = NULL;
    framebufe = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(framebufe,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);*/
    
    while(1) {
        
        WPAD_ScanPads();
        
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_1) printError("lol.gif");
        /*if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
            VIDEO_SetBlack(FALSE);
            VIDEO_SetNextFramebuffer(framebufe);
            VIDEO_Flush();
            
            VIDEO_WaitVSync();

            S_FIFONode* tmp = sgfifo->first;
            while (tmp != NULL) {
                printf("un elemento della coda\n");
                tmp = tmp->next;
            }
            sleep(10);
            exit(0);
        }*/
    
    }
}