//
// memory.h
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@limitpt.com>
// Maintainer: LPS
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#ifdef __GNUC__
#pragma interface
#endif

#ifndef _util_group_memory_h
#define _util_group_memory_h

#include <iostream.h>

#include <scconfig.h>
#include <util/class/class.h>
#include <util/group/rnglock.h>

#if 0 // this can be used to catch accidental conversions to int
class distsize_t {
    friend size_t distsize_to_size(const distsize_t &a);
    friend distsize_t operator *(const int &a,const distsize_t &b);
    friend distsize_t operator +(const int &a,const distsize_t &b);
    friend distsize_t operator -(const int &a,const distsize_t &b);
    friend distsize_t operator /(const int &a,const distsize_t &b);
    friend distsize_t operator %(const int &a,const distsize_t &b);
    friend ostream& operator <<(ostream& o, const distsize_t &s);
  private:
    unsigned long long s;
  public:
    distsize_t(): s(999999999999999LL) {}
    distsize_t(int a): s(a) {}
    distsize_t(unsigned int a): s(a) {}
    distsize_t(unsigned long long a): s(a) {}
    distsize_t &operator =(const distsize_t &a)
        { s=a.s; return *this; }
    distsize_t &operator +=(const distsize_t &a)
        { s+=a.s; return *this; }
    distsize_t operator *(const distsize_t &a) const
        { return s*a.s; }
    distsize_t operator +(const distsize_t &a) const
        { return s+a.s; }
    distsize_t operator -(const distsize_t &a) const
        { return s-a.s; }
    distsize_t operator /(const distsize_t &a) const
        { return s/a.s; }
    distsize_t operator %(const distsize_t &a) const
        { return s%a.s; }
    bool operator <(const distsize_t &a) const
        { return s<a.s; }
    bool operator <=(const distsize_t &a) const
        { return s<=a.s; }
    bool operator >(const distsize_t &a) const
        { return s>a.s; }
    bool operator >=(const distsize_t &a) const
        { return s>=a.s; }
    bool operator ==(const distsize_t &a) const
        { return s==a.s; }
    distsize_t operator *(const int &a) const
        { return s*a; }
    distsize_t operator +(const int &a) const
        { return s+a; }
    distsize_t operator -(const int &a) const
        { return s-a; }
    distsize_t operator /(const int &a) const
        { return s/a; }
    distsize_t operator %(const int &a) const
        { return s%a; }
};
inline distsize_t operator *(const int &a,const distsize_t &b)
{ return a*b.s; }
inline distsize_t operator +(const int &a,const distsize_t &b)
{ return a+b.s; }
inline distsize_t operator -(const int &a,const distsize_t &b)
{ return a-b.s; }
inline distsize_t operator /(const int &a,const distsize_t &b)
{ return a/b.s; }
inline distsize_t operator %(const int &a,const distsize_t &b)
{ return a%b.s; }
inline ostream& operator <<(ostream& o, const distsize_t &s) { return o<<s.s; }
inline size_t distsize_to_size(const distsize_t &a) {return a.s;}
#elif defined(HAVE_LONG_LONG)
typedef unsigned long long distsize_t;
typedef long long distssize_t;
inline size_t distsize_to_size(const distsize_t &a) {return a;}
#else
typedef unsigned long distsize_t;
typedef long distssize_t;
inline size_t distsize_to_size(const distsize_t &a) {return a;}
#endif

//. The \clsnm{MemoryGrp} abstract class provides the appearance of global
//. shared memory in a parallel machine.
class MemoryGrp: public DescribedClass {
#define CLASSNAME MemoryGrp
#include <util/class/classda.h>
  private:
    RangeLock locks_;

  protected:
    // derived classes must fill in all these
    // ~MemoryGrp deletes the arrays
    int me_;
    int n_;
    distsize_t *offsets_; // offsets_[n_] is the fence for all data

    int use_locks_;

    // set to nonzero for debugging information
    int debug_;

  public:
    MemoryGrp();
    MemoryGrp(const RefKeyVal&);
    virtual ~MemoryGrp();
    
    //. Returns who I am and how many nodes there are.
    int me() const { return me_; }
    int n() const { return n_; }

    //. Set the size of locally held memory.
    //. When memory is accessed using a global offset counting
    //. starts at node 0 and proceeds up to node \srccd{n()} - 1.
    virtual void set_localsize(int) = 0;
    //. Returns the amount of memory residing locally on \srccd{me()};
    int localsize() { return distsize_to_size(offsets_[me_+1]-offsets_[me_]); }
    //. Returns a pointer to the local data.
    virtual void *localdata() = 0;
    //. Returns the global offset to this node's memory.
    distsize_t localoffset() { return offsets_[me_]; }
    //. Returns the amount of memory residing on \vrbl{node}.
    int size(int node)
        { return distsize_to_size(offsets_[node+1] - offsets_[node]); }
    //. Returns the global offset to \vrbl{node}'s memory.
    distsize_t offset(int node) { return offsets_[node]; }
    //. Returns the sum of all memory allocated on all nodes.
    distsize_t totalsize() { return offsets_[n_]; }

