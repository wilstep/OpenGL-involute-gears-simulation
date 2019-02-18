// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3


#include <vector>
#include <array>
#include <iostream>
#include <cmath>
#include "gear.h"

// cutter depth D+f = 2.157 / DP (inches), or 2.157 M (mm)
// side clearance on circular pitch (measured around circumferance), tooth 0.48 wide, gap 0.52
// major diameter (N+2)/N * pitch diameter
// minor diameter (N-2.314) * pitch diameter

// rp is the pitch radius

static const float pi = 3.1415926535897932f;
//static const float pa = (20.0f / 180.0f * pi); // pressure angle in radians
static const float Df = 2.157f; // tooth depth
static const unsigned int Ninv = 14; // number of involute vertices on one side of tooth, incuding fillet curve
static const unsigned int Nfillet = 1; // number of extra points used for tooth root fillet curve
static const float filletR = 0.2; // radius of tooth root fillet

gear::gear(unsigned int Ni, float pai, float dZ):N(Ni), nVertices(8*(1+Ninv)*Ni+2), nIndices(24*Ninv*Ni),
    n1indices(Ni *(12*Ninv+6)), rp((float) Ni / 2.0f), rbc(rp * cos(pai)), rmaj((float) (Ni+2) / 2.0f),
    rmin((float) (Ni+2) / 2.0f - Df), delZ(dZ), pa(pai), cospa(cos(pai)), sinpa(sin(pai))
{
    verts.resize(6*nVertices);
    vert_it = verts.end() - 12; // offset for centre verticies
    inds.resize(nIndices);
    ind_it0 = inds.begin(); // the blue stuff, 12 * Ninv + 6 indicies
    ind_it1 = ind_it0 + n1indices; // the cut stuff, 12 * Ninv - 6 indicies
    invo_curve_x.resize(Ninv);
    invo_curve_y.resize(Ninv);
    invo_curve_xn.resize(Ninv);
    invo_curve_yn.resize(Ninv);

    sectorVerts();
    for(unsigned int i=0; i<N; ++i) sectorV(i);
    sectorIndicies();
    for(unsigned int i=0; i<N; ++i) sectorI(i);
}

gear::~gear()
{
}

// make a preliminary (2D in xy plane only) template
// consisting of 1 tooth worth of verticies
// and their norms for the two curved involute faces
// either side of tooth.
void gear::sectorVerts()
{
    unsigned int i, j;
    float x, y, xn, yn;
    float sinx, cosx;
    float del_gap;
    const unsigned int Nt = 2 * Ninv;

    vertx.resize(Nt);
    verty.resize(Nt);
    vertxn.resize(Nt); // norms to involute
    vertyn.resize(Nt); // norms to involute
    involute();
    del_gap = 2.0f * 0.48 * pi / (float) N;
    // rotate involute after flipping for 2nd side
    sinx = sin(del_gap);
    cosx = cos(del_gap);
    for(i=0; i<Ninv; ++i){
        j = Ninv + i;
        x = -(vertx[i] = invo_curve_x[i]);
        y = verty[i] = invo_curve_y[i];
        xn = -(vertxn[i] = invo_curve_xn[i]);
        yn = vertyn[i] = invo_curve_yn[i];
        vertx[j] = cosx * x - sinx * y;
        verty[j] = sinx * x + cosx * y;
        vertxn[j] = cosx * xn - sinx * yn;
        vertyn[j] = sinx * xn + cosx * yn;
    }
    // 2 centre verticies, common to all teeth
    unsigned int cnt = 0;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = delZ;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = 1.0f;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = -delZ;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = 0.0f;
    vert_it[cnt++] = -1.0f;
}

