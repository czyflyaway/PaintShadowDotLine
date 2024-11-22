#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>

class Shader : public QOpenGLShaderProgram
{
    Q_OBJECT
public:
    Shader(QObject* parent = nullptr);
    ~Shader();

    bool initShader(const QString& vertex, const QString frag);
};

#endif // SHADERPROGRAM_H
