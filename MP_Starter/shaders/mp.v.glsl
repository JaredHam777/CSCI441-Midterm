#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat4 modelMtx;

// the normal matrix
uniform mat3 normMtx;

//the point light positions
uniform vec3 pointLightPos;
uniform vec3 spotLightPos;
uniform vec3 spotLightVec;
uniform vec3 directLightDir;
uniform float spotLightAngle;
uniform float spotLightFallOffAngle;
//uniform vec3 pointLightPos2;

//cam position
uniform vec3 camPos;


// the direction the incident ray of light is traveling
//uniform vec3 lightDir;
// the color of the light
uniform vec3 lightColorPoint;
uniform vec3 lightColorDirect;
uniform vec3 lightColorSpotLight;

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

    vec3 lightVec1 = vec3(pointLightPos.x-partialtransform.x, pointLightPos.y-partialtransform.y, pointLightPos.z - partialtransform.z);
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
        colorPt1 = (lightColorPoint * materialColor * dotProd);
    }

    vec3 colorPt1Spec;

    if (dotProdSpec < 0){
        colorPt1Spec = vec3(0,0,0);
    }else{
        //specular
        colorPt1Spec = lightColorPoint  * materialColor * pow(dotProdSpec,shinyness);
    }



    //attenuation
    colorPt1 = (colorPt1+colorPt1Spec) / (abcDropoff.x + abcDropoff.y*length(lightVec1) + abcDropoff.z * length(lightVec1) * length(lightVec1));

    //spot light code

    vec3 colorSpot = vec3(0,0,0);

    vec3 lightVec2 = vec3(spotLightPos.x-partialtransform.x, spotLightPos.y-partialtransform.y, spotLightPos.z - partialtransform.z);
    vec3 lightVec2Norm = normalize(lightVec2);

    float spotLightAngleToPoint = dot(-1*lightVec2Norm,normalize(spotLightVec));
    float dotProd2 = dot(lightVec2Norm,normVecWorld);

    vec3 hVec2 = normalize(lightVec2Norm + normalize(vec3(camPos.x-partialtransform.x, camPos.y - partialtransform.y, camPos.z - partialtransform.z)));
    float dotProd2Spec = dot(hVec2, normVecWorld);
    //values with a smaller angle will have a larger cos value
    if (spotLightAngleToPoint < spotLightAngle){
        if (spotLightAngleToPoint < spotLightFallOffAngle){
            colorSpot = vec3(0,0,0);
        }else{
            //diffuse
            if (dotProd2 < 0){
                colorSpot = vec3(0,0,0);
            }else{
                colorSpot = lightColorSpotLight * materialColor * dotProd2;
            }
            //specular
            if (dotProd2Spec < 0){
                colorSpot += vec3(0,0,0);
            }else{
                colorSpot += lightColorSpotLight  * materialColor * pow(dotProd2Spec,shinyness);
            }
            //scale back color by distance
            colorSpot = colorSpot * spotLightAngleToPoint/spotLightAngle;
        }
    }else{
        //colorSpot = lightColorSpotLight * materialColor;
        //diffuse
        if (dotProd2 < 0){
            colorSpot = vec3(0,0,0);
        }else{
            colorSpot = lightColorSpotLight * materialColor * dotProd2;
        }


        //specular
        if (dotProd2Spec < 0){
            colorSpot += vec3(0,0,0);
        }else{
            colorSpot += lightColorSpotLight  * materialColor * pow(dotProd2Spec,shinyness);
        }


    }


   // colorSpot = colorSpot / (abcDropoff.x + abcDropoff.y*length(lightVec2) + abcDropoff.z * length(lightVec2) * length(lightVec2));


    //directional light
    // convert the light direction to our normalized light vector
    vec3 lightVecNorm;
    lightVecNorm = normalize(directLightDir*-1);


    // compute the diffuse component of the Phong Illumination Model
    dotProd = dot(lightVecNorm,normVecWorld);
    vec3 dirColor;
    //diffuse
    if (dotProd < 0){
        dirColor = vec3(0,0,0);
    }else{
        dirColor = lightColorDirect * materialColor * dotProd;
    }

    vec3 hVecDir = normalize(lightVecNorm + normalize(vec3(camPos.x-partialtransform.x, camPos.y - partialtransform.y, camPos.z - partialtransform.z)));
    float dotDirSpec = dot(hVecDir, normVecWorld);
    vec3 colorDirSpec;

    if (dotDirSpec < 0){
        colorDirSpec = vec3(0,0,0);
    }else{
        //specular
        colorDirSpec = lightColorDirect  * materialColor * pow(dotDirSpec,shinyness);
    }
    //no attenuation for directional light



    //ONLY ADD AMBIENT ONCE (5% of material color at the moment, as multicolor lights will be complex)
    vec3 colorPtAmb = materialColor * 0.05;
   //final color output
    color = colorPt1 + dirColor + colorDirSpec + colorSpot + colorPtAmb;


    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);


}