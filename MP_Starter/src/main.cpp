/*
 *  CSCI 441, Computer Graphics, Fall 2020
 *
 *  Project: Midterm Project
 *  File: main.cpp
 *
 *	Author: Jeffrey Paone - Fall 2020 (Reused lab2 code, some reused lab5 code)
 *	Steven Vennard - New assignment 3 + 4 code
 *
 *  Description: Draws car with arcball camera
 *
 */

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
#include "TrashCar.h"

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
const GLint WINDOW_WIDTH = 840, WINDOW_HEIGHT = 680;

int leftMouseButton;    	 				// status of the mouse button
double mouseX = -99999, mouseY = -99999;    // last known X and Y of the mouse


glm::vec3 camPos;            				// camera POSITION in cartesian coordinates
GLdouble cameraTheta, cameraPhi;            // camera DIRECTION in spherical coordinates
glm::vec3 camDir; 			                // camera DIRECTION in cartesian coordinates
glm::vec3 camPosSky;
glm::vec3 camDirSky;
glm::vec3 camAngleSky;

glm::vec3 camPosFirst;
glm::vec3 camDirFirst;

TrashCar heroTrashCar(0,0,0, 10, 52,-52,-52,52);




//booleans to allow multiple keys to be held
bool forward = false;
bool backward = false;
bool left = false;
bool right = false;
bool control = false;


GLboolean keys[256] = {0};              // keep track of our key states

//chooses camera in PIP
int cameraChoice = 2;


//TODO: add more hero  choices
//0 : Trash Car (Steven V.)
int heroChoice = 0;


//holds the info for drawTree()
struct treeData {
    float size;
    float x;
    float z;
};
std::vector<treeData> forest;

// values to track our grid properties
GLuint groundVAO;

//const glm::vec3 WHITE_COLOR( 1.0f, 1.0f, 1.0f );
//GLuint gridVAO = 0;
//GLuint numGridPoints = 0;

//because the direction vector is unit length, and we probably don't want
//to move one full unit every time a button is pressed, just create a constant
//to keep track of how far we want to move at each step. you could make
//this change w.r.t. the amount of time the button's held down for
//simple scale-sensitive movement!
const GLfloat MOVEMENT_CONSTANT = 1.5f;

bool mackHack = false;

// Shader Program information
CSCI441::ShaderProgram *lightingShader = nullptr;   // the wrapper for our shader program
LightingShaderUniforms  lightingShaderUniforms;
LightingShaderAttributes lightingShaderAttributes;




// END GLOBAL VARIABLES
//********************************************************************************

//********************************************************************************
//
// Helper Functions

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
GLdouble getRand() { return rand() / (GLdouble)RAND_MAX; }

// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta, cameraPhi, or cameraRadius is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {

    if (cameraPhi >= M_PI){
        cameraPhi = M_PI - .01;
    }
    if (cameraPhi <= M_PI/2.0){
        cameraPhi = M_PI/2.0 + .01;
    }

    if (heroChoice == 0) {
        camPos = glm::vec3(heroTrashCar.getCamRadius() * sin(cameraTheta) * sin(cameraPhi) + heroTrashCar.getX(), -heroTrashCar.getCamRadius() * cos(cameraPhi) + heroTrashCar.getY(),
                           -heroTrashCar.getCamRadius() * cos(cameraTheta) * sin(cameraPhi) + heroTrashCar.getZ());
        camDir = {heroTrashCar.getX(), heroTrashCar.getY(), heroTrashCar.getZ()};

        camPosSky = glm::vec3(heroTrashCar.getX(), heroTrashCar.getY() + 20, heroTrashCar.getZ());
        camDirSky = {heroTrashCar.getX(), heroTrashCar.getY(), heroTrashCar.getZ()};
        camAngleSky = glm::normalize(glm::vec3(sin(heroTrashCar.getCarAngle()), 0, cos(heroTrashCar.getCarAngle())));

        camPosFirst = {heroTrashCar.getX() + sin(heroTrashCar.getCarAngle()), heroTrashCar.getY() + heroTrashCar.getCarBounce() + 2, heroTrashCar.getZ() + cos(heroTrashCar.getCarAngle())};
        camDirFirst = glm::vec3(heroTrashCar.getX() + 2 * sin(heroTrashCar.getCarAngle()), heroTrashCar.getY() + heroTrashCar.getCarBounce() + 2, heroTrashCar.getZ() + 2 * cos(heroTrashCar.getCarAngle()));
    }


}

