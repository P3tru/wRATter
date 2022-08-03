//
// Created by Stephane Zsoldos on 6/27/22.
//

#ifndef WRATTER_INCLUDE_WRATTER_WRAPPER_HH_
#define WRATTER_INCLUDE_WRATTER_WRAPPER_HH_

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
 private:
  TChain *ChT;
  TChain *ChRunT;

  RAT::DS::Run *RUN;
  RAT::DS::Root *DS;

  int iEvt;
  int iTrig;
  RAT::DS::EV *EV;
  void Init();
 public:
  // ########################################## //
  // #### #### #### CONSTRUCTORS #### #### #### //
  // ########################################## //
  wRAT();
  explicit wRAT(const char *filename);
  explicit wRAT(const std::vector<const char *> &vFiles);
  void ReadFile(const char *filename);
  void ReadFiles(std::vector<const char *>filename);
  void Set();
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
  RAT::DS::Root *GetDS() const;
  void SetDS(RAT::DS::Root *ds);
  int GetIEvt() const;
  int GetITrig() const;
  // ##################################### //
  // #### #### #### METHODS #### #### #### //
  // ##################################### //
  // Point the class to a chosen event iEvt
  void SetEvt(int i);
  // Get total nb of events
  long GetNEvts();
  // Get Nb of recorded triggers
  // (this depends on the rat-pac DAQ)
  int GetNTriggers();
  // For a trigger iTrig, get trigger time
  double GetTriggerTime();

  // Get TRUE primary particle info
  int GetNPrimaryParticle();
  TVector3 GetPosTrue(int iParticle);
  TVector3 GetDirTrue(int iParticle);
  double GetTTrue(int iParticle);
  double GetETrue(int iParticle);
  void GetPrimaryParticleInfo(double &TriggerTime,
							  TVector3 &Pos, TVector3 &Dir,
							  double &E,
							  double &T);


  // Get Nb of recorded hits
  double GetQ();
  int GetNHits();

  // Create bool GetNextEvent()
  // Returns true if there is a next event
  bool GetNextEvent();
  // Create bool GetPrevEvent()
  // Returns true if there is a previous event
  bool GetPrevEvent();

  // Create bool GetNextTrigger()
  // Returns true if there is a next trigger
  bool GetNextTrigger();
  // Create bool GetPrevTrigger()
  // Returns true if there is a previous trigger
  bool GetPrevTrigger();

  // Create tag from Event and Trigger number
  std::string GetTag() const;



};

#endif //WRATTER_INCLUDE_WRATTER_WRAPPER_HH_
