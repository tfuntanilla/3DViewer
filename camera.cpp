#include "camera.h"

Camera::Camera()
{
    eye = QVector3D(1.0, 0.0, 1.0);
    center = QVector3D(0.0, 0.0, 0.0);
    up = QVector3D(0.0, 1.0, 0.0);
    rotationMatrix.setToIdentity();    
}

Camera::~Camera()
{

}

void Camera::camTranslate(float dist, QVector3D trans)
{
    trans.normalize();
    trans = dist*trans;
    center += trans;
    eye += trans;
    
}

void Camera::camRotate(float amount, QVector3D rot)
{
    rotationMatrix.rotate(amount, rot);
}

void Camera::camZoom(float amount)
{
    camTranslate(amount, QVector3D(1.0, 0.0, 0.0));
    camTranslate(amount, QVector3D(0.0, 1.0, 0.0));
    camTranslate(amount, QVector3D(0.0, 0.0, 1.0));
}

QMatrix4x4 Camera::returnView()
{

    QMatrix4x4 camView;
    camView.lookAt(eye, center, up);
    return camView * rotationMatrix;
    
}
