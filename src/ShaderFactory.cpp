#include <debugnet.h>
#include <psp2/gxt.h>
#include <cstring>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <stdlib.h>
#include "ShaderFactory.hh"

bool	ShaderFactory::initialize()
{
    const unsigned int patcherBufferSize= 64*1024;
    const unsigned int patcherVertexUsseSize= 64*1024;
    const unsigned int patcherFragmentUsseSize= 64*1024;

    void *patcherBuffer = gpu_alloc(
				    SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				    patcherBufferSize,
				    4,
				    SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				    &_patcherBufferUid);

    unsigned int patcherVertexUsseOffset;
    void *patcherVertexUsse = vertex_usse_alloc(
						patcherVertexUsseSize,
						&_patcherVertexUsseUid,
						&patcherVertexUsseOffset);

    unsigned int patcherFragmentUsseOffset;
    void *patcherFragmentUsse = fragment_usse_alloc(
						    patcherFragmentUsseSize,
						    &_patcherFragmentUsseUid,
						    &patcherFragmentUsseOffset);

    // create a shader patcher
    SceGxmShaderPatcherParams patcherParams;
    memset(&patcherParams, 0, sizeof(SceGxmShaderPatcherParams));
    patcherParams.userData= NULL;
    patcherParams.hostAllocCallback= &patcher_host_alloc;
    patcherParams.hostFreeCallback= &patcher_host_free;
    patcherParams.bufferAllocCallback= NULL;
    patcherParams.bufferFreeCallback= NULL;
    patcherParams.bufferMem= patcherBuffer;
    patcherParams.bufferMemSize= patcherBufferSize;
    patcherParams.vertexUsseAllocCallback= NULL;
    patcherParams.vertexUsseFreeCallback= NULL;
    patcherParams.vertexUsseMem= patcherVertexUsse;
    patcherParams.vertexUsseMemSize= patcherVertexUsseSize;
    patcherParams.vertexUsseOffset= patcherVertexUsseOffset;
    patcherParams.fragmentUsseAllocCallback= NULL;
    patcherParams.fragmentUsseFreeCallback= NULL;
    patcherParams.fragmentUsseMem= patcherFragmentUsse;
    patcherParams.fragmentUsseMemSize= patcherFragmentUsseSize;
    patcherParams.fragmentUsseOffset= patcherFragmentUsseOffset;

    int err = sceGxmShaderPatcherCreate(&patcherParams, &_shaderPatcher);
    debugNetPrintf(INFO, (char *)"sceGxmShaderPatcherCreate : %#x\n", err);
    if (err != SCE_OK)
      return false;
    
    return true;
}

bool	ShaderFactory::loadShader(Shader &shader,
				  const std::string &vertexFile,
				  const std::string &fragmentFile)
{
  SceGxmProgram *vertexProgram = nullptr;
  SceGxmProgram *fragmentProgram = nullptr;

  if ((vertexProgram = readShader(vertexFile)) == nullptr)
    return false;
  if ((fragmentProgram = readShader(fragmentFile)) == nullptr)
    {
      free(vertexProgram);
      return false;
    }
  
  return shader.loadProgram(_shaderPatcher, vertexProgram, fragmentProgram);
}

SceGxmProgram* ShaderFactory::readShader(const std::string & fileName) const
{
  SceUID fd = sceIoOpen(fileName.c_str(), SCE_O_RDONLY, 0777);
  if (fd < 0)
    {
      debugNetPrintf(ERROR, (char*)"Error while opening file : %s\nExiting...", fileName.c_str());
      return nullptr;
    }

  SceOff size = sceIoLseek(fd, 0, SCE_SEEK_END);
  sceIoLseek(fd, 0, SCE_SEEK_SET);
  
  unsigned char *program = (unsigned char*)malloc(size * sizeof(unsigned char));
  if (program == nullptr)
    {
      debugNetPrintf(ERROR, (char*)"Error while allocating : %d\nExiting...", size);
      sceIoClose(fd);
      return nullptr;
    }
  
  if (sceIoRead(fd, program, size) <= 0)
    {
      debugNetPrintf(ERROR, (char*)"sceIoRead failed");
      free(program);
      sceIoClose(fd);
      return nullptr;
    }

  sceIoClose(fd);
  
  return (SceGxmProgram*)program;
}

void	ShaderFactory::release()
{
  sceGxmShaderPatcherDestroy(_shaderPatcher);
  fragment_usse_free(_patcherFragmentUsseUid);
  vertex_usse_free(_patcherVertexUsseUid);
  gpu_free(_patcherBufferUid);
}

void	ShaderFactory::releaseShader(Shader &shader)
{
  shader.release(_shaderPatcher);
}
