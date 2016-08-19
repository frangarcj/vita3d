#pragma once

#include <psp2/gxm.h>
#include <psp2/types.h>
#include <psp2/kernel/sysmem.h>

#define ALIGN(x, a)(((x) + ((a) - 1)) & ~((a) - 1))

void *gpu_alloc(SceKernelMemBlockType type,
		unsigned int size,
		unsigned int alignment,
		unsigned int attribs,
		SceUID *uid);
void gpu_free(SceUID uid);
void *vertex_usse_alloc(unsigned int size, SceUID *uid, unsigned int *usse_offset);
void vertex_usse_free(SceUID uid);
void *fragment_usse_alloc(unsigned int size, SceUID *uid, unsigned int *usse_offset);
void fragment_usse_free(SceUID uid);
void *patcher_host_alloc(void *user_data, unsigned int size);
void patcher_host_free(void *user_data, void *mem);
