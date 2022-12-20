#version 460

layout(location = 0) out vec4 fragColor;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec3 normal;
    vec2 texcoord;
} vertexData;


uniform mat4 um4mv;
uniform mat4 um4p;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ke;
uniform float Ni;
uniform float d;
uniform float Ns;
uniform int illum;

uniform int outputMode;

uniform int textureNumber;
layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
uniform int colorChannel0;
uniform int colorChannel1;
uniform int colorChannel2;

void main()
{
   if (outputMode == 0)
   {
      if (textureNumber > 0)
      {
         if (colorChannel0 > 1)
         {
            fragColor = texture(texture0, vertexData.texcoord);
         }
         else
         {
            fragColor = texture(texture0, vertexData.texcoord).rrra;
         }

         if(fragColor.a < 0.3)
         {
            discard;
         }
      }
      else
      {
         fragColor = vec4(Kd, d);
      }
   }
   else
   {
      fragColor = vec4(vertexData.normal, 1.0);
   }
}
