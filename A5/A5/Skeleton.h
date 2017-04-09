#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#include <vector>				// STL dynamic memory
#include <windows.h>

#include "Antons_maths_funcs.h" // Anton's maths functions
#include "Bone.h"
#include "InverseKinematics.h"
#include "Mesh.h"

enum Movements { JUMP, TURN_LEFT, TURN_RIGHT, MOVE_FORWARD, MOVE_BACKWARD };
enum Animations { IDLE, PRE_JUMP, DO_JUMP, POST_JUMP, LOOK_LEFT, LOOK_RIGHT, PRE_JUMP_FORWARD, JUMP_FORWARD, POST_JUMP_FORWARD, NUM_ANIMATIONS };
enum Bones {BASE, LOWER_ARM, UPPER_ARM, HEAD, LIGHT};

class Skeleton {
public:
	Bone* rootBone;
	Bone* bones[20];
	int numBones = 20;
	int handIndex = 4;

	Skeleton();
	//void createLeftHand(Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell, int startIndex);
	void createRightHand(Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell, int startIndex);
	void createTorso(Mesh torsoMesh, Mesh shoulderMesh, Mesh shoulderShell, Mesh upperArmShell, Mesh lowerArmShell, Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell);
	void createLamp(Mesh base, Mesh arm_lower, Mesh arm_upper, Mesh head, Mesh light);
	void drawSkeleton(mat4 view, mat4 projection, vec4 viewPosition);
	void rotateWrist360();
	void closeFist();
	void openFist();
	void closeAndOpenFist();
	void moveTo(vec3 position);
	void moveToCCD(vec3 position, bool hand);
	void moveLamp(int direction, GLfloat distance);
	void turnLamp(int direction, GLfloat rotation);
	void preAndPostJumpLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode);
	void jumpLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode);
	void preAndPostJumpForwardLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode);
	void jumpForwardLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode);
	//void lookLeftLamp(GLfloat time, GLfloat timeChange, GLuint &animationMode);
	//void lookRightLamp(GLfloat time, GLfloat timeChange, GLuint &animationMode);
	vec3 getRootPosition();
	void setLightColour(vec4 colour);

private:
	bool close = true;
	bool once = false;

	GLfloat count = 0.0f;
	GLfloat speed = 0.15f;
	GLfloat theta1 = 0.0f, theta2 = 0.0f, theta3 = 0.0f;
	GLfloat theta4 = 0.0f, theta5 = 0.0f, theta6 = 0.0f;

	vec4 joint_colour = vec4(1.0f, 1.0f, 1.0f, 2.0f);
	vec4 shell_colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 light_colour = vec4(0.85f, 0.75f, 0.0f, 0.5f);

	vec3 jumpBase[4], jumpHead[4];
	vec3 lookPoints[4];
};

Skeleton::Skeleton()
{

}

