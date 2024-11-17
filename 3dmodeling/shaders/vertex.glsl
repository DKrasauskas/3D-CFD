
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float col;
out vec3 pos;

uniform float scale = 1.0f;
uniform float x = 0.0f;
uniform float y = 0.0f;

uniform vec3 offset;
uniform mat4 projection;
uniform mat4 view_direction;
uniform mat4 model;
uniform vec3 camera_global;

void main() {

	vec3 data = 0.5080 * vec3((aPos.x), (aPos.y), (aPos.z));
	data += camera_global;
	gl_Position = projection * view_direction * vec4(data, 1.0);
	pos = vec3(1000, 0, 0);/// color_map(lininterp(data[gl_VertexID]));
}