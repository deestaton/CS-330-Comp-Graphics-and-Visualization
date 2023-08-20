#include "Light.h"

Light::Light()
{
	color = glm::vec3(1.0f, 0.0f, 0.0f); // Tells us how much of the color in each pixel should be shown
	ambientIntensity = 0.0f; // brightness

	direction = glm::vec3(1.0f, 0.0f, -1.0f);
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat difIntensity)
{
	color = glm::vec3(red, green, blue);
	ambientIntensity = ambIntensity;

	direction = glm::vec3(xDir, yDir, zDir);
	diffuseIntensity = difIntensity;
}

void Light::UseLight(GLfloat ambIntensityLocation, GLfloat ambColorLocation, GLfloat difIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambColorLocation, color.x, color.y, color.z);
	glUniform1f(ambIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(difIntensityLocation, diffuseIntensity);
}

Light::~Light()
{
}