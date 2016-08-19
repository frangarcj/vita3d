#include "Camera.hh"

void Camera::setup()
{
  projection = glm::perspective(glm::radians(50.0f), 960.0f / 544.0f, 0.1f, 100.0f);
}

void Camera::update(const SceCtrlData & pad)
{
    if (firstTime)
    {
      lastX = pad.rx;
      lastY = pad.ry;
      lastXLeft = pad.lx;
      lastYLeft = pad.ly;
      firstTime = false;
    }

    float xoffsetR = pad.lx - lastXLeft;
    float yoffsetR = pad.ly - lastYLeft;
    
    if (yoffsetR > 10)
      cameraPos += cameraSpeed * cameraFront;
    if (yoffsetR < -10)
      cameraPos -= cameraSpeed * cameraFront;
    if (xoffsetR < -10)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (xoffsetR > 10)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    

    lastXLeft = pad.lx;
    lastYLeft = pad.ly;
    
    float xoffset = pad.rx - lastX;
    float yoffset = pad.ry - lastY; 
    lastX = pad.rx;
    lastY = pad.ry;
    
    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
  
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getProjectionMatrix() const
{
  return projection;
}
