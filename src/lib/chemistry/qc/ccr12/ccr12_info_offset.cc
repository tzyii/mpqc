//
// ccr12_info_offset.cc
//
// Copyright (C) 2008 Toru Shiozaki
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

#include <algorithm>
#include <chemistry/qc/ccr12/ccr12_info.h>

using namespace std;
using namespace sc;


void CCR12_Info::offset_f1(){
  long size = 0L;
  for (long g1b = 0L; g1b < nab(); ++g1b) {
    for (long g2b = 0L; g2b < nab(); ++g2b) {
      if (get_spin(g1b) == get_spin(g2b)) {
        if ((get_sym(g1b) ^ get_sym(g2b)) == irrep_f_) {
          if (!restricted_ || get_spin(g1b) + get_spin(g2b) != 4L) {
            if (!need_w1() && !need_w2() && g1b<noab()+nvab() && g2b<noab()+nvab()){
              // OBS only
              d_f1->input_offset(g2b + nab() * g1b, size);
              size += get_range(g1b)*get_range(g2b);
            } else if (need_w1() && !(g1b >= noab() + nvab() && g2b >= noab() + nvab())) {
              // R12 theories: (R12),-R12,...
              d_f1->input_offset(g2b + nab() * g1b, size);
              size += get_range(g1b) * get_range(g2b);
            }
          }
        }
      }
    }
  }
  d_f1->set_filesize(size);
  ExEnv::out0() << indent << "f1 file  :   " << d_f1->filename() << endl;
  ExEnv::out0() << indent << "size     : " << setw(10) << size  << " doubles" << endl << endl;
  d_f1->createfile();
}


void CCR12_Info::offset_v2(){
  long size = 0L;
  for (long g1b = 0L; g1b < nab(); ++g1b) {
    for (long g2b = g1b; g2b < nab(); ++g2b) {
      for (long g3b = 0L; g3b < nab(); ++g3b) {
        for (long g4b = g3b; g4b < nab(); ++g4b) {
          if (get_spin(g1b) + get_spin(g2b) == get_spin(g3b) + get_spin(g4b)) {
            if ((get_sym(g1b) ^ (get_sym(g2b) ^ (get_sym(g3b) ^ get_sym(g4b)))) == irrep_v_) {
              if (!restricted_ || get_spin(g1b) + get_spin(g2b) + get_spin(g3b) + get_spin(g4b) != 8L) {
                if (!need_w1() && !need_w2() && g2b < noab() + nvab() && g4b < noab() + nvab()){
                  // OBS only
                  d_v2->input_offset(g4b + nab() * (g3b + nab() * (g2b + nab() * g1b)), size);
                  size += get_range(g1b) * get_range(g2b) * get_range(g3b) * get_range(g4b);
                } else if (need_w1() && !need_w2() && ((g1b < noab() + nvab() && g4b < noab() + nvab())
                    || (g3b < noab() + nvab() && g2b < noab() + nvab()))) {
                  // (R12) type method
                  d_v2->input_offset(g4b + nab() * (g3b + nab() * (g2b + nab() * g1b)), size);
                  size += get_range(g1b) * get_range(g2b) * get_range(g3b) * get_range(g4b);
                } else if (need_w2() && ((g1b < noab() + nvab() && g3b < noab() + nvab()) || g2b < noab() + nvab())){
                  // R12 type method
                  d_v2->input_offset(g4b + nab() * (g3b + nab() * (g2b + nab() * g1b)), size);
                  size += get_range(g1b) * get_range(g2b) * get_range(g3b) * get_range(g4b);
                }
              }
            }
          }
        }
      }
    }
  }
  d_v2->set_filesize(size);
  ExEnv::out0() << indent << "v2 file  :   " << d_v2->filename() << endl;
  ExEnv::out0() << indent << "size     : " << setw(10) << size  << " doubles" << endl << endl;
  d_v2->createfile();
}


