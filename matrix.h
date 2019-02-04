// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

class matrix
{
public:
    matrix();
    GLfloat* data() { return vdata.data(); }
    int putVdat(int i, GLfloat x) { if(i>=n2 || i<0) return 1; 
        vdata[i] = x; return 0; }
    int perspective(GLfloat theta, GLfloat ar, GLfloat zn, GLfloat zf);
    void scale(GLfloat s);
    void shift(GLfloat step);
    void rotate(GLfloat theta, GLfloat ux, GLfloat uy, GLfloat uz);
    void print();
private:
    const int n, n2;
    std::vector<GLfloat> vdata;  
    void LMultiply(matrix lm);
};
