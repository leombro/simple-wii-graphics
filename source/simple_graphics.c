//
//  simple_graphics.c
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 08/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#include "simple_graphics.h"

void* framebuffer[2] = {NULL, NULL}; // External framebuffers
u32 fb; // Framebuffer index
void* gx_fifo = NULL;
GXRModeObj* rmode = NULL;
S_FIFO* sgfifo;
extern mutex_t mtx;
extern u8 end;

GXColor rgb2GXC (u32 color) {
    /*u8 r1, g1, b1, a1;
    a1 = (color << 24) % 256;
    r1 = (color << 16) % 256;
    g1 = (color << 8) % 256;
    b1 = color % 256;
    GXColor result;
    result.a = a1;
    result.r = r1;
    result.g = g1;
    result.b = b1;
    return result;*/
    return (GXColor) {255, 255, 255, 0xFF};
}

void* Bresenham(void* params)
{
    u32* args = (u32*)params;
    u16 x1 = args[0],
        y1 = args[1],
        x2 = args[2],
        y2 = args[3];
    u32 color = args[4];
    u16 i;
    
    if (x1 == x2) {
        printf("deh\n");
        for (i = y1; i < y2; i++) {
            drawpixel(x1, i, rgb2GXC(color));
        }
    } else if (y1 == y2) {
        printf("doh\n");
        for (i = x1; i < x2; i++) {
            drawpixel(i, y1, rgb2GXC(color));
        }
    } else {
        printf("deh-doh\n");
        u16 dx = x2 - x1, dy = y2 - y1; // Distance between the two points
        u16 sx, sy;
        u16 x = x1, y = y1;
        u8 swapped = 0;
        u16 d;
        
        // Take note of the relative position of the points, and then
        // use the abs of the distance
        
        if (dx > 0)
            sx = 1;
        else {
            sx = -1;
            dx = -dx;
        }
        if (dy > 0)
            sy = -1;
        else {
            sy = -1;
            dy = -dy;
        }
        
        // Take the shortcut, and if |dy| > |dx| (m > 1) rotate the
        // cartesian axis by 90 degrees
        
        if (dy > dx) {
            u16 tmp = dx;
            swapped = 1;
            dx = dy;
            dy = tmp;
        }
        
        d = 2 * dy - dx;
        drawpixel(x, y, rgb2GXC(color));
        drawpixel(x2, y2, rgb2GXC(color));
        
        while (x != x2) {
            if (d > 0) {    // paint the "bottom" pixel
                x = x + sx;
                y = y + sy;
                d = d + 2 * (dy - dx);
            } else {
                if (swapped) {
                    y = y + sy;
                } else {
                    x = x + sx;
                }
                d = d + 2 * dy;
            }
            drawpixel(x, y, rgb2GXC(color));
        }
    }
    return NULL;
}

void drawFunc(int firstfr)
{
    S_FIFONode* curr = NULL;
    lwp_t draw;
    u32* lineparam = NULL;
    
    //while (LWP_MutexLock(mtx)) ;
    curr = sgfifo->first;
    while (curr != NULL) {
        u16* args = (curr->prim).params;
        switch ((curr->prim).type) {
            case S_DRAWLINE:
                lineparam = (u32*)malloc(5*sizeof(u32));
                lineparam[0] = args[0];
                lineparam[1] = args[1];
                lineparam[2] = args[2];
                lineparam[3] = args[3];
                lineparam[4] = (curr->prim).color;
                //if (LWP_CreateThread(&draw, Bresenham, (void*) lineparam, NULL, 16*1024, 50) == 0)
                //    printf("brutto!\n");
                //LWP_JoinThread(draw, NULL);
                Bresenham(lineparam);
                free(lineparam);
                curr = curr->next;
                break;
                    
            default:
                curr = curr->next;
                break;
        }
            
    }
    
    //LWP_MutexUnlock(mtx);
    
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(framebuffer[fb],GX_TRUE);
    GX_DrawDone();
        
    VIDEO_SetNextFramebuffer(framebuffer[fb]);
    if (firstfr) {
        VIDEO_SetBlack(FALSE);
    }
    VIDEO_Flush();
        
    VIDEO_WaitVSync();
    
    fb ^= 1;		// flip framebuffer
    return NULL;
}

