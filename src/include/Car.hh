#pragma once

#include "GameObject.hh"
#include "PhysicVehicle.hh"

class Car : public GameObject
{
public:
  virtual bool init(AssetsManager & manager) override;
  virtual void update(float elapsed, SceCtrlData & pad) override;
  virtual void draw(Camera & camera, SceGxmContext *context, AssetsManager & manager) override;
  virtual void release() override;

  PhysicVehicle & getVehicle();
  
private:
  PhysicVehicle _vehicle;
};
