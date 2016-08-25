#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <psp2/types.h>
#include <psp2/ctrl.h>

class Camera
{
public:
  void	setup();
  void update(const SceCtrlData & pad);
  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix() const;
  const glm::vec3 getPosition() const
  {
    return _cameraPos;
  }
  
private:
  glm::vec3	_cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);
  glm::vec3	_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3	_cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
  float		_cameraSpeed = 0.05f;
  glm::mat4	_projection;  
  bool		_firstTime = true;
  glm::vec2	_lastLeft;
  glm::vec2	_lastRight;    
  float yaw = -90.0f;
  float pitch = 0;
};
