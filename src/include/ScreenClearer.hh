#pragma once

#include <psp2/types.h>
#include <psp2/gxm.h>
#include <debugnet.h>
#include <glm/glm.hpp>
#include <string.h>
#include <string>
#include <psp2/gxt.h>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderFactory.hh"
#include "Utils.hh"

class ScreenClearer
{
public:
  bool initialize(SceGxmContext *context, SceGxmShaderPatcher *patcher);  
  bool release(SceGxmShaderPatcher *shaderPatcher);
  void clear(SceGxmContext *context);
  
private:
  SceUID			_verticesUid;
  SceUID			_indicesUid;

  uint16_t* _pIndices;
  glm::vec3* _pVertices;
  
  const SceGxmProgram		*_vertexProgramGxp;
  const SceGxmProgram		*_fragmentProgramGxp;
  
  SceGxmShaderPatcherId		_vertexProgramId;
  SceGxmShaderPatcherId		_fragmentProgramId;

  SceGxmVertexProgram*		_vertexProgram = nullptr;
  SceGxmFragmentProgram*	_fragmentProgram = nullptr;

  SceGxmVertexAttribute		_vertexAttribute[1];
  SceGxmVertexStream		_vertexStream[1];

  glm::vec4			_clearColor;
};
