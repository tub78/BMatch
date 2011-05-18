
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, May 1999
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   goblinController.h
/// \brief  #goblinController and #goblinRootObject class interfaces

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "globals.h"
#include "timers.h"


// Forward declarations
class goblinImport;
class goblinExport;
class attributePool;
class goblinController;


const unsigned long goblinRandMax   = RAND_MAX;
const int MAX_MODULE_NESTING = 100;

/// \brief  Base class for all managed objects

class goblinRootObject
{
friend class goblinController;

private:

    mutable goblinRootObject* prevObject;
    mutable goblinRootObject* nextObject;

protected:

    THandle                 OH;

public:

    goblinRootObject() : OH(THandle(0)) {goblinNObjects++;};
    virtual ~goblinRootObject() {goblinNObjects--;};

    virtual char*  Display() const throw(ERFile,ERRejected) = 0;
    virtual const char*  Label() const throw() = 0;
    virtual unsigned long  Size() const throw() = 0;


    /// \addtogroup classifications
    /// @{

    /// \brief  Query if this is a graph like data object
    ///
    /// \retval true  This is a graph-like data object
    virtual bool  IsGraphObject() const;

    /// \brief  Query if this is an MIP like data object
    ///
    /// \retval true  This is a MIP or LP data object
    virtual bool  IsMixedILP() const;

    /// @}


    virtual objectType  ClassID() const throw() {return IDUnknown;};
    virtual void*  BackCast() const throw() {return NULL;};
    THandle  Handle() const throw() {return OH;};


    /// \addtogroup objectDimensions
    /// @{

public:

    /// \brief  Query a specified object dimension
    ///
    /// \param arrayDim  The entity type (e.g. graph edges)
    /// \param size      Asks either for the actual or for the reserved number of entities
    /// \return          The number of object entities
    ///
    /// This is used by attribute pools to determine the correct attribute capacities
    virtual size_t  SizeInfo(TArrayDim arrayDim,TSizeInfo size) const throw();

    /// @}


    virtual void  ConfigDisplay(goblinController &) const throw();


    /// \addtogroup groupObjectExport
    /// @{

public:

    /// \brief  Write a pseudo attribute to file
    ///
    /// \param F      An output file stream
    /// \param pool   The attribute pool
    /// \param token  An index in this pool
    ///
    /// This writes data to files which is logically associated with a certain
    /// attribute pool in terms of the file format, but which is not internally
    /// stored as an attribute.
    virtual void  WriteSpecial(goblinExport& F,const attributePool& pool,TPoolEnum token) const throw();

    /// @}


    /// \addtogroup groupObjectImport
    /// @{

public:

    virtual void  ReadSpecial(goblinImport&,attributePool&,TPoolEnum) throw(ERParse);

    /// @}


    // *************************************************************** //
    //                     Local Type Declarations                     //
    // *************************************************************** //

    enum TOwnership {
        OWNED_BY_SENDER = 1,
        OWNED_BY_RECEIVER = 0
    };

    enum TProgressOptions {
        PROGRESS_BOUNDED = 0,
        PROGRESS_WRAP_AROUND = 1
    };

#if defined(_HEAP_MON_LOCAL_)

    // *************************************************************** //
    //                       Memory Management                         //
    // *************************************************************** //

    void* operator new(size_t size) throw (std::bad_alloc);
    void* operator new(size_t size,const std::nothrow_t&) throw ();
    void* operator new[](size_t size) throw (std::bad_alloc);
    void* operator new[](size_t size,const std::nothrow_t&) throw ();

    void operator delete(void *p) throw ();
    void operator delete(void *p,const std::nothrow_t&) throw ();
    void operator delete[](void *p) throw ();
    void operator delete[](void *p,const std::nothrow_t&) throw ();

    void* GoblinRealloc(void* p,size_t size) const throw (std::bad_alloc);

#endif

};


inline bool goblinRootObject::IsGraphObject() const
{
    return false;
}


inline bool goblinRootObject::IsMixedILP() const
{
    return false;
}


inline size_t goblinRootObject::SizeInfo(TArrayDim arrayDim,TSizeInfo size) const throw()
{
    return 0;
}


