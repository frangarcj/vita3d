#include "ScreenClearer.hh"

bool ScreenClearer::initialize(SceGxmContext *context, SceGxmShaderPatcher *patcher)
  {       
    _pIndices = static_cast<uint16_t*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
							     3 * sizeof(uint16_t),
							     2,
							     SCE_GXM_MEMORY_ATTRIB_READ,
							     &_indicesUid));

    _pVertices = static_cast<glm::vec3*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
						   3 * sizeof(glm::vec3),
						   4,
						   SCE_GXM_MEMORY_ATTRIB_READ,
						   &_verticesUid));

    _pIndices[0] = 0;
    _pIndices[1] = 1;
    _pIndices[2] = 2;
    
    _pVertices[0] = glm::vec3(-1.0f, -1.0f, 1.0f);
    _pVertices[1] = glm::vec3(3.0f, -1.0f, 1.0f);
    _pVertices[2] = glm::vec3(-1.0f, 3.0f, 1.0f);


    
    _vertexProgramGxp = ShaderFactory::readShader("app0:res/Shaders/clear_v.gxp");
    _fragmentProgramGxp = ShaderFactory::readShader("app0:res/Shaders/clear_f.gxp");


    int err = sceGxmProgramCheck(_vertexProgramGxp);
    debugNetPrintf(INFO, (char *)"sceGxmProgramCheck(vertex) : %#x\n", err);
    if (err != SCE_OK)
      return false;

    err = sceGxmProgramCheck(_fragmentProgramGxp);
    debugNetPrintf(INFO, (char *)"sceGxmProgramCheck(fragment) : %#x\n", err);
    if (err != SCE_OK)
      return false;

    err = sceGxmShaderPatcherRegisterProgram(patcher,
					     _vertexProgramGxp,
					     &_vertexProgramId);
    debugNetPrintf(INFO, (char *)"sceGxmShaderPatcherRegisterProgram : %#x\n", err);
    if (err != SCE_OK)
      return false;
  
    err = sceGxmShaderPatcherRegisterProgram(patcher,
					   _fragmentProgramGxp,
					   &_fragmentProgramId);
    debugNetPrintf(INFO, (char *)"sceGxmShaderPatcherRegisterProgram : %#x\n", err);
    if (err != SCE_OK)
      return false;
    
    const SceGxmProgramParameter *posAttribute =
      sceGxmProgramFindParameterByName(_vertexProgramGxp, "aPosition");
  
    _vertexAttribute[0].streamIndex = 0;
    _vertexAttribute[0].offset = 0;
    _vertexAttribute[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
    _vertexAttribute[0].componentCount = 3;
    _vertexAttribute[0].regIndex = sceGxmProgramParameterGetResourceIndex(posAttribute);
  
    _vertexStream[0].stride = sizeof(glm::vec3);
    _vertexStream[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
  
    SceGxmBlendInfo blend_info;  
    blend_info.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
    blend_info.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
    blend_info.colorSrc  = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
    blend_info.colorDst  = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_info.alphaSrc  = SCE_GXM_BLEND_FACTOR_ONE;
    blend_info.alphaDst  = SCE_GXM_BLEND_FACTOR_ZERO;
    blend_info.colorMask = SCE_GXM_COLOR_MASK_ALL;
  
    err = sceGxmShaderPatcherCreateVertexProgram(patcher,
						 _vertexProgramId,
						 _vertexAttribute,
						 1,
						 _vertexStream,
						 1,
						 &_vertexProgram);
    debugNetPrintf(INFO, (char*)"sceGxmShaderPatcherCreateVertexProgram : %#x\n", err);
    if (err != SCE_OK)
      return false;
    
    err = sceGxmShaderPatcherCreateFragmentProgram(patcher,
						   _fragmentProgramId,
						   SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
						   SCE_GXM_MULTISAMPLE_NONE,
						   &blend_info,
						   _vertexProgramGxp,
						   &_fragmentProgram);
    
    debugNetPrintf(INFO, (char*)"sceGxmShaderPatcherCreateFragmentProgram : %#x\n", err);
    if (err != SCE_OK)
      return false;
    /*
    _clearColorUniform = sceGxmProgramFindParameterByName(_fragmentProgramGxp, "uClearColor");
    debugNetPrintf(INFO,
		   (char*)"_clearColorUniform : %d\n",
		   sceGxmProgramParameterGetCategory(_clearColorUniform) ==
		   SCE_GXM_PARAMETER_CATEGORY_UNIFORM);

    _clearColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    */    
    return true;
  }
  
bool  ScreenClearer::release(SceGxmShaderPatcher *shaderPatcher)
{
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _fragmentProgramId);
  sceGxmShaderPatcherUnregisterProgram(shaderPatcher, _vertexProgramId);
  
  gpu_free(_verticesUid);
  gpu_free(_indicesUid);
  
  return true;
}

void  ScreenClearer::clear(SceGxmContext *context)
{
  sceGxmSetVertexProgram(context, _vertexProgram);
  sceGxmSetFragmentProgram(context, _fragmentProgram);
  
  /*
    void *buff;
    sceGxmReserveFragmentDefaultUniformBuffer(context, &buff);
    sceGxmSetUniformDataF(buff, _clearColorUniform, 0, 4, glm::value_ptr(_clearColor));
  */    
  sceGxmSetVertexStream(context, 0, _pVertices); 
  sceGxmDraw(context,
	     SCE_GXM_PRIMITIVE_TRIANGLES,
	     SCE_GXM_INDEX_FORMAT_U16,
	     _pIndices,
	     3);
}