void CCR12_Info::offset_t1(Ref<Tensor>& d_t1_, bool nprint){
  long size=0L;
  for (long p1b = noab(); p1b < noab() + nvab(); ++p1b) {
    for (long h2b = 0L; h2b < noab(); ++h2b) {
      if (get_spin(p1b) == get_spin(h2b)) {
        if ((get_sym(p1b) ^ get_sym(h2b)) == irrep_t_) {  /// needs to be generalized when used for the R1 tensor in EOM-CC
          if (!restricted_ || get_spin(p1b) + get_spin(h2b)!=4L) {
            d_t1_->input_offset(h2b + noab() * (p1b - noab()), size);
            size += get_range(p1b) * get_range(h2b);
          }
        }
      }
    }
  }
  d_t1_->set_filesize(size);
  if (nprint) {
    ExEnv::out0() << indent << "t1 file  :   " << d_t1_->filename() << endl;
    ExEnv::out0() << indent << "size     : " << setw(10) << size  << " doubles" << endl << endl;
  }
  d_t1_->createfile();
}


void CCR12_Info::offset_t2(Ref<Tensor>& d_t2_,bool nprint){
  long size = 0L;
  for (long p1b = noab(); p1b < noab()+nvab(); ++p1b) {
    for (long p2b = p1b; p2b < noab()+nvab(); ++p2b) {
      for (long h3b = 0L; h3b < noab(); ++h3b) {
        for (long h4b = h3b; h4b < noab(); ++h4b) {
          if (get_spin(p1b) + get_spin(p2b) == get_spin(h3b) + get_spin(h4b)) {
            if ((get_sym(p1b) ^ (get_sym(p2b) ^ (get_sym(h3b) ^ get_sym(h4b)))) == irrep_t_) {
              if (!restricted_ || get_spin(p1b) + get_spin(p2b) + get_spin(h3b) + get_spin(h4b) != 8L) {
                d_t2_->input_offset(h4b + noab() * (h3b + noab() * (p2b - noab() + nvab() * (p1b - noab()))), size);
                size += get_range(p1b) * get_range(p2b) * get_range(h3b) * get_range(h4b);
              }
            }
          }
        }
      }
    }
  }
  d_t2_->set_filesize(size);
  if (nprint) {
    ExEnv::out0() << indent << "t2 file  :   " << d_t2_->filename() << endl;
    ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  }
  d_t2_->createfile();
}


void CCR12_Info::offset_gt2(Ref<Tensor>& d_gt2_, bool nprint){
  long size = 0L;
  for (long h1b = 0L; h1b < noab(); ++h1b) {
    for (long h2b = h1b; h2b < noab(); ++h2b) {
      for (long h3b = 0L; h3b < noab(); ++h3b) {
        for (long h4b = h3b; h4b < noab(); ++h4b) {
          if (get_spin(h1b) + get_spin(h2b) == get_spin(h3b) + get_spin(h4b)) {
            if ((get_sym(h1b) ^ (get_sym(h2b) ^ (get_sym(h3b) ^ get_sym(h4b)))) == irrep_t_) {
              if (!restricted_ || get_spin(h1b) + get_spin(h2b) + get_spin(h3b) + get_spin(h4b) != 8L) {
                d_gt2_->input_offset(h4b + noab() * (h3b + noab() * (h2b + noab() * h1b)), size);
                size += get_range(h1b) * get_range(h2b) * get_range(h3b) * get_range(h4b);
              }
            }
          }
        }
      }
    }
  }
  d_gt2_->set_filesize(size);
  if (nprint) {
    ExEnv::out0() << indent << "gt2 file :   " << d_gt2_->filename() << endl;
    ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  }
  d_gt2_->createfile();
}


