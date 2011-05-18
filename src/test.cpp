/*
 * B-matching toolbox
 * Stu Andrews
 *
 * */
#ifdef _LEAKTEST_
#include <unistd.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;
#include <cmath>
#include <time.h>
#include "Parameter.hpp"
#include "AbstractBmatchSolver.hpp"
#include "greedyapprox.hpp"
#include "goblinexact.hpp"
#include "beliefprop.hpp"



/* initialize N x 2 array with rows [ 0 , b ] */
void init_const_degrees(const int & num_node, DoubleVec & deg_bdd_vec, const double & b)
{
    register int i;
    deg_bdd_vec.clear();
    deg_bdd_vec.reserve(2*num_node);
    for (i=0; i<num_node; i++)
    {
        deg_bdd_vec.push_back(0.0);
        deg_bdd_vec.push_back(b);
    }
}
/* end of init_const_degrees */


/* read degree LB, UB */
void read_degrees(istream & in, int & num_node, DoubleVec & deg_bdd_vec)
{
    register int i;
    DoubleVec TTvec;
    int N;
    int M;
    double const_b;
    read_mn_matrix(in, M, N, deg_bdd_vec);
    /* convert scalar input into Mx2 format using repmat([0,UB],num_node,1) */
    if ((M==1)&&(N==1))
    {
        const_b = deg_bdd_vec[0];
        init_const_degrees(num_node,deg_bdd_vec,const_b);
        return;
    }
    /* convert Mx1 inputs into Mx2 format, using [zeros(num_node,1),UB_vec] */
    if ((M>=num_node)&&(N==1))
    {
        TTvec.reserve(2*M);
        for (i=0; i<M; i++)
        {
            TTvec.push_back(0.0);
            TTvec.push_back(deg_bdd_vec[i]);
        }
        deg_bdd_vec = TTvec;
        num_node = M;
        return;
    }
    /* if Mx2 */
    if ((M>=num_node)&&(N==2))
    {
        num_node = M;
        return;
    }
    /* convert 1xM into Mx2 using [zeros(num_node,1),UB_vec] */
    if ((M==1)&&(N>=num_node)) 
    {
        TTvec.reserve(2*N);
        for (i=0; i<N; i++)
        {
            TTvec.push_back(0.0);
            TTvec.push_back(deg_bdd_vec[i]);
        }
        deg_bdd_vec = TTvec;
        num_node = N;
        return;
    }
    /* convert 1x2 into Mx2 using repmat([LB,UB],num_node,1) */
    if (((M==1)&&(N==2))||((M==2)&&(N==1))) 
    {
        TTvec.reserve(2*M);
        for (i=0; i<num_node; i++)
        {
            TTvec.push_back(deg_bdd_vec[0]);
            TTvec.push_back(deg_bdd_vec[1]);
        }
        deg_bdd_vec = TTvec;
        return;
    }
    /* */
    cerr << "read_degrees!> unrecognized format" << endl;
    deg_bdd_vec.clear();
    num_node = 0;
}
/* end read_degrees */







