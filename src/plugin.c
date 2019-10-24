#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>

#include "m64p_config.h"

#include "plugin.h"
#include "version.h"
#include "controller.h"

#include "osal_dynamiclib.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8082

/* definitions of pointers to Core config functions */
ptr_ConfigOpenSection      ConfigOpenSection = NULL;
ptr_ConfigDeleteSection    ConfigDeleteSection = NULL;
ptr_ConfigListParameters   ConfigListParameters = NULL;
ptr_ConfigSetParameter     ConfigSetParameter = NULL;
ptr_ConfigGetParameter     ConfigGetParameter = NULL;
ptr_ConfigGetParameterHelp ConfigGetParameterHelp = NULL;
ptr_ConfigSetDefaultInt    ConfigSetDefaultInt = NULL;
ptr_ConfigSetDefaultFloat  ConfigSetDefaultFloat = NULL;
ptr_ConfigSetDefaultBool   ConfigSetDefaultBool = NULL;
ptr_ConfigSetDefaultString ConfigSetDefaultString = NULL;
ptr_ConfigGetParamInt      ConfigGetParamInt = NULL;
ptr_ConfigGetParamFloat    ConfigGetParamFloat = NULL;
ptr_ConfigGetParamBool     ConfigGetParamBool = NULL;
ptr_ConfigGetParamString   ConfigGetParamString = NULL;

ptr_ConfigGetSharedDataFilepath ConfigGetSharedDataFilepath = NULL;
ptr_ConfigGetUserConfigPath     ConfigGetUserConfigPath = NULL;
ptr_ConfigGetUserDataPath       ConfigGetUserDataPath = NULL;
ptr_ConfigGetUserCachePath      ConfigGetUserCachePath = NULL;

/* global data definitions */
SController controller[NUM_CONTROLLERS];

/* static data definitions */
static void (*l_DebugCallback)(void *, int, const char *) = NULL;
static void *l_DebugCallContext = NULL;
static int l_PluginInit = 0;

/* Global functions */
void DebugMessage(int level, const char *message, ...)
{
  char msgbuf[1024];
  va_list args;

  if (l_DebugCallback == NULL)
      return;

  va_start(args, message);
  vsprintf(msgbuf, message, args);

  (*l_DebugCallback)(l_DebugCallContext, level, msgbuf);

  va_end(args);
}

