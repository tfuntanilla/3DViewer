/* ****************************************************
 * SceneHandler class for processing JSON files
 *****************************************************/

#ifndef SCENEHANDLER_H
#define SCENEHANDLER_H

#include <QJsonDocument>
#include <QMatrix4x4>

struct Node
{
    QString type;
    QVector<Node*> children;
};

struct Model : public Node
{
    QString name;
    QString fileName;
    QMatrix4x4 transformMat;

    QVector3D translate;
    QVector3D rotate;
    QVector3D scale;
};

struct Scene
{
    QString name;
    QString description;
    QVector<Node*> root;
};

class SceneHandler {

public:
    SceneHandler();
    ~SceneHandler();

    Model* parseModel(const QJsonObject& model);
    Node* parseNode(const QJsonObject& parentNode) ;
    void readScene(const QString& filename, QVector<Scene *> &scenes);
    QJsonObject writeNode(Node* node);
    void writeScene(const QString& filename, QVector<Scene *> &scenes);

};

#endif