/*void Skeleton::createLeftHand(Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell, int startIndex = 0)
{
	// Hand
	if (startIndex == 0)
	{
		bones[startIndex + 0] = new Bone("right_hand", true, scale(identity_mat4(), vec3(0.2f, 0.2f, 0.2f)), handMesh, handShell, true, joint_colour, shell_colour);
		rootBone = bones[startIndex + 0];
	}
	else
	{
		mat4 hand_local = scale(identity_mat4(), vec3(0.3f, 0.3f, 0.3f));
		hand_local = rotate_y_deg(hand_local, 180.0f);
		hand_local = translate(hand_local, vec3(-7.0f, 0.0f, 0.0f));
		bones[startIndex + 0] = new Bone("right_hand", bones[startIndex - 1], hand_local, handMesh, handShell, true, joint_colour, shell_colour);
		bones[startIndex - 1]->addChild(bones[startIndex + 0]);
		bones[startIndex + 0]->rollJoint(radians(90.0f));
	}

	// Thumb
	mat4 thumb1_local = scale(identity_mat4(), vec3(0.75f, 1.0f, 1.0f) * 0.8f);
	thumb1_local = translate(thumb1_local, vec3(3.0f, 0.0f, 2.5f));
	bones[startIndex + 1] = new Bone("right_thumb_1", bones[startIndex + 0], thumb1_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 1]);
	bones[startIndex + 1]->pivotJoint(radians(30.0f));
	bones[startIndex + 1]->rollJoint(radians(45.0f));

	mat4 thumb2_local = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 2] = new Bone("right_thumb_2", bones[startIndex + 1], thumb2_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 1]->addChild(bones[startIndex + 2]);
	bones[startIndex + 2]->bendJoint(radians(2.0f));

	mat4 thumb3_local = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 3] = new Bone("right_thumb_3", bones[startIndex + 2], thumb3_local, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 2]->addChild(bones[startIndex + 3]);
	bones[startIndex + 3]->bendJoint(radians(5.0f));

	// Finger 1
	mat4 finger11_local = scale(identity_mat4(), vec3(0.9f, 0.9f, 0.9f) * 0.8f);
	finger11_local = translate(finger11_local, vec3(-4.0f, 0.0f, 3.1f));
	bones[startIndex + 4] = new Bone("right_finger_11", bones[startIndex + 0], finger11_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 4]);

	mat4 finger_joint_offset = scale(identity_mat4(), vec3(0.85f, 0.85f, 0.85f));
	finger_joint_offset = translate(finger_joint_offset, vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 5] = new Bone("right_finger_12", bones[startIndex + 4], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 4]->addChild(bones[startIndex + 5]);

	mat4 finger_tip_offset = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 6] = new Bone("right_finger_13", bones[startIndex + 5], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 5]->addChild(bones[startIndex + 6]);

	// Finger 2
	mat4 finger21_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f) * 0.8f);
	finger21_local = translate(finger21_local, vec3(-4.0f, 0.0f, 0.9f));
	bones[startIndex + 7] = new Bone("right_finger_21", bones[startIndex + 0], finger21_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 7]);

	bones[startIndex + 8] = new Bone("right_finger_22", bones[startIndex + 7], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 7]->addChild(bones[startIndex + 8]);

	bones[startIndex + 9] = new Bone("right_finger_23", bones[startIndex + 8], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 8]->addChild(bones[startIndex + 9]);

	// Finger 3
	mat4 finger31_local = scale(identity_mat4(), vec3(0.9f, 0.9f, 0.9f) * 0.8f);
	finger31_local = translate(finger31_local, vec3(-4.0f, 0.0f, -1.3f));
	bones[startIndex + 10] = new Bone("right_finger_31", bones[startIndex + 0], finger31_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 10]);

	bones[startIndex + 11] = new Bone("right_finger_32", bones[startIndex + 10], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 10]->addChild(bones[startIndex + 11]);

	bones[startIndex + 12] = new Bone("right_finger_33", bones[startIndex + 11], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 11]->addChild(bones[startIndex + 12]);

	// Finger 4
	mat4 finger41_local = scale(identity_mat4(), vec3(0.7f, 0.7f, 0.7f) * 0.8f);
	finger41_local = translate(finger41_local, vec3(-4.0f, 0.0f, -3.1f));
	bones[startIndex + 13] = new Bone("right_finger_41", bones[startIndex + 0], finger41_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 13]);

	bones[startIndex + 14] = new Bone("right_finger_42", bones[startIndex + 13], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 13]->addChild(bones[startIndex + 14]);

	bones[startIndex + 15] = new Bone("right_finger_43", bones[startIndex + 14], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 14]->addChild(bones[startIndex + 15]);
}*/