// 8 * N * (1 + Ninv) + 2 verticies total
// 8 * (1 + Nivn) verticies per sector
// 6 floats per vertex
void gear::sectorV(unsigned int n)
{
    unsigned int i, j, k, cnt;
    const unsigned int N1 = Ninv - 1;
    float cosx, sinx, theta;
    // use iterator to move to this sector
    std::vector<float>::iterator vr = verts.begin() + 48 * n * (1 + Ninv);

    // rotate whole tooth by theta
    theta = 2.0f * pi * (float) n / (float) N;
    cosx = cos(theta);
    sinx = sin(theta);
    // outside diameter of gear blank, 4 verticies
    cnt = 0;
    j = Ninv + N1;
    // front first
    vr[cnt++] = cosx * vertx[N1] - sinx * verty[N1];
    vr[cnt++] = sinx * vertx[N1] + cosx * verty[N1];
    vr[cnt++] = delZ;
    vr[cnt++] = vr[0] / rmaj; // normal vector,
    vr[cnt++] = vr[1] / rmaj;
    vr[cnt++] = 0.0f;
    // front second
    vr[cnt++] = cosx * vertx[j] - sinx * verty[j];
    vr[cnt++] = sinx * vertx[j] + cosx * verty[j];
    vr[cnt++] = delZ;
    vr[cnt++] = vr[6] / rmaj; // normal vector
    vr[cnt++] = vr[7] / rmaj;
    vr[cnt++] = 0.0f;
    // back first
    i = 0;
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = -vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    // back second
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = -vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    // Now do minor diameter (between teeth), 4 verticies already done
    // front first
    i = j = cnt;
    vr[cnt++] = cosx * vertx[0] - sinx * verty[0];
    vr[cnt++] = sinx * vertx[0] + cosx * verty[0];
    vr[cnt++] = delZ;
    vr[cnt++] = vr[j] / rmin; // normal vector
    vr[cnt++] = vr[j+1] / rmin;
    vr[cnt++] = 0.0f;
    // front second
    j = cnt;
    vr[cnt++] = cosx * vertx[Ninv] - sinx * verty[Ninv];
    vr[cnt++] = sinx * vertx[Ninv] + cosx * verty[Ninv];
    vr[cnt++] = delZ;
    vr[cnt++] = vr[j] / rmin; // normal vector
    vr[cnt++] = vr[j+1] / rmin;
    vr[cnt++] = 0.0f;
    // back first
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = -vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    // back second
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = -vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    vr[cnt++] = vr[i++];
    // Now: move onto involute faces, 8 verticies already done
    for(i=0; i<Ninv; ++i){
        k = i + Ninv;
        // first front face
        vr[cnt++] = cosx * vertx[i] - sinx * verty[i];
        vr[cnt++] = sinx * vertx[i] + cosx * verty[i];
        vr[cnt++] = delZ;
        vr[cnt++] = cosx * vertxn[i] - sinx * vertyn[i];
        vr[cnt++] = sinx * vertxn[i] + cosx * vertyn[i];
        vr[cnt++] = 0.0f;
        // second front face
        vr[cnt++] = cosx * vertx[k] - sinx * verty[k];
        vr[cnt++] = sinx * vertx[k] + cosx * verty[k];
        vr[cnt++] = delZ;
        vr[cnt++] = cosx * vertxn[k] - sinx * vertyn[k];
        vr[cnt++] = sinx * vertxn[k] + cosx * vertyn[k];
        vr[cnt++] = 0.0f;
        // first back face
        vr[cnt++] = cosx * vertx[i] - sinx * verty[i];
        vr[cnt++] = sinx * vertx[i] + cosx * verty[i];
        vr[cnt++] = -delZ;
        vr[cnt++] = cosx * vertxn[i] - sinx * vertyn[i];
        vr[cnt++] = sinx * vertxn[i] + cosx * vertyn[i];
        vr[cnt++] = 0.0f;
        // second back face
        vr[cnt++] = cosx * vertx[k] - sinx * verty[k];
        vr[cnt++] = sinx * vertx[k] + cosx * verty[k];
        vr[cnt++] = -delZ;
        vr[cnt++] = cosx * vertxn[k] - sinx * vertyn[k];
        vr[cnt++] = sinx * vertxn[k] + cosx * vertyn[k];
        vr[cnt++] = 0.0f;
    }
    // gear blank faces, sides of teeth, 8 + 4 * Ninv verticies already done
    for(i=0; i<Ninv; ++i){
        k = i + Ninv;
        // first front face
        vr[cnt++] = cosx * vertx[i] - sinx * verty[i];
        vr[cnt++] = sinx * vertx[i] + cosx * verty[i];
        vr[cnt++] = delZ;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 1.0f;
        // second front face
        vr[cnt++] = cosx * vertx[k] - sinx * verty[k];
        vr[cnt++] = sinx * vertx[k] + cosx * verty[k];
        vr[cnt++] = delZ;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 1.0f;
        // first back face
        vr[cnt++] = cosx * vertx[i] - sinx * verty[i];
        vr[cnt++] = sinx * vertx[i] + cosx * verty[i];
        vr[cnt++] = -delZ;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 0.0f;
        vr[cnt++] = -1.0f;
        // second back face
        vr[cnt++] = cosx * vertx[k] - sinx * verty[k];
        vr[cnt++] = sinx * vertx[k] + cosx * verty[k];
        vr[cnt++] = -delZ;
        vr[cnt++] = 0.0f;
        vr[cnt++] = 0.0f;
        vr[cnt++] = -1.0f;
    }
    // centres, 8 * (Ninv + 1) done
}


