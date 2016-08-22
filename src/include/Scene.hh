#pragma once

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include "Context.hh"
#include "ShaderFactory.hh"
#include "Camera.hh"
#include "ObjLoader.hh"


class Scene
{
public:
  Scene(Context & context, ShaderFactory & factory);
  ~Scene();
  bool	init();
  void	update(SceCtrlData & pad);
  void	draw();
  void  release();
  
private:
  void			clearScreen();
  
  Context &		_context;
  ShaderFactory &	_factory;
  Camera		_camera;
  
  Shader		_basicShader;

  std::vector<Mesh>	_model;

  Shader		_clearShader;
  Mesh			_clearMesh;
};
