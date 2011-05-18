/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 * */
#include <iostream>
#include <cmath>
#include <climits>
using namespace std;
#include "goblinexact.hpp"


void myLogEventHandler(msgType msg,TModule mod,THandle OH,char *logText)
{
    goblinDefaultContext.PlainLogEventHandler(msg,mod,OH,logText);
}

GoblinExactBmatch::
~GoblinExactBmatch()
{
    // cerr << "GoblinExactBmatch destructor> " << endl;
    num_node = 0;
    num_ijw = 0;
    if (gptr)
    {
        delete gptr;
    }
    /*
    if (LB_TCap_vec)
        delete[] LB_TCap_vec; 
    if (UB_TCap_vec)
        delete[] UB_TCap_vec;
    */
    /* seg. fault, b/c gra2bal object does this */
}


/* print memory */
void GoblinExactBmatch::
print(const int & print_what)
{
    if (verbose==0) 
    {
        return;
    }
    /* nodes */
    if (print_what==1) 
    {
        cerr << " Nodes:" << endl;
        TNode v;
        for (v=0; v<gptr->N(); v++ ) 
        {
            cerr << " node=" << v;
            cerr << " deg=" << gptr->Deg(v);
            cerr << " indeg=" << gptr->DegIn(v);
            cerr << " outdeg=" << gptr->DegOut(v);
            cerr << " demand=" << gptr->Demand(v);
            cerr << endl;
        }
    }
    /* arcs */
    if (print_what==2) 
    {
        cerr << " Edges:" << endl;
        TArc a;
        for (a=0; a<gptr->M(); a++ ) 
        {
            cerr << " edge=" << 2*a;
            cerr << " u=" << gptr->StartNode(2*a);
            cerr << " v=" << gptr->EndNode(2*a);
            cerr << " lcap=" << gptr->LCap(2*a);
            cerr << " ucap=" << gptr->UCap(2*a);
            cerr << " length=" << gptr->Length(2*a);
            cerr << endl;
        }
    }
    /* cout << nodeI << " " << nodeJ << " " << int(cost) << endl; */
}
/* end of print */

/* initialize assuming 0-based indices */
void GoblinExactBmatch::
init(const int & in_verbose, const int& in_num_node, const DoubleVec & deg_bdd_vec, const int & in_num_ijw, const DoubleVec & ijw_vec)
{
    /* */
    register int i;
    /* */
    num_node = in_num_node;

    /* create goblin arrays of lower/upper capacities for b-matching */
    LB_TCap_vec = new TCap[num_node]; // +1
    UB_TCap_vec = new TCap[num_node]; // +1

    if (1) 
    {
        /* initialize N node sparse graph */
        gptr = new graph((TNode)num_node);
    }
    else
    {
        /* initialize N node dense graph */
        /* 
         * Goblin has fast heuristic methods for optimizing over 
         * dense graph data structures, however, the behaviours of 
         * edge capacities and lengths are not exactly the same.
         * */
        gptr = new denseGraph((TNode)num_node);
    }
    /* gptr = new graph((TNode)0, (TOption)0); */


    /* */
    verbose = min(in_verbose,1);
    num_ijw = in_num_ijw;


    /* */
    goblinController &CT = goblinDefaultContext;
    CT.traceLevel = min(verbose,1); 
    /* larger traceLevel uses Tcl/Tk ... equivalently, one can
     * disable Tcl/Tk by UN-defining _TRACING_ in goblin/include/config.h
     * */

    /* */
    if (CT.traceLevel>0) 
    {
        //CT.logStream       = new ofstream("/dev/null");
        CT.logEventHandler = &myLogEventHandler;
        CT.Trace(gptr->Handle(),CT.traceLevel);
    }
    else
    {
        CT.logEventHandler = NULL;
    }


    /* initialize capacities */
    for (i=0; i<num_node; i++) 
    {
        LB_TCap_vec[i] = int(deg_bdd_vec[2*i+0]); 
        UB_TCap_vec[i] = int(deg_bdd_vec[2*i+1]);
    }

    /* */
    TNode u;
    TNode v;
    float cost;
    /* connect goblin nodes */
    for (i=0; i<num_ijw; i++) 
    {
        u = TNode(int(ijw_vec[3*i+0]));
        v = TNode(int(ijw_vec[3*i+1]));
        cost = float(floor(ijw_vec[3*i+2]));
        gptr->InsertArc(u, v, 1, int(cost));
        /* InsertArc(node, node, capacity, cost) */
    }

    /* print */
    print(1); /* nodes */
    print(2); /* edges */
}
/* end of init */

