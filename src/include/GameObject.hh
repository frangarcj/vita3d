#pragma once

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include "AssetsManager.hh"
#include "Camera.hh"

class GameObject
{
public:
  virtual bool init(AssetsManager & manager) = 0;
  virtual void update(float elapsed, SceCtrlData & pad) = 0;
  virtual void draw(Camera & camera, SceGxmContext *context, AssetsManager & manager) = 0;
  virtual void release() = 0;
};
