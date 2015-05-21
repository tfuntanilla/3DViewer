#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderwindow.h"
#include "scenehandler.h"

#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* HELPER FUNCTIONS */

static const char* convertQStringtoString(QString string)
{

    QByteArray byteArray = string.toUtf8();
    const char* cString = byteArray.constData();

    return cString;
}

void MainWindow::handleScene(QString file, string sceneFilePath)
{
    SceneHandler scene;
    QVector<Scene*> scenes;
    QVector<Model*> models;

    scene.readScene(file, scenes);
    for (int i = 0; i < scenes[0]->root.size(); i++) {
        if (scenes[0]->root[i]->type == "model") {
            models.push_back(static_cast<Model*>(scenes[0]->root[i]->children[0]));
        }
    }

    QVector<QString> names;
    QVector<QString> objFileNames;
    QVector<QMatrix4x4> transformMats;
    QVector<QVector3D> t, r, s;

    // store contents of JSON file in appropriate vectors
    for (int i = 0; i < models.size(); i++) {
        names.push_back(models[i]->name);
        objFileNames.push_back(models[i]->fileName);
        transformMats.push_back(models[i]->transformMat);
        t.push_back(models[i]->translate);
        r.push_back(models[i]->rotate);
        s.push_back(models[i]->scale);
        // insert each object into the object list widget
        ui->objects_listWidget->insertItem(i, models[i]->name);
    }

    ui->objects_listWidget->setCurrentRow(0);

    // prepare the objects for rendering
    ui->renderwindowwidget->GetRenderWindow()->getFilesInfo(names, objFileNames, sceneFilePath);
    ui->renderwindowwidget->GetRenderWindow()->setObjectProperties(models.size(), t, r, s, transformMats);

    // render

} // end handleScene()

/* MENU BAR FUNCTIONS */

void MainWindow::on_actionOpen_triggered()
{
    // open file dialog
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), "../", tr("JSON (*.json *.JSON);;OBJ (*.obj *.OBJ)"));

    // check for valid file
    if (file.isEmpty()) {
        QMessageBox::information(this, tr("Alert"), tr("No file selected."));
        return;
    }

    // check file extension
    QFileInfo info(file);
    QString filePath = info.absoluteFilePath();
    QString base = info.baseName();
    QString suffix = info.suffix();
    base.append(".");
    base.append(suffix);
    filePath.remove(base);
    string absoluteFilePath = convertQStringtoString(filePath);

    if (info.suffix() == "json" || info.suffix() == "JSON") {
        handleScene(file, absoluteFilePath);
    }
    else if (info.suffix() == "obj" || info.suffix() == "OBJ") {

    }
}

void MainWindow::on_actionSave_triggered()
{

}

