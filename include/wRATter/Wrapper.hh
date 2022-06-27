//
// Created by Stephane Zsoldos on 6/27/22.
//

#ifndef WRATTER_INCLUDE_WRATTER_WRAPPER_HH_
#define WRATTER_INCLUDE_WRATTER_WRAPPER_HH_

#include "Hit.hh"

// ####################################### //
// #### #### ####   C/C++   #### #### #### //
// ####################################### //
#include <string>
#include <vector>

// ####################################### //
// #### #### ####   ROOT    #### #### #### //
// ####################################### //
#include <TChain.h>
#include <TVector3.h>

// ####################################### //
// #### #### ####    RAT    #### #### #### //
// ####################################### //
#include <RAT/DS/Run.hh>
#include <RAT/DS/Root.hh>


class wRAT {
 protected:
  TChain *ChT;
  TChain *ChRunT;

  RAT::DS::Run *RUN;
  RAT::DS::Root *DS;
 public:
  // ########################################## //
  // #### #### #### CONSTRUCTORS #### #### #### //
  // ########################################## //
  wRAT();
  explicit wRAT(const char *filename);
  explicit wRAT(const std::string& filename);
  explicit wRAT(const std::vector<const char *> &vFiles);
  explicit wRAT(const std::vector<std::string> &vFiles);
  // ######################################### //
  // #### #### #### DESTRUCTORS #### #### #### //
  // ######################################### //
  virtual ~wRAT();
  // ############################################### //
  // #### #### #### GETTERS / SETTERS #### #### #### //
  // ############################################### //
  TChain *GetChT() const;
  void SetChT(TChain *ch_t);
  TChain *GetChRunT() const;
  void SetChRunT(TChain *ch_run_t);
  RAT::DS::Run *GetRun() const;
  void SetRun(RAT::DS::Run *run);
  RAT::DS::Root *GetDs() const;
  void SetDs(RAT::DS::Root *ds);
  // ##################################### //
  // #### #### #### METHODS #### #### #### //
  // ##################################### //
  // Point the class to a chosen event iEvt
  void SetEvt(unsigned iEvt);
  // Get total nb of events
  unsigned long GetNEvts();
  // Get Nb of recorded triggers
  // (this depends on the rat-pac DAQ)
  unsigned GetNTriggers();
  // For a trigger iTrig, get trigger time
  double GetTriggerTime(const int &iTrig);

  int GetNPrimaryParticle();

  // Get TRUE primary particle info
  void GetPrimaryParticleInfo(const double &TriggerTime,
							  TVector3 &Pos, TVector3 &Dir,
							  double &E,
							  double &T);

  TVector3 GetPosTrue(int iParticle);
  TVector3 GetDirTrue(int iParticle);
  double GetTTrue(int iParticle);
  double GetETrue(int iParticle);

  TVector3 GetPosRec(const int &iTrig);
  double GetTRec(const int &iTrig);

  double GetChi2(const int &iTrig);

  double GetQ(const int &iTrig);
  int GetNHits(const int &iTrig);

  std::vector<Hit> GetVHits(const int& iTrig, const double& TTrigCut = -std::numeric_limits<double>::max());


};

#endif //WRATTER_INCLUDE_WRATTER_WRAPPER_HH_
