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
#include "Context.hh"
#include "Shader.hh"

#define ip_server "192.168.1.3"
#define port_server 18194  

int	main()
{
  SceCtrlData pad;
  
  sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
  debugNetInit((char*)ip_server, port_server, DEBUG);

  Context ctx;

  sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
  
  memset(&pad, 0, sizeof(pad));

  if (!ctx.init())
    {
      debugNetPrintf(ERROR, (char*)"Error while initializing context!\nExiting...");
      debugNetFinish();
      sceKernelExitProcess(0);
      return 0;
    }

  // Test read file

  SceUID fd = sceIoOpen("app0:test.txt", SCE_O_RDONLY, 0777);
  if (fd < 0)
    debugNetPrintf(ERROR, (char*)"Error while opening file!\nExiting...");
  else
    {
      char buff[100];
      std::stringstream toto;

      bzero(buff, 100);
      while (sceIoRead(fd, buff, 100) > 0)
	{
	  toto << buff;
	  bzero(buff, 100);
	}
      
      debugNetPrintf(ERROR, (char*)"%s", toto.str().c_str());
    }
  
  int fps = 0;
  while (1)
    {
      sceCtrlPeekBufferPositive(0, &pad, 1);
      
      if (pad.buttons & SCE_CTRL_START)
	break;
      
      //      ctx.update(pad);
      ctx.startDrawing();
      ctx.endDrawing();
      ctx.swapBuffers();

      /*
      fps++;
      if (fps >= 60)
	{
	  debugNetPrintf(ERROR, (char*)"%d %d %d %d!\n", pad.lx, pad.ly, pad.rx, pad.ry);
	  fps = 0;
	}
      */
    }
  

  ctx.release();
  
  debugNetFinish();
  sceKernelExitProcess(0);
  return 0;
}
