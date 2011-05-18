
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, May 1999
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   managedObject.h
/// \brief  #managedObject class interface

#ifndef _MANAGED_OBJECT_H_
#define _MANAGED_OBJECT_H_

#include "goblinController.h"
#include "attributePool.h"


/// \brief  The base class for all kinds of managed data objects

class managedObject : public virtual goblinRootObject
{
protected:

    goblinController&   CT;

private:

    mutable unsigned int refCounter;
    char*               objectName;

public:

    managedObject(goblinController &thisContext = goblinDefaultContext,
        TOption options = 0) throw();
    virtual ~managedObject() throw();

    unsigned long   Allocated() const throw();


    // *************************************************************** //
    //                     Local Type Declarations                     //
    // *************************************************************** //

    enum TWriteInc {
        WRITE_INCIDENCES = 1,
        NO_INCIDENCES    = 0
    };

    enum TConstructorOpt {
        OPT_CLONE     = 128,
        OPT_SUB       = 32,
        OPT_PARALLELS = 64,
        OPT_COMPLETE  = 16,
        OPT_REVERSE   = 8,
        OPT_MAPPINGS  = 4,
        OPT_NO_LOOPS  = 2
    };

    enum TFoldOptions {
        NO_INDENT = 1,
        SHOW_TITLE = 2
    };

    enum TObjectSense {
        MAXIMIZE=2,
        NO_OBJECTIVE=0,
        MINIMIZE=1
    };

    /// \brief  Alternative options for the local search procedures
    enum TMethLocal {
        LOCAL_DEFAULT  = -1, ///< Apply the default method set in #goblinController::methLocal
        LOCAL_DISABLED =  0, ///< Do not apply post optimization procedures
        LOCAL_OPTIMIZE =  1  ///< Apply certain post optimization procedures
    };


    // *************************************************************** //
    //                        Memory Management                        //
    // *************************************************************** //

    goblinController&  Context() const throw() {return CT;};


    // *************************************************************** //
    //                Management of Object References                  //
    // *************************************************************** //

    void            MakeRef() const throw() {refCounter++;};
    void            ReleaseRef() const throw() {refCounter--;};
    bool            IsReferenced() const throw() {return (refCounter>0);};


    // *************************************************************** //
    //                     Dummy display option                        //
    // *************************************************************** //

    /// \brief  Unconditional display of data objects
    virtual char*   Display() const throw(ERFile,ERRejected);
    bool            IsUnnamed() const throw() {return (objectName==NULL);};
    const char*     Label() const throw();
    void            SetLabel(const char* l) throw();


    /// \addtogroup groupCanvasBuilder
    /// @{

    /// \brief  Export a data object to a xfig canvas file
    ///
    /// \param fileName  The destination file name
    virtual void  ExportToXFig(const char* fileName) const throw(ERFile,ERRejected);

    /// \brief  Export a data object to a tk canvas file
    ///
    /// \param fileName  The destination file name
    virtual void  ExportToTk(const char* fileName) const throw(ERFile,ERRejected);

    /// \brief  Export a data object to a graphviz dot file
    ///
    /// \param fileName  The destination file name
    virtual void  ExportToDot(const char* fileName) const throw(ERFile,ERRejected);

    /// @}


    /// \addtogroup groupTextDisplay
    /// @{

    /// \brief  Export a data object to file, in a class dependent ASCII format
    ///
    /// \param fileName  The destination file name
    /// \param format    A class dependent format specification
    virtual void  ExportToAscii(const char* fileName,TOption format=0) const throw(ERFile,ERRejected);

    /// @}


    // *************************************************************** //
    //                  Error Handling and Logging                     //
    // *************************************************************** //

    void  LogEntry(msgType msg,char* logEntry) const throw()
    {
        CT.LogEntry(msg,OH,logEntry);
    };

    THandle  LogStart(msgType msg,char* logEntry) const throw()
    {
        return CT.LogStart(msg,OH,logEntry);
    };

    void  LogAppend(THandle LH,char* logEntry) const throw()
    {
        CT.LogAppend(LH,logEntry);
    };

    void  LogEnd(THandle LH,char* logEntry = NULL) const throw()
    {
        CT.LogEnd(LH,logEntry);
    };

