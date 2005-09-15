/**CFile****************************************************************

  FileName    [abcs.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Sequential synthesis package.]

  Synopsis    [External declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: abcs.h,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#ifndef __ABCS_H__
#define __ABCS_H__

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "abc.h"

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

// the maximum number of latches on the edge
#define ABC_MAX_EDGE_LATCH   16   

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

// representation of latch on the edge
typedef struct Abc_RetEdge_t_       Abc_RetEdge_t;   
struct Abc_RetEdge_t_ // 1 word
{
    unsigned         iNode    : 24;  // the ID of the node
    unsigned         iEdge    :  1;  // the edge of the node
    unsigned         iLatch   :  7;  // the latch number counting from the node
};

// representation of one retiming step
typedef struct Abc_RetStep_t_       Abc_RetStep_t;   
struct Abc_RetStep_t_ // 1 word
{
    unsigned         iNode    : 24;  // the ID of the node
    unsigned         nLatches :  8;  // the number of latches to retime
};

static inline int            Abc_RetEdge2Int( Abc_RetEdge_t Str )  { return *((int *)&Str);           }
static inline Abc_RetEdge_t  Abc_Int2RetEdge( int Num )            { return *((Abc_RetEdge_t *)&Num); }

static inline int            Abc_RetStep2Int( Abc_RetStep_t Str )  { return *((int *)&Str);           }
static inline Abc_RetStep_t  Abc_Int2RetStep( int Num )            { return *((Abc_RetStep_t *)&Num); }

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFITIONS                             ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DEFINITIONS                          ///
////////////////////////////////////////////////////////////////////////

// getting the number of the edge for this fanout
static inline int Abc_ObjEdgeNum( Abc_Obj_t * pObj, Abc_Obj_t * pFanout )  
{
    assert( Abc_NtkIsSeq(pObj->pNtk) );
    if ( Abc_ObjFaninId0(pFanout) == (int)pObj->Id )
        return 0;
    else if ( Abc_ObjFaninId1(pFanout) == (int)pObj->Id )
        return 1;
    assert( 0 );
    return -1;
}

// getting the latch number of the fanout
static inline int Abc_ObjFanoutL( Abc_Obj_t * pObj, Abc_Obj_t * pFanout )
{
    return Abc_ObjFaninL( pFanout, Abc_ObjEdgeNum(pObj,pFanout) );
}

// setting the latch number of the fanout
static inline void Abc_ObjSetFanoutL( Abc_Obj_t * pObj, Abc_Obj_t * pFanout, int nLats )  
{ 
    Abc_ObjSetFaninL( pFanout, Abc_ObjEdgeNum(pObj,pFanout), nLats );
}

// adding to the latch number of the fanout
static inline void Abc_ObjAddFanoutL( Abc_Obj_t * pObj, Abc_Obj_t * pFanout, int nLats )  
{ 
    Abc_ObjAddFaninL( pFanout, Abc_ObjEdgeNum(pObj,pFanout), nLats );
}

// returns the latch number of the fanout
static inline int Abc_ObjFanoutLMax( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nLatchCur, nLatchRes;
    nLatchRes = 0;
    Abc_ObjForEachFanout( pObj, pFanout, i )
    {
        nLatchCur = Abc_ObjFanoutL(pObj, pFanout);
        if ( nLatchRes < nLatchCur )
            nLatchRes = nLatchCur;
    }
    assert( nLatchRes >= 0 );
    return nLatchRes;
}

// returns the latch number of the fanout
static inline int Abc_ObjFanoutLMin( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nLatchCur, nLatchRes;
    nLatchRes = ABC_INFINITY;
    Abc_ObjForEachFanout( pObj, pFanout, i )
    {
        nLatchCur = Abc_ObjFanoutL(pObj, pFanout);
        if ( nLatchRes > nLatchCur )
            nLatchRes = nLatchCur;
    }
    assert( nLatchRes < ABC_INFINITY );
    return nLatchRes;
}

// returns the sum of latches on the fanout edges
static inline int Abc_ObjFanoutLSum( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nSum = 0;
    Abc_ObjForEachFanout( pObj, pFanout, i )
        nSum += Abc_ObjFanoutL(pObj, pFanout);
    return nSum;
}

// returns the sum of latches on the fanin edges
static inline int Abc_ObjFaninLSum( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanin;
    int i, nSum = 0;
    Abc_ObjForEachFanin( pObj, pFanin, i )
        nSum += Abc_ObjFaninL(pObj, i);
    return nSum;
}


// getting the bit-string of init values of the edge
static inline unsigned Abc_ObjFaninLGetInit( Abc_Obj_t * pObj, int Edge )  
{ 
    return (unsigned)Vec_IntEntry( pObj->pNtk->vInits, 2 * pObj->Id + Edge );
}

// setting bit-string of init values of the edge
static inline void Abc_ObjFaninLSetInit( Abc_Obj_t * pObj, int Edge, unsigned Init )  
{ 
    Vec_IntWriteEntry( pObj->pNtk->vInits, 2 * pObj->Id + Edge, Init );
}

// getting the init value of the given latch on the edge
static inline Abc_InitType_t Abc_ObjFaninLGetInitOne( Abc_Obj_t * pObj, int Edge, int iLatch )  
{
    return 0x3 & (Abc_ObjFaninLGetInit(pObj, Edge) >> (2*iLatch));
}

// setting the init value of the given latch on the edge
static inline void Abc_ObjFaninLSetInitOne( Abc_Obj_t * pObj, int Edge, int iLatch, Abc_InitType_t Init )  
{
    unsigned EntryCur = Abc_ObjFaninLGetInit(pObj, Edge);
    unsigned EntryNew = (EntryCur & ~(0x3 << (2*iLatch))) | (Init << (2*iLatch));
    assert( iLatch < Abc_ObjFaninL(pObj, Edge) );
    Abc_ObjFaninLSetInit( pObj, Edge, EntryNew );
}

// geting the init value of the first latch on the edge
static inline Abc_InitType_t Abc_ObjFaninLGetInitFirst( Abc_Obj_t * pObj, int Edge )  
{
    return 0x3 & Abc_ObjFaninLGetInit( pObj, Edge );
}

// geting the init value of the last latch on the edge
static inline Abc_InitType_t Abc_ObjFaninLGetInitLast( Abc_Obj_t * pObj, int Edge )  
{
    assert( Abc_ObjFaninL(pObj, Edge) > 0 );
    return 0x3 & (Abc_ObjFaninLGetInit(pObj, Edge) >> (2 * (Abc_ObjFaninL(pObj, Edge) - 1)));
}

// insert the first latch on the edge
static inline void Abc_ObjFaninLInsertFirst( Abc_Obj_t * pObj, int Edge, Abc_InitType_t Init )  
{ 
    unsigned EntryCur = Abc_ObjFaninLGetInit(pObj, Edge);
    unsigned EntryNew = ((EntryCur << 2) | Init);
    assert( Init >= 0 && Init < 4 );
    assert( Abc_ObjFaninL(pObj, Edge) < ABC_MAX_EDGE_LATCH );
    Abc_ObjFaninLSetInit( pObj, Edge, EntryNew );
    Abc_ObjAddFaninL( pObj, Edge, 1 );
}

// insert the last latch on the edge
static inline void Abc_ObjFaninLInsertLast( Abc_Obj_t * pObj, int Edge, Abc_InitType_t Init )  
{ 
    unsigned EntryCur = Abc_ObjFaninLGetInit(pObj, Edge);
    unsigned EntryNew = EntryCur | (Init << (2 * Abc_ObjFaninL(pObj, Edge)));
    assert( Init >= 0 && Init < 4 );
    assert( Abc_ObjFaninL(pObj, Edge) < ABC_MAX_EDGE_LATCH );
    Abc_ObjFaninLSetInit( pObj, Edge, EntryNew );
    Abc_ObjAddFaninL( pObj, Edge, 1 );
}

// delete the first latch on the edge
static inline Abc_InitType_t Abc_ObjFaninLDeleteFirst( Abc_Obj_t * pObj, int Edge )  
{ 
    unsigned EntryCur = Abc_ObjFaninLGetInit(pObj, Edge);
    Abc_InitType_t Init = 0x3 & EntryCur;
    unsigned EntryNew = EntryCur >> 2;
    assert( Abc_ObjFaninL(pObj, Edge) > 0 );
    Abc_ObjFaninLSetInit( pObj, Edge, EntryNew );
    Abc_ObjAddFaninL( pObj, Edge, -1 );
    return Init;
}

// delete the last latch on the edge
static inline Abc_InitType_t Abc_ObjFaninLDeleteLast( Abc_Obj_t * pObj, int Edge )  
{ 
    unsigned EntryCur = Abc_ObjFaninLGetInit(pObj, Edge);
    Abc_InitType_t Init = 0x3 & (EntryCur >> (2 * (Abc_ObjFaninL(pObj, Edge) - 1)));
    unsigned EntryNew = EntryCur & ~(((unsigned)0x3) << (2 * (Abc_ObjFaninL(pObj, Edge)-1)));
    assert( Abc_ObjFaninL(pObj, Edge) > 0 );
    Abc_ObjFaninLSetInit( pObj, Edge, EntryNew );
    Abc_ObjAddFaninL( pObj, Edge, -1 );
    return Init;
}

// retime node forward without initial states
static inline void Abc_ObjRetimeForwardTry( Abc_Obj_t * pObj, int nLatches )  
{
    Abc_Obj_t * pFanout;
    int i;
    // make sure it is an AND gate
    assert( Abc_ObjFaninNum(pObj) == 2 );
    // make sure it has enough latches
    assert( Abc_ObjFaninL0(pObj) >= nLatches );
    assert( Abc_ObjFaninL1(pObj) >= nLatches );
    // subtract these latches on the fanin side
    Abc_ObjAddFaninL0( pObj, -nLatches );
    Abc_ObjAddFaninL1( pObj, -nLatches );
    // add these latches on the fanout size
    Abc_ObjForEachFanout( pObj, pFanout, i )
        Abc_ObjAddFanoutL( pObj, pFanout, nLatches );
}

// retime node backward without initial states
static inline void Abc_ObjRetimeBackwardTry( Abc_Obj_t * pObj, int nLatches )  
{
    Abc_Obj_t * pFanout;
    int i;
    // make sure it is an AND gate
    assert( Abc_ObjFaninNum(pObj) == 2 );
    // subtract these latches on the fanout side
    Abc_ObjForEachFanout( pObj, pFanout, i )
    {
        assert( Abc_ObjFanoutL(pObj, pFanout) >= nLatches );
        Abc_ObjAddFanoutL( pObj, pFanout, -nLatches );
    }
    // add these latches on the fanin size
    Abc_ObjAddFaninL0( pObj, nLatches );
    Abc_ObjAddFaninL1( pObj, nLatches );
}


////////////////////////////////////////////////////////////////////////
///                        ITERATORS                                 ///
////////////////////////////////////////////////////////////////////////

// iterating through the initial values of the edge
#define Abc_ObjFaninLForEachValue( pObj, Edge, Init, i, Value ) \
    for ( i = 0, Init = Abc_ObjFaninLGetInit(pObj, Edge); \
          i < Abc_ObjFaninL(pObj, Edge) && ((Value = ((Init >> i) & 3)), 1); i++ ) 

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== abcRetCore.c ===========================================================*/
extern void               Abc_NtkSeqRetimeForward( Abc_Ntk_t * pNtk );
extern void               Abc_NtkSeqRetimeBackward( Abc_Ntk_t * pNtk );
extern void               Abc_NtkSeqRetimeInitial( Abc_Ntk_t * pNtk );
extern void               Abc_NtkSeqRetimeDelay( Abc_Ntk_t * pNtk );
/*=== abcRetDelay.c ==========================================================*/
extern Vec_Str_t *        Abc_NtkSeqRetimeDelayLags( Abc_Ntk_t * pNtk );
/*=== abcRetImpl.c ===========================================================*/
extern int                Abc_NtkImplementRetiming( Abc_Ntk_t * pNtk, Vec_Str_t * vLags );
extern void               Abc_NtkImplementRetimingForward( Abc_Ntk_t * pNtk, Vec_Ptr_t * vMoves );
extern int                Abc_NtkImplementRetimingBackward( Abc_Ntk_t * pNtk, Vec_Ptr_t * vMoves );
/*=== abcRetUtil.c ===========================================================*/
extern Vec_Ptr_t *        Abc_NtkUtilRetimingTry( Abc_Ntk_t * pNtk, bool fForward );
extern Vec_Ptr_t *        Abc_NtkUtilRetimingGetMoves( Abc_Ntk_t * pNtk, Vec_Int_t * vNodes, bool fForward );
extern Vec_Int_t *        Abc_NtkUtilRetimingSplit( Vec_Str_t * vLags, int fForward );
/*=== abcSeq.c ===============================================================*/
extern Abc_Ntk_t *        Abc_NtkAigToSeq( Abc_Ntk_t * pNtk );
extern Abc_Ntk_t *        Abc_NtkSeqToLogicSop( Abc_Ntk_t * pNtk );
/*=== abcShare.c =============================================================*/
extern void               Abc_NtkSeqShareFanouts( Abc_Ntk_t * pNtk );
/*=== abcUtil.c ==============================================================*/
extern int                Abc_NtkSeqLatchNum( Abc_Ntk_t * pNtk );
extern int                Abc_NtkSeqLatchNumShared( Abc_Ntk_t * pNtk );
extern char *             Abc_ObjFaninGetInitPrintable( Abc_Obj_t * pObj, int Edge );

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#endif

