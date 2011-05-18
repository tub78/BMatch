
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, May 2001
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   exportToTk.h
/// \brief  #exportToTk class interface

#ifndef _EXPORT_TO_TK_H_
#define _EXPORT_TO_TK_H_

#include "canvasBuilder.h"


/// \addtogroup groupCanvasBuilder
/// @{

/// \brief This class implements a builder for Tk canvas files of graph objects

class exportToTk : public canvasBuilder
{
private:

    ofstream       expFile;

    char*          canvasName;
    char*          smallFont;
    char*          largeFont;

public:

    exportToTk(const abstractMixedGraph &GG,const char* expFileName) throw(ERFile);
    ~exportToTk() throw();

    enum TObjectTypeID {
        ID_GRAPH_NODE = 0,
        ID_GRAPH_EDGE = 1,
        ID_NODE_LABEL = 2,
        ID_EDGE_LABEL = 3,
        ID_UPPER_LEFT = 4,
        ID_LOWER_RIGHT = 5,
        ID_BEND_NODE = 6,
        ID_ALIGN_NODE = 7,
        ID_BACKGROUND = 8,
        ID_PRED_ARC = 9,
        ID_ARROW = 10
    };

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();

    void    DisplayLegenda(long xm,long ym,long radius) throw();
    void    WriteNodeLegenda(long x,long y,char* index) throw();

    void    WriteLine(TArc,TNode,TNode,TLineStyle,int,TArrowDir,TIndex colourIndex,int depth) throw();
    void    WriteArrow(TArc a,long xtop,long ytop,double dx,double dy) throw();
    void    WriteArcLabel(TArc a,long xm,long ym) throw();

    void    DisplayNode(TNode v) throw();
    void    WriteSmallNode(TNode v,long x,long y) throw();
    void    WriteCircularNode(TNode v,long x,long y,char* fillColour) throw();
    void    WriteRectangularNode(TNode v,long x,long y,char* fillColour) throw();
    void    WriteNodeLabel(TNode v,long x,long y) throw();

    void    DisplayArtificialNode(TNode v) throw();

};

/// @}

#endif
