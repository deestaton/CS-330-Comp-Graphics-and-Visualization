#include "Material.h"

Material::Material() 
{
	specularIntensity = 0;
	shininess = 0;
}

Material::Material(GLfloat specIntensity, GLfloat shine)
{
	specularIntensity = specIntensity;
	shininess = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}

Material::~Material() {}