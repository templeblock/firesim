#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

void Camera::init() {
	camPos = vec3(0.f, 0.f, 1.f);
	camTarget = vec3(0.f, 0.f, 0.f);
	calculateAxes();
	getViewMatrix();
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
	calculateAxes();
}