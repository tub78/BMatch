
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, March 2001
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   gosh.h
/// \brief  Declaration of all Tcl command procedures for GOBLIN objects

#include <goblin.h>
#include <tcl.h>
#include <tk.h>
#include <setjmp.h>
#include <pthread.h>


/// \cond DOCUMENT_SHELL

enum TGoshRetCode {
    GOSH_OK = 0,        ///< Alias for TCL_OK
    GOSH_ERROR = 1,     ///< Alias for TCL_ERROR
    GOSH_UNHANDLED = 2  ///< Command must be interpreted by another function
};

/// \brief  Data required for threads running an Tcl interpreter

typedef struct _TThreadData
{
    pthread_t       threadID;
    jmp_buf         jumpBuffer;
} TThreadData;

const unsigned MAX_NUM_THREADS = 10;

extern TThreadData goblinThreadData[MAX_NUM_THREADS];

unsigned Goblin_ReserveThreadIndex() throw();
unsigned Goblin_MyThreadIndex() throw();
void Goblin_FreeThreadData() throw();

extern goblinController *CT;
extern bool destroyThread;


extern "C" int Goblin_Init(Tcl_Interp* interp);
void Goblin_Config(goblinController& context) throw();

void *Goblin_Thread(void*);
void Goblin_MasterEventHandler();
void Goblin_TraceEventHandler(char*);
void Goblin_LogEventHandler(msgType,TModule,THandle,char*);
bool Goblin_StopSignalHandler();
void Goblin_TerminateHandler();

int Goblin_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete (ClientData clientData) throw();

int Goblin_Exit (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw();

int Goblin_Generic_Cmd (managedObject* X,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);

managedObject* Goblin_Read_Object(const char*,goblinController&) throw();
int Goblin_Propagate_Exception(Tcl_Interp* interp) throw();

void WrongNumberOfArguments(Tcl_Interp* interp,int argc,_CONST_QUAL_ char* argv[]) throw();


    // Support of GOBLIN Polymorphism

int Goblin_Mixed_Graph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Mixed_Graph (ClientData clientData) throw();

int Goblin_Sparse_Graph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Sparse_Graph (ClientData clientData) throw();

int Goblin_Dense_Graph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Dense_Graph (ClientData clientData) throw();

int Goblin_Sparse_Digraph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Sparse_Digraph (ClientData clientData) throw();

int Goblin_Dense_Digraph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Dense_Digraph (ClientData clientData) throw();

int Goblin_Sparse_Bigraph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Sparse_Bigraph (ClientData clientData) throw();

int Goblin_Dense_Bigraph_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Dense_Bigraph (ClientData clientData) throw();

int Goblin_Balanced_FNW_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Delete_Balanced_FNW (ClientData clientData) throw();


    //  Class Dependent Graph Optimization Commands

int Goblin_Sparse_Cmd (abstractMixedGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);

int Goblin_Bipartite_Cmd (abstractBiGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);

int Goblin_Undirected_Cmd (abstractGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);

int Goblin_Directed_Cmd (abstractDiGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);


    //  General Commands for Graph Manipulation

int Goblin_Generic_Graph_Cmd (abstractMixedGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);
int Goblin_Node_Cmd (abstractMixedGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);
int Goblin_Arc_Cmd (abstractMixedGraph* G,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);


    //  Commands for LP/ILP manipulation

int Goblin_Ilp_Cmd (ClientData clientData,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]);
int Goblin_Row_Cmd (goblinILPWrapper* XLP,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);
int Goblin_Variable_Cmd (goblinILPWrapper* XLP,Tcl_Interp* interp,
    int argc,_CONST_QUAL_ char* argv[]) throw(ERRejected,ERRange);
int Goblin_Delete_Ilp (ClientData clientData) throw();

/// \endcond
