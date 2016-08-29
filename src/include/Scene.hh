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
#include "GameObject.hh"

class Scene
{
public:
  Scene(Context & context, ShaderFactory & factory);
  ~Scene();
  bool loadRessources();
  bool	init();
  void	update(SceCtrlData & pad, float totalT, float elapsedT);
  void	draw();
  void  release();
  
private:
  void			clearScreen();  
  Context &			_context;
  ShaderFactory &		_factory;
  AssetsManager			_manager;  
  Camera			_camera;
  Shader			_lightShader;
  ScreenClearer			_clearer;
  PhysicSystem			_physicSystem;
  std::vector<GameObject*>	_gameObjects;
};
