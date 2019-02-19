// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#include "oglwidget.h"
#include "gear.h"

#include <iostream>
#include <QTextStream>

#include <QMatrix4x4>
#include <qmath.h>
//#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include<QApplication>
//#include<cstdlib>

#include "myshaders.h"

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OGLWidget::~OGLWidget()
{
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void OGLWidget::initializeGL()
{
    float OGL_ver;
    bool newVer = false;

    initializeOpenGLFunctions();
    //OGL_ver = (float) atof((const char*) glGetString(GL_VERSION));
    OGL_ver = std::stof(std::string((const char*) glGetString(GL_VERSION)));
    std::cout << "OGL_ver = " << OGL_ver << std::endl;
    if(OGL_ver >= 3.3f) newVer = true;
    // Vertex Shader
    {
        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if(newVer) glShaderSource(vertexShader, 1, &vertexShaderSourceNew, NULL);
        else glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        int success;
        char infoLog[1024];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
            std::string str = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog);
            throw std::runtime_error(str);
        }

        if(!newVer){
            glBindAttribLocation(vertexShader, 0, "aPos");
            glBindAttribLocation(vertexShader, 1, "aNormal");
            //std::cout << "glGetError = " << glGetError() << std::endl;
        }

        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if(newVer) glShaderSource(fragmentShader, 1, &fragmentShaderSourceNew, NULL);
        else glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
            std::string str = "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(infoLog);
            throw std::runtime_error(str);
        }
        // Link the vertex and fragment shader into a shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glUseProgram(shaderProgram);
    }
    buildGears();
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    uni = glGetUniformLocation(shaderProgram, "matrix");
    uniRot = glGetUniformLocation(shaderProgram, "rot");
    uniPerspective = glGetUniformLocation(shaderProgram, "perspective");
    uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
    uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    std::cout << "\nOpenGL core profile version string: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    xCentre = frameSize().width() / 2;
    yCentre = frameSize().height() / 2;
}

void  OGLWidget::buildGears(bool redo)
{
    // initialise gear objects, which provide vertices and indices
    auto myGa = std::make_unique<gear>(Na, pa, 4.001f); // gear slightly thicker so it shows above any overlap
    auto myGb = std::make_unique<gear>(Nb, pa, 4.0f);

    Nind_a = myGa -> GetNInds();
    Nind1_a = myGa -> GetN1Inds();
    Nind_b = myGb -> GetNInds();
    Nind1_b = myGb -> GetN1Inds();
    myGa -> RotateVerts(-90.0f);
    myGb -> RotateVerts(90.0f);

    // vertex data
    std::vector<GLfloat> vertices, vbuff;
    vertices = myGa -> GetVerts();
    vbuff = myGb -> GetVerts();
    vertices.insert(vertices.end(), vbuff.begin(), vbuff.end());
    // index data
    std::vector<GLuint> indices, ibuff;
    indices = myGa -> GetInds();
    ibuff = myGb -> GetInds();
    GLuint nverts = myGa -> GetNverts();
    for(auto &a: ibuff) a += nverts;
    indices.insert(indices.end(), ibuff.begin(), ibuff.end());


    if(redo){
        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // element buffer object
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    }
    else{
        // Create Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create a Vertex Buffer Object and copy the vertex data to it
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        // Create an element buffer object and copy index data to it
        glGenBuffers(1, &ebo); // and an element buffer object
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // element buffer object
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Specify the layout of the vertex data
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}


// y axis on screen is upside down, so map y to -y for mouse movements
void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x1 = event->x();
    int y1 = -event->y();
    int x0 = lastPos.x();
    int y0 = -lastPos.y();

    // Is shift button being held down
    if(Qt::ShiftModifier == QApplication::keyboardModifiers()){ // translate in x,y plane
        const float xmin = 10.0 * delZ;
        const float xmax = -xmin;
        const float ymin = xmin * (float) yCentre / (float) xCentre;
        const float ymax = -ymin;
        if(delX < xmin) delX = xmin;
        if(delX > xmax) delX = xmax;
        if(delY < ymin) delY = ymin;
        if(delY > ymax) delY = ymax;
        if(event->buttons() & Qt::LeftButton){
            QVector3D rv((float) (x1-x0), (float) (y1-y0), 0.0f);
            rv *= 0.0025; // magnitude for rotation
            float xt, yt;
            xt = delX - 0.3 * delZ * rv.x();
            yt = delY - 0.3 * delZ * rv.y();
            if(xt < xmax && xt > xmin) delX = xt;
            if(yt < ymax && yt > ymin) delY = yt;
        }
        else if(event->buttons() & Qt::RightButton){ // zoom in/out
            float zt;
            zt = delZ - 0.001 * delZ * (float) (y1 - y0);
            if(zt < -5.0f && zt > -180.f) delZ = zt;
        }
    }
    else if(event->buttons() & Qt::LeftButton){ // roll, i.e. rotate around vector in x, y plane
        QVector3D rv((float) (y0-y1), (float) (x1-x0), 0.0f); // orthogonal vector in plane
        float length = 0.0025 * rv.length(); // magnitude for rotation
        QQuaternion rot(cos(length), sin(length) * rv.normalized()); // rotation quaternion
        rot *= QuatOrient;
        QuatOrient = rot; // make a Left hand time ordered sequence, just like time dependent Quantum Mechanics
    }
    else if(event->buttons() & Qt::RightButton){ // rotate around z
        // xCentre, yCentre
        QVector3D rv0((float)(x0 - xCentre), (float) (y0 + yCentre), 0.0f);
        QVector3D rv1((float)(x1 - xCentre), (float) (y1 + yCentre), 0.0f);
        rv0.normalize();
        rv1.normalize();
        QVector3D rcp = QVector3D::crossProduct(rv0, rv1);
        float theta = 0.5 * asin(rcp.length()); // half angle for quaternion
        QQuaternion rot(cos(theta), sin(theta) * rcp.normalized()); // rotation quaternion
        rot *= QuatOrient;
        QuatOrient = rot;
    }
    lastPos = event->pos();
    if(paused) update();
}

