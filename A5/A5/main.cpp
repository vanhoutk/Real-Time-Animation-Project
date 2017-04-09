/*
*	Includes
*/
#include <assimp/cimport.h>		// C importer
#include <assimp/scene.h>		// Collects data
#include <assimp/postprocess.h> // Various extra operations
#include <chrono>
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
#include "LightParticle.h"
#include "Mesh.h"
#include "PlaneRotation.h"
#include "Shader_Functions.h"
#include "Skeleton.h"
#include "text.h"
#include "time.h"

using namespace std;
using namespace chrono;// clock = std::chrono::system_clock;

/*
*	Globally defined variables and constants
*/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))  // Macro for indexing vertex buffer

#define NUM_MESHES   7
#define NUM_SHADERS	 5
#define NUM_TEXTURES 2

bool firstAnimation = false;
bool firstMouse = true;
bool forwardAnimation = true;
bool keys[1024];
Camera camera(vec3(-1.5f, 2.0f, 30.0f));
enum Bezier { WAVE, CURVE };
enum GameStates { TITLE, INTRO, GAME };
enum LampModes { LAMP_OFF, LAMP_ON };
enum Meshes { LAMP_BASE_MESH, LAMP_ARM_LOWER_MESH, LAMP_ARM_UPPER_MESH, LAMP_HEAD_MESH, LAMP_LIGHT_MESH, LIGHT_MESH, GROUND_MESH };
enum Shaders { SKYBOX, BASIC_COLOUR_SHADER, BASIC_TEXTURE_SHADER, LIGHT_SHADER, LIGHT_TEXTURE_SHADER };
enum Textures { METAL_TEXTURE, WOOD_TEXTURE };
GLfloat animationTime = 0.75f;
GLfloat cameraDistance = 20.0f, cameraPitch = 0.5f, cameraSpeed = 0.005f, cameraYaw = 2.4f;
GLfloat movementSpeed = 0.1f;
//GLfloat currentTime = 0.0f;
GLfloat timeChange = 0.01f;
GLfloat introTime = 4.0f;
GLfloat yOffset = 3.0f;
GLuint animationMode = IDLE;
GLuint bezierCurve = CURVE;
GLuint currentIntroText = 0;
GLuint gameState = GAME;
GLuint lampMode = LAMP_ON;
GLuint numIntroTexts = 5;
GLuint lastX = 400, lastY = 300;
GLuint shaderProgramID[NUM_SHADERS];
int titleText[2], introText[5], gameText[2];
int screenWidth = 1000;
int screenHeight = 800;
LightParticle light;
long currentTime, lightTimer;
Mesh lampBaseMesh, lampArmLowerMesh, lampArmUpperMesh, lampHeadMesh, lampLightMesh, lightMesh, groundMesh;
Skeleton lampSkeleton;
time_t startTime;
//time_point<system_clock, milliseconds> currentTime;
vec3 lampPosition = vec3(0.0f, 0.0f, 0.0f);
vec4 lampLight[2] = { vec4(0.0f, 0.0f, 0.0f, 0.8f), vec4(0.85f, 0.75f, 0.0f, 0.5f) };
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
const char * meshFiles[NUM_MESHES] = { "../Meshes/lamp_base.dae", "../Meshes/lamp_arm.dae", "../Meshes/lamp_arm_upper.dae", "../Meshes/lamp_head.obj", "../Meshes/lamp_light.obj", "../Meshes/particle.dae", "../Meshes/plane.dae" };
const char * skyboxTextureFiles[6] = { "../Textures/TCWposx.png", "../Textures/TCWnegx.png", "../Textures/TCWposy.png", "../Textures/TCWnegy.png", "../Textures/TCWposz.png", "../Textures/TCWnegz.png" };
const char * textureFiles[NUM_TEXTURES] = { "../Textures/metal.jpg", "../Textures/wood.jpg" };

