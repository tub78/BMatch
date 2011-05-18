/*
 * B-matching toolbox
 * Stuart Andrews
 * January 2008
 * */
#ifndef _ABSTRACT_BMATCH_SOLVER_H_
#define _ABSTRACT_BMATCH_SOLVER_H_

#include <string>
using namespace std;
#include "Vector.hpp"


class AbstractBmatchSolver
{
    public:
        AbstractBmatchSolver() : description_str("bmatch interface") { };

        virtual ~AbstractBmatchSolver() { };

        virtual string description();

        /* 
         * a single interface that does everything
         * returns success (1) or failure (0)
         * */
        virtual int solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose) = 0;

        /* description string */
        string description_str;

};

#endif

