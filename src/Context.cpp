#include <debugnet.h>
#include <string.h>
#include <string>
#include <psp2/gxm.h>
#include <psp2/gxt.h>
#include <psp2/display.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Context.hh"
#include "Utils.hh"
#include "Shader.hh"
#include "Camera.hh"

bool Context::init()
{
  int err;

  SceGxmInitializeParams initializeParams;
  memset(&initializeParams, 0, sizeof(initializeParams));

  initializeParams.flags = 0;
  initializeParams.displayQueueMaxPendingCount = DISPLAY_MAX_PENDING_SWAPS;
  initializeParams.displayQueueCallback = displayCallback;
  initializeParams.displayQueueCallbackDataSize = sizeof(DisplayData);
  initializeParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

  err = sceGxmInitialize(&initializeParams);
  debugNetPrintf(INFO, (char*)"sceGxmInitialize : %#x\n", err);

  if (err != SCE_OK)
    return false;

  void *vdmRingBuffer = gpu_alloc(
				  SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				  SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
				  4,
				  SCE_GXM_MEMORY_ATTRIB_READ,
				  &_vdmRingBufferUid);

  void *vertexRingBuffer = gpu_alloc(
				     SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				     SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
				     4,
				     SCE_GXM_MEMORY_ATTRIB_READ,
				     &_vertexRingBufferUid);

  void *fragmentRingBuffer = gpu_alloc(
				       SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				       SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
				       4,
				       SCE_GXM_MEMORY_ATTRIB_READ,
				       &_fragmentRingBufferUid);

  unsigned int fragmentUsseRingBufferOffset;
  void *fragmentUsseRingBuffer = fragment_usse_alloc(
						     SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
						     &_fragmentUsseRingBufferUid,
						     &fragmentUsseRingBufferOffset);

  memset(&_contextParams, 0, sizeof(SceGxmContextParams));

  _contextParams.hostMem = malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
  _contextParams.hostMemSize = SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
  _contextParams.vdmRingBufferMem = vdmRingBuffer;
  _contextParams.vdmRingBufferMemSize = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
  _contextParams.vertexRingBufferMem = vertexRingBuffer;
  _contextParams.vertexRingBufferMemSize = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
  _contextParams.fragmentRingBufferMem = fragmentRingBuffer;
  _contextParams.fragmentRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
  _contextParams.fragmentUsseRingBufferMem = fragmentUsseRingBuffer;
  _contextParams.fragmentUsseRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
  _contextParams.fragmentUsseRingBufferOffset = fragmentUsseRingBufferOffset;

  err = sceGxmCreateContext(&_contextParams, &_gxmContext);
  debugNetPrintf(INFO, (char *)"sceGxmCreateContext : %#x\n", err);

  if (err != SCE_OK)
    return false;

  SceGxmRenderTargetParams renderTargetParams;
  memset(&renderTargetParams, 0, sizeof(SceGxmRenderTargetParams));
  renderTargetParams.flags= 0;
  renderTargetParams.width= DISPLAY_WIDTH;
  renderTargetParams.height= DISPLAY_HEIGHT;
  renderTargetParams.scenesPerFrame = 1;
  renderTargetParams.multisampleMode = MSAA_MODE;
  renderTargetParams.multisampleLocations = 0;
  renderTargetParams.driverMemBlock = -1; // Invalid UID

  err = sceGxmCreateRenderTarget(&renderTargetParams, &_renderTarget);
  debugNetPrintf(INFO, (char *)"sceGxmCreateRenderTarget : %#x\n", err);

  if (err != SCE_OK)
    return false;

  for (int i = 0; i < DISPLAY_BUFFER_COUNT; i++) {
    // allocate memory for display
    _displayBufferData[i] = gpu_alloc(
				     SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
				     4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT,
				     SCE_GXM_COLOR_SURFACE_ALIGNMENT,
				     SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				     &_displayBufferUid[i]);
    
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      unsigned int *row = (unsigned int *)_displayBufferData[i] + y*DISPLAY_STRIDE_IN_PIXELS;
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
	row[x] = 0xff000000;
      }
    }
    
    err = sceGxmColorSurfaceInit(
				 &_displaySurface[i],
				 DISPLAY_COLOR_FORMAT,
				 SCE_GXM_COLOR_SURFACE_LINEAR,
				 SCE_GXM_COLOR_SURFACE_SCALE_NONE,
				 SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
				 DISPLAY_WIDTH,
				 DISPLAY_HEIGHT,
				 DISPLAY_STRIDE_IN_PIXELS,
				 _displayBufferData[i]);
    debugNetPrintf(INFO, (char *)"sceGxmColorSurfaceInit : %#x\n", err);
    if (err != SCE_OK)
      return false;
    
    err = sceGxmSyncObjectCreate(&_displayBufferSync[i]);
    debugNetPrintf(INFO, (char *)"sceGxmSyncObjectCreate : %#x\n", err);
    if (err != SCE_OK)
      return false;
  }

  const unsigned int alignedWidth = ALIGN(DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX);
  const unsigned int alignedHeight = ALIGN(DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY);
  unsigned int sampleCount = alignedWidth*alignedHeight;
  unsigned int depthStrideInSamples = alignedWidth;

  _depthBufferData = gpu_alloc(
			      SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
			      4*sampleCount,
			      SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
			      SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
			      &_depthBufferUid);

  err = sceGxmDepthStencilSurfaceInit(
				      &_depthSurface,
				      SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
				      SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
				      depthStrideInSamples,
				      _depthBufferData,
				      nullptr);
  
  debugNetPrintf(INFO, (char *)"sceGxmDepthStencilSurfaceInit : %#x\n", err);
  if (err != SCE_OK)
    return false;  
  
  _poolSize = DEFAULT_TEMP_POOL_SIZE;
  _poolAddr = gpu_alloc(
			SCE_KERNEL_MEMBLOCK_TYPE_USER_RW,
			_poolSize,
			sizeof(void *),
			SCE_GXM_MEMORY_ATTRIB_READ,
			&_poolUid);
  
  _backBufferIndex = 0;
  _frontBufferIndex = 0;    
  
    return true;
}

