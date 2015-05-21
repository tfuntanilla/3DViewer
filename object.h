#ifndef OBJECT_H
#define OBJECT_H

#include <QVector3D>
#include <QMatrix4x4>

class Object
{
public:
    Object();
    ~Object();

    void setNameAndIndex(QString n, QString f, int i);
    void setTransformationProperties(QVector3D t, QVector3D r, QVector3D s);
    void setModel(QMatrix4x4 matrix);
    void setProjection(QMatrix4x4 matrix);
    void setView(QMatrix4x4 matrix);
    QMatrix4x4 getModel();
    QMatrix4x4 getProjection();
    QMatrix4x4 getView();
    QVector3D getTranslateXYZ();
    QVector3D getRotateXYZ();
    QVector3D getScaleXYZ();

private:

    int index;
    QString objectName;
    QString fileOfOrigin;

    QVector3D translateXYZ;
    QVector3D rotateXYZ;
    QVector3D scaleXYZ;

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;

};

#endif // OBJECT_H
