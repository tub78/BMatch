/*
 * B-matching toolbox
 * Stuart Andrews
 * January 2008
 * */
#ifndef _BMATCH_SOLVER_H_
#define _BMATCH_SOLVER_H_

#include "Vector.hpp"
#include "AbstractBmatchSolver.hpp"


class BmatchSolver : public AbstractBmatchSolver
{
    public:
        BmatchSolver()  
        {
            description_str = string("bmatch solver template");
        }

        virtual ~BmatchSolver();

        /* 
         * a single interface that does everything
         * returns success (1) or failure (0)
         * */
        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose);


    private:

        /* print memory */
        virtual void print(const int & print_what) = 0;

        /* initialize assuming 1-based indices */
        virtual void init(const int & in_verbose, const int& num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec) = 0; 

        /* solve */
        virtual int solve() = 0;

        /* copy b-matching approx solution
         *  assuming  0-based indices */
        virtual void solution(const int & num_bmatch_edge, DoubleVec & bmatch_edge_vec) = 0;

};


#endif
