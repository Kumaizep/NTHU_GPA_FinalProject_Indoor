#version 450

layout(location = 0) out vec4 color0;

uniform int lightMode;

void main()
{
	if (lightMode == 3)
		color0 = vec4(1.0);
	else
		color0 = vec4(vec3(0.0), 1.0);

	// color0 = vec4(1.0);
}
