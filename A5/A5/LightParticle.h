#pragma once

#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include "Mesh.h"

GLfloat deltaTime = 1.0f / 60.0f;
GLfloat friction = 0.0f;
GLfloat restitution = 1.0f;
vec3 gravity = vec3(0.0f, -9.81f, 0.0f);
vec3 groundVector = vec3(0.0f, 0.0f, 0.0f);
vec3 groundNormal = vec3(0.0f, 1.0f, 0.0f);

// Plane axes
vec3 yAxis = vec3(0.0f, 1.0f, 0.0f);
vec3 xAxis = vec3(1.0f, 0.0f, 0.0f);
vec3 zAxis = vec3(0.0f, 0.0f, 1.0f);

class LightParticle {
public:
	bool active;
	GLfloat mass;
	Mesh mesh;
	vec3 force;
	vec3 position;
	vec3 velocity;
	vec4 colour;

	LightParticle();
	LightParticle(bool active, GLfloat mass, Mesh mesh, vec3 force, vec3 position, vec3 velocity, vec4 colour);
	void checkPlaneCollision(vec3 plane, vec3 normal);
	void drawParticle(mat4 view, mat4 projection, vec4 viewPosition);
	void respawnParticle(vec3 position);
	void updatePosition();
};

LightParticle::LightParticle()
{
	active = true;
	mass = 1.0f;
	mesh = Mesh();
	force = vec3(0.0f, 0.0f, 0.0f);
	position = vec3(0.0f, 0.0f, 0.0f);
	velocity = vec3(0.0f, 0.0f, 0.0f);
	colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

LightParticle::LightParticle(bool active, GLfloat mass, Mesh mesh, vec3 force, vec3 position, vec3 velocity, vec4 colour)
{
	this->active = active;
	this->mass = mass;
	this->mesh = mesh;
	this->force = force;
	this->position = position;
	this->velocity = velocity;
	this->colour = colour;
}

void LightParticle::checkPlaneCollision(vec3 plane, vec3 normal)
{
	if ((dot((position - plane), normal) < 0.0f) && (dot(normal, velocity) < 0.0f))
	{
		vec3 velocityNormal = normal * (dot(velocity, normal));
		vec3 velocityTangent = velocity - velocityNormal;
		velocity = (velocityTangent * (1 - friction)) - (velocityNormal * restitution);
		position -= normal * (2 * (dot((position - plane), normal)));
	}
}

void LightParticle::drawParticle(mat4 view, mat4 projection, vec4 viewPosition)
{
	mat4 model = scale(identity_mat4(), vec3(0.25f, 0.25f, 0.25f));
	model = translate(model, position);
	mesh.drawMesh(view, projection, model, colour, viewPosition);
}

void LightParticle::respawnParticle(vec3 position)
{
	active = true;
	this->position = position;
	float randomX = (rand() % 100 - 50) / 50.0f;
	float randomZ = (rand() % 100 - 50) / 50.0f;
	this->velocity = vec3(randomZ, 0.0f, randomX);
}



void LightParticle::updatePosition()
{
	// Update forces
	force = gravity * mass;

	// Update the velocity
	velocity += (force / mass) * deltaTime;

	// Update the position
	position += velocity * deltaTime;

	// Check for collision
	checkPlaneCollision(groundVector, yAxis);
	checkPlaneCollision(xAxis * -50.0f, xAxis);
	checkPlaneCollision(xAxis * 50.0f, xAxis * -1.0f);
	checkPlaneCollision(zAxis * -50.0f, zAxis);
	checkPlaneCollision(zAxis * 50.0f, zAxis * -1.0f);
}
