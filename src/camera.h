#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

class Camera {
public:

	Camera() {}
	void init();

	//Cam position and cam target position
	vec3 camPos, camTarget;

	//Axes of camera coordinate system
	vec3 TargetToCam, camRight, camUp;

	//View Matrix
	mat4 w2c;

	//Spherical orientation from target
	double phi, theta, r;

	void calculateAxes();
	mat4 getViewMatrix();

	void move(vec3 moveBy);

private:

};
#endif