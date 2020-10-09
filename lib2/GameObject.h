//
// Created by JaredHam777 on 9/23/2020.
//

#ifndef TUTORIALS_GAMEOBJECT_H
#define TUTORIALS_GAMEOBJECT_H

#include "Transform.cpp"
#include <string>
#include "Component.h"
#include <vector>

#include <cstdio>				// for printf functionality
#include <cstdlib>			    // for exit functionality
class GameObject    {
public:
    Transform transform;
    std::string meshType;
    glm::vec3 color;
    void draw();
    GameObject();
    template <class T>
    Component getComponent(T);
    void addComponent(Component component);

private:
    std::vector<Component> components;

};
#endif //TUTORIALS_GAMEOBJECT_H
