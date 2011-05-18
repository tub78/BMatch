
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, August 2000
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file canvasBuilder.h
/// \brief #canvasBuilder class interface

#ifndef _CANVAS_BUILDER_H_
#define _CANVAS_BUILDER_H_

#include "abstractMixedGraph.h"


/// \addtogroup groupCanvasBuilder
/// @{

/// \brief Builder class interface for displaying a canvas or writing canvas files
///
/// This class partially implements the drawing of graph objects in an unspecified
/// file format of graphical tool kit. The class interface also declares the
/// methods which must be implemented by concrete canvas buiders.

class canvasBuilder : protected managedObject
{
protected:

    const abstractMixedGraph &G;
    goblinController CFG;

    char* label;

    char predColour[8];
    char inftyColour[8];
    char nodeColour[8];

    TArc maxNodeColour;
    TArc maxEdgeColour;

    const TFloat* piG;

    double zoom;

    double minX;
    double maxX;
    double minY;
    double maxY;

    double nodeSize;
    double arrowSize;

public:

    canvasBuilder(const abstractMixedGraph &GC) throw(ERRejected);
    virtual ~canvasBuilder() throw();

    unsigned long   Allocated() const throw();

    void            ComputeBoundingBox() throw();
    double          ScaleDisplayParameters(double zoomCalibration,double sizeX, double sizeY) throw();

    TFloat          CX(TNode v) throw()
                        {return G.C(v,0);};
    TFloat          CY(TNode v) throw()
                        {return G.C(v,1);};
    int             RoundToInt(double val) const throw()
                        {return ((val<0) ? int(val-0.5) : int(val+0.5));};
    long            RoundToLong(double val) const throw()
                        {return ((val<0) ? long(val-0.5) : long(val+0.5));};

    /// \brief  Determine the canvas X coordinate value of a graph or layout node
    ///
    /// \param v  A node index ranged [0,1,..,n+ni-1]
    /// \return   The ordinate value of v in the canvas
    long  CanvasCX(TNode v) throw()
                        {return long(CFG.xShift+G.C(v,0)*CFG.xZoom);};

    /// \brief  Determine the canvas Y coordinate value of a graph or layout node
    ///
    /// \param v  A node index ranged [0,1,..,n+ni-1]
    /// \return   The abscissa value of v in the canvas
    long  CanvasCY(TNode v) throw()
                        {return long(CFG.yShift+G.C(v,1)*CFG.yZoom);};


    /// \brief  Possible modes for the computation of arc display ends
    enum TPortMode {
        PORTS_IMPLICIT = 0, ///< Arc display ends are computed to match the displayed node size
        PORTS_EXPLICIT = 1  ///< Arc display ends are specified by explicit layout nodes
    };

protected:

    /// \brief Applied port mode, depending on the layout model of this graph object
    TPortMode portMode;

public:

    /// \brief  Determine the canvas X coordinate value of a virtual port node
    ///
    /// Arcs are not routed in the canvas between the center points of its two
    /// graph end nodes. Instead, two virtual port nodes are introduced which
    /// occur on the border line of the visualization of the end nodes. This
    /// procedure is necessary for the correct alignment of arrow heads, but
    /// works perfectly only if the graph nodes are visualized by circles.
    ///
    /// \param u  A layout point index ranged [0,1,..,n+ni-1]. This point is
    ///           considered to be the adjacent bend node with v on the processed arc
    /// \param v  A graph node index ranged [0,1,..,n-1]
    /// \return   The ordinate value of the virtual port node in the canvas
    long  PortCX(TNode u,TNode v) throw();

    /// \brief  Determine the canvas Y coordinate value of a virtual port node
    ///
    /// This is the analogue of PortCX() for the canvas abscissa
    ///
    /// \param u  A layout point index ranged [0,1,..,n+ni-1]. This point is
    ///           considered to be the adjacent bend node with v on the processed arc
    /// \param v  A graph node index ranged [0,1,..,n-1]
    /// \return   The abscissa value of the virtual port node in the canvas
    long  PortCY(TNode u,TNode v) throw();

    goblinController &Configuration() throw() {return CFG;};

    virtual void    DisplayGraph() throw(ERRejected);

    enum TArrowDir
    {
        ARROW_NONE = 0,
        ARROW_FORWARD  = 1,
        ARROW_BACKWARD = 2,
        ARROW_BOTH     = 3
    };

    enum TLineStyle
    {
        LINE_STYLE_SOLID    = 0,
        LINE_STYLE_DOT      = 1,
        LINE_STYLE_DASH     = 2,
        LINE_STYLE_DASH_DOT = 3,
        LINE_STYLE_ZIG_ZAG  = 8,
        LINE_STYLE_WAVED    = 16
    };

    void        DisplayArc(TArc) throw();
    TArrowDir   ArrowDirections(TArc a) throw();
    void        DisplayArrow(TArc,TNode,TNode) throw();
    void        DisplayArcLabel(TArc a,TNode u,TNode v,TArrowDir displayedCenteredArrows) throw();
    void        ComposeArcLabel(TArc a,ofstream& expFile) throw();
    char*       ArcLabel(TArc,int) throw(ERRejected);
    void        ComposeNodeLabel(TNode v,ofstream& expFile) throw();
    char*       NodeLabel(TNode,int) throw(ERRejected);
    char*       ArcLegenda(int) throw(ERRejected);
    char*       NodeLegenda(char*,int) throw(ERRejected);

    /// \brief Translate a node colour index to a string representing fixed colours
    ///
    /// \param c  An internal node colour index
    /// \return   A 6 digit hexadecimal RGB string
    char*       FixedNodeColour(TIndex c) throw();

    /// \brief Translate an edge colour index to a string representing fixed colours
    ///
    /// \param c  An internal edge colour index
    /// \return   A 6 digit hexadecimal RGB string
    char*       FixedEdgeColour(TIndex c) throw();

    /// \brief Translate a colour index to a string representing floating colours
    ///
    /// \param c            A colour to be translated
    /// \param maxColour    The maximal representable colour index
    /// \param undefColour  The default colour for unhandled colour indices
    /// \return             A 6 digit hexadecimal RGB string
    char*       SmoothColour(TIndex c,TIndex maxColour,goblinController::TColourDummy undefColour) throw();


    // *************************************************************** //
    //                Prototypes                                       //
    // *************************************************************** //

    virtual void    WriteArc(TArc,TNode,TNode,TLineStyle,int,TArrowDir,TIndex colourIndex,int) throw();
    virtual void    WriteLine(TArc,TNode,TNode,TLineStyle,int,TArrowDir,TIndex colourIndex,int) throw() = 0;
    virtual void    WriteArrow(TArc a,long xtop,long ytop,double dx,double dy) throw() = 0;
    virtual void    WriteArcLabel(TArc a,long xm,long ym) throw() = 0;
    virtual void    DisplayNode(TNode) throw() = 0;
    virtual void    DisplayLegenda(long xm,long ym,long radius) throw() = 0;

};

/// @}

#endif
