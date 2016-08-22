#include <sstream>
#include <iterator>
#include <psp2/io/fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <debugnet.h>
#include "ObjLoader.hh"


std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;

  while(getline(ss, tok, delimiter))
    {
      if (!tok.empty())
	internal.push_back(tok);
    }

  return internal;
}

bool ObjLoader::loadModel(const std::string &fileName, std::vector<Mesh> & model)
{
  SceUID fd = sceIoOpen(fileName.c_str(), SCE_O_RDONLY, 0777);
  if (fd < 0)
    {
      debugNetPrintf(ERROR, (char*)"Error while opening file : %s\nExiting...", fileName.c_str());
      return false;
    }

  char buff[512];
  memset(buff, 0, 512);

  std::stringstream sstr;

  while (sceIoRead(fd, buff, 511) > 0)
    {
      sstr << buff;
      memset(buff, 0, 512);
    }

  sceIoClose(fd);
  
  return parseFile(sstr, model);
}


bool ObjLoader::parseFile(std::stringstream & sstr, std::vector<Mesh> & model)
{
  int numMesh = 0;

  std::vector<glm::vec3> vertices;
  uint16_t index = 0;

  model.push_back(Mesh());
  
  for (std::string line; std::getline(sstr, line);)
    {
      std::vector<std::string> tokens = split(line, ' ');
      
      if (tokens.size() > 0)
	{
	  if (tokens[0] == "o" && tokens.size() == 2)
	    {	      
	      model.push_back(Mesh());
	      numMesh++;
	      index = 0;
	    }
	  
	  if (tokens[0] == "v" && tokens.size() == 4)
	    {
	      float v[3];
	      for (int i = 0; i < 3; i++)
		v[i] = atof(tokens[i + 1].c_str());

	      vertices.push_back(glm::vec3(v[0], v[1], v[2]));	     
	    }

	  if (tokens[0] == "f" && tokens.size() == 4)
	    {
	      int v[3];
	      int vt[3];
	      int n[3];
	      parse_face(tokens, v, vt, n);

	      for (int i = 0; i < 3; i++)
		{
		  glm::vec3 vec = vertices[v[i] - 1];  
		  model.at(numMesh).addVertex(vec);
		  model.at(numMesh).addIndex(index);
		  index++;
		}
	      
	    }
	}     
    }

  for (auto & m : model)
    m.uploadToVram();
  
  return true;
}

void ObjLoader::parse_face(const std::vector<std::string> & tokens, int* v, int *vt, int *n)
{
  for (int i = 0; i < 3; i++)
    {
      std::vector<std::string> tokens_f = split(tokens[i + 1], '/');      
      v[i] = atoi(tokens_f[0].c_str());      
    }  
}
