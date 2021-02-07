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
	i32 buffer_length;
	i32* ptr;
	i32* length;
};

struct Color {
	u8 r, g, b, a;
};

struct Image {
	i32 width;
	i32 height;
	uchar* data;
};

void InitRenderer(Image* screen_buffer, i32 screen_width, i32 screen_height, VertexBuffer* v_buf, ModelBuffer* models, PermanentResourceAllocator* allocator);

void OutputDebugFromRenderer(char* string);
void Render(Image* screen, f32* z_buffer, VertexBuffer* vertex_buffer, ModelBuffer* model_buffer);

void LoadOBJ(char* filename, VertexBuffer* v_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator);

#endif
