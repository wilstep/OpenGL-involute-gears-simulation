// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_0>
#include <QMouseEvent>
#include <QQuaternion>
#include <QVector3D>


class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    void incRotate();
    void setPaused(bool x) { paused = x; }
    void setPa(float x) { pa = x; }
    void setNa(GLuint x) { Na = x; }
    void setNb(GLuint x) { Nb = x; }
    void rebuild() { rebuild_flg = true; }
    void setSpeed(float x){ speed = x * 12.0f / (float) Na; }
    void setLightX(float x){ lightX = x; update(); }
    void setLightY(float y){ lightY = y; update(); }
    void setLightZ(float z){ lightZ = z; update(); }
    void setSeperation(const float del);
    void reset() { delX = delY = 0.0f; delZ = delZ0; QuatOrient = QQuaternion(); update(); }
    void reZeroThetas() { theta_a = theta_b = 0.0; }
protected:
    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void buildGears(bool redo=false);
    float NewtonRaphson(const unsigned int n, const float rp, const float fac);

    int rotate;
    GLuint shaderProgram;
    GLuint vao, vbo, ebo;
    GLint uniMat, uniRot, uniColor, uniPerspective, uniLightPos;
    QPoint lastPos;
    bool paused = false;
    QQuaternion QuatOrient; // initialised to unit quaternion, stores the global orientation
    int xCentre, yCentre;
    const float delZ0 = -125.0f;
    float delX =0.0f, delY = 0.0f, delZ = delZ0;
    float delSeperation = 0.0f, delTheta_a = 0.0f;
    float pa;
    GLuint Na, Nb, Nind_a, Nind1_a, Nind_b, Nind1_b;
    const double delTheta = 0.1;
    double theta_a = 0.0, theta_b = 0.0;
    bool rebuild_flg = false;
    float speed;
    float lightX, lightY, lightZ;
};

#endif // OGLWIDGET_H
