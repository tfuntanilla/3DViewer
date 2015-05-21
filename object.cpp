#include "object.h"

Object::Object()
{
    objectName = "";
    fileOfOrigin = "";
    index = 0;

    translateXYZ = QVector3D(0.0, 0.0, 0.0);
    rotateXYZ = QVector3D(0.0, 0.0, 0.0);
    scaleXYZ = QVector3D(0.0, 0.0, 0.0);

}

Object::~Object()
{

}

void Object::setNameAndIndex(QString n, QString f, int i)
{
    objectName = n;
    fileOfOrigin = f;
    index = i;
} // end setNameandIndex()

void Object::setTransformationProperties(QVector3D t, QVector3D r, QVector3D s)
{
    translateXYZ = t;
    rotateXYZ = r;
    scaleXYZ = s;
} // end setTransformationProperties

void Object::setModel(QMatrix4x4 matrix)
{
    model *= matrix;
} // end setModel()

void Object::setView(QMatrix4x4 matrix)
{
    view = matrix;
} // end setView()

void Object::setProjection(QMatrix4x4 matrix)
{
    projection = matrix;
} // end setProjection()

QMatrix4x4 Object::getModel()
{
    return model;
} // end getModel()

QMatrix4x4 Object::getProjection()
{
    return projection;
} // end getProjection()

QMatrix4x4 Object::getView()
{
    return view;
} // end getView()

QVector3D Object::getTranslateXYZ()
{
    return translateXYZ;
} // end getTranslateXYZ()

QVector3D Object::getRotateXYZ()
{
    return rotateXYZ;
} // end getRotateXYZ()

QVector3D Object::getScaleXYZ()
{
    return scaleXYZ;
} // end getScaleXYZ()
