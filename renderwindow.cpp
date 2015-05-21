#include "renderwindow.h"

#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QScreen>
#include <QColorDialog>
#include <QMouseEvent>
#include <QToolTip>

#include <cstdlib>
#include <iostream>

using namespace std;

static const char *vertexShaderSource =
    "#version 330\n"
    "uniform mat4 mvp;\n"
    "in vec4 posAttr;\n"
    "in vec4 colAttr;\n"
    "out vec4 col;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = mvp * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 330\n"
    "in vec4 col;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = col;\n"
    "}\n";

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

GLfloat r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

RenderWindow::RenderWindow()
    : OpenGLWindow()
    , m_program(0)
    , m_frame(0)
    , m_useSourceCode(true)
{
    setAnimating(false);

    perspectiveEnabled = true;
    wireFrameEnabled = false;
}

void RenderWindow::checkError(const QString &prefix)
{
    // if OpenGL Logging is enabled or active then use that instead.

    if(isOpenGLLogging()) {
        return;
    }

    GLenum glErr = glGetError();

    while(glErr != GL_NO_ERROR) {
        QString error;
        switch (glErr)
        {
        case GL_NO_ERROR:               error="NO_ERROR";               break;
        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        if(error != "NO_ERROR") {
            qDebug() << prefix << ":" << error;
        }
        glErr = glGetError();
    }
} // end checkError()

void RenderWindow::initialize()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
} // end initialize()

void RenderWindow::linkShaderProgram()
{
    m_program = new QOpenGLShaderProgram(this);
    if(!m_useSourceCode) {
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../simple.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../simple.frag");
    } else {
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    }
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("mvp");

} // end linkShaderProgram()

void RenderWindow::enableAttributes(int vlen)
{
    m_program->enableAttributeArray(m_posAttr);
    m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3);
    m_program->enableAttributeArray(m_colAttr);
    m_program->setAttributeBuffer(m_colAttr, GL_FLOAT, vlen * sizeof(GLfloat), 3);

    checkError("after enabling attributes");

} // end enableAttributes()

void RenderWindow::render()
{

    linkShaderProgram();

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // allocate space for vertex array objects as necessary
    for (int i = 0; (i < objFileNames.size() && m_vao.size() != objFileNames.size()); i++) {
        m_vao.push_back(new QOpenGLVertexArrayObject(this));
        m_vao[i]->create();

        m_vbo.push_back(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
        m_vbo[i]->create();
        m_ibo.push_back(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));
        m_ibo[i]->create();

        checkError("state initialized");

        // check for errors to ensure OpenGL is functioning properly.
        checkError("after program bind");
    }

    m_program->bind();

    /* *********************************************************************************************** */
    /* *********************************************************************************************** */

    // read the file, store the vertices, etc.

    for (int i = 0; i < objFileNames.size(); i++) {
        m_vao[i]->bind();

        if (!objFileNames.empty()) {
            string fp = filePath;

            // convert from QString to string
            QByteArray byteArray = objFileNames[i].toUtf8();
            const char* ofn_string = byteArray.constData();

            tinyobj::LoadObj(shapes, materials, (fp.append(ofn_string)).c_str());
        }

        if (!shapes.empty()) {
            int vlen = 0;
            int ilen = 0;

            for (size_t i = 0; i < shapes.size(); i++) {
                vlen += shapes[i].mesh.positions.size();
                ilen += shapes[i].mesh.indices.size();
            }

            GLfloat vertices[vlen];
            GLuint indices[ilen];

            vector<QVector3D> verticesVector;
            indicesVector.push_back(ilen);

            for (size_t i = 0; i < shapes.size(); i++) {
                for (size_t j = 0; j < shapes[i].mesh.positions.size() / 3; j++) {
                    vertices[3*j+0] = shapes[i].mesh.positions[3*j+0];
                    vertices[3*j+1] = shapes[i].mesh.positions[3*j+1];
                    vertices[3*j+2] = shapes[i].mesh.positions[3*j+2];

                    //qDebug() << vertices[3*j+0] << vertices[3*j+1] << vertices[3*j+2];

                    // store vertices in verticesVector
                    QVector3D vertex(vertices[3*j+0], vertices[3*j+1], vertices[3*j+2]);
                    verticesVector.push_back(vertex);
                }

                for (size_t j = 0; j < shapes[i].mesh.indices.size() / 3; j++) {
                    indices[3*j+0] = shapes[i].mesh.indices[3*j+0];
                    indices[3*j+1] = shapes[i].mesh.indices[3*j+1];
                    indices[3*j+2] = shapes[i].mesh.indices[3*j+2];
                }
            }

            int clen = vlen;
            GLfloat colors[clen];
            fill_n(colors, clen, 1.0f);
            for (int i = 0; i < clen; i+=9) {
               colors[i] = 1.0f;
               if ((i+4) <= clen) {
                    colors[i+4] = 1.0f;
               }
               if ((i+4) <= clen) {
                    colors[i+8] = 1.0f;
                }
            }

            m_vbo[i]->bind();
            m_vbo[i]->allocate((vlen + clen)*sizeof(GLfloat));
            m_vbo[i]->write(0, vertices, vlen * sizeof(GLfloat));
            m_vbo[i]->write(vlen*sizeof(GLfloat), colors, clen * sizeof(GLfloat));

            checkError("after vertex buffer allocation");

            m_ibo[i]->bind();
            m_ibo[i]->allocate(indices, ilen*sizeof(GLuint));

            checkError("after index buffer allocation");

            enableAttributes(vlen);

            /* *********************************************************************************************** */
            /* *********************************************************************************************** */

            // set up transformation matrix
            QMatrix4x4 projection = objects[i].getProjection();
            if (perspectiveEnabled) {
                projection.perspective(60.0, (float)width()/(float)height(), 0.1, 1000);
            }
            else if (!perspectiveEnabled) {
                projection.ortho(-(float)width()/2, (float)width()/2, -(float)height()/2, (float)height()/2, 0.0, 1000.0);
            }

            QMatrix4x4 model = objects[i].getModel();
            QVector3D translate = objects[i].getTranslateXYZ();
            QVector3D rotate = objects[i].getRotateXYZ();
            QVector3D scale = objects[i].getScaleXYZ();
            model.translate(translate.x(), translate.y(), translate.z());
            model.rotate(rotate.x()/16.0, 1.0, 0.0, 0.0);
            model.rotate(rotate.y()/16.0, 0.0, 1.0, 0.0);
            model.rotate(rotate.z()/16.0, 0.0, 0.0, 1.0);
            model.scale(scale.x(), scale.y(), scale.z());

            QMatrix4x4 view = camera.returnView();

            m_program->setUniformValue(m_matrixUniform, projection * view * model);

            m_vao[i]->bind();
            glDrawElements(GL_TRIANGLES, indicesVector[i], GL_UNSIGNED_INT, 0);
            m_vao[i]->release();

        }
    }

    m_program->release();
    checkError("after program release");

    if(isAnimating()) {
        ++m_frame;
    }
} // end render()


