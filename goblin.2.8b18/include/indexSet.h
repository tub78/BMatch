
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, July 2005
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file indexSet.h
/// \brief Interface to index sets and some inline class implementations


#ifndef _INDEX_SET_H_
#define _INDEX_SET_H_

#include "managedObject.h"


/// \addtogroup indexSets
/// @{

/// \brief Interface class for index set objects

template <class TItem>
class indexSet : public virtual managedObject
{
protected:

    TItem maxIndex;

public:

    indexSet(TItem _maxIndex,goblinController &_context = goblinDefaultContext) throw() :
        managedObject(_context), maxIndex(_maxIndex) {};
    virtual ~indexSet() throw() {};

    virtual unsigned long Size() const throw() = 0;

    char* Display() const throw()
    {
        this -> LogEntry(MSG_TRACE,"Index set");

        THandle LH = this->LogStart(MSG_TRACE2,"  {");
        TItem counter = 0;

        for (TItem i=0;i<maxIndex;i++)
        {
            if (!IsMember(i)) continue;

            if (counter==0)
            {
                sprintf(this->CT.logBuffer,"%ld",i);
                this -> LogAppend(LH,this->CT.logBuffer);
            }
            else if (counter%10==0)
            {
                this -> LogEnd(LH,",");
                sprintf(this->CT.logBuffer,"   %ld",i);
                LH = this->LogStart(MSG_TRACE2,this->CT.logBuffer);
            }
            else
            {
                sprintf(this->CT.logBuffer,", %ld",i);
                this -> LogAppend(LH,this->CT.logBuffer);
            }

            counter++;
        }

        this -> LogEnd(LH,"}");

        return NULL;
    }

    /// \brief  Test for indet set membership
    ///
    /// \param i      An index to test for indet set membership
    /// \retval true  The indexed element is contained by this set
    virtual bool IsMember(TItem i) const throw(ERRange) = 0;

    /// \brief  Query the smallest index in an index set
    ///
    /// \return  The smallest index contained by this set
    virtual TItem First() const throw()
    {
        for (TItem i=0;i<maxIndex;i++)
        {
            if (IsMember(i)) return i;
        }

        return maxIndex;
    };

    /// \brief  Enumeration of the indices contained in a set
    ///
    /// \return  The least upper bound index  contained by this set
    virtual TItem Successor(const TItem i) const throw(ERRange)
    {
        for (TItem j=i+1;j<maxIndex;j++)
        {
            if (IsMember(j)) return j;
        }

        return maxIndex;
    };

};


/// \brief Index set representing a single index in an unlimited range

template <class TItem>
class singletonIndex : public indexSet<TItem>
{
private:

    TItem containedIndex;

public:

    singletonIndex(TItem _i,TItem _n,goblinController &_context = goblinDefaultContext) throw() :
        managedObject(_context),indexSet<TItem>(_n,_context), containedIndex(_i) {};
    ~singletonIndex() throw() {};

    unsigned long  Size() const throw()
    {
        return sizeof(singletonIndex<TItem>) + managedObject::Allocated();
    };

    bool  IsMember(TItem i) const throw(ERRange)
    {
        return (i==containedIndex);
    };

    TItem  First() const throw()
    {
        return this->containedIndex;
    };

    TItem  Successor(const TItem i) const throw(ERRange)
    {
        return this->maxIndex;
    };

};


/// \brief Index set representing all indices in an unlimited range

template <class TItem>
class fullIndex : public indexSet<TItem>
{
public:

    fullIndex(TItem _n,goblinController &_context = goblinDefaultContext) throw() :
        managedObject(_context),indexSet<TItem>(_n,_context) {};
    ~fullIndex() throw() {};

    unsigned long  Size() const throw()
    {
        return sizeof(fullIndex<TItem>) + managedObject::Allocated();
    };

    bool  IsMember(TItem i) const throw()
    {
        return true;
    };

    TItem  First() const throw()
    {
        return 0;
    };

    TItem  Successor(const TItem i) const throw(ERRange)
    {
        return i+1;
    };

};


/// \brief Index set representing no indices in an unlimited range

template <class TItem>
class voidIndex : public indexSet<TItem>
{
public:

    voidIndex(TItem _n,goblinController &_context = goblinDefaultContext) throw() :
        managedObject(_context),indexSet<TItem>(_n,_context) {};
    ~voidIndex() throw() {};

    unsigned long  Size() const throw()
    {
        return sizeof(voidIndex<TItem>) + managedObject::Allocated();
    };

