
#include "BmatchSolver.hpp"


/* destructor */
BmatchSolver::
~BmatchSolver()
{
    //cerr << "BmatchSolver destructor> " << endl;
}

/* a single interface that does everything returns success (1) or failure (0) */
int BmatchSolver::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    register int i;
    register int j;
    register int k;

    int     success = 0;

    /* local (non-const) copies */
    int         L_num_node = in_num_node;
    int         L_num_ijw = in_num_ijw;
    DoubleVec   L_ijw_vec = in_ijw_vec;
    DoubleVec   L_deg_bdd_vec = in_deg_bdd_vec;
    DoubleVec   L_deg_vec(L_num_node, 0);     /* degrees of input graph */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec WWmat;
    SparseVecIterator sp;

    /* */
    num_bmatch_edge = 0;
    bmatch_edge_vec.clear();

    /* check for positive wgts */
    for (k=0; k<L_num_ijw; k++) {
        if (L_ijw_vec[3*k + 2]<0) {
            cerr << " bmatch_ijw!> weights must be positive " << endl;
            return 0;
        }
    }

    /* remove edges that are incident to nodes with upper bound at zero */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, AAmat);
    for (i=0; i<L_num_node; i++)
    {
        if (L_deg_bdd_vec[2*i+1]==0)
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
        L_deg_vec[i] += 1;
        L_deg_vec[j] += 1;
    }

    /* ensure validity of bounds */
    for (i=0; i<L_num_node; i++) 
    { 
        L_deg_bdd_vec[2*i+0] = max(0.0, min(L_deg_bdd_vec[2*i+0],L_deg_vec[i]));
        L_deg_bdd_vec[2*i+1] = max(0.0, min(L_deg_bdd_vec[2*i+1],L_deg_vec[i]));
        if (L_deg_bdd_vec[2*i+0] > L_deg_bdd_vec[2*i+1])
        {
            cerr << " BmatchSolver!> LB > UB" << endl;
            /* local cleanup */
            delete_svecvec(AAmat);
            return 0;
        }
    }

    /* debug */
    if (verbose>2)
    {
        cerr << "ijw_vec:" << endl;
        write_mn_matrix(cerr, L_num_ijw, 3, L_ijw_vec, 4);
        cerr << "deg_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 1, L_deg_vec, 4);
        cerr << "deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, L_num_node, 2, L_deg_bdd_vec, 4);
    }


    /*****************************************************************/
    /*****************************************************************/

    /* initialize */
    init(max(verbose-1,0), L_num_node, L_deg_bdd_vec, L_num_ijw, L_ijw_vec);

    /* solve problem */
    num_bmatch_edge = solve();
    if (verbose>1) 
    {
        cerr << endl;
    }
    if (num_bmatch_edge<0)
    {
        success = 0;
        num_bmatch_edge = 0;
        cerr << " bmatch_ijw warning!> Perfect matching did not exist" << endl;
    }
    else
    {
        success = 1;
    }
    if (verbose>0) 
    {
        cerr << " bmatch_ijw> returning ";
        cerr << num_bmatch_edge << " edges" << endl;
    }

    /* copy solution */
    solution(num_bmatch_edge, bmatch_edge_vec);

    /*****************************************************************/
    /*****************************************************************/

    /* ijw_upper_tri_nnz */
    ijw_upper_tri(num_bmatch_edge,bmatch_edge_vec,0);
    ijw_nnz(num_bmatch_edge,bmatch_edge_vec);

    /* create WWmat */
    ijw_2_svecvec_symm(L_num_ijw, L_ijw_vec, L_num_node, WWmat);

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

