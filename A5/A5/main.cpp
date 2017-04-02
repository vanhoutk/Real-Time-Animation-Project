/*
*	Includes
*/
#include <assimp/cimport.h>		// C importer
#include <assimp/scene.h>		// Collects data
#include <assimp/postprocess.h> // Various extra operations
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#include <vector>				// STL dynamic memory
#include <windows.h>

#include "Antons_maths_funcs.h" // Anton's maths functions
#include "Camera.h"
#include "Mesh.h"
#include "PlaneRotation.h"
#include "Shader_Functions.h"
#include "Skeleton.h"
#include "time.h"

using namespace std;

/*
*	Globally defined variables and constants
*/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))  // Macro for indexing vertex buffer

#define NUM_MESHES   6
#define NUM_SHADERS	 5
#define NUM_TEXTURES 1

bool firstMouse = true;
bool forwardAnimation = true;
bool keys[1024];
Camera camera(vec3(-1.5f, 2.0f, 30.0f));
enum Bezier { WAVE, CURVE };
enum Meshes { LAMP_BASE_MESH, LAMP_ARM_LOWER_MESH, LAMP_ARM_UPPER_MESH, LAMP_HEAD_MESH, LAMP_LIGHT_MESH, LIGHT_MESH, GROUND_MESH };
enum Shaders { SKYBOX, BASIC_COLOUR_SHADER, BASIC_TEXTURE_SHADER, LIGHT_SHADER, LIGHT_TEXTURE_SHADER };
enum Textures { METAL_TEXTURE };
GLfloat cameraDistance = 20.0f;
GLfloat cameraPitch = 0.0f;
GLfloat cameraSpeed = 0.005f;
GLfloat cameraYaw = 0.0f;
GLfloat movementSpeed = 0.1f;
GLfloat currentTime = 0.0f;
GLfloat timeChange = 0.002f;
GLfloat yOffset = 3.0f;
GLuint animationMode = -1;
GLuint bezierCurve = CURVE;
GLuint lastX = 400, lastY = 300;
GLuint shaderProgramID[NUM_SHADERS];
int screenWidth = 1000;
int screenHeight = 800;
Mesh lampBaseMesh, lampArmLowerMesh, lampArmUpperMesh, lampHeadMesh, lampLightMesh, lightMesh, groundMesh;
Skeleton lampSkeleton;
vec3 lampPosition = vec3(0.0f, 0.0f, 0.0f);
vec3 spherePosition;// = vec3(-9.0f, 10.0f, 0.0f);
// | Spline points
vec3 p1 = vec3(-10.0f, 8.0f, 0.0f);
vec3 p2 = vec3(-11.0f, 11.0f, 1.0f);
vec3 p3 = vec3(-9.0f, 14.0f, 2.0f);
vec3 p4 = vec3(-2.0f, 10.0f, 3.0f);

vec3 wave[4] = { vec3(-9.0f, 8.0f, 4.0f), vec3(-7.0f, 11.0f, 4.0f), vec3(-5.0f, 13.0f, 4.0f), vec3(-4.0f, 10.0f, 4.0f) };
vec3 curve1[4] = { vec3(-6.0f, 6.0f, 4.0f), vec3(-9.0f, 6.0f, 4.0f), vec3(-9.0f, 10.0f, 4.0f), vec3(-6.0f, 10.0f, 4.0f) };
vec3 curve2[4] = { vec3(-6.0f, 10.0f, 4.0f), vec3(-3.0f, 10.0f, 4.0f), vec3(-3.0f, 6.0f, 4.0f), vec3(-6.0f, 6.0f, 4.0f) };

// | Resource Locations
const char * meshFiles[NUM_MESHES] = { "../Meshes/lamp_base.dae", "../Meshes/lamp_arm.dae", "../Meshes/lamp_arm_upper.dae", "../Meshes/lamp_head.obj", "../Meshes/lamp_light.obj", "../Meshes/particle.dae" };
const char * skyboxTextureFiles[6] = { "../Textures/TCWposx.png", "../Textures/TCWnegx.png", "../Textures/TCWposy.png", "../Textures/TCWnegy.png", "../Textures/TCWposz.png", "../Textures/TCWnegz.png" };
const char * textureFiles[NUM_TEXTURES] = { "../Textures/metal.jpg" };

const char * vertexShaderNames[NUM_SHADERS] = { "../Shaders/SkyboxVertexShader.txt", "../Shaders/ParticleVertexShader.txt", "../Shaders/BasicTextureVertexShader.txt", "../Shaders/LightVertexShader.txt", "../Shaders/LightTextureVertexShader.txt" };
const char * fragmentShaderNames[NUM_SHADERS] = { "../Shaders/SkyboxFragmentShader.txt", "../Shaders/ParticleFragmentShader.txt", "../Shaders/BasicTextureFragmentShader.txt", "../Shaders/LightFragmentShader.txt", "../Shaders/LightTextureFragmentShader.txt" };