void CCR12_Info::offset_t3(Ref<Tensor>& d_t3_,bool nprint){
  long size=0L;
  const long o = noab();
  const long v = nvab();
  for (long p1b = o; p1b < o+v; ++p1b) {
    for (long p2b = p1b; p2b < o+v; ++p2b) {
      for (long p3b = p2b; p3b < o+v; ++p3b) {
        for (long h4b = 0L; h4b < o; ++h4b) {
          for (long h5b = h4b; h5b < o; ++h5b) {
            for (long h6b = h5b; h6b < o; ++h6b) {
              if (get_spin(p1b)+get_spin(p2b)+get_spin(p3b) == get_spin(h4b)+get_spin(h5b)+get_spin(h6b)) {
                if ((get_sym(p1b)^(get_sym(p2b)^(get_sym(p3b)^(get_sym(h4b)^(get_sym(h5b)^get_sym(h6b)))))) == irrep_t_) {
                  if (!restricted_ || get_spin(p1b)+get_spin(p2b)+get_spin(p3b)
                                     +get_spin(h4b)+get_spin(h5b)+get_spin(h6b) != 12L) {
                    d_t3_->input_offset(h6b+o*(h5b+o*(h4b+o*(p3b-o+v*(p2b-o+v*(p1b-o))))), size);
                    size += get_range(p1b) * get_range(p2b) * get_range(p3b)
                        * get_range(h4b) * get_range(h5b) * get_range(h6b);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  d_t3_->set_filesize(size);
  if (nprint) {
    ExEnv::out0() << indent << "t3 file  :   " << d_t3_->filename() << endl;
    ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  }
  d_t3_->createfile();
}


void CCR12_Info::offset_t4(Ref<Tensor>& d_t4_,bool nprint){
  long size=0L;
  const long o = noab();
  const long v = nvab();
  for (long p1b = o; p1b < o+v; ++p1b) {
    for (long p2b = p1b; p2b < o+v; ++p2b) {
      for (long p3b = p2b; p3b < o+v; ++p3b) {
        for (long p4b = p3b; p4b < o+v; ++p4b) {
          for (long h5b = 0L; h5b < o; ++h5b) {
            for (long h6b = h5b; h6b < o; ++h6b) {
              for (long h7b = h6b; h7b < o; ++h7b) {
                for (long h8b = h7b; h8b < o; ++h8b) {
                  if (get_spin(p1b)+get_spin(p2b)+get_spin(p3b)+get_spin(p4b) ==
                      get_spin(h5b)+get_spin(h6b)+get_spin(h7b)+get_spin(h8b)) {
                    if ((get_sym(p1b)^(get_sym(p2b)^(get_sym(p3b)^(get_sym(p4b)^
                        (get_sym(h5b)^(get_sym(h6b)^(get_sym(h7b)^get_sym(h8b)))))))) == irrep_t_) {
                      if (!restricted_ || get_spin(p1b)+get_spin(p2b)+get_spin(p3b)+get_spin(p4b)
                          +get_spin(h5b)+get_spin(h6b)+get_spin(h7b)+get_spin(h8b) != 16L) {
                        d_t4_->input_offset(h8b+o*(h7b+o*(h6b+o*(h5b+o*(p4b-o+v*(p3b-o+v*(p2b-o+v*(p1b-o))))))), size);
                        size += get_range(p1b)*get_range(p2b)*get_range(p3b)*get_range(p4b)
                               *get_range(h5b)*get_range(h6b)*get_range(h7b)*get_range(h8b);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  d_t4_->set_filesize(size);
  if (nprint) {
    ExEnv::out0() << indent << "t4 file  :   " << d_t4_->filename() << endl;
    ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  }
  d_t4_->createfile();
}



void CCR12_Info::offset_l1(Ref<Tensor>& d_l1_){
  long size = 0L;
  for (long h2b = 0L; h2b < noab(); ++h2b) {
    for (long p1b = noab(); p1b < noab() + nvab(); ++p1b) {
      if (get_spin(p1b) == get_spin(h2b)) {
        if ((get_sym(p1b) ^ get_sym(h2b)) == irrep_t_) {  /// needs to be generalized for EOM-CC
          if (!restricted_ || get_spin(p1b) + get_spin(h2b) != 4L) {
            d_l1_->input_offset(p1b - noab() + nvab() * h2b, size);
            size += get_range(p1b) * get_range(h2b);
          }
        }
      }
    }
  }
  d_l1_->set_filesize(size);
  d_l1_->createfile();
}


void CCR12_Info::offset_l2(Ref<Tensor>& d_l2_){
  long size = 0L;
  for (long h3b = 0L; h3b < noab(); ++h3b) {
    for (long h4b = h3b; h4b < noab(); ++h4b) {
      for (long p1b = noab(); p1b < noab() + nvab(); ++p1b) {
        for (long p2b = p1b; p2b < noab() + nvab(); ++p2b) {
          if (get_spin(p1b) + get_spin(p2b) == get_spin(h3b) + get_spin(h4b)) {
            if ((get_sym(p1b) ^ (get_sym(p2b) ^ (get_sym(h3b) ^ get_sym(h4b)))) == irrep_t_) {
              if (!restricted_ || get_spin(p1b) + get_spin(p2b) + get_spin(h3b) + get_spin(h4b) != 8L) {
                d_l2_->input_offset(p2b - noab() + nvab() * (p1b - noab() + nvab() * (h4b + noab() * h3b)), size);
                size += get_range(p1b) * get_range(p2b) * get_range(h3b) * get_range(h4b);
              }
            }
          }
        }
      }
    }
  }
  d_l2_->set_filesize(size);
  d_l2_->createfile();
}


/////////////////////////////////////////////////////////////////////////////////////////////

/// V^gg_ii tensor (referred to as d_vr2 in smith)
void CCR12_Info::offset_vr2(){
  long size=0L;
  for (long g1b = 0L; g1b<nab(); ++g1b) {
    for (long g2b = g1b; g2b<nab(); ++g2b) {
      for (long h3b = 0L; h3b<noab(); ++h3b) {
        for (long h4b = h3b; h4b<noab(); ++h4b) {
          if (get_spin(g1b)+get_spin(g2b) == get_spin(h3b)+get_spin(h4b)) {
            if ((get_sym(g1b)^(get_sym(g2b)^(get_sym(h3b)^get_sym(h4b)))) == (irrep_t_^irrep_v_)) {
              if (!restricted_ || get_spin(g1b)+get_spin(g2b)+get_spin(h3b)+get_spin(h4b) != 8L) {
                if (g1b<noab()+nvab()) { // it can be put into the outermost loop;
                                         // but make sure that tags are evaluated with nab()
                                         // ** only one index can go to CABS
                                         // in (R12) methods, we don't need V_pA type intermediates
                  if (!need_VpA_ && g2b >= noab() + nvab()) continue;

                  d_vr2->input_offset(h4b+noab()*(h3b+noab()*(g2b+nab()*g1b)), size);
                  size += get_range(g1b)*get_range(g2b)*get_range(h3b)*get_range(h4b);
                }
              }
            }
          }
        }
      }
    }
  }
  d_vr2->set_filesize(size);
  ExEnv::out0() << indent << "vr file  :   " << d_vr2->filename() << endl;
  ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  d_vr2->createfile();
}

/// V^ii_gg tensor (referred to as d_vd2 in smith)
void CCR12_Info::offset_vd2(){
  long size=0L;
  for (long h1b = 0L; h1b < noab(); ++h1b) {
    for (long h2b = h1b; h2b < noab(); ++h2b) {
      for (long g3b = 0L; g3b < nab(); ++g3b) {
        for (long g4b = g3b; g4b < nab(); ++g4b) {
          if (get_spin(h1b)+get_spin(h2b)==get_spin(g3b)+get_spin(g4b)) {
            if ((get_sym(h1b)^(get_sym(h2b)^(get_sym(g3b)^get_sym(g4b)))) == (irrep_t_^irrep_v_)) {
              if (!restricted_ || get_spin(h1b)+get_spin(h2b)+get_spin(g3b)+get_spin(g4b) != 8L) {
                if (g3b<noab()+nvab()) { // it can be put into the outermost loop;
                                         // but make sure that tags are evaluated with nab()
                                         // ** only one index can go to CABS
                  // in some cases, we don't need V_pA type intermediates
                  if (!need_VpA_ && g4b >= noab() + nvab()) continue;

                  d_vd2->input_offset(g4b+nab()*(g3b+nab()*(h2b+noab()*h1b)), size);
                  size += get_range(h1b)*get_range(h2b)*get_range(g3b)*get_range(g4b);
                }
              }
            }
          }
        }
      }
    }
  }
  d_vd2->set_filesize(size);
  ExEnv::out0() << indent << "vd file  :   " << d_vd2->filename() << endl;
  ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  d_vd2->createfile();
}


/// F12^AA_ii tensor (referred to as d_fr2 in smith)
void CCR12_Info::offset_fr2(){
  long size=0L;
  for (long g1b = 0L; g1b < nab(); ++g1b) {
    for (long g2b = g1b; g2b < nab(); ++g2b) {
      for (long h3b = 0L; h3b < noab(); ++h3b) {
        for (long h4b = h3b; h4b < noab(); ++h4b) {
          if (get_spin(g1b)+get_spin(g2b) == get_spin(h3b)+get_spin(h4b)) {
            if ((get_sym(g1b)^(get_sym(g2b)^(get_sym(h3b)^get_sym(h4b)))) == irrep_v_) {
              if (!restricted_ || get_spin(g1b)+get_spin(g2b)+get_spin(h3b)+get_spin(h4b) != 8L) {
                if (g1b>=noab() && g2b>=noab()+nvab()) {
                              // make sure that tags are still evaluated with nab()
                              // ** modified ansatz 2
                  if (!need_FAA_ && g1b>=noab()+nvab()) continue;
                  d_fr2->input_offset(h4b+noab()*(h3b+noab()*(g2b+nab()*g1b)), size);
                  size += get_range(g1b)*get_range(g2b)*get_range(h3b)*get_range(h4b);
                }
              }
            }
          }
        }
      }
    }
  }
  d_fr2->set_filesize(size);
  ExEnv::out0() << indent << "F12 file :   " << d_fr2->filename() << endl;
  ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  d_fr2->createfile();
}


/// F12^ii_AA tensor (referred to as d_fd2 in smith)
void CCR12_Info::offset_fd2(){
  long size=0L;
  for (long h1b = 0L; h1b < noab(); ++h1b) {
    for (long h2b = h1b; h2b < noab(); ++h2b) {
      for (long g3b = 0L; g3b < nab(); ++g3b) {
        for (long g4b = g3b; g4b < nab(); ++g4b) {
          if (get_spin(h1b)+get_spin(h2b) == get_spin(g3b)+get_spin(g4b)) {
            if ((get_sym(h1b)^(get_sym(h2b)^(get_sym(g3b)^get_sym(g4b)))) == irrep_v_) {
              if (!restricted_ || get_spin(h1b)+get_spin(h2b)+get_spin(g3b)+get_spin(g4b) != 8L) {
                if (g3b >= noab() && g4b >= noab()+nvab()) {
                              // make sure that tags are still evaluated with nab()
                              // ** modified ansatz 2
                  if (!need_FAA_ && g3b>=noab()+nvab()) continue;
                  d_fd2->input_offset(g4b+nab()*(g3b+nab()*(h2b+noab()*h1b)), size);
                  size += get_range(h1b)*get_range(h2b)*get_range(g3b)*get_range(g4b);
                }
              }
            }
          }
        }
      }
    }
  }
  d_fd2->set_filesize(size);
  ExEnv::out0() << indent << "F12d file:   " << d_fd2->filename() << endl;
  ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  d_fd2->createfile();
}


void CCR12_Info::offset_qy(){
  long size = 0L;
  for (long p3b = noab(); p3b < noab() + nvab(); ++p3b) {
    for (long q4b = noab() + nvab(); q4b < nab(); ++q4b) {
      for (long h1b = 0L; h1b < noab(); ++h1b) {
        for (long h2b = h1b; h2b < noab(); ++h2b) {
          if (get_spin(p3b) + get_spin(q4b) == get_spin(h1b) + get_spin(h2b)) {
            if ((get_sym(p3b) ^ (get_sym(q4b) ^ (get_sym(h1b) ^ get_sym(h2b)))) == irrep_t()) {
              if (!restricted_ || get_spin(p3b) + get_spin(q4b) + get_spin(h1b) + get_spin(h2b) != 8L) {
                d_qy->input_offset(h2b+noab()*(h1b+noab()*(q4b-noab()-nvab()+ncab()*(p3b-noab()))), size);
                size += get_range(p3b) * get_range(q4b) * get_range(h1b) * get_range(h2b);
              }
            }
          }
        }
      }
    }
  }
  d_qy->set_filesize(size);
  d_qy->createfile();
}


void CCR12_Info::offset_qx(){
  long size = 0L;
  for (long q3b = noab() + nvab(); q3b < nab(); ++q3b) {
    for (long q4b = q3b; q4b < nab(); ++q4b) {
      for (long h1b = 0L; h1b < noab(); ++h1b) {
        for (long h2b = h1b; h2b < noab(); ++h2b) {
          if (get_spin(q3b) + get_spin(q4b) == get_spin(h1b) + get_spin(h2b)) {
            if ((get_sym(q3b) ^ (get_sym(q4b) ^ (get_sym(h1b) ^ get_sym(h2b)))) == irrep_t()) {
              if (!restricted_ || get_spin(q3b) + get_spin(q4b) + get_spin(h1b) + get_spin(h2b) != 8L) {
                d_qx->input_offset(h2b+noab()*(h1b+noab()*(q4b-noab()-nvab()+ncab()*(q3b-noab()-nvab()))), size);
                size += get_range(q3b) * get_range(q4b) * get_range(h1b) * get_range(h2b);
              }
            }
          }
        }
      }
    }
  }
  d_qx->set_filesize(size);
  d_qx->createfile();
}


void CCR12_Info::offset_ly(){
  long size = 0L;
  for (long h1b = 0L; h1b < noab(); ++h1b) {
    for (long h2b = h1b; h2b < noab(); ++h2b) {
      for (long p3b = noab(); p3b < noab() + nvab(); ++p3b) {
        for (long q4b = noab() + nvab();q4b < nab(); ++q4b) {
          if (get_spin(p3b) + get_spin(q4b) == get_spin(h1b) + get_spin(h2b)) {
            if ((get_sym(p3b) ^ (get_sym(q4b) ^ (get_sym(h1b) ^ get_sym(h2b)))) == irrep_t()) {
              if (!restricted_ || get_spin(p3b) + get_spin(q4b) + get_spin(h1b) + get_spin(h2b) != 8L) {
                d_ly->input_offset(q4b-noab()-nvab()+ncab()*(p3b-noab()+nvab()*(h2b+noab()*h1b)), size);
                size += get_range(p3b) * get_range(q4b) * get_range(h1b) * get_range(h2b);
              }
            }
          }
        }
      }
    }
  }
  d_ly->set_filesize(size);
  d_ly->createfile();
}


void CCR12_Info::offset_lx(){
  long size = 0L;
  for (long h1b = 0L; h1b < noab(); ++h1b) {
    for (long h2b = h1b; h2b < noab(); ++h2b) {
      for (long q3b = noab() + nvab(); q3b < nab(); ++q3b) {
        for (long q4b = q3b; q4b < nab(); ++q4b) {
          if (get_spin(q3b) + get_spin(q4b) == get_spin(h1b) + get_spin(h2b)) {
            if ((get_sym(q3b) ^ (get_sym(q4b) ^ (get_sym(h1b) ^ get_sym(h2b)))) == irrep_t()) {
              if (!restricted_ || get_spin(q3b) + get_spin(q4b) + get_spin(h1b) + get_spin(h2b) != 8L) {
                d_lx->input_offset(q4b-noab()-nvab()+ncab()*(q3b-noab()-nvab()+ncab()*(h2b+noab()*h1b)), size);
                size += get_range(q3b) * get_range(q4b) * get_range(h1b) * get_range(h2b);
              }
            }
          }
        }
      }
    }
  }
  d_lx->set_filesize(size);
  d_lx->createfile();
}


void CCR12_Info::offset_e(Ref<Tensor>& d_e_){
  d_e_->input_offset(0L,0L);
  d_e_->set_filesize(1L);
  d_e_->createfile();
}


// Generalized V^dagger tensor. The geminal generating space is taken to be
// either ix or xx (where x runs over OBS).
void CCR12_Info::offset_vd2_gen(bool need_cabs, bool need_xx){
  long size=0L;
  const long o = noab();
  const long ov = noab() + nvab();
  const long ovc = nab();

  for (long x1b = 0L; x1b < ov; ++x1b) {
    for (long x2b = x1b; x2b < ov; ++x2b) {
      for (long g3b = 0L; g3b < ovc; ++g3b) {
        for (long g4b = g3b; g4b < ovc; ++g4b) {
          if (get_spin(x1b)+get_spin(x2b) == get_spin(g3b)+get_spin(g4b)) {
            if ((get_sym(x1b)^(get_sym(x2b)^(get_sym(g3b)^get_sym(g4b)))) == (irrep_t_^irrep_v_)) {
              if (!restricted_ || get_spin(x1b)+get_spin(x2b)+get_spin(g3b)+get_spin(g4b) != 8L) {

                if (g3b < ov) { // we don't need AA
                  if (!need_cabs && g4b >= ov) continue; // we don't need pA sometimes (A is in CABS).
                  if (!need_xx && x1b >= o) continue; // we don't need aa in geminal generating space sometimes.

                  d_vd2_gen->input_offset(g4b+ovc*(g3b+ovc*(x2b+ov*x1b)), size);
                  size += get_range(x1b)*get_range(x2b)*get_range(g3b)*get_range(g4b);
                }

              }
            }
          }
        }
      }
    }
  }
  d_vd2_gen->set_filesize(size);
  ExEnv::out0() << indent << "vdg file :   " << d_vd2_gen->filename() << endl;
  ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  d_vd2_gen->createfile();
}


// Generalized X^dagger tensor etc. The geminal generating space is taken to be
// either ix or xx (where x runs over OBS).
void CCR12_Info::offset_x_gen(Ref<Tensor>& t, const bool need_xx, const bool lprint){
  long size=0L;
  const long o = noab();
  const long ov = noab() + nvab();

  for (long x1b = 0L; x1b < ov; ++x1b) {
    for (long x2b = x1b; x2b < ov; ++x2b) {
      for (long g3b = 0L; g3b < ov; ++g3b) {
        for (long g4b = g3b; g4b < ov; ++g4b) {
          if (get_spin(x1b)+get_spin(x2b) == get_spin(g3b)+get_spin(g4b)) {
            if ((get_sym(x1b)^(get_sym(x2b)^(get_sym(g3b)^get_sym(g4b)))) == (irrep_t_^irrep_v_)) {
              if (!restricted_ || get_spin(x1b)+get_spin(x2b)+get_spin(g3b)+get_spin(g4b) != 8L) {

                if (!need_xx && g3b >= o) continue; // we don't need aa in geminal generating space sometimes.
                if (!need_xx && x1b >= o) continue; // we don't need aa in geminal generating space sometimes.

                t->input_offset(g4b+ov*(g3b+ov*(x2b+ov*x1b)), size);
                size += get_range(x1b)*get_range(x2b)*get_range(g3b)*get_range(g4b);

              }
            }
          }
        }
      }
    }
  }
  t->set_filesize(size);
  if (lprint) {
    ExEnv::out0() << indent << "ipip file:   " << t->filename() << endl;
    ExEnv::out0() << indent << "size     : "   << setw(10) << size  << " doubles" << endl << endl;
  }
  t->createfile();
}