/* solve */
int GoblinExactBmatch::
solve()
{
    int num_bmatch_edge;

    /* Solve problem */
    //if (!gptr->MinCMatching(LB_TCap_vec)) 
    if (!gptr->MinCMatching(LB_TCap_vec, UB_TCap_vec)) 
    {
        return -1;
    }

    /* Count number of edges */
    num_bmatch_edge = 0;
    for (TArc a = 0; a < gptr->M(); a++) 
    {
        if (gptr->Sub(2*a)>0)
        {
            num_bmatch_edge += 1;
        }
    }
    return num_bmatch_edge;
}
/* end of solve */
    
/* copy b-matching approx solution
 *  assuming  0-based indices */
void GoblinExactBmatch::
solution(const int & num_bmatch_edge, DoubleVec & bmatch_edge_vec)
{
    /* copy bmatching edges */ 
    bmatch_edge_vec.clear();
    bmatch_edge_vec.reserve(3*num_bmatch_edge);
    for (TArc a = 0; a < gptr->M(); a++) 
    {
        /* 2*a indexing b/c graph includes both fwd/bkwd arcs */
        if (gptr->Sub(2*a)>0)
        {
            bmatch_edge_vec.push_back(gptr->StartNode(2*a));
            bmatch_edge_vec.push_back(gptr->EndNode(2*a));
            bmatch_edge_vec.push_back(1.0);
        }
    }
}
/* end of solution */


/* scale_weight3 
 * Edit by blake 
 * This function now scales the weights so that their sum is specified by max1.
 * The min is not shifted.
 * */
void GoblinExactBmatch::
scale_weight3(const int & num_ijw, DoubleVec & ijw_vec, const double & max1)
{
    register int k;
    register double  sum0;

    /* compute sum */
    sum0 = 0.0;
    for (k=0; k<num_ijw; k++) 
    {
        sum0 = sum0 + ijw_vec[3*k+2];
    }

    for (k=0; k<num_ijw; k++) 
    {
        ijw_vec[3*k+2] = (ijw_vec[3*k+2] / sum0) * max1;
    }
}
/* end scale_weight3 */


/* scale_weight2 */
void GoblinExactBmatch::
scale_weight2(const int & num_ijw, DoubleVec & ijw_vec, const double & ff)
{
    register int k;
    for (k=0; k<num_ijw; k++) 
    {
        ijw_vec[3*k+2] = ijw_vec[3*k+2] * ff;
    }
}
/* end scale_weight2 */


/* scale_weight */
void GoblinExactBmatch::
scale_weight(const int & num_ijw, DoubleVec & ijw_vec, const double & min1, const double & max1)
{
    register int k;
    register double  min0;
    register double  max0;
    register double val;

    /* compute min and max */
    min0 = ijw_vec[3*0 + 2];
    max0 = ijw_vec[3*0 + 2];
    for (k=0; k<num_ijw; k++) 
    {
        min0 = min(min0, ijw_vec[3*k+2]);
        max0 = max(max0, ijw_vec[3*k+2]);
    }

    for (k=0; k<num_ijw; k++) 
    {
        val = ijw_vec[3*k+2] - min0;
        val = val * (max1 - min1) / (max0 - min0 + 1.0);
        ijw_vec[3*k+2] = val + min1;
    }
}
/* end scale_weight */





/* solve_bmatching_problem 
 *
 *  MIN COST formulation
 *
 * - removes edges that are incident to nodes with upper bound at zero
 * - ensures validity of degree bounds, first checking
 *   1)  0 <= LB <= ith_degree(graph), and then
 *   2) LB <= UB <= ith_degree(graph)
 *   (i.e. if LB>UB, then UB is set to LB, which is different from maxwgt)
 * */
