#include <debugnet.h>
#include <string.h>
#include <stdlib.h>
#include "Texture.hh"
#include "Utils.hh"


#define PNG_SIGSIZE (8)
#define GXM_TEX_MAX_SIZE 4096

bool		Texture::loadFromPng(const std::string &fileName)
{
  png_byte	pngsig[PNG_SIGSIZE];
  SceUID	fd;

  if ((fd = sceIoOpen(fileName.c_str(), SCE_O_RDONLY, 0777)) < 0)
    {
      debugNetPrintf(ERROR, (char*)"Error while opening file %s\n", fileName.c_str());
      return false;
    }

  if (sceIoRead(fd, pngsig, PNG_SIGSIZE) != PNG_SIGSIZE)
    {
      debugNetPrintf(ERROR, (char*)"Invalid file %s\n", fileName.c_str());
      return false;
    }

  load(static_cast<void*>(&fd), pngReadFileFn);
  sceIoClose(fd);
  
  return true;
}

bool Texture::load(const void *pIo, png_rw_ptr readDataFn)
{

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr)
    {
      return false;
    }
  
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
    {
      png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
      return false;
    }
  
  png_bytep *row_ptrs = nullptr;
  
  if (setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
    if (row_ptrs != NULL)
      free(row_ptrs);
    return false;
  }
  
  png_set_read_fn(png_ptr, (png_voidp)pIo, pngReadFileFn);
  png_set_sig_bytes(png_ptr, PNG_SIGSIZE);
  png_read_info(png_ptr, info_ptr);
  
  unsigned int width, height;
  int bit_depth, color_type;
  
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
	       &color_type, NULL, NULL, NULL);
  
  if ((color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
      || (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)
      || (bit_depth == 16))
    {
      png_set_expand(png_ptr);
    }

  if (bit_depth == 16)
    png_set_scale_16(png_ptr);

  if (bit_depth == 8 && color_type == PNG_COLOR_TYPE_RGB)
    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_palette_to_rgb(png_ptr);
      png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }
  
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  if (bit_depth < 8)
    png_set_packing(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  row_ptrs = (png_bytep *)malloc(sizeof(png_bytep) * height);
  if (!row_ptrs)
    {
      png_destroy_info_struct(png_ptr, &info_ptr);
      png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
      return false;
    }

  if (!createEmptyTexture(width, height))
    {
      free(row_ptrs);
      png_destroy_info_struct(png_ptr, &info_ptr);
      png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
      return false;
    }

  void *texture_data = sceGxmTextureGetData(&_gxmTex);
  unsigned int stride = getStride();

  int i;
  for (i = 0; i < height; i++) {
    row_ptrs[i] = (png_bytep)(texture_data + i*stride);
  }

  png_read_image(png_ptr, row_ptrs);

  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
  free(row_ptrs);
  return true;
}

void Texture::pngReadFileFn(png_structp pPng, png_bytep data, png_size_t length)
{
  SceUID fd = *(SceUID*)png_get_io_ptr(pPng);
  sceIoRead(fd, data, length);
}

int	Texture::getStride()
{
  return ((getWidth() + 7) & ~ 7)
    * formatToBytespp(getFormat());
}

unsigned int Texture::getWidth()
{
  return sceGxmTextureGetWidth(&_gxmTex);
}

unsigned int Texture::getHeight()
{
  return sceGxmTextureGetHeight(&_gxmTex);
}

SceGxmTextureFormat Texture::getFormat()
{
  return sceGxmTextureGetFormat(&_gxmTex);
}

int Texture::formatToBytespp(SceGxmTextureFormat format)
{
  switch (format & 0x9f000000U) {
  case SCE_GXM_TEXTURE_BASE_FORMAT_U8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_P8:
    return 1;
  case SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4:
  case SCE_GXM_TEXTURE_BASE_FORMAT_U8U3U3U2:
  case SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5:
  case SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6:
  case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8:
    return 2;
  case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8:
    return 3;
  case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8:
  case SCE_GXM_TEXTURE_BASE_FORMAT_F32:
  case SCE_GXM_TEXTURE_BASE_FORMAT_U32:
  case SCE_GXM_TEXTURE_BASE_FORMAT_S32:
  default:
    return 4;
  }
}

bool   Texture::createEmptyTexture(unsigned int w, unsigned int h)
{
  SceGxmTextureFormat format = SCE_GXM_TEXTURE_FORMAT_A8B8G8R8;
  
  if (w > GXM_TEX_MAX_SIZE || h > GXM_TEX_MAX_SIZE)
    return false;

  const int tex_size =  w * h * formatToBytespp(format);

  /* Allocate a GPU buffer for the texture */
  void *texture_data = gpu_alloc(
				 SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
				 tex_size,
				 SCE_GXM_TEXTURE_ALIGNMENT,
				 SCE_GXM_MEMORY_ATTRIB_READ,
				 &_dataUID);

  if (!texture_data)
    {
      return false;
    }

  /* Clear the texture */
  memset(texture_data, 0, tex_size);

  /* Create the gxm texture */
  sceGxmTextureInitLinear(
			  &_gxmTex,
			  texture_data,
			  format,
			  w,
			  h,
			  0);
  
  if ((format & 0x9f000000U) == SCE_GXM_TEXTURE_BASE_FORMAT_P8)
    {      
      const int pal_size = 256 * sizeof(uint32_t);
      
      void *texture_palette = gpu_alloc(
					SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
					pal_size,
					SCE_GXM_PALETTE_ALIGNMENT,
					SCE_GXM_MEMORY_ATTRIB_READ,
					&_paletteUID);
      
      if (!texture_palette)
      {
	_paletteUID = 0;
	return false;
      }
    
    memset(texture_palette, 0, pal_size);    
    sceGxmTextureSetPalette(&_gxmTex, texture_palette);
    }
  else
    {
      _paletteUID = 0;
    }
  
  return true;
}

SceGxmTexture *Texture::getGxmTex()
{
  return &_gxmTex;
}

void	Texture::release()
{
  if (_paletteUID)
    gpu_free(_paletteUID);
  gpu_free(_dataUID);
}