void Context::displayCallback(const void *callbackData)
{
  SceDisplayFrameBuf framebuf;
  const DisplayData *displayData = (const DisplayData*)callbackData;

  memset(&framebuf, 0, sizeof(SceDisplayFrameBuf));
  framebuf.size = sizeof(SceDisplayFrameBuf);
  framebuf.base = displayData->address;
  framebuf.pitch = DISPLAY_STRIDE_IN_PIXELS;
  framebuf.pixelformat = DISPLAY_PIXEL_FORMAT;
  framebuf.width = DISPLAY_WIDTH;
  framebuf.height = DISPLAY_HEIGHT;

  sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
  sceDisplayWaitVblankStart();
}

void Context::update(const SceCtrlData &pad)
{
}

void Context::startDrawing()
{
  _poolIndex = 0;

  sceGxmBeginScene(
		   _gxmContext,
		   0,
		   _renderTarget,
		   NULL,
		   NULL,
		   _displayBufferSync[_backBufferIndex],
		   &_displaySurface[_backBufferIndex],
		   &_depthSurface);
}

void Context::endDrawing()
{
  sceGxmEndScene(_gxmContext, nullptr, nullptr);
}

void Context::swapBuffers()
{
  sceGxmPadHeartbeat(&_displaySurface[_backBufferIndex], _displayBufferSync[_backBufferIndex]);

  DisplayData displayData;
  displayData.address = _displayBufferData[_backBufferIndex];
  sceGxmDisplayQueueAddEntry(
			     _displayBufferSync[_frontBufferIndex],// OLD fb
			     _displayBufferSync[_backBufferIndex],// NEW fb
			     &displayData);

  // update buffer indices
  _frontBufferIndex = _backBufferIndex;
  _backBufferIndex = (_backBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
}

void Context::release()
{
  sceGxmFinish(_gxmContext);
  sceGxmDisplayQueueFinish();


  gpu_free(_depthBufferUid);
  for (int i = 0; i < DISPLAY_BUFFER_COUNT; i++)
    {
      memset(_displayBufferData[i], 0, DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4);
      gpu_free(_displayBufferUid[i]);
      sceGxmSyncObjectDestroy(_displayBufferSync[i]);
    }
  

  gpu_free(_depthBufferUid);
  
  sceGxmDestroyRenderTarget(_renderTarget);

  sceGxmDestroyContext(_gxmContext);
  fragment_usse_free(_fragmentUsseRingBufferUid);
  gpu_free(_fragmentRingBufferUid);
  gpu_free(_vertexRingBufferUid);
  gpu_free(_vdmRingBufferUid);
  free(_contextParams.hostMem);

  gpu_free(_poolUid);

  // terminate libgxm
  sceGxmTerminate();
}
