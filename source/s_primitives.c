//
//  s_primitives.c
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 09/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#include "s_primitives.h"

void __drawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u32 color, S_FIFO* sgfifo)
{
    u16 args[5];
    
    args[0] = x1;
    args[1] = y1;
    args[2] = x2;
    args[3] = y2;
    args[4] = width;
    
    S_Primitive drawline = {S_DRAWLINE, args, 5, color};
    
    S_FIFONode* node = NULL;
    
    if ((node = (S_FIFONode*) malloc(sizeof(S_FIFONode))) == NULL) {
        printError("Out of memory.");
    }
    
    node->prim = drawline;
    node->next = NULL;
    //while (LWP_MutexLock(mtx)) ;
    (sgfifo->last)->next = node;
    sgfifo->last = node;
    //LWP_MutexUnlock(mtx);
}

void __drawBezier(u16 p0x, u16 p0y, u16 p1x, u16 p1y,
                u16 p2x, u16 p2y, u16 p3x, u16 p3y, u16 width, u32 color, S_FIFO* sgfifo)
{
    u16 args[9];
    
    args[0] = p0x;
    args[1] = p0y;
    args[2] = p1x;
    args[3] = p1y;
    args[4] = p2x;
    args[5] = p2y;
    args[6] = p3x;
    args[7] = p3y;
    args[8] = width;
    
    S_Primitive drawbezier = {S_DRAWBEZIER, args, 8, color};
    
    S_FIFONode* node = NULL;
    
    if ((node = (S_FIFONode*) malloc(sizeof(S_FIFONode))) == NULL) {
        printError("Out of memory.");
    }
    
    node->prim = drawbezier;
    node->next = NULL;
    (sgfifo->last)->next = node;
    sgfifo->last = node;
}