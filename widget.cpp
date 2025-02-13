#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->spinBox_x, SIGNAL(valueChanged(int)), this, SLOT(slotSpinChanged(int)));
    connect(ui->spinBox_y, SIGNAL(valueChanged(int)), this, SLOT(slotSpinChanged(int)));
    connect(ui->spinBox_z, SIGNAL(valueChanged(int)), this, SLOT(slotSpinChanged(int)));
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
    Q_ASSERT(_quadDepthShader.initShader(":/asserts/shader/quaddepth.vert", ":/asserts/shader/quaddepth.frag"));
    Q_ASSERT(_cubeDotShader.initShader(":/asserts/shader/cubedot.vert", ":/asserts/shader/cubedot.frag"));
}

void Widget::initQuad()
{
    float pos = 1.0f;
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -pos,  pos,  0.0f, 1.0f,
        -pos, -pos,  0.0f, 0.0f,
        pos, -pos,  1.0f, 0.0f,

        -pos,  pos,  0.0f, 1.0f,
        pos, -pos,  1.0f, 0.0f,
        pos,  pos,  1.0f, 1.0f
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
    float pos = 1.0f;
    float cubeVertices[24] = {
        pos, pos, -pos,
        pos, -pos, -pos,
        pos, -pos, pos,
        pos, pos, pos,
        -pos, pos, pos,
        -pos, -pos, pos,
        -pos, -pos, -pos,
        -pos, pos, -pos
    };
    edgeIndices = {
        0,1,1,2,2,3,
        3,0,4,3,2,5,
        5,4,1,6,6,5,
        6,7,7,4,7,0
    };

    faceIndices = {
        0,1,2,3,
        4,3,2,5,
        5,2,1,6,
        6,7,4,5,
        7,0,3,4,
        6,1,0,7
    };
    glGenVertexArrays(1, &_cubeVAO);
    glGenBuffers(1, &_cubeVBO);
    glGenBuffers(1, &_cubeEBO);
    glGenBuffers(1, &_cubeFaceEBO);

    glBindVertexArray(_cubeVAO);
    //vbo
    glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // //ebo
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * edgeIndices.size(), &edgeIndices.front(), GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeFaceEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faceIndices.size(), &faceIndices.front(), GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

}

void Widget::initFrameBuffer()
{
    glGenFramebuffers(1, &_frameBuffer);
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

void Widget::initFrameBufferDepth()
{
    glGenFramebuffers(1, &_frameBufferDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);

    glGenTextures(1, &_frameBufferDepthTextureDepth);
    glBindTexture(GL_TEXTURE_2D, _frameBufferDepthTextureDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _frameBufferDepthTextureDepth, 0);

    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Error: _frameBufferDepth is not completed";
    else
        qDebug() << "Success: _frameBufferDepth";
    glBindTexture(GL_TEXTURE_2D, 0);
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
    initFrameBufferDepth();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef TEST
    initTest();
#endif
}

void Widget::resizeGL(int w, int h)
{
    _srcWidth = w;
    _srcHeight = h;
    qDebug() << "resize GL";
    glViewport(0, 0, _srcWidth, _srcHeight);
    updateMVP(float(w) / float(h));
    updateFrameBuffer(w, h);
}

void Widget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //just draw quad
    // drawQuad();
    // just draw cube
    //    drawCubeFaces();
    //test render framebuffer
    // drawCubeFramebuffer();
    // drawQuad();
    //     //test depthmap
    //     drawCubeDepthFrameBuffer();
    //     drawDepthMap();
    //    //test dot cube
    ////    glLineWidth(3.0f);
    //    drawCubeDepthFrameBuffer();
    //    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    drawCubeDot();
    //1.
    //    drawCubeFaces();
    //2.
    //    glViewport(0, 0, _shadowWidth, _shadowHeight);
    //    drawCubeFacesDepthFrameBuffer();
    //    glViewport(0, 0, _srcWidth, _srcHeight);
    //    drawDepthMap();
    //3.
    glLineWidth(1.0f);
    generateDepthMap();
    glViewport(0, 0, _srcWidth, _srcHeight);
    drawCubeDot();
    glLineWidth(1.0f);
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
#if LINESMOOTH
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif
    glBindVertexArray(_cubeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

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
    _cubeDepthMapShader.bind();
    _cubeDepthMapShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCube();
    _cubeDepthMapShader.release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::drawDepthMap()
{
    glClearColor(1.0f, .0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    _quadDepthShader.bind();
    _quadDepthShader.setUniformValue("depthMap", 0);
    _quadDepthShader.setUniformValue("near_plane", _nearPlane);
    _quadDepthShader.setUniformValue("far_plane", _farPlane);
    glBindVertexArray(_quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _frameBufferDepthTextureDepth);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    _quadDepthShader.release();
}

void Widget::drawCubeDotDepthFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    _cubeDepthMapShader.bind();
    _cubeDepthMapShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCubeDotDepthFrameBuffer();
    _cubeDepthMapShader.release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::drawCubeDot()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _srcWidth, _srcHeight);

    _cubeDotShader.bind();
    _cubeDotShader.setUniformValue("depthbuf", 0);
    _cubeDotShader.setUniformValue("uMvpMatrix", _cubeMVP);

#if LINESMOOTH
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _frameBufferDepthTextureDepth);	// use the color attachment texture as the texture of the quad plane

    glBindVertexArray(_cubeVAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_LINES, (GLsizei)edgeIndices.size(), GL_UNSIGNED_INT, &edgeIndices.front());
    glBindVertexArray(0);
    _cubeDotShader.release();
}

