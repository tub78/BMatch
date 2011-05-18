/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 * */
#ifndef _GREEDYAPPROX_SOLVER_H_
#define _GREEDYAPPROX_SOLVER_H_

#include <iostream>
using namespace std;

#include "method_names.hpp"
#include "BmatchSolver.hpp"
#include "Vector.hpp"

/******************************************************************/
/* typedefs */ 
/******************************************************************/

typedef map<int,int>                hash_map_type;
typedef hash_map_type::value_type   hash_entry_type;

typedef pair<int, int>              NodePair;
typedef vector<NodePair>            EdgeList;


 
/* */
class GreedyApproxBmatch : public BmatchSolver 
{

    public:
        GreedyApproxBmatch() 
        {
            description_str = string(GREEDYAPPROXBMATCH);
        }


        virtual ~GreedyApproxBmatch();


    private:

        /* print memory */
        virtual void print(const int & print_what);

        /* initialize assuming 1-based indices */
        virtual void init(const int & in_verbose, const int& num_node, const DoubleVec & node_capacities_vec, const int & num_ijw, const DoubleVec & ijw_vec); 

        /* solve */
        virtual int solve();

        /* copy b-matching approx solution
         *  assuming  0-based indices */
        virtual void solution(const int & num_bmatch_edge, DoubleVec & bmatch_edge_vec);


        /* print verbosity */
        int verbose;

        /* num nodes */
        int num_node;

        /* num ijw */
        int num_ijw;

        /* adjacency lists */
        SparseVecVec Amat;

        /* edge weight lists */
        SparseVecVec Wmat;

        /* vector of node capacities */
        LongIntVec bcap_vec;

        /* vector of node degrees */
        LongIntVec deg_vec;

        /* b-matchings */
        EdgeList B_match;

        /* b-matchings */
        EdgeList M_walk;

};


/* */
class RecursiveGreedyApproxBmatch : public GreedyApproxBmatch
{
    public:
        RecursiveGreedyApproxBmatch() 
        {
            description_str = string(RECURSIVEGREEDYAPPROXBMATCH);
        }

        virtual ~RecursiveGreedyApproxBmatch();

        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);

};


#endif

