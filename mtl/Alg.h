/*******************************************************************************************[Alg.h]
MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Alg_h
#define Alg_h

#include <iostream>
#include "Vec.h"
#include "../Solver/SolverTypes.h"
#include "../Solver/Watched.h"

#ifdef _MSC_VER
#include <msvc/stdint.h>
#else
#include <stdint.h>
#endif //_MSC_VER

//=================================================================================================
// Useful functions on vectors

template<class V, class T>
static inline void remove(V& ts, const T& t)
{
    uint32_t j = 0;
    for (; j < ts.size() && ts[j] != t; j++);
    assert(j < ts.size());
    for (; j < (uint16_t)(ts.size()-1); j++) ts[j] = ts[j+1];
    ts.resize(ts.size() -1);
}

template<class V>
static inline uint32_t removeAll(V& ts, const Var t)
{
    typedef typename V::iterator myiter;
    myiter i = ts.begin();
    myiter j = i;
    for (myiter end = ts.end(); i != end; i++) {
        if (i->var() != t) {
            *j++ = *i;
        }
    }
    ts.resize(ts.size() - (i-j));

    return (i-j);
}

template<class V, class T>
static inline void removeW(V& ts, const T& t)
{
    uint32_t j = 0;
    for (; j < ts.size() && ts[j] != t; j++);
    assert(j < ts.size());
    for (; j < ts.size()-1; j++) ts[j] = ts[j+1];
    ts.pop_back();
}

template<class V, class T>
static inline bool find(V& ts, const T& t)
{
    uint32_t j = 0;
    for (; j < ts.size() && ts[j] != t; j++);
    return j < ts.size();
}

template<class V, class T>
static inline bool findW(V& ts, const T& t)
{
    uint32_t j = 0;
    for (; j < ts.size() && ts[j].clause != t; j++);
    return j < ts.size();
}

//////////////////
// NORMAL Clause
//////////////////
static inline bool findWCl(const vec<Watched>& ws, const ClauseOffset c)
{
    vec<Watched>::const_iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isClause() || i->getNormOffset() != c); i++);
    return i != end;
}

static inline void removeWCl(vec<Watched> &ws, const ClauseOffset c)
{
    vec<Watched>::iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isClause() || i->getNormOffset() != c); i++);
    assert(i != end);
    vec<Watched>::iterator j = i;
    i++;
    for (; i != end; j++, i++) *j = *i;
    ws.shrink_(1);
}

//////////////////
// TRI Clause
//////////////////

static inline bool findWTri(const vec<Watched> &ws, const Lit lit1, const Lit lit2)
{
    vec<Watched>::const_iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isTriClause() || i->getOtherLit() != lit1 || i->getOtherLit2() != lit2); i++);
    return i != end;
}

static inline void removeWTri(vec<Watched> &ws, const Lit lit1, const Lit lit2)
{
    vec<Watched>::iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isTriClause() || i->getOtherLit() != lit1 || i->getOtherLit2() != lit2); i++);
    assert(i != end);
    vec<Watched>::iterator j = i;
    i++;
    for (; i != end; j++, i++) *j = *i;
    ws.shrink_(1);
}

//////////////////
// BINARY Clause
//////////////////
static inline bool findWBin(const std::vector<vec<Watched> >& wsFull, const Lit lit1, const Lit impliedLit)
{
    vec<Watched>::const_iterator i = wsFull[(~lit1).toInt()].begin();
    vec<Watched>::const_iterator end = wsFull[(~lit1).toInt()].end();
    for (; i != end && (!i->isBinary() || i->getOtherLit() != impliedLit); i++);
    return i != end;
}

static inline bool findWBin(const std::vector<vec<Watched> >& wsFull, const Lit lit1, const Lit impliedLit, const bool learnt)
{
    vec<Watched>::const_iterator i = wsFull[(~lit1).toInt()].begin();
    vec<Watched>::const_iterator end = wsFull[(~lit1).toInt()].end();
    for (; i != end && (!i->isBinary() || i->getOtherLit() != impliedLit || i->getLearnt() != learnt); i++);
    return i != end;
}

static inline void removeWBin(vec<Watched> &ws, const Lit impliedLit, const bool learnt)
{
    vec<Watched>::iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isBinary() || i->getOtherLit() != impliedLit || i->getLearnt() != learnt); i++);
    assert(i != end);
    vec<Watched>::iterator j = i;
    i++;
    for (; i != end; j++, i++) *j = *i;
    ws.shrink_(1);
}

static inline void removeWBin(std::vector<vec<Watched> > &watches, const Lit lit1, const Lit impliedLit, const bool learnt)
{
    vec<Watched>& ws = watches[(~lit1).toInt()];
    vec<Watched>::iterator i = ws.begin(), end = ws.end();
    for (; i != end && (!i->isBinary() || i->getOtherLit() != impliedLit || i->getLearnt() != learnt); i++);
    assert(i != end);
    vec<Watched>::iterator j = i;
    i++;
    for (; i != end; j++, i++) *j = *i;
    ws.shrink_(1);
}

static inline const std::pair<uint32_t, uint32_t>  removeWBinAll(vec<Watched> &ws, const Lit impliedLit)
{
    uint32_t removedLearnt = 0;
    uint32_t removedNonLearnt = 0;

    vec<Watched>::iterator i = ws.begin();
    vec<Watched>::iterator j = i;
    for (vec<Watched>::iterator end = ws.end(); i != end; i++) {
        if (!i->isBinary() || i->getOtherLit() != impliedLit)
            *j++ = *i;
        else {
            if (i->getLearnt())
                removedLearnt++;
            else
                removedNonLearnt++;
        }
    }
    ws.shrink_(i-j);

    return std::make_pair(removedLearnt, removedNonLearnt);
}

static inline Watched& findWatchedOfBin(std::vector<vec<Watched> >& wsFull, const Lit lit1, const Lit lit2, const bool learnt)
{
    vec<Watched>& ws = wsFull[(~lit1).toInt()];
    for (vec<Watched>::iterator i = ws.begin(), end = ws.end(); i != end; i++) {
        if (i->isBinary() && i->getOtherLit() == lit2 && i->getLearnt() == learnt)
            return *i;
    }

    assert(false);
    return *ws.begin();
}

static inline Watched& findWatchedOfBin(std::vector<vec<Watched> >& wsFull, const Lit lit1, const Lit lit2)
{
    vec<Watched>& ws = wsFull[(~lit1).toInt()];
    for (vec<Watched>::iterator i = ws.begin(), end = ws.end(); i != end; i++) {
        if (i->isBinary() && i->getOtherLit() == lit2)
            return *i;
    }

    assert(false);
    return *ws.begin();
}

#endif
