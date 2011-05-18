
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, December 1998
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   sparseGraph.h
/// \brief  Class interface to sparse undirected graph objects

#if !defined(_SPARSE_GRAPH_H_)
#define _SPARSE_GRAPH_H_

#include "abstractGraph.h"
#include "sparseRepresentation.h"


/// \brief A class for undirected graphs represented with incidence lists

class graph : public abstractGraph
{
    friend class sparseRepresentation;

private:

    bool mode;

protected:

    sparseRepresentation X;

public:

    /// \brief  Default constructor for sparse undirected graphs
    ///
    /// \param _n       The initial number of nodes
    /// \param _CT      The context to which this graph object is attached
    /// \param _mode    If true, the graph is not displayed before destruction
    graph(TNode _n=0,goblinController& _CT=goblinDefaultContext,
        bool _mode=false) throw();

    /// \brief  File constructor for sparse undirected graphs
    ///
    /// \param fileName  The source file name
    /// \param _CT       The context to which this graph object is attached
    graph(const char* fileName,goblinController& _CT=goblinDefaultContext)
        throw(ERFile,ERParse);

    /// \brief  Copy constructor for sparse undirected graphs
    ///
    /// \param G        The original mixed graph object
    /// \param options  A combination of OPT_CLONE, OPT_PARALLELS, OPT_MAPPINGS and OPT_SUB
    graph(abstractMixedGraph& G,TOption options=0) throw();

    ~graph() throw();

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();
    virtual objectType ClassID() const throw() {return IDSparseGraph;};
    virtual void*   BackCast() const throw() {return (void*)this;};

    #include <sparseInclude.h>

};


/// \addtogroup groupGraphComposition
/// @{

/// \brief Complementary graph
///
/// A graph in which nodes are adjacent if they are not adjacent in the original
/// graph.

class complementaryGraph : public graph
{
public:

    complementaryGraph(abstractMixedGraph& G,TOption = 0) throw(ERRejected);

};

/// @}


/// \addtogroup groupPlanarComposition
/// @{

/// \brief  Planar line graphs of 3-dimensional convex polyhedra
///
/// A planar line graph has the original graph edges as its node set. Nodes are
/// adjacent if the original edges have an end node and a face in common.

class planarLineGraph : public graph
{
public:

    planarLineGraph(abstractMixedGraph& G,TOption = 0) throw(ERRejected);

};


/// \brief  Vertex truncations of 3-dimensional convex polyhedra
///
/// In this graph, every node of the original graph is replaced by face.
/// For every original edge, the new end nodes appear in the face
/// associated with the former end nodes. Arcs are added to enclose the
/// the new faces.

class vertexTruncation : public graph
{
public:

    vertexTruncation(abstractMixedGraph& _G,TOption = 0) throw(ERRejected);

};


/// \brief  Facet separations of 3-dimensional convex polyhedra
///
/// In this graph, every edge of the original graph is split into a pair of
/// edges. The original faces are maintained, but separated from each other.
/// Depending on the #TOptRotation constructor parameter, additional faces
/// are added.

class facetSeparation : public graph
{
public:

    /// \brief  Rule for adding the separating faces
    enum TOptRotation {
        ROT_NONE  = 0, ///< Adjacent faces are separated by rectangles
        ROT_LEFT  = 1, ///< Adjacent faces are separated by triangles according to some left-hand rule
        ROT_RIGHT = 2  ///< Adjacent faces are separated by triangles according to some right-hand rule
    };

    facetSeparation(abstractMixedGraph& G,TOptRotation mode = ROT_NONE) throw(ERRejected);

};


/// \brief  Undirected duals of planar graphs
///
/// The nodes of the dual graph are the faces in the original graph. Every edge
/// of the original graph is associated with an edge in the dual graph where it
/// connects the nodes mapped from the faces which where originally separated by
/// this arc.

class dualGraph : public graph
{
public:

    dualGraph(abstractMixedGraph& G,TOption = 0) throw(ERRejected);

};


/// \brief Unroll the faces of regular convex polyhedra
///
/// This constructor requires a spanning tree of the original graph. All
/// edges other than the tree edges are are duplicated. The original faces
/// are maintained, but an exterior face is introduced. This is enclosed by
/// all duplicated edges.
///
/// The original faces are drawn by regular polygones. It my happen that the
/// faces overlap. This depends on the choice of the spanning tree.

