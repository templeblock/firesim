#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;

class Camera {
public:
    Camera() {}

    void init(double width, double height);

    void changeScreenDimens(double width, double height);

    /*****************************************/
    /* Position, Orientation, Movement, etc. */
    /*****************************************/
    //Screen Dimensions
    double SCR_WIDTH, SCR_HEIGHT;

    //Cam position and cam target position
    vec3 camPos, camTarget;

    //Axes of camera coordinate system
    vec3 TargetToCam, camRight, camUp;

    //View Matrix
    mat4 w2c;

    void calculateAxes();

    mat4 getViewMatrix();

    void move(vec3 moveBy);

    void rotateBy(double deltaX, double deltaY);

    /*****************/
    /* Viewing Plane */
    /*****************/

    //camera space to world space func
    //generate ray func

private:
};

#endif