#pragma once

#include <psp2/gxm.h>
#include <debugnet.h>
#include <map>
#include <string>
#include "Mesh.hh"

class Shader
{
public:
  bool	loadProgram(SceGxmShaderPatcher *shaderPatcher,
			    const SceGxmProgram *vertex,
			    const SceGxmProgram *fragment);

  SceGxmVertexProgram *getVertexProgram() const;
  SceGxmFragmentProgram *getFragmentProgram() const;
  void addVertexUniform(const std::string & name);
  void addFragmentUniform(const std::string & name);
  void setUniformMatrix(const std::string & name, SceGxmContext *, const glm::mat4 & mat);
  void setUniformVec4(const std::string & name, SceGxmContext *, const glm::vec4 & vec);
  void release(SceGxmShaderPatcher * shaderPatcher);
  void bind(SceGxmContext *context)
  {
    sceGxmSetVertexProgram(context, _vertexProgram);
    sceGxmSetFragmentProgram(context, _fragmentProgram);
  }
  
private:
  const SceGxmProgram		*_vertexProgramGxp;
  const SceGxmProgram		*_fragmentProgramGxp;
  
  SceGxmShaderPatcherId		_vertexProgramId;
  SceGxmShaderPatcherId		_fragmentProgramId;

  SceGxmVertexProgram*		_vertexProgram = nullptr;
  SceGxmFragmentProgram*	_fragmentProgram = nullptr;

  SceGxmVertexAttribute		_vertexAttribute[1];
  SceGxmVertexStream		_vertexStream[1];

  std::map<std::string, const SceGxmProgramParameter*> _uniforms;
};
