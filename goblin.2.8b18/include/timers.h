
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, June 2003
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   timers.h
/// \brief  #goblinTimer class interface

#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "globals.h"


#if defined(_TIMERS_)

/// \brief  Module performance measurement

class goblinTimer
{
private:

    double          clockTick;

    double          accTime;
    double          minTime;
    double          maxTime;
    double          prevTime;
    unsigned long   nRounds;

    bool            enabled;
    unsigned long   nestingDepth;
    double          startTime;

    double*         savedTime;
    goblinTimer**   globalTimer;

    TFloat      lowerBound;
    TFloat      upperBound;

public:

    enum TProgressOptions {
        PROGRESS_BOUNDED = 0,
        PROGRESS_WRAP_AROUND = 1
    };

private:

    TProgressOptions    progressMode;
    double              progressCounter;
    double              maxCounter;

public:

    goblinTimer(goblinTimer** = NULL) throw();
    ~goblinTimer() throw();

    void        Reset() throw(ERRejected);
    bool        Enable() throw();
    bool        Disable() throw();

    double      AccTime() const throw();
    double      AvTime() const throw();
    double      MaxTime() const throw();
    double      MinTime() const throw();
    double      PrevTime() const throw();
    double      CurrTime() const throw();
    double      ChildTime(TTimer) const throw();

    bool        Enabled() const throw() {return enabled;};
    bool        FullInfo() const throw() {return (savedTime!=NULL);};

    void        ResetBounds(TFloat = -InfFloat,TFloat = InfFloat) throw();
    bool        SetLowerBound(TFloat) throw(ERRejected);
    bool        SetUpperBound(TFloat) throw(ERRejected);

    TFloat      LowerBound() const throw() {return lowerBound;};
    TFloat      UpperBound() const throw() {return upperBound;};

    void        InitProgressCounter(double,TProgressOptions) throw();
    void        ProgressStep(double) throw();
    void        SetProgressCounter(double) throw();
    void        SetProgressMax(double) throw();
    double      ProgressCounter() const throw() {return progressCounter;};
    double      ProgressMax() const throw() {return maxCounter;};
    TProgressOptions    ProgressMode() const throw() {return progressMode;};

};


typedef goblinTimer* pGoblinTimer;

#endif

#endif
