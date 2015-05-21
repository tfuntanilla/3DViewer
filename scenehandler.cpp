#include "scenehandler.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

SceneHandler::SceneHandler()
{

}


SceneHandler::~SceneHandler()
{

}

Model* SceneHandler::parseModel(const QJsonObject& model)
{
    Model* out = new Model();

    out->name = model["name"].toString();
    out->fileName = model["filename"].toString();
    QJsonArray jmatrix = model["matrix"].toArray();
    QJsonArray tvector = model["translation"].toArray();
    QJsonArray rvector = model["rotation"].toArray();
    QJsonArray svector = model["scale"].toArray();

    if (jmatrix.size() != 16) {
        qDebug() << "name: " << out->name
                 << " file: " << out->fileName
                 << " matrix length: " << jmatrix.size()
                 << " not 16";
        out->transformMat.setToIdentity();
    } else {
        float values[16];
        for (int i = 0; i < jmatrix.size(); ++i) {
            values[i] = (float)jmatrix[i].toDouble();
        }
        out->transformMat = QMatrix4x4(values);
    } // end if-else jmatrix

    if (tvector.size() != 3) {
        out->translate = QVector3D(0.0, 0.0, 0.0);
    } else {
        float values[3];
        for (int i = 0; i < tvector.size(); ++i) {
            values[i] = (float)tvector[i].toDouble();
        }
        out->translate = QVector3D(values[0], values[1], values[2]);
    } // end if-else t-vector

    if (rvector.size() != 3) {
        out->rotate = QVector3D(0.0, 0.0, 0.0);
    } else {
        float values[3];
        for (int i = 0; i < rvector.size(); ++i) {
            values[i] = (float)rvector[i].toDouble();
        }
        out->rotate = QVector3D(values[0], values[1], values[2]);
    } // end if-else rvector

    if (svector.size() != 3) {
        out->scale = QVector3D(30.0, 45.0, 45.0);
    } else {
        float values[3];
        for (int i = 0; i < svector.size(); ++i) {
            values[i] = (float)svector[i].toDouble();
        }
        out->scale = QVector3D(values[0], values[1], values[2]);
    } // end if-else svector

    qDebug() << "load :" << out->name
             << " file: " << out->fileName
             << " transformed " << out->transformMat
             << " translate" << out->translate
             << " rotate" << out->rotate
             << " scale" << out->scale;

    return out;

} // end parseModel()

Node* SceneHandler::parseNode(const QJsonObject& parentNode) {

    Node* out = new Node();
    out->type = parentNode["type"].toString();

    // detected a model
    if (out->type == "model") {
        out->children.push_back(parseModel(parentNode));
    }
    else if(out->type == "node") {
        QJsonArray nodeArray = parentNode["children"].toArray();
        for (int i = 0; i < nodeArray.size(); ++i) {
            out->children.push_back(parseNode(nodeArray[i].toObject()));
        }
    }

    return out;
} // end parseNode()


void SceneHandler::readScene(const QString &filename, QVector<Scene*>& scenes) {

    QString fileContents;
    QFile file(filename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    fileContents = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileContents.toUtf8());
    QJsonArray sceneList = doc.array();

    for (int i = 0; i < sceneList.size(); ++i) {
        Scene* scene = new Scene();
        QJsonObject object = sceneList[i].toObject();
        scene->name = object["name"].toString();
        scene->description = object["description"].toString();
        QJsonArray rootNode = object["root"].toArray();
        qDebug() << "scene: " << i << " " << scene->name << " " << scene->description;

        for (int i = 0; i < rootNode.size(); ++i) {
            scene->root.push_back(parseNode(rootNode[i].toObject()));
        }
        scenes.push_back(scene);
    }
} // end readScene()

QJsonObject SceneHandler::writeNode(Node* node) {

    QJsonObject out;
    out["type"] = node->type;

    if (node->type == "model") {
        Model* m = static_cast<Model*>(node->children[0]);
        out["name"] = m->name;
        out["filename"] = m->fileName;
        qDebug() << out["name"] << " " << out["filename"];

        QJsonArray data;
        for (int i = 0; i < 16; ++i) {
            data.push_back((float)m->transformMat.data()[i]);
        }

        out["matrix"] = data;

        QJsonArray t, r, s;
        t.push_back((float)m->translate.x());
        t.push_back((float)m->translate.y());
        t.push_back((float)m->translate.z());
        r.push_back((float)m->rotate.x());
        r.push_back((float)m->rotate.y());
        r.push_back((float)m->rotate.z());
        s.push_back((float)m->scale.x());
        s.push_back((float)m->scale.y());
        s.push_back((float)m->scale.z());

        out["translation"] = t;
        out["rotation"] = r;
        out["scale"] = s;
    }
    else if (node->type == "node") {
        QJsonArray children;
        for (int i = 0; i < node->children.size(); ++i) {
            children.push_back(writeNode(node->children[i]));
        }
        out["children"] = children;
    }

    return out;
} // end writeNode()

void SceneHandler::writeScene(const QString &filename, QVector<Scene *> &scenes) {

    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QJsonDocument doc;
    QJsonArray sceneList;
    for (int i = 0; i < scenes.size(); ++i) {
        QJsonObject scene;
        scene["name"] = scenes[i]->name;
        scene["description"] = scenes[i]->description;
        qDebug() << "writing : " << scene["name"] << " " << scene["description"];
        QJsonArray rootNode;
        for (int j = 0; j < scenes[i]->root.size(); ++j) {
            QJsonValue value = writeNode(scenes[i]->root[j]);
            rootNode.push_back(value);
        }
        scene["root"] = rootNode;
        sceneList.push_back(scene);
    }

    doc.setArray(sceneList);
    file.write(doc.toJson());
    file.close();

} // end writeScene
