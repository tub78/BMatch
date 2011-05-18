/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 *
 * Belief Propagation Algorithm
 * Bert Huang (2006) 
 * Send comments and questions to bert at cs.columbia.edu
 * */



#include <cmath>
#include "beliefprop.hpp"

/* destructor */
BeliefPropBmatch::
~BeliefPropBmatch()
{
}


/* solve_bmatching_problem */
int BeliefPropBmatch::
solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    /* */
    /* ****************** expecting full matrix ******************** */
    /* ****************** degree of self-loops=1 ******************* */
    /* ****************** expecting b<n **************************** */
    /* */

    register int i;
    register int j;
    register int k;

    int     success = 0;
    int     node_cap = in_num_node;

    /* local (non-const) copies */
    DoubleVec   L_ijw_vec = in_ijw_vec;     /* degrees of input graph */
    DoubleVec   deg_vec(in_num_node, 0);     /* degrees of input graph */

    /* adjacency and weights */
    SparseVecVec AAmat;
    SparseVecVec WWmat;
    SparseVecIterator sp;

    /* check for positive wgts */
    for (k=0; k<in_num_ijw; k++) {
        if (L_ijw_vec[3*k + 2]<0) {
            cerr << " bmatch_ijw!> weights must be positive " << endl;
            return 0;
        }
    }

    /* compute degree */
    /* ****************** (self-loops=1)******************** */
    for (k=0; k<in_num_ijw; k++) {
        i = int(L_ijw_vec[3*k+0]);
        deg_vec[i] += 1;
        //j = L_ijw_vec[3*k+1];
        //deg_vec[j] += 1;
    }

    /* assuming constant degree */
    for (i=0; i<in_num_node; i++) 
    {
        node_cap=int(max(0.0,min(double(node_cap),in_deg_bdd_vec[2*i+1])));
        node_cap=int(max(0.0,min(double(node_cap),deg_vec[i])));
    }

    /* debug */
    if (verbose>2)
    {
        cerr << "ijw_vec:" << endl;
        write_mn_matrix(cerr, in_num_ijw, 3, L_ijw_vec, 4);
        cerr << "deg_vec:" << endl;
        write_mn_matrix(cerr, in_num_node, 1, deg_vec, 4);
        cerr << "deg_bdd_vec:" << endl;
        write_mn_matrix(cerr, in_num_node, 2, in_deg_bdd_vec, 4);
    }




    /********************************************************/
    /* call C code */
    /* */
    num_bmatch_edge = node_cap*in_num_node;
    bmatch_edge_vec.clear();
    bmatch_edge_vec.resize(3*num_bmatch_edge, 0.0);
    /* */
    solve(in_num_ijw, &L_ijw_vec[0], in_num_node, node_cap, &bmatch_edge_vec[0], verbose);
    /* */
    /********************************************************/

    /* create WWmat */
    ijw_2_svecvec(in_num_ijw, in_ijw_vec, in_num_node, WWmat);


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



  

