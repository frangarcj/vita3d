#include "Car.hh"
#include <debugnet.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define CUBE_HALF_EXTENTS 1.f

bool Car::init(AssetsManager & manager)
{
  return true;
}

void Car::update(float elapsedTime, SceCtrlData & pad)
{
  _vehicle.resetAcceleration();

  if (pad.buttons & SCE_CTRL_CROSS)
    _vehicle.accelerate();
  if (pad.buttons & SCE_CTRL_SQUARE)
    _vehicle.brake();
  if (pad.buttons & SCE_CTRL_LEFT)
    _vehicle.turnLeft();
  if (pad.buttons & SCE_CTRL_RIGHT)
    _vehicle.turnRight();

  _vehicle.update();
}

void Car::draw(Camera & camera, SceGxmContext *context, AssetsManager & manager)
{
  glm::mat4 m;  

  _vehicle.getChassisTansform().getOpenGLMatrix(&m[0][0]);


  glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
  glm::mat4 n = glm::inverseTranspose(m);

  Shader & shader = manager.getShader("light");
  Mesh & chassis = manager.getMesh("chassis");
  Mesh & wheel = manager.getMesh("wheel");
  Material & mat_chassis = manager.getMaterial("MAT_chassis");
  Material & mat_wheel = manager.getMaterial("MAT_wheel");

  shader.bind(context);
  shader.setUniformMat4(VERTEX, "wvp", context, mvp);
  shader.setUniformMat4(VERTEX, "normalMatrix", context, n);
  shader.setUniformTexture(context, manager.getTexture(mat_chassis.map_diffuse));
  chassis.draw(context);


  for (int i = 0; i < 4; i++)
    {
      _vehicle.getWheelTransform(i).getOpenGLMatrix(&m[0][0]);

      mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
      n = glm::inverseTranspose(m);

      shader.bind(context);
      shader.setUniformMat4(VERTEX, "wvp", context, mvp);
      shader.setUniformMat4(VERTEX, "normalMatrix", context, n);
      shader.setUniformTexture(context, manager.getTexture(mat_wheel.map_diffuse));
      wheel.draw(context);
    }
}

void Car::release()
{
}

PhysicVehicle & Car::getVehicle()
{
  return _vehicle;
}