int GoblinExactBmatch::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    register int i;
    register int j;
    register int k;
    int success;

    /* local (non-const) copies */
    int L_num_node = in_num_node;
    int L_num_ijw = in_num_ijw;
    DoubleVec L_ijw_vec = in_ijw_vec;
    DoubleVec L_deg_bdd_vec = in_deg_bdd_vec;
    DoubleVec L_in_deg_bdd_vec = in_deg_bdd_vec;

    /* degrees */
    DoubleVec deg_vec(L_num_node, 0);     /* of input graph */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec WWmat;
    SparseVecIterator sp;

    /* remove edges that are incident to nodes with upper bound at zero */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, AAmat);
    for (i=0; i<L_num_node; i++)
    {
        if (in_deg_bdd_vec[2*i + 1]<=0)
        {
            AAmat[i]->clear();
            for (j=0; j<L_num_node; j++) 
            {
                AAmat[j]->erase(i);
            }
        }
    }
    /* extract updated upper triangle ijw */
    svecvec_2_ijw(L_num_node, AAmat, L_num_ijw, L_ijw_vec);
    ijw_upper_tri(L_num_ijw, L_ijw_vec, 0);
    
    /* compute degree (self-loops=2)*/
    for (k=0; k<L_num_ijw; k++) {
        i = int(L_ijw_vec[3*k+0]);
        j = int(L_ijw_vec[3*k+1]);
        deg_vec[i] += 1;
        deg_vec[j] += 1;
    }
    /* ensure validity of bounds */
    for (i=0; i<L_num_node; i++) 
    {
        L_in_deg_bdd_vec[2*i + 0] = max(0.0, min(L_in_deg_bdd_vec[2*i + 0], deg_vec[i]));
        L_in_deg_bdd_vec[2*i + 1] = max(L_in_deg_bdd_vec[2*i + 0], min(L_in_deg_bdd_vec[2*i + 1], deg_vec[i]));
    }


    /* scale weigths for (integral) resolution with goblin
     * 
     * 1) this is efficient, because we are operating on the sparse ijw_vec 
     * representation
     *
     * 2) we only perform multiplicatioin by GOBLIN_COST_SCALE, and explicitly 
     * avoid translations of the weights, because translations can affect the
     * optimality of the solution
     *
     * 3) selecting an appropriate scale factor GOBLIN_COST_SCALE is tricky;
     * it should be large enough so that all distinct weights are mapped to 
     * distinct integer values, but small enough so that the maximum weight 
     * does not exceed the float capacity
     *
     * One solution is to use the maximum possible scaling as determined 
     * by the precision of the weights.
     *
	 * ... 2^31 - 1 = 2147483647
	 * ... 2^30 - 1 = 1073741823
     *
     * DEPRECATED: scale_weight(L_num_ijw, L_ijw_vec, 1.0, GOBLIN_COST_SCALE);
     * OLD: scale_weight2(L_num_ijw, L_ijw_vec, GOBLIN_COST_SCALE);
	 * OLD: scale_weight3(L_num_ijw, L_ijw_vec, 1073741823);
     * */
	scale_weight3(L_num_ijw, L_ijw_vec, INT_MAX);


    /* */
    success = BmatchSolver::solve_bmatching_problem(L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);


    /* create WWmat */
    ijw_2_svecvec_symm(in_num_ijw, in_ijw_vec, in_num_node, WWmat);

    /* compute weight */
    bmatch_wgt = 0.0;
    for (k=0; k<num_bmatch_edge; k++) 
    {
        i = int(bmatch_edge_vec[3*k+0]);
        j = int(bmatch_edge_vec[3*k+1]);
        bmatch_wgt += (*WWmat[i])[j];
    }

    /* local cleanup */
    delete_svecvec(AAmat);
    delete_svecvec(WWmat);

    return success;
}
/* end solve_bmatching_problem */







/* destructor */
ComplementaryGoblinExactBmatch::
~ComplementaryGoblinExactBmatch()
{
    //cerr << "ComplementaryGoblinExactBmatch destructor> " << endl;
}


