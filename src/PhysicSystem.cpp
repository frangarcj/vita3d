#include "PhysicSystem.hh"

void PhysicSystem::init()
{
  _broadphase = new btDbvtBroadphase();  
  _collisionConfiguration = new btDefaultCollisionConfiguration();
  _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
  
  _solver = new btSequentialImpulseConstraintSolver;

  _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
  _dynamicsWorld->setGravity(btVector3(0, -10, 0));

  
  _groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);  
  btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, _groundShape, btVector3(0, 0, 0));
  
  _groundRigidBody = new btRigidBody(groundRigidBodyCI);
  _dynamicsWorld->addRigidBody(_groundRigidBody);
}

void PhysicSystem::update(float elapsed)
{
  _dynamicsWorld->stepSimulation(elapsed, 10);
}

void PhysicSystem::release()
{
  _dynamicsWorld->removeRigidBody(_groundRigidBody);
  delete _groundRigidBody->getMotionState();
  delete _groundRigidBody;  
  delete _groundShape;
}

btDiscreteDynamicsWorld* PhysicSystem::getWorld()
{
  return _dynamicsWorld;
}
