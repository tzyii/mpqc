
#ifndef _chemistry_qc_scf_ltbgrad_h
#define _chemistry_qc_scf_ltbgrad_h

#ifdef __GNUC__
#pragma interface
#endif

#include <math/scmat/offset.h>

#include <chemistry/qc/basis/tbint.h>
#include <chemistry/qc/basis/petite.h>

#include <chemistry/qc/scf/tbgrad.h>
  
template<class T>
class LocalTBGrad : public TBGrad<T> {
  protected:
    RefMessageGrp grp_;
    RefTwoBodyDerivInt tbi_;
    RefIntegral integral_;
    RefGaussianBasisSet gbs_;

  public:
    LocalTBGrad(T& t, const RefIntegral& ints, const RefGaussianBasisSet& bs,
                const RefMessageGrp& g) :
      TBGrad<T>(t), grp_(g), integral_(ints), gbs_(bs)
    {
      tbi_ = integral_->electron_repulsion_deriv();
    }

    ~LocalTBGrad() {}
    
    void build_tbgrad(const RefSCVector& tbgrad,
                      double pmax, double accuracy) {
      // grab ref for convenience
      GaussianBasisSet& gbs = *gbs_.pointer();
      Molecule& mol = *gbs.molecule().pointer();
      RefPetiteList rpl = integral_->petite_list();
      PetiteList& pl = *rpl.pointer();
      
      // create vector to hold skeleton gradient
      RefSCVector tbint = tbgrad.clone();
      tbint.assign(0.0);

      // for bounds checking
      int PPmax = (int) (log(6.0*pmax*pmax)/log(2.0));
      int threshold = (int) (log(accuracy)/log(2.0));
  
      for (int i=0; i < gbs.nshell(); i++) {
        if (!pl.in_p1(i))
          continue;
    
        int ni=gbs(i).nfunction();
        int fi=gbs.shell_to_function(i);
    
        for (int j=0; j <= i; j++) {
          int ij=i_offset(i)+j;
          if (!pl.in_p2(ij))
            continue;
      
          if (tbi_->log2_shell_bound(i,j,-1,-1)+PPmax < threshold)
            continue;
      
          int nj=gbs(j).nfunction();
          int fj=gbs.shell_to_function(j);
    
          for (int k=0; k <= i; k++) {
            int nk=gbs(k).nfunction();
            int fk=gbs.shell_to_function(k);
    
            for (int l=0; l <= ((i==k)?j:k); l++) {
              if (tbi_->log2_shell_bound(i,j,k,l)+PPmax < threshold)
                continue;
          
              int kl=i_offset(k)+l;
              int qijkl;
              if (!(qijkl=pl.in_p4(ij,kl,i,j,k,l)))
                continue;
          
              int nl=gbs(l).nfunction();
              int fl=gbs.shell_to_function(l);

              DerivCenters cent;
              tbi_->compute_shell(i,j,k,l,cent);

              const double * buf = tbi_->buffer();
          
              double cscl, escl;

              set_scale(cscl, escl, i, j, k, l);

              int indijkl=0;
              int nx=cent.n();
              if (cent.has_omitted_center()) nx--;
              for (int x=0; x < nx; x++) {
                int ix=cent.atom(x);
                int io=cent.omitted_atom();
                for (int ixyz=0; ixyz < 3; ixyz++) {
                  for (int ip=0, ii=fi; ip < ni; ip++, ii++) {
                    for (int jp=0, jj=fj; jp < nj; jp++, jj++) {
                      for (int kp=0, kk=fk; kp < nk; kp++, kk++) {
                        for (int lp=0, ll=fl; lp < nl; lp++, ll++, indijkl++) {
                          double contrib;
                          double qint = buf[indijkl]*qijkl;

                          contrib = cscl*qint*
                            contribution.cont(ij_offset(ii,jj),
                                              ij_offset(kk,ll));

                          tbint.accumulate_element(ixyz+ix*3,  contrib);
                          tbint.accumulate_element(ixyz+io*3, -contrib);

                          contrib = escl*qint*
                            contribution.cont(ij_offset(ii,kk),
                                              ij_offset(jj,ll));

                          tbint.accumulate_element(ixyz+ix*3,  contrib);
                          tbint.accumulate_element(ixyz+io*3, -contrib);

                          if (i!=j && k!=l) {
                            contrib = escl*qint*
                              contribution.cont(ij_offset(ii,ll),
                                                ij_offset(jj,kk));

                            tbint.accumulate_element(ixyz+ix*3,  contrib);
                            tbint.accumulate_element(ixyz+io*3, -contrib);
                          }
                        }
                      }
                    }
                  }
                }

                if (cent.atom(x) == cent.omitted_atom())
                  x++;
              }
            }
          }
        }
      }
      
      tbint.print("two electron contribution");

      tbgrad.assign(tbint);
      CharacterTable ct = mol.point_group().char_table();
      SymmetryOperation so;
  
      for (int alpha=0; alpha < mol.natom(); alpha++) {
        for (int g=1; g < ct.order(); g++) {
          so = ct.symm_operation(g);
          int ap = pl.atom_map(alpha,g);

          tbgrad.accumulate_element(alpha*3+0,
                                    tbint.get_element(ap*3+0)*so(0,0) +
                                    tbint.get_element(ap*3+1)*so(1,0) +
                                    tbint.get_element(ap*3+2)*so(2,0));
          tbgrad.accumulate_element(alpha*3+1,
                                    tbint.get_element(ap*3+0)*so(0,1) +
                                    tbint.get_element(ap*3+1)*so(1,1) +
                                    tbint.get_element(ap*3+2)*so(2,1));
          tbgrad.accumulate_element(alpha*3+2,
                                    tbint.get_element(ap*3+0)*so(0,2) +
                                    tbint.get_element(ap*3+1)*so(1,2) +
                                    tbint.get_element(ap*3+2)*so(2,2));
        }
      }
    
      tbgrad.scale(1.0/ct.order());
      tbgrad.print("two electron contribution");
    }
};

#endif
