/*
 * B-matching toolbox
 * Stu Andrews
 * January 2008
 * */
#ifndef _GOBLINEXACT_SOLVER_H_
#define _GOBLINEXACT_SOLVER_H_

#include "method_names.hpp"
#include "BmatchSolver.hpp"
#include "goblin.h"

#define GOBLIN_COST_SCALE (100.0)


/* */
class GoblinExactBmatch : public BmatchSolver 
{

    public:
        GoblinExactBmatch() 
        {
            description_str = string(GOBLINEXACTBMATCH);
        }

        virtual ~GoblinExactBmatch();

        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & in_num_ijw, const DoubleVec & in_ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);

        /* scale_weight */
        virtual void scale_weight(const int & num_ijw, DoubleVec & ijw_vec, const double & min1, const double & max1);


        /* scale_weight2 */
        virtual void scale_weight2(const int & num_ijw, DoubleVec & ijw_vec, const double & ff);


        /* scale_weight3 */
        virtual void scale_weight3(const int & num_ijw, DoubleVec & ijw_vec, const double & ff);



    private:
        /* print memory */
        virtual void print(const int & print_what);

        /* initialize assuming 0-based indices */
        virtual void init(const int & in_verbose, const int& num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec); 

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

        /* graph */
        //graph*  gptr;
        abstractGraph*  gptr;

        /* vectors of node lower and upper capacities */
        TCap*   LB_TCap_vec;
        TCap*   UB_TCap_vec;

};


/* */
class ComplementaryGoblinExactBmatch : public GoblinExactBmatch
{

    public:
        ComplementaryGoblinExactBmatch() 
        {
            description_str = string(COMPLEMENTARYGOBLINEXACTBMATCH);
        }

        virtual ~ComplementaryGoblinExactBmatch();

        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);


};


/* */
class NegatedWeightsGoblinExactBmatch : public GoblinExactBmatch
{

    public:
        NegatedWeightsGoblinExactBmatch()
        {
            description_str = string(NEGATEDWEIGHTSGOBLINEXACTBMATCH);
        }

        virtual ~NegatedWeightsGoblinExactBmatch();

        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);



};

#endif
