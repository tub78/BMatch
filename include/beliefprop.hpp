/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 * */
#ifndef _BELIEFPROP_SOLVER_H_
#define _BELIEFPROP_SOLVER_H_

#include "method_names.hpp"
#include "Vector.hpp"
#include "AbstractBmatchSolver.hpp"

/* */
/* ****************** expecting full matrix ******************** */
/* ****************** degree of self-loops=1 ******************** */
/* */
class BeliefPropBmatch : public AbstractBmatchSolver
{
    public :
        /* constructor */
        BeliefPropBmatch() 
        {
            description_str = string(BELIEFPROPBMATCH);
        }
        /* destructor */
        ~BeliefPropBmatch();

        /* solve_bmatching_problem */
        int solve_bmatching_problem(const int & in_num_node, const DoubleVec & in_deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);

};

/*********************************************************************/
/* C implementation */
/*********************************************************************/
extern "C" 
{

/* node_t */
typedef struct node_t_struct {
  int id;
  int neighborhood; //number of neighbors (not counting itself)
  struct node_t_struct **neighbors; //vector of pointers to neighbors
  double *message; //vector of messages from this node to all other nodes
  double *phi;   //potential
  int *beliefs; //current top b node ids
} node_t;


/* initNode */
node_t * initNode(int n, int id, int b, int total);

/* freeNode */
void freeNode(node_t *node);

/* updates the messages stored in the given node 
 * by using messages from neighbors in opposite */
void updateMessages(node_t *node, int b);

/* checks if there is a valid bmatching in the beliefs */
int permCheck(node_t **nodes, int**P, int n, int b);

/* solve */
int solve(int num_input_edge, double * input_edge_ptr, int num_input_node, int node_cap, double * output_edge_ptr, int verbose);


} 
/*********************************************************************/
/* end extern "C" */
/*********************************************************************/

#endif

