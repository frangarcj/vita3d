#include <glm/gtc/matrix_inverse.hpp>
#include "Scene.hh"
#include "ScreenClearer.hh"
#include "Texture.hh"

Scene::Scene(Context & context, ShaderFactory & factory) :
  _context(context), _factory(factory)
{

}

Scene::~Scene()
{

}

bool	Scene::init()
{
  ObjLoader loader;
  
  if (!_clearer.initialize(_context.gxmContext(), _factory.shaderPatcher()))
    return false;
  
  if (!_factory.loadShader(_lightShader,
			   "app0:res/Shaders/light_v.gxp",
			   "app0:res/Shaders/light_f.gxp"))
    return false;

  _physicSystem.init();
  
  _lightShader.addUniform(VERTEX, "wvp");
  _lightShader.addUniform(VERTEX, "normalMatrix");
  _lightShader.addUniform(FRAGMENT, "cameraPosition");
  _lightShader.addUniform(FRAGMENT, "model");

  _car.init(loader, _manager, _physicSystem.getWorld());
  _camera.setup();

  
  return true;
}

void	Scene::update(SceCtrlData & pad, float totalT, float elapsed)
{
  _camera.update(pad);
  _car.update(elapsed, pad);  
  _physicSystem.update(elapsed);  
}

void	Scene::clearScreen()
{
  _clearer.clear(_context.gxmContext());
}

void	Scene::draw()
{  
  clearScreen();
  _car.draw(_lightShader, _camera, _context.gxmContext(), _manager);
}

void	Scene::release()
{
  _car.release(_physicSystem.getWorld());
  _factory.releaseShader(_lightShader);
  _clearer.release(_factory.shaderPatcher());
  _physicSystem.release();
}