/******************************************************************/
/* Interface for the ijw-based MEX function */
/******************************************************************/
//int main(int argc, const char *argv[])
int main(int argc, char *argv[])
{
    register int    k;

    int             verbose;
    int             success;

    int             num_node;
    DoubleVec       deg_bdd_vec;

    int             num_ijw_node;
    int             num_ijw;
    DoubleVec       ijw_vec;

    int             num_bmatch_edge = 0;
    DoubleVec       bmatch_edge_vec;
    double          bmatch_wgt;

    time_t          time1;
    time_t          time2;
    double          total_time;


    AbstractBmatchSolver * ALG;

    string method_str;


    Parameter       PP;
    ifstream        fin_strm;
    istream         * in_strm_ptr;
    ofstream        fout_strm;
    ostream         * out_strm_ptr;

    /* */
    time1 = time(&time1);


    cout.precision(4);
    cerr.precision(4);  

    /* defaults params */
    PP.init( 
        " -weights      NULL "
        " -degrees      NULL "
        " -output       NULL "
        " -b            -1   "
        " -s            0    "
        " -method       1    "
        " -verbose      0    "
    );
    /* parse cmd line */
    PP.init(argc,argv);
    verbose = PP.verbose;
    /* debugging */
    if (verbose>1)
    {
        PP.print();
    }
    bool is_weights_file = (PP.weights.find("NULL", 0) == string::npos);
    bool is_degrees_file = (PP.degrees.find("NULL", 0) == string::npos);
    bool is_output_file = (PP.output.find("NULL", 0) == string::npos);

    /* missing files */
    if (!is_weights_file) 
    {
        in_strm_ptr = &cin;
        if (PP.sparse)
        {
            cerr << "INPUT WEIGHTS(sparse)>" << endl;
        }
        else
        {
            cerr << "INPUT WEIGHTS(dense)>" << endl;
        }
    }
    else
    {
        fin_strm.open(PP.weights.c_str());
        in_strm_ptr = &fin_strm;
    }

    /* ijw data */
    if (PP.sparse==1)
    {
        /* read ijw */ 
        ijw_read_ijw(*in_strm_ptr,num_ijw,ijw_vec);
    }
    else 
    {
        /* read matrix */ 
        ijw_read_matrix(*in_strm_ptr,num_ijw,ijw_vec);
        /* keep only non-zeros from upper triangle */ 
        ijw_upper_tri(num_ijw,ijw_vec,0);
        ijw_nnz(num_ijw,ijw_vec);
    }
    if (is_weights_file) 
    {
        fin_strm.close();
    }


    /* update the maximum node number */
    num_node = 0;
    num_ijw_node = 0;
    for (k=0; k<num_ijw; k++) 
    {
        num_ijw_node = int(max(double(num_ijw_node), (1.0 + ijw_vec[3*k + 0])));
        num_ijw_node = int(max(double(num_ijw_node), (1.0 + ijw_vec[3*k + 1])));
    }
    num_node = num_ijw_node; 


    /* degrees */ 
    if (PP.const_b>=0) 
    {
        /* initialize num_node array of constant = b */ 
        init_const_degrees(num_node,deg_bdd_vec,PP.const_b);
    }
    else
    {
        if (!is_degrees_file) 
        {
            in_strm_ptr = &cin;
            cerr << "INPUT DEGREES>" << endl;
        }
        else
        {
            fin_strm.open(PP.degrees.c_str());
            in_strm_ptr = &fin_strm;
        }
        /* read  */
        read_degrees(*in_strm_ptr,num_node,deg_bdd_vec);
        if (is_degrees_file)
        {
            fin_strm.close();
        }
        /* num_node could change */ 
        if (num_node<num_ijw_node)
        {
            cerr << "bmatch!> missing nodes in degree bounds" << endl;
            exit(1);
        }
    }
    /* */
    if ((!is_weights_file) || ((!is_degrees_file)&&(PP.const_b<0)) )
    {
        cerr << "bmatch> solving problem ..." << endl;
    }

    /* debugging */ 
    if (verbose>1)
    {
        cerr << "bmatch: num_node " << num_node << endl;
        write_mn_matrix(cerr, num_node, 2, deg_bdd_vec, 4);
        if (num_node<=num_ijw) 
        {
            ijw_write_matrix(cerr, num_node, num_ijw, ijw_vec, 3);
        }
        else
        {
            write_mn_matrix(cerr, num_ijw, 3, ijw_vec, 3);
        }
    }


    /* */
    SparseVecVec Amat;
    ijw_2_svecvec_symm(num_ijw, ijw_vec, num_node, Amat);
    delete_svecvec(Amat);
    ijw_2_svecvec_symm(num_ijw, ijw_vec, num_node, Amat);
    delete_svecvec(Amat);
    

#ifdef _LEAKTEST_
    sleep(600);
#endif

    return 0;

    /*****************************************************************/
    /* bmatch_ijw interface */
    /*****************************************************************/
    switch (PP.method) {
        case 1: 
            ALG = new ComplementaryGoblinExactBmatch();
            break;
        case 2: 
            ALG = new NegatedWeightsGoblinExactBmatch();
            break;
        case 3: 
            ALG = new GreedyApproxBmatch();
            break;
        case 5: 
            ALG = new RecursiveGreedyApproxBmatch();
            break;
        case 7: 
            ALG = new BeliefPropBmatch();
            /* use full matrix */
            ijw_add_upper_tri_transpose(num_ijw, ijw_vec, 1);
            break;
        default:
            cerr << "Method not recognized" << endl;
            exit(1);
    }
    /* */
    /* */
    method_str = ALG->description();
    success = ALG->solve_bmatching_problem(num_node, deg_bdd_vec, num_ijw, ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);
    /* */
    delete ALG;
    /* */
    /* */
    if (PP.method==7) /* special case */
    {
        bmatch_wgt = 0.5*bmatch_wgt;
    }
    /* */
    /*****************************************************************/



    /*****************************************************************/
    /* output */
    /*****************************************************************/

    /* redirect */
    if (!is_output_file) 
    {
        out_strm_ptr = &cout;
        cerr << "MATCHING>" << endl;
    }
    else
    {
        fout_strm.open(PP.output.c_str());
        out_strm_ptr = &fout_strm;
    }
    if (PP.sparse==1)
    {
        ijw_write_ijw(*out_strm_ptr, num_bmatch_edge, bmatch_edge_vec, 3);
    }
    else
    {
        if (PP.method!=7) /* special case */
        {
            ijw_add_upper_tri_transpose(num_bmatch_edge, bmatch_edge_vec, 1);
        }
        /* write full symmetrix matrix in ijw format */
        ijw_write_matrix(*out_strm_ptr,num_node,num_bmatch_edge,bmatch_edge_vec,3);
        if (PP.method!=7) /* special case */
        {
            ijw_upper_tri(num_bmatch_edge, bmatch_edge_vec, 0);
        }
    }
    if (is_output_file) 
    {
        fout_strm.close();
    }

    /* */
    time2 = time(&time2);
    total_time = difftime(time2, time1);

    if (verbose>0)
    {
        cerr << "bmatch done:" << endl;
        cerr << "       method = " << method_str << endl;
        cerr << "   in # nodes = " << num_node << endl;
        cerr << "   in # edges = " << num_ijw << endl;
        cerr << "  out # edges = " << num_bmatch_edge << endl;
        cerr << "          wgt = " << bmatch_wgt << endl;
        cerr << "  time (sec.) = " << total_time << endl;
    }

#ifdef _LEAKTEST_
    sleep(600);
#endif

    return (~success);
}
/* end of main */


