#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initShader()
{
    Q_ASSERT(_quadShader.initShader(":/asserts/shader/quad.vert", ":/asserts/shader/quad.frag"));
}

void Widget::initQuad()
{
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -0.7f,  0.7f,  0.0f, 1.0f,
        -0.7f, -0.7f,  0.0f, 0.0f,
        0.7f, -0.7f,  1.0f, 0.0f,

        -0.7f,  0.7f,  0.0f, 1.0f,
        0.7f, -0.7f,  1.0f, 0.0f,
        0.7f,  0.7f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &_quadVAO);
    glGenBuffers(1, &_quadVBO);
    glBindVertexArray(_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    _quadShader.bind();
    _quadShader.setUniformValue("screenTexture", 0);
}

void Widget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    initShader();
    initQuad();
#ifdef TEST
    initTest();
#endif
}

void Widget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Widget::paintGL()
{
    // glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    // clear all relevant buffers
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置为线框模式
    _quadShader.bind();

    glBindVertexArray(_quadVAO);
#ifndef TEST
    glBindTexture(GL_TEXTURE_2D, _frameBufferTextureColor);	// use the color attachment texture as the texture of the quad plane
#else
    glActiveTexture(GL_TEXTURE0);
    test_texture->bind();
#endif
    glDrawArrays(GL_TRIANGLES, 0, 6);
#ifdef TEST
    test_texture->release();
#endif
    glBindVertexArray(0);
    _quadShader.release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Widget::initTest()
{
    initTestTexture();
}
void Widget::initTestTexture()
{
    test_texture = new QOpenGLTexture(QImage(":/asserts/image/img1.png").mirrored());
}
