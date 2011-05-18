
//  This file forms part of the GOBLIN C++ Class Library.
//
//  Initially written by Christian Fremuth-Paeger, May 2006
//
//  Copying, compiling, distribution and modification
//  of this source code is permitted only in accordance
//  with the GOBLIN general licence information.

/// \file   switchBaseTypes.h
/// \brief  Inline switch/case code which runs over the possible #attribute template instanciations


switch (table[(*J)].arrayType)
{
    case TYPE_NODE_INDEX:
    {
        attribute<TNode>* A = static_cast<attribute<TNode>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_ARC_INDEX:
    {
        attribute<TArc>* A = static_cast<attribute<TArc>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_FLOAT_VALUE:
    {
        attribute<TFloat>* A = static_cast<attribute<TFloat>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_CAP_VALUE:
    {
        attribute<TCap>* A = static_cast<attribute<TCap>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_INDEX:
    {
        attribute<TIndex>* A = static_cast<attribute<TIndex>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_ORIENTATION:
    {
        attribute<char>* A = static_cast<attribute<char>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_INT:
    {
        attribute<int>* A = static_cast<attribute<int>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_BOOL:
    {
        attribute<bool>* A = static_cast<attribute<bool>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_STRING:
    {
        attribute<TString>* A = static_cast<attribute<TString>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_VAR_INDEX:
    {
        attribute<TVar>* A = static_cast<attribute<TVar>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_RESTR_INDEX:
    {
        attribute<TRestr>* A = static_cast<attribute<TRestr>*>(*I);
        _STATEMENT
        break;
    }
    case TYPE_SPECIAL:
    {
        break;
    }
}
