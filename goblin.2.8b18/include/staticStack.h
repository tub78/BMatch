
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, September 1998
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   staticStack.h
/// \brief  #staticStack class interface

#ifndef _STATIC_STACK_H_
#define _STATIC_STACK_H_

#include "goblinQueue.h"
#include "indexSet.h"


/// \addtogroup contiguousMemory
/// @{

/// \brief Implements a contiguous memory stack

template <class TItem,class TKey>
class staticStack : public goblinQueue<TItem,TKey>, public indexSet<TItem>
{
private:

    TItem*          prev;
    THandle*        set;
    TItem           top;
    TItem           bottom;
    TItem           n;
    TItem           depth;
    bool            master;

public:

    staticStack(TItem nn,goblinController &thisContext) throw();
    staticStack(staticStack<TItem,TKey> &S) throw();
    ~staticStack() throw();

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();
    char*           Display() const throw();

    void  Insert(TItem w,TKey alpha,TOptInsert mode) throw(ERRange,ERCheck);
    inline void  Insert(TItem w,TOptInsert mode) throw(ERRange,ERCheck);
    inline void  Insert(TItem w,TKey alpha = 0) throw(ERRange,ERCheck);

    TItem           Delete() throw(ERRejected);
    TItem           Peek() const throw(ERRejected);
    bool            Empty() const throw();
    TItem           Cardinality() const throw();

    bool            IsMember(TItem i) const throw(ERRange);
    TItem           First() const throw();
    TItem           Successor(const TItem i) const throw(ERRange);

};


template <class TItem,class TKey>
inline void staticStack<TItem,TKey>::Insert(TItem w,TOptInsert mode)
    throw(ERRange,ERCheck)
{
    Insert(w,0,mode);
}


template <class TItem,class TKey>
inline void staticStack<TItem,TKey>::Insert(TItem w,TKey alpha)
    throw(ERRange,ERCheck)
{
    Insert(w,alpha,INSERT_TWICE_THROW);
}


template <class TItem,class TKey>
inline bool staticStack<TItem,TKey>::Empty() const throw()
{
    return (top==n);
}


template <class TItem,class TKey>
inline TItem staticStack<TItem,TKey>::Cardinality() const throw()
{
    return depth;
}

/// @}

#endif
