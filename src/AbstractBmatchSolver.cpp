
#include "AbstractBmatchSolver.hpp"


/* a single interface that does everything returns success (1) or failure (0) */
int AbstractBmatchSolver::
solve_bmatching_problem(const int & num_node, const DoubleVec & deg_bdd_vec, const int & num_ijw, const DoubleVec & ijw_vec, int & num_bmatch_edge, DoubleVec & bmatch_edge_vec, double & bmatch_wgt, const int & verbose)
{
    return 0;
}

/* */
string AbstractBmatchSolver::
description()
{
    return description_str; 
}

