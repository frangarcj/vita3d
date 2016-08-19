#pragma once

#include <psp2/gxm.h>
#include <psp2/types.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/ctrl.h>

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544
#define DISPLAY_STRIDE_IN_PIXELS 1024
#define DISPLAY_COLOR_FORMAT SCE_GXM_COLOR_FORMAT_A8B8G8R8
#define DISPLAY_PIXEL_FORMAT SCE_DISPLAY_PIXELFORMAT_A8B8G8R8
#define DISPLAY_BUFFER_COUNT 3
#define DISPLAY_MAX_PENDING_SWAPS 2
#define MSAA_MODE SCE_GXM_MULTISAMPLE_NONE
#define DEFAULT_TEMP_POOL_SIZE (1 * 1024 * 1024)

struct DisplayData
{
  void *address;
};

struct vec2
{
  float x;
  float y;
};

struct vec3
{
  float x;
  float y;
  float z;
};

class Context
{
public:
  bool	init();
  void	release();
  static void displayCallback(const void *callback_data);

  void	update(const SceCtrlData & pad);
  void	startDrawing();
  void	endDrawing();
  void	swapBuffers();
  
private:
  SceUID			_vdmRingBufferUid;
  SceUID			_vertexRingBufferUid;
  SceUID			_fragmentRingBufferUid;
  SceUID			_fragmentUsseRingBufferUid;

  SceGxmContextParams		_contextParams;
  
  SceGxmContext*		_gxmContext;
  SceGxmRenderTarget*		_renderTarget = nullptr;

  void*				_displayBufferData[DISPLAY_BUFFER_COUNT];
  SceUID			_displayBufferUid[DISPLAY_BUFFER_COUNT];
  SceGxmColorSurface		_displaySurface[DISPLAY_BUFFER_COUNT];
  SceGxmSyncObject*		_displayBufferSync[DISPLAY_BUFFER_COUNT];
  SceUID			_depthBufferUid;
  SceGxmDepthStencilSurface	_depthSurface;
  void*				_depthBufferData = nullptr;

  unsigned int			_backBufferIndex = 0;
  unsigned int			_frontBufferIndex = 0;
  
  SceGxmShaderPatcher		*_shaderPatcher = nullptr;
  SceUID			_patcherBufferUid;
  SceUID			_patcherVertexUsseUid;
  SceUID			_patcherFragmentUsseUid;

  void*				_poolAddr = nullptr;
  SceUID			_poolUid;
  unsigned int			_poolIndex;
  unsigned int			_poolSize;
};
