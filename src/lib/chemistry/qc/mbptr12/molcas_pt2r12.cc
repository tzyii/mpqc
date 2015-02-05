//
// molcas_pt2r12.cc
//
// Copyright (C) 2014 Chong Peng
//
// Authors: Chong Peng
// Maintainer: Chong Peng and Edward Valeev
//
// This file is part of the MPQC Toolkit.
//
// The MPQC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The MPQC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the MPQC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#include <cstdlib>
#include <fstream>
#include <algorithm>

#include <mpqc_config.h>
#include <chemistry/qc/mbptr12/molcas_pt2r12.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/qc/basis/integral.h>
#include <chemistry/qc/lcao/wfnworld.h>
#include <chemistry/qc/basis/union.h>
#include <extern/moinfo/moinfo.h>

using namespace sc;

ClassDesc MolcasPT2R12::class_desc_(typeid(MolcasPT2R12),
                                "MolcasPT2R12",
                                1,
                                "public MolecularEnergy",
                                0,
                                create<MolcasPT2R12>,
                                0
                                );

MolcasPT2R12::MolcasPT2R12 (const Ref<KeyVal>& kv) :
  MolecularEnergy(kv)
{
  prefix_ = kv->stringvalue("prefix", KeyValValuestring(std::string()));
  if (prefix_.empty()){
    throw InputError("empty keyword value",
            __FILE__, __LINE__, "prefix", prefix_.c_str(),
            this->class_desc());
  }

  molcas_ = kv->stringvalue("molcas", KeyValValuestring(std::string()));
  if (molcas_.empty()){
    throw InputError("empty keyword value",
            __FILE__, __LINE__, "molcas", molcas_.c_str(),
            this->class_desc());
  }

  molcas_input_ = kv->stringvalue("molcas_input", KeyValValuestring(std::string()));
  if (molcas_input_.empty()){
    throw InputError("empty keyword value",
            __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
            this->class_desc());
  }

  xyz_file_ = kv->stringvalue("xyz_file", KeyValValuestring(std::string()));

  std::string obs_name = kv->stringvalue("obs",KeyValValuestring(std::string()));

  std::string cabs_name = kv->stringvalue("cabs", KeyValValuestring(std::string()) );
  if (cabs_name.empty() && not obs_name.empty()) {
    cabs_name = R12Technology::default_cabs_name(obs_name);
  }

  std::string cabs_contraction = kv->stringvalue("cabs_contraction", KeyValValuestring("true") );
  std::string dfbs_name = kv->stringvalue("dfbs", KeyValValuestring(std::string()) );
  if (dfbs_name.empty() && not obs_name.empty())
    dfbs_name = DensityFittingRuntime::default_dfbs_name(obs_name, 1); // for OBS with cardinal number X use DFBS with cardinal number X+1
  if (dfbs_name == "none") dfbs_name = "";

  std::string f12exp = kv->stringvalue("f12exp", KeyValValuestring(std::string()) );

  // if OBS given but F12 exponent is not, look up a default value
  if (f12exp.empty() && not obs_name.empty()) {
    const double f12exp_default = R12Technology::default_stg_exponent(obs_name);
    if (f12exp_default != 0.0) {
      std::ostringstream oss;
      oss << f12exp_default;
      f12exp = oss.str();
    }else{ //if return 0 value, set it to default 1.0
      f12exp = "1.0";
    }
  }

  std::string r12 = kv->stringvalue("r12", KeyValValuestring("true") );

#if defined(HAVE_MPQC3_RUNTIME)
  std::string cabs_singles = kv->stringvalue("cabs_singles", KeyValValuestring("true") );
  std::string cabs_singles_basis = kv->stringvalue("singles_basis", KeyValValuestring(std::string()) );
  std::string partition = kv->stringvalue("patitionH", KeyValValuestring("fock") );

  if (!partition.empty()){
    if (partition!= "fock"
      &partition != "dyall_1"
      &partition != "dyall_2"){
        throw InputError("invalid keyword value",
                       __FILE__, __LINE__, "partitionH", partition.c_str(),
                       this->class_desc());
      }
  }
#endif

  Ref<Integral> integral;
  integral << kv->describedclassvalue("integrals");

  //construct the keyval object needed for ExternPT2R12
  {
    Ref<AssignedKeyVal> kva = new AssignedKeyVal;
    kva->assign("f12exp", f12exp);
    kva->assign("obs", obs_name);
    kva->assign("cabs", cabs_name);
    kva->assign("dfbs", dfbs_name);
    kva->assign("integrals", integral.pointer());
    if(not r12.empty())
      kva->assign("pt2_correction", r12);
    if(not cabs_contraction.empty())
      kva->assign("cabs_contraction", cabs_contraction);

#if defined(HAVE_MPQC3_RUNTIME)
    if(not cabs_singles.empty())
      kva->assign("cabs_singles", cabs_singles);
    if(not cabs_singles_basis.empty())
      kva->assign("cabs_singles_basis", cabs_singles_basis);
    if(not partition.empty())
      kva->assign("cabs_singles_h0", partition);
#endif
    extern_pt2r12_akv_ = kva;
  }
  extern_pt2r12_ = 0;
  rasscf_energy_ = 0;
  caspt2_energy_ = 0;
}


