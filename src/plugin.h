#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "m64p_config.h"
#include "m64p_plugin.h"
#include "m64p_types.h"

#define NUM_CONTROLLERS 4

typedef struct
{
    char host[256];
    int port;
    CONTROL *control; // pointer to CONTROL struct in Core library
    BUTTONS buttons;  // pointer to BUTTONS struct in Core library
} SController;

/* global data definitions */
extern SController controller[NUM_CONTROLLERS];

/* global function definitions */
extern void DebugMessage(int level, const char *message, ...);

/* declarations of pointers to Core config functions */
extern ptr_ConfigListSections     ConfigListSections;
extern ptr_ConfigOpenSection      ConfigOpenSection;
extern ptr_ConfigDeleteSection    ConfigDeleteSection;
extern ptr_ConfigListParameters   ConfigListParameters;
extern ptr_ConfigSetParameter     ConfigSetParameter;
extern ptr_ConfigGetParameter     ConfigGetParameter;
extern ptr_ConfigGetParameterHelp ConfigGetParameterHelp;
extern ptr_ConfigSetDefaultInt    ConfigSetDefaultInt;
extern ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat;
extern ptr_ConfigSetDefaultBool   ConfigSetDefaultBool;
extern ptr_ConfigSetDefaultString ConfigSetDefaultString;
extern ptr_ConfigGetParamInt      ConfigGetParamInt;
extern ptr_ConfigGetParamFloat    ConfigGetParamFloat;
extern ptr_ConfigGetParamBool     ConfigGetParamBool;
extern ptr_ConfigGetParamString   ConfigGetParamString;

extern ptr_ConfigGetSharedDataFilepath ConfigGetSharedDataFilepath;
extern ptr_ConfigGetUserConfigPath     ConfigGetUserConfigPath;
extern ptr_ConfigGetUserDataPath       ConfigGetUserDataPath;
extern ptr_ConfigGetUserCachePath      ConfigGetUserCachePath;

#endif // __PLUGIN_H__
