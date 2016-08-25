#include "Car.hh"


bool Car::init(ObjLoader & loader, AssetsManager & manager, btDiscreteDynamicsWorld *world)
{
  _position = glm::vec3(0.f, 50.f, 0.f);
  _rotation = glm::vec3(0.f);

  if (!loader.loadModel("app0:res/Mesh/dpv/dpv.obj", _meshes, manager))
    return false;

  _shape = new btBoxShape(btVector3(1, 1, 1));


  btDefaultMotionState* fallMotionState =
    new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
  btScalar mass = 1;
  btVector3 fallInertia(0, 0, 0);
  _shape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,
							   fallMotionState,
							   _shape,
							   fallInertia);
  _rigidBody = new btRigidBody(fallRigidBodyCI);
  world->addRigidBody(_rigidBody);
  
  return true;
}

void Car::update(float elapsedTime, SceCtrlData & pad)
{
  btTransform trans;
  _rigidBody->getMotionState()->getWorldTransform(trans);
  _position.x = trans.getOrigin().getX();
  _position.y = trans.getOrigin().getY();
  _position.z = trans.getOrigin().getZ();
  
  if (pad.buttons & SCE_CTRL_CROSS)
    {
      _rigidBody->applyForce(btVector3(0.f, 0.f, 10.f), btVector3(0.f, 0.f, 0.f));
    }
}

void Car::draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager)
{
  glm::mat4 m(1.0f);

  m = glm::eulerAngleYXZ(_rotation.x, _rotation.y, _rotation.z);
  m = glm::translate(m, _position);

  
  glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
  glm::mat4 mv = camera.getViewMatrix() * m;  
  glm::mat3 n = glm::inverseTranspose(glm::mat3(mv));

  shader.bind(context);
  
  shader.setUniformMat4(VERTEX, "wvp", context, mvp);  
  shader.setUniformMat3(VERTEX, "normalMatrix", context, n);
  shader.setUniformVec3(FRAGMENT, "cameraPosition", context, camera.getPosition());
  shader.setUniformMat4(FRAGMENT, "model", context, m);

  for (auto & m : _meshes)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
        shader.setUniformTexture(context, manager.getTexture(diff));
      m.draw(context);
    }
}

void Car::release(btDiscreteDynamicsWorld *world)
{
  world->removeRigidBody(_rigidBody);
  delete _rigidBody->getMotionState();
  delete _rigidBody;
  
  for (auto & m : _meshes)
    m.release();
}