void OGLWidget::incRotate()
{
    theta_a -= speed * delTheta;
    theta_b += speed * delTheta * (double) Na / (double) Nb;
}

void OGLWidget::paintGL()
{
    static bool first = true;
    if(first){
        QMatrix4x4 matrix;
        matrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 280.0f);
        glUniformMatrix4fv(uniPerspective, 1, GL_FALSE, matrix.data());
        first = false;
    }
    glUniform3f(uniLightPos, lightX, lightY, lightZ); // position for light source
    if(rebuild_flg){
        rebuild_flg = false;
        buildGears(true);
    }

    const qreal retinaScale = devicePixelRatio();
    const float delXa = -(float) Nb * 0.5f, delXb = (float) Na * 0.5f;

    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 matrixA, matrixB;
    QMatrix4x4 matRotA, matRotB;
    QMatrix4x4 matRot(QuatOrient.toRotationMatrix());

    matrixA.translate(delX, delY, delZ + 15.0);
    matrixA = matrixA * matRot;
    matrixB = matrixA; // store in matrix_b for later use
    matrixA.translate(delXa, 0.0f, 0.0f);
    matrixA.rotate(theta_a, 0.0f, 0.0f, 1.0f);
    matrixB.translate(delXb, 0.0f, 0.0f);
    matrixB.rotate(theta_b, 0.0f, 0.0f, 1.0f);
    matRotA = matRot;
    matRotA.rotate(theta_a, 0.0f, 0.0f, 1.0f);
    matRotB = matRot;
    matRotB.rotate(theta_b, 0.0f, 0.0f, 1.0f);

    // draw first gear
    glUniformMatrix4fv(uni, 1, GL_FALSE, matrixA.data()); // transpose is set to true/false
    glUniformMatrix4fv(uniRot, 1, GL_FALSE, matRotA.data());
    glUniform3f(uniColor, 0.1f, 0.2f, 0.5f); // set color
    glDrawElements(GL_TRIANGLES, Nind1_a, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
    glUniform3f(uniColor, 0.184314, 0.309804, 0.184314); // dark green
    glDrawElements(GL_TRIANGLES, Nind_a - Nind1_a, GL_UNSIGNED_INT, (void*)(Nind1_a * sizeof(GLuint)));

    // draw second gear
    glUniformMatrix4fv(uni, 1, GL_FALSE, matrixB.data()); // transpose is set to true/false
    glUniformMatrix4fv(uniRot, 1, GL_FALSE, matRotB.data());
    glUniform3f(uniColor, 0.1f, 0.1f, 0.4f); // set color
    glDrawElements(GL_TRIANGLES, Nind1_b, GL_UNSIGNED_INT, (void*)(Nind_a * sizeof(GLuint)));
    glUniform3f(uniColor, 0.25f, 0.25f, 0.25f); // grey
    glDrawElements(GL_TRIANGLES, Nind_b - Nind1_b, GL_UNSIGNED_INT, (void*)((Nind_a + Nind1_b) * sizeof(GLuint)));
}

