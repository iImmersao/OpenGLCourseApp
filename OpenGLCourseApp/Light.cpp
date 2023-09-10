#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f); // Determines how much of the given light elements get shown.
	ambientIntensity = 1.0f;
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat shadowWidth, GLfloat shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensityIn, GLfloat dIntensity) {
	shadowMap = new ShadowMap();
	shadowMap->Init(shadowWidth, shadowHeight);

	colour = glm::vec3(red, green, blue);
	ambientIntensity = ambientIntensityIn;
	diffuseIntensity = dIntensity;
}

Light::~Light() {

}