
#ifdef __GNUC__
#pragma interface
#endif

#ifndef _math_symmetry_pointgrp_h
#define _math_symmetry_pointgrp_h

#include <stdio.h>

#include <util/class/class.h>
#include <util/state/state.h>
#include <util/keyval/keyval.h>
#include <math/topology/point.h>

////////////////////////////////////////////////////////////////////

//.  The \clsnm{SymmetryOperation} class provides a 3 by 3 matrix
//representation of a symmetry operation, such as a rotation or reflection.
class SymmetryOperation {
  private:
    double d[3][3];

  public:
    SymmetryOperation();
    ~SymmetryOperation();

    //. returns the trace of the transformation matrix
    double trace() const { return d[0][0]+d[1][1]+d[2][2]; }

    //. returns the i'th row of the transformation matrix
    double* operator[](int i) { return d[i]; }

    //. const version of the above
    const double* operator[](int i) const { return d[i]; }

    //. returns a reference to the (i,j)th element of the transformation
    // matrix
    double& operator()(int i, int j) { return d[i][j]; }

    //. const version of the above
    const double operator()(int i, int j) const { return d[i][j]; }

    //. zero out the symop
    void zero() { memset(d,0,sizeof(double)*9); }

    //. This operates on \srccd{this} with r (i.e. return r * \srccd{this}).
    SymmetryOperation operate(const SymmetryOperation& r) const;

    //. This performs the transform r * \srccd{this} * r~
    SymmetryOperation sim_transform(const SymmetryOperation& r) const;
    
    //. Set equal to a unit matrix
    void unit() { zero(); d[0][0] = d[1][1] = d[2][2] = 1.0; }

    //. Set equal to E
    void E() { unit(); }
    
    //. Set equal to an inversion
    void i() { zero(); d[0][0] = d[1][1] = d[2][2] = -1.0; }

    //. Set equal to reflection in xy plane
    void sigma_h() { unit(); d[2][2] = -1.0; }

    //. Set equal to reflection in xz plane
    void sigma_xz() { unit(); d[1][1] = -1.0; }

    //. Set equal to reflection in yz plane
    void sigma_yz() { unit(); d[0][0] = -1.0; }

    //. Set equal to a clockwise rotation by 2pi/n
    void rotation(int n);
    void rotation(double theta);
    
    //. Set equal to C2 about the x axis
    void c2_x() { i(); d[0][0] = 1.0; }

    //. Set equal to C2 about the x axis
    void c2_y() { i(); d[1][1] = 1.0; }

    //. print the matrix 
    void print(FILE* =stdout) const;
};

////////////////////////////////////////////////////////////////////

//.  The \clsnm{SymRep} class provides an n dimensional matrix
//representation of a symmetry operation, such as a rotation or reflection.
//The trace of a \clsnm{SymRep} can be used as the character for that
//symmetry operation.  d is hardwired to 5x5 since the H irrep in Ih is 5
//dimensional.
class SymRep {
  private:
    int n;
    double d[5][5];

  public:
    SymRep(int =0);
    SymRep(const SymmetryOperation&);
    ~SymRep();

    //. Cast to a SymmetryOperation.
    operator SymmetryOperation() const;
    
    //. returns the trace of the transformation matrix
    inline double trace() const;

    //. set the dimension of d
    void set_dim(int i) { n=i; }
    
    //. returns the i'th row of the transformation matrix
    double* operator[](int i) { return d[i]; }
    //. const version of the above
    const double* operator[](int i) const { return d[i]; }

    //. returns a reference to the (i,j)th element of the transformation
    // matrix
    double& operator()(int i, int j) { return d[i][j]; }
    //. const version of the above
    const double operator()(int i, int j) const { return d[i][j]; }

    //. zero out the symop
    void zero() { memset(d,0,sizeof(double)*25); }

    //. This operates on \srccd{this} with r (i.e. return r * \srccd{this}).
    SymRep operate(const SymRep& r) const;

    //. This performs the transform r * \srccd{this} * r~
    SymRep sim_transform(const SymRep& r) const;
    
    //. Set equal to a unit matrix
    void unit() {
      zero(); d[0][0] = d[1][1] = d[2][2] = d[3][3] = d[4][4] = 1.0;
    }
    
    //. Set equal to the identity
    void E() { unit(); }
    
    //. Set equal to an inversion
    void i() { zero(); d[0][0] = d[1][1] = d[2][2] = d[3][3] = d[4][4] = -1.0;}

    //. Set equal to reflection in xy plane
    void sigma_h();

    //. Set equal to reflection in xz plane
    void sigma_xz();

    //. Set equal to reflection in yz plane
    void sigma_yz();

