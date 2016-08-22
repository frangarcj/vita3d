#include <psp2/gxt.h>
#include <string>
#include <psp2/sysmodule.h>
#include <cstring>
#include <map>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hh"

bool Shader::loadProgram(SceGxmShaderPatcher *shaderPatcher,
			 const SceGxmProgram *vertex,
			 const SceGxmProgram *fragment)
{
  _vertexProgramGxp = vertex;
  _fragmentProgramGxp = fragment;
  
  int err = sceGxmProgramCheck(vertex);
  debugNetPrintf(INFO, (char *)"sceGxmProgramCheck(vertex) : %#x\n", err);
  if (err != SCE_OK)
    return false;

  err = sceGxmProgramCheck(fragment);
  debugNetPrintf(INFO, (char *)"sceGxmProgramCheck(fragment) : %#x\n", err);
  if (err != SCE_OK)
    return false;

  err = sceGxmShaderPatcherRegisterProgram(shaderPatcher,
					   vertex,
					   &_vertexProgramId);
  debugNetPrintf(INFO, (char *)"sceGxmShaderPatcherRegisterProgram : %#x\n", err);
  if (err != SCE_OK)
    return false;
  
  err = sceGxmShaderPatcherRegisterProgram(shaderPatcher,
					   fragment,
					   &_fragmentProgramId);
  debugNetPrintf(INFO, (char *)"sceGxmShaderPatcherRegisterProgram : %#x\n", err);
  if (err != SCE_OK)
    return false;

  const SceGxmProgramParameter *posAttribute =
    sceGxmProgramFindParameterByName(vertex, "aPosition");
  
  debugNetPrintf(INFO, (char *)"posAttribute addr : %p\n", posAttribute);
  if (err != SCE_OK)
    return false;
  
  _vertexAttribute[0].streamIndex = 0;
  _vertexAttribute[0].offset = 0;
  _vertexAttribute[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
  _vertexAttribute[0].componentCount = 3;
  _vertexAttribute[0].regIndex = sceGxmProgramParameterGetResourceIndex(posAttribute);

  _vertexStream[0].stride = sizeof(glm::vec3);
  _vertexStream[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

  SceGxmBlendInfo blend_info;  
  blend_info.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
  blend_info.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
  blend_info.colorSrc  = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
  blend_info.colorDst  = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blend_info.alphaSrc  = SCE_GXM_BLEND_FACTOR_ONE;
  blend_info.alphaDst  = SCE_GXM_BLEND_FACTOR_ZERO;
  blend_info.colorMask = SCE_GXM_COLOR_MASK_ALL;

  err = sceGxmShaderPatcherCreateVertexProgram(shaderPatcher,
						   _vertexProgramId,
						   _vertexAttribute,
						   1,
						   _vertexStream,
						   1,
						   &_vertexProgram);
  debugNetPrintf(INFO, (char*)"sceGxmShaderPatcherCreateVertexProgram : %#x\n", err);
  if (err != SCE_OK)
    return false;

  err = sceGxmShaderPatcherCreateFragmentProgram(shaderPatcher,
						 _fragmentProgramId,
						 SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
						 SCE_GXM_MULTISAMPLE_NONE,
						 &blend_info,
						 vertex,
						 &_fragmentProgram);

  debugNetPrintf(INFO, (char*)"sceGxmShaderPatcherCreateFragmentProgram : %#x\n", err);
  if (err != SCE_OK)
    return false;
  
  return true;
}

SceGxmVertexProgram *Shader::getVertexProgram() const
{
  return _vertexProgram;
}

SceGxmFragmentProgram *Shader::getFragmentProgram() const
{
    return _fragmentProgram;
}

void Shader::addFragmentUniform(const std::string & name)
{
  _uniforms[name] = sceGxmProgramFindParameterByName(_fragmentProgramGxp, name.c_str());
  debugNetPrintf(INFO, (char*)"%s : %p\n", name.c_str(), _uniforms[name]);
}

void Shader::addVertexUniform(const std::string & name)
{
  _uniforms[name] = sceGxmProgramFindParameterByName(_vertexProgramGxp, name.c_str());
  debugNetPrintf(INFO, (char*)"%s : %p\n", name.c_str(), _uniforms[name]);
}

void Shader::setUniformMatrix(const std::string & name,
			      SceGxmContext *context,
			      const glm::mat4 & mat)
{
  void *buff;
  sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
  sceGxmSetUniformDataF(buff, _uniforms[name], 0, 16,glm::value_ptr(mat));
}

void Shader::setUniformVec4(const std::string & name, SceGxmContext *context, const glm::vec4 & vec)
{
  void *buff;
  sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
  sceGxmSetUniformDataF(buff, _uniforms[name], 0, 4,glm::value_ptr(vec));
}

void Shader::release(SceGxmShaderPatcher *shaderPatcher)
{
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _fragmentProgramId);
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _vertexProgramId);
}
