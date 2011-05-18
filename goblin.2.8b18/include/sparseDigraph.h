
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, December 1998
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   sparseDigraph.h
/// \brief  Class interface to sparse directed graph objects

#if !defined(_SPARSE_DIGRAPH_H_)
#define _SPARSE_DIGRAPH_H_

#include "abstractDigraph.h"
#include "sparseRepresentation.h"


/// \brief A class for directed graphs represented with incidence lists

class diGraph : public abstractDiGraph
{
    friend class sparseRepresentation;

private:

    bool mode;

protected:

    sparseRepresentation X;

public:

    /// \brief  Default constructor for sparse digraphs
    ///
    /// \param _n       The initial number of nodes
    /// \param _CT      The context to which this graph object is attached
    /// \param _mode    If true, the graph is not displayed before destruction
    diGraph(TNode _n=0,goblinController& _CT=goblinDefaultContext,
        bool _mode = false) throw();

    /// \brief  File constructor for sparse digraphs
    ///
    /// \param fileName  The source file name
    /// \param _CT       The context to which this graph object is attached
    diGraph(const char* fileName,goblinController& _CT=goblinDefaultContext)
        throw(ERFile,ERParse);

    /// \brief  Copy constructor for sparse digraphs
    ///
    /// \param G        The original mixed graph object
    /// \param options  A combination of OPT_CLONE, OPT_PARALLELS, OPT_MAPPINGS and OPT_SUB
    diGraph(abstractMixedGraph& G,TOption options=0) throw();

    ~diGraph() throw();

    unsigned long   Size() const throw();
    unsigned long   Allocated() const throw();
    virtual objectType ClassID() const throw() {return IDSparseDiGraph;};
    virtual void*   BackCast() const throw() {return (void*)this;};

    #include <sparseInclude.h>

};


/// \addtogroup groupGraphComposition
/// @{

/// \brief Complete orientation of a given mixed graph object
///
/// This digraph replaces all undirected edges of the original mixed graph object
/// by an antiparallel arc pair. If the #OPT_REVERSE option is specified in the
/// constructor call, directed arcs are also duplicated.

class completeOrientation : public diGraph
{
private:

    TArc*   origin;
    char    type;

public:

    completeOrientation(abstractMixedGraph &G,TOption options = 0) throw();
    ~completeOrientation() throw();

    unsigned long   Size() throw();
    unsigned long   Allocated() throw();

    TArc  OriginalArc(TArc a) throw(ERRange);

    /// \brief  Map a subgraph from the original graph to this object
    ///
    /// \param G  A reference of the original graph
    void  MapFlowForward(abstractMixedGraph &G) throw(ERRejected);

    /// \brief  Map a subgraph back to the original graph
    ///
    /// \param G  A reference of the original graph
    void  MapFlowBackward(abstractMixedGraph &G) throw(ERRejected);

};


/// \brief Orientation of a given mixed graph object
///
/// This digraph orientates all arcs and edges of the original mixed graph object
/// so that the start node has a smaller node colour index than the end node. So,
/// if the node colours are pairwise different, a DAG results. If the node colours
/// encode an st-numbering, even a bipolar digraph results.

class inducedOrientation : public diGraph
{
public:

    inducedOrientation(abstractMixedGraph &G,TOption options = 0) throw(ERRejected);

};


/// \brief Split all nodes of a given mixed graph object
///
/// In this digraph, every node of the original graph is replaced by a node pair
/// and a directed arc connecting these two nodes. The original arcs end up at the mapped
/// start nodes and start at the mapped end nodes. Additionally, the original arcs are
/// oriented by applying the same rules as in the #completeOrientation constructor.

class nodeSplitting : public diGraph
{

private:

    abstractMixedGraph& G;
    bool mapUnderlying;
    bool mapCapacities;

public:

    /// \param _G       The original mixed graph
    /// \param options  A combination of TOptNodeSplitting values
    nodeSplitting(abstractMixedGraph& _G,TOption options=0) throw();

    void MapEdgeCut() throw();

};


/// \brief Place a regular tree

class regularTree : public diGraph
{
public:

    regularTree(TNode _depth,TNode deg,TNode _n = NoNode,
        goblinController& thisContext = goblinDefaultContext) throw(ERRejected);

};

/// @}


/// \addtogroup groupPlanarComposition
/// @{

/// \brief Directed duals of planar bipolar digraphs
///
/// The nodes of the dual graph are the faces in the original graph. Every edge
/// of the original graph is associated with an edge in the dual graph where it
/// connects the nodes mapped from the faces which where originally separated by
/// this arc.
///
/// As a specialization of the undirected dual graph, the arc orientations are
/// set according to a st-numbering given in advance.

class directedDual : public diGraph
{
public:

    directedDual(abstractMixedGraph &G,TOption options = 0) throw(ERRejected);

};

/// @}


/// \addtogroup groupDagComposition
/// @{

/// \brief The minimal transitive supergraph of a given DAG
///
/// This digraph adds to the original digraph all edges a=uv for which a directed
/// uv-path exists.

class transitiveClosure : public diGraph
{
public:

    transitiveClosure(abstractDiGraph &G,TOption options = 0) throw(ERRejected);

};


/// \brief The maximal subgraph of a given DAG which does not have transitive edges
///
/// This digraph eliminates from the original digraph all edges a=uv for which a
/// non-trivial directed uv-path can be found.

class intransitiveReduction : public diGraph
{
public:

    intransitiveReduction(abstractDiGraph &G,TOption options = 0) throw(ERRejected);

};

/// @}


#endif