/*********************************************************************/
/* C code */
/*********************************************************************/
extern "C" 
{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NEG_INF -9999
#define MAX_ITER 10000
#define CONVERGENCE_INSURANCE 20

/* node_t */
node_t * initNode(int n, int id, int b, int total)
{
    int i;
    node_t *node;
    node = (node_t *)malloc(sizeof(node_t));
    node->id = id;
    node->neighborhood = n;
    node->message = (double*)malloc(total*sizeof(double));
    node->phi = (double*)malloc(n*sizeof(double));
    node->neighbors = (node_t**)malloc(n*sizeof(node_t*));

    for (i=0; i<total; i++) {
        node->message[i]=1;		
    }
    for (i=0; i<n; i++)
        node->phi[i]=1;

    node->beliefs = (int*) malloc(b*sizeof(int));
    for (i=0; i<b; i++) {
        node->beliefs[i] = i;
    }

    return node;
}

/* freeNode */
void freeNode(node_t *node)
{
    free(node->message);
    free(node->beliefs);
    free(node->phi);
    free(node->neighbors);
}

/* updates the messages stored in the given node 
 * by using messages from neighbors in opposite */
void updateMessages(node_t *node, int b)
{
    double *workvec;
    int *topvals, n=node->neighborhood, i ,j, minind, bth, counter;

    workvec = (double*)malloc(n*sizeof(double));
    topvals = (int*)malloc((b+1)*sizeof(int));

    //compute work vector
    for (i=0; i<n; i++) {
        workvec[i] = node->phi[i]*node->neighbors[i]->message[node->id];
    }

    //initialize topvals to be the first b+1 indexes
    minind=0; bth=0;
    for (i=0; i<b+1; i++) {
        topvals[i]=i;
        if (workvec[topvals[i]]<workvec[topvals[minind]])
            minind=i;
    }

    //find true topvals and true minind
    for (i=b+1; i<n; i++) {
        if (workvec[i]>workvec[topvals[minind]]) {
            topvals[minind] = i;
            for (j=0; j<b+1; j++) {
                if (workvec[topvals[j]]<workvec[topvals[minind]])
                    minind=j;
            }
        }
    }

    //find bth entry
    if (minind==0)
        bth=1;
    for (i=0; i<b+1; i++) {
        if (i!=minind && workvec[topvals[i]]<workvec[topvals[bth]])
            bth = i;
    }

    //update messages
    for (i=0; i<n; i++) {
        if (workvec[i]>=workvec[topvals[bth]])
            node->message[node->neighbors[i]->id]=node->phi[i]/workvec[topvals[minind]];
        else
            node->message[node->neighbors[i]->id]=node->phi[i]/workvec[topvals[bth]];
    }

    //update beliefs
    counter=0;
    for (i=0; i<b+1; i++) {
        if (i!=minind) {
            node->beliefs[counter++] = node->neighbors[topvals[i]]->id;
        }
    }

    free(workvec);
    free(topvals);
}


/* checks if there is a valid bmatching in the beliefs */
int permCheck(node_t **nodes, int**P, int n, int b) 
{
    int ret=1, i, j, *counters;

    counters = (int*)malloc(n*sizeof(int));

    for (i=0; i<n; i++)
        counters[i]=0;

    for (i=0; i<n; i++) {
        for (j=0; j<b; j++) {
            P[i][j]=nodes[i]->beliefs[j];
            counters[nodes[i]->beliefs[j]]++;
        }
    }

    for (i=0; i<n; i++) 
        if (counters[i]!=b)
            ret=0;

    free(counters);
    return ret;
}






/*
 * main() from bpbmatch.c
 * */
int solve(int num_input_edge, double * input_edge_ptr, int num_input_node, int node_cap, double * output_edge_ptr, int verbose) 
{
    int nnz;
    int num_output_edge;
    int ii;
    int nodeI;
    int nodeJ;
    double di;
    double dj;
    double wgt;

    /*********************************************************************/
    /*********************************************************************/
    /* cerr << "Usage: ./cbpbm <input file> <size> <b> <output file>" << endl;*/
    /* assume n objects, nxn matrix */
    double **W; //fsum
    int i,j,n, sum, counter, converged=CONVERGENCE_INSURANCE, iters=0, links=0, b, lcount;
    int **P;
    node_t **alpha, **beta;


    srand(time(NULL));
    n = num_input_node;
    b = node_cap;

    W = (double**)malloc(n*sizeof(double*));
    P = (int**)malloc(n*sizeof(int*));
    for (i=0; i<n; i++) {
        P[i] = (int*)malloc(b*sizeof(int));
        W[i] = (double*)malloc(n*sizeof(double));
    }
    /*********************************************************************/
    /*********************************************************************/




    /* readMatrix(W,argv[1], n); */
    /*      insert edge weights */
    for (ii = 0; ii < num_input_edge; ii++) {
        di = input_edge_ptr[3*ii + 0];
        dj = input_edge_ptr[3*ii + 1];
        wgt = input_edge_ptr[3*ii + 2];

        nodeI = ((int)di);
        nodeJ = ((int)dj);
        W[nodeI][nodeJ] = wgt;
    }




    /*********************************************************************/
    /*********************************************************************/
    /* allocate and initialize */

    alpha = (node_t**)malloc(n*sizeof(node_t*));
    beta = (node_t**)malloc(n*sizeof(node_t*));

    for (i=0; i<n; i++) { //initialize nodes
        sum=0;
        for (j=0; j<n; j++) {
            if (W[i][j]>NEG_INF) { //if negative weight, cut link
                sum++;
                lcount++;
            }
        }
        alpha[i] = initNode(sum,i,b,n);
    }

    for (i=0; i<n; i++) { //initialize nodes
        sum=0;
        for (j=0; j<n; j++) {
            if (W[j][i]>NEG_INF) { //if negative weight, cut link
                sum++;
                lcount++;
            }
        }
        beta[i] = initNode(sum,i,b,n);
    }

    //now connect the nodes

    for (i=0; i<n; i++) {
        counter = 0;
        for (j=0; j<n; j++) {
            if (W[i][j]>NEG_INF) {
                alpha[i]->neighbors[counter] = beta[j];
                alpha[i]->phi[counter] = exp(W[i][j]);
                counter++;
                links++;
            }
        }
    }
    for (i=0; i<n; i++) {
        counter = 0;
        for (j=0; j<n; j++) {
            if (W[j][i]>NEG_INF) {
                beta[i]->neighbors[counter] = alpha[j];
                beta[i]->phi[counter] = exp(W[j][i]);
                counter++;
                links++;
            }
        }
    }

    if (verbose>1)
    {
        cerr << "Graph allocated, " <<  n;
        cerr << " nodes, " << links;
        cerr << " links, " << 100*(double)links/(double)(n*n);
        cerr << " connectivity" << endl; ;
    }

    /*********************************************
      belief propagation
     *********************************************/
    while(converged>0) {

        for (i=0; i<n; i++) {
            updateMessages(beta[i],b);
        }
        for (i=0; i<n; i++) {
            updateMessages(alpha[i],b);
        }

        //cerr << "message update successful" << endl;

        //check for convergence
        converged -= permCheck(alpha,P,n,b);

        //cerr << "permcheck successful" << endl;

        if (++iters>MAX_ITER) {
            cerr << " beliefprop.solve> Reached maximum iterations without converging" << endl;
            converged=0;
        }
    }
    /*********************************************************************/
    /*********************************************************************/

    /* number of edges */
    num_output_edge = node_cap*num_input_node;

    /* writeOut(P,argv[4],n,b); */
    /*        - output answer   */
    if (converged==0) {
        /* Copy [i,j] edges */
        nnz = 0;
        for (nodeI = 0; nodeI < num_input_node; nodeI++) {
            for (ii = 0; ii < node_cap; ii++) {
                output_edge_ptr[3*nnz + 0] = (double)(nodeI);
                output_edge_ptr[3*nnz + 1] = (double)(P[nodeI][ii]);
                output_edge_ptr[3*nnz + 2] = (double)(1.0);
                nnz += 1;
            }
        }
    }
    /*
       for (ii = 0; ii < num_output_edge; ii++) {
       cerr << "(" << output_edge_ptr[ii + 0*num_output_edge];
       cerr << "," << output_edge_ptr[ii + 1*num_output_edge] << ")" << endl;
       } */


    /*********************************************************************/
    /*********************************************************************/
    //clean up
    for (i=0; i<n; i++) {
        free(W[i]);
        free(P[i]);
    }
    free(alpha);
    free(beta);
    free(P);
    free(W);
    /*********************************************************************/
    /*********************************************************************/


    if (converged==0) {
        return 0;
    } else {
        return 1;
    }
}

}
/*********************************************************************/
/* end "C" code */
/*********************************************************************/
 
