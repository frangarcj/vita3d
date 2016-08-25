#include <string.h>
#include <stdlib.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/types.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/message_dialog.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <sstream>
#include "Scene.hh"
#include "Context.hh"
#include "Shader.hh"
#include "ShaderFactory.hh"


#define ip_server "192.168.1.3"
#define port_server 18194  

int	main()
{
  sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
  debugNetInit((char*)ip_server, port_server, DEBUG);

  Context context;
  ShaderFactory shaderFactory;
  SceCtrlData pad;
  
  sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
  memset(&pad, 0, sizeof(pad));

  if (!context.init())
    {
      debugNetPrintf(ERROR, (char*)"Error while initializing context!\nExiting...");
      debugNetFinish();
      sceKernelExitProcess(0);
      return 0;
    }
  if (!shaderFactory.initialize())
    {
      context.release();
      debugNetFinish();
      sceKernelExitProcess(0);
      return 0;
    }

  Scene scene(context, shaderFactory);
  if (!scene.init())
    {
      shaderFactory.release();
      context.release();
  
      debugNetFinish();
      sceKernelExitProcess(0);
      return 0;
    }

  SceKernelSysClock sysClock;
  sceKernelGetProcessTime(&sysClock);
  float lastTime = (sysClock / 1000000.0f);
  while (1)
    {
      sceKernelGetProcessTime(&sysClock);
      float totalT = (sysClock / 1000000.0f);
      
      sceCtrlPeekBufferPositive(0, &pad, 1);
      
      if (pad.buttons & SCE_CTRL_START)
	break;

      
      scene.update(pad, totalT, totalT - lastTime);
      context.startDrawing();
      scene.draw();
      context.endDrawing();
      context.swapBuffers();

      lastTime = totalT;
    }

  scene.release();

 exit:
  shaderFactory.release();
  context.release();
  
  debugNetFinish();
  sceKernelExitProcess(0);
  return 0;
}
