//
// Created by zsoldos on 9/15/20.
//

#ifndef _HIT_HH_
#define _HIT_HH_

// ####################################### //
// #### #### ####   C/C++   #### #### #### //
// ####################################### //
#include <iostream>
#include <string>

#include <TVector3.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

#define PI 3.14159265359
#define RINDEX_WATER 1.33
#define SOL_VACUUM 299.792 // mm/ns
#define SOL SOL_VACUUM/RINDEX_WATER


static double rad2deg(double angrad){
  return angrad*180/PI;
}

static double deg2rad(double angdeg){
  return angdeg*PI/180;
}


typedef struct Hit {
  TVector3 PMTPos;
  double Q, T;

  // ######################################################## //
  // #### #### #### CONSTRUCTORS / DESTRUCTORS #### #### #### //
  // ######################################################## //

  Hit() {
    PMTPos = TVector3(0.,0.,0.);
    Q=0;
    T=0;
  }
  explicit Hit(const TVector3 &pmt_pos) : Hit() { PMTPos = pmt_pos; }
  explicit Hit(const double &t) : Hit() { T = t; }
  Hit(const double &q, const double &t) : Hit() { Q = q; T = t; }
  Hit(const TVector3& pos, const double& q, const double& t)
    : PMTPos(pos), Q(q), T(t){

  };
  double GetD(const TVector3& OrigPos) const {
    return (PMTPos - OrigPos).Mag();
  };
  double GetTRes(const TVector3& OrigPos, const double& OrigT, const double& SoL=SOL) const {
    return T-OrigT - GetD(OrigPos)/SoL;
  };
  double GetCosTheta(const TVector3& OrigPos, const TVector3& OrigDir) const {
    return OrigDir.Dot(PMTPos-OrigPos) / (PMTPos-OrigPos).Mag();
  };
  void Print() const {
    std::cout << T << " "
	      << Q << " ";
    PMTPos.Print();
  }


  bool IsCerHit(const TVector3& OrigPos, const TVector3& OrigDir,
		double Theta = 40.4/*deg*/,
		double minRing = 15./*deg*/, double maxRing = 15./*deg*/) const {

    double CT = GetCosTheta(OrigPos, OrigDir);

    double up_cher = cos(deg2rad(Theta) + deg2rad(maxRing));
    double down_cher = cos(deg2rad(Theta) - deg2rad(minRing));

    return CT > std::min(up_cher, down_cher) && CT < std::max(up_cher, down_cher);
  }

} Hit;

bool operator<(const Hit& h1, const Hit&h2){
  return h1.T<h2.T;
}
bool operator>(const Hit& h1, const Hit&h2){
  return h1.T>h2.T;
}

int GetNHits(const std::vector<Hit>& vHits){
  return vHits.size();
}

int GetNPrompts(const std::vector<Hit>& vHits, const int& TCutPrompt = 2.0/*ns*/){
  std::size_t nPrompts=0;
  for(auto& hit:vHits){
	if(hit.T < TCutPrompt){
	  nPrompts++;
	}
  }
  return nPrompts;
}

double GetQ(const std::vector<Hit>& vHits){
  double sumQ=0.;
  for(const auto& h: vHits){
    sumQ+=h.Q;
  }
  return sumQ;
}

static double GetQ2(const std::vector<Hit>& vHits){
  double sumQ2=0.;
  for(const auto& hit:vHits)
	sumQ2+=hit.Q*hit.Q;
  return sumQ2;
}


std::vector<Hit> GetVPromptHits(const std::vector<Hit>& vHits, const int& TCutPrompt = 2.0/*ns*/){
  std::vector<Hit> vPrompt;
  for(auto& hit:vHits){
    if(hit.T < TCutPrompt){
      vPrompt.emplace_back(hit);
    }
  }
  std::sort(vPrompt.begin(), vPrompt.end());
  return vPrompt;
}

double fweight(const Hit& hit, const unsigned int& wPower = 1){
  return wPower > 0 ? std::pow(hit.Q, wPower) : 1;
}

TH1D* GetTResHist(const std::string& tag, const std::vector<Hit>& vHits,
				  const TVector3& Pos, const double& T,
				  const unsigned int wPower = 1,
				  const unsigned int& nBins = 600, const double& min = -200, const double& max = 400){

  TH1D *hExp = new TH1D(Form("hTRes_%s", tag.c_str()), tag.c_str(), nBins, min, max);

  // Fill histogram to calculate NLL TRes
  for(auto& hit:vHits){
	hExp->Fill(hit.GetTRes(Pos, T), fweight(hit, wPower));
  }

  return hExp;

}

TH1D* GetTResHist(const std::string& tag, const std::vector<Hit>& vHits,
				  const std::vector<double>& x,
				  const unsigned int wPower = 1,
				  const unsigned int& nBins = 600, const double& min = -200, const double& max = 400){

  TH1D *hExp = new TH1D(Form("hTRes_%s", tag.c_str()), tag.c_str(), nBins, min, max);

  // Fill histogram to calculate NLL TRes
  for(auto& hit:vHits){
	hExp->Fill(hit.GetTRes(TVector3(x[0], x[1], x[2]), x[3]), fweight(hit, wPower));
  }

  return hExp;

}

TH3D* GetEventHist(const std::string& tag, const std::vector<Hit>& vHits, const TVector3& vBnds) {

  auto hEvt = new TH3D(Form("h3Evt_%s", tag.c_str()), " ; x [mm] ; y [mm] ; z [mm]",
					   10, -vBnds.x(), vBnds.x(),
					   10, -vBnds.y(), vBnds.y(),
					   10, -vBnds.z(), vBnds.z());

  for(auto& hit: vHits) {
    hEvt->Fill(hit.PMTPos.x(), hit.PMTPos.y(), hit.PMTPos.z(), hit.Q);
  }

  return hEvt;

}

TH2D* GetBarrelEventHist(const std::string& tag, const std::vector<Hit>& vHits, const double& z){

  auto hEvt = new TH2D(Form("h2Evt_%s", tag.c_str()), " ; #Phi [rad] ; z [mm]",
					   100, -1., 1.,
					   100, -z, z);

  for(auto& hit: vHits) {
	hEvt->Fill(hit.PMTPos.Phi(), hit.PMTPos.z(), hit.Q);
  }

  return hEvt;

}


#endif //_HIT_HH_