void RenderWindow::getFilesInfo(QVector<QString> oNames, QVector<QString> fNames, string sceneFilePath)
{
    objectNames = oNames;
    objFileNames = fNames;
    filePath = sceneFilePath;

} // end getFilesInfo()

void RenderWindow::setObjectProperties(int size, QVector<QVector3D> translate, QVector<QVector3D> rotate, QVector<QVector3D> scale, QVector<QMatrix4x4> transformMats)
{
    objects.resize(size);
    for (int i = 0; i < size; i++) {
        objects[i].setNameAndIndex(objectNames[i], objFileNames[i], i);
        objects[i].setTransformationProperties(translate[i], rotate[i], scale[i]);
        objects[i].setModel(transformMats[i]);
    }
} // end setObjectProperties()


void RenderWindow::toggleWireFrame(bool on)
{
    if (on) {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    renderLater();
} // end toggleWireFrame()


/*********************************************************************************/
/********************************  MOUSE CONTROLS ********************************/
/*********************************************************************************/

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
} // end qNormalizeAngle()

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
} // end mousePressEvent()

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    int xAngle = dx*8;
    int yAngle = dy*8;

    if (event->buttons() & Qt::LeftButton) {
        camera.camTranslate(-dx, QVector3D(1, 0, 0));
        camera.camTranslate(dy, QVector3D(0, 1, 0));
    } else if (event->buttons() & Qt::RightButton) {
        qNormalizeAngle(xAngle);
        qNormalizeAngle(yAngle);

        camera.camRotate((float)xAngle/16.0, QVector3D(1, 0, 0));
        camera.camRotate((float)yAngle/16.0, QVector3D(0, 1, 0));
    }

    lastPos = event->pos();
    renderLater();
} // end mouseMoveEvent()

void RenderWindow::wheelEvent(QWheelEvent *event)
{
    int steps = 0;
    if (event != NULL) {
        steps++;
        QPoint z = event->angleDelta();
        if (z.y() == -120) {
            camera.camZoom((float)steps*-3.0);
        }
        else {
            camera.camZoom((float)steps*3.0);
        }
    }

    renderLater();
} // end wheelEvent()
