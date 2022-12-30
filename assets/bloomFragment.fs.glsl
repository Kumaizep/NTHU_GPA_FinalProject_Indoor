#version 450

in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D texture0;
void main()
{
    vec4 result = texture(texture0, texCoords);
    float brightness = dot(result, vec4(0.2126, 0.7152, 0.0722,0));
    if(brightness > 0.9)
        fragColor = result;
    else
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}