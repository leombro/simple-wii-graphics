//
//  s_errors.c
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 08/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#include "s_errors.h"

static void* framebuf = NULL;
static u8 inited = 0;
static GXRModeObj* myrmode = NULL;
static mutex_t* mx;
u8 end = 0;
void ERROR_Init(GXRModeObj* rmode, mutex_t* mutex)
{
    // (Try to) Allocate the framebuffer
    framebuf = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    
    myrmode = rmode;
    
    if (framebuf == NULL) {
        exit(-1);
    }
    
    // Initialize the console
	console_init(framebuf,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
    
    mx = mutex;
    
    // Place the cursor
    printf("\x1b[2;0H");
    
    inited = 1;
    
}

void printError(char* string)
{
    end = 1;
    if (mx != NULL) LWP_MutexDestroy(*mx);
    if (inited) {
        // Set this framebuffer as the current one
        VIDEO_SetNextFramebuffer(framebuf);
        
        // Make the display visible
        VIDEO_SetBlack(FALSE);
        
        // Flush the video register changes to the hardware
        VIDEO_Flush();
        
        // Wait for Video setup to complete
        VIDEO_WaitVSync();
        if(myrmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
        
        printf(string);
        printf("\nExiting...");
        sleep(2);
        
    }
        exit(-1);
}