    //. Activate is called before the memory is to be used and
    //. deactivate is called afterwards.  (Necessary to avoid
    //. problems with broken message passing routines.)
    virtual void activate();
    virtual void deactivate();

    //. Locking of memory regions can be turned off if the application
    //. can be sure that only one node has write access to a region
    //. at time.  This might improve performance a bit.  lock must
    //. be called with the same argument on all nodes.
    virtual void lock(int truefalse);

    //. These are should only used by MemoryGrpBuf objects.
    virtual void *obtain_writeonly(distsize_t offset, int size);
    virtual void *obtain_readwrite(distsize_t offset, int size) = 0;
    virtual void *obtain_readonly(distsize_t offset, int size) = 0;
    virtual void release_read(void *data, distsize_t offset, int size) = 0;
    virtual void release_write(void *data, distsize_t offset, int size) = 0;

    virtual void sum_reduction(double *data, distsize_t doffset, int dsize);
    virtual void sum_reduction_on_node(double *data, int doffset, int dsize,
                                       int node = -1);

    //. Synchronizes all the nodes.  Consider using this when the way you
    //. you access memory changes.
    virtual void sync() = 0;

    //. Some memory group implementations don't have access to
    //. real shared memory or even active messages.  Instead, requests
    //. are processed whenever certain memory group routines are called.
    //. This can cause large latencies and buffer overflows.  If this
    //. is a problem, then the catchup member can be called to
    //. process all outstanding requests.
    virtual void catchup();

    //. Prints out information about the object.
    virtual void print(ostream &o = cout) const;

    //. Create a memory group.  This routine looks for a -memorygrp
    //argument, then the environmental variable MEMORYGRP, and, finally,
    //the default \clsnmref{MessageGrp} object to decide which
    //specialization of \clsnm{MemoryGrp} would be appropriate.  The
    //argument to -memorygrp should be either string for a
    //\clsnmref{ParsedKeyVal} constructor or a classname.
    static MemoryGrp* initial_memorygrp(int &argc, char** argv);
    static MemoryGrp* initial_memorygrp();
};
DescribedClass_REF_dec(MemoryGrp);

//. The \clsnm{MemoryGrpBug} class provides access to pieces of the
//. global shared memory that have been obtained with \clsnmref{MemoryGrp}.
//. \clsnm{MemoryGrpBug} is a template class that is parameterized on
//. \srccd{data\_t}.  All lengths and offsets of given in terms
//. of \srccd{sizeof(data\_t)}.
template <class data_t>
class MemoryGrpBuf {
    RefMemoryGrp grp_;
    enum LockType { None, Read, Write };
    LockType locktype_;
    data_t *data_;
    distsize_t offset_;
    int length_;
  public:
    //. Creates a new \clsnm{MemoryGrpBuf} given a \clsnmref{MemoryGrp}
    //. reference.  This is a template class parameterized on
    //. \srccd{data\_t}.
    MemoryGrpBuf(const RefMemoryGrp &);
    //. Request write only access to global memory at the global address
    //. \vrbl{offset} and with size \vrbl{length}.  Writing the same
    //. bit of memory twice without an intervening sync of the MemoryGrp
    //. will have undefined results.
    data_t *writeonly(distsize_t offset, int length);
    //. Request read write access to global memory at the global address
    //. \vrbl{offset} and with size \vrbl{length}.  This will lock the
    //. memory it uses until release is called unless locking has been
    //. turned off in the MemoryGrp object.
    data_t *readwrite(distsize_t offset, int length);
    //. Request read only access to global memory at the global address
    //. \vrbl{offset} and with size \vrbl{length}.  Writing to the
    //. specified region without an intervening sync of the MemoryGrp
    //. will have undefined results.
    const data_t *readonly(distsize_t offset, int length);
    //. These behave like \srccd{writeonly}, \srccd{readwrite}, and
    //. \srccd{readonly}, except the \vrbl{offset} is local to the
    //. node specified by \vrbl{node}.  If \vrbl{node} = -1, then
    //. the local node is used.
    data_t *writeonly_on_node(int offset, int length, int node = -1);
    data_t *readwrite_on_node(int offset, int length, int node = -1);
    const data_t *readonly_on_node(int offset, int length, int node = -1);
    //. Release the access to the chunk of global memory that was
    //. obtained with \srccd{writeonly}, \srccd{readwrite},
    //. \srccd{readonly}, \srccd{writeonly\_on\_node},
    //. \srccd{readwrite\_on\_node}, and \srccd{readonly\_on\_node}.
    void release();
    //. The length of the current bit of memory.
    int length() const { return length_; }
};

#ifndef __GNUG__
#  include <util/group/memory.cc>
#endif

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
