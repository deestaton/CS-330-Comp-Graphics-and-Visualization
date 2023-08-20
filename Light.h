#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat difIntensity);

	// TODO: change types from GLfloat to GLuint (unsigned int in glew lib)
	void UseLight(GLfloat ambIntensityLocation, GLfloat ambColorLocation, GLfloat difIntensityLocation, GLfloat directionLocation);

	~Light();

private:
	glm::vec3 color;
	GLfloat ambientIntensity;

	glm::vec3 direction; // diffuse lighting depends on the direction light is coming from
	GLfloat diffuseIntensity;
};

