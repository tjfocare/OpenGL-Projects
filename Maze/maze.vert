#version 330

layout (location = 0) in vec3 a_vertex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 pos;

void main(void)
{
	pos = vec4(a_vertex, 1.0);

    // get transformed coordinates and send to frag shader
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