class spreadOutRegular : public graph
{
public:

    spreadOutRegular(abstractMixedGraph& G,TOption = 0) throw(ERRejected);

};


/// \brief  Replace the interior faces of a rectangular grid by copies of a certain graph

class tiling : public graph
{
public:

    tiling(abstractMixedGraph& G,TOption,TNode,TNode) throw();

    TNode Index(TNode,TNode,TNode,TNode,TNode) throw();

};

/// @}


/// \addtogroup groupGraphComposition
/// @{

/// \brief  Discrete Voronoi diagrams of graphs with non-negative edge lengths
///
/// A Voronoi diagram (in the graph-theoretic sense) is some kind of surface graph.
/// The constructor takes a set of terminal nodes, a partition of the node set
/// (implicitly assigning every node to its minimum distance terminal) and the
/// distance labels. It identifies the nodes in every part (applying the same rules
/// as the #colourContraction constructor) and assigns length labels according to
/// the shortest path lengths between two terminal nodes.

class voronoiDiagram : public graph
{
private:

    abstractMixedGraph&     G;
    const indexSet<TNode>&  Terminals;
    TArc*                   revMap;

public:

    voronoiDiagram(abstractMixedGraph& _G,const indexSet<TNode>& _Terminals) throw();
    ~voronoiDiagram() throw();

    TFloat UpdateSubgraph() throw();

};


/// \brief  Line graphs of complete graphs with a given node set cardinality

class triangularGraph : public graph
{
public:

    triangularGraph(TNode cardinality,
        goblinController& thisContext = goblinDefaultContext) throw();

};

/// @}


/// \addtogroup groupPlanarComposition
/// @{

/// \brief  Sierpinski triangles with a given depth

class sierpinskiTriangle : public graph
{
public:

    sierpinskiTriangle(TNode depth,
        goblinController& thisContext = goblinDefaultContext) throw();
};


/// \brief Place a regular grid in the plane

class openGrid : public graph
{
public:

    /// \brief  Open grid type
    enum TOptGrid {
        GRID_TRIANGULAR = 0, ///< Grid is formed of triangular faces
        GRID_SQUARE     = 1, ///< Grid is formed of square faces
        GRID_HEXAGONAL  = 2  ///< Grid is formed of hexagonal faces
    };

    openGrid(TNode _k,TNode _l,TOptGrid shape,
        goblinController& thisContext = goblinDefaultContext) throw(ERRejected);

};


/// \brief Place a regular grid on a sphere

class polarGrid : public graph
{
public:

    /// \brief  Spheric grid type
    enum TOptPolar {
        POLAR_DEFAULT    = 0, ///< Use default options
        POLAR_TRIANGULAR = 1, ///< Grid is formed of triangles
        POLAR_SQUARE     = 2, ///< Grid is formed of squares
        POLAR_CONE       = 3, ///< Grid is embedded on a cone
        POLAR_HEMISPHERE = 4, ///< Grid is embedded on a hemisphere
        POLAR_TUBE       = 5  ///< Grid is embedded 2d instead of 3d
    };

    polarGrid(TNode _k,TNode _l,TNode _p,TOptPolar facets,TOptPolar dim,
        goblinController& thisContext = goblinDefaultContext) throw(ERRejected);

};

/// @}


/// \addtogroup groupGraphComposition
/// @{

/// \brief Place a regular grid on the surface of a torus

class torusGrid : public graph
{
public:

    /// \brief  Torus grid type
    enum TOptTorus {
        TORUS_DEFAULT    = 0, ///< Use default options
        TORUS_TRIANGULAR = 1, ///< Grid is formed of triangles
        TORUS_SQUARE     = 2, ///< Grid is formed of rectangles
        TORUS_HEXAGONAL  = 3, ///< Grid is formed of hexagonal faces
        TORUS_HEMISPHERE = 4  ///< Grid is embedded on a hemisphere only
    };

    torusGrid(TNode _k,TNode _l,TOptTorus facets,TOptTorus dim,
        goblinController& thisContext = goblinDefaultContext) throw(ERRejected);

};

/// @}


#endif
