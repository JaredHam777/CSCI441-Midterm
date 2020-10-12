//
// Created by Steven Vennard on 10/11/2020.
//





#include "TrashCar.h"


TrashCar::TrashCar(float x, float y, float z, float camRadius, float topBound, float botBound, float leftBound,
                   float rightBound) {
  _x = x;
  _y = y;
  _z = z;
  _camRadius = camRadius;
  _leftBound = leftBound;
  _topBound = topBound;
  _rightBound = rightBound;
  _botBound = botBound;



};


void TrashCar::setLightingShaderUandA(CSCI441::ShaderProgram &lightingShader, LightingShaderUniforms &lightingShaderUniforms, LightingShaderAttributes &lightingShaderAttributes) {

    _lightingShaderUniforms = lightingShaderUniforms;
    _lightingShaderAttributes = lightingShaderAttributes;
    _shaderProgram = &lightingShader;
}

void TrashCar::drawTrashCar( glm::mat4 viewMtx, glm::mat4 projMtx) {
    _shaderProgram->useProgram();
   // glEnableVertexAttribArray( _lightingShaderAttributes.vPos );
    //glEnableVertexAttribArray(_lightingShaderAttributes.vNorm);
    glm::mat4 modelMtx(1.0f);
    modelMtx = glm::translate( modelMtx, glm::vec3( _x, _y, _z ) );
    modelMtx = glm::rotate( modelMtx,_carAngle, glm::vec3(0,1,0) );
    drawCarBody(modelMtx, viewMtx, projMtx);
    drawCarCab(modelMtx, viewMtx, projMtx);
    drawWheels(modelMtx, viewMtx, projMtx);

}
void TrashCar::drawWheel(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glm::vec3 wheelColor(0.18275, 0.17, 0.225);
    glUniform3fv(_lightingShaderUniforms.materialColor, 1, &wheelColor[0]);

    float shinyness = 3.0;
    glUniform1f(_lightingShaderUniforms.shinyness,shinyness);

    modelMtx = glm::scale( modelMtx, glm::vec3( .5, .5, .5 ) );

   modelMtx = glm::translate( modelMtx, glm::vec3( 0, 1.5, 0 ) );
    modelMtx = glm::rotate( modelMtx,_wheelRoll, glm::vec3(0,0,1) );
    //modelMtx = glm::scale( modelMtx, glm::vec3( 5, 5, 5 ) );


    computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    CSCI441::drawSolidTorus(.8,1.0,3,10);


}
void TrashCar::drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    float rotAngle = 90.0*M_PI/180.0;
    glm::mat4 modelMat1 = glm::translate( modelMtx, glm::vec3( 1.7, 0, 2.8 ) );
    modelMat1 = glm::rotate( modelMat1,rotAngle, glm::vec3(0,1,0) );
    drawWheel(modelMat1, viewMtx, projMtx);


    glm::mat4 modelMat2 = glm::translate( modelMtx, glm::vec3( -1.7, 0, -2.8 ) );
     modelMat2 = glm::rotate( modelMat2,rotAngle, glm::vec3(0,1,0) );
    drawWheel(modelMat2, viewMtx, projMtx);


    glm::mat4 modelMat3 = glm::translate( modelMtx, glm::vec3( 1.7, 0, -2.8 ) );
    modelMat3 = glm::rotate( modelMat3,rotAngle, glm::vec3(0,1,0) );
    drawWheel( modelMat3, viewMtx, projMtx);


    glm::mat4 modelMat4 = glm::translate( modelMtx, glm::vec3( -1.7, 0, 2.8 ) );
    modelMat4 = glm::rotate( modelMat4 ,rotAngle, glm::vec3(0,1,0) );
    drawWheel(modelMat4, viewMtx, projMtx);
}

void TrashCar::drawCarCab(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx){
    glm::vec3 cabColor(0.4, 0.44, 0.89);
    glUniform3fv(_lightingShaderUniforms.materialColor, 1, &cabColor[0]);

    float shinyness = 10;
    glUniform1f(_lightingShaderUniforms.shinyness,shinyness);



    modelMtx = glm::translate( modelMtx, glm::vec3( 0, 1.5+_carBounce, -1 ) );
    modelMtx = glm::scale( modelMtx, glm::vec3( 2.9, 1.5, 3 ) );
    computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    CSCI441::drawSolidCube(1.0f);

}
void TrashCar::drawCarBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx){

    glm::vec3 bodyColor(0.54, 1.0, 0.63);
    glUniform3fv(_lightingShaderUniforms.materialColor, 1, &bodyColor[0]);

    float shinyness = 10;
    glUniform1f(_lightingShaderUniforms.shinyness,shinyness);



    modelMtx = glm::scale( modelMtx, glm::vec3( 3, 1, 6 ) );
    modelMtx = glm::translate( modelMtx, glm::vec3( 0, 1+_carBounce, 0 ) );
    computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    CSCI441::drawSolidCube(1.0f);

}
void TrashCar::updateCar(bool forward, bool backward, bool left, bool right) {
    //makes the car bounce upwards and downwards between -.2 and .2
    if (_carBounce > .2 || _carBounce < -.2){
        _carBounceFlip = -_carBounceFlip;
        _carBounce+=_carBounceFlip;
    }else{
        _carBounce+=_carBounceFlip;
    }

    if (forward == true){
        _z+= .1*cos(_carAngle);
        _x+= .1*sin(_carAngle);
        if (_wheelRoll >= M_PI*2.0){
            _wheelRoll = 0;
        }else{
            _wheelRoll += M_PI/36.0;
        }
    }
    if (backward == true){
        _z-= .1*cos(_carAngle);
        _x-= .1*sin(_carAngle);
        if (_wheelRoll <= 0){
            _wheelRoll = M_PI*2.0;
        }else{
            _wheelRoll -= M_PI/36.0;
        }
    }
    if (left == true){
        _carAngle += M_PI/100.0;
    }
    if (right == true){
        _carAngle -= M_PI/100.0;
    }
    if (_carAngle >= 2.0*M_PI){
        _carAngle = 0;
    }
    if (_carAngle <= -2.0*M_PI){
        _carAngle = 0;
    }
    // std::cout << _x << std::endl;
    if (_x > _topBound){
        _x = _topBound;
    }
    if (_z > _rightBound){
        _z = _rightBound;
    }
    if (_x < _botBound){
        _x = _botBound;
    }
    if (_z < _leftBound){
        _z = _leftBound;
    }

}

void TrashCar::setCamRadius(float newCamRad) {
    _camRadius = newCamRad;
}
float TrashCar::getCamRadius() {
    return _camRadius;
}
float TrashCar::getCarBounce() {
    return _carBounce;
}
float TrashCar::getZ() {
    return _z;
}
float TrashCar::getX() {
    return _x;
}
float TrashCar::getY() {
    return _y;
}
float TrashCar::getCarAngle() {
    return _carAngle;
}


void TrashCar::computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUniformMatrix4fv(_lightingShaderUniforms.modelMtx, 1, GL_FALSE, &modelMtx[0][0]);
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glUniformMatrix4fv(_lightingShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0] );

    //  compute and send the normal matrix to the GPU
    glm::mat3 normalMtx	= glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glUniformMatrix3fv(_lightingShaderUniforms.normMtx,1,GL_FALSE, &normalMtx[0][0]);
}