// Set the incicies for drawing the triangles from the vertercies
void gear::sectorIndicies()
{
    unsigned int i, j, k, dN;
    const unsigned int N1 = Ninv - 1;

    inds0.resize(12 * Ninv + 6); // blank's triangles (blue)
    inds1.resize(12 * Ninv - 6); // cut surface's triangles
    // first render the blank (blue)
    // sides of teeth, 8 + 4 * Ninv verticies already done
    dN = 8 + 4 * Ninv;
    for(i=j=0; i<N1; ++i){
        k = 4 * i + dN;
        // front 2 triangles
        inds0[j++] = k;
        inds0[j++] = k + 1;
        inds0[j++] = k + 4;
        inds0[j++] = k + 1;
        inds0[j++] = k + 4;
        inds0[j++] = k + 5;
        // back 2 triangles
        inds0[j++] = k + 2;
        inds0[j++] = k + 3;
        inds0[j++] = k + 6;
        inds0[j++] = k + 3;
        inds0[j++] = k + 6;
        inds0[j++] = k + 7;
    }  // currently j = 12 * (Ninv - 1) indicies
    // outside diameter,
    k = 0; // front tooth OD
    inds0[j++] = k;
    inds0[j++] = k + 1;
    inds0[j++] = k + 2;
    inds0[j++] = k + 2;
    inds0[j++] = k + 3;
    inds0[j++] = k + 1;
    // side faces, 8 + 4 * Ninv verticies already done
    // currently j = 12 * N1 + 6
    //////////////////////////
    // Now do 2 triangles which go to the centre
    i = nVertices - 2; // centres
    dN = 8 + 4 * Ninv;
    k = 8 * (Ninv + 1) * (N - 1); // vertex off neighbouring sector, around the world less one!
    inds0[j++] = i; // front face centre
    inds0[j++] = dN;
    inds0[j++] = dN + 1;
    inds0[j++] = i;
    inds0[j++] = dN;
    inds0[j++] = dN + k + 1;     // around world
    inds0[j++] = i + 1;      // back face centre
    inds0[j++] = dN + 2;
    inds0[j++] = dN + 3;
    inds0[j++] = i + 1;
    inds0[j++] = dN + 2;
    inds0[j++] = dN + k + 3; //around world
    //////////////////////////////////////////////
    // now for the cut surfaces, different colour
    // involute faces, start 8 verticies down list
    for(i=j=0; i<N1; ++i){
        k = 4 * i + 8;
        // first rectangle
        inds1[j++] = k;
        inds1[j++] = k + 4;
        inds1[j++] = k + 2;
        inds1[j++] = k + 4;
        inds1[j++] = k + 2;
        inds1[j++] = k + 6;
        // rectangle on opposite side
        inds1[j++] = k + 1;
        inds1[j++] = k + 5;
        inds1[j++] = k + 3;
        inds1[j++] = k + 5;
        inds1[j++] = k + 3;
        inds1[j++] = k + 7;
    } // currently j = 12 * (Ninv - 1)
    // minor diameter
    k = 8 * (Ninv + 1) * (N - 1); // vertex off neighbouring sector, around the world less one!
    inds1[j++] = 4; // front face of gear disk
    inds1[j++] = 6; // rear face of gear disk
    inds1[j++] = k + 5; // front face, around the world
    inds1[j++] = k + 5;
    inds1[j++] = k + 7; // rear face, around the world
    inds1[j++] = 6;
    // if(ind_it[i] >= nVertices) ind_it[i] -= nVertices - 2;
}


