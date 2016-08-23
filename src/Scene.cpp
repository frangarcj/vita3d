#include "Scene.hh"
#include "ScreenClearer.hh"

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
  loader.loadModel("app0:res/Mesh/dpv/dpv.obj", _model);

  _clearer.initialize(_context.gxmContext(), _factory.shaderPatcher());
  
  _camera.setup();
  
  return true;
}

float angleX = 0;
float angleY = 0;

void	Scene::update(SceCtrlData & pad)
{
  if (pad.buttons & SCE_CTRL_RIGHT)
    angleX += 0.1f;
  if (pad.buttons & SCE_CTRL_LEFT)
    angleX -= 0.1f;
  if (pad.buttons & SCE_CTRL_UP)
    angleY += 0.1f;
  if (pad.buttons & SCE_CTRL_DOWN)
    angleY -= 0.1f;
}

void	Scene::clearScreen()
{
  _clearer.clear(_context.gxmContext());
}

void	Scene::draw()
{  
  clearScreen();
  _basicShader.bind(_context.gxmContext());

  
  glm::mat4 m = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(0.0f, 1.0f, 0.0f));
  m = glm::rotate(m, angleY, glm::vec3(1.0f, 0.0f, 0.0f));  
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;

  _basicShader.setUniformMatrix("wvp", _context.gxmContext(), mvp);
  
  for (auto & m : _model)
    {
      m.draw(_context.gxmContext());
    }

}

void	Scene::release()
{
  _factory.releaseShader(_basicShader);
  for (auto & m : _model)
    m.draw(_context.gxmContext());
  _clearer.release(_factory.shaderPatcher());
}
