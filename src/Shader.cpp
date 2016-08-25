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

  const SceGxmProgramParameter *normalAttribute =
    sceGxmProgramFindParameterByName(vertex, "aNormal");
  
  debugNetPrintf(INFO, (char *)"normalAttribute addr : %p\n", normalAttribute);
  if (err != SCE_OK)
    return false;

  const SceGxmProgramParameter *texCoordsAttribute =
    sceGxmProgramFindParameterByName(vertex, "aTexCoord");
  
  debugNetPrintf(INFO, (char *)"texCoordsAttribute addr : %p\n", texCoordsAttribute);
  if (err != SCE_OK)
    return false;
  
  _vertexAttribute[0].streamIndex = 0;
  _vertexAttribute[0].offset = 0;
  _vertexAttribute[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
  _vertexAttribute[0].componentCount = 3;
  _vertexAttribute[0].regIndex = sceGxmProgramParameterGetResourceIndex(posAttribute);

  _vertexAttribute[1].streamIndex = 0;
  _vertexAttribute[1].offset = 12;
  _vertexAttribute[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
  _vertexAttribute[1].componentCount = 3;
  _vertexAttribute[1].regIndex = sceGxmProgramParameterGetResourceIndex(normalAttribute);
  
  _vertexAttribute[2].streamIndex = 0;
  _vertexAttribute[2].offset = 24;
  _vertexAttribute[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
  _vertexAttribute[2].componentCount = 2;
  _vertexAttribute[2].regIndex = sceGxmProgramParameterGetResourceIndex(texCoordsAttribute);
  
  _vertexStream[0].stride = sizeof(Vattrib);
  _vertexStream[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
  
  SceGxmBlendInfo blend_info;  
  blend_info.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
  blend_info.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
  blend_info.colorSrc  = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
  blend_info.colorDst  = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blend_info.alphaSrc  = SCE_GXM_BLEND_FACTOR_ONE;
  blend_info.alphaDst  = SCE_GXM_BLEND_FACTOR_ZERO;
  blend_info.colorMask = SCE_GXM_COLOR_MASK_ALL;

  debugNetPrintf(INFO, (char *)"Creating vertex program");
  if (err != SCE_OK)
    return false;
  
  err = sceGxmShaderPatcherCreateVertexProgram(shaderPatcher,
						   _vertexProgramId,
						   _vertexAttribute,
						   3,
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

void Shader::addUniform(int flags, const std::string & name)
{
  if (flags & FRAGMENT)
    {
      _fUniforms[name] = sceGxmProgramFindParameterByName(_fragmentProgramGxp, name.c_str());
      debugNetPrintf(INFO, (char*)"%s : %p\n", name.c_str(), _fUniforms[name]);
    }
  if (flags & VERTEX)
    {
      _vUniforms[name] = sceGxmProgramFindParameterByName(_vertexProgramGxp, name.c_str());
      debugNetPrintf(INFO, (char*)"%s : %p\n", name.c_str(), _vUniforms[name]);
    }
}

void Shader::setUniformMat3(int flag,
			    const std::string &name,
			    SceGxmContext *context,
			    const glm::mat3 &mat)
{
  if (flag & FRAGMENT)
    {
      void *buff;
      sceGxmReserveFragmentDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _fUniforms[name], 0, 9, glm::value_ptr(mat));
    }
  if (flag & VERTEX)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _vUniforms[name], 0, 9, glm::value_ptr(mat));
    }
}

void Shader::setUniformMat4(int flag,
			    const std::string & name,
			      SceGxmContext *context,
			      const glm::mat4 & mat)
{
  if (flag & FRAGMENT)
    {
      void *buff;
      sceGxmReserveFragmentDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _fUniforms[name], 0, 16,glm::value_ptr(mat));
    }
  if (flag & VERTEX)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _vUniforms[name], 0, 16,glm::value_ptr(mat));
    }
}

void Shader::setUniformVec4(int flag,
			    const std::string & name,
			    SceGxmContext *context,
			    const glm::vec4 & vec)
{
  if (flag & FRAGMENT)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _fUniforms[name], 0, 4,glm::value_ptr(vec));
    }
  if (flag & VERTEX)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _vUniforms[name], 0, 4,glm::value_ptr(vec));
    }
}

void Shader::setUniformVec3(int flag,
			    const std::string & name,
			    SceGxmContext *context,
			    const glm::vec3 & vec)
{
  if (flag & FRAGMENT)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _fUniforms[name], 0, 3, glm::value_ptr(vec));
    }
  if (flag & VERTEX)
    {
      void *buff;
      sceGxmReserveVertexDefaultUniformBuffer(context, &buff);
      sceGxmSetUniformDataF(buff, _vUniforms[name], 0, 3, glm::value_ptr(vec));
    }
}


void Shader::release(SceGxmShaderPatcher *shaderPatcher)
{
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _fragmentProgramId);
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _vertexProgramId);
}