// 8 * N * (1 + Ninv) + 2 verticies per sector
void gear::sectorI(unsigned int n)
{
    unsigned int i;
    const unsigned int Nmax = nVertices - 2, N1 = Ninv - 1;
    std::vector<unsigned int>::iterator it0, it1;
    const unsigned int dnv = 8 * (Ninv + 1) * n; // rotate sectors
    const unsigned int nc1 = 12 * N1 + 6, nc2 = 12 * N1 + 9;
    const unsigned int nc3 = 12 * N1 + 12, nc4 = 12 * N1 + 15;
    unsigned int Ni;

    it0 = ind_it0 + n * (12 * Ninv + 6); // the blank stuff (blue)
    it1 = ind_it1 + n * (12 * Ninv - 6); // the cut stuff
    Ni = 12 * Ninv + 6;
    for(i=0; i<Ni; ++i){
        it0[i] = inds0[i];
        if(i != nc1 && i != nc2 && i != nc3 && i != nc4){
            it0[i] += dnv; // don't move centres
            if(it0[i] >= Nmax) it0[i] -= Nmax;  // move backwards around the world!
        }
    }
    Ni = 12 * Ninv - 6;
    for(i=0; i<Ni; ++i){
        it1[i] = inds1[i] + dnv;
        if(it1[i] >= Nmax) it1[i] -= Nmax;  // move backwards around the world!
    }
}


// cutter depth D+f = 2.157 / DP (inches), or 2.157 M (mm)
// side clearance on circular pitch (measured around circumferance), tooth 0.48 wide, gap 0.52
// major diameter (N+2)/N * pitch diameter
// minor diameter (N-2.314) * pitch diameter

// method computes the verticies for the involute and
// stores them in arrays invo_curve_x[], invo_curve_y[]
void gear::involute()
{
    unsigned int Ncrit = 2.0 * 1.157 / (1.0 - cos(pa));
    if(N > Ncrit) involute_pure(); // full involute
    else involute_part();   // involute base circle has been cut
}

void gear::involute_pure()
{
    float x, y, r;
    float xd, yd, norm;
    float theta, del;

    theta = -sinpa / cospa; // puts on base circle
    theta *= (rp - rmin) / (rp - rbc);
    del = (rmaj - rmin) / (float)(Ninv - 1);
    for(unsigned int i=0; i<Ninv; ++i){
        r = rmin + (float) i * del;
        NewtonRaphson(6, r, theta, x, y);
        invo_curve_x[i] = x;
        invo_curve_y[i] = y;
        xd = -y + rp * cospa * cos(pa + theta);
        yd = x + rp * cospa * sin(pa + theta);
        norm = 1.0f / sqrtf(xd * xd + yd * yd);
        invo_curve_xn[i] = yd * norm;
        invo_curve_yn[i] = -xd * norm;
        r = sqrtf(x * x + y * y);
        //std::cout << "x = " << x << ", y = " << y << ", r = " << r;
        //std::cout << ", xn = " << invo_curve_xn[i] << ", yn = " << invo_curve_yn[i];
        //std::cout << ", angle = " << atan(invo_curve_yn[i] / invo_curve_xn[i]) * 180.0f / pi;
        //std::cout << ", theta = " << theta << std::endl;
    }
}

// Nfillet, filletR, Ninv