inline void goblinRootObject::ConfigDisplay(goblinController &) const throw()
{
}


inline void goblinRootObject::WriteSpecial(
    goblinExport& F,const attributePool& pool,TPoolEnum token) const throw()
{
}


inline void goblinRootObject::ReadSpecial(
    goblinImport&,attributePool&,TPoolEnum) throw(ERParse)
{
}



/// \brief  A class for job / session management
///
/// With every data object, a controller object is assigned.
/// This controller object handles the following:
/// - Serves as a container for all related data objects
/// - Runtime configuration of optimization and display methods
/// - Runtime configuration and control of the tracing functionality
/// - Runtime configuration and control of the logging functionality
/// - Modul entry points for LP solvers

class goblinController : public goblinRootObject
{
friend class goblinExport;
friend class goblinILPWrapper;
private:

    int         externalPrecision;
    int         externalFloatLength;
    TNode       maxNode;
    TArc        maxArc;
    THandle     maxHandle;
    TIndex      maxIndex;
    long int    maxInt;

    int     savedTraceLevel;

    int            breakPoint;
    unsigned long  traceCounter;

    bool        isDefault;

public:

    goblinController() throw();
    goblinController(const goblinController&,bool = false) throw();
    virtual ~goblinController() throw();

    char*   Display() const throw();
    void    DisplayAll() const throw();

    int     FindParam(int pc,char* pStr[],const char* token,int offset = 1) throw();
    int     FindParam(int pc,const char* pStr[],const char* token,int offset = 1) throw();
    void    Configure(int ParamCount,const char* ParamStr[]) throw();
    void    SetDisplayParameters(TLayoutModel) throw();

    void    ReadConfiguration(const char*) throw(ERFile,ERParse);
    void    WriteConfiguration(const char*) throw(ERFile);

    char*   objectExtension;

    TNode   sourceNode;
    TNode   targetNode;
    TNode   rootNode;

    #if defined(_TIMERS_)

    pGoblinTimer* globalTimer;

    #endif


    // *************************************************************** //
    //           Module Management                                     //
    // *************************************************************** //

    static const int    majorVersion;
    static const int    minorVersion;
    static const char   patchLevel[];

    static char const*  pLPModule;

    void                ReleaseLPModule() throw();

    // *************************************************************** //
    //           Memory Management                                     //
    // *************************************************************** //

    bool            checkMem;

    unsigned long   Size() const throw();


    // *************************************************************** //
    //           Object Management                                     //
    // *************************************************************** //

    static char*                controllerTable;
    static goblinRootObject*    firstController;

    char*               objectTable;
    goblinRootObject*   firstObject;
    goblinRootObject*   masterObject;

    THandle     LinkController() throw(ERRejected);
    void        RegisterController() throw(ERRejected);
    void*       BackCast() const throw() {return (void*)this;};

    THandle     InsertObject(goblinRootObject*) throw(ERRejected);
    void        RegisterObject(goblinRootObject*,THandle) throw(ERRejected);
    void        DeleteObject(const goblinRootObject*) throw(ERRejected);
    bool        IsReferenced() throw()
                    {return (firstObject->nextObject != firstObject);};

    void        SetMaster(THandle) throw(ERRejected);
    THandle     Master() throw(ERCheck);
    const char* Label() const throw();
    goblinRootObject*   ObjectPointer(THandle) const throw(ERRejected);
    goblinRootObject*   Lookup(THandle) const throw(ERRejected);

    THandle (*newObjectHandler)();


    // *************************************************************** //
    //           Logging                                               //
    // *************************************************************** //

    int     logMeth;
    int     logMem;
    int     logMan;
    int     logIO;
    int     logRes;
    int     logWarn;
    int     logTimers;
    int     logGaps;

    int     logDepth;
    int     logLevel;

private:

    TModule nestedModule[MAX_MODULE_NESTING];
    int     moduleNestingLevel;

    unsigned long suppressCount;
    void (*savedLogEventHandler)(msgType,TModule,THandle,char*);

public:

    void IncreaseLogLevel() throw(ERRejected);
    void DecreaseLogLevel() throw(ERRejected);

