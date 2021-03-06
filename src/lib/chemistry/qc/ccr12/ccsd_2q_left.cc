//
// ccsd_2q_left.cc
//
// Copyright (C) 2009 Toru Shiozaki
//
// Author: Toru Shiozaki <shiozaki.toru@gmail.com>
// Maintainer: TS
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
  
// This is a C++ code generated by SMITH
  
#include <algorithm>
#include <chemistry/qc/ccr12/ccsd_2q_left.h>
#include <chemistry/qc/ccr12/parenthesis2tnum.h>
using namespace sc;
  
  
CCSD_2Q_LEFT::CCSD_2Q_LEFT(CCR12_Info* info):Parenthesis2tNum(info){};
  
  
  
void CCSD_2Q_LEFT::compute_amp(double* a_i0,const long t_h5b,const long t_h6b,const long t_h7b,const long t_h8b,const long t_p1b,const long t_p2b,const long t_p3b,const long t_p4b,const long toggle){
  
in.resize(8);
i1xn.resize(16);
  
if (toggle==2L) {
smith_0_1(a_i0,t_h5b,t_h6b,t_h7b,t_h8b,t_p1b,t_p2b,t_p3b,t_p4b);
}
  
}
  
void CCSD_2Q_LEFT::smith_0_1(double* a_i0,const long t_h5b,const long t_h6b,const long t_h7b,const long t_h8b,const long t_p1b,const long t_p2b,const long t_p3b,const long t_p4b){ 
      
const long perm[36][8]={0,1,2,3,4,5,6,7, 0,1,2,3,4,6,5,7, 0,1,2,3,4,7,5,6, 0,1,2,3,5,6,4,7, 0,1,2,3,5,7,4,6, 0,1,2,3,6,7,4,5, 0,2,1,3,4,5,6,7, 0,2,1,3,4,6,5,7, 0,2,1,3,4,7,5,6, 0,2,1,3,5,6,4,7, 0,2,1,3,5,7,4,6, 0,2,1,3,6,7,4,5, 0,3,1,2,4,5,6,7, 0,3,1,2,4,6,5,7, 0,3,1,2,4,7,5,6, 0,3,1,2,5,6,4,7, 0,3,1,2,5,7,4,6, 0,3,1,2,6,7,4,5, 1,2,0,3,4,5,6,7, 1,2,0,3,4,6,5,7, 1,2,0,3,4,7,5,6, 1,2,0,3,5,6,4,7, 1,2,0,3,5,7,4,6, 1,2,0,3,6,7,4,5, 1,3,0,2,4,5,6,7, 1,3,0,2,4,6,5,7, 1,3,0,2,4,7,5,6, 1,3,0,2,5,6,4,7, 1,3,0,2,5,7,4,6, 1,3,0,2,6,7,4,5, 2,3,0,1,4,5,6,7, 2,3,0,1,4,6,5,7, 2,3,0,1,4,7,5,6, 2,3,0,1,5,6,4,7, 2,3,0,1,5,7,4,6, 2,3,0,1,6,7,4,5}; 
const long t_b[8]={t_h5b,t_h6b,t_h7b,t_h8b,t_p1b,t_p2b,t_p3b,t_p4b}; 
for (long permutation=0L;permutation<36L;++permutation) { 
 const long h5b=t_b[perm[permutation][0]]; 
 const long h6b=t_b[perm[permutation][1]]; 
 const long h7b=t_b[perm[permutation][2]]; 
 const long h8b=t_b[perm[permutation][3]]; 
 const long p1b=t_b[perm[permutation][4]]; 
 const long p2b=t_b[perm[permutation][5]]; 
 const long p3b=t_b[perm[permutation][6]]; 
 const long p4b=t_b[perm[permutation][7]]; 
 bool skip=false; 
 for (long p_p=0L;p_p<permutation;++p_p) { 
  if(h5b==t_b[perm[p_p][0]] 
  && h6b==t_b[perm[p_p][1]] 
  && h7b==t_b[perm[p_p][2]] 
  && h8b==t_b[perm[p_p][3]] 
  && p1b==t_b[perm[p_p][4]] 
  && p2b==t_b[perm[p_p][5]] 
  && p3b==t_b[perm[p_p][6]] 
  && p4b==t_b[perm[p_p][7]]) skip=true; 
 } 
 if (skip) continue; 
 if (!z->restricted() || z->get_spin(h5b)+z->get_spin(h6b)+z->get_spin(h7b)+z->get_spin(h8b)+z->get_spin(p1b)+z->get_spin(p2b)+z->get_spin(p3b)+z->get_spin(p4b)!=16L) { 
  if (z->get_spin(h5b)+z->get_spin(h6b)+z->get_spin(h7b)+z->get_spin(h8b)==z->get_spin(p1b)+z->get_spin(p2b)+z->get_spin(p3b)+z->get_spin(p4b)) { 
   if ((z->get_sym(h5b)^(z->get_sym(h6b)^(z->get_sym(h7b)^(z->get_sym(h8b)^(z->get_sym(p1b)^(z->get_sym(p2b)^(z->get_sym(p3b)^z->get_sym(p4b))))))))==(z->irrep_y()^z->irrep_v())) { 
    long dimc=z->get_range(h5b)*z->get_range(h6b)*z->get_range(h7b)*z->get_range(h8b)*z->get_range(p1b)*z->get_range(p2b)*z->get_range(p3b)*z->get_range(p4b); 
    double* k_c_sort=z->mem()->malloc_local_double(dimc); 
    std::fill(k_c_sort,k_c_sort+(size_t)dimc,0.0); 
    if (z->get_spin(h5b)+z->get_spin(h6b)==z->get_spin(p1b)+z->get_spin(p2b)) { 
     if ((z->get_sym(h5b)^(z->get_sym(h6b)^(z->get_sym(p1b)^z->get_sym(p2b))))==z->irrep_y()) { 
      long h5b_0,h6b_0,p1b_0,p2b_0; 
      z->restricted_4(h5b,h6b,p1b,p2b,h5b_0,h6b_0,p1b_0,p2b_0); 
      long h7b_1,h8b_1,p3b_1,p4b_1; 
      z->restricted_4(h7b,h8b,p3b,p4b,h7b_1,h8b_1,p3b_1,p4b_1); 
      long dim_common=1L; 
      long dima0_sort=z->get_range(h5b)*z->get_range(h6b)*z->get_range(p1b)*z->get_range(p2b); 
      long dima0=dim_common*dima0_sort; 
      long dima1_sort=z->get_range(h7b)*z->get_range(h8b)*z->get_range(p3b)*z->get_range(p4b); 
      long dima1=dim_common*dima1_sort; 
      if (dima0>0L && dima1>0L) { 
       double* k_a0_sort=z->mem()->malloc_local_double(dima0); 
       double* k_a0=z->mem()->malloc_local_double(dima0); 
       z->lambda2()->get_block(p2b_0-z->noab()+z->nvab()*(p1b_0-z->noab()+z->nvab()*(h6b_0+z->noab()*(h5b_0))),k_a0); 
       z->sort_indices4(k_a0,k_a0_sort,z->get_range(h5b),z->get_range(h6b),z->get_range(p1b),z->get_range(p2b),3,2,1,0,+1.0); 
       z->mem()->free_local_double(k_a0); 
       double* k_a1_sort=z->mem()->malloc_local_double(dima1); 
       double* k_a1=z->mem()->malloc_local_double(dima1); 
       z->v2()->get_block(p4b_1+(z->nab())*(p3b_1+(z->nab())*(h8b_1+(z->nab())*(h7b_1))),k_a1); 
       z->sort_indices4(k_a1,k_a1_sort,z->get_range(h7b),z->get_range(h8b),z->get_range(p3b),z->get_range(p4b),3,2,1,0,+1.0); 
       z->mem()->free_local_double(k_a1); 
       double factor=1.0; 
       z->smith_dgemm(dima0_sort,dima1_sort,dim_common,factor,k_a0_sort,dim_common,k_a1_sort,dim_common,1.0,k_c_sort,dima0_sort); 
       z->mem()->free_local_double(k_a1_sort); 
       z->mem()->free_local_double(k_a0_sort); 
      } 
     } 
    } 
    if (h7b>=h6b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,5,4,1,0,+1.0); 
    } 
    if (h7b>=h6b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,5,1,4,0,-1.0); 
    } 
    if (h7b>=h6b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,1,5,4,0,+1.0); 
    } 
    if (h7b>=h6b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,5,1,0,4,+1.0); 
    } 
    if (h7b>=h6b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,1,5,0,4,-1.0); 
    } 
    if (h7b>=h6b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,6,3,2,1,0,5,4,+1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,5,4,1,0,-1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,5,1,4,0,+1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,1,5,4,0,-1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,5,1,0,4,-1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,1,5,0,4,+1.0); 
    } 
    if (h8b>=h6b && h6b>=h7b && h7b>=h5b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,6,2,1,0,5,4,-1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,5,4,1,0,+1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,5,1,4,0,-1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,1,5,4,0,+1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,5,1,0,4,+1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,1,5,0,4,-1.0); 
    } 
    if (h8b>=h6b && h5b>=h7b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,6,2,1,0,5,4,+1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,5,4,1,0,+1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,5,1,4,0,-1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,1,5,4,0,+1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,5,1,0,4,+1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,1,5,0,4,-1.0); 
    } 
    if (h6b>=h8b && h7b>=h5b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),7,3,2,6,1,0,5,4,+1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,5,4,1,0,-1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,5,1,4,0,+1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,1,5,4,0,-1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,5,1,0,4,-1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,1,5,0,4,+1.0); 
    } 
    if (h6b>=h8b && h8b>=h5b && h5b>=h7b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,7,2,6,1,0,5,4,-1.0); 
    } 
    if (h5b>=h8b && p3b>=p2b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,5,4,1,0,+1.0); 
    } 
    if (h5b>=h8b && p4b>=p2b && p2b>=p3b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,5,1,4,0,-1.0); 
    } 
    if (h5b>=h8b && p4b>=p2b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,1,5,4,0,+1.0); 
    } 
    if (h5b>=h8b && p2b>=p4b && p3b>=p1b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,5,1,0,4,+1.0); 
    } 
    if (h5b>=h8b && p2b>=p4b && p4b>=p1b && p1b>=p3b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,1,5,0,4,-1.0); 
    } 
    if (h5b>=h8b && p1b>=p4b) { 
     z->sort_indices_acc8(k_c_sort,a_i0,z->get_range(p4b),z->get_range(p3b),z->get_range(h8b),z->get_range(h7b),z->get_range(p2b),z->get_range(p1b),z->get_range(h6b),z->get_range(h5b),3,2,7,6,1,0,5,4,+1.0); 
    } 
    z->mem()->free_local_double(k_c_sort); 
   } 
  } 
 } 
} 
} 

