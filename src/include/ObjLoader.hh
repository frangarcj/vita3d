#pragma once

#include <string>
#include "Mesh.hh"

class ObjLoader
{
public:
  bool loadModel(const std::string & filename, std::vector<Mesh> & model);
};
