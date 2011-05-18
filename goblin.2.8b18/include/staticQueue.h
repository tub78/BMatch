
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, September 1998
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   staticQueue.h
/// \brief  #staticQueue class interface

#ifndef _STATIC_QUEUE_H_
#define _STATIC_QUEUE_H_

#include "goblinQueue.h"
#include "indexSet.h"


/// \addtogroup contiguousMemory
/// @{

/// \brief Implements a contiguous memory FIFO queue

template <class TItem,class TKey>
class staticQueue : public goblinQueue<TItem,TKey>, public indexSet<TItem>
{
private:

    TItem*          next;
    THandle*        set;
    TItem           first;
    TItem           last;
    TItem           n;
    TItem           length;
    bool            master;

public:

    staticQueue(TItem nn,goblinController &thisContext) throw();
    staticQueue(staticQueue<TItem,TKey> &Q) throw();
    ~staticQueue() throw();

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();
    char*           Display() const throw();

    void  Insert(TItem w,TKey alpha,TOptInsert mode) throw(ERRange,ERCheck);
    inline void  Insert(TItem w,TOptInsert mode) throw(ERRange,ERCheck);
    inline void  Insert(TItem w,TKey alpha = 0) throw(ERRange,ERCheck);

    TItem           Delete() throw(ERRejected);
    TItem           Peek() const throw(ERRejected);
    inline bool     Empty() const throw();
    inline TItem    Cardinality() const throw();

    bool            IsMember(TItem i) const throw(ERRange);
    TItem           First() const throw();
    TItem           Successor(const TItem i) const throw(ERRange);

};


template <class TItem,class TKey>
inline void staticQueue<TItem,TKey>::Insert(TItem w,TOptInsert mode)
    throw(ERRange,ERCheck)
{
    Insert(w,0,mode);
}


template <class TItem,class TKey>
inline void staticQueue<TItem,TKey>::Insert(TItem w,TKey alpha)
    throw(ERRange,ERCheck)
{
    Insert(w,alpha,INSERT_TWICE_THROW);
}


template <class TItem,class TKey>
inline bool staticQueue<TItem,TKey>::Empty() const throw()
{
    return (first==n);
}


template <class TItem,class TKey>
inline TItem staticQueue<TItem,TKey>::Cardinality() const throw()
{
    return length;
}

/// @}

#endif
