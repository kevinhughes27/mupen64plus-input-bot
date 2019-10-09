#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "m64p_plugin.h"
#include "m64p_types.h"

#define NUM_CONTROLLERS 4

typedef struct
{
    CONTROL *control; // pointer to CONTROL struct in Core library
    BUTTONS buttons;  // pointer to BUTTONS struct in Core library
} SController;

/* global data definitions */
extern SController controller[NUM_CONTROLLERS];

/* global function definitions */
extern void DebugMessage(int level, const char *message, ...);

#endif // __PLUGIN_H__