void MolcasPT2R12::compute()
{
  initialize();

  const double value = extern_pt2r12_->value();

  double total_energy = value + caspt2_energy_;
  ExEnv::out0() << indent << scprintf("RASSCF energy [au]:                    %17.12lf",
          rasscf_energy_) << std::endl;
  ExEnv::out0() << indent << scprintf("CASPT2 energy [au]:                    %17.12lf",
          caspt2_energy_ - rasscf_energy_) << std::endl;
  ExEnv::out0() << indent << scprintf("Total PT2R12  energy [au]:             %17.12lf",
          total_energy) << std::endl;
  ExEnv::out0() << std::endl << std::endl;

  set_energy(total_energy);
}


void MolcasPT2R12::initialize()
{

  // run molcas
  run_molcas();
  // read molcas log file
  read_energy();

  Ref<Integral> integral;
  integral << extern_pt2r12_akv_->describedclassvalue("integrals");
  //
  // Read molecule, basis, and orbitals
  //

  std::string obs_name_ = extern_pt2r12_akv_->stringvalue("obs", KeyValValuestring(std::string()));
  std::string dfbs_name_ = extern_pt2r12_akv_->stringvalue("dfbs", KeyValValuestring(std::string()));

  Ref<ExternMOInfo> rdorbs = new ExternMOInfo(prefix_ + ".pt2r12.dat",
          integral,
          obs_name_); // all MO info is contained in rdorbs
  Ref<OrbitalSpace> orbs = rdorbs->orbs();
  Ref<GaussianBasisSet> basis = orbs->basis();
  RefSCMatrix C_ao = orbs->coefs();
  const std::vector<unsigned int>& fzcpi = rdorbs->fzcpi();
  const std::vector<unsigned int>& inactpi = rdorbs->inactpi();
  const std::vector<unsigned int>& actpi = rdorbs->actpi();
  const std::vector<unsigned int>& fzvpi = rdorbs->fzvpi();
  const unsigned int nfzc = std::accumulate(fzcpi.begin(), fzcpi.end(), 0.0);
  const unsigned int ninact = std::accumulate(inactpi.begin(), inactpi.end(), 0.0);
  const unsigned int nact = std::accumulate(actpi.begin(), actpi.end(), 0.0);
  const unsigned int nfzv = std::accumulate(fzvpi.begin(), fzvpi.end(), 0.0);
  const unsigned int nmo = orbs->rank();
  const unsigned int nuocc = nmo - nfzc - ninact - nact - nfzv;

  //basis = orbs->basis();
  C_ao = orbs->coefs();


  /////////////////////////////////////////////
  // Read 2-RDM
  /////////////////////////////////////////////

  // molcas reports 2-RDM in terms of active occupied orbitals only, indexed occording to molcas convention
  // thus use the map from molcas active occupied orbitals to MPQC occupied range
  // first make an OrbitalSpace for MPQC occupied orbitals
  Ref<OrbitalSpace> occ_orbs = new OrbitalSpace(std::string("z(sym)"),
          std::string("symmetry-ordered occupied MOInfo orbitals"),
          orbs->coefs(),
          orbs->basis(),
          orbs->integral(), orbs->evals(),
          0, nuocc + nfzv,
          OrbitalSpace::symmetry);

  Ref<ExternSpinFreeRDMTwo> rdrdm2;
  rdrdm2 = new ExternSpinFreeRDMTwo(prefix_ + ".pt2r12.rdm2.dat",
          rdorbs->actindexmap_occ(),
          occ_orbs);


  // create World in which we will compute
  // use defaults for all params
  Ref<WavefunctionWorld> world;
  {
    Ref<AssignedKeyVal> kva = new AssignedKeyVal;
    if (dfbs_name_.empty() == false) {
      Ref<AssignedKeyVal> tmpkv = new AssignedKeyVal;
      tmpkv->assign("name", dfbs_name_.c_str());
      if (dfbs_name_.find("aug-cc-pV") != std::string::npos &&
              dfbs_name_.find("Z-RI") != std::string::npos) { // if aug-cc-pVXZ-RI, make one as a union of
        // cc-pVXZ-RI and augmentation-cc-pVXZ-RI
        std::string ccpvxzri_name(dfbs_name_, 4, dfbs_name_.size()-4);

        Ref<AssignedKeyVal> tmpkv1 = new AssignedKeyVal;
        tmpkv1->assign("name", ccpvxzri_name);
        tmpkv1->assign("molecule", basis->molecule().pointer());

        Ref<GaussianBasisSet> ccpvxzri = new GaussianBasisSet(tmpkv1);

        Ref<AssignedKeyVal> tmpkv2 = new AssignedKeyVal;
        tmpkv2->assign("name", std::string("augmentation-") + ccpvxzri_name);
        tmpkv2->assign("molecule", basis->molecule().pointer());
        Ref<GaussianBasisSet> augmentationccpvxzri = new GaussianBasisSet(tmpkv2);

        Ref<GaussianBasisSet> df_basis = new UnionBasisSet(ccpvxzri, augmentationccpvxzri);
        kva->assign("df_basis", df_basis.pointer());
      }
      else { // otherwise assume the basis exists in the library
        tmpkv->assign("molecule", basis->molecule().pointer());
        Ref<KeyVal> kv = tmpkv;
        Ref<GaussianBasisSet> df_basis = new GaussianBasisSet(kv);
        kva->assign("df_basis", df_basis.pointer());
      }
    }
    Ref<KeyVal> kv = kva;
    world = new WavefunctionWorld(kv);
  }

  extern_pt2r12_akv_->assign("orbs_info",rdorbs.pointer());
  extern_pt2r12_akv_->assign("rdm2",rdrdm2.pointer());
  extern_pt2r12_akv_->assign("world",world.pointer());
  extern_pt2r12_akv_->assign("basis",orbs->basis().pointer());
  extern_pt2r12_akv_->assign("molecule",orbs->basis()->molecule().pointer());

  Ref<KeyVal> kv = extern_pt2r12_akv_;
  extern_pt2r12_ = new ExternPT2R12(kv);

}

