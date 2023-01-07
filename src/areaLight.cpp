#include "areaLight.hpp"

AreaLight::AreaLight(){}

void AreaLight::initialization(vec3 t, vec3 r)
{
	translateVec = t;
	rotateVec = r;
	updateModel();

    LTCTO1 = loadMTexture();
    LTCTO2 = loadLUTTexture();

	incrementRoughness(0.5f);
	incrementLightIntensity(1.0f);

	setupAreaLightbuffer();
}

void AreaLight::setTranslate(vec3 val)
{
	if (translateVec == val)
		return;
	translateVec = val;
	updateModel();
}
void AreaLight::setRotate(vec3 val)
{
	if (rotateVec == val)
		return;
	rotateVec = val;
	updateModel();
}


void AreaLight::drawModel(Shader& shader)
{
	shader.setMat4("um4m", modelMatrix);
	glBindVertexArray(areaLightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void AreaLight::drawSetup(Shader& shader, GLuint unit1,GLuint unit2)
{
    shader.setVec3("areaLight.translate", translateVec);
	shader.setVec3("areaLight.points[0]", areaLightVertices[0].position);
    shader.setVec3("areaLight.points[1]", areaLightVertices[1].position);
    shader.setVec3("areaLight.points[2]", areaLightVertices[4].position);
    shader.setVec3("areaLight.points[3]", areaLightVertices[5].position);
    shader.setVec3("areaLight.color", LIGHT_COLOR);
    shader.setFloat("areaLight.intensity", intensity);
    shader.setFloat("areaLight.roughness", roughness);
    shader.setMat4("areaLight.rotateMat", rotateMatrix);

	glActiveTexture(GL_TEXTURE0 + unit1);
    glBindTexture(GL_TEXTURE_2D, LTCTO1);
    glActiveTexture(GL_TEXTURE0 + unit2);
    glBindTexture(GL_TEXTURE_2D, LTCTO2);
}

void AreaLight::updateModel()
{
	mat4 translateMatrix = translate(mat4(1.0f), translateVec);
	mat4 rotateXMatrix   = rotate(mat4(1.0f), radians(rotateVec.x), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotateYMatrix   = rotate(mat4(1.0f), radians(rotateVec.y), vec3(0.0f, 1.0f, 0.0f));
	mat4 rotateZMatrix   = rotate(mat4(1.0f), radians(rotateVec.z), vec3(0.0f, 0.0f, 1.0f));
	rotateMatrix    = rotateYMatrix * rotateZMatrix * rotateXMatrix;
	modelMatrix = translateMatrix * rotateMatrix;
}


void AreaLight::setupAreaLightbuffer()
{
    // AREA LIGHT
    glGenVertexArrays(1, &areaLightVAO);
    glBindVertexArray(areaLightVAO);

    glGenBuffers(1, &areaLightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, areaLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(areaLightVertices), areaLightVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

GLuint AreaLight::loadMTexture()
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint AreaLight::loadLUTTexture()
{
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void AreaLight::incrementRoughness(float val)
{
	roughness = val;
	roughness = glm::clamp(roughness, 0.0f, 1.0f);
}

void AreaLight::incrementLightIntensity(float val)
{
	intensity = val;
	intensity = glm::clamp(intensity, 0.0f, 10.0f);
}