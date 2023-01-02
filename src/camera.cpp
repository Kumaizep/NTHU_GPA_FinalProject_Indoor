#include "camera.hpp"
#include "GLM/fwd.hpp"

Camera::Camera()
{
    position       = vec3(0.0f, 0.0f, 0.0f);
    front          = vec3(1.0f, 0.0f, 0.0f);
    top            = vec3(0.0f, 1.0f, 0.0f);
    right          = vec3(0.0f, 0.0f, 1.0f);
    lookAtDistance = 1.0f;
    fieldOfView    = 60.0f;
    aspect         = (float)INIT_WIDTH / (float)INIT_HEIGHT;
    nearDinstace   = 0.1f;
    farDinstace    = 1000.0f;
    moveSpeed      = 10.0f;
    trackballSpeed = 0.1f;
    theta          = 0.0f;
    phi            = 0.0f;
}

Camera& Camera::withPosition(vec3 val)
{
    position = val;
    return *this;
}

Camera& Camera::withFieldOfView(float val)
{
    fieldOfView = val;
    return *this;
}

Camera& Camera::withAspect(float val)
{
    aspect = val;
    return *this;
}

Camera& Camera::withNear(float val)
{
    nearDinstace = val;
    return *this;
}

Camera& Camera::withFar(float val)
{
    farDinstace = val;
    return *this;
}

Camera& Camera::withMoveSpeed(float val)
{
    moveSpeed = val;
    return *this;
}

Camera& Camera::withTrackballSpeed(float val)
{
    trackballSpeed = val;
    return *this;
}

Camera& Camera::withTheta(float val)
{
    theta = val;
    updateCameraPositionStatus();
    return *this;
}
Camera& Camera::withPhi(float val)
{
    if (val < 85 && val> -85)
        phi = val;
    updateCameraPositionStatus();
    return *this;
}

void Camera::setPosition(vec3 val)
{
    vec3 tempLookAt = position + front * lookAtDistance;
    position = val;
    setLookAt(tempLookAt);
}

void Camera::setLookAt(vec3 val)
{
    lookAtDistance = length(val - position);
    front = normalize(val - position);
    updateCameraRotationStatus();
    updateCameraPositionStatus();
}

vec3 Camera::getPosition()
{
    return position;
}

vec3 Camera::getLookAt()
{
    return lookAtDistance * front + position;
}

mat4 Camera::getPerspective()
{
    return perspective(radians(fieldOfView), aspect, nearDinstace, farDinstace);
}

mat4 Camera::getView()
{
    // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << position.x << " " << position.y << " " << position.z << endl;
    // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << front.x << " " << front.y << " " << front.z << endl;
    return lookAt(position, position + front, top);
}

void Camera::processMove(MoveDirection moveDirction, float timeDifferent)
{
    float shift = moveSpeed * timeDifferent;
    cout << "DEBUG::MAIN::C-CAMERA-F-PM-1: " << shift << endl;
    if (moveDirction == FORWARD)
        position += vec3(front.x, 0.0f, front.z) * shift;
    if (moveDirction == BACKWARD)
        position -= vec3(front.x, 0.0f, front.z) * shift;
    if (moveDirction == LEFT)
        position -= right * shift;
    if (moveDirction == RIGHT)
        position += right * shift;
    if (moveDirction == UP)
        position += vec3(0.0f, 1.0f, 0.0f) * shift;
    if (moveDirction == DOWN)
        position -= vec3(0.0f, 1.0f, 0.0f) * shift;
    cout << "DEBUG::MAIN::C-CAMERA-F-PM-2: " << position. x << " " << position.y << " " << position.z << endl;
}

void Camera::processTrackball(float thetaDifferent, float phiDifferent)
{
    theta -= thetaDifferent * trackballSpeed;
    float newPhi = phi + phiDifferent * trackballSpeed;
    if (newPhi < 85 && newPhi> -85)
        phi = newPhi;
    updateCameraPositionStatus();
};

void Camera::updateCameraPositionStatus()
{
    front.x = cos(radians(theta)) * cos(radians(phi));
    front.y = sin(radians(phi));
    front.z = sin(radians(theta)) * cos(radians(phi));
    front = normalize(front);

    right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
    top = normalize(cross(right, front));
}

void Camera::updateCameraRotationStatus()
{
    theta = degrees(atan2(front.z, front.x));
    phi = degrees(atan2(front.y, sqrt(pow(front.x, 2) + pow(front.z, 2))));

    right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
    top = normalize(cross(right, front));
}