void Skeleton::createRightHand(Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell, int startIndex = 0)
{
	// Hand
	if (startIndex == 0)
	{
		bones[startIndex + 0] = new Bone("right_hand", true, scale(identity_mat4(), vec3(0.2f, 0.2f, 0.2f)), handMesh, handShell, true, joint_colour, shell_colour);
		rootBone = bones[startIndex + 0];
	}
	else
	{
		mat4 hand_local = scale(identity_mat4(), vec3(0.3f, 0.3f, 0.3f));
		hand_local = translate(hand_local, vec3(-6.0f, 0.0f, 0.0f));
		bones[startIndex + 0] = new Bone("right_hand", bones[startIndex - 1], hand_local, handMesh, handShell, true, joint_colour, shell_colour);
		bones[startIndex - 1]->addChild(bones[startIndex + 0]);
		bones[startIndex + 0]->rollJoint(radians(90.0f));
	}

	// Thumb
	mat4 thumb1_local = scale(identity_mat4(), vec3(0.75f, 1.0f, 1.0f) * 0.8f);
	thumb1_local = translate(thumb1_local, vec3(-1.0f, 0.0f, 2.5f));
	bones[startIndex + 1] = new Bone("right_thumb_1", bones[startIndex + 0], thumb1_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 1]);
	bones[startIndex + 1]->pivotJoint(radians(30.0f));
	bones[startIndex + 1]->rollJoint(radians(45.0f));

	mat4 thumb2_local = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 2] = new Bone("right_thumb_2", bones[startIndex + 1], thumb2_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 1]->addChild(bones[startIndex + 2]);
	bones[startIndex + 2]->bendJoint(radians(2.0f));

	mat4 thumb3_local = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 3] = new Bone("right_thumb_3", bones[startIndex + 2], thumb3_local, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 2]->addChild(bones[startIndex + 3]);
	bones[startIndex + 3]->bendJoint(radians(5.0f));

	// Finger 1
	mat4 finger11_local = scale(identity_mat4(), vec3(0.9f, 0.9f, 0.9f) * 0.8f);
	finger11_local = translate(finger11_local, vec3(-8.0f, 0.0f, 3.1f));
	bones[startIndex + 4] = new Bone("right_finger_11", bones[startIndex + 0], finger11_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 4]);

	mat4 finger_joint_offset = scale(identity_mat4(), vec3(0.85f, 0.85f, 0.85f));
	finger_joint_offset = translate(finger_joint_offset, vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 5] = new Bone("right_finger_12", bones[startIndex + 4], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 4]->addChild(bones[startIndex + 5]);

	mat4 finger_tip_offset = translate(identity_mat4(), vec3(-5.0f, 0.0f, 0.0f));
	bones[startIndex + 6] = new Bone("right_finger_13", bones[startIndex + 5], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 5]->addChild(bones[startIndex + 6]);

	// Finger 2
	mat4 finger21_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f) * 0.8f);
	finger21_local = translate(finger21_local, vec3(-8.0f, 0.0f, 0.9f));
	bones[startIndex + 7] = new Bone("right_finger_21", bones[startIndex + 0], finger21_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 7]);

	bones[startIndex + 8] = new Bone("right_finger_22", bones[startIndex + 7], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 7]->addChild(bones[startIndex + 8]);

	bones[startIndex + 9] = new Bone("right_finger_23", bones[startIndex + 8], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 8]->addChild(bones[startIndex + 9]);

	// Finger 3
	mat4 finger31_local = scale(identity_mat4(), vec3(0.9f, 0.9f, 0.9f) * 0.8f);
	finger31_local = translate(finger31_local, vec3(-8.0f, 0.0f, -1.3f));
	bones[startIndex + 10] = new Bone("right_finger_31", bones[startIndex + 0], finger31_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 10]);

	bones[startIndex + 11] = new Bone("right_finger_32", bones[startIndex + 10], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 10]->addChild(bones[startIndex + 11]);

	bones[startIndex + 12] = new Bone("right_finger_33", bones[startIndex + 11], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 11]->addChild(bones[startIndex + 12]);

	// Finger 4
	mat4 finger41_local = scale(identity_mat4(), vec3(0.7f, 0.7f, 0.7f) * 0.8f);
	finger41_local = translate(finger41_local, vec3(-8.0f, 0.0f, -3.1f));
	bones[startIndex + 13] = new Bone("right_finger_41", bones[startIndex + 0], finger41_local, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 0]->addChild(bones[startIndex + 13]);

	bones[startIndex + 14] = new Bone("right_finger_42", bones[startIndex + 13], finger_joint_offset, jointMesh, jointShell, true, joint_colour, shell_colour);
	bones[startIndex + 13]->addChild(bones[startIndex + 14]);

	bones[startIndex + 15] = new Bone("right_finger_43", bones[startIndex + 14], finger_tip_offset, tipMesh, tipShell, true, joint_colour, shell_colour);
	bones[startIndex + 14]->addChild(bones[startIndex + 15]);
}

