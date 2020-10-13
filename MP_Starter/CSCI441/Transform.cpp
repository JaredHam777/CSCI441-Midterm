//
// Created by JaredHam777 on 9/23/2020.
//

#include "Transform.h"


    void Transform::Rotate(glm::vec3 eulers)   {

    }
    void Transform::updateMatrix() {
        glm::mat4 scaleMtx = glm::scale(glm::mat4(1), scale);
        glm::mat4 rotationMtx = glm::toMat4(rotation);
        glm::mat4 translationMtx = glm::translate(glm::mat4(1), position);
        matrix = (translationMtx * scaleMtx) * rotationMtx;
        if(parent != nullptr) {
            matrix = parent->matrix * matrix;
        }
    }
    glm::mat4 Transform::getMatrix()    {
        updateMatrix();
        return matrix;
    }
    void Transform::revertToDefaultValues()    {
        position = glm::vec3(0,0,0);
        rotation = glm::quat(0,0,0,1);
        scale = glm::vec3(1,1,1);
        parent = nullptr;
    }

    glm::vec3 Transform::eulerAngles()    {
            glm::vec3 angles;
            // roll (x-axis rotation)
            float sinr_cosp = 2 * (rotation.w * rotation.x + rotation.y * rotation.z);
            float cosr_cosp = 1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y);
            angles.z = std::atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            float sinp = 2 * (rotation.w * rotation.y - rotation.z * rotation.x);
            if (std::abs(sinp) >= 1)
                angles.y = std::copysign(float(M_PI) / 2, sinp); // use 90 degrees if out of range
            else
                angles.y = std::asin(sinp);

            // yaw (z-axis rotation)
            float siny_cosp = 2 * (rotation.w * rotation.z + rotation.x * rotation.y);
            float cosy_cosp = 1 - 2 * (rotation.y * rotation.y + rotation.z * rotation.z);
            angles.x = std::atan2(siny_cosp, cosy_cosp);

            return angles;
    }

    void Transform::setRotation(glm::vec3 angles)   {
        rotation = Transform::toQuaternion(angles.x, angles.y, angles.z);
    }

    glm::quat Transform::toQuaternion(float yaw, float pitch, float roll) // yaw (Z), pitch (Y), roll (X)
    {
    // Abbreviations for the various angular functions
        float cy = glm::cos(yaw * 0.5);
        float sy = glm::sin(yaw * 0.5);
        float cp = glm::cos(pitch * 0.5);
        float sp = glm::sin(pitch * 0.5);
        float cr = glm::cos(roll * 0.5);
        float sr = glm::sin(roll * 0.5);
        glm::quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
    return q;
    }

    glm::vec3 Transform::forward() {
        glm::vec3 v;
        float x = rotation.x;
        float y = rotation.y;
        float z = rotation.z;
        float w = rotation.w;
        v.x = 2 * (x*z + w*y);
        v.y = 2 * (y*z - w*x);
        v.z = 1 - 2 * (x*x + y*y);
        return v;

    }
    glm::vec3 Transform::up()   {
        glm::vec3 v;
        float x = rotation.x;
        float y = rotation.y;
        float z = rotation.z;
        float w = rotation.w;
        v.x = 2 * (x*y - w*z);
        v.y = 1 - 2 * (x*x + z*z);
        v.z = 2 * (y*z + w*x);
        return v;
    }
    glm::vec3 Transform::left()  {
        glm::vec3 v;
        float x = rotation.x;
        float y = rotation.y;
        float z = rotation.z;
        float w = rotation.w;
        v.x = 1 - 2 * (y*y + z*z);
        v.y = 2 * (x*y + w*z);
        v.z = 2 * (x*z - w*y);
        return v;
    }