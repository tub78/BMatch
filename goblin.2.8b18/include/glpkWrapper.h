
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Bernhard Schmidt, August 2003
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   glpkWrapper.h
/// \brief  #goblinGLPKWrapper class interface

#ifndef _GLPK_WRAPPER_H_
#define _GLPK_WRAPPER_H_

#include "ilpWrapper.h"
#include "glpk.h"


/// \brief  Wrapper class for the GLPK 4.0 linear programming kit

class goblinGLPKWrapper : public virtual goblinILPWrapper
{
private:

    LPX* lp;

    mutable TRestr* index;
    mutable TRestr* rowIndex;

    mutable int    indexBuffer[20000];
    mutable double valueBuffer[20000];

    TObjectSense objSense;

public:

    goblinGLPKWrapper(TRestr = 0,TVar = 0,TIndex = 0,TObjectSense = MINIMIZE,
    goblinController& = goblinDefaultContext) throw();
    goblinGLPKWrapper(const char*,goblinController& = goblinDefaultContext)
        throw(ERFile,ERParse);
    ~goblinGLPKWrapper() throw();

    unsigned long   Allocated() const throw() {return 0;};
    unsigned long   Size() const throw() {return sizeof(goblinGLPKWrapper);};
    objectType      ClassID() const throw() {return IDGlpkWrapperObject;};
    void*           BackCast() const throw() {return (void*)this;};
    void            DeleteYourself() throw() {delete this;};
    void            Resize(TRestr,TVar,TIndex) throw(ERRange){};
    void            Strip() throw() {};

    // *************************************************************** //
    //                      File Input and Output                      //
    // *************************************************************** //

    void  ReadMPSFile(char*) throw();
    void  ReadCPLEX_LP(char*) throw();
    void  ReadBASFile(char*) throw(ERParse,ERRejected);
    void  WriteMPSFile(char*,TLPFormat = MPS_CPLEX) const throw(ERFile,ERRejected);
    void  WriteBASFile(char*,TLPFormat = BAS_CPLEX) const throw(ERFile,ERRejected);

    // *************************************************************** //
    //                      Instance Manipulation                      //
    // *************************************************************** //

    TVar    AddVar(TFloat lb,TFloat ub,TFloat cost,TVarType = VAR_FLOAT)
                throw(ERRejected);
    TRestr  AddRestr(TFloat lb,TFloat ub) throw(ERRejected);

    void    DeleteVar(TVar) throw(ERRange,ERRejected);
    void    DeleteRestr(TRestr) throw(ERRange);

    void    SetURange(TVar,TFloat) throw(ERRange);
    void    SetLRange(TVar,TFloat) throw(ERRange);
    void    SetUBound(TRestr,TFloat) throw(ERRange);
    void    SetLBound(TRestr,TFloat) throw(ERRange);
    void    SetCost(TVar,TFloat) throw(ERRange);
    void    SetVarType(TVar,TVarType) throw(ERRange);
    void    SetVarLabel(TVar,char*,TOwnership = OWNED_BY_SENDER)
                throw(ERRange,ERRejected);
    void    SetRestrLabel(TRestr,char*,TOwnership = OWNED_BY_SENDER)
                throw(ERRange,ERRejected);

    void    SetObjectSense(TObjectSense dd) throw();

    void    SetCoeff(TRestr,TVar,TFloat) throw(ERRange);

    void    SetRow(TRestr i,TVar len,TVar* index,double* val) throw(ERRange);
    // Sets row i of the coefficient matrix. len is the number
    // of nonzeros in the new row i. The coefficients are given by
    // index and val.

    void    SetColumn(TVar j,TRestr len,TRestr* index,double* val) throw(ERRange);
    // Sets columns j of the coefficient matrix. len is the
    // number of nonzeros in the new column j. The coefficients
    // are given by index and val.


    // *************************************************************** //
    //                      Retrieval Operations                       //
    // *************************************************************** //

    TRestr  K() const throw();
    TVar    L() const throw() ;
    TIndex  NZ() const throw();

    TFloat  Cost(TVar) const throw(ERRange);
    TFloat  URange(TVar) const throw(ERRange);
    TFloat  LRange(TVar) const throw(ERRange);

    TFloat  UBound(TRestr) const throw(ERRange);
    // UBound(i) = upper bound of restriction i, i=0,...,kAct+lAct-1

    TFloat  LBound(TRestr) const throw(ERRange);
    // LBound(i) = lower bound of restriction i, i=0,...,kAct+lAct-1

    TVarType    VarType(TVar) const throw(ERRange);
    char*   VarLabel(TVar,TOwnership = OWNED_BY_RECEIVER) const throw(ERRange);
    char*   RestrLabel(TRestr,TOwnership = OWNED_BY_RECEIVER) const throw(ERRange);
    TObjectSense  ObjectSense() const throw();

    TFloat  Coeff(TRestr,TVar) const throw(ERRange);

    TVar    GetRow(TRestr i,TVar* index,double* val) const throw(ERRange);
    // Gets Row i of the coefficient matrix and stores it in index
    // and val. val[j] is the coefficient of variable index[j] in row i. 
    // Returns the number of nonzeros of row i. index and val have to
    // be allocated before calling the method.

    TRestr  GetColumn(TVar j,TRestr* index,double* val) const throw(ERRange);
    // Gets Column j of the coefficient matrix and stores it in index
    // and val. val[i] is the coefficient of variable index[i] in row i. 
    // Returns the number of nonzeros of column j. index and val have to
    // be allocated before calling the method.


    // *************************************************************** //
    //                    Basis dependent information                  //
    // *************************************************************** //

private:

    void    EvaluateBasis() throw(ERRejected);

public:

