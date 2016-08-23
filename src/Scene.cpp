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
 
  if (!_factory.loadShader(_basicShader, "app0:res/Shaders/basic_v.gxp", "app0:res//Shaders/basic_f.gxp"))
    return false;
  _basicShader.addVertexUniform("wvp");

  ObjLoader loader;
  if (!loader.loadModel("app0:res/Mesh/dpv/dpv.obj", _buggy, _manager))
    return false;
  if (!loader.loadModel("app0:res/Mesh/girl/girl.obj", _girl, _manager))
    return false;

  if (!_clearer.initialize(_context.gxmContext(), _factory.shaderPatcher()))
    return false;
  
  _camera.setup();
  
  return true;
}

float angleX = 0;
float angleY = 0;

int fps = 0;

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
  _basicShader.bind(_context.gxmContext());

  
  glm::mat4 m = glm::mat4(1.0f);
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;

  _basicShader.setUniformMatrix("wvp", _context.gxmContext(), mvp);

  for (auto & m : _buggy)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
	_basicShader.setUniformTexture(_context.gxmContext(), _manager.getTexture(diff));
      m.draw(_context.gxmContext());     
    }

  m = glm::translate(m, glm::vec3(3.0f, 0.0f, 0.0f));
  mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;
  _basicShader.setUniformMatrix("wvp", _context.gxmContext(), mvp);
  for (auto & m : _girl)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
	_basicShader.setUniformTexture(_context.gxmContext(), _manager.getTexture(diff));
      m.draw(_context.gxmContext());     
    }  
  
}

void	Scene::release()
{
  _factory.releaseShader(_basicShader);
  for (auto & m : _buggy)
    m.draw(_context.gxmContext());
  for (auto & m : _girl)
    m.draw(_context.gxmContext());
  _clearer.release(_factory.shaderPatcher());
}
