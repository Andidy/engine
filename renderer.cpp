#include "renderer.h"

void InitRenderer(Image* screen_buffer, i32 screen_width, i32 screen_height, VertexBuffer* v_buf, ModelBuffer* models, PermanentResourceAllocator* allocator) {
	screen_buffer->width = screen_width;
	screen_buffer->height = screen_height;
	screen_buffer->data = (uchar*)allocator->Allocate(sizeof(uchar) * 4 * screen_width * screen_height);
	
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









void DrawLine(ivec2 start, ivec2 end, Image* screen, Color color) {
	if ((start.x == end.x) && (start.y == end.y)) {
		return;
	}

	bool steep = false;
	if (abs(start.x - end.x) < abs(start.y - end.y)) {
		i32 temp = start.x;
		start.x = start.y;
		start.y = temp;

		temp = end.x;
		end.x = end.y;
		end.y = temp;

		steep = true;
	}

	if (start.x > end.x) {
		i32 temp = start.x;
		start.x = end.x;
		end.x = temp;

		temp = start.y;
		start.y = end.y;
		end.y = temp;
	}

	for (i32 x = start.x; x <= end.x; x++) {
		f32 t = (x - start.x) / (f32)(end.x - start.x);
		i32 y = (i32)((f32)start.y * (1.0f - t) + ((f32)end.y * t));
		if (steep) {
			i32 pixel_index = 4 * (y + screen->width * (screen->height - x));
			screen->data[pixel_index + 0] = color.b;
			screen->data[pixel_index + 1] = color.g;
			screen->data[pixel_index + 2] = color.r;
			screen->data[pixel_index + 3] = color.a;
		}
		else {
			i32 pixel_index = 4 * (x + screen->width * (screen->height - y));
			screen->data[pixel_index + 0] = color.b;
			screen->data[pixel_index + 1] = color.g;
			screen->data[pixel_index + 2] = color.r;
			screen->data[pixel_index + 3] = color.a;
		}
	}
}

vec3 Barycentric(vec3 p0, vec3 p1, vec3 p2, vec3 P) {
	vec3 temp0 = Vec3(p2.x - p0.x, p1.x - p0.x, p0.x - P.x);
	vec3 temp1 = Vec3(p2.y - p0.y, p1.y - p0.y, p0.y - P.y);
	vec3 u = Cross(temp0, temp1);

	// degenerate triangle
	if (fabsf(u.z) > 0.001f) 
		return Vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

	return Vec3(-1, -1, -1);
}

void DrawTriangle(vec3* pts, f32* zbuffer, Image* screen, Color color) {
	vec2 bboxmin = Vec2(FLT_MAX, FLT_MAX);
	vec2 bboxmax = Vec2(-FLT_MAX, -FLT_MAX);
	vec2 clamping_vec = Vec2((f32)screen->width - 1, (f32)screen->height - 1);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin.data[j] = (i32)floatmax(0.0f, floatmin(bboxmin.data[j], pts[i].data[j]));
			bboxmax.data[j] = floatmin(clamping_vec.data[j], floatmax(bboxmax.data[j], pts[i].data[j]));
		}
	}

	vec3 P = Vec3(0, 0, 0);
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x+=1) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y+=1) {
			vec3 bc_screen = Barycentric(pts[0], pts[1], pts[2], P);

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

			P.z = 0;
			for (int i = 0; i < 3; i++) {
				P.z += pts[i].z * bc_screen.data[i];
			}

			if (zbuffer[(i32)(P.x + P.y * screen->width)] < P.z) {
				zbuffer[(i32)(P.x + P.y * screen->width)] = P.z;

				i32 pixel_index = 4 * (i32)(P.x + screen->width * (screen->height - P.y));
				screen->data[pixel_index + 0] = color.b;
				screen->data[pixel_index + 1] = color.g;
				screen->data[pixel_index + 2] = color.r;
				screen->data[pixel_index + 3] = color.a;
			}
		}
	}
}

f32 helper(f32 v) {
	return (v + 1.0f) / 2.0f;
}

void Render(Image* screen, f32* z_buffer, VertexBuffer* vertex_buffer, ModelBuffer* model_buffer) {
	vec3 light_dir = Vec3(0.0f, 0.0f, -1.0f);

	// mat4 proj = PerspectiveMat(45, (window_width / window_height), 0.1, 1000);
	// mat4 view = LookAtMat(Vec3(0, 0, -1), ZeroVec(), UpVec());

	for (int model = 0; model < model_buffer->num_models; model++) {
		for (int i = 0; i < model_buffer->length[model]; i += 3) {
			vec3 screen_coords[3];
			vec3 world_coords[3];
			for (int j = 0; j < 3; j++) {
				vec3 temp = vertex_buffer->positions[model_buffer->ptr[model] + i + j];

				if (model == 0) {
					temp.x += 2;
					temp.y += 1;
				}
				else if (model == 1) {
					temp.x += 4;
					temp.y += 4;
				}

				world_coords[j] = temp;
				screen_coords[j] = Vec3(helper(world_coords[j].x) * (f32)screen->width / 4.0f, helper(world_coords[j].y) * (f32)screen->height / 4.0f, world_coords[j].z);
			}

			vec3 n = Cross(SubVec(world_coords[2], world_coords[0]), SubVec(world_coords[1], world_coords[0]));
			n = NormVec(n);

			float intensity = Dot(n, light_dir);
			if (intensity > 0) {
				Color rand_color;
				rand_color.b = (uchar)(intensity * 255);
				rand_color.g = (uchar)(intensity * 255);
				rand_color.r = (uchar)(intensity * 255);
				rand_color.a = 255;
				DrawTriangle(screen_coords, z_buffer, screen, rand_color);
			}
		}
	}
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


	i32 index = v_buffer->num_vertices;
	for (fi = 0; fi < num_vertices_and_indices; fi++) { // compose vertices 

		v_buffer->positions[index + 0] = vertices[faces[fi].f[0].v];
		v_buffer->normals[index + 0] = normals[faces[fi].f[0].n];
		v_buffer->texcoords[index + 0] = texcoords[faces[fi].f[0].t];

		v_buffer->positions[index + 1] = vertices[faces[fi].f[1].v];
		v_buffer->normals[index + 1] = normals[faces[fi].f[1].n];
		v_buffer->texcoords[index + 1] = texcoords[faces[fi].f[1].t];

		v_buffer->positions[index + 2] = vertices[faces[fi].f[2].v];
		v_buffer->normals[index + 2] = normals[faces[fi].f[2].n];
		v_buffer->texcoords[index + 2] = texcoords[faces[fi].f[2].t];

		index += 3;
	}

	v_buffer->num_vertices += num_vertices_and_indices;

	allocator->Free();
}