//
// Created by JaredHam777 on 9/23/2020.
//
#include "GameObject.h"
#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>
#include <iostream>
#include <string>


    void GameObject::draw() {
        CSCI441::SimpleShader3::pushTransformation( transform.getMatrix());
        CSCI441::SimpleShader3::setMaterialColor(color);
        if(meshType=="cube")    {
            CSCI441::drawSolidCube(1.0);
        }   else if(meshType == "sphere")   {
            CSCI441::drawSolidSphere(1, 16, 16);
        }   else if(meshType == "cylinder") {
            CSCI441::drawSolidCylinder(1,1,1,1,16);
        }   else if(meshType == "torus")    {
            CSCI441::drawSolidTorus(0.5f, 1, 32, 16);
        }   else if(meshType == "disk")    {
            CSCI441::drawSolidDisk(0.5f, 1, 32, 16);
        }   else if(meshType == "cone")    {
            CSCI441::drawSolidCone(1,2,1,8);
        }
        CSCI441::SimpleShader3::popTransformation();
    }
    GameObject::GameObject() {
        transform.revertToDefaultValues();
        meshType = "cube";
        color = glm::vec3(1,1,1);
    }
    template <class T>
    Component GameObject::getComponent(T)   {

        for (Component component : components)  {
            std::cout<<typeid(component).name()<<std::endl;
            std::cout<<typeid(T).name()<<std::endl;
        }

        return  components[0];

    }
    void GameObject::addComponent(Component component)  {
        components.push_back(component);
    }



