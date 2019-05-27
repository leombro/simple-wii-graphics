//
//  simple_graphics.h
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 08/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#ifndef __simplewiigraphics__simple_graphics__
#define __simplewiigraphics__simple_graphics__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <unistd.h>
#include <ogc/lwp.h>
#include "s_errors.h"
#include "s_primitives.h"
#define DEFAULT_FIFO_SIZE (256 * 1024)
#define drawpixel(x, y, c) GX_PokeARGB(x, y, c)

void SG_Init();
void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u32 color);
void DrawBezier(u16 p0x, u16 p0y, u16 p1x, u16 p1y,
                  u16 p2x, u16 p2y, u16 p3x, u16 p3y, u16 width, u32 color);
void Invalidate();
#endif /* defined(__simplewiigraphics__simple_graphics__) */
