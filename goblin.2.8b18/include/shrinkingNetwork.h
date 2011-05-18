
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, February 1999
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   shrinkingNetwork.h
/// \brief  #layeredShrNetwork class interface

#ifndef _SHRINKING_NETWORK_H_
#define _SHRINKING_NETWORK_H_

#include "abstractBalanced.h"
#include "auxiliaryNetwork.h"
#include "dynamicStack.h"


/// \brief  Encapsulates the shortest augmenting path code for non-weighted balanced network flows

class layeredShrNetwork : public layeredAuxNetwork
{
friend class iLayeredAuxNetwork;

private:

    abstractBalancedFNW &   H;
    TFloat*                 dist;

    dynamicStack<TNode,TFloat>*          LeftSupport;
    dynamicStack<TNode,TFloat>*          RightSupport;
    staticQueue<TNode,TFloat>**          Q;
    staticQueue<TArc,TFloat>**           Anomalies;
    staticQueue<TArc,TFloat>**           Bridges;
    iLayeredAuxNetwork*    LI;
    iLayeredAuxNetwork*    RI;

    TNode*                 breakPoint;
    TArc*                  leftProp;
    TArc*                  rightProp;
    TArc*                  firstProp;
    TArc*                  petal;

public:

    layeredShrNetwork(abstractBalancedFNW& _H,TNode _s,
                        staticQueue<TNode,TFloat>** Q,
                        staticQueue<TArc,TFloat>** Anomalies,
                        staticQueue<TArc,TFloat>** Bridges) throw();
    ~layeredShrNetwork() throw();

    void            Init() throw();
    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();

    TNode           StartNode(TArc a) const throw(ERRange);
    TCap            UCap(TArc a) const throw(ERRange);

    TNode           DDFS(TArc a) throw(ERRange);
    void            ShrinkBlossom(TNode b,TArc currentPetal,TFloat tenacity)
                        throw(ERRange);

    TFloat          FindPath(TNode t) throw(ERRange,ERRejected);
    void            Expand(TNode x,TNode y) throw();
    void            CoExpand(TNode x,TNode y) throw();
    void            Traverse(TNode b,TNode x,TNode y,TArc currentPetal,
                        TArc* thisProp,TArc* otherProp) throw();

    TFloat          Augment(TArc currentPetal) throw();
};


#endif

