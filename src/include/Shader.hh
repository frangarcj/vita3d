#pragma once

#include <psp2/gxm.h>
#include <debugnet.h>
#include <map>
#include <string>
#include "Texture.hh"
#include "Mesh.hh"

#define VERTEX		0x00000001
#define FRAGMENT	0x00000002

class Shader
{
public:
  bool	loadProgram(SceGxmShaderPatcher *shaderPatcher,
			    const SceGxmProgram *vertex,
			    const SceGxmProgram *fragment);

  SceGxmVertexProgram *getVertexProgram() const;
  SceGxmFragmentProgram *getFragmentProgram() const;
  void addUniform(int flags, const std::string & name);
  void setUniformMat4(int flag, const std::string & name, SceGxmContext *, const glm::mat4 & mat);
  void setUniformMat3(int flag, const std::string & name, SceGxmContext *, const glm::mat3 & mat);
  void setUniformVec4(int flag, const std::string & name, SceGxmContext *, const glm::vec4 & vec);
  void setUniformVec3(int flag, const std::string & name, SceGxmContext *, const glm::vec3 & vec);
  void release(SceGxmShaderPatcher * shaderPatcher);
  void bind(SceGxmContext *context)
  {
    sceGxmSetVertexProgram(context, _vertexProgram);
    sceGxmSetFragmentProgram(context, _fragmentProgram);
  }
  void setUniformTexture(SceGxmContext *context, Texture & texture)
  {
    sceGxmSetFragmentTexture(context, 0, texture.getGxmTex());
  }

  
private:
  const SceGxmProgram		*_vertexProgramGxp;
  const SceGxmProgram		*_fragmentProgramGxp;
  
  SceGxmShaderPatcherId		_vertexProgramId;
  SceGxmShaderPatcherId		_fragmentProgramId;

  SceGxmVertexProgram*		_vertexProgram = nullptr;
  SceGxmFragmentProgram*	_fragmentProgram = nullptr;

  SceGxmVertexAttribute		_vertexAttribute[3];
  SceGxmVertexStream		_vertexStream[1];

  std::map<std::string, const SceGxmProgramParameter*> _vUniforms;
  std::map<std::string, const SceGxmProgramParameter*> _fUniforms;
};
