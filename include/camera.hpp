#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp"
#include <cmath>
#include <math.h>

enum MoveDirection
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Camera
{
public:
    // camera status
    vec3 position;
    vec3 front;
    vec3 top;
    vec3 right;
    float lookAtDistance;
    // perspective status
    float fieldOfView;
    float aspect;
    float nearDinstace;
    float farDinstace;
    // move parameter
    float moveSpeed;
    // trackball parameter
    float trackballSpeed;
    float theta;
    float phi;

    Camera();

    Camera& withPosition(vec3 val);
    Camera& withFieldOfView(float val);
    Camera& withAspect(float val);
    Camera& withNear(float val);
    Camera& withFar(float val);
    Camera& withMoveSpeed(float val);
    Camera& withTrackballSpeed(float val);
    Camera& withTheta(float val);
    Camera& withPhi(float val);

    void setPosition(vec3 val);
    void setLookAt(vec3 val);

    vec3 getPosition();
    vec3 getLookAt();
    mat4 getPerspective();
    mat4 getView();
    mat4 getRotate();

    void processMove(MoveDirection moveDirction, float timeDifferent);
    void processTrackball(float thetaDifferent, float phiDifferent);

private:
    void updateCameraPositionStatus();

    void updateCameraRotationStatus();
};


#endif
