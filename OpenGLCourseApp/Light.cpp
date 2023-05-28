#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f); // Determines how much of the given light elements get shown.
	ambientIntensity = 1.0f;

	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensityIn,
	GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity) {

	colour = glm::vec3(red, green, blue);
	ambientIntensity = ambientIntensityIn;

	direction = glm::vec3(xDir, yDir, zDir);
	diffuseIntensity = dIntensity;
}

void Light::UseLight(GLint ambientIntensityLocation, GLint ambientColourLocation,
	GLint diffuseIntensityLocation, GLint directionLocation) {

	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light() {

}