void Skeleton::createTorso(Mesh torsoMesh, Mesh shoulderMesh, Mesh shoulderShell, Mesh upperArmShell, Mesh lowerArmShell, Mesh handMesh, Mesh handShell, Mesh jointMesh, Mesh jointShell, Mesh tipMesh, Mesh tipShell)
{

	// Torso
	mat4 torso_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	//torso_local = translate(torso_local, vec3(-5.0f, 0.0f, 0.0f));
	bones[0] = new Bone("torso", true, torso_local, torsoMesh, Mesh(), false, joint_colour);
	rootBone = bones[0];

	// Shoulders
	mat4 shoulder_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	shoulder_local = translate(shoulder_local, vec3(0.0f, 7.5f, 0.0f));
	bones[1] = new Bone("shoulders", bones[0], shoulder_local, shoulderMesh, shoulderShell, true, joint_colour, shell_colour);
	bones[0]->addChild(bones[1]);

	// Upper Arm
	mat4 upper_arm_local = scale(identity_mat4(), vec3(1.2f, 1.2f, 1.2f) * 0.8f);
	upper_arm_local = translate(upper_arm_local, vec3(-2.5f, 0.0f, 0.0f));
	bones[2] = new Bone("upper_arm", bones[1], upper_arm_local, jointMesh, upperArmShell, true, joint_colour, shell_colour);
	bones[1]->addChild(bones[2]);
	bones[2]->rollJoint(radians(-90.0f));
	//bones[2]->pivotJoint(radians(-70.0f));

	// Lower Arm
	mat4 lower_arm_local = scale(identity_mat4(), vec3(0.9f, 0.9f, 0.9f));
	lower_arm_local = translate(lower_arm_local, vec3(-5.0f, 0.0f, 0.0f));
	bones[3] = new Bone("lower_arm", bones[2], lower_arm_local, jointMesh, lowerArmShell, true, joint_colour, shell_colour);
	bones[2]->addChild(bones[3]);
	//bones[3]->pivotJoint(radians(-20.0f));

	createRightHand(handMesh, handShell, jointMesh, jointShell, tipMesh, tipShell, 4);
}

