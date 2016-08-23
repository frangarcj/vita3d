#include "Camera.hh"

void Camera::setup()
{
  _projection = glm::perspective(glm::radians(50.0f), 960.0f / 544.0f, 0.1f, 100.0f);
}

void Camera::update(const SceCtrlData & pad)
{
    if (_firstTime)
    {
      _lastRight = glm::vec2(pad.rx, pad.ry);
      _firstTime = false;
    }

    float xoffset = pad.lx - 127.0f;
    float yoffset = pad.ly - 127.0f;    

    if (xoffset > 50.f)
      {
	_cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * 0.1f;
      }
    else if (xoffset < -50.f)
      {
	_cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * 0.1f;
      }
    if (yoffset > 50.f)
      {
	_cameraPos -= 0.1f * _cameraFront;
      }
    else if (yoffset < -50.f)
      {
	_cameraPos += 0.1f * _cameraFront;
      }


    float sensitivity = 0.007f;
    xoffset = pad.rx - 127.0f;
    yoffset = pad.ry - 127.0f; 

    if (xoffset > 50.f)
      {
	xoffset *= sensitivity;
	yaw += xoffset;
      }
    else if (xoffset < -50.f)
      {
	xoffset *= sensitivity;
	yaw += xoffset;
      }

    if (yoffset > 50.f)
      {
	yoffset *= sensitivity;
	pitch -= yoffset;
      }
    else if (yoffset < -50.f)
      {
	yoffset *= sensitivity;
	pitch -= yoffset;
      }
   
    if (pad.buttons & SCE_CTRL_CROSS)
      {
	_cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);
	_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	_cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
	yaw = -90.0f;
	pitch = 0.0f;
      }


    
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    _cameraFront = glm::normalize(front);
}

glm::mat4 Camera::getViewMatrix() const
{
  return glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}

glm::mat4 Camera::getProjectionMatrix() const
{
  return _projection;
}
