#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

class Camera {
public:

	Camera() {}
	void init();

	/*****************************************/
	/* Position, Orientation, Movement, etc. */
	/*****************************************/

	//Cam position and cam target position
	vec3 camPos, camTarget;

	//Axes of camera coordinate system
	vec3 TargetToCam, camRight, camUp;

	//View Matrix
	mat4 w2c;

	void calculateAxes();
	mat4 getViewMatrix();

	void move(vec3 moveBy);

	/*****************/
	/* Viewing Plane */
	/*****************/

	//camera space to world space func
	//generate ray func

private:

};
#endif