    enum TFoldOptions {
        NO_INDENT = 1,
        SHOW_TITLE = 2
    };

    void OpenFold(TModule,TOption = 0) throw(ERRejected);
    void CloseFold(TModule,TOption = 0) throw(ERRejected);

#if defined(_PROGRESS_)

private:

    int progressHigh, progressLow;
    double progressNextStep;

public:

    void InitProgressCounter(double,
        TProgressOptions = PROGRESS_BOUNDED) throw();
    double ProgressCounter() const throw();
    double EstimatedExecutionTime() const throw();
    void ProgressStep(double) throw();
    void SetProgressCounter(double) throw();
    void SetProgressMax(double) throw();
    void SetProgressNext(double) throw();

#endif

    void SuppressLogging() throw();
    void RestoreLogging() throw();

    ostream* logStream;
    void (*logEventHandler)(msgType,TModule,THandle,char*);

    void PlainLogEventHandler(msgType,TModule,THandle,char*) throw();
    void DefaultLogEventHandler(msgType,TModule,THandle,char*) throw();

    mutable char logBuffer[LOGBUFFERSIZE];
    mutable bool compoundLogEntry;

private:

    THandle LogFilter(msgType,THandle,char*) const throw();

public:

    void    LogEntry(msgType,THandle,char*) const throw();
    THandle LogStart(msgType,THandle,char*) const throw();
    void    LogAppend(THandle,char*) const throw();
    void    LogEnd(THandle,char* = NULL) const throw();


    // *************************************************************** //
    //           Error Handling                                        //
    // *************************************************************** //

    void Error(msgType,THandle,char*,char*)
        throw(ERRange,ERRejected,ERCheck,ERFile,ERParse);

    char     savedErrorMethodName[LOGBUFFERSIZE];
    char     savedErrorDescription[LOGBUFFERSIZE];
    THandle  savedErrorOriginator;
    msgType  savedErrorMsgType;

    // *************************************************************** //
    //           Graph Browser                                         //
    // *************************************************************** //

    int    displayMode;

    int    xShift;
    int    yShift;
    double xZoom;
    double yZoom;

    int    layoutModel;

    int    nodeSize;
    int    nodeScalable;
    int    arrowSize;
    int    legenda;
    int    nodeSep;
    int    bendSep;
    int    fineSep;

    int    nodeStyle;
    int    nodeLabels;
    int    nodeColours;
    int    arcStyle;
    int    arcLabels;
    int    arcLabels2;
    int    arcLabels3;
    int    arrows;
    int    arcColours;
    int    subgraph;
    int    predecessors;

    char*  nodeFormatting;
    char*  arcFormatting;
    char*  wallpaper;


    // *************************************************************** //
    //           Tracing                                               //
    // *************************************************************** //

    int     traceLevel;
    int     commLevel;
    int     breakLevel;
    int     traceData;
    int     threshold;
    int     fileCounter;
    int     traceStep;

    void    Trace(THandle HH,unsigned long priority) throw();
    void    ResetCounters() throw();
    void    ResetTimer(TTimer timer) throw(ERRange);
    void    ResetTimers() throw();
    void    IncreaseCounter()
        {if (fileCounter<10000) fileCounter++; else fileCounter = 0;};

    void (*traceEventHandler)(char*);


    // *************************************************************** //
    //           Selection of mathematical methods                     //
    // *************************************************************** //

    int     methFailSave;

    /// Default method for the shortest path solver according to #abstractMixedGraph::TMethSPX
    int methSPX;

    /// Default method for the spanning tree solver according to #abstractMixedGraph::TMethMST
    int methMST;

    /// Default method for the max-flow solver according to #abstractMixedGraph::TMethMXF
    int methMXF;

    /// Default method for the min-cut solver according to #abstractMixedGraph::TMethMCC
    int methMCC;

    /// Default method for the min-cost st-flow solver according to #abstractMixedGraph::TMethMCFST
    int methMCFST;

