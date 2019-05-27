//
//  s_errors.h
//  simplewiigraphics
//
//  Created by Orlando Leombruni on 08/03/15.
//  Copyright (c) 2015 ium. All rights reserved.
//

#ifndef __simplewiigraphics__s_errors__
#define __simplewiigraphics__s_errors__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>

void ERROR_Init(GXRModeObj* rmode, mutex_t* mutex);
void printError(char* string);

#endif /* defined(__simplewiigraphics__s_errors__) */
