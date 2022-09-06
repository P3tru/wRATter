//
// Created by Stephane Zsoldos on 6/27/22.
//

#include <wRATter/Wrapper.hh>

void wRAT::Init() {
  ChT = new TChain("T");
  ChRunT = new TChain("runT");
  RUN = new RAT::DS::Run();
  DS = new RAT::DS::Root();
  EV = new RAT::DS::EV();
  iEvt = -1;
  iTrig = -1;
}
void wRAT::ReadFile(const char *filename) {
  ChT->Add(filename);
  ChRunT->Add(filename);
}
void wRAT::ReadFiles(std::vector<const char *> filename) {
  for (auto &f: filename) {
	ReadFile(f);
  }
}
void wRAT::Set(){
  ChRunT->SetBranchAddress("run", &RUN);
  ChRunT->GetEntry(0);
  ChT->SetBranchAddress("ds", &DS);
}

wRAT::wRAT() {
  Init();
}
wRAT::wRAT(const char *filename) : wRAT() {
  ReadFile(filename);
  Set();
}
wRAT::wRAT(const std::vector<const char *> &vFiles) : wRAT() {
  ReadFiles(vFiles);
  Set();
}

wRAT::~wRAT() {
  delete RUN;
  delete DS;

  delete ChRunT;
  delete ChT;

  delete EV;
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

RAT::DS::Root *wRAT::GetDS() const {
  return DS;
}
void wRAT::SetDS(RAT::DS::Root *ds) {
  DS = ds;
}

int wRAT::GetIEvt() const {
  return iEvt;
}
int wRAT::GetITrig() const {
  return iTrig;
}

void wRAT::SetEvt(int i) {
  ChT->GetEntry(i);
}

long wRAT::GetNEvts() {
  return ChT->GetEntries();
}
int wRAT::GetNTriggers() {
  return DS->GetEVCount();
}

double wRAT::GetTriggerTime() {
  return DS->GetEV(iTrig)->GetCalibratedTriggerTime();
}

int wRAT::GetNPrimaryParticle() {
  return DS->GetMC()->GetMCParticleCount();
}
TVector3 wRAT::GetPosTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetPosition(); }
TVector3 wRAT::GetDirTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetMomentum().Unit(); }
double wRAT::GetTTrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetTime(); }
double wRAT::GetETrue(int iParticle) { return DS->GetMC()->GetMCParticle(iParticle)->GetKE(); }

void wRAT::GetPrimaryParticleInfo(double &TriggerTime, TVector3 &Pos, TVector3 &Dir, double &E, double &T) {

  TriggerTime = GetTriggerTime();

  auto nParticle = GetNPrimaryParticle();
  auto iParticle = nParticle > 1 ? (TriggerTime > 1e3 ? 1 : 0) : 0;

  Pos = GetPosTrue(iParticle);
  Dir = GetDirTrue(iParticle);
  E = GetETrue(iParticle);
  T = GetTTrue(iParticle);

}

double wRAT::GetQ() { return DS->GetEV(iTrig)->GetTotalCharge(); }
int wRAT::GetNHits(){ return DS->GetEV(iTrig)->GetPMTCount(); }

bool wRAT::GetNextEvent() {
  if (++iEvt < GetNEvts()) {
    SetEvt(iEvt);
    return true;
  } else {
    iEvt = -1;
  	return false;
  }
}

bool wRAT::GetPrevEvent() {
  if (--iEvt >= 0) {
    SetEvt(iEvt);
    return true;
  } else {
    iEvt = GetNEvts();
  	return false;
  }
}

bool wRAT::GetNextTrigger() {
  if(++iTrig < GetNTriggers()) {
  	return true;
  } else {
    iTrig=-1;
    return false;
  }
}

bool wRAT::GetPrevTrigger() {
  if(--iTrig >= 0) {
  	return true;
  } else {
    iTrig=GetNTriggers();
    return false;
  }
}

std::string wRAT::GetTag() const {
  return Form("%d_%d", iEvt, iTrig);
}

int wRAT::GetNPMTs() {
  return RUN->GetPMTInfo()->GetPMTCount();
}
