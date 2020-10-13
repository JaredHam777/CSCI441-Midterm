//
// Created by ssvjr on 10/11/2020.
//

#ifndef MP_LIGHTINGSHADERSTRUCTS_H
#define MP_LIGHTINGSHADERSTRUCTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>			// include GLFW framework header

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightingShaderUniforms {         // stores the locations of all of our shader uniforms
    // add variables to store the new uniforms that were created
    GLint mvpMatrix;
    GLint materialColor;
    GLint normMtx;
    GLint directLightDir;
    GLint camPos;
    GLint lightColorDirect;
    GLint lightColorPoint;
    GLint lightColorSpotLight;
    GLint pointLightPos;
    GLint spotLightPos;
    GLint spotLightAngle;
    GLint spotLightFallOffAngle;
    GLint spotLightVec;
    GLint shinyness;
    GLint abcDropoff;
    GLint modelMtx;

};
struct LightingShaderAttributes {       // stores the locations of all of our shader attributes
    // add variables to store the new attributes that were created
    GLint vPos;
    GLint vNorm;
};
#endif //MP_LIGHTINGSHADERSTRUCTS_H
