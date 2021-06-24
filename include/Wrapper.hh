#ifndef WRATTER_LIBRARY_H
#define WRATTER_LIBRARY_H

// ####################################### //
// #### #### ####   C/C++   #### #### #### //
// ####################################### //
#include <vector>

// ####################################### //
// #### #### ####   ROOT    #### #### #### //
// ####################################### //
#include <TChain.h>

// ####################################### //
// #### #### ####    RAT    #### #### #### //
// ####################################### //
#include <RAT/DS/Run.hh>
#include <RAT/DS/Root.hh>

#include "Hit.hh"

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

  wRAT() {

    ChT = new TChain("T");
    ChRunT = new TChain("runT");

    RUN = new RAT::DS::Run();
    DS = new RAT::DS::Root();

  };

  explicit wRAT(const std::string& filename) : wRAT() {

    ChT->Add(filename.c_str());
    ChRunT->Add(filename.c_str());

    ChRunT->SetBranchAddress("run", &RUN);
    ChRunT->GetEntry(0);

    ChT->SetBranchAddress("ds", &DS);

  }

  explicit wRAT(const char *filename) : wRAT() {

    ChT->Add(filename);
    ChRunT->Add(filename);

    ChRunT->SetBranchAddress("run", &RUN);
    ChRunT->GetEntry(0);

    ChT->SetBranchAddress("ds", &DS);

  }

  explicit wRAT(const std::vector<std::string> &vFiles) : wRAT() {

    for (const auto &file:vFiles) {

      ChT->Add(file.c_str());
      ChRunT->Add(file.c_str());

    }

    ChRunT->SetBranchAddress("run", &RUN);
    ChRunT->GetEntry(0);

    ChT->SetBranchAddress("ds", &DS);

  }

  explicit wRAT(const std::vector<const char *> &vFiles) : wRAT() {

    for (const auto &file:vFiles) {

      ChT->Add(file);
      ChRunT->Add(file);

    }

    ChRunT->SetBranchAddress("run", &RUN);
    ChRunT->GetEntry(0);

    ChT->SetBranchAddress("ds", &DS);

  }

  // ######################################### //
  // #### #### #### DESTRUCTORS #### #### #### //
  // ######################################### //

  virtual ~wRAT() {
    delete RUN;
    delete DS;

    delete ChRunT;
    delete ChT;
  }

  // ############################################### //
  // #### #### #### GETTERS / SETTERS #### #### #### //
  // ############################################### //

  TChain *GetChT() { return ChT; }

  RAT::DS::Run *GetRUN() const { return RUN; }
  RAT::DS::Root *GetDS() const { return DS; }

  // ##################################### //
  // #### #### #### METHODS #### #### #### //
  // ##################################### //

  /*
    GLOBAL
    Acting at the wrapper level, at a collection of files.
  */

  // Point the class to a chosen event iEvt
  void SetEvt(unsigned iEvt) { ChT->GetEntry(iEvt); }
  // Get total nb of events
  unsigned long GetNEvts() { return static_cast<unsigned long>(ChT->GetEntries()); }

  /*
    LOCAL
    Inside a specific event where the wrapper points at.
    For better efficiency, use SetEvt to point in your macro
    at a specific event, and then use the local methods
    to access event specific information.
  */

  // Get Nb of recorded triggers
  // (this depends on the rat-pac DAQ)
  unsigned GetNTriggers() { return DS->GetEVCount(); }
  // For a trigger iTrig, get trigger time
  double GetTriggerTime(const int &iTrig) { return DS->GetEV(iTrig)->GetCalibratedTriggerTime(); }

  int GetNPrimaryParticle() { return DS->GetMC()->GetMCParticleCount(); }

  // Get TRUE primary particle info
  void GetPrimaryParticleInfo(const double &TriggerTime,
			      TVector3 &Pos, TVector3 &Dir,
			      double &E,
			      double &T) {

    auto nParticle = GetNPrimaryParticle();
    auto iParticle = nParticle > 1 ? (TriggerTime > 1e3 ? 1 : 0) : 0;

    Pos = DS->GetMC()->GetMCParticle(iParticle)->GetPosition();
    Dir = DS->GetMC()->GetMCParticle(iParticle)->GetMomentum().Unit();
    E = DS->GetMC()->GetMCParticle(iParticle)->GetKE();
    T = DS->GetMC()->GetMCParticle(iParticle)->GetTime();

  }

  TVector3 GetPosTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetPosition(); }
  TVector3 GetDirTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetMomentum().Unit(); }
  double GetTTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetTime(); }
  double GetETrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetKE(); }

  TVector3 GetPosRec(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetPosition(); }
  double GetTRec(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetTime(); }

  double GetChi2(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetGoodness(); }
  
  double GetQ(const int &iTrig) { return DS->GetEV(iTrig)->GetTotalCharge(); }
  int GetNHits(const int &iTrig) { return DS->GetEV(iTrig)->Nhits(); }

  std::vector<Hit> GetVHits(const int& iTrig, const double& TTrigCut = -std::numeric_limits<double>::max()) {

    std::vector<Hit> vHit;
    auto EV = DS->GetEV(iTrig);
    auto nPMTs = EV->GetPMTCount();

    for (auto iPMT = 0; iPMT < nPMTs; iPMT++) {

      auto PMT = EV->GetPMT(iPMT);
      auto ID = PMT->GetID();

      const auto PMTType = RUN->GetPMTInfo()->GetType(ID);

	  if (PMTType == 1) {
		auto T = PMT->GetTime();
		if(T>TTrigCut){
		  auto Pos = RUN->GetPMTInfo()->GetPosition(ID);
		  auto QHit = PMT->GetCharge();
		  Hit hit(Pos, QHit, T);
		  vHit.emplace_back(hit);
		}
	  }

    }

    return vHit;

  }

  std::map<unsigned int, Hit> GetMHits(const int& iTrig, const double& TTrigCut = 0.){

    std::map<unsigned int, Hit> mHits;

	auto EV = DS->GetEV(iTrig);
	auto nPMTs = EV->GetPMTCount();

	for (auto iPMT = 0; iPMT < nPMTs; iPMT++) {

	  auto PMT = EV->GetPMT(iPMT);
	  auto ID = PMT->GetID();

	  const auto PMTType = RUN->GetPMTInfo()->GetType(ID);

	  if (PMTType == 1) {
		auto Pos = RUN->GetPMTInfo()->GetPosition(ID);
		auto QHit = PMT->GetCharge();
		auto T = PMT->GetTime();
		Hit hit(Pos, QHit, T);
		mHits.insert(std::make_pair(ID, hit));
	  }

	}

	return mHits;

  }

  TH1D* GetHTRes(const int& iTrig, const std::string& tag = "hTRes",
				 const int& nBins = 600, const double& min =-200., const double& max = 400.) {

    TH1D* hTRes = new TH1D(tag.c_str(), " ; T_{Res} [ns] ; Counts ",
						   nBins, min, max);

	const auto TrigTime = GetTriggerTime(iTrig);
	auto iParticle = GetNPrimaryParticle() > 1 ? (TrigTime > 1e3 ? 1 : 0) : 0;

	auto vHits = GetVHits(iTrig);

    for(auto& h:vHits){
	  if(h.T>0.)
		hTRes->Fill(h.GetTRes(GetPosTrue(iParticle), -TrigTime));
    }

	return hTRes;

  }

};

#endif //WRATTER_LIBRARY_H
