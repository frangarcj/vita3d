#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <stdlib.h>
#include "Camera.hh"
#include "AssetsManager.hh"
#include "MeshCollider.hh"
#include "GameObject.hh"

 class Ground : public GameObject
{
public:
  virtual bool init(AssetsManager & manager) override;
  virtual void update(float elapsed, SceCtrlData & pad) override;
  virtual void draw(Camera & camera, SceGxmContext *context, AssetsManager & manager) override;
  virtual void release() override;
  MeshCollider & getCollider();
  
private:
  MeshCollider		_collider;
};
