#pragma once

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include "Context.hh"
#include "ShaderFactory.hh"
#include "Camera.hh"
#include "ObjLoader.hh"
#include "ScreenClearer.hh"
#include "AssetsManager.hh"
#include "Car.hh"
#include "PhysicSystem.hh"
#include "Ground.hh"

class Scene
{
public:
  Scene(Context & context, ShaderFactory & factory);
  ~Scene();
  bool	init();
  void	update(SceCtrlData & pad, float totalT, float elapsedT);
  void	draw();
  void  release();
  
private:
  void			clearScreen();
  
  Context &		_context;
  ShaderFactory &	_factory;
  Camera		_camera;
  Shader		_lightShader;
  ScreenClearer		_clearer;
  AssetsManager		_manager;
  Car			_car;
  Ground		_ground;
  PhysicSystem		_physicSystem;
};