// computeAndSendMatrixUniforms() ///////////////////////////////////////////////
//
// This function precomputes the matrix uniforms CPU-side and then sends them
// to the GPU to be used in the shader for each vertex.  It is more efficient
// to calculate these once and then use the resultant product in the shader.
//
////////////////////////////////////////////////////////////////////////////////
void computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUniformMatrix4fv(lightingShaderUniforms.modelMtx, 1, GL_FALSE, &modelMtx[0][0]);
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glUniformMatrix4fv( lightingShaderUniforms.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0] );

    //  compute and send the normal matrix to the GPU
    glm::mat3 normalMtx	= glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glUniformMatrix3fv(lightingShaderUniforms.normMtx,1,GL_FALSE, &normalMtx[0][0]);
}

//*************************************************************************************
//
// Event Callbacks

//
//	void error_callback( int error, const char* description )
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
static void error_callback( int error, const char* description ) {
    fprintf( stderr, "[ERROR]: %d\n\t%s\n", error, description );
}


static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    if( action == GLFW_PRESS ) {
        switch( key ) {
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;

            case GLFW_KEY_W:
                forward = true;
                break;
            case GLFW_KEY_S:
                backward = true;
                break;
            case GLFW_KEY_A:
                left = true;
                break;
            case GLFW_KEY_D:
                right = true;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                control = true;
                break;
            case GLFW_KEY_1:
                cameraChoice = 1;
                break;
            case GLFW_KEY_2:
                cameraChoice = 2;
                break;
            default: break; // to remove CLion warning
        }

    }
    if( action == GLFW_RELEASE ) {
        switch( key ) {
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;

            case GLFW_KEY_W:
                forward = false;
                break;
            case GLFW_KEY_S:
                backward = false;
                break;
            case GLFW_KEY_A:
                left = false;
                break;
            case GLFW_KEY_D:
                right = false;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                control = false;
                break;
            default:
                break; // to remove CLion warning
        }
    }

}

static void cursor_callback( GLFWwindow *window, double x, double y ) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(mouseX == -99999) {
        mouseX = x;
        mouseY = y;
    }
    if(leftMouseButton == GLFW_PRESS && control == false) {
        if (mouseX > x){
            cameraTheta -= .05;
        }else if  (mouseX < x){
            cameraTheta += .05;
        }
        if (mouseY > y){
            cameraPhi += .05;
        }else if (mouseY < y){
            cameraPhi -= .05;
        }

        recomputeOrientation();     //update camera (x,y,z) based on (radius,theta,phi)
    }
    if (leftMouseButton == GLFW_PRESS && control == true){
        if (heroChoice == 0) {
            if (mouseX > x) {
                heroTrashCar.setCamRadius(heroTrashCar.getCamRadius() - .3);
            } else if (mouseX < x) {
                heroTrashCar.setCamRadius(heroTrashCar.getCamRadius() + .3);
            }
            if (heroTrashCar.getCamRadius() < 1) {
                heroTrashCar.setCamRadius(1);
            }
        }
    }


    mouseX = x;
    mouseY = y;
}

static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        leftMouseButton = action;
    }
}

// generateEnvironment() ///////////////////////////////////////////////////////
//
//  This function creates our world which will consist of a grid in the XZ-Plane
//      and randomly placed and sized buildings of varying colors.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironment() {
    // parameters to size our world

    const GLint GRID_WIDTH = 100;
    const GLint GRID_LENGTH = 100;
    /*
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5;
    */


    //make some trees
    for (int i = 0; i < 50; i++){
        treeData temp;
        temp.size = getRand()+.3;
        temp.x = (GRID_WIDTH - 2*getRand()*GRID_WIDTH)/2;
        temp.z = (GRID_LENGTH - 2*getRand()*GRID_LENGTH)/2;
        forest.push_back(temp);
    }
/*
    // create the grid - lab02 code
    std::vector< glm::vec3 > points;
    for(GLfloat i = LEFT_END_POINT; i <= RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        points.emplace_back( glm::vec3(i, 0, BOTTOM_END_POINT) );
        points.emplace_back( glm::vec3(i, 0, TOP_END_POINT) );
    }
    for(GLfloat j = BOTTOM_END_POINT; j <= TOP_END_POINT; j += GRID_SPACING_LENGTH) {
        points.emplace_back( glm::vec3(LEFT_END_POINT, 0, j) );
        points.emplace_back( glm::vec3(RIGHT_END_POINT, 0, j) );
    }
    numGridPoints = points.size();
    gridVAO = CSCI441::SimpleShader3::registerVertexArray(points, std::vector<glm::vec3>(numGridPoints));
    */
}






