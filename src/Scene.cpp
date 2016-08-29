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

bool Scene::loadRessources()
{
  Shader lightShader;
  _manager.addShader("light", lightShader);

  if (!_factory.loadShader(_manager.getShader("light"),
			   "app0:res/Shaders/light_v.gxp",
			   "app0:res/Shaders/light_f.gxp"))
    return false;

    _manager.getShader("light").addUniform(VERTEX, "wvp");
  _manager.getShader("light").addUniform(VERTEX, "normalMatrix");

  if (!_manager.addMesh("app0:res/Mesh/Ground/ground.obj"))
    return false;
  if (!_manager.addMesh("app0:res/Mesh/car/chassis.obj"))
    return false;
  if (!_manager.addMesh("app0:res/Mesh/car/wheel.obj"))
    return false;


  Car* car = new Car;
  car->getVehicle().init(_physicSystem);
  car->init(_manager);


  Ground *ground = new Ground;
  ground->init(_manager);


  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3(0,0,0));
  _physicSystem.addCollisionShape(ground->getCollider().getCollisionShape());
  _physicSystem.localCreateRigidBody(0, tr, ground->getCollider().getCollisionShape());

  Shader & s = _manager.getShader("light");


    _gameObjects.push_back(car);
    _gameObjects.push_back(ground);

  return true;
}

bool	Scene::init()
{
  ObjLoader loader;

  _physicSystem.init();

  if (!loadRessources())
    return false;

  if (!_clearer.initialize(_context.gxmContext(), _factory.shaderPatcher()))
    return false;


  _camera.setup();

  return true;
}

void	Scene::update(SceCtrlData & pad, float totalT, float elapsed)
{
  _camera.update(pad);
  _physicSystem.update(elapsed);

  for (auto& e : _gameObjects)
    e->update(elapsed, pad);
}

void	Scene::clearScreen()
{
  _clearer.clear(_context.gxmContext());
}

void	Scene::draw()
{
  clearScreen();
  for (auto& e : _gameObjects)
    e->draw(_camera, _context.gxmContext(), _manager);
}

void	Scene::release()
{
  _factory.releaseShader(_lightShader);
  _clearer.release(_factory.shaderPatcher());
}