void Skeleton::createLamp(Mesh base, Mesh arm_lower, Mesh arm_upper, Mesh head, Mesh light)
{
	// Base
	mat4 base_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	//torso_local = translate(torso_local, vec3(-5.0f, 0.0f, 0.0f));
	bones[0] = new Bone("base", true, base_local, base, Mesh(), false, joint_colour);
	rootBone = bones[0];

	// Lower arm
	mat4 lower_arm_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	bones[1] = new Bone("lower_arm", bones[0], lower_arm_local, arm_lower, Mesh(), false, joint_colour);
	bones[0]->addChild(bones[1]);

	// Upper Arm
	mat4 upper_arm_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	upper_arm_local = translate(upper_arm_local, vec3(0.0f, 3.150f, 0.0f));
	bones[2] = new Bone("upper_arm", bones[1], upper_arm_local, arm_upper, Mesh(), false, joint_colour);
	bones[1]->addChild(bones[2]);

	// Head
	mat4 head_local = scale(identity_mat4(), vec3(1.0f, 1.0f, 1.0f));
	head_local = translate(head_local, vec3(0.0f, 3.0f, 0.0f));
	bones[3] = new Bone("head", bones[2], head_local, head, Mesh(), false, joint_colour);
	bones[2]->addChild(bones[3]);

	// Light
	mat4 light_local = scale(identity_mat4(), vec3(1.1f, 1.1f, 1.1f));
	light_local = translate(light_local, vec3(0.0f, 0.0f, -1.5f));
	bones[4] = new Bone("head", bones[3], light_local, light, Mesh(), false, light_colour);
	bones[3]->addChild(bones[4]);

	analyticalIK(bones[HEAD]->getGlobalPosition() - vec4(0.0f, 1.0f, 0.0f, 0.0f) - rootBone->getGlobalPosition(), 3.15f, 3.0f, theta1, theta2, theta3, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

	bones[LOWER_ARM]->pivotJoint(-1.0f * radians(theta3));
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[LOWER_ARM]->bendJoint(1.0f * radians(theta1));

	//theta1 = theta2 = theta3 = 0.0f;
}

void Skeleton::drawSkeleton(mat4 view, mat4 projection, vec4 viewPosition = vec4(0.0f, 0.0f, 0.0f, 0.0f))
{
	rootBone->drawBone(view, projection, viewPosition);
}

void Skeleton::rotateWrist360()
{
	bones[handIndex]->rollJoint(radians(speed));
}

void Skeleton::closeFist()
{
	if (close)
	{
		count += speed;
		for (int i = handIndex + 1; i < handIndex + 4; i++)
			bones[i]->bendJoint(radians(speed * (i - handIndex) / 10.0f));
		for (int i = handIndex + 4; i < numBones; i++)
			bones[i]->bendJoint(radians(speed));
	}
	if (count >= 90.0f)
		close = false;
}

void Skeleton::openFist()
{
	if(!close)
	{
		count -= speed;
		for (int i = handIndex + 1; i < handIndex + 4; i++)
			bones[i]->bendJoint(radians(-speed * (i - handIndex) / 10.0f));
		for (int i = handIndex + 4; i < numBones; i++)
			bones[i]->bendJoint(radians(-speed));
	}

	if (count <= 0.0f)
		close = true;
}

void Skeleton::closeAndOpenFist()
{
	closeFist();
	openFist();
}

void Skeleton::moveTo(vec3 position)
{
	// Undo previous rotation
	bones[3]->pivotJoint(1.0f * radians(theta2));
	bones[2]->pivotJoint(1.0f * radians(theta1));

	vec3 start_position = vec3(-2.5f, 7.5f, 0.0f);
	//analyticalIK(position - start_position, 5.0f, 5.0f, theta1, theta2, theta3);

	// Rotate bones
	bones[2]->pivotJoint(-1.0f * radians(theta1));
	bones[3]->pivotJoint(-1.0f * radians(theta2));
}

void Skeleton::moveToCCD(vec3 position, bool hand)
{
	if (hand)
		CCDIK(position, handIndex, handIndex - 2, bones);
	else
		CCDIK(position, handIndex + 3, handIndex - 2, bones);
	//if (CCD)
	//	cout << "True. \n";
	//else
	//	cout << "False. \n";
}

void Skeleton::moveLamp(int direction, GLfloat distance)
{
	if (direction == MOVE_FORWARD)
	{
		vec4 newPosition = rootBone->getGlobalPosition() + (rootBone->forwardVector * distance);
		rootBone->setPosition(newPosition);
	}
	else if (direction == MOVE_BACKWARD)
	{
		vec4 newPosition = rootBone->getGlobalPosition() - (rootBone->forwardVector * distance);
		rootBone->setPosition(newPosition);
	}
}

void Skeleton::turnLamp(int direction, GLfloat rotation)
{
	if (direction == TURN_LEFT)
	{
		rootBone->pivotJoint(rotation);
	}
	else if (direction == TURN_RIGHT)
	{
		rootBone->pivotJoint(-1.0f * rotation);
	}
}

vec3 Skeleton::getRootPosition()
{
	return rootBone->getGlobalPosition();
}

void Skeleton::preAndPostJumpLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode)
{
	if (firstAnimation)
	{
		jumpHead[0] = jumpHead[3] = bones[HEAD]->getGlobalPosition();
		jumpHead[1] = jumpHead[2] = jumpHead[0] - rootBone->upVector * 1.5f;
		firstAnimation = false;
	}

	if (time >= 1.0f)
	{
		if (animationMode == PRE_JUMP)
			animationMode = DO_JUMP;
		else
			animationMode = IDLE;

		time = 1.0f;
		firstAnimation = true;
	}

	vec3 head_position = splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time);

	// Undo previous rotation
	bones[LOWER_ARM]->bendJoint(-1.0f * radians(theta1));
	bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
	bones[LOWER_ARM]->pivotJoint(-1.0f * radians(theta3));

	//Check the first parameter of this
	analyticalIK(head_position - rootBone->getGlobalPosition(), 3.15f, 3.0f, theta1, theta2, theta3, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

	bones[LOWER_ARM]->pivotJoint(1.0f * radians(theta3));
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[LOWER_ARM]->bendJoint(1.0f * radians(theta1));
}

