
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, October 2000
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file denseInclude.h
/// \brief Methods which are common among all dense represented graph objects


bool IsDense() const throw() {return true;};
void ConfigDisplay(goblinController &CCT) const throw()
{
    if (CCT.subgraph>1) CCT.subgraph = 1;
};


#include "graphInclude.h"