void display()
{
	// Tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST);	// Enable depth-testing
	glDepthFunc(GL_LESS);		// Depth-testing interprets a smaller value as "closer"
	glClearColor(5.0f / 255.0f, 1.0f / 255.0f, 15.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the view matrix to first or third person views
	mat4 view = look_at(camera.Position, lampPosition + vec3(0.0f, yOffset, 0.0f), camera.WorldUp);//  camera.GetViewMatrix();

	// Projection Matrix
	mat4 projection = perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	
	// Model Matrix
	//mat4 model = translate(identity_mat4(), lampPosition);

	vec4 view_position = vec4(camera.Position.v[0], camera.Position.v[1], camera.Position.v[2], 0.0f);

	//lampBaseMesh.drawMesh(view, projection, model, vec4(1.0f, 1.0f, 1.0f, 1.0f), view_position);
	//lampArmMesh.drawMesh(view, projection, model, vec4(1.0f, 1.0f, 1.0f, 1.0f), view_position);
	//lampHeadMesh.drawMesh(view, projection, model, vec4(1.0f, 1.0f, 1.0f, 1.0f), view_position);
	//lampLightMesh.drawMesh(view, projection, model, vec4(0.9f, 0.9f, 0.0f, 0.75f), view_position);
	lampSkeleton.drawSkeleton(view, projection, view_position);

	glutSwapBuffers();
}

void processInput()
{
	if (keys[GLUT_KEY_UP])
		lampSkeleton.moveLamp(MOVE_FORWARD, movementSpeed);
	if (keys[GLUT_KEY_DOWN])
		lampSkeleton.moveLamp(MOVE_BACKWARD, movementSpeed);
	if (keys[GLUT_KEY_LEFT])
		lampSkeleton.turnLamp(TURN_LEFT, movementSpeed / 10.0f);
	if (keys[GLUT_KEY_RIGHT])
		lampSkeleton.turnLamp(TURN_RIGHT, movementSpeed / 10.0f);

	if (keys['w'])
	{
		cameraPitch += cameraSpeed;
		if (cameraPitch > 1.57f)
			cameraPitch = 1.57f;
	}
	if (keys['s'])
	{
		cameraPitch -= cameraSpeed;
		if (cameraPitch < 0.0f)
			cameraPitch = 0.0f;
	}
	if (keys['a'])
		cameraYaw -= cameraSpeed;
	if (keys['d'])
		cameraYaw += cameraSpeed;

	// Move the sphere position around manually
	/*if (animationMode == CCD_IK_MANUAL || animationMode == CCD_IK_MANUAL_FINGER)
	{
		if (keys['p'])
			spherePosition += vec3(0.01f, 0.0f, 0.0f);
		if (keys['o'])
			spherePosition += vec3(0.0f, 0.01f, 0.0f);
		if (keys['l'])
			spherePosition += vec3(0.0f, -0.01f, 0.0f);
		if (keys['i'])
			spherePosition += vec3(-0.01f, 0.0f, 0.0f);
		if (keys['j'])
			spherePosition += vec3(0.0f, 0.0f, 0.01f);
		if (keys['k'])
			spherePosition += vec3(0.0f, 0.0f, -0.01f);
	}*/

	// Change animation mode
	/*if (keys['1'])
		animationMode = ROTATE_HAND;
	if (keys['2'])
		animationMode = CLOSE_FIST;
	if (keys['3'])
		animationMode = OPEN_FIST;
	if (keys['4'])
		animationMode = CLOSE_AND_OPEN_FIST;
	//if (keys['5'])
		//animationMode = ANALYTICAL_IK_2D;
	if (keys['6'])
		animationMode = CCD_IK_MANUAL;
	if (keys['7'])
		animationMode = CCD_IK_SPLINE;
	if (keys['8'])
		animationMode = CCD_IK_MANUAL_FINGER;
	if (keys['9'])
		animationMode = CCD_IK_SPLINE_FINGER;
	if (keys['0'])
		animationMode = -1;

	if (keys['n'])
		bezierCurve = WAVE;
	if (keys['m'])
		bezierCurve = CURVE;*/

	// Close the window if 'Esc' is pressed
	if (keys[(char)27])
		exit(0);
}

void updateAnimation()
{
	if (currentTime <= 1.0f)
		currentTime += timeChange;
	else
	{
		currentTime = 0.0f;
		animationMode = -1;
	}

	switch (animationMode)
	{
	case JUMP:
		cout << "Jump" << endl;
		lampSkeleton.jumpLamp(currentTime, (currentTime <= timeChange));
		break;
	}

	//if(forwardAnimation)
	//	spherePosition = splinePositionBezier(p1, p2, p3, p4, currentTime);
	//else
	//	spherePosition = splinePositionBezier(p4, p3, p2, p1, currentTime);
	if (bezierCurve == WAVE)
	{
		if (forwardAnimation)
			spherePosition = splinePositionBezier(wave[0], wave[1], wave[2], wave[3], currentTime);
		else
			spherePosition = splinePositionBezier(wave[3], wave[2], wave[1], wave[0], currentTime);
	}
	else
	{
		if (forwardAnimation)
			spherePosition = splinePositionBezier(curve1[0], curve1[1], curve1[2], curve1[3], currentTime);
		else
			spherePosition = splinePositionBezier(curve2[0], curve2[1], curve2[2], curve2[3], currentTime);
	}
}

void updateScene()
{
	if(animationMode != -1)
		updateAnimation();

	processInput();
	lampPosition = lampSkeleton.getRootPosition();
	camera.UpdateCamera3rdPerson(cameraPitch, cameraYaw, lampPosition, cameraDistance, yOffset);
	// Draw the next frame
	glutPostRedisplay();
}

void initialiseMeshes()
{
	lampBaseMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lampBaseMesh.generateObjectBufferMesh(meshFiles[LAMP_BASE_MESH]);

	lampArmLowerMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lampArmLowerMesh.generateObjectBufferMesh(meshFiles[LAMP_ARM_LOWER_MESH]);

	lampArmUpperMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lampArmUpperMesh.generateObjectBufferMesh(meshFiles[LAMP_ARM_UPPER_MESH]);

	lampHeadMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lampHeadMesh.generateObjectBufferMesh(meshFiles[LAMP_HEAD_MESH]);

	lampLightMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lampLightMesh.generateObjectBufferMesh(meshFiles[LAMP_LIGHT_MESH]);

	lightMesh = Mesh(&shaderProgramID[LIGHT_SHADER]);
	lightMesh.generateObjectBufferMesh(meshFiles[LIGHT_MESH]);

	//groundMesh = Mesh(&shaderProgramID[LIGHT_TEXTURE_SHADER]);
	//groundMesh.generateObjectBufferMesh(meshFiles[GROUND_MESH]);
}

void init()
{
	// Compile the shaders
	for (int i = 0; i < NUM_SHADERS; i++)
	{
	shaderProgramID[i] = CompileShaders(vertexShaderNames[i], fragmentShaderNames[i]);
	}

	// Create all of the meshes
	initialiseMeshes();

	//handSkeleton.createRightHand(handMesh, handShellMesh, fingerJointMesh, jointShellMesh, fingerTipMesh, tipShellMesh);
	//torsoSkeleton.createTorso(torsoMesh, shouldersMesh, shouldersShellMesh, upperArmShellMesh, lowerArmShellMesh, handMesh, handShellMesh, fingerJointMesh, jointShellMesh, fingerTipMesh, tipShellMesh);
	lampSkeleton.createLamp(lampBaseMesh, lampArmLowerMesh, lampArmUpperMesh, lampHeadMesh, lampLightMesh);
}

/*
*	User Input Functions
*/
#pragma region USER_INPUT_FUNCTIONS
void pressNormalKeys(unsigned char key, int x, int y)
{
	keys[key] = true;

	if (keys['v'])
	{
		animationMode = JUMP;
	}
}

void releaseNormalKeys(unsigned char key, int x, int y)
{
	keys[key] = false;
}

void pressSpecialKeys(int key, int x, int y)
{
	keys[key] = true;
}

void releaseSpecialKeys(int key, int x, int y)
{
	keys[key] = false;
}

void mouseClick(int button, int state, int x, int y)
{}

void processMouse(int x, int y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	int xoffset = x - lastX;
	int yoffset = lastY - y;

	lastX = x;
	lastY = y;

	camera.ProcessMouseMovement((GLfloat)xoffset, (GLfloat)yoffset);
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
		cameraDistance -= 10.0f * cameraSpeed;
	else
		cameraDistance += 10.0f * cameraSpeed;
}
#pragma endregion

/*
*	Main
*/
int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - screenWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - screenHeight) / 4);
	glutCreateWindow("Hand Hierarchy");

	// Glut display and update functions
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);

	// User input functions
	glutKeyboardFunc(pressNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(processMouse);
	glutMouseWheelFunc(mouseWheel);

	glewExperimental = GL_TRUE; //for non-lab machines, this line gives better modern GL support

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Set up meshes and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}