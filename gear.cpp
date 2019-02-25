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
// pa is pressure angle in radians

static const float pi = 3.1415926535897932f;
static const float Df = 2.157f; // tooth depth
static float gap = 0.52; // number less than 0.5: side clearance on circular pitch
static const unsigned int Ninv = 16; // number of involute vertices on one side of tooth, incuding fillet curve
static const unsigned int Nfillet = 5; // number of extra points used for tooth root fillet curve, must be 2 or more
static const float filletR = 0.4; // radius of tooth root fillet

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
    del_gap = 2.0f * (1.0f - gap) * pi / (float) N;
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


// callculate involute with fillet radius for case where
// fillet is entirely inside base circle
void gear::sectorFillet()
{
    float x0, y0, dtheta1, dtheta2;

    {
        float x, y, theta;

        theta = - rp * sinpa / rbc; // angle where involute crosses base circle
        x0 = filletR;
        y0 = rbc; // put fillet tangent to y sector line at base circle radius
        dtheta1 = pa + theta; // angle to rotate onto tooth
        dtheta2 = gap * pi / (float) N; // rotate gear so middle of gullet is on y axis
        dtheta1 += dtheta2;
        x = x0 * cos(dtheta1) - y0 * sin(dtheta1);
        y = x0 * sin(dtheta1) + y0 * cos(dtheta1); // now have height of fillet radius
        if(y < rmin + filletR){ // fillet's on the involute
            involute_fillet();
            //involute_part();
            return;
        }

        float dy = rmin + filletR - y;
        y += dy;
        x -= dy * tan(dtheta2);
        x0 = x;
        y0 = y;
        dtheta2 = -dtheta2;
        x = x0 * cos(dtheta2) - y0 * sin(dtheta2);
        y = x0 * sin(dtheta2) + y0 * cos(dtheta2);
        x0 = x; // x0, y0 are centre coordinate of fillet
        y0 = y;
        dtheta1 = pa + theta; // angle from y axis where involute crosses base circle
        dtheta2 = -dtheta2;   // angle to rotate gear so middle of gullet is on y axis
    }

    float x, y, r;
    float xd, yd;
    float theta, norm;
    float cost, sint;

    // point on base circle, largest diameter of sector
    //theta = -sinpa / cospa; // = -tan(pa)
    theta = dtheta1 - pa;
    cost = cos(pa + theta);
    sint = sin(pa + theta);
    x = -rbc * sint + rp * (sinpa + theta * cospa) * cost;
    y = rbc * cost + rp * (sinpa + theta * cospa) * sint;
    invo_curve_x[Nfillet] = x;
    invo_curve_y[Nfillet] = y;
    xd = y; // start of sector, norm is tangent to base circle
    yd = -x;
    norm = 1.0f / sqrtf(xd * xd + yd * yd);
    invo_curve_xn[Nfillet] = xd * norm;
    invo_curve_yn[Nfillet] = yd * norm;

    // fillets
    float theta1, theta2;
    theta1 = 1.5f * pi - dtheta2; // blend into gullet
    theta2 = pi + dtheta1; // blend into sector
    const float delTheta = (theta2 - theta1) / (float) (Nfillet - 1);
    theta = theta1;
    for(unsigned int i=0; i<Nfillet; ++i){
        cost = cos(theta);
        sint = sin(theta);
        x = x0 + filletR * cost;
        y = y0 + filletR * sint;
        invo_curve_x[i] = x;
        invo_curve_y[i] = y;
        norm = 1.0f / sqrtf(cost * cost + sint * sint);
        invo_curve_xn[i] = -cost * norm;
        invo_curve_yn[i] = -sint * norm;
        theta += delTheta;
    }

    theta = -0.5f * sinpa / cospa; // half way between pitch circle and base circle
    const float dr = (rmaj - rbc) / (float) (Ninv - Nfillet - 1);
    for(unsigned int i=Nfillet+1; i<Ninv; ++i){
        r = (float)(i - Nfillet) * dr + rbc;
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


// callculate involute with fillet radius for case where
// fillet is tangent to involute
void gear::involute_fillet()
{
    float x, y, dx, dy;
    float theta, beta;
    float hPrime;
    const float gamma = 2.0f * gap * pi / (float) N;
    const float sing = sin(gamma), cosg = cos(gamma);

    theta = 0.0f;
    float cost, sint;
    float xf, yf, dh;
    for(int i=0; i<6; ++i){
        cost = cos(pa + theta);
        sint = sin(pa + theta);
        x = -rbc * sint + rp * (sinpa + theta * cospa) * cost;
        y = rbc * cost + rp * (sinpa + theta * cospa) * sint;
        // derivatives wrt theta
        dx = -y + rp * cospa * cost;
        dy = x + rp * cospa * sint;
        beta = -atan(dx / dy);
        xf = x + filletR * cos(beta);
        yf = y + filletR * sin(beta);
        hPrime = sing * xf + cosg * yf;
        dh = hPrime - rmin - filletR;

        // now calculate derivative of gradient
        float ddx, ddy, gdd, dhPrime;
        ddx = -dy - rp * cospa * sint;
        ddy = dx + rp * cospa * cost;
        // gdd is d/d_theta (dx / dy)
        gdd = ddx / dy - dx * ddy / (dy * dy);
        float dxdy = dx / dy;
        float dBeta_dTheta = -1.0f / (1.0f + dxdy * dxdy) * gdd;
        dhPrime = sing * (dx - filletR * sin(beta) * dBeta_dTheta);
        dhPrime += cosg * (dy + filletR * cos(beta) * dBeta_dTheta);
        theta -= dh / dhPrime;
    }

    // fillets
    float r = sqrt(x * x + y * y); // currently smallest diameter of involute
    float x0 = x + filletR * cos(beta);
    float y0 = y + filletR * sin(beta);
    float theta1 = 1.5f * pi - gap * pi / (float) N; // blend into gullet
    float theta2 = pi + beta; // blend into tooth
    const float delTheta = (theta2 - theta1) / (float) (Nfillet - 1);
    theta = theta1;
    float norm;
    for(unsigned int i=0; i<Nfillet; ++i){
        cost = cos(theta);
        sint = sin(theta);
        x = x0 + filletR * cost;
        y = y0 + filletR * sint;
        invo_curve_x[i] = x;
        invo_curve_y[i] = y;
        norm = 1.0f / sqrtf(cost * cost + sint * sint);
        invo_curve_xn[i] = -cost * norm;
        invo_curve_yn[i] = -sint * norm;
        theta += delTheta;
    }

    float dr = (rmaj - r) / (float) (Ninv - Nfillet);
    for(unsigned int i=Nfillet; i<Ninv; ++i){
        r += dr;
        NewtonRaphson(6, r, theta, x, y);
        invo_curve_x[i] = x;
        invo_curve_y[i] = y;
        dx = -y + rp * cospa * cos(pa + theta);
        dy = x + rp * cospa * sin(pa + theta);
        norm = 1.0f / sqrt(dx * dx + dy * dy);
        invo_curve_xn[i] = dy * norm;
        invo_curve_yn[i] = -dy * norm;
    }
}

// method calls computation of the verticies for the involute and
// stores them in arrays invo_curve_x[], invo_curve_y[]
void gear::involute()
{
    sectorFillet();
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


// returns the tangent angle at given point on the involute
float gear::tangent(float theta)
{
    float dx, dy;
    float cost, sint;

    cost = cos(pa + theta);
    sint = sin(pa + theta);
    dx = -rbc * cost + rp * cospa * cost - rp * (sinpa + theta * cospa) * sint;
    dy = -rbc * sint + rp * cospa * sint + rp * (sinpa + theta * cospa) * cost;
    return atan(-dx / dy);
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







