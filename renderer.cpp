#include "renderer.h"

void InitRenderer(VertexBuffer* v_buf, ModelBuffer* models, PermanentResourceAllocator* allocator) {
	v_buf->buffer_length = 1000000;
	v_buf->num_vertices = 0;
	v_buf->positions = (vec3*)allocator->Allocate(sizeof(vec3) * v_buf->buffer_length);
	v_buf->normals = (vec3*)allocator->Allocate(sizeof(vec3) * v_buf->buffer_length);
	v_buf->texcoords = (vec2*)allocator->Allocate(sizeof(vec2) * v_buf->buffer_length);

	models->buffer_length = 100;
	models->num_models = 0;
	models->ptr = (i32*)allocator->Allocate(sizeof(i32) * models->buffer_length);
	models->length = (i32*)allocator->Allocate(sizeof(i32) * models->buffer_length);
}

struct FaceInt3 {
	int v, n, t;
};

struct Face {
	FaceInt3 f[3];
};

void LoadOBJ(char* filename, VertexBuffer* v_buffer, ModelBuffer* m_buffer, PermanentResourceAllocator* allocator) {
	debug_ReadFileResult obj_file = debug_ReadFile(filename);

	uchar* ptr = (uchar*)obj_file.data;


	m_buffer->ptr[m_buffer->num_models] = v_buffer->num_vertices;


	i32 num_vertices_and_indices = 0;
	u64 i = 0;
	while (i < obj_file.size) {
		if (ptr[i] == 'f') {
			num_vertices_and_indices += 1;
		}
		while (ptr[i] != '\n') { i += 1; }
		i += 1;
	}
	num_vertices_and_indices *= 3;

	m_buffer->length[m_buffer->num_models] = num_vertices_and_indices;
	m_buffer->num_models += 1;

	// allocate temporary space
	vec3* vertices = (vec3*)allocator->Allocate(sizeof(vec3) * num_vertices_and_indices);
	vec3* normals = (vec3*)allocator->Allocate(sizeof(vec3) * num_vertices_and_indices);
	vec2* texcoords = (vec2*)allocator->Allocate(sizeof(vec2) * num_vertices_and_indices);

	Face* faces = (Face*)allocator->Allocate(sizeof(Face) * num_vertices_and_indices);

	i32 vi = 0;
	i32 ni = 0;
	i32 ti = 0;
	i32 fi = 0;

	i = 0;
	while (i < obj_file.size) {
		if (ptr[i] == '#' || ptr[i] == ' ' || ptr[i] == '\n') { // skip lines which have the comment operator #, and skip blank lines
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 'm') { // we ignore materials for now
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 's' || ptr[i] == 'o' || ptr[i] == 'g' || ptr[i] == 'u') { // ignore soft shading, objects, groups, and usematerial for now
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
		}
		else if (ptr[i] == 'v') { // we found vertex information
			i += 1;

			if(ptr[i] == ' ') { // we found a vertex
				while (ptr[i] == ' ') { i += 1; }
				vertices[vi].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				vertices[vi].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				vertices[vi].z = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				vi += 1;
			}
			else if (ptr[i] == 't') { // we found a texcoord
				i += 1;
				while (ptr[i] == ' ') { i += 1; }
				texcoords[ti].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				texcoords[ti].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				ti += 1;
			}
			else if (ptr[i] == 'n') { // we found a normal
				i += 1;
				while (ptr[i] == ' ') { i += 1; }
				normals[ni].x = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				normals[ni].y = (float)atof((char*)&ptr[i]);
				while (ptr[i] != ' ') {
					i += 1;
				}
				i += 1;
				normals[ni].z = (float)atof((char*)&ptr[i]);
				while (ptr[i] != '\n') {
					i += 1;
				}
				i += 1;

				ni += 1;
			}
			else if (ptr[i] == 'p') { // ignore parametric curve stuff
				while (ptr[i] != '\n') { i += 1; }
				i += 1;
			}
		}
		else if (ptr[i] == 'f') { // faces
			i += 2;

			faces[fi].f[0].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[0].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[0].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != ' ') { i += 1; }
			i += 1;

			faces[fi].f[1].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[1].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[1].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != ' ') { i += 1; }
			i += 1;

			faces[fi].f[2].v = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[2].t = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '/') { i += 1; }
			i += 1;
			faces[fi].f[2].n = atoi((char*)&ptr[i]) - 1;
			while (ptr[i] != '\n') { i += 1; }
			i += 1;
			fi += 1;
		}
	}


	for (fi = v_buffer->num_vertices; fi < v_buffer->num_vertices + num_vertices_and_indices; fi++) { // compose vertices 
		int index = 3 * fi;

		v_buffer->positions[index + 0] = vertices[faces[fi].f[0].v];
		v_buffer->normals[index + 0] = normals[faces[fi].f[0].n];
		v_buffer->texcoords[index + 0] = texcoords[faces[fi].f[0].t];

		v_buffer->positions[index + 1] = vertices[faces[fi].f[1].v];
		v_buffer->normals[index + 1] = normals[faces[fi].f[1].n];
		v_buffer->texcoords[index + 1] = texcoords[faces[fi].f[1].t];

		v_buffer->positions[index + 2] = vertices[faces[fi].f[2].v];
		v_buffer->normals[index + 2] = normals[faces[fi].f[2].n];
		v_buffer->texcoords[index + 2] = texcoords[faces[fi].f[2].t];
	}

	v_buffer->num_vertices += num_vertices_and_indices;
}