//
// Created by Stephane Zsoldos on 6/27/22.
//

#include <wRATter/Wrapper.hh>

wRAT::wRAT() {

  ChT = new TChain("T");
  ChRunT = new TChain("runT");

  RUN = new RAT::DS::Run();
  DS = new RAT::DS::Root();

}

wRAT::wRAT(const char *filename) : wRAT() {

  ChT->Add(filename);
  ChRunT->Add(filename);

  ChRunT->SetBranchAddress("run", &RUN);
  ChRunT->GetEntry(0);

  ChT->SetBranchAddress("ds", &DS);

}

wRAT::wRAT(const std::string &filename) : wRAT(filename.c_str()) {

}

wRAT::wRAT(const std::vector<const char *> &vFiles) : wRAT() {

  for (const auto &file:vFiles) {

	ChT->Add(file);
	ChRunT->Add(file);

  }

  ChRunT->SetBranchAddress("run", &RUN);
  ChRunT->GetEntry(0);

  ChT->SetBranchAddress("ds", &DS);

}

wRAT::wRAT(const std::vector<std::string> &vFiles) : wRAT() {

  for (const auto &file:vFiles) {

	ChT->Add(file.c_str());
	ChRunT->Add(file.c_str());

  }

  ChRunT->SetBranchAddress("run", &RUN);
  ChRunT->GetEntry(0);

  ChT->SetBranchAddress("ds", &DS);

}

wRAT::~wRAT() {
  delete RUN;
  delete DS;

  delete ChRunT;
  delete ChT;
}

TChain *wRAT::GetChT() const {
  return ChT;
}

void wRAT::SetChT(TChain *ch_t) {
  ChT = ch_t;
}

TChain *wRAT::GetChRunT() const {
  return ChRunT;
}

void wRAT::SetChRunT(TChain *ch_run_t) {
  ChRunT = ch_run_t;
}

RAT::DS::Run *wRAT::GetRun() const {
  return RUN;
}

void wRAT::SetRun(RAT::DS::Run *run) {
  RUN = run;
}

RAT::DS::Root *wRAT::GetDs() const {
  return DS;
}

void wRAT::SetDs(RAT::DS::Root *ds) {
  DS = ds;
}

void wRAT::SetEvt(unsigned int iEvt) {
  ChT->GetEntry(iEvt);
}

unsigned long wRAT::GetNEvts() {
  return static_cast<unsigned long>(ChT->GetEntries());
}

unsigned wRAT::GetNTriggers() {
  return DS->GetEVCount();
}

double wRAT::GetTriggerTime(const int &iTrig) {
  return DS->GetEV(iTrig)->GetCalibratedTriggerTime();
}

int wRAT::GetNPrimaryParticle() {
  return DS->GetMC()->GetMCParticleCount();
}

void wRAT::GetPrimaryParticleInfo(const double &TriggerTime, TVector3 &Pos, TVector3 &Dir, double &E, double &T) {

  auto nParticle = GetNPrimaryParticle();
  auto iParticle = nParticle > 1 ? (TriggerTime > 1e3 ? 1 : 0) : 0;

  Pos = DS->GetMC()->GetMCParticle(iParticle)->GetPosition();
  Dir = DS->GetMC()->GetMCParticle(iParticle)->GetMomentum().Unit();
  E = DS->GetMC()->GetMCParticle(iParticle)->GetKE();
  T = DS->GetMC()->GetMCParticle(iParticle)->GetTime();

}

TVector3 wRAT::GetPosTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetPosition(); }
TVector3 wRAT::GetDirTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetMomentum().Unit(); }
double wRAT::GetTTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetTime(); }
double wRAT::GetETrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetKE(); }

TVector3 wRAT::GetPosRec(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetPosition(); }
double wRAT::GetTRec(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetTime(); }

double wRAT::GetChi2(const int &iTrig) { return DS->GetEV(iTrig)->GetPathFit()->GetGoodness(); }

double wRAT::GetQ(const int &iTrig) { return DS->GetEV(iTrig)->GetTotalCharge(); }
int wRAT::GetNHits(const int &iTrig) { return DS->GetEV(iTrig)->Nhits(); }

std::vector<Hit> wRAT::GetVHits(const int& iTrig, const double& TTrigCut) {

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
