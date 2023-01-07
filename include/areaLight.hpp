#ifndef AREALIGHT_HPP
#define AREALIGHT_HPP

#include "common.hpp"
#include "shader.hpp"
#include "LTCMatrix.hpp"

struct VertexAL {
	vec3 position;
	vec3 normal;
	vec2 texcoord;
};

static const VertexAL areaLightVertices[6] = {
	{ {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // 0 1 5 4
	{ { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
	{ { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
	{ {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
	{ { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
	{ {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} }
};

class AreaLight
{
public:
	vec3 LIGHT_COLOR = vec3(0.8, 0.6, 0.0);
	vec3 translateVec;
	vec3 rotateVec;

	AreaLight();

	void initialization(vec3 t, vec3 r);

	void setTranslate(vec3 val);

	void setRotate(vec3 val);

	void drawModel(Shader& shader);

	void drawSetup(Shader& shader, GLuint unit1,GLuint unit2);

private:
	GLuint areaLightVBO, areaLightVAO;
	GLuint LTCTO1, LTCTO2;
	float roughness;
	float intensity;
	mat4 rotateMatrix;
	mat4 modelMatrix;
	vec3 vertexPosition[4];

	void updateModel();

	void setupAreaLightbuffer();

	GLuint loadMTexture();

	GLuint loadLUTTexture();

	void incrementRoughness(float val);

	void incrementLightIntensity(float val);
};

#endif
