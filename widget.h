#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QImage>

#include "shader.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

#define TEST
class Widget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initShader();
    void initQuad();

    void updateFrameBuffer();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    Ui::Widget *ui;

//quad
    Shader _quadShader;
    GLuint _quadVAO, _quadVBO;
    GLuint _frameBuffer;
    GLuint _frameBufferTextureColor;
    GLuint _frameBufferTextureDepth;



private:
    //test
    void initTest();
    void initTestTexture();
    QOpenGLTexture* test_texture;
};
#endif // WIDGET_H