void Skeleton::jumpLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode)
{
	if (firstAnimation)
	{
		jumpBase[0] = jumpBase[3] = rootBone->getGlobalPosition();
		jumpBase[1] = jumpBase[2] = jumpBase[0] + vec3(0.0f, 5.0f, 0.0f);
		
		jumpHead[0] = jumpHead[3] = bones[HEAD]->getGlobalPosition();
		jumpHead[1] = jumpHead[2] = jumpHead[0] + rootBone->upVector * 6.0f;

		firstAnimation = false;
	}

	if (time >= 1.0f)
	{
		animationMode = POST_JUMP;
		time = 1.0f;
		firstAnimation = true;
	}

	rootBone->setPosition(vec4(splinePositionBezier(jumpBase[0], jumpBase[1], jumpBase[2], jumpBase[3], time), 0.0f));
	//bones[HEAD]->setPosition(vec4(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), 0.0f));
	vec3 head_position = splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time);

	// Undo previous rotation
	bones[LOWER_ARM]->bendJoint(-1.0f * radians(theta1));
	bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
	bones[LOWER_ARM]->pivotJoint(-1.0f * radians(theta3));

	analyticalIK(head_position - rootBone->getGlobalPosition(), 3.15f, 3.0f, theta1, theta2, theta3, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

	bones[LOWER_ARM]->pivotJoint(1.0f * radians(theta3));
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[LOWER_ARM]->bendJoint(1.0f * radians(theta1));

	//CCDIK(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), HEAD, LOWER_ARM, bones);
}

void Skeleton::preAndPostJumpForwardLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode)
{
	if (firstAnimation)
	{
		jumpHead[0] = bones[HEAD]->getGlobalPosition();
		jumpHead[1] = jumpHead[2] = jumpHead[0] - rootBone->upVector * 1.5f;
		jumpHead[3] = jumpHead[0] + rootBone->forwardVector * 0.5f;
		firstAnimation = false;
	}

	if (time >= 1.0f)
	{
		if (animationMode == PRE_JUMP_FORWARD)
			animationMode = JUMP_FORWARD;
		else
			animationMode = IDLE;

		time = 1.0f;
		firstAnimation = true;
	}

	vec3 head_position = splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time);

	// Undo previous rotation
	bones[LOWER_ARM]->bendJoint(-1.0f * radians(theta1));
	bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
	bones[LOWER_ARM]->pivotJoint(-1.0f * radians(theta3));

	//Check the first parameter of this
	analyticalIK(head_position - rootBone->getGlobalPosition(), 3.15f, 3.0f, theta1, theta2, theta3, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

	bones[LOWER_ARM]->pivotJoint(1.0f * radians(theta3));
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[LOWER_ARM]->bendJoint(1.0f * radians(theta1));
}

void Skeleton::jumpForwardLamp(GLfloat time, bool &firstAnimation, GLuint &animationMode)
{
	if (firstAnimation)
	{
		jumpBase[0] = rootBone->getGlobalPosition();
		jumpBase[3] = jumpBase[0] + rootBone->forwardVector * 5.0f;
		jumpBase[1] = jumpBase[0] + vec3(0.0f, 5.0f, 0.0f);
		jumpBase[2] = jumpBase[3] + vec3(0.0f, 5.0f, 0.0f);

		jumpHead[0] = bones[HEAD]->getGlobalPosition();
		jumpHead[3] = jumpHead[0] + rootBone->forwardVector * 4.0f;
		jumpHead[1] = jumpHead[0] + rootBone->upVector * 6.0f;
		jumpHead[2] = jumpHead[3] + rootBone->upVector * 6.0f;
		firstAnimation = false;
	}

	if (time >= 1.0f)
	{
		animationMode = POST_JUMP_FORWARD;
		time = 1.0f;
		firstAnimation = true;
	}
	else
	{
		rootBone->setPosition(vec4(splinePositionBezier(jumpBase[0], jumpBase[1], jumpBase[2], jumpBase[3], time), 0.0f));
		//bones[HEAD]->setPosition(vec4(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), 0.0f));
		vec3 head_position = splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time);

		// Undo previous rotation
		bones[LOWER_ARM]->bendJoint(-1.0f * radians(theta1));
		bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
		bones[LOWER_ARM]->pivotJoint(-1.0f * radians(theta3));

		analyticalIK(head_position - rootBone->getGlobalPosition(), 3.15f, 3.0f, theta1, theta2, theta3, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

		bones[LOWER_ARM]->pivotJoint(1.0f * radians(theta3));
		bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
		bones[LOWER_ARM]->bendJoint(1.0f * radians(theta1));
	}

	//CCDIK(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), HEAD, LOWER_ARM, bones);
}

