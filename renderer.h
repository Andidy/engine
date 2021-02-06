#ifndef RENDERER_H
#define RENDERER_H

#include "universal.h"

struct VertexBuffer {
	i32 buffer_length;
	i32 num_vertices;
	vec3* positions;
	vec3* normals;
	vec2* texcoords;
};

struct ModelBuffer {
	i32 num_models;
	i32* ptr;
	i32* num_indices;
};

void InitRenderer(VertexBuffer* v_buf, ModelBuffer* models, PermanentResourceAllocator* allocator);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator);

#endif
