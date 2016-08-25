#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>

class PhysicSystem
{
public:
  void init();
  void update(float elapsed);
  void release();
  btDiscreteDynamicsWorld*	getWorld();
  
private:
  btBroadphaseInterface*		_broadphase;
  btDefaultCollisionConfiguration*	_collisionConfiguration;
  btCollisionDispatcher*		_dispatcher;
  btSequentialImpulseConstraintSolver*	_solver;
  btDiscreteDynamicsWorld*		_dynamicsWorld;
  btCollisionShape* _groundShape;
  btRigidBody* _groundRigidBody;
};