/* Mupen64Plus plugin functions */
EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context, void (*DebugCallback)(void *, int, const char *))
{
    if (l_PluginInit)
        return M64ERR_ALREADY_INIT;

    /* first thing is to set the callback function for debug info */
    l_DebugCallback = DebugCallback;
    l_DebugCallContext = Context;

    DebugMessage(M64MSG_INFO, "%s version %i.%i.%i startup.", PLUGIN_NAME, VERSION_PRINTF_SPLIT(PLUGIN_VERSION));

    /* Get the core config function pointers from the library handle */
    ConfigOpenSection = (ptr_ConfigOpenSection) osal_dynlib_getproc(CoreLibHandle, "ConfigOpenSection");
    ConfigDeleteSection = (ptr_ConfigDeleteSection) osal_dynlib_getproc(CoreLibHandle, "ConfigDeleteSection");
    ConfigListParameters = (ptr_ConfigListParameters) osal_dynlib_getproc(CoreLibHandle, "ConfigListParameters");
    ConfigSetParameter = (ptr_ConfigSetParameter) osal_dynlib_getproc(CoreLibHandle, "ConfigSetParameter");
    ConfigGetParameter = (ptr_ConfigGetParameter) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParameter");
    ConfigSetDefaultInt = (ptr_ConfigSetDefaultInt) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultInt");
    ConfigSetDefaultFloat = (ptr_ConfigSetDefaultFloat) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultFloat");
    ConfigSetDefaultBool = (ptr_ConfigSetDefaultBool) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultBool");
    ConfigSetDefaultString = (ptr_ConfigSetDefaultString) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultString");
    ConfigGetParamInt = (ptr_ConfigGetParamInt) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamInt");
    ConfigGetParamFloat = (ptr_ConfigGetParamFloat) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamFloat");
    ConfigGetParamBool = (ptr_ConfigGetParamBool) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamBool");
    ConfigGetParamString = (ptr_ConfigGetParamString) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamString");

    ConfigGetSharedDataFilepath = (ptr_ConfigGetSharedDataFilepath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetSharedDataFilepath");
    ConfigGetUserConfigPath = (ptr_ConfigGetUserConfigPath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserConfigPath");
    ConfigGetUserDataPath = (ptr_ConfigGetUserDataPath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserDataPath");
    ConfigGetUserCachePath = (ptr_ConfigGetUserCachePath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserCachePath");

    if (!ConfigOpenSection || !ConfigDeleteSection || !ConfigSetParameter || !ConfigGetParameter ||
        !ConfigSetDefaultInt || !ConfigSetDefaultFloat || !ConfigSetDefaultBool || !ConfigSetDefaultString ||
        !ConfigGetParamInt   || !ConfigGetParamFloat   || !ConfigGetParamBool   || !ConfigGetParamString ||
        !ConfigGetSharedDataFilepath || !ConfigGetUserConfigPath || !ConfigGetUserDataPath || !ConfigGetUserCachePath)
    {
        DebugMessage(M64MSG_ERROR, "Couldn't connect to Core configuration functions");
        return M64ERR_INCOMPATIBLE;
    }

    l_PluginInit = 1;
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
    if (!l_PluginInit)
        return M64ERR_NOT_INIT;

    l_PluginInit = 0;
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
    /* set version info */
    if (PluginType != NULL)
        *PluginType = M64PLUGIN_INPUT;

    if (PluginVersion != NULL)
        *PluginVersion = PLUGIN_VERSION;

    if (APIVersion != NULL)
        *APIVersion = INPUT_PLUGIN_API_VERSION;

    if (PluginNamePtr != NULL)
        *PluginNamePtr = PLUGIN_NAME;

    if (Capabilities != NULL)
        *Capabilities = 0;

    return M64ERR_SUCCESS;
}

/******************************************************************
  Function: InitiateControllers
  Purpose:  This function initialises how each of the controllers
            should be handled.
  input:    - The handle to the main window.
            - A controller structure that needs to be filled for
              the emulator to know how to handle each controller.
  output:   none
*******************************************************************/
EXPORT void CALL InitiateControllers(CONTROL_INFO ControlInfo)
{
    DebugMessage(M64MSG_INFO, "InitiateControllers");

    // reset controllers
    memset(controller, 0, NUM_CONTROLLERS * sizeof(SController));

    // set our CONTROL struct pointers to the array that was passed in to this function from the core
    // this small struct tells the core whether each controller is plugged in, and what type of pak is connected
    for (int i = 0; i < NUM_CONTROLLERS; i++) {
      // init controller
      controller[i].control = ControlInfo.Controls + i;
      controller[i].control->Plugin = PLUGIN_NONE;

      m64p_handle pConfig;

      /* Open the configuration section for this controller */
      char SectionName[32];
      sprintf(SectionName, "Input-Bot-Control%i", i); 
      if (ConfigOpenSection(SectionName, &pConfig) != M64ERR_SUCCESS) {
          DebugMessage(M64MSG_ERROR, "Couldn't open config section '%s'", SectionName);
          continue;
      }
      DebugMessage(M64MSG_INFO, "Reading %s", SectionName);

      /* throw warnings if 'plugged' is missing or can't be parsed */
      char plugged_str[256];
      if (ConfigGetParameter(pConfig, "plugged", M64TYPE_STRING, plugged_str, 256) == M64ERR_SUCCESS) {
          int result = -1;
          if (sscanf(plugged_str, "%d", &result) == 1) {
            controller[i].control->Present = result;
            DebugMessage(M64MSG_INFO, "    plugged=%d", controller[i].control->Present);
          } else {
            DebugMessage(M64MSG_WARNING, "Parsing error in 'plugged' parameter for controller %d; expected an integer, but got %s", i, plugged_str);
          }
      } else {
        if(i == 0) // Maintain previous behavior of always assuming the first controller is connected
          controller[i].control->Present = 1;
        else
          controller[i].control->Present = 0;
        DebugMessage(M64MSG_WARNING, "Missing 'plugged' parameter. Set to %d", controller[i].control->Present);
      }

      if (controller[i].control->Present) {
        /* throw warnings if 'host' is missing */
        char host_str[256];
        memset(controller[i].host, '\0', sizeof(controller[i].host));
        if (ConfigGetParameter(pConfig, "host", M64TYPE_STRING, host_str, 256) == M64ERR_SUCCESS) {
            strcpy(controller[i].host, host_str);
            DebugMessage(M64MSG_INFO, "    host=%s", controller[i].host);
        } else {
            strcpy(controller[i].host, DEFAULT_HOST);
            DebugMessage(M64MSG_WARNING, "Missing 'host' parameter. Setting to %s", controller[i].host);
        }

        /* throw warnings if 'port' is missing or can't be parsed */
        char port_str[256];
        if (ConfigGetParameter(pConfig, "port", M64TYPE_STRING, port_str, 256) == M64ERR_SUCCESS) {
            if(sscanf(port_str, "%d", &controller[i].port) == 1)
              DebugMessage(M64MSG_INFO, "    port=%d", controller[i].port);
            else
              DebugMessage(M64MSG_WARNING, "Parsing error in 'port' parameter for controller %d; expected an integer, but got %s", i, port_str);
        } else {
            controller[i].port = DEFAULT_PORT;
            DebugMessage(M64MSG_WARNING, "missing 'port' parameter. Set to %d", controller[i].port);
        }
      }
    }

    DebugMessage(M64MSG_INFO, "%s version %i.%i.%i initialized.", PLUGIN_NAME, VERSION_PRINTF_SPLIT(PLUGIN_VERSION));
}

/******************************************************************
  Function: ControllerCommand
  Purpose:  To process the raw data that has just been sent to a
            specific controller.
  input:    - Controller Number (0 to 3) and -1 signalling end of
              processing the pif ram.
            - Pointer of data to be processed.
  output:   none

  note:     This function is only needed if the DLL is allowing raw
            data, or the plugin is set to raw

            the data that is being processed looks like this:
            initilize controller: 01 03 00 FF FF FF
            read controller:      01 04 01 FF FF FF FF
*******************************************************************/
EXPORT void CALL ControllerCommand(int Control, unsigned char *Command)
{
}

/******************************************************************
  Function: ReadController
  Purpose:  To process the raw data in the pif ram that is about to
            be read.
  input:    - Controller Number (0 to 3) and -1 signalling end of
              processing the pif ram.
            - Pointer of data to be processed.
  output:   none
  note:     This function is only needed if the DLL is allowing raw
            data.
*******************************************************************/
EXPORT void CALL ReadController(int Control, unsigned char *Command)
{
}

/******************************************************************
  Function: RomOpen
  Purpose:  This function is called when a rom is open. (from the
            emulation thread)
  input:    none
  output:   none
*******************************************************************/
EXPORT int CALL RomOpen(void)
{
    return 1;
}

/******************************************************************
  Function: RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/
EXPORT void CALL RomClosed(void)
{
}

/******************************************************************
  Function: GetKeys
  Purpose:  To get the current state of the controllers buttons.
  input:    - Controller Number (0 to 3)
            - A pointer to a BUTTONS structure to be filled with
            the controller state.
  output:   none
*******************************************************************/
EXPORT void CALL GetKeys( int Control, BUTTONS *Keys )
{
    read_controller(Control);

    #ifdef _DEBUG
      DebugMessage(M64MSG_VERBOSE, "Controller #%d value: 0x%8.8X", 0, *(int *)&controller[Control].buttons );
    #endif

    *Keys = controller[Control].buttons;
}

/******************************************************************
  Function: SDL_KeyDown
  Purpose:  To pass the SDL_KeyDown message from the emulator to the
            plugin.
  input:    keymod and keysym of the SDL_KEYDOWN message.
  output:   none
*******************************************************************/
EXPORT void CALL SDL_KeyDown(int keymod, int keysym)
{
}

/******************************************************************
  Function: SDL_KeyUp
  Purpose:  To pass the SDL_KeyUp message from the emulator to the
            plugin.
  input:    keymod and keysym of the SDL_KEYUP message.
  output:   none
*******************************************************************/
EXPORT void CALL SDL_KeyUp(int keymod, int keysym)
{
}
