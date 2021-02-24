//
// Created by zsoldos on 2/3/21.
//

#ifndef SEEDNDESTROY_WRATTER_INCLUDE_EVENTDISPLAY_HH_
#define SEEDNDESTROY_WRATTER_INCLUDE_EVENTDISPLAY_HH_

#include <map>

#include <TROOT.h>
#include <TStyle.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TEllipse.h>
#include <TMarker.h>
#include <TColor.h>

#include <TF2.h>

#include "Wrapper.hh"

static bool IsBarrel(const TVector3& v, const TVector3& vBnds){
  return std::abs(v.z()) < std::abs(vBnds.z());
}

typedef struct PMTGrid {
  std::map<unsigned int, TMarker*> mBarrel;
  std::map<unsigned int, TMarker*> mTop;
  std::map<unsigned int, TMarker*> mBottom;
  TVector3 vBnds;
} PMTGrid;

static TVector3 CreateVBnds(const double& radius, const double& halfheight){
  TVector3 vBnds(1., 1., 1.);
  vBnds.SetPerp(radius); vBnds.SetZ(halfheight);
  return vBnds;
}

static PMTGrid GetPMTGrid(wRAT& w_rat,
						  const TVector3& vBnds = CreateVBnds(9000., 35000.)){

  //
  // Init vMarkers obj
  //

  const unsigned int NPMTs = w_rat.GetRUN()->GetPMTInfo()->GetPMTCount();
  PMTGrid PMTGridMarkers;
  PMTGridMarkers.vBnds = vBnds;

  //
  // Load vector of PMTs
  //

  std::map<unsigned int, TVector3> vPMTs;
  for(auto iPMT=0; iPMT<NPMTs; iPMT++){
	vPMTs[iPMT] = w_rat.GetRUN()->GetPMTInfo()->GetPosition(iPMT);
  }

  for(const auto& p:vPMTs){

	if(IsBarrel(p.second, vBnds)){
	  // BARREL

	  PMTGridMarkers.mBarrel.insert(
		  std::make_pair(p.first, new TMarker(p.second.Phi()*vBnds.Perp(), p.second.z(), kFullDotLarge))
	  );

	} else {

	  if(p.second.z() > 0){
		// TOP

		PMTGridMarkers.mTop.insert(
			std::make_pair(p.first, new TMarker(p.second.x(), p.second.y(), kFullDotLarge))
		);

	  } else {
		// BOTTOM

		PMTGridMarkers.mBottom.insert(
			std::make_pair(p.first, new TMarker(p.second.x(), p.second.y(), kFullDotLarge))
		);

	  }

	}

  }


  return PMTGridMarkers;

}

// static void CreatePalette(std::vector<float> HSLR = {0.00, 0.59, 0.23} /*#5c1818*/,
// 						  std::vector<float> HSLG = {18.0, 0.54, 0.50} /*#c3633a*/,
// 						  std::vector<float> HSLB = {34.0, 0.54, 0.52} /*#c68d41*/){
//
//   const Int_t Number = 3;
//   Double_t Red[Number], Green[Number], Blue[Number];
//
//   auto GetDamnColor = [](const std::vector<float>& hsl, Double_t* Col){
// 	float a, b, c;
// 	TColor::HLS2RGB(hsl[0], hsl[1], hsl[2], a, b, c);
// 	Col[0] = a; Col[1] = b; Col[2] = c;
//   };
//
//   GetDamnColor(HSLR, Red);
//   GetDamnColor(HSLG, Green);
//   GetDamnColor(HSLB, Blue);
//   Double_t Length[Number] = { 0.00, 0.50, 1.00 };
//   Int_t nb=50;
//   TColor::CreateGradientColorTable(Number, Length, Red, Green, Blue, nb);
//   auto c2  = new TCanvas("c2","c2",0,0,600,400);
//   TF2 *f2 = new TF2("f2","0.1+(1-(x-2)*(x-2))*(1-(y-2)*(y-2))",1,3,1,3);
//   f2->SetContour(nb);
//   f2->SetLineWidth(1);
//   f2->SetLineColor(kBlack);
//   f2->Draw("surf1z");
//
// }

