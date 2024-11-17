#version 330 core
#define SMOOTH 0

out vec4 FragColor;
in vec3 pos;

uniform sampler2D tex;

uniform float id;

vec4 color_map(float data){
	
	float p = .3f;
	if (data < 0.166f) {
		return vec4(0.0, 0.0, data * 6, p);
	}
	if (data < 0.333f) {
		return vec4(0.0, (data - 0.1666f) * 6, 1.0f, p);
	}
	if (data < 0.5f) {
		return vec4(0.0, 1.0, 1.0 - (data - 0.33f) * 6, p);
	}
	if (data < 0.666f) {
		return vec4((data - 0.5f) * 6, 1.0, 0.0f, p);
	}
	if (data < 0.8333f) {
		return vec4(1.0, 1.0 - (data - 0.66f) * 6, 0.0, p);
	}
	if (data < .99f) {
		return vec4(1.0 - (data - 0.833f) * 6, 0.0f, 0.0, 0.5);
	}
	if (data < 10.0f) {
		return vec4(1.0 - (data - 0.833f) * 6, 0.0f, 0.0, 0.5);
	}
	return vec4(1.0f, 1.0f, 1.0f, 0.9);
}

void main()
{
	FragColor = color_map( pos.x * 0.2f);
}