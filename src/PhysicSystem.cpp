#include "PhysicSystem.hh"

void PhysicSystem::init()
{
  _collisionConfiguration = new btDefaultCollisionConfiguration();
  _dispatcher = new btCollisionDispatcher(_collisionConfiguration);

  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  _overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
  _constraintSolver = new btSequentialImpulseConstraintSolver();
  _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher,
					       _overlappingPairCache,
					       _constraintSolver,
					       _collisionConfiguration);

  
}

void PhysicSystem::update(float elapsed)
{
  _dynamicsWorld->stepSimulation(elapsed, 10);
}

void PhysicSystem::release()
{
  for (int i = 0; i < _collisionShapes.size(); i++)
    {
      btCollisionShape* shape = _collisionShapes[i];
      delete shape;
    }
  delete _dynamicsWorld;  
}

btDiscreteDynamicsWorld* PhysicSystem::getWorld()
{
  return _dynamicsWorld;
}

btRigidBody* PhysicSystem::localCreateRigidBody(btScalar mass,
						const btTransform &startTransform,
						btCollisionShape *shape)
{
  bool isDynamic = (mass != 0.f);

  btVector3 localInertia(0,0,0);
  if (isDynamic)
    shape->calculateLocalInertia(mass,localInertia);
  btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);
  btRigidBody* body = new btRigidBody(cInfo);
  _dynamicsWorld->addRigidBody(body);
  return body;
}

void PhysicSystem::addCollisionShape(btCollisionShape *collisionShape)
{
  _collisionShapes.push_back(collisionShape);
}
