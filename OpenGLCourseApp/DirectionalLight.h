#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
    DirectionalLight();

    DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
        GLfloat red, GLfloat green, GLfloat blue,
        GLfloat ambientIntensity, GLfloat dIntensity,
        GLfloat xDir, GLfloat yDir, GLfloat zDir);

    void UseLight(GLint ambientIntensityLocation, GLint ambientColourLocation,
        GLint diffuseIntensityLocation, GLint directionLocation);

    glm::mat4 CalculateLightTransform();

    ~DirectionalLight();

private:
    glm::vec3 direction;
};