    bool  IsMember(TItem i) const throw()
    {
        return false;
    };

    TItem  First() const throw()
    {
        return this->maxIndex;
    };

    TItem  Successor(const TItem i) const throw(ERRange)
    {
        return this->maxIndex;
    };

};


/// \brief Index set representing the set union of two index sets

template <class TItem>
class indexSetUnion : public indexSet<TItem>
{
private:

    indexSet<TItem>& set1;
    indexSet<TItem>& set2;

public:

    indexSetUnion(indexSet<TItem> &_set1,indexSet<TItem> &_set2)  throw() :
        managedObject(_set1.Context()),
        indexSet<TItem>(
            (_set1.maxIndex > _set2.maxIndex) ? _set1.maxIndex : _set2.maxIndex,_set1.Context()),
        set1(_set1), set2(_set2) {};

    ~indexSetUnion() throw() {};

    unsigned long Size() const throw()
        {return sizeof(indexSetUnion<TItem>) + managedObject::Allocated();};

    bool IsMember(TItem i) const throw(ERRange)
        {return (set1.IsMember(i) || set2.IsMember(i));};

};


/// \brief Index set representing the cut of two index sets

template <class TItem>
class indexSetCut : public indexSet<TItem>
{
private:

    indexSet<TItem>& set1;
    indexSet<TItem>& set2;

public:

    indexSetCut(indexSet<TItem> &_set1,indexSet<TItem> &_set2)  throw() :
        managedObject(_set1.Context()),
        indexSet<TItem>(
            (_set1.maxIndex > _set2.maxIndex) ? _set1.maxIndex : _set2.maxIndex,_set1.Context()),
        set1(_set1), set2(_set2) {};

    ~indexSetCut() throw() {};

    unsigned long Size() const throw()
        {return sizeof(indexSetCut<TItem>) + managedObject::Allocated();};

    bool IsMember(TItem i) const throw(ERRange)
        {return (set1.IsMember(i) && set2.IsMember(i));};

};


/// \brief Index set representing the difference of two index sets

template <class TItem>
class indexSetMinus : public indexSet<TItem>
{
private:

    indexSet<TItem>& set1;
    indexSet<TItem>& set2;

public:

    indexSetMinus(indexSet<TItem> &_set1,indexSet<TItem> &_set2)  throw() :
        managedObject(_set1.Context()),
        indexSet<TItem>(
            (_set1.maxIndex > _set2.maxIndex) ? _set1.maxIndex : _set2.maxIndex,_set1.Context()),
        set1(_set1), set2(_set2) {};

    ~indexSetMinus() throw() {};

    unsigned long Size() const throw()
        {return sizeof(indexSetMinus<TItem>) + managedObject::Allocated();};

    bool IsMember(TItem i) const throw(ERRange)
        {return (set1.IsMember(i) && !set2.IsMember(i));};

};


/// \brief Index set representing the symmetric difference of two index sets

template <class TItem>
class indexSetDifference : public indexSet<TItem>
{
private:

    indexSet<TItem>& set1;
    indexSet<TItem>& set2;

public:

    indexSetDifference(indexSet<TItem> &_set1,indexSet<TItem> &_set2)  throw() :
        managedObject(_set1.Context()),
        indexSet<TItem>(
            (_set1.maxIndex > _set2.maxIndex) ? _set1.maxIndex : _set2.maxIndex,_set1.Context()),
        set1(_set1), set2(_set2) {};

    ~indexSetDifference() throw() {};

    unsigned long Size() const throw()
        {return sizeof(indexSetDifference<TItem>) + managedObject::Allocated();};

    bool IsMember(TItem i) const throw(ERRange)
        {return (set1.IsMember(i) ^ set2.IsMember(i));};

};


/// \brief Index set representing the complement of another index sets

template <class TItem>
class indexSetComplement : public indexSet<TItem>
{
private:

    indexSet<TItem>& set1;

public:

    indexSetComplement(indexSet<TItem> &_set1)  throw() :
        managedObject(_set1.Context()),
        indexSet<TItem>(_set1.maxIndex,_set1.Context()),
        set1(_set1) {};

    ~indexSetComplement() throw() {};

    unsigned long Size() const throw()
        {return sizeof(indexSetComplement<TItem>) + managedObject::Allocated();};

    bool IsMember(TItem i) const throw(ERRange)
        {return !set1.IsMember(i);};

};

/// @}


#endif