TCanvas *GetEventDisplay(const std::map<unsigned int, Hit>& mHits,
						 const PMTGrid& PMTGridMarkers,
						 const std::string& tag = "test",
						 const double PromptCut = 4.0){


  auto canvas = new TCanvas(Form("cED_%s", tag.c_str()), Form("cED_%s", tag.c_str()),
							800., 800.);

  auto pBarrel = new TPad(Form("pBarrel_%s", tag.c_str()), "pBarrel", 0., 0.25, 1., 0.75);
  pBarrel->SetFillColor(TColor::GetColor("#447666"));
  pBarrel->SetFillStyle(3244);
  pBarrel->Range(-PMTGridMarkers.vBnds.Perp(), -PMTGridMarkers.vBnds.z(), PMTGridMarkers.vBnds.Perp(), PMTGridMarkers.vBnds.z());

  enum {kBOTTOM=0, kTOP};
  std::vector<TPad*> vpCaps = {
	  new TPad(Form("pBarrel_%s", tag.c_str()), "pBarrel", 0.375, 0., 0.625, 0.25),
	  new TPad(Form("pBarrel_%s", tag.c_str()), "pBarrel", 0.375, 0.75, 0.625, 1.)
  };

  auto SetFillColorCaps = [](TPad* p, const int& col = TColor::GetColor("#447666")) {
	// p->SetFillColor(col);
	// p->SetFillStyle(3244);
  };
  SetFillColorCaps(vpCaps[kBOTTOM]);
  SetFillColorCaps(vpCaps[kTOP]);

  auto SetRangeCaps = [&PMTGridMarkers](TPad *p){
	p->Range(-PMTGridMarkers.vBnds.Perp(), -PMTGridMarkers.vBnds.Perp(), PMTGridMarkers.vBnds.Perp(), PMTGridMarkers.vBnds.Perp());
  };
  SetRangeCaps(vpCaps[kBOTTOM]);
  SetRangeCaps(vpCaps[kTOP]);

  //
  // DO THE BARREL
  //

  pBarrel->Draw();
  pBarrel->cd();

  auto IsPromptHit = [&PromptCut](const Hit& h){
	return h.T < PromptCut;
  };

  auto DrawMarker =
	  [&IsPromptHit](
		  const std::pair<unsigned int, Hit>& mHit,
		  const std::map<unsigned int, TMarker*>& mMarker){

		auto findHit = mMarker.find(mHit.first);
		if(findHit != mMarker.end()){
		  if(IsPromptHit(mHit.second))
			findHit->second->SetMarkerColor(kRed-4);
		  findHit->second->Draw();
		}

	  }
  ;

  for(auto& hit: mHits){
	DrawMarker(hit, PMTGridMarkers.mBarrel);
  }

  //
  // DO THE CAPS
  //

  auto DrawCaps = [](TPad* p){p->Draw(); p->cd();};
  auto eCap = new TEllipse(0.,0.,PMTGridMarkers.vBnds.Perp());
  eCap->SetFillColor(TColor::GetColor("#447666"));
  eCap->SetFillStyle(3249);

  canvas->cd();
  DrawCaps(vpCaps[kBOTTOM]);
  eCap->Draw("SAME");

  for(auto& hit: mHits){
	DrawMarker(hit, PMTGridMarkers.mBottom);
  }

  canvas->cd();
  DrawCaps(vpCaps[kTOP]);

  eCap->Draw("SAME");

  for(auto& hit: mHits){
	DrawMarker(hit, PMTGridMarkers.mTop);
  }

  return canvas;

}

#endif //SEEDNDESTROY_WRATTER_INCLUDE_EVENTDISPLAY_HH_
