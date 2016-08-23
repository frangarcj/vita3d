#pragma once

#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/gxm.h>
#include <png.h>
#include <string>

class Texture
{
public:
  bool	loadFromPng(const std::string & fileName);
  bool	createEmptyTexture(unsigned int width, unsigned int height);
  int	getStride();
  unsigned int getWidth();
  unsigned int getHeight();
  SceGxmTextureFormat getFormat();
  static int formatToBytespp(SceGxmTextureFormat format);
  SceGxmTexture *getGxmTex()
  {
    return &_gxmTex;
  }
  
private:
  bool load(const void *pIo, png_rw_ptr readDataFn);
  static void pngReadFileFn(png_structp pPng, png_bytep data, png_size_t length);  

  SceGxmTexture _gxmTex;
  SceUID	_dataUID;
  SceUID	_paletteUID;
};
