//
// Created by zsoldos on 9/15/20.
//

#ifndef _HIT_HH_
#define _HIT_HH_

// ####################################### //
// #### #### ####   C/C++   #### #### #### //
// ####################################### //
#include <iostream>

#include <TVector3.h>

#define PI 3.14159265359
#define RINDEX_WATER 1.33
#define SOL_VACUUM 299.792
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

double GetQ(const std::vector<Hit>& vHits){
  double sumQ=0.;
  for(const auto& h: vHits){
    sumQ+=h.Q;
  }
  return sumQ;
}

double fweight(const Hit& hit, const unsigned int& wPower = 1){
  return wPower > 0 ? std::pow(hit.Q, wPower) : 1;
}


#endif //_HIT_HH_
