//
// Created by Stephane Zsoldos on 6/27/22.
//

#include <fstream>

#include <wRATter/Wrapper.hh>

int main(int argc, char* argv[]){

  if(argc<3){
	std::cout << "./ROOT2CSV in.root out.csv\n";
	return EXIT_FAILURE;
  }

  std::ofstream fCSVOut(argv[3], std::ios::out);
  fCSVOut << "ievt,itrig,pmt_pos_x,pmt_pos_y,pmt_pos_z,pmt_T,pmt_Q\n";


  wRAT w_rat(argv[2]);
  for(auto iEvt=0; iEvt<w_rat.GetNEvts(); iEvt++){
	w_rat.SetEvt(iEvt);
	for(auto iTrig=0; iTrig<w_rat.GetNTriggers(); iTrig++){
	  std::vector<Hit> vHits = w_rat.GetVHits(iTrig);
	  for(const auto& h:vHits)
		fCSVOut << iEvt << "," << iTrig
				<< "," << h.PMTPos.x() << "," << h.PMTPos.y() << "," << h.PMTPos.z()
				<< "," << h.T << "," << h.Q << std::endl;

	}
  }

  fCSVOut.close();

  return EXIT_SUCCESS;
}