    void    SetRestrType(TRestr,TLowerUpper) throw(ERRange,ERRejected);
    void    SetIndex(TRestr,TVar,TLowerUpper) throw(ERRange,ERRejected);

    void    InitIndex() const throw();
    void    ReleaseIndex() const throw();

    TRestr      Index(TVar) const throw(ERRange);
    TRestr      RowIndex(TRestr) const throw(ERRange);
    TVar        RevIndex(TRestr) const throw(ERRange);
    TRestrType  RestrType(TRestr) const throw(ERRange);

    void    InitBasis() const throw();
    // Sets variable range restrictions as basis (standardbasis)

    TFloat  X(TVar) const throw(ERRange);
    TFloat  Y(TRestr,TLowerUpper) const throw(ERRange);

    TFloat  Tableau(TIndex,TIndex) const throw(ERRange,ERRejected); 
    TFloat  BaseInverse(TIndex,TIndex) const throw(ERRange,ERRejected);

    void    Pivot(TIndex,TIndex,TLowerUpper) throw(ERRange,ERRejected);

    // *************************************************************** //
    //                           Algorithms                            //
    // *************************************************************** //

    TFloat  SolveLP() throw(ERRejected);
    TFloat  SolveMIP() throw(ERRejected);

    // *************************************************************** //
    //                        New methods                               //
    // *************************************************************** //

    //Read LP from files in GNU Math Programming language
    void  ReadGNU_MP_Model(char*,char*,char*) throw();

    //Read LP from file in CPLEX LP format
    void  WriteCPLEX_LP(char*) const throw();

    //Write basic solution to text file in readable format
    void  WriteSimplexSolution(char*) const throw();

    //Calculates z:=A_kA_B^{-1} and stores it in index and val. 
    //Returns the number of nonzeros of z
    // k must be a ***nonbasic*** row. index and val have to be
    //allocated before calling the method.  
    TIndex TableauRow(TIndex k,TIndex* index,double* val) const throw(ERRange);

    //Calculates w:=AA_B^{-1}e_i where B_i=k and stores it in index and val.
    //Returns the number of nonzeros of w.
    //k must be a ***basic** row. index and val have to be 
    //allocated before calling the method.
    TIndex TableauColumn(TIndex k,TIndex* index,double* val) const throw(ERRange);

    //Calculates t:=rA_B^{-1} for a prescribed row r which has
    //to be stored in index and val before calling the method.
    //len is the number of nonzeros of r.
    //Stores t in index and val.
    //Returns the number of nonzeros of t.
    //Enough memory has to be allocated for index and val
    //before calling the method.
    TIndex TransformRow(TIndex len,TIndex* index,double* val);

    //Sets all control parameters to default values
    void ResetParameters();

    //Sets an integer control parameter. A description of all control
    //parameters can be found in the GLPK reference manual.
    void SetIntegerParameter(int parameter,int value);

    //Sets a real control parameter. A description of all control
    //parameters can be found in the GLPK reference manual.
    void SetRealParameter(int parameter,double value);

    //Returns the value of an integer parameter. A description of all control
    //parameters can be found in the GLPK reference manual.
    int GetIntegerParameter(int parameter);

    //Returns the value of a real parameter. A description of all control
    //parameters can be found in the GLPK reference manual.
    double GetRealParameter(int parameter);


    // *************************************************************** //
    //                           Not Available                         //
    // *************************************************************** //

public:

    bool    CCost()    const throw() {return false;};
    bool    CURange()  const throw() {return false;};
    bool    CLRange()  const throw() {return false;};
    bool    CUBound()  const throw() {return false;};
    bool    CLBound()  const throw() {return false;};
    bool    CVarType() const throw() {return false;};
    bool    Initial()  const throw() {return false;};

    bool    StartPrimal() throw() {return false;};
    TFloat  SolvePrimal() throw() {return 0;};
    bool    StartDual() throw() {return false;};
    TFloat  SolveDual() throw() {return 0;};

    TVar    PricePrimal() throw(ERRejected){return 0;};
    TRestr  QTestPrimal(TVar) throw(ERRejected){return 0;};

    TRestr  PriceDual() throw(ERRejected){return 0;};
    TVar    QTestDual(TRestr) throw(ERRejected){return 0;};

};



/// \brief  Object factory and package information for the GLPK 4.0 linear programming kit

class glpkModule : public virtual goblinILPModule
{
public:

    glpkModule() throw() : managedObject(goblinDefaultContext) {};

    goblinILPWrapper*
        NewInstance(TRestr k,TVar l,TIndex nz,TObjectSense dir,
            goblinController& thisContext) const throw(ERRange)
            {return new goblinGLPKWrapper(k,l,nz,dir,thisContext);};

    goblinILPWrapper*
        ReadInstance(const char* impFileName,goblinController& thisContext)
            const throw(ERRange)
            {return new goblinGLPKWrapper(impFileName,thisContext);};

    goblinILPWrapper*
        Reinterprete(void* X) const throw(ERRange)
        {return (goblinGLPKWrapper*)X;};

    ~glpkModule() throw() {};

    objectType ClassID() const throw()   {return IDGlpkWrapperModule;};
    void*   BackCast() const throw()     {return (void*)this;};
    void    DeleteYourself() throw()     {delete this;};

    char*   Authors() const throw()      {return "Andrew Makhorin";};
    int     MajorRelease() const throw() {return 4;};
    int     MinorRelease() const throw() {return 0;};
    char*   PatchLevel() const throw()   {return "";};
    char*   BuildDate() const throw()    {return "";};
    char*   License() const throw()      {return "GPL";};

    TLPOrientation  Orientation() const throw() {return ROW_ORIENTED;};
};


#endif
