#pragma once

#include <string>
#include "Shader.hh"
#include "Utils.hh"

class ShaderFactory
{
public:
  bool	initialize();
  void	release();
  bool	loadShader(Shader & shader,
		   const std::string & vertexFile,
		   const std::string & fragmentFile);
  void	releaseShader(Shader & shader);
  static SceGxmProgram*	        readShader(const std::string & fileName);
  SceGxmShaderPatcher*		shaderPatcher()
  {
    return _shaderPatcher;
  }
  
private:

  SceGxmShaderPatcher*		_shaderPatcher = nullptr;
  SceUID			_patcherBufferUid;
  SceUID			_patcherVertexUsseUid;
  SceUID			_patcherFragmentUsseUid;
};