//Note: Trees used to be made of cylinders/cones, but the library was causing a memory leak
void drawTree(float size, float x, float z, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glm::mat4 modelMtxTotal = glm::translate( glm::mat4(1.0), glm::vec3( x, 0, z ) );
    modelMtxTotal = glm::scale( modelMtxTotal, glm::vec3( size, size, size ) );



    glm::vec3 treeColor(0.075,	0.615,	0.076);
    glUniform3fv(lightingShaderUniforms.materialColor, 1, &treeColor[0]);

    float shinyness = 0.6;
    glUniform1f(lightingShaderUniforms.shinyness,shinyness);

    glm::mat4 modelMtx = glm::translate( modelMtxTotal, glm::vec3( 0, 1.5, 0 ) );
    computeAndSendMatrixUniforms(modelMtx,viewMtx,projMtx);
    CSCI441::drawSolidCube(1.5);


    glm::vec3 treeColor2(.7,0.4,0.2);
    glUniform3fv(lightingShaderUniforms.materialColor, 1, &treeColor2[0]);

    shinyness = 0.2;
    glUniform1f(lightingShaderUniforms.shinyness,shinyness);



    modelMtx = glm::translate( modelMtxTotal, glm::vec3( 0, .5, 0 ) );
    computeAndSendMatrixUniforms(modelMtx,viewMtx,projMtx);
    CSCI441::drawSolidCube(1);
    //::drawSolidCylinder(1.2,1.2,2,4,4);

}
void drawGroundTile(float size, float x, float z, glm::mat4 viewMtx, glm::mat4 projMtx){
    glm::mat4 modelMtx = glm::translate(glm::mat4(1.0f), glm::vec3(x,0,z));
    modelMtx = glm::scale(modelMtx, glm::vec3(size,.1,size));

    //draw flat ground

    glm::vec3 groundColor(0.6,0.6,0.6);
    glUniform3fv(lightingShaderUniforms.materialColor, 1, &groundColor[0]);

    float shinyness = 0.1;
    glUniform1f(lightingShaderUniforms.shinyness,shinyness);


    computeAndSendMatrixUniforms(modelMtx,viewMtx,projMtx);
    glBindVertexArray(groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
}


// renderScene() ///////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) {
    // use our lighting shader program
    lightingShader->useProgram();
    heroTrashCar.drawTrashCar(viewMtx,projMtx);
    recomputeOrientation();


    for (treeData current: forest){
        drawTree(current.size, current.x, current.z, viewMtx, projMtx);
    }
    float tileSize = 10;
    for (int x = -50; x < 60; x+=10){
        for (int z = -50; z < 60; z+=10){
            drawGroundTile(tileSize, x,z, viewMtx,projMtx);
        }
    }



    // draw our grid
    /*
    CSCI441::SimpleShader3::disableLighting();
    CSCI441::SimpleShader3::setMaterialColor( WHITE_COLOR );
    CSCI441::SimpleShader3::draw(GL_LINES, gridVAO, numGridPoints);
    CSCI441::SimpleShader3::enableLighting();
*/

}


//*************************************************************************************
//
// Setup Functions

//
//  void setupGLFW()
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
GLFWwindow* setupGLFW() {
    // set what function to use when registering errors
    // this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
    // all other GLFW calls must be performed after GLFW has been initialized
    glfwSetErrorCallback( error_callback );

    // initialize GLFW
    if( !glfwInit() ) {
        fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
        exit( EXIT_FAILURE );
    } else {
        fprintf( stdout, "[INFO]: GLFW initialized\n" );
    }

    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );  // request forward compatible OpenGL context
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );        // request OpenGL Core Profile context
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );	// request OpenGL v4.X
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	// request OpenGL vX.1
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		    // do not allow our window to be able to be resized
    glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );         // request double buffering

    // create a window for a given size, with a given title
    GLFWwindow *window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 4 - The Main Attraction", nullptr, nullptr );
    if( !window ) {						// if the window could not be created, NULL is returned
        fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    } else {
        fprintf( stdout, "[INFO]: GLFW Window created\n" );
    }

    glfwMakeContextCurrent(window);		                    // make the created window the current window
    glfwSwapInterval(1);				     	    // update our screen after at least 1 screen refresh

    glfwSetKeyCallback( window, keyboard_callback );		// set our keyboard callback function
    glfwSetCursorPosCallback( window, cursor_callback );	// set our cursor position callback function
    glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function

    return window;						                     // return the window that was created
}

