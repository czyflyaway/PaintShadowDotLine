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
    Q_ASSERT(_cubeShader.initShader(":/asserts/shader/cube.vert", ":/asserts/shader/cube.frag"));
    Q_ASSERT(_cubeDepthMapShader.initShader(":/asserts/shader/cubedepth.vert", ":/asserts/shader/cubedepth.frag"));
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _quadShader.bind();
    _quadShader.setUniformValue("screenTexture", 0);
}

void Widget::initCube()
{
    float pos = 0.9f;
    float cubeVertices[] ={
        -pos, pos, -pos,
        pos, pos, -pos,
        pos, -pos, -pos,
        -pos, -pos, -pos,
        -pos, pos, pos,
        -pos, -pos, pos,
        pos, -pos, pos,
        pos, pos, pos
    };
    GLuint edgeIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // 后面四条边
        4, 5, 5, 6, 6, 7, 7, 4, // 前面四条边
        0, 4, 3, 5, 2, 6, 1, 7  // 连接前后面的边
    };

    glGenVertexArrays(1, &_cubeVAO);
    glGenBuffers(1, &_cubeVBO);
    glGenBuffers(1, &_cubeEBO);

    glBindVertexArray(_cubeVAO);
    //vbo
    glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    //ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edgeIndices), edgeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

}

void Widget::initFrameBuffer()
{
    glGenFramebuffers(1, &_frameBuffer);
}

void Widget::initFrameBufferDepth()
{
    glGenFramebuffers(1, &_frameBufferDepth);
}

void Widget::updateFrameBuffer(int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glGenTextures(1, &_frameBufferTextureColor);
    glBindTexture(GL_TEXTURE_2D, _frameBufferTextureColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frameBufferTextureColor, 0);

    glGenRenderbuffers(1, &_frameBufferRenderBufferDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _frameBufferRenderBufferDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _frameBufferRenderBufferDepthStencil);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Error: framebuffer is not completed";
    else
        qDebug() << "Success: framebuffer";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::udpateFrameBufferDepth(int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);

    glGenTextures(GL_TEXTURE_2D, &_frameBufferDepthTextureDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _frameBufferDepthTextureDepth, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    initShader();
    initQuad();
    initCube();
    initFrameBuffer();
#ifdef TEST
    initTest();
#endif
}

void Widget::resizeGL(int w, int h)
{
    qDebug() << "resize GL";
    glViewport(0, 0, w, h);
    updateMVP(float(w) / float(h));
    updateFrameBuffer(w, h);
}

void Widget::paintGL()
{
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // drawCube();
    // drawCubeFramebuffer();
    drawCubeDepthFrameBuffer();
    drawDepthMap();
    // drawQuad();
}

void Widget::drawQuad()
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
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

void Widget::drawCube()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBindVertexArray(_cubeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}

void Widget::drawCubeFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _cubeShader.bind();
    _cubeShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCube();
    _cubeShader.release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::drawCubeDepthFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    _cubeDepthMapShader.bind();
    _cubeDepthMapShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCube();
    _cubeDepthMapShader.release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::drawDepthMap()
{
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, .0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    _quadShader.bind();

    glBindVertexArray(_quadVAO);
    glBindTexture(GL_TEXTURE_2D, _frameBufferDepthTextureDepth);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    _quadShader.release();
}

void Widget::updateMVP(float aspect)
{
    _cubeMVP.setToIdentity();
    QMatrix4x4 model;
    model.rotate(30, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 camera;
    camera.lookAt(QVector3D(0, 0, 5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    QMatrix4x4 projecttion;
    projecttion.perspective(45, aspect, 0.1f, 100.f);
    _cubeMVP = projecttion * camera * model;
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
