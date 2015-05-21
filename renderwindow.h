#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "openglwindow.h"
#include "tiny_obj_loader.h"
#include "object.h"
#include "camera.h"

#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>

using namespace std;

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class RenderWindow : public OpenGLWindow
{
public:
    RenderWindow();

    void initialize();
    void render();
    void checkError(const QString& prefix);

    void linkShaderProgram();
    void enableAttributes(int vlen);

    void getFilesInfo(QVector<QString> oNames, QVector<QString> fNames, string sceneFilePath);
    void setObjectProperties(int size, QVector<QVector3D> translate, QVector<QVector3D> rotate, QVector<QVector3D> scale, QVector<QMatrix4x4> transformMats);
    void toggleWireFrame(bool on);

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    QVector<Object> objects;

    bool perspectiveEnabled; // toggle between perspective and orthographic projections
    bool wireFrameEnabled; // toggle between wire frame and solid face rendering

private:

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;

    QVector<QOpenGLVertexArrayObject*> m_vao;
    QVector<QOpenGLBuffer*> m_vbo;
    QVector<QOpenGLBuffer*> m_ibo;

    int m_frame;
    bool m_useSourceCode;

    QVector<QString> objectNames;
    QVector<QString> objFileNames;
    string filePath;
    vector<GLuint> indicesVector;

    // used for storing OBJ data parsed using tinyobj parser
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    Camera camera;
    QPoint lastPos;

};

class RenderWindowWidget : public QWidget
{
    Q_OBJECT
public:
  RenderWindowWidget(QWidget* parent): QWidget(parent) {

      renWin = new RenderWindow();

      QWidget* widget = QWidget::createWindowContainer(renWin);
      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->addWidget(widget);

  }

  RenderWindow* GetRenderWindow() {
      return renWin;
  }

private:
  RenderWindow* renWin;

};

#endif // RENDERWINDOW_H