    void  Error(msgType msg,char* methodName,char* description)
        const throw(ERRange,ERRejected,ERCheck,ERFile,ERParse)
    {
        CT.Error(msg,OH,methodName,description);
    };

    void  Warning(char* methodName,char* description) const throw()
    {
        CT.Error(MSG_WARN,OH,methodName,description);
    };

    void  NoSuchNode(char* methodName,TNode) const throw(ERRange);
    void  NoMoreArcs(char* methodName,TNode) const throw(ERRejected);
    void  NoSuchArc(char* methodName,TArc) const throw(ERRange);
    void  CancelledArc(char* methodName,TArc) const throw(ERRejected);
    void  NoSuchHandle(char* methodName,THandle) const throw(ERRejected);
    void  NoSuchItem(char* methodName,unsigned long) const throw(ERRange);
    void  NoSuchItem(char* methodName,unsigned short) const throw(ERRange);
    void  NoSuchIndex(char* methodName,unsigned long) const throw(ERRange);
    void  NoSuchIndex(char* methodName,unsigned short) const throw(ERRange);
    void  NoSuchCoordinate(char* methodName,TDim) const throw(ERRange);
    void  UnknownOption(char* methodName,int) const throw(ERRejected);
    void  AmountOutOfRange(char* methodName,TFloat) const throw(ERRejected);
    void  NoRepresentation(char* methodName) const throw(ERRejected);
    void  NoSparseRepresentation(char* methodName) const throw(ERRejected);


    // *************************************************************** //
    //                        Module Management                        //
    // *************************************************************** //

    void  OpenFold()  const throw(ERRejected) {CT.IncreaseLogLevel();};
    void  CloseFold() const throw(ERRejected) {CT.DecreaseLogLevel();};

    void  OpenFold(const TModule,const TOption = 0) const throw(ERRejected);
    void  CloseFold(const TModule,const TOption = 0) const throw(ERRejected);

    void  SetLowerBound(const TTimer,const TFloat) const throw(ERRejected);
    void  SetUpperBound(const TTimer,const TFloat) const throw(ERRejected);
    void  SetBounds(const TTimer,const TFloat,const TFloat) const throw(ERRejected);
    void  ResetBounds(const TTimer,const TFloat = -InfFloat,const TFloat = InfFloat) const throw();

    TFloat  LowerBound(const TTimer) const throw();
    TFloat  UpperBound(const TTimer) const throw();

    void  Trace(unsigned long step = 0) throw()
    {
        CT.Trace(OH,step);
    };

    void  Trace(managedObject& X,unsigned long step = 0) throw()
    {
        CT.Trace(X.Handle(),step);
    };

#if defined(_PROGRESS_)

    void  InitProgressCounter(double l,TProgressOptions mode = PROGRESS_BOUNDED) throw()
    {
        CT.InitProgressCounter(l,mode);
    };

    void  ProgressStep(double step) const throw()
    {
        CT.ProgressStep(step);
    };

    void  SetProgressCounter(double l) throw()
    {
        CT.SetProgressCounter(l);
    };

    void  SetProgressMax(double l) throw()
    {
        CT.SetProgressMax(l);
    };

    void  SetProgressNext(double step) throw()
    {
        CT.SetProgressNext(step);
    };

#endif

};


inline void managedObject::ExportToXFig(const char* fileName) const
    throw(ERFile,ERRejected)
{
    throw ERRejected();
}


inline void managedObject::ExportToTk(const char* fileName) const
    throw(ERFile,ERRejected)
{
    throw ERRejected();
}


inline void managedObject::ExportToDot(const char* fileName) const
    throw(ERFile,ERRejected)
{
    throw ERRejected();
}


inline void managedObject::ExportToAscii(const char* fileName,TOption format) const
    throw(ERFile,ERRejected)
{
    throw ERRejected();
}


#define InternalError(scope,event) {sprintf(this->CT.logBuffer,"%s (%s, line: %d)",event,__FILE__,__LINE__); Error(ERR_INTERNAL,scope,this->CT.logBuffer);}
#define InternalError1(scope) {sprintf(this->CT.logBuffer,"%s (%s, line: %d)",this->CT.logBuffer,__FILE__,__LINE__); Error(ERR_INTERNAL,scope,this->CT.logBuffer);}


#endif