void Widget::drawCubeFacesDepthFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    _cubeDepthMapShader.bind();
    _cubeDepthMapShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCubeFacesHelper();
    _cubeDepthMapShader.release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Widget::drawCubeFaces()
{
    _cubeShader.bind();
    _cubeShader.setUniformValue("uMvpMatrix", _cubeMVP);
    drawCubeFacesHelper();
    _cubeShader.release();
}

void Widget::drawCubeFacesHelper()
{
    glBindVertexArray(_cubeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeFaceEBO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Widget::drawCubeBorder()
{
#if LINESMOOTH
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif
    glBindVertexArray(_cubeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Widget::updateMVP(float aspect)
{
    _cubeMVP.setToIdentity();
    QMatrix4x4 model;
    model.rotate(_xDegree, 1.0f, 0.0f, 0.0f);
    model.rotate(_yDegree, 0.0f, 1.0f, 0.0f);
    model.rotate(_zDegree, 0.0f, 0.0f, 1.0f);
    QMatrix4x4 camera;
    camera.lookAt(QVector3D(0, 0, 5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    QMatrix4x4 projecttion;
    projecttion.perspective(45, aspect, _nearPlane, _farPlane);
    //    float edge = 3.0f;
    //    projecttion.ortho(-edge, edge, -edge, edge, _nearPlane, _farPlane);
    _cubeMVP = projecttion * camera * model;

    qDebug() << _cubeMVP;
    float pos = 1.0f;
    QVector4D cubeVertices[] ={
        QVector4D(-pos, pos, -pos, 1.0f),
        QVector4D(pos, pos, -pos, 1.0f),
        QVector4D(pos, -pos, -pos, 1.0f),
        QVector4D(-pos, -pos, -pos, 1.0f),
        QVector4D(-pos, pos, pos, 1.0f),
        QVector4D(-pos, -pos, pos, 1.0f),
        QVector4D(pos, -pos, pos, 1.0f),
        QVector4D(pos, pos, pos, 1.0f)
    };
    for(int i = 0; i < 8; ++i)
    {
        //        qDebug() << (_cubeMVP * cubeVertices[i]) * 0.5 + QVector3D(0.5, 0.5, 0.5);
        //        qDebug() << _cubeMVP * cubeVertices[i].toVector3D();
        QVector4D ret =_cubeMVP * cubeVertices[i];
        //        qDebug() << ret;
        qDebug() << "---" << ret *  (1.0f / ret.w());
    }
}

void Widget::bindDepthMap()
{

}

void Widget::generateDepthMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDepth);
    glViewport(0, 0, _shadowWidth, _shadowHeight);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _cubeDepthMapShader.bind();
    _cubeDepthMapShader.setUniformValue("uMvpMatrix", _cubeMVP);

    glBindVertexArray(_cubeVAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeFaceEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_QUADS, (GLsizei)faceIndices.size(), GL_UNSIGNED_INT, &faceIndices.front());
    glBindVertexArray(0);
    _cubeDepthMapShader.release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Widget::on_pushButton_clicked()
{

}

void Widget::slotSpinChanged(int value)
{
    _xDegree =  ui->spinBox_x->value();
    _yDegree =  ui->spinBox_y->value();
    _zDegree =  ui->spinBox_z->value();
    updateMVP(float(size().width()) / float(size().height()));
    update();
}


