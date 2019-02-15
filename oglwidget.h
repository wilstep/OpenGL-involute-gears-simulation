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
protected:
    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void buildGears(bool redo=false);

    int rotate;
    GLuint shaderProgram;
    GLuint vao, vbo, ebo;
    GLint uni, uniColor;
    QPoint lastPos;
    bool paused = false;
    QQuaternion QuatOrient; // initialise to unit quaternion
    int xCentre, yCentre;
    float delX =0.0f, delY = 0.0f, delZ = -125.0f;
    float pa;
    GLuint Na, Nb, Nind_a, Nind1_a, Nind_b, Nind1_b;
    const double delTheta = 0.1;
    double theta_a = 0.0, theta_b = 0.0;
    bool rebuild_flg = false;
};

#endif // OGLWIDGET_H