//
//  void setupOpenGL()
//
//      Used to setup everything OpenGL related.  For now, the only setting
//	we need is what color to make the background of our window when we clear
//	the window.  In the future we will be adding many more settings to this
//	function.
//
void setupOpenGL() {
    // initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewResult = glewInit();

    // check for an error
    if( glewResult != GLEW_OK ) {
        printf( "[ERROR]: Error initializing GLEW\n");
        exit(EXIT_FAILURE);
    }

    // tell OpenGL to perform depth testing with the Z-Buffer to perform hidden
    //		surface removal.  We will discuss this more very soon.
    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// set the clear color to black
}

void setupScene() {
    // give the camera a scenic starting point.
    camDir = {0,0,0};
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    recomputeOrientation();

    srand( time(nullptr) );	// seed our random number generator
    generateEnvironment();
  //  generateEnvironmentDL();                // create our environment display list
    lightingShader->useProgram();
    // BIG TODO  set the light direction and color
    //glm::vec3 lightDirection = {-1,-1,-1};
    glm::vec3 lightColor1 = {1,1,1};
    glm::vec3 lightColor2 = {0,1,0};

    glm::vec3 pointLightPos1 = {20,20,20};
    glm::vec3 pointLightPos2 = {-20,20,-20};

    glm::vec3 abcDropoff = {1.0f, 0.025f, 0.0055f};

    //glUniform3fv(lightingShaderUniforms.lightDir,1,&lightDirection[0]);
    glUniform3fv(lightingShaderUniforms.lightColor1,1,&lightColor1[0]);
    glUniform3fv(lightingShaderUniforms.lightColor2,1,&lightColor2[0]);
    glUniform3fv(lightingShaderUniforms.pointLightPos1,1 ,&pointLightPos1[0]);
    glUniform3fv(lightingShaderUniforms.pointLightPos2,1, &pointLightPos2[0]);

    glUniform3fv(lightingShaderUniforms.abcDropoff,1,&abcDropoff[0]);

    float shinyness = 0.5;
    glUniform1f(lightingShaderUniforms.shinyness,shinyness);
}

void setupShaders() {
    lightingShader = new CSCI441::ShaderProgram( "shaders/mp.v.glsl", "shaders/mp.f.glsl" );
    lightingShaderUniforms.mvpMatrix      = lightingShader->getUniformLocation("mvpMatrix");
    // assign the uniform and attribute locations
    lightingShaderUniforms.materialColor  = lightingShader->getUniformLocation("materialColor");
    lightingShaderUniforms.normMtx  = lightingShader->getUniformLocation("normMtx");
    lightingShaderUniforms.lightColor1  = lightingShader->getUniformLocation("lightColor1");
    lightingShaderUniforms.lightColor2  = lightingShader->getUniformLocation("lightColor2");
    lightingShaderUniforms.camPos = lightingShader->getUniformLocation("camPos");
    lightingShaderUniforms.shinyness = lightingShader->getUniformLocation("shinyness");
    //lightingShaderUniforms.lightDir  = lightingShader->getUniformLocation("lightDir");
    lightingShaderUniforms.pointLightPos1 = lightingShader->getUniformLocation("pointLightPos1");
    lightingShaderUniforms.pointLightPos2 = lightingShader->getUniformLocation("pointLightPos2");
    lightingShaderUniforms.abcDropoff = lightingShader->getUniformLocation("abcDropoff");
    lightingShaderUniforms.modelMtx = lightingShader->getUniformLocation("modelMtx");

    lightingShaderAttributes.vPos         = lightingShader->getAttributeLocation("vPos");
    lightingShaderAttributes.vNorm        = lightingShader->getAttributeLocation("vNorm");

    heroTrashCar.setLightingShaderUandA(lightingShaderUniforms,lightingShaderAttributes);
}
void setupBuffers() {
    // expand our struct to store vertex normals
    struct VertexNormal {
        GLfloat x, y, z;
        GLfloat xNorm,yNorm,zNorm;
    };

    // specify the specific vertex normal values
    VertexNormal groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f, 0,1,0},
            { 1.0f, 0.0f, -1.0f, 0,1,0},
            {-1.0f, 0.0f,  1.0f, 0,1,0},
            { 1.0f, 0.0f,  1.0f, 0,1,0}
    };

    GLushort indices[4] = {0,1,2,3};

    glGenVertexArrays(1, &groundVAO);
    glBindVertexArray(groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(lightingShaderAttributes.vPos);
    glVertexAttribPointer(lightingShaderAttributes.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)0);


    glEnableVertexAttribArray(lightingShaderAttributes.vNorm);
    glVertexAttribPointer(lightingShaderAttributes.vNorm,3,GL_FLOAT,GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(float)*3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}


