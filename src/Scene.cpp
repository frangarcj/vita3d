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
  if (!_factory.loadShader(_lightShader, "app0:res/Shaders/light_v.gxp", "app0:res/Shaders/light_f.gxp"))
    return false;
  _lightShader.addUniform(VERTEX, "wvp");
  _lightShader.addUniform(VERTEX, "normalMatrix");
  _lightShader.addUniform(FRAGMENT, "cameraPosition");
  _lightShader.addUniform(FRAGMENT, "model");
  
  ObjLoader loader;
  if (!loader.loadModel("app0:res/Mesh/dpv/dpv.obj", _buggy, _manager))
    return false;

  if (!loader.loadModel("app0:res/Mesh/Sphere/sphere.obj", _earth, _manager))
    return false;
  
  if (!_clearer.initialize(_context.gxmContext(), _factory.shaderPatcher()))
    return false;
  
  _camera.setup();
  
  return true;
}

void	Scene::update(SceCtrlData & pad)
{ 
  _camera.update(pad);
}

void	Scene::clearScreen()
{
  _clearer.clear(_context.gxmContext());
}

void	Scene::draw()
{  
  clearScreen();
  _lightShader.bind(_context.gxmContext());

  
  glm::mat4 m = glm::mat4(1.0f);
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;
  glm::mat4 mv = _camera.getViewMatrix() * m;  
  glm::mat3 n = glm::inverseTranspose(glm::mat3(mv));

  _lightShader.setUniformMat4(VERTEX, "wvp", _context.gxmContext(), mvp);  
  _lightShader.setUniformMat3(VERTEX, "normalMatrix", _context.gxmContext(), n);
  _lightShader.setUniformVec3(FRAGMENT, "cameraPosition", _context.gxmContext(),
			      _camera.getPosition());
  _lightShader.setUniformMat4(FRAGMENT, "model", _context.gxmContext(), m);

  for (auto & m : _buggy)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
        _lightShader.setUniformTexture(_context.gxmContext(), _manager.getTexture(diff));
      m.draw(_context.gxmContext());     
    }

  m = glm::translate(m, glm::vec3(4.0f, 0.0f, 0.0));
  mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;
  mv = _camera.getViewMatrix() * m;
  n = glm::inverseTranspose(glm::mat3(mv));

  _lightShader.setUniformMat4(VERTEX, "wvp", _context.gxmContext(), mvp);  
  _lightShader.setUniformMat3(VERTEX, "normalMatrix", _context.gxmContext(), n);
  _lightShader.setUniformMat4(FRAGMENT, "model", _context.gxmContext(), m);

  for (auto & m : _earth)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
        _lightShader.setUniformTexture(_context.gxmContext(), _manager.getTexture(diff));
      m.draw(_context.gxmContext());     
    }
}

void	Scene::release()
{
  _factory.releaseShader(_basicShader);
  for (auto & m : _buggy)
    m.release();
  _clearer.release(_factory.shaderPatcher());
}
