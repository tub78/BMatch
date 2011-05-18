
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, January 2005
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   branchMIP.h
/// \brief  #branchMIP class interface

#ifndef _BRANCH_MIP_H_
#define _BRANCH_MIP_H_

#include "branchScheme.h"
#include "ilpWrapper.h"


/// \addtogroup mixedInteger
/// @{

/// \brief  Branch & bound implementation for mixed integer programming

class branchMIP : public branchNode<TVar,TFloat>
{
private:

    TTimer          timerModule;

public:

    goblinILPWrapper& X;        // The original problem
    goblinILPWrapper* Y;        // Cloned from the parent branch node
/*
    THandle         index;
    THandle         numNodes;
    THandle         maxNode;

    THandle*        parentNode;
    TVar*           varIndex;
    TFloat*         sepValue;
*/
    branchMIP(goblinILPWrapper&,TTimer) throw();
    branchMIP(branchMIP&) throw();
    ~branchMIP() throw();

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();

    TNode           SelectVariable() throw();        // Select variable to branch with
    TBranchDir      DirectionConstructive(TArc) throw(ERRange);
    TBranchDir      DirectionExhaustive(TArc) throw(ERRange);
                        // Select branching direction

    branchNode<TVar,TFloat>* Clone() throw();      // Generate a copy of the branch node
    void            Raise(TVar) throw(ERRange);    // Raise lower variable bound
    void            Lower(TVar) throw(ERRange);    // Lower upper variable bound
    TFloat          SolveRelaxation() throw();     // Returns the objective value of the relaxed
                                                   // subproblem and a corresponding solution
    TObjectSense    ObjectSense() const throw()
                        {return TObjectSense(X.ObjectSense());};
    TFloat          Infeasibility() const throw();
    bool            Feasible() throw();            // Check for integrality
    void            SaveSolution() throw();        // Copy the current solution to the original instance
    TTimer          TimerModule() const throw() {return timerModule;};
        // The timer to which improved bounds are reported

};

/// @}

#endif
