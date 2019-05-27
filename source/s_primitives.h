//
//  s_primitives.h
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 09/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#ifndef __simplewiigraphics__s_primitives__
#define __simplewiigraphics__s_primitives__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <unistd.h>
#include <ogc/lwp.h>
#include "s_errors.h"

// Graphical primitive type macros
#define S_NOPRIMITIVE 0
#define S_DRAWLINE 1
#define S_DRAWBEZIER 2
#define S_DRAWRECTANGLE 3
#define S_DRAWELLIPSE 4
#define S_FILLRECTANGLE 5
#define S_FILLELLIPSE 6

// Graphical primitive struct
typedef struct __sprim {
    u8 type;        // Primitive's type
    u16* params;    // Parameters
    u8 paramsz;     // How many parameters
    u32 color;      // Color (in ARGB)
} S_Primitive;

// FIFO node
typedef struct __snode {
    S_Primitive prim;
    struct __snode* next;
} S_FIFONode;

// FIFO with insertion in constant time & mutex for multithreading
typedef struct {
    S_FIFONode* first;
    S_FIFONode* last;
    mutex_t mtx;
} S_FIFO;


void __drawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u32 color, S_FIFO* sgfifo);
void __drawBezier(u16 p0x, u16 p0y, u16 p1x, u16 p1y,
                u16 p2x, u16 p2y, u16 p3x, u16 p3y, u16 width, u32 color, S_FIFO* sgfifo);

#endif /* defined(__simplewiigraphics__s_primitives__) */