const char * vertexShaderNames[NUM_SHADERS] = { "../Shaders/SkyboxVertexShader.txt", "../Shaders/ParticleVertexShader.txt", "../Shaders/BasicTextureVertexShader.txt", "../Shaders/LightVertexShader.txt", "../Shaders/LightTextureVertexShader.txt" };
const char * fragmentShaderNames[NUM_SHADERS] = { "../Shaders/SkyboxFragmentShader.txt", "../Shaders/ParticleFragmentShader.txt", "../Shaders/BasicTextureFragmentShader.txt", "../Shaders/LightFragmentShader.txt", "../Shaders/LightTextureFragmentShader.txt" };

void initText()
{
	titleText[0] = add_text("The lamp who lost its light", -0.9f, 0.3f, 70.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	centre_text(titleText[0], 0.5f, 0.4f);
	titleText[1] = add_text("Continue...", -0.95f, 0.9f, 60.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	centre_text(titleText[1], 0.5f, -0.2f);

	introText[0] = add_text("Once upon a time, \nthere was a small white lamp...", -0.95f, 0.95f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	introText[1] = add_text("...and like most lamps are want to do, \nthis lamp wanted to light up the world.", -0.95f, 0.95f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	introText[2] = add_text("Unfortunately, despite trying its best, \nthe lamp had one little problem...", -0.95f, 0.95f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	introText[3] = add_text("It kept losing its light.", -0.95f, 0.95f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	introText[4] = add_text("This is the story of its quest to keep it.", -0.95f, 0.95f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	centre_text(introText[0], 0.5f, 0.2f);
	centre_text(introText[1], 0.5f, 0.2f);
	centre_text(introText[2], 0.5f, 0.2f);
	centre_text(introText[3], 0.5f, 0.2f);
	centre_text(introText[4], 0.5f, 0.2f);

	//gameText[0] = add_text("The lamp who lost its light", -0.95f, 0.95f, 25.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	//gameText[1] = add_text("Continue...", -0.95f, 0.9f, 25.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void changeGameState(int state)
{
	switch (state)
	{
	case INTRO:
		for (int i = 0; i < sizeof(titleText); i++)
			change_text_colour(titleText[i], 1.0f, 1.0f, 1.0f, 0.0f);
		change_text_colour(introText[0], 1.0f, 1.0f, 1.0f, 1.0f);

		gameState = INTRO;
		startTime = time(0);
		break;
	case GAME:
		gameState = GAME;
		break;
	}
}

void display()
{
	// Tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST);	// Enable depth-testing
	glDepthFunc(GL_LESS);		// Depth-testing interprets a smaller value as "closer"
	glClearColor(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (gameState)
	{
	case TITLE:
		break;
	case INTRO:
		break;
	case GAME:
		//mat4 view = look_at(camera.Position, lampPosition + vec3(0.0f, yOffset, 0.0f), camera.WorldUp);//  camera.GetViewMatrix();
		mat4 view = look_at(camera.Position, vec3(lampPosition.v[0], yOffset, lampPosition.v[2]), camera.WorldUp);//  camera.GetViewMatrix();
		mat4 projection = perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		vec4 view_position = vec4(camera.Position.v[0], camera.Position.v[1], camera.Position.v[2], 0.0f);
		lampSkeleton.drawSkeleton(view, projection, view_position);

		if (light.active)
			light.drawParticle(view, projection, view_position);

		mat4 ground_model = scale(identity_mat4(), vec3(100.0f, 0.0f, 100.0f));
		groundMesh.drawMesh(view, projection, ground_model, vec4(0.0f, 0.0f, 0.0f, 1.0f), view_position);

		break;
	}

	draw_texts();

	glutSwapBuffers();
}

void processInput()
{
	switch (gameState)
	{
	case GAME:
		if (animationMode == IDLE)
		{
			if (keys[GLUT_KEY_UP])
			{
				firstAnimation = true;
				animationMode = PRE_JUMP_FORWARD;
				//lampSkeleton.moveLamp(MOVE_FORWARD, movementSpeed);
			}
			if (keys[GLUT_KEY_DOWN])
				lampSkeleton.moveLamp(MOVE_BACKWARD, movementSpeed);
			if (keys[GLUT_KEY_LEFT])
				lampSkeleton.turnLamp(TURN_LEFT, movementSpeed / 10.0f);
			if (keys[GLUT_KEY_RIGHT])
				lampSkeleton.turnLamp(TURN_RIGHT, movementSpeed / 10.0f);

			if (keys['v'])
			{
				firstAnimation = true;
				animationMode = PRE_JUMP;
			}
			else if (keys['b'])
			{
				animationMode = LOOK_LEFT;
			}
		}

		if (keys['u'])
		{
			cameraPitch += cameraSpeed;
			if (cameraPitch > 1.57f)
				cameraPitch = 1.57f;
		}
		if (keys['j'])
		{
			cameraPitch -= cameraSpeed;
			if (cameraPitch < 0.0f)
				cameraPitch = 0.0f;
		}
		if (keys['h'])
			cameraYaw -= cameraSpeed;
		if (keys['k'])
			cameraYaw += cameraSpeed;

		if (keys['o'])
		{
			light.respawnParticle(lampPosition + vec3(0.0f, 5.0f, 0.0f));
			lightTimer = (long)milliseconds_now();
			lampMode = LAMP_OFF;
			lampSkeleton.setLightColour(lampLight[lampMode]);
		}

		break;
	}

	// Close the window if 'Esc' is pressed
	if (keys[(char)27])
		exit(0);
}

void updateIntro()
{
	GLfloat seconds_since_start = (GLfloat)difftime(time(0), startTime);
	if(seconds_since_start > introTime)
	{
		//currentTime = 0.0f;

		change_text_colour(introText[currentIntroText], 1.0f, 1.0f, 1.0f, 0.0f);
		currentIntroText++;

		if (currentIntroText >= numIntroTexts)
			changeGameState(GAME);
		else
		{
			change_text_colour(introText[currentIntroText], 1.0f, 1.0f, 1.0f, 1.0f);
			startTime = time(0);
		}
	}
}

void updateAnimation()
{
	//if (firstAnimation) startTime = system_clock::to_time_t(system_clock::now()); //startTime = time(0);
	//if (firstAnimation) 
	//	currentTime = time_point_cast<milliseconds>(system_clock::now()); //startTime = time(0);
	if (firstAnimation) currentTime = (long)milliseconds_now();


	//double seconds_since_start = (system_clock::to_time_t(system_clock::now()), startTime);
	//GLfloat seconds_since_start = (GLfloat)(system_clock::now() - currentTime).count();
	long milliseconds_since_start = (long)milliseconds_now() - currentTime;
	//if (seconds_since_start > animationTime)
	//{
	//	startTime = time(0);
	//	seconds_since_start = (GLfloat)difftime(time(0), startTime);
	//}

	/*if (currentTime <= 1.0f)
		currentTime += timeChange;
	else
	{
		currentTime = 0.0f;
		//animationMode = -1;
	}*/

	GLfloat current_time = (GLfloat)milliseconds_since_start / (1000.0f * animationTime);

	switch (animationMode)
	{
	case PRE_JUMP:
	case POST_JUMP:
		lampSkeleton.preAndPostJumpLamp(current_time, firstAnimation, animationMode);
		break;
	case DO_JUMP:
		lampSkeleton.jumpLamp(current_time, firstAnimation, animationMode);
		break;
	/*case LOOK_LEFT:
		lampSkeleton.lookLeftLamp(currentTime, timeChange, animationMode);
		break;
	case LOOK_RIGHT:
		lampSkeleton.lookRightLamp(currentTime, timeChange, animationMode);
		break;*/
	case PRE_JUMP_FORWARD:
	case POST_JUMP_FORWARD:
		lampSkeleton.preAndPostJumpForwardLamp(current_time, firstAnimation, animationMode);
		break;
	case JUMP_FORWARD:
		lampSkeleton.jumpForwardLamp(current_time, firstAnimation, animationMode);
		break;
	}
}

void checkCollision()
{
	long milliseconds_since_start = (long)milliseconds_now() - lightTimer;
	GLfloat current_time = (GLfloat)milliseconds_since_start / 1000.0f;

	if (current_time > 1.0f)
	{
		float x1 = lampPosition.v[0];
		float z1 = lampPosition.v[2];
		float x2 = light.position.v[0];
		float z2 = light.position.v[2];

		float x1_x2 = x1 - x2;
		float z1_z2 = z1 - z2;

		float x1_x2_2 = x1_x2 * x1_x2;
		float z1_z2_2 = z1_z2 * z1_z2;

		if (sqrt(x1_x2_2 + z1_z2_2) < 2.5f)
		{
			light.active = false;
			lampMode = LAMP_ON;
			lampSkeleton.setLightColour(lampLight[lampMode]);
		}
	}
}

void updateScene()
{
	processInput();

	switch (gameState)
	{
	case TITLE:
		break;
	case INTRO:
		updateIntro();
		break;
	case GAME:
		if (animationMode != IDLE)
			updateAnimation();

		lampPosition = lampSkeleton.getRootPosition();

		if (light.active)
		{
			light.updatePosition();
			checkCollision();
		}

		camera.UpdateCamera3rdPerson(cameraPitch, cameraYaw, lampPosition, cameraDistance, yOffset);

		break;
	}
	
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

	groundMesh = Mesh(&shaderProgramID[LIGHT_TEXTURE_SHADER]);
	groundMesh.generateObjectBufferMesh(meshFiles[GROUND_MESH]);
	groundMesh.loadTexture(textureFiles[WOOD_TEXTURE]);
}

void init()
{
	// Initialise the text rendering and display
	if (!init_text_rendering("../Textures/SpiraxReg.png", "../Textures/SpiraxReg.meta", screenWidth, screenHeight))
	{
		fprintf(stderr, "ERROR init text rendering\n");
		exit(1);
	}
	initText();

	// Compile the shaders
	for (int i = 0; i < NUM_SHADERS; i++)
	{
	shaderProgramID[i] = CompileShaders(vertexShaderNames[i], fragmentShaderNames[i]);
	}

	// Create all of the meshes
	initialiseMeshes();

	lampSkeleton.createLamp(lampBaseMesh, lampArmLowerMesh, lampArmUpperMesh, lampHeadMesh, lampLightMesh);
	lampSkeleton.setLightColour(lampLight[lampMode]);

	light = LightParticle(false, 1.0f, lightMesh, vec3(), vec3(), vec3(), vec4(0.85f, 0.75f, 0.0f, 0.5f));
}

/*
*	User Input Functions
*/
#pragma region USER_INPUT_FUNCTIONS
void pressNormalKeys(unsigned char key, int x, int y)
{
	keys[key] = true;
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
{
	if (state == GLUT_DOWN)
	{
		if (gameState == TITLE)
		{
			if (x > 370 && x < 620 && y > 500 && y < 560)
				changeGameState(INTRO);
		}
	}
}

void processMouse(int x, int y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	switch (gameState)
	{
	case TITLE:
		if(x > 370 && x < 620 && y > 500 && y < 560)
			change_text_colour(titleText[1], 0.388f, 1.0f, 1.0f, 0.8f);
		else
			change_text_colour(titleText[1], 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}

	/*int xoffset = x - lastX;
	int yoffset = lastY - y;

	lastX = x;
	lastY = y;

	camera.ProcessMouseMovement((GLfloat)xoffset, (GLfloat)yoffset);*/
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
	glutCreateWindow("The lamp who lost its light");

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