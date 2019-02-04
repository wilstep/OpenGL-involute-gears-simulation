// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3


#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>
#include "matrix.h"

const GLfloat pi = 3.1415926535897932;

matrix::matrix():n(4), n2(4 * 4)
{
    vdata.resize(n * n, 0.0f); 
    int j = 0; 
    // make it a unit matrix
    for(int i=0; i<n; ++i){
        vdata[j] = 1.0f;
        j += n + 1;    
    } 
}

void matrix::print()
{
    int i, j, k = 0;
    
    for(i=0; i<n; ++i){
        for(j=0; j<n-1; ++j) std::cout << vdata[k++] << ", ";
        std::cout << vdata[k++] << std::endl;
    }
    std::cout << std::endl;
}

// left hand multiply lm * (*this)
void matrix::LMultiply(matrix lm){
    std::vector<GLfloat> bv(n*n);
    std::vector<GLfloat*> lh(n), rh(n), buff(n);
    
    // turn vectors into 2d arrays
    int i, j, k;
    for(i=0; i<n; ++i){
        j = i * n;
        if(i == 0){
            lh[i] = lm.data();
            rh[i] = vdata.data();
            buff[i] = bv.data();
        }
        else{
            lh[i] = lh[0] + j;
            rh[i] = rh[0] + j;
            buff[i] = buff[0] + j;
        }
    }
    for(i=0; i<n; ++i)
        for(j=0; j<n; ++j){
            buff[i][j] = 0.0;
            for(k=0; k<n; ++k) buff[i][j] += lh[i][k] * rh[k][j];       
        }
    vdata = bv;
}

void matrix::scale(GLfloat s)
{
    for(auto &a: vdata) a = 0.0f; 
    vdata[0] = vdata[5] = vdata[10] = s;
    vdata[15] = 1.0;   
}

void matrix::shift(GLfloat step)
{
    for(auto &a: vdata) a = 0.0f; 
    vdata[0] = vdata[5] = vdata[10] = 1.0;
    vdata[3] = vdata[7] = vdata[11] = step;
    vdata[15] = 1.0;   
}

// multiply the current matrix by the specified rotation matrix
void matrix::rotate(GLfloat theta, GLfloat ux, GLfloat uy, GLfloat uz)
{
    GLfloat norm;
    GLfloat sint, cost;
    matrix m;
    
    norm = ux * ux + uy * uy + uz * uz;
    norm = 1.0 / sqrt(norm);
    ux *= norm;
    uy *= norm;
    uz *= norm;
    theta = theta * pi / 180.0f;
    sint = sin(theta);
    cost = cos(theta);
    // m currently a unit matrix
    // first row
    vdata[0] = cost + ux * ux * (1.0f - cost);
    vdata[1] = ux * uy * (1.0f - cost) - uz * sint;
    vdata[2] = ux * uz * (1.0f - cost) + uy * sint;
    // second row 
    vdata[4] = uy * ux * (1.0f - cost) + uz * sint;
    vdata[5] = cost + uy * uy * (1.0f - cost);
    vdata[6] = uy * uz * (1.0f - cost) - ux * sint;
    // third row
    vdata[8] = uz * ux * (1.0f - cost) - uy * sint;
    vdata[9] = uz * uy * (1.0f - cost) + ux * sint;
    vdata[10] = cost + uz * uz * (1.0f - cost);
    vdata[3] = vdata[7] = vdata[11] = vdata[12] = vdata[13] = vdata[14] = 0.0f;
    vdata[15] = 1.0f;
}

// ar is the screen's aspect ratio, width / height
int matrix::perspective(GLfloat theta, GLfloat ar, GLfloat zn, GLfloat zf)
{
    if(n != 4) return 1; 
    theta = theta * pi / 360.0f; // half angle in radians
    GLfloat tanx = tan(theta);
    
    for(auto &a: vdata) a = 0.0f; 
    vdata[0] = 1.0f / (tanx * ar);
    vdata[5] = 1.0f / tanx;
    vdata[10] = -(zf + zn) / (zf - zn);
    vdata[11] = -2.0f * zf * zn / (zf - zn);
    vdata[14] = -1.0;
    return 0;
}
