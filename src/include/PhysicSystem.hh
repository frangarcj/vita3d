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
  void addCollisionShape(btCollisionShape *collisionShape);
  btRigidBody* localCreateRigidBody(btScalar mass,
				  const btTransform& startTransform,
				    btCollisionShape* shape);

  
private:
  btBroadphaseInterface*			_overlappingPairCache;
  btCollisionDispatcher*			_dispatcher;
  btConstraintSolver*				_constraintSolver;
  btBroadphaseInterface*			_broadphase;
  btDefaultCollisionConfiguration*		_collisionConfiguration;
  btSequentialImpulseConstraintSolver*		_solver;
  btDiscreteDynamicsWorld*			_dynamicsWorld;
  btAlignedObjectArray<btCollisionShape*>	_collisionShapes;
};
