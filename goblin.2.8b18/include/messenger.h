
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, June 2002
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   messenger.h
/// \brief  #goblinMessenger class interface

#if !defined(_MESSENGER_H_)

#define _MESSENGER_H_

#include "goblinController.h"
#include <pthread.h>


typedef unsigned long TLog;


/// \brief  Control of thread communication

class goblinMessenger
{
private:

    goblinController&   CT;

    TLog        qSize;
    TLog        firstEntry;
    TLog        firstFree;
    TLog        lastEntry;
    TLog        currentEntry;
    msgType*    qMsg;
    TModule*    qModule;
    THandle*    qHandle;
    int*        qLevel;
    char**      qText;
    TLog*       qNext;
    bool*       qHidden;

    pthread_mutex_t msgLock;
    pthread_mutex_t traceLock;
    pthread_mutex_t traceEventLock;
    pthread_mutex_t solverStateLock;

    char*       cTraceFile;
    bool        traceEvent;

    char        signalHalt;

public:

    goblinMessenger(TLog,goblinController &thisContext = goblinDefaultContext) throw();
    ~goblinMessenger() throw();

    unsigned long   Size() throw();

    void        Restart() throw();

    void        LogEventHandler(msgType,TModule,THandle,char*) throw();

    void        MsgAppend(msgType,TModule,THandle,char*) throw();

    void        MsgReset() throw();
    bool        MsgEOF() throw();
    bool        MsgVoid() throw();
    void        MsgSkip() throw(ERRejected);

    char*       MsgText() throw(ERRejected);
    msgType     MsgClass() throw(ERRejected); 
    TModule     MsgModule() throw(ERRejected); 
    int         MsgLevel() throw(ERRejected); 
    THandle     MsgHandle() throw(ERRejected); 
    bool        MsgHidden() throw(ERRejected);

    void        TraceAppend(char*) throw();
    void        TraceSemTake() throw();

    char*       TraceFilename() throw(ERRejected);
    bool        TraceEvent() throw();
    void        TraceUnblock() throw();

    bool        SolverIdle() throw();
    bool        SolverRunning() throw();
    void        SolverSignalPending() throw();
    void        SolverSignalStarted() throw();
    void        SolverSignalStop() throw();
    void        SolverSignalIdle() throw();

};


#endif
