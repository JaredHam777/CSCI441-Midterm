//
// Created by Steven Vennard on 10/11/2020.
//

#ifndef MP_TRASHCAR_H
#define MP_TRASHCAR_H


// include the OpenGL library headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>			// include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include C and C++ libraries
#include <cmath>				// for cos(), sin() functionality
#include <cstdio>				// for printf functionality
#include <cstdlib>			    // for exit functionality
#include <ctime>			    // for time() functionality
#include <vector>
#include <iostream>

// include our class libraries
#include <CSCI441/OpenGLUtils.hpp>
#include <CSCI441/objects.hpp>  // for our 3D objects
#include <CSCI441/ShaderProgram.hpp>    // a wrapper class for Shader Programs
#include <CSCI441/SimpleShader.hpp>
#include "LightingShaderStructs.h"

class TrashCar {
public:
    TrashCar(float x, float y, float z, float camRadius, float topBound, float botBound, float leftBound,
             float rightBound);


    void setLightingShaderUandA(CSCI441::ShaderProgram &lightingShader, LightingShaderUniforms &lightingShaderUniforms,
                                LightingShaderAttributes &lightingShaderAttributes);

    float getX();

    float getY();

    float getZ();

    void setCamRadius(float newCamRad);

    float getCamRadius();

    float getCarAngle();

    float getCarBounce();

    void drawTrashCar(glm::mat4 viewMtx, glm::mat4 projMtx);

    void updateCar(bool forward, bool backward, bool left, bool right);

private:
    float _x;
    float _y;
    float _z;
    float _camRadius;
    float _wheelRoll = 0;
    float _topBound;
    float _botBound;
    float _leftBound;
    float _rightBound;
    float _carAngle = 0;
    float _carBounce = 0.0;
    float _carBounceFlip = 0.01;
    CSCI441::ShaderProgram *_shaderProgram = nullptr;
    LightingShaderUniforms _lightingShaderUniforms;
    LightingShaderAttributes _lightingShaderAttributes;
    void drawCarBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void drawCarCab(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void drawWheel(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);


};

#endif //MP_TRASHCAR_H
