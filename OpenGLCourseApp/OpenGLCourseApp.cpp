// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

// Creating identity matrices:
// Old style:
// glm::mat4 model;
// New style:
//glm::mat4 model(1.0f)
// or:
//glm::mat4 model = glm::mat4(1.0f)

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecaularIntensity = 0, uniformShininess = 0,
uniformDirectionalLigtTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model xwing;
Model blackhawk;
Model tooth30;
Model sphere;
Model eyeball;
Model airliner;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLdouble deltaTime = 0.0f;
GLdouble lastTime = 0.0f;

GLfloat blackhawkAngle = 0.0f;

const float toRadians = 3.14159265f / 180.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat* vertices, unsigned int verticesCount, unsigned int vLength, unsigned int normalOffset) {
    for (unsigned int i = 0; i < indicesCount; i += 3) {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (unsigned int i = 0; i < verticesCount/ vLength; i++) {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}

void CreateObjects() {
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //  x       y       z       u       v       normal x/y/z
        -1.0f, -1.0f, -0.6f,     0.0f,   0.0f,   0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f,   0.0f,   0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -0.6f,      1.0f,   0.0f,   0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f,   1.0f,   0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.f,   0.0f, 0.0f,        0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.f,    10.0f, 0.0f,       0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,   0.0f, 10.0f,       0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,    10.0f, 10.0f,      0.0f, -1.0f, 0.0f
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    directionalShadowShader = Shader();
    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
    omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    meshList[1]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    meshList[2]->RenderMesh();

    /*
    */
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
    model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    xwing.RenderModel();

    /*
    */
    blackhawkAngle += 0.1f;
    if (blackhawkAngle > 360.0f) {
        blackhawkAngle = 0.1f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
    model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    blackhawk.RenderModel();

    /*
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10.0f, 2.0f, 10.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    tooth30.RenderModel();
    */

    /*
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 5.0f));
    //model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    sphere.RenderModel();
    */

    /*
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 5.0f));
    //model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    eyeball.RenderModel();
    */

    /*
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 5.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecaularIntensity, uniformShininess);
    airliner.RenderModel();
    */
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
    directionalShadowShader.UseShader();

    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    light->GetShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.GetModelLocation();
    glm::mat4 transform = light->CalculateLightTransform();
    directionalShadowShader.SetDirectionalLightTransform(&transform);
    //directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
    omniShadowShader.UseShader();

    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    light->GetShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = omniShadowShader.GetModelLocation();
    uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

    glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
    glUniform1f(uniformFarPlane, light->GetFarPlane());
    omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecaularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    glViewport(0, 0, 1366, 768);

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount);
    shaderList[0].SetSpotLights(spotLights, spotLightCount);
    glm::mat4 transform = mainLight.CalculateLightTransform();
    shaderList[0].SetDirectionalLightTransform(&transform);
    //shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

    mainLight.GetShadowMap()->Read(GL_TEXTURE1);
    shaderList[0].SetTexture(0);
    shaderList[0].SetDirectionalShadowMap(1);

    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.3f;
    //spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

    RenderScene();
}

int main() {
    mainWindow = Window(1366, 768); // Or 1280x1024, 1024,768
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(4.0f, 256);
    dullMaterial = Material(0.3f, 4);

    /*
    */
    xwing = Model();
    xwing.LoadModel("Models/x-wing.obj");

    /*
    */
    blackhawk = Model();
    blackhawk.LoadModel("Models/uh60.obj");

    /*
    tooth30 = Model();
    tooth30.LoadModel("Models/Lower_Right_First_Molar_30_Enamel.obj");
    */

    /*
    sphere = Model();
    sphere.LoadModel("Models/sphere5.obj");
    */

    /*
    eyeball = Model();
    eyeball.LoadModel("Models/eyeball.obj");
    */

    /*
    airliner = Model();
    airliner.LoadModel("Models/11803_Airplane_v1_l1.obj");
    */


    mainLight = DirectionalLight(2048, 2048,
                                1.0f, 1.0f, 1.0f,
                                0.1f, 0.3f,
                                0.0f, -15.0f, -10.0f);

    pointLights[0] = PointLight(1024, 1024,
                                0.01f, 100.0f,
                                0.0f, 0.0f, 1.0f,
                                0.0f, 0.1f,
                                0.0f, 0.0f, 0.0f,
                                0.3f, 0.2f, 0.1f);
    pointLightCount++;
    pointLights[1] = PointLight(1024, 1024,
                                0.01f, 100.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.1f,
                                -4.0f, 2.0f, 0.0f,
                                0.3f, 0.1f, 0.1f);
    pointLightCount++;

    spotLights[0] = SpotLight(1024, 1024,
                                0.01f, 100.0f,
                                1.0f, 1.0f, 1.0f,
                                0.0f, 2.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f);
    spotLightCount++;
    spotLights[1] = SpotLight(1024, 1024,
                                0.01f, 100.0f,
                                1.0f, 1.0f, 1.0f,
                                0.0f, 1.0f,
                                0.0f, 1.5f, 0.0f,
                                -2.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f);
    spotLightCount++;

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat) mainWindow.getBufferWidth()/mainWindow.getBufferHeight(), 0.1f, 100.0f);

    //Assimp::Importer importer; // = Assimp::Importer();

    // Loop until window closed
    while (!mainWindow.getShouldClose()) {
        GLdouble now = glfwGetTime(); // or SDL_GetPerformanceCounter();
        deltaTime = now - lastTime; // and then: (now - lastTime) * 1000/SDL_GetPerformanceFrequence();
        lastTime = now;

        // Get and handle user input events
        glfwPollEvents();

        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        DirectionalShadowMapPass(&mainLight);

        for (size_t i = 0; i < pointLightCount; i++) {
            OmniShadowMapPass(&pointLights[i]);
        }

        for (size_t i = 0; i < spotLightCount; i++) {
            OmniShadowMapPass(&spotLights[i]);
        }

        RenderPass(projection, camera.calculateViewMatrix());

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