void gear::involute_part()
{
    float x, y, r;
    float xd, yd;
    float theta, norm;
    float cost, sint;
    unsigned int n1 = Ninv / 2;

    // place on pitch circle
    x = -rbc * sinpa + rp * sinpa * cospa;
    y = rbc * cospa + rp * sinpa * sinpa;
    invo_curve_x[n1] = x;
    invo_curve_y[n1] = y;
    xd = -y + rp * cospa * cospa;
    yd = x + rp * cospa * sinpa;
    norm = 1.0f / sqrtf(xd * xd + yd * yd);
    invo_curve_xn[n1] = yd * norm;
    invo_curve_yn[n1] = -xd * norm;

    // point on base circle
    theta = -sinpa / cospa;
    cost = cos(pa + theta);
    sint = sin(pa + theta);
    x = -rbc * sint + rp * (sinpa + theta * cospa) * cost;
    y = rbc * cost + rp * (sinpa + theta * cospa) * sint;
    r = sqrtf(x * x + y * y);
    invo_curve_x[1] = x;
    invo_curve_y[1] = y;
    xd = -x; // on base circle, norm is tangent to circle for straight line
    yd = y;  // from this point, is a corner
    norm = 1.0f / sqrtf(xd * xd + yd * yd);
    invo_curve_xn[0] = invo_curve_xn[1] = yd * norm;
    invo_curve_yn[0] = invo_curve_yn[1] = -xd * norm;

    // point on minor diameter
    invo_curve_x[0] = x * rmin / r;
    invo_curve_y[0] = y * rmin / r;
    theta *= 0.5; // half way between pitch circle and base circle
    for(unsigned int i=2; i<Ninv; ++i){
        if(i==n1) continue;  // on pitch circle
        if(i < n1) r = 0.5 * (rp - rbc) + rbc;
        else r = (float)(i - n1) / (float) n1 * (rmaj - rp) + rp;
        NewtonRaphson(6, r, theta, x, y);
        invo_curve_x[i] = x;
        invo_curve_y[i] = y;
        xd = -y + rp * cospa * cos(pa + theta);
        yd = x + rp * cospa * sin(pa + theta);
        norm = 1.0f / sqrtf(xd * xd + yd * yd);
        invo_curve_xn[i] = yd * norm;
        invo_curve_yn[i] = -xd * norm;
    }
}

// find coords to bring involute curve to distance r from centre
// theta inputs initial guess for its value, x and y input are garbage
void gear::NewtonRaphson(unsigned int n, const float r, float &theta, float &x, float &y)
{
    float cost, sint;
    float dx, dy, dr;
    float rx;
    float del_theta;

    for(unsigned int i=0; i<n; ++i){
        cost = cos(pa + theta);
        sint = sin(pa + theta);
        x = -rbc * sint + rp * (sinpa + theta * cospa) * cost;
        y = rbc * cost + rp * (sinpa + theta * cospa) * sint;
        rx = sqrtf(x * x + y * y);
        dx = -rbc * cost + rp * cospa * cost - rp * (sinpa + theta * cospa) * sint;
        dy = -rbc * sint + rp * cospa * sint + rp * (sinpa + theta * cospa) * cost;
        dr = x * dx + y * dy;
        dr /= rx;
        del_theta = (r - rx) / dr;
        theta += del_theta;
        //std::cout << "theta = " << theta << ", rx = " << rx << ", r = " << r << std::endl;
    }
    cost = cos(pa + theta);
    sint = sin(pa + theta);
    x = -rbc * sint + rp * (sinpa + theta * cospa) * cost;
    y = rbc * cost + rp * (sinpa + theta * cospa) * sint;
    rx = sqrtf(x * x + y * y);
}

void gear::RotateVerts(float theta)
{
    theta = pi * theta / 180.0f;
    const float cosx = cos(theta);
    const float sinx = sin(theta);
    float x, y;

    for(unsigned int i=0, j; i<nVertices; ++i){
        j = i * 6;
        x = verts[j];
        y = verts[j+1];
        verts[j] = cosx * x - sinx * y;
        verts[j+1] = sinx * x + cosx * y;
    }
}