/* solve_bmatching_problem
 *
 *  MAX WGT formulation
 *
 * - removes edges that are incident to nodes with upper bound at zero
 * - ensures validity of degree bounds, first checking
 *   1) 0 <= UB <= ith_degree(graph), and then
 *   2) 0 <= LB <= UB
 *   (i.e. if LB>UB, then LB is set to UB, which is different from mincost)
 * */
int ComplementaryGoblinExactBmatch::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    register int i;
    register int j;
    register int k;

    int success;

    int     L_num_node = in_num_node;
    int     L_num_ijw = in_num_ijw;
    DoubleVec L_ijw_vec = in_ijw_vec;
    DoubleVec L_deg_bdd_vec = in_deg_bdd_vec;
    /* NEW */
    DoubleVec L_in_deg_bdd_vec = in_deg_bdd_vec;
    /* NEW */

    /* degrees */
    DoubleVec deg_vec(L_num_node, 0);     /* of input graph */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec BBmat;
    SparseVecVec WWmat;
    SparseVecIterator sp;

    /* remove edges that are incident to nodes with upper bound at zero */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, AAmat);
    for (i=0; i<L_num_node; i++)
    {
        if (L_in_deg_bdd_vec[2*i + 1]<=0)
        {
            AAmat[i]->clear();
            for (j=0; j<L_num_node; j++) 
            {
                AAmat[j]->erase(i);
            }
        }
    }
    /* extract updated upper triangle ijw */
    svecvec_2_ijw(L_num_node, AAmat, L_num_ijw, L_ijw_vec);
    ijw_upper_tri(L_num_ijw, L_ijw_vec, 0);
    
    /* make AAmat logical */
    logical_svecvec(AAmat);

    /* compute degree (self-loops=2)*/
    for (k=0; k<L_num_ijw; k++) {
        i = int(L_ijw_vec[3*k+0]);
        j = int(L_ijw_vec[3*k+1]);
        deg_vec[i] += 1;
        deg_vec[j] += 1;
    }
    /* ensure validity of bounds */
    for (i=0; i<L_num_node; i++) 
    {
        L_in_deg_bdd_vec[2*i + 1] = max(0.0, min(L_in_deg_bdd_vec[2*i + 1], deg_vec[i]));
        L_in_deg_bdd_vec[2*i + 0] = max(0.0, min(L_in_deg_bdd_vec[2*i + 0], L_in_deg_bdd_vec[2*i + 1]));
    }
    /* get complementary degree bounds */
    for (i=0; i<L_num_node; i++)
    {
        L_deg_bdd_vec[2*i + 0] = deg_vec[i] - L_in_deg_bdd_vec[2*i + 1];
        L_deg_bdd_vec[2*i + 1] = deg_vec[i] - L_in_deg_bdd_vec[2*i + 0];
    }

    /* debug */
    if (verbose>2)
    {
        cerr << "ijw_vec:" << endl;
        write_mn_matrix(cerr, L_num_ijw, 3, L_ijw_vec, 4);
        cerr << "deg_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 1, deg_vec, 4);
        cerr << "maxwgt_deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 2, L_in_deg_bdd_vec, 4);
        cerr << "mincost_deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 2, L_deg_bdd_vec, 4);
    }


    /* */
    success = GoblinExactBmatch::solve_bmatching_problem(L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);


    /* matching: sparse symmetric adjacency */
    ijw_2_svecvec_symm(num_bmatch_edge, bmatch_edge_vec, L_num_node,BBmat);
    logical_svecvec(BBmat);

    /* take complement of solution matching A - B */
    for (i=0; i<L_num_node; i++) 
    {
        for (sp=BBmat[i]->begin(); sp!=BBmat[i]->end(); sp++) 
        {
            j = sp->first;
            AAmat[i]->erase(j);
        }
    }
    /* extract updated ijw */
    svecvec_2_ijw(L_num_node,AAmat,num_bmatch_edge,bmatch_edge_vec);
    ijw_upper_tri(num_bmatch_edge,bmatch_edge_vec,0);

    /* create WWmat */
    ijw_2_svecvec_symm(in_num_ijw, in_ijw_vec, in_num_node, WWmat);

    /* compute weight and degree (self-loops=2) */
    bmatch_wgt = 0.0;
    for (k=0; k<num_bmatch_edge; k++) 
    {
        i = int(bmatch_edge_vec[3*k+0]);
        j = int(bmatch_edge_vec[3*k+1]);
        bmatch_wgt += (*WWmat[i])[j];
    }


    /* local cleanup */
    delete_svecvec(AAmat);
    delete_svecvec(BBmat);
    delete_svecvec(WWmat);


    return success;
}
/* end solve_bmatching_problem */







