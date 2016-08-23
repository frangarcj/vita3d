#pragma once

#include <string>
#include "Mesh.hh"
#include "AssetsManager.hh"

class ObjLoader
{
public:
  bool loadModel(const std::string & filename, std::vector<Mesh> & model, AssetsManager &);

private:
  bool  parseFile(std::stringstream & sstr, std::vector<Mesh> & model);
  void  parse_face(const std::vector<std::string> & tokens, int* v, int *vt, int *n);
};