    //. Set equal to a clockwise rotation by 2pi/n
    void rotation(int n);
    void rotation(double theta);
    
    //. Set equal to C2 about the x axis
    void c2_x();

    //. Set equal to C2 about the x axis
    void c2_y();

    //. print the matrix 
    void print(FILE* =stdout) const;
};

inline double
SymRep::trace() const
{
  double r=0;
  for (int i=0; i < n; i++)
    r += d[i][i];
  return r;
}

////////////////////////////////////////////////////////////////////


class CharacterTable;

//.  The \clsnm{IrreducibleRepresentation} class provides information
//associated with a particular irreducible representation of a point group.
//This includes the Mulliken symbol for the irrep, the degeneracy of the
//irrep, the characters which represent the irrep, and the number of
//translations and rotations in the irrep.  The order of the point group is
//also provided (this is equal to the number of characters in an irrep).
class IrreducibleRepresentation {
  friend class CharacterTable;

  private:
    int g;         // the order of the group
    int degen;     // the degeneracy of the irrep
    int nrot_;     // the number of rotations in this irrep
    int ntrans_;   // the number of translations in this irrep
    int complex_;  // true if this irrep has a complex representation
    char *symb;    // mulliken symbol for this irrep

    SymRep *rep;   // representation matrices for the symops

  public:
    IrreducibleRepresentation();
    IrreducibleRepresentation(const IrreducibleRepresentation&);
    //. This constructor takes as arguments the order of the point group,
    // the degeneracy of the irrep, and the Mulliken symbol of the irrep.
    // The Mulliken symbol is copied internally.
    IrreducibleRepresentation(int,int,const char*);

    ~IrreducibleRepresentation();

    IrreducibleRepresentation& operator=(const IrreducibleRepresentation&);

    //. Initialize the order, degeneracy, and Mulliken symbol of the irrep.
    void init(int =0, int =0, const char* =0);
    
    //. Returns the order of the group.
    int order() const { return g; }

    //. Returns the degeneracy of the irrep.
    int degeneracy() const { return degen; }

    //. Returns the value of \srccd{complex\_}.
    int complex() const { return complex_; }

    //. Returns the number of projection operators for the irrep.
    int nproj() const { return degen*degen; }

    //. Returns the number of rotations associated with the irrep.
    int nrot() const { return nrot_; }

    //. Returns the number of translations associated with the irrep.
    int ntrans() const { return ntrans_; }

    //. Returns the Mulliken symbol for the irrep.
    const char * symbol() const { return symb; }

    //.
    // Returns the character for the i'th symmetry operation of the point
    // group.
    double character(int i) const {
      return complex_ ? 0.5*rep[i].trace() : rep[i].trace();
    }

    //.
    // Returns the element (x1,x2) of the i'th representation matrix.
    double p(int x1, int x2, int i) const { return rep[i](x1,x2); }
    
    //.
    // Returns the character for the d'th contribution to the i'th 
    // representation matrix.
    double p(int d, int i) const {
      int dc=d/degen; int dr=d%degen;
      return rep[i](dr,dc);
    }

    //.
    // This prints the irrep to the given file, or stdout if none is given.
    // The second argument is an optional string of spaces to offset by.
    void print(FILE* =stdout, const char * =" ");
};

/////////////////////////////////////////////////////////////
//.
// The \clsnm{CharacterTable} class provides a workable character table for
// all of the non-cubic point groups.  While I have tried to match the
// ordering in Cotton's book, I don't guarantee that it is always followed.
// It shouldn't matter anyway.  Also note that I don't lump symmetry operations
// of the same class together.  For example, in C3v there are two distinct
// C3 rotations and 3 distinct reflections, each with a separate character.
// Thus symop has 6 elements rather than the 3 you'll find in most published
// character tables.
class CharacterTable {
  public:
    enum pgroups {C1, CS, CI, CN, CNV, CNH, DN, DND, DNH, SN, T, TH, TD, O,
                  OH, I, IH};

  private:
    int g;                               // the order of the point group
    int nt;                              // order of the princ rot axis
    pgroups pg;                          // the class of the point group
    int nirrep_;                         // the number of irreps in this pg
    IrreducibleRepresentation *gamma_;   // an array of irreps
    SymmetryOperation *symop;            // the matrices describing sym ops
    int *_inv;                           // index of the inverse symop
    char *symb;                          // the Schoenflies symbol for the pg

    //. this determines what type of point group we're dealing with
    int parse_symbol();
    //. this fills in the irrep and symop arrays.
    int make_table();

    // these create the character tables for the cubic groups
    void t();
    void th();
    void td();
    void o();
    void oh();
    void i();
    void ih();

