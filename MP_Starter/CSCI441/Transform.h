//
// Created by JaredHam777 on 9/23/2020.
//
// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#ifndef TUTORIALS_TRANSFORM_H
#define TUTORIALS_TRANSFORM_H
class Transform {
public:
    Transform *parent;
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 scale;
    glm::mat4 getMatrix();
    void Rotate(glm::vec3 eulers);
    void revertToDefaultValues();
    glm::vec3 eulerAngles();
    void setRotation(glm::vec3 angles);
    static glm::quat toQuaternion(float yaw, float pitch, float roll);
    glm::vec3 forward();
    glm::vec3 left();
    glm::vec3 up();
    void updateMatrix();
private:
    glm::mat4 matrix;


};
#endif //TUTORIALS_TRANSFORM_H
