#include "Scene.hh"

Scene::Scene(Context & context, ShaderFactory & factory) :
  _context(context), _factory(factory)
{

}

Scene::~Scene()
{

}

bool	Scene::init()
{
  if (!_factory.loadShader(_basicShader, "app0:res/basic_v.gxp", "app0:res/basic_f.gxp"))
    return false;
  _basicShader.addVertexUniform("wvp");

  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f,-1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));
  _cube.addVertex(glm::vec3(1.0f,-1.0f, 1.0));

  for (uint16_t i = 0; i < 12 *3; i++)
    _cube.addIndex(i);
  _cube.uploadToVram();

  _camera.setup();

    if (!_factory.loadShader(_clearShader, "app0:res/clear_v.gxp", "app0:res/clear_f.gxp"))
      return false;
  
  _clearMesh.addVertex(glm::vec3(-1.0f, -1.0f, 1.0f));
  _clearMesh.addVertex(glm::vec3(3.0f, -1.0f, 1.0f));
  _clearMesh.addVertex(glm::vec3(-1.0f, 3.0f, 1.0f));
  
  for (uint16_t i = 0; i < 3; i++)      
    _clearMesh.addIndex(i);
  _clearMesh.uploadToVram();

  
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
  _clearShader.bind(_context.gxmContext());
  _clearMesh.draw(_context.gxmContext());
}

void	Scene::draw()
{
  clearScreen();
  _basicShader.bind(_context.gxmContext());

  glm::mat4 m = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(0.0f, 1.0f, 0.0f));
  m = glm::rotate(m, angleY, glm::vec3(1.0f, 0.0f, 0.0f));  
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * m;

  _basicShader.setUniformMatrix("wvp", _context.gxmContext(), mvp);
  _cube.draw(_context.gxmContext());
}

void	Scene::release()
{
  _factory.releaseShader(_basicShader);
  _factory.releaseShader(_clearShader);  
  _cube.release();
  _clearMesh.release();
}
