#include "shader.h"
#include <QDebug>

Shader::Shader(QObject *parent)
    : QOpenGLShaderProgram(parent){}

Shader::~Shader(){}

bool Shader::initShader(const QString &vertex, const QString frag)
{
    if(!addShaderFromSourceFile(QOpenGLShader::Vertex, vertex)){
        qDebug() << "Add vert shader failed.";
        return false;
    }
    if(!addShaderFromSourceFile(QOpenGLShader::Fragment, frag)){
        qDebug() << "Add frag shader failed.";
        return false;
    }
    if(!link()){
        qDebug() << "Link failed.";
        return false;
    }
    if(!bind()){
        qDebug() << "bind shader failed.";
        return false;
    }
    return true;
}