void MolcasPT2R12::run_molcas()
{
  // update the xyz_file
  std::ofstream new_xyz_file;
  new_xyz_file.open(xyz_file_);
  this->molecule()->print_xyz(new_xyz_file, xyz_file_.c_str());
  new_xyz_file.close();

  //excute molcas command
  std::string command_str;
  command_str = molcas_ + " -f " + molcas_input_;
  //std::system(command_str.c_str());

  // check molcas status
  std::ifstream fstatus(prefix_ + ".status");
  if ( fstatus.good() ){

    std::string status;
    std::getline(fstatus, status);
    fstatus.close();

    if ( strcmp(status.c_str(), "Happy landing ") != 0 ){
      throw InputError("Molcas Failed! Check Your Molcas Input File",
              __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
              this->class_desc());
    }

  }else{
    throw InputError("No Molcas Status File Found! Check Your Molcas Excutable Path",
            __FILE__, __LINE__, "molcas", molcas_.c_str(),
            this->class_desc());
  }

  //check the input for pt2r12
  std::ifstream fdat(prefix_ + ".pt2r12.dat");
  std::ifstream frdm2(prefix_ + ".pt2r12.rdm2.dat");
  if (!fdat.good() || !frdm2.good()){
    throw InputError("No PT2R12 DAT File Found! Check Your Molcas Input",
            __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
            this->class_desc());
  }
  fdat.close();
  frdm2.close();

}

void MolcasPT2R12::read_energy()
{

  std::string molcas_log = prefix_ + ".log";
  std::fstream flog(molcas_log);

  if (!flog.good()){
    throw InputError("No Molcas Log File Found! Check Your Molcas Input",
            __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
            this->class_desc());
  }

  std::string line;

  while(std::getline(flog, line)){
    if (line.find("RASSCF") != std::string::npos && line.find("Total energy") != std::string::npos){
      std::string num = line.substr(50, 15);
      rasscf_energy_ = std::stod(num);
    }
    else if(line.find("CASPT2") != std::string::npos && line.find("Total energy") != std::string::npos)
    {
      std::string num = line.substr(40, 15);
      caspt2_energy_ = std::stod(num);
    }
  }

  flog.close();

  if ( rasscf_energy_ == 0.0) {
    throw InputError("RASSCF Energy is 0. Check Your Molcas Input",
            __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
            this->class_desc());
  }
  if (caspt2_energy_== 0.0){
    throw InputError("CASPT2 Energy is 0. Check Your Molcas Input",
            __FILE__, __LINE__, "molcas_input", molcas_input_.c_str(),
            this->class_desc());
  }
}

void MolcasPT2R12::purge()
{
  extern_pt2r12_ = 0;
  rasscf_energy_ = 0;
  caspt2_energy_ = 0;
}