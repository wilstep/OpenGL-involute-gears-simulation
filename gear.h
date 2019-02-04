// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

class gear
{
public:
    gear(GLuint Ni);
    std::vector<GLfloat> GetVerts(){ return verts; };
    std::vector<GLuint> GetInds(){ return inds; };
    GLuint GetNInds(){ return nIndices; }
    GLuint GetN1Inds(){ return n1indices; }
    GLuint GetNverts() { return nVertices; }
    void RotateVerts(GLfloat);
private:
    void sectorVerts();
    void sectorIndicies();
    void sectorV(GLuint n);
    void sectorI(GLuint n);
    void involute();
    void involute_pure();
    void involute_part();
    void NewtonRaphson(GLuint n, const GLfloat r, GLfloat &theta, GLfloat &x, GLfloat &y);
    const GLuint N, nVertices, nIndices, n1indices;
    // pitch radius, base circle radius, major radius, minor radius
    const GLfloat rp, rbc, rmaj, rmin, z; 
    const GLfloat cospa, sinpa;
    std::vector<GLfloat> verts;
    std::vector<GLfloat>::iterator vert_it;
    std::vector<GLfloat> vertx, verty, vertxn, vertyn;
    std::vector<GLuint> inds;
    std::vector<GLuint> inds0, inds1; // two colours
    std::vector<GLuint>::iterator ind_it0, ind_it1;
    std::vector<GLfloat> invo_curve_x, invo_curve_y, invo_curve_xn, invo_curve_yn;
};