void FIFO_Init()
{
    S_FIFONode* emptynode = NULL;
    
    //LWP_MutexInit(&(mtx), false);
    
    if ((emptynode = (S_FIFONode*) malloc(sizeof(S_FIFONode))) == NULL) {
        printError("Cannot allocate memory for the SG FIFO");
    }
    
    S_Primitive emptyprim = {S_NOPRIMITIVE, NULL, 0, 0x00000000};
    emptynode->prim = emptyprim;
    emptynode->next = NULL;
    
    //LWP_MutexLock(mtx);
    if ((sgfifo = (S_FIFO*) malloc(sizeof(S_FIFO))) == NULL) {
        printError("Cannot allocate memory for the SG FIFO");
    }
    sgfifo->first = emptynode;
    sgfifo->last = emptynode;
    //LWP_MutexUnlock(mtx);
}


void SG_Init()
{
    f32 yscale; // Difference between EFB and XFB height
    u32 xfbHeight; // XFB height
    Mtx44 perspective; // 4x4 matrix for perspective (only for GX setup)
    u8 firstframe = 1; // Useful to actually draw the first frame
    lwp_t drawingthread; // drawing thread handle
    
    // Initialize the Wii Video subsystem
    VIDEO_Init();
    
    // Get the preferred video mode (from the Wii system settings)
    rmode = VIDEO_GetPreferredMode(NULL);
    
    // Initialize the graphical error handler
    ERROR_Init(rmode, NULL);
    
    // Allocate memory for the external framebuffers in non-cached memory
    framebuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    framebuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    
    if (framebuffer[0] == NULL || framebuffer[1] == NULL) {
        printError("Cannot allocate memory");
    }
    
    // fb is the "flipping" variable for double buffering
    fb = 0;
    
    // Configure the video output with the desired settings
    VIDEO_Configure(rmode);
    
    // Set one of the framebuffers as the current one...
    VIDEO_SetNextFramebuffer(framebuffer[fb]);
    
    //console_init(framebuffer[1],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
    
    // ...and flip the switch so we begin to render on the other one
    fb ^= 1;
    
    // Clear the framebuffers from garbage data
    VIDEO_ClearFrameBuffer (rmode, framebuffer[0], COLOR_BLACK);
    VIDEO_ClearFrameBuffer (rmode, framebuffer[1], COLOR_BLACK);
    
    // We don't want only a black screen
    VIDEO_SetBlack(FALSE);
    
    // Write the settings on the GPU registers
    VIDEO_Flush();
    
    // Wait the first V-Sync (the first two if the screen is interlaced)
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
    
    printf("\x1b[2;0H");
    
    // Allocate the GX subsystem FIFO
    gx_fifo = memalign(32, DEFAULT_FIFO_SIZE);
    if (gx_fifo == NULL) {
        printError("Cannot allocate FIFO memory");
    }
    memset(gx_fifo, 0, DEFAULT_FIFO_SIZE);
    
    // Init the GX subsystem
    GX_Init(gx_fifo,DEFAULT_FIFO_SIZE);
    // Set the background color to black and clear the GX z-buffer (that we won't use)
    GX_SetCopyClear((GXColor) {0, 0, 0, 0xFF}, 0x00FFFFFF);
    // Setup the default, fullscreen viewport
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    
    // Tell the GX subsystem about the difference between EFB's and XFB's heights
    yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    xfbHeight = GX_SetDispCopyYScale(yscale);
    
    // Set the default culling rectangle (we want the entire screen)
    GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
    
    // Set the values for the EFB -> XFB copy
    GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
    
    // Set the default filtering technique when copying from the EFB
    GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
    
    if (rmode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    
    // Set some things about 3D field rendering... Needed because the GX_Init func is broken
    GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
    
    // We don't want unnecessary culling, thank you very much
    GX_SetCullMode(GX_CULL_NONE);
    
    // Set the gamma when copying from EFB to XFB
    GX_SetDispCopyGamma(GX_GM_1_0);
    
    // Copy the current EFB in the (other, non-shown) XFB to get ready for the copy
    GX_CopyDisp(framebuffer[fb], GX_TRUE);

    // Setup an orthogonal 2D perspective matrix
    // The values are: top, bottom, left, right, near clipping plane, far clipping plane
    guOrtho(perspective,0,479,0,639,1.0,300.0);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
    // Init our FIFO
    FIFO_Init();
    
    // Do a drawing
    drawFunc(1);
    // done!
}


void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u32 color) {
    __drawLine(x1, y1, x2, y2, width, color, sgfifo);
    S_FIFONode* tmp = sgfifo->first;
    int i = 0;
    while (tmp != NULL) {
        i++;
        tmp = tmp->next;
    }
    Invalidate(i);
}

void DrawBezier(u16 p0x, u16 p0y, u16 p1x, u16 p1y,
                u16 p2x, u16 p2y, u16 p3x, u16 p3y, u16 width, u32 color) {
    __drawBezier(p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, width, color, sgfifo);
    Invalidate(0);
}

void Invalidate(int i) {
    drawFunc(0);
}


