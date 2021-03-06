//
// lselect.h
//
// Copyright (C) 2007 Edward Valeev
//
// Author: Edward Valeev <evaleev@vt.edu>
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

#ifndef _chemistry_qc_basis_lselect_h
#define _chemistry_qc_basis_lselect_h

#include <chemistry/qc/basis/gaussbas.h>

namespace sc {

/** The LSelectBasisSet class is used to select shells by angular momentum from a mother basis.
*/
class LSelectBasisSet: public GaussianBasisSet {

  public:

    /** The KeyVal constructor.  It does not call the basis class
        KeyVal constructor.

        <dl>

        <dt><tt>basis</tt><dd> The gives the GaussianBasisSet object
        that will be used to form this basis set (the mother basis).
        If <tt>basis</tt> is not given, the constructor will try to construct
        a GaussianBasisSet object from the current input and use it as
        the mother basis.


        <dt><tt>lmin</tt><dd> Shells of this angular momentum and higher
	will be used in this basis. Default is 0.

        <dt><tt>lmax</tt><dd> Shells of this angular momentum and lower
	will be used in this basis. Default is the maximum angular momentum
	of the mother basis.

        <dt><tt>l</tt><dd> This array specifies the angular momenta which will
	be used in this basis. If given, this keyword overrides <tt>lmin</tt>
	and <tt>lmax</tt>.

        </dl>

        */
    LSelectBasisSet(const Ref<KeyVal>&);

    /** LSelect the given GaussianBasisSet object. */
    LSelectBasisSet(const Ref<GaussianBasisSet>&);

    LSelectBasisSet(StateIn&);

    void save_data_state(StateOut&);

  private:

    /// the angular momenta selected from the mother basis
    std::vector<unsigned int> l_;

    void lselect(const Ref<GaussianBasisSet>&);
    
    // set to 1 to debug
    static int debug() { return 0; }

};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
