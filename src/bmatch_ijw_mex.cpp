/*
 * B-matching toolbox
 *  Stuart Andrews
 *
 * Many thanks to contributors for early versions and suggestions
 *  Vlad Shchogolev
 *  Blake Shaw
 *  Bert Huang
 * */

#include <string>
using namespace std;
#include <time.h>
#include "mex.h"
#include "Vector.hpp"
#include "goblinexact.hpp"
#include "AbstractBmatchSolver.hpp"
#include "greedyapprox.hpp"
#include "goblinexact.hpp"
#include "beliefprop.hpp"

/* from mathworks mexcpp.cpp */
extern void _main();

/******************************************************************/
/* Interface for the ijw-based MEX function */
/******************************************************************/
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    register int    k;
    int             ii;
    const int *     dims;

    int             verbose;
    int             method;
    int             success;

    int             num_node;
    const double *  deg_bdd_ptr;
    DoubleVec       deg_bdd_vec;

    int             num_ijw;
    const double *  ijw_ptr;
    DoubleVec       ijw_vec;

    int             num_bmatch_edge;
    double *        bmatch_edge_ptr;
    DoubleVec       bmatch_edge_vec;
    double          bmatch_wgt;

    time_t          time1;
    time_t          time2;
    double          total_time;

    AbstractBmatchSolver * ALG;
    string          method_str;

    /* */
    time1 = time(&time1);

    /* */
    num_bmatch_edge = 0;
    bmatch_edge_vec.clear();
    bmatch_wgt = -1;

    /* one or two outputs */
    if (nlhs > 3) {
        mexErrMsgTxt("Too many output arguments.\n");
    }

    /* first argument */
    ii = 0;
    if (mxGetClassID(prhs[ii])!=mxDOUBLE_CLASS) {
        mexErrMsgTxt("Could not convert ARG 1 to double.");
    }
    if (mxGetNumberOfDimensions(prhs[ii])<2) {
        mexErrMsgTxt("Could not convert ARG 1 to weighted edge list.");
    }
    dims = mxGetDimensions(prhs[ii]);
    num_ijw = int(dims[0]);
    if (dims[1]!=3) {
        mexErrMsgTxt("size(ARG_1) must be NUM_EDGE x 3.");
    }
    /* */
    ijw_ptr = mxGetPr(prhs[ii]);

    /* find the maximum node number */
    num_node = -1;
    for (ii=0; ii<2*num_ijw; ii++) {
        if (ijw_ptr[ii]>num_node) {
            num_node = int(ijw_ptr[ii]);
        }
    }
    num_node = num_node + 1;

    /* second argument */
    ii = 1;
    if (mxGetClassID(prhs[ii])!=mxDOUBLE_CLASS) {
        mexErrMsgTxt("Could not convert ARG 2 to double.");
    }
    if (mxGetNumberOfDimensions(prhs[ii])<2) {
        mexErrMsgTxt("Could not convert ARG 2 to degree bounds.");
    }
    dims = mxGetDimensions(prhs[ii]);
    if ((dims[0]<num_node) || (dims[1]!=2)) {
        mexErrMsgTxt("size(ARG_2) must be Nx2, N > max(max(ARG_1(:,[1,2]))).");
    }
    /* */
    deg_bdd_ptr = mxGetPr(prhs[ii]);

    /* update maximum node number */
    if (dims[0]>num_node) {
        num_node = dims[0];
    }

    /* optional third argument */
    method = 1;
    if (nrhs > 2) {
        ii = 2;
        if (mxGetClassID(prhs[ii])!=mxDOUBLE_CLASS) {
            mexErrMsgTxt("Could not convert ARG 3 to double.");
        }
        if (mxGetNumberOfElements(prhs[ii])!=1) {
            mexErrMsgTxt("Could not convert ARG 3 to scalar.");
        }
        /* */
        method = int(*mxGetPr(prhs[ii]));
    }

    /* optional fourth argument */
    verbose = 0;
    if (nrhs > 3) {
        ii = 3;
        if (mxGetClassID(prhs[ii])!=mxDOUBLE_CLASS) {
            mexErrMsgTxt("Could not convert ARG 3 to double.");
        }
        if (mxGetNumberOfElements(prhs[ii])!=1) {
            mexErrMsgTxt("Could not convert ARG 3 to scalar.");
        }
        /* */
        verbose = int(*mxGetPr(prhs[ii]));
    }


    /*****************************************************************/
    /* copy matlab memory into STL vectors */
    /*****************************************************************/
    deg_bdd_vec.clear();
    deg_bdd_vec.reserve(2*num_node);
    for (k=0; k<num_node; k++)
    {
        /* N.B. convert from column-major to row-major ordering */
        deg_bdd_vec.push_back(deg_bdd_ptr[k+0*num_node]);
        deg_bdd_vec.push_back(deg_bdd_ptr[k+1*num_node]);
    }
    ijw_vec.clear();
    ijw_vec.reserve(3*num_ijw);
    for (k=0; k<num_ijw; k++)
    {
        /* N.B. convert from column-major to row-major ordering */
        ijw_vec.push_back(ijw_ptr[k+0*num_ijw]);
        ijw_vec.push_back(ijw_ptr[k+1*num_ijw]);
        ijw_vec.push_back(ijw_ptr[k+2*num_ijw]);
    }


    /*****************************************************************/
    /* bmatch_ijw interface */
    /*****************************************************************/
    success = 1;
    switch (method) {
        case 1: 
            ALG = new ComplementaryGoblinExactBmatch();
            break;
        case 2: 
            ALG = new GoblinExactBmatch();
            break;
        case 3: 
            ALG = new GreedyApproxBmatch();
            break;
        case 4: 
            ALG = new RecursiveGreedyApproxBmatch();
            break;
        case 5: 
            ALG = new BeliefPropBmatch();
            /* use full matrix */
            ijw_add_upper_tri_transpose(num_ijw, ijw_vec, 1);
            break;
        default:
            cerr << "Method not recognized" << endl;
            method_str = string("Method not recognized");
            success = 0;
            break;
    }
    /* */
    /* */
    if (success)
    {
        method_str = ALG->description();
        success = ALG->solve_bmatching_problem(num_node, deg_bdd_vec, num_ijw, ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);
        delete ALG;
    }
    /* */
    /* */
    if (method==5) /* special case */
    {
        bmatch_wgt = 0.5*bmatch_wgt;
    }
    /* */
    /*****************************************************************/


    /*****************************************************************/
    /* copy STL vectors into matlab memory */
    /*****************************************************************/

    /* could cause trouble if 0 x 3 */
    plhs[0] = mxCreateDoubleMatrix(num_bmatch_edge, 3, mxREAL);

    /* copy into matlab memory */
    if (num_bmatch_edge>0) 
    {
        bmatch_edge_ptr = mxGetPr(plhs[0]);
        for (k=0; k<num_bmatch_edge; k++)
        {
            /* N.B. convert from row-major to column-major ordering */
            /* N.B. convert from _vec to _ptr */
            bmatch_edge_ptr[k+0*num_bmatch_edge] = bmatch_edge_vec[3*k+0];
            bmatch_edge_ptr[k+1*num_bmatch_edge] = bmatch_edge_vec[3*k+1];
            bmatch_edge_ptr[k+2*num_bmatch_edge] = bmatch_edge_vec[3*k+2];
        }
    }
    /* return weight */
    if (nlhs > 1) {
        plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
        *(mxGetPr(plhs[1])) = bmatch_wgt;
    }
    /* return success value */
    if (nlhs > 2) {
        plhs[2] = mxCreateDoubleMatrix(1, 1, mxREAL);
        *(mxGetPr(plhs[2])) = success;
    }

    /* */
    time2 = time(&time2);
    total_time = difftime(time2, time1);

    /* */
    if (verbose>0)
    {
        cerr << " bmatch done:" << endl;
        cerr << "       method = " << method_str << endl;
        cerr << "   in # nodes = " << num_node << endl;
        cerr << "   in # edges = " << num_ijw << endl;
        cerr << "  out # edges = " << num_bmatch_edge << endl;
        cerr << "          wgt = " << bmatch_wgt << endl;
        cerr << "  time (sec.) = " << total_time << endl;
    }
}
/* end of mexFunction */