///*************************************************************************************
//
// Our main function

//
//	int main()
//
int main() {
    // GLFW sets up our OpenGL context so must be done first
    GLFWwindow *window = setupGLFW();	                // initialize all of the GLFW specific information related to OpenGL and our window
    setupOpenGL();										// initialize all of the OpenGL specific information
    CSCI441::OpenGLUtils::printOpenGLInfo();
    setupShaders();                                         // load our shader program into memory
    setupBuffers();
    setupScene();

    CSCI441::setVertexAttributeLocations( lightingShaderAttributes.vPos, lightingShaderAttributes.vNorm );

    printf("Controls:\n");
    printf("\tW / S - Move forwards / backwards\n");
    printf("\tMouse Drag - Pan camera\n");
    printf("\tA /D - Turn left/right\n");
    printf("\tCtrl + Mouse Drag left/right - Zoom in and out \n");
    printf("\t1 and 2 to swap between first-person and sky cam in PIP \n");
    printf("\tQ / ESC - Quit program\n");

    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(window) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window
        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );


        // update the projection matrix based on the window size
        // the GL_PROJECTION matrix governs properties of the view coordinates;
        // i.e. what gets seen - use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.001f, 1000.0f );


        // Arcball camera

        glm::mat4 viewMtx = glm::lookAt( camPos, camDir,glm::vec3(  0,  1,  0 ) );
        glUniform3fv(lightingShaderUniforms.camPos, 1, &camPos[0]);
        // multiply by the look at matrix - this is the same as our view matrix


        renderScene(viewMtx,projMtx);					                // draw everything to the window




        //Glscissor makes sure only part of the screen is cleared (the part for our camera)
        //This code clears everything by the color white in the area we want,
        //Then clears with transparent color inside a border to make the cam look nice
        glEnable(GL_SCISSOR_TEST);
        glScissor(WINDOW_WIDTH - framebufferWidth/4 - 5,WINDOW_HEIGHT - framebufferHeight/4 - 5, framebufferWidth/4 + 5, framebufferHeight/4 + 5);
        glClearColor(1.0,1.0,1.0,1.0);
        glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );
        glScissor(WINDOW_WIDTH - framebufferWidth/4,WINDOW_HEIGHT - framebufferHeight/4, framebufferWidth/4, framebufferHeight/4);
        glClearColor(0.0,0.0,0.0,0.0);
        glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT );

        glViewport(WINDOW_WIDTH - framebufferWidth/4,WINDOW_HEIGHT - framebufferHeight/4, framebufferWidth/4, framebufferHeight/4);
        projMtx = glm::perspective( 45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.001f, 1000.0f );
        if (cameraChoice == 1) {
            //sky cam
            viewMtx = glm::lookAt(camPosSky, camDirSky, camAngleSky);
            glUniform3fv(lightingShaderUniforms.camPos, 1, &camPosSky[0]);
        }else if (cameraChoice == 2){
            //first person cam
            viewMtx = glm::lookAt(camPosFirst, camDirFirst, glm::vec3(  0,  1,  0 ));
            glUniform3fv(lightingShaderUniforms.camPos, 1, &camPosFirst[0]);
        }
        // multiply by the look at matrix - this is the same as our view matrix

        renderScene(viewMtx,projMtx);					                // draw everything to the small window
        glDisable(GL_SCISSOR_TEST);
        glfwSwapBuffers(window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen

        // the following code is a hack for OSX Mojave
        // the window is initially black until it is moved
        // so instead of having the user manually move the window,
        // we'll automatically move it and then move it back
        if( !mackHack ) {
            GLint xPos, yPos;
            glfwGetWindowPos(window, &xPos, &yPos);
            glfwSetWindowPos(window, xPos+10, yPos+10);
            glfwSetWindowPos(window, xPos, yPos);
            mackHack = true;
        }

        if (heroChoice == 0) {
            heroTrashCar.updateCar(forward, backward, left, right);
        }


    }
    delete lightingShader;                                  // delete our shader program
    CSCI441::deleteObjectVBOs();                            // delete our library VBOs
    CSCI441::deleteObjectVAOs();                            // delete our library VAOs

    glfwDestroyWindow( window );// clean up and close our window
    glfwTerminate();						// shut down GLFW to clean up our context

    return EXIT_SUCCESS;				// exit our program successfully!
}
