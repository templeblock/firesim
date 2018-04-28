#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace glm;

void Camera::init(double width, double height) {
	camPos = vec3(0.f, 0.f, 2.f);
	camTarget = vec3(0.f, 0.f, 0.f);
	changeScreenDimens(width, height);
	calculateAxes();
	getViewMatrix();
}

void Camera::changeScreenDimens(double width, double height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void Camera::calculateAxes() {
	TargetToCam = normalize(camPos - camTarget);
	vec3 up = vec3(0.f, 1.f, 0.f);

	camRight = cross(up, TargetToCam);
	if (length(camRight) != 0) { //avoid NaN by checking for 0 magnitude
		camRight = normalize(camRight);
	}
	camUp = cross(TargetToCam, camRight);
	if (length(camUp) != 0) { //avoid NaN by checking for 0 magnitude
		camUp = normalize(camUp);
	}
}

mat4 Camera::getViewMatrix() {
	w2c = lookAt(camPos, camTarget, camUp);
	//std::cout << camUp.x <<" ," << camUp.y << " ," << camUp.z << " ";
	//std::cout << w2c[0][1];
	return w2c;
}

void Camera::move(vec3 moveBy) {
	camPos += moveBy;
	if (camPos.z < 0.1f) camPos.z = 0.1f;
	calculateAxes();
}

void Camera::rotateBy(double deltaX, double deltaY) {
	float dist = length(camPos - camTarget);
	//XZ (Horizontal)
	if (camPos.z < 0) {
		camPos.x -= deltaX / SCR_HEIGHT * 2.;
	} else {
		camPos.x += deltaX / SCR_HEIGHT * 2.;
	}
		
	if (camPos.x < 0) {
		camPos.z += deltaX / SCR_HEIGHT;
	}
	else {
		camPos.z -= deltaX / SCR_HEIGHT;
	}
	vec3 direction = camPos - camTarget;
	camPos = camTarget + (normalize(direction) * dist);

	//YZ (Vertical)
	if (camPos.y < 0) {
		camPos.z -= deltaY / SCR_HEIGHT;
	}
	else {
		camPos.z += deltaY / SCR_HEIGHT;
	}
	if (camPos.z < 0) {
		camPos.y += deltaY / SCR_HEIGHT * 2.;
	}
	else {
		camPos.y -= deltaY / SCR_HEIGHT * 2.;
	}

	direction = camPos - camTarget;
	camPos = camTarget + (normalize(direction) * dist);

	calculateAxes();
}