//
// p4.cc
//
// Copyright (C) 2001 Edward Valeev
//
// Author: Edward Valeev <evaleev@vt.edu>
// Maintainer: EV
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

#include <util/misc/math.h>

#include <chemistry/qc/libint2/int1e.h>
#include <chemistry/qc/libint2/macros.h>

using namespace sc;

void Int1eLibint2::p4(int sh1, int sh2) {
  zero_buffers_();
  compute_doublet_info_(sh1, sh2);
  
  int maxam1 = int_shell1_->max_am();
  int minam1 = int_shell1_->min_am();
  int maxam2 = int_shell2_->max_am();
  int minam2 = int_shell2_->min_am();
  
  p4_full_general_();
}

void Int1eLibint2::p4_full_general_() {
  int maxam1 = int_shell1_->max_am();
  int maxam2 = int_shell2_->max_am();
  
  /* See if need to transform to spherical harmonics */
  bool need_cart2sph_transform = false;
  if (int_shell1_->has_pure() || int_shell2_->has_pure())
    need_cart2sph_transform = true;
  
  /* See if contraction quartets need to be resorted into a shell quartet */
  bool need_sort_to_shell_doublet = false;
  int num_gen_shells = 0;
  if (int_shell1_->ncontraction() > 1)
    num_gen_shells++;
  if (int_shell2_->ncontraction() > 1)
    num_gen_shells++;
  if (maxam1 + maxam2 && num_gen_shells >= 1)
    need_sort_to_shell_doublet = true;
  
  /* Determine where integrals need to go at each stage */
  if (need_sort_to_shell_doublet) {
    prim_ints_ = cart_ints_;
    if (need_cart2sph_transform)
      contr_doublets_ = sphharm_ints_;
    else
      contr_doublets_ = cart_ints_;
    shell_doublet_ = target_ints_buffer_;
  } else {
    if (need_cart2sph_transform) {
      prim_ints_ = cart_ints_;
      contr_doublets_ = target_ints_buffer_;
      shell_doublet_ = target_ints_buffer_;
    } else {
      prim_ints_ = target_ints_buffer_;
      shell_doublet_ = target_ints_buffer_;
    }
  }
  
  /* Begin loops over primitives. */
  for (int p1=0; p1<int_shell1_->nprimitive(); p1++) {
    double a1 = int_shell1_->exponent(p1);
    for (int p2=0; p2<int_shell2_->nprimitive(); p2++) {
      double a2 = int_shell2_->exponent(p2);
      
      double gamma = a1+a2;
      double oog = 1.0/gamma;
      double over_pf = exp(-a1*a2*doublet_info_.AB2*oog)*sqrt(M_PI*oog)*M_PI
          *oog;
      
      double P[3], PA[3], PB[3], PC[3];
      for (int xyz=0; xyz<3; xyz++) {
        P[xyz] = (a1*doublet_info_.A[xyz] + a2*doublet_info_.B[xyz])*oog;
        PA[xyz] = P[xyz] - doublet_info_.A[xyz];
        PB[xyz] = P[xyz] - doublet_info_.B[xyz];
      }
      
      OI_OSrecurs_(OIX_, OIY_, OIZ_, PA, PB, gamma, maxam1+2, maxam2+2);
      
      /*--- contract each buffer into appropriate location ---*/
      double *ints_buf = prim_ints_;
      for (int gc1=0; gc1<int_shell1_->ncontraction(); gc1++) {
        double norm1 = int_shell1_->coefficient_unnorm(gc1, p1);
        int am1 = int_shell1_->am(gc1);
        for (int gc2=0; gc2<int_shell2_->ncontraction(); gc2++) {
          double norm2 = int_shell2_->coefficient_unnorm(gc2, p2);
          int am2 = int_shell2_->am(gc2);
          double total_pf = over_pf * norm1 * norm2;
          
          int k1, l1, m1, k2, l2, m2;
          FOR_CART(k1,l1,m1,am1)
          FOR_CART(k2,l2,m2,am2)
          double x0 = OIX_[k1][k2];
          double y0 = OIY_[l1][l2];
          double z0 = OIZ_[m1][m2];
          
          /* One-dimensional integrals over kinetic energy operators T */

          double tx = a2*(2*k2+1)*OIX_[k1][k2] - 2*a2*a2*OIX_[k1][k2+2];
          if (k2 >= 2)
            tx -= 0.5*k2*(k2-1)*OIX_[k1][k2-2];
          double ty = a2*(2*l2+1)*OIY_[l1][l2] - 2*a2*a2*OIY_[l1][l2+2];
          if (l2 >= 2)
            ty -= 0.5*l2*(l2-1)*OIY_[l1][l2-2];
          double tz = a2*(2*m2+1)*OIZ_[m1][m2] - 2*a2*a2*OIZ_[m1][m2+2];
          if (m2 >= 2)
            tz -= 0.5*m2*(m2-1)*OIZ_[m1][m2-2];
          
          /* One-dimensional integrals over operators T^2 */

          double t2x = 4*(2*k1+1)*(2*k2+1)*a1*a2*x0 - 8*(2*k1+1)*a1*a2*a2
              *OIX_[k1][k2+2] - 8*(2*k2+1)*a1*a1*a2*OIX_[k1+2][k2]
              + 16*a1*a1*a2*a2*OIX_[k1+2][k2+2];
          if (k1 >= 2)
            t2x += 4*k1*(k1-1)*a2*a2*OIX_[k1-2][k2+2] - 2*k1*(k1-1)*(2
                *k2+1)*a2*OIX_[k1-2][k2];
          if (k2 >= 2)
            t2x += 4*k2*(k2-1)*a1*a1*OIX_[k1+2][k2-2] - 2*(2*k1+1)*k2
                *(k2-1)*a1*OIX_[k1][k2-2];
          if ((k1 >= 2) && (k2 >= 2))
            t2x += k1*(k1-1)*k2*(k2-1)*OIX_[k1-2][k2-2];
          //
          double t2y = 4*(2*l1+1)*(2*l2+1)*a1*a2*y0 - 8*(2*l1+1)*a1*a2*a2
              *OIY_[l1][l2+2] - 8*(2*l2+1)*a1*a1*a2*OIY_[l1+2][l2]
              + 16*a1*a1*a2*a2*OIY_[l1+2][l2+2];
          if (l1 >= 2)
            t2y += 4*l1*(l1-1)*a2*a2*OIY_[l1-2][l2+2] - 2*l1*(l1-1)*(2
                *l2+1)*a2*OIY_[l1-2][l2];
          if (l2 >= 2)
            t2y += 4*l2*(l2-1)*a1*a1*OIY_[l1+2][l2-2] - 2*(2*l1+1)*l2
                *(l2-1)*a1*OIY_[l1][l2-2];
          if ((l1 >= 2) && (l2 >= 2))
            t2y += l1*(l1-1)*l2*(l2-1)*OIY_[l1-2][l2-2];
          //
          double t2z = 4*(2*m1+1)*(2*m2+1)*a1*a2*z0 - 8*(2*m1+1)*a1*a2*a2
              *OIZ_[m1][m2+2] - 8*(2*m2+1)*a1*a1*a2*OIZ_[m1+2][m2]
              + 16*a1*a1*a2*a2*OIZ_[m1+2][m2+2];
          if (m1 >= 2)
            t2z += 4*m1*(m1-1)*a2*a2*OIZ_[m1-2][m2+2] - 2*m1*(m1-1)*(2
                *m2+1)*a2*OIZ_[m1-2][m2];
          if (m2 >= 2)
            t2z += 4*m2*(m2-1)*a1*a1*OIZ_[m1+2][m2-2] - 2*(2*m1+1)*m2
                *(m2-1)*a1*OIZ_[m1][m2-2];
          if ((m1 >= 2) && (m2 >= 2))
            t2z += m1*(m1-1)*m2*(m2-1)*OIZ_[m1-2][m2-2];
          
          *(ints_buf++) += total_pf*(t2x*y0*z0 + x0*t2y*z0 + x0*y0*t2z +
              8*tx*ty*z0 + 8*tx*y0*tz + 8*x0*ty*tz);
          END_FOR_CART
          END_FOR_CART

        }
      }
    }
  }
  
  if (need_cart2sph_transform)
    transform_contrquartets_(prim_ints_, contr_doublets_);
  
  // If not CCA-compliant normalization -- re-normalize all integrals
#if INTEGRALLIBINT2_NORMCONV != INTEGRALLIBINT2_NORMCONV_CCA
  norm_contrcart1_(need_cart2sph_transform ? contr_doublets_ : prim_ints_);
#endif

  if (need_sort_to_shell_doublet)
    sort_contrdoublets_to_shelldoublet_(contr_doublets_, shell_doublet_);
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