/* destructor */
NegatedWeightsGoblinExactBmatch::
~NegatedWeightsGoblinExactBmatch()
{
    //cerr << "NegatedWeightsGoblinExactBmatch destructor> " << endl;
}



/* solve_bmatching_problem */
int NegatedWeightsGoblinExactBmatch::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    register int i;
    register int j;
    register int k;

    int success;

    int L_num_node = in_num_node;
    int L_num_ijw = in_num_ijw;
    DoubleVec L_ijw_vec = in_ijw_vec;
    DoubleVec L_deg_bdd_vec = in_deg_bdd_vec;

    /* degrees */
    DoubleVec deg_vec(L_num_node, 0);     /* of input graph */
    DoubleVec bval_vec(L_num_node, 0);    /* upper bounds */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec WWmat;

    /* working copy of upper degree bounds */
    for (i=0; i<L_num_node; i++) 
    {
        bval_vec[i] = L_deg_bdd_vec[2*i + 1];
    }
    /* remove edges that are incident to nodes with upper bound at zero */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, AAmat);

    for (i=0; i<L_num_node; i++)
    {
        if (bval_vec[i]==0)
        {
            AAmat[i]->clear();
            for (j=0; j<L_num_node; j++) 
            {
                AAmat[j]->erase(i);
            }
        }
    }
    /* extract updated upper triangle ijw */
    svecvec_2_ijw(L_num_node, AAmat, L_num_ijw, L_ijw_vec);
    ijw_upper_tri(L_num_ijw,L_ijw_vec,0);
    
    /* compute degree (self-loops=2)*/
    for (k=0; k<L_num_ijw; k++) {
        i = int(L_ijw_vec[3*k+0]);
        j = int(L_ijw_vec[3*k+1]);
        deg_vec[i] += 1;
        deg_vec[j] += 1;
    }
    /* ensure validity of bounds */
    for (i=0; i<L_num_node; i++) 
    {
        bval_vec[i] = max(0.0, min(bval_vec[i], deg_vec[i]));
    }
    /* setup lower bound using previous upper bounds */
    for (i=0; i<L_num_node; i++)
    {
        L_deg_bdd_vec[2*i + 0] = bval_vec[i];
        L_deg_bdd_vec[2*i + 1] = deg_vec[i];
    }
    /* negate weights */
    scale_weight2(L_num_ijw, L_ijw_vec, -1.0);
    /* shift positive again */
    scale_weight(L_num_ijw, L_ijw_vec, 1e-32, 1.0);
    /* let GoblinExactBmatch solver handle GOBLIN_COST_SCALE scaling */


    /* debug */
    if (verbose>2)
    {
        cerr << "ijw_vec:" << endl;
        write_mn_matrix(cerr, L_num_ijw, 3, L_ijw_vec, 4);
        cerr << "deg_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 1, deg_vec, 4);
        cerr << "bval_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 1, bval_vec, 4);
        cerr << "deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 2, L_deg_bdd_vec, 4);
    }

    /* */
    success =  GoblinExactBmatch::solve_bmatching_problem(L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec, num_bmatch_edge, bmatch_edge_vec, bmatch_wgt, verbose);

    /* create WWmat */
    ijw_2_svecvec_symm(in_num_ijw, in_ijw_vec, in_num_node, WWmat);

    /* compute weight and degree (self-loops=2) */
    bmatch_wgt = 0.0;
    for (k=0; k<num_bmatch_edge; k++) 
    {
        i = int(bmatch_edge_vec[3*k+0]);
        j = int(bmatch_edge_vec[3*k+1]);
        bmatch_wgt += (*WWmat[i])[j];
    }

    /* local cleanup */
    delete_svecvec(AAmat);
    delete_svecvec(WWmat);

    return success;
}
/* end solve_bmatching_problem */