void Skeleton::setLightColour(vec4 colour)
{
	bones[LIGHT]->jointColour = colour;
}

/*void Skeleton::lookLeftLamp(GLfloat time, GLfloat timeChange, GLuint &animationMode)
{
	if (time <= timeChange)
	{
		lookPoints[0] = lookPoints[3] = bones[LIGHT]->getGlobalPosition();
		lookPoints[1] = lookPoints[0] - rootBone->rightVector * 1.5f + rootBone->upVector * 0.5f;
		lookPoints[2] = lookPoints[0] - rootBone->forwardVector * 1.0f -rootBone->rightVector * 2.0f + rootBone->upVector * 0.5f;

		//bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
		//bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
	}

	if (time == 0.0f)
	{
		animationMode = IDLE;
	}

	//bones[HEAD]->setPosition(vec4(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), 0.0f));
	vec3 light_position = splinePositionBezier(lookPoints[0], lookPoints[1], lookPoints[2], lookPoints[3], time);

	// Undo previous rotation
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[HEAD]->bendJoint(1.0f * radians(theta5));
	bones[UPPER_ARM]->pivotJoint(-1.0f * radians(theta6));

	analyticalIK(light_position - bones[UPPER_ARM]->getGlobalPosition(), 3.0f, 1.0f, theta2, theta5, theta6, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);
	theta2 = 90 + theta2;


	bones[UPPER_ARM]->pivotJoint(1.0f * radians(theta6));
	bones[HEAD]->bendJoint(-1.0f * radians(theta5));
	bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));

	//CCDIK(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), HEAD, LOWER_ARM, bones);
}

void Skeleton::lookRightLamp(GLfloat time, GLfloat timeChange, GLuint &animationMode)
{
	if (time <= timeChange)
	{
		lookPoints[0] = lookPoints[3] = bones[LIGHT]->getGlobalPosition();
		lookPoints[1] = lookPoints[0] + rootBone->rightVector * 1.5f + rootBone->upVector * 0.5f;
		lookPoints[2] = lookPoints[0] - rootBone->forwardVector * 1.0f + rootBone->rightVector * 2.0f + rootBone->upVector * 0.5f;
	}

	if (time == 0.0f)
	{
		animationMode = IDLE;
	}

	//bones[HEAD]->setPosition(vec4(splinePositionBezier(jumpHead[0], jumpHead[1], jumpHead[2], jumpHead[3], time), 0.0f));
	vec3 light_position = splinePositionBezier(lookPoints[0], lookPoints[1], lookPoints[2], lookPoints[3], time);

	// Undo previous rotation
	bones[UPPER_ARM]->bendJoint(-1.0f * radians(theta2));
	bones[HEAD]->bendJoint(1.0f * radians(theta5));
	bones[UPPER_ARM]->pivotJoint(-1.0f * radians(theta6));

	analyticalIK(light_position - bones[UPPER_ARM]->getGlobalPosition(), 3.0f, 1.0f, theta2, theta5, theta6, rootBone->forwardVector, rootBone->rightVector, rootBone->upVector);

	bones[UPPER_ARM]->pivotJoint(1.0f * radians(theta6));
	bones[HEAD]->bendJoint(-1.0f * radians(theta5));
	bones[UPPER_ARM]->bendJoint(1.0f * radians(theta2));
}*/