// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#ifndef GEAR_H
#define GEAR_H

class gear
{
public:
    gear(unsigned int Ni, float pai, float dZ);
    ~gear();
    std::vector<float>& GetVerts(){ return verts; }
    std::vector<unsigned int>& GetInds(){ return inds; }
    unsigned int GetNInds(){ return nIndices; }
    unsigned int GetN1Inds(){ return n1indices; }
    unsigned int GetNverts() { return nVertices; }
    void RotateVerts(float);
private:
    void sectorVerts();
    void sectorIndicies();
    void sectorV(unsigned int n);
    void sectorI(unsigned int n);
    void involute();
    void sectorFillet();
    void involute_pure();
    void involute_part();
    void NewtonRaphson(unsigned int n, const float r, float &theta, float &x, float &y);
    float tangent(float theta);
    const unsigned int N, nVertices, nIndices, n1indices;
    // pitch radius, base circle radius, major radius, minor radius
    const float rp, rbc, rmaj, rmin, delZ;
    const float pa, cospa, sinpa;
    std::vector<float> verts;
    std::vector<float>::iterator vert_it;
    std::vector<float> vertx, verty, vertxn, vertyn;
    std::vector<unsigned int> inds;
    std::vector<unsigned int> inds0, inds1; // two colours
    std::vector<unsigned int>::iterator ind_it0, ind_it1;
    std::vector<float> invo_curve_x, invo_curve_y, invo_curve_xn, invo_curve_yn;
};

#endif // GEAR_H