    /// Default method for the min-cost flow solver according to #abstractMixedGraph::TMethMCF
    int methMCF;
    int methNWPricing;
    int methMaxBalFlow;
    int methBNS;
    int methDSU;
    int methPQ;
    int methLocal;
    int methSolve;
    int methMinCBalFlow;
    int methPrimalDual;
    int methModLength;
    int methCandidates;
    int methColour;
    int methHeurTSP;
    int methRelaxTSP1;
    int methRelaxTSP2;
    int methMaxCut;

    int methLP;
    int methLPPricing;
    int methLPQTest;
    int methLPStart;

    int maxBBIterations;
    int maxBBNodes;

    int methFDP;

    /// Default plane embedding method according to #abstractMixedGraph::TMethPlanarity
    int methPlanarity;

    /// Default orthogonal drawing method according to #abstractMixedGraph::TMethOrthogonal
    int methOrthogonal;

    bool (*stopSignalHandler)();

    bool SolverRunning() const throw();


    // *************************************************************** //
    //           Colour Table                                          //
    // *************************************************************** //

    /// \brief    Colour table lookup operation
    ///
    /// Colour indices exeeding the table are translated to some grey RGB colour.
    ///
    /// \param c  An internal colour index
    /// \return   An 24 bit RGB colour
    unsigned long   Colour(TIndex c) const throw();

    /// \brief Indices for adressing colours with a special meaning
    enum TColourDummy {
        PRED_COLOUR    = 0, ///< Index representing the colour used for predecessor arcs
        INFTY_COLOUR   = 1, ///< Colour index indicating unreachable nodes (dist == InfFloat)
        NO_COLOUR      = 2, ///< Colour index matching the node colour NoNode
        OUTLINE_COLOUR = 3, ///< Index representing the colour used for outlining
        ZERO_COLOUR    = 4, ///< Colour index matching the node and / or edge colour 0
        MAX_COLOUR     = 24 ///< First colour index exeeding the colour table
    };


    // *************************************************************** //
    //           Random Generators                                     //
    // *************************************************************** //

    unsigned long   Rand(unsigned long) throw(ERRange);
    TFloat          UnsignedRand() throw();
    TFloat          SignedRand() throw();
    void            SetRandomBounds(long int,long int) throw(ERRejected);

    int     randMin;
    int     randMax;

    int     randUCap;
    int     randLCap;
    int     randLength;
    int     randGeometry;
    int     randParallels;


    // Precision and External Number Formats

    TFloat  epsilon;
    TFloat  tolerance;

    void     SetExternalPrecision(int,int) throw(ERRejected);
    void     SetBounds(TNode,TArc,THandle,TIndex,long int) throw(ERRejected);
    TNode    MaxNode() const throw() {return maxNode;};
    TArc     MaxArc() const throw() {return maxArc;};
    THandle  MaxHandle() const throw() {return maxHandle;};
    TIndex   MaxIndex() const throw() {return maxIndex;};
    long int MaxInt() const throw() {return maxInt;};

    char     ExternalIntLength(long int) const throw();
    char     ExternalFloatLength(TFloat) const throw();

    template <typename T> size_t ExternalLength(TFloat value) const throw()
    {
        return ExternalFloatLength(value);
    }

    template <typename T> size_t ExternalLength(TCap value) const throw()
    {
        return ExternalFloatLength(value);
    }

    template <typename T> size_t ExternalLength(char* label) const throw()
    {
        if (label==NULL) return 2;
        else return strlen(label)+2;
    }

    template <typename T> size_t ExternalLength(T value) const throw()
    {
        // Intended for integer types only
        return ExternalIntLength(value);
    }


    /// \brief Indices for format specifications
    enum TFileFormat {
        FMT_GOBLIN      = 0, ///< Index representing the native file format
        FMT_DIMACS_MCF  = 1, ///< Index representing the Dimacs minCostFlow format
        FMT_DIMACS_EDGE = 2  ///< Index representing the Dimacs format for plain undirected graphs
    };

    goblinRootObject*   ImportFromFile(const char* filename, TFileFormat format) throw(ERParse);

private:

    goblinRootObject*   Import_DimacsMCF(const char* filename) throw(ERParse);
    goblinRootObject*   Import_DimacsEdge(const char* filename) throw(ERParse);

};


extern goblinController goblinDefaultContext;


#endif
