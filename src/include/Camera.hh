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
  
private:
  glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  10.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
  float cameraSpeed = 0.5f;
  glm::mat4 projection;
  
  bool firstTime = true;

  float lastXLeft;
  float lastYLeft;
  
  float lastX = 0;
  float lastY = 0;
  
  float yaw = -90.0f;
  float pitch = 0;
};
