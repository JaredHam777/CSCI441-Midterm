
#version 410 core


// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat4 modelMtx;

// the normal matrix
uniform mat3 normMtx;

//the point light positions
uniform vec3 pointLightPos1;
uniform vec3 pointLightPos2;

//cam position
uniform vec3 camPos;


// the direction the incident ray of light is traveling
//uniform vec3 lightDir;
// the color of the light
uniform vec3 lightColor1;
uniform vec3 lightColor2;

uniform vec3 abcDropoff;

//shininess for blinn-phong
uniform float shinyness;


uniform vec3 materialColor;             // the material color for our vertex (& whole object)


// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space

// the normal of this specific vertex in object space
in vec3 vNorm;
// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex


//at the moment, 2 point lights
void main() {
    //point light code
    vec4 partialtransform;
    partialtransform = modelMtx * vec4(vPos,1.0);

    vec3 lightVec1 = vec3(pointLightPos1.x-partialtransform.x, pointLightPos1.y-partialtransform.y, pointLightPos1.z - partialtransform.z);
    vec3 lightVec1Norm = normalize(lightVec1);

    // transform the vertex normal in to world space
    vec3 normVecWorld;
    normVecWorld = normMtx*vNorm;

    float dotProd = dot(lightVec1Norm,normVecWorld);

    vec3 hVec1 = normalize(lightVec1Norm + normalize(vec3(camPos.x-partialtransform.x, camPos.y - partialtransform.y, camPos.z - partialtransform.z)));

    float dotProdSpec = dot(hVec1, normVecWorld);


    vec3 colorPt1;

    if (dotProd < 0){
        colorPt1 = vec3(0,0,0);
    }else{
        //diffuse
        colorPt1 = (lightColor1 * materialColor * dotProd);
    }

    vec3 colorPt1Spec;

    if (dotProdSpec < 0){
        colorPt1Spec = vec3(0,0,0);
    }else{
        //specular
        colorPt1Spec = lightColor1 * materialColor * pow(dotProdSpec,shinyness);
    }
    vec3 colorPt1Amb = lightColor1 * materialColor ;


    //attenuation
    colorPt1 = (colorPt1+colorPt1Spec+colorPt1Amb) / (abcDropoff.x + abcDropoff.y*length(lightVec1) + abcDropoff.z * length(lightVec1) * length(lightVec1));



    vec3 lightVec2 = vec3(pointLightPos2.x-partialtransform.x, pointLightPos2.y-partialtransform.y, pointLightPos2.z - partialtransform.z);
    vec3 lightVec2Norm = normalize(lightVec2);


    dotProd = dot(lightVec2Norm,normVecWorld);

    vec3 hVec2 = normalize(lightVec2Norm + normalize(vec3(camPos.x-partialtransform.x, camPos.y - partialtransform.y, camPos.z - partialtransform.z)));

    dotProdSpec = dot(hVec2, normVecWorld);


    vec3 colorPt2;
    if (dotProd < 0){
        colorPt2 = vec3(0,0,0);
    }else{
        //diffuse
        colorPt2 = (lightColor2 * materialColor * dotProd);
    }

    vec3 colorPt2Spec;
    if (dotProdSpec < 0){
        colorPt2Spec = vec3(0,0,0);
    }else{
        //specular
        colorPt2Spec = lightColor2 * materialColor * pow(dotProdSpec,shinyness);
    }
    vec3 colorPt2Amb = lightColor2 * materialColor;

    //attenuation
    colorPt2 = (colorPt2+colorPt2Spec+colorPt2Amb) / (abcDropoff.x + abcDropoff.y*length(lightVec2) + abcDropoff.z * length(lightVec2) * length(lightVec2));


    color = colorPt1 + colorPt2;
    //directional light code
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    /*
    leftover directional light code
    // convert the light direction to our normalized light vector
    vec3 lightVecNorm;
    lightVecNorm = normalize(lightDir*-1);


    // compute the diffuse component of the Phong Illumination Model
    float dotProd = dot(lightVecNorm,normVecWorld);

    // output the illumination color of this vertex
    if (dotProd < 0){
        color = vec3(0,0,0);
    }else{
        color = lightColor * materialColor * dotProd;
    }
    */

}