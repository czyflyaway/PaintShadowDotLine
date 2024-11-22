#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QImage>
#include <QMatrix4x4>
#include <QQuaternion>
#include "shader.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// #define TEST
class Widget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initShader();
    void initQuad();
    void initCube();
    void initFrameBuffer();
    void initFrameBufferDepth();
    void updateFrameBuffer(int w, int h);
    void udpateFrameBufferDepth(int w, int h);
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    void drawQuad();
    void drawCube();
    void drawCubeFramebuffer();
    void drawCubeDepthFrameBuffer();
    void drawDepthMap();
private:
    void updateMVP(float aspect);
private:
    Ui::Widget *ui;

//quad
    Shader _quadShader;
    GLuint _quadVAO, _quadVBO;
//cube
    Shader _cubeShader;
    GLuint _cubeVAO, _cubeVBO, _cubeEBO;
    QMatrix4x4 _model;
    QMatrix4x4 _camera;
    QMatrix4x4 _projection;
    QMatrix4x4 _cubeMVP;
    QQuaternion _rotation;
    Shader _cubeDepthMapShader;
//framebuffer
    GLuint _frameBuffer;
    GLuint _frameBufferTextureColor;
    GLuint _frameBufferRenderBufferDepthStencil;
//framebuffer depth
    GLuint _frameBufferDepth;
    GLuint _frameBufferDepthTextureDepth;
private:
    //test
    void initTest();
    void initTestTexture();
    QOpenGLTexture* test_texture;
};
#endif // WIDGET_H