  public:
    CharacterTable();
    //. This constructor takes the Schoenflies symbol of a point group as
    // input.
    CharacterTable(const char*);
    //. This is like the above, but it also takes a reference to a
    //\clsnmref{SymmetryOperation} which is the frame of reference.  All
    //symmetry operations are transformed to this frame of reference.
    CharacterTable(const char*,const SymmetryOperation&);
    CharacterTable(const CharacterTable&);
    ~CharacterTable();

    CharacterTable& operator=(const CharacterTable&);

    //. Returns the number of irreps.
    int nirrep() const { return nirrep_; }
    //. Returns the order of the point group
    int order() const { return g; }
    //. Returns the Schoenflies symbol for the point group
    const char * symbol() const { return symb; }
    //. Returns the i'th irrep.
    IrreducibleRepresentation& gamma(int i) { return gamma_[i]; }
    //. Returns the i'th symmetry operation.
    SymmetryOperation& symm_operation(int i) { return symop[i]; }

    //. Cn, Cnh, Sn, T, and Th point groups have complex representations.
    // This function returns 1 if the point group has a complex representation,
    // 0 otherwise.
    int complex() const {
      if (pg==CN || pg==SN || pg==CNH || pg==T || pg==TH)
        return 1;
      return 0;
    }

    //. Returns the index of the symop which is the inverse of symop[i].
    int inverse(int i) const { return _inv[i]; }
    
    //. Returns sum\_ir degen(ir);
    int ncomp() const {
      int ret=0;
      for (int i=0; i < nirrep_; i++) {
        int nc = (gamma_[i].complex()) ? 1 : gamma_[i].degen;
        ret += nc;
      }
      return ret;
    }

    //. Returns the irrep component i belongs to.
    int which_irrep(int i) {
      for (int ir=0, cn=0; ir < nirrep_; ir++) {
        int nc = (gamma_[ir].complex()) ? 1 : gamma_[ir].degen;
        for (int c=0; c < nc; c++,cn++)
          if (cn==i)
            return ir;
      }
      return -1;
    }

    //. Returns the which compenent i is.
    int which_comp(int i) {
      for (int ir=0, cn=0; ir < nirrep_; ir++) {
        int nc = (gamma_[ir].complex()) ? 1 : gamma_[ir].degen;
        for (int c=0; c < nc; c++,cn++)
          if (cn==i)
            return c;
      }
      return -1;
    }
    
    //. 
    // This prints the irrep to the given file, or stdout if none is given.
    // The second argument is an optional string of spaces to offset by.
    void print(FILE* =stdout, const char * =" ");
};

/////////////////////////////////////////////////////////////

//.
// The \clsnm{PointGroup} class is really a place holder for a
// \clsnmref{CharacterTable}.  It contains a string representation of the
// Schoenflies symbol of a point group, a frame of reference for the
// symmetry operation transformation matrices, and a point of origin.
// The origin may one day disappear...I once thought it would be useful.
// Maybe I'll remember why some day.
//
// \clsnm{PointGroup} is the only class in libsymmetry which is a
// \clsnmref{SavableState}.  I did this to save space...it takes less than a
// second to generate a character table from the Schoenflies symbol, but
// the character table for Ih will occupy a great deal of memory.  Best to
// free up that memory when it's not needed.
class PointGroup: public SavableState {
#   define CLASSNAME PointGroup
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
  private:
    char *symb;
    SymmetryOperation frame;
    Point origin_;

  public:
    PointGroup();
    //. This constructor takes a string containing the Schoenflies symbol
    // of the point group as its only argument.
    PointGroup(const char*);
    //. Like the above, but this constructor also takes a frame of reference
    // as an argument.
    PointGroup(const char*,SymmetryOperation&);
    //. Like the above, but this constructor also takes a point of origin
    // as an argument.
    PointGroup(const char*,SymmetryOperation&,Point&);
    //. The \clsnmref{KeyVal} constructor.
    PointGroup(const RefKeyVal&);

    PointGroup(StateIn&);
    PointGroup(PointGroup&);
    ~PointGroup();

    PointGroup& operator=(const PointGroup&);

    //. Returns the \clsnmref{CharacterTable} for this point group.
    CharacterTable char_table() const;
    //. Returns the Schoenflies symbol for this point group.
    const char * symbol() const { return symb; }
    //. Returns the frame of reference for this point group.
    SymmetryOperation& symm_frame() { return frame; }
    //. A const version of the above
    const SymmetryOperation& symm_frame() const { return frame; }
    //. Returns the origin of the symmetry frame.
    Point& origin() { return origin_; }
    //. A const version of the above.
    const Point& origin() const { return origin_; }

    //. Sets (or resets) the Schoenflies symbol.
    void set_symbol(const char*);

    void save_data_state(StateOut& so);
};

#endif
