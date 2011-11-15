#ifndef DiPhotonPurity_hh
#define DiPhotonPurity_hh


#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

#include <TH1D.h>

#include "base/TreeReader.hh"
#include "base/UserAnalysisBase.hh"

#include "TLorentzVector.h"

#include "EnergyCorrection.hh"

#define DEBUG 0

class DiPhotonPurity : public UserAnalysisBase{
public:
  DiPhotonPurity(TreeReader *tr = NULL, std::string dataType="data");
	virtual ~DiPhotonPurity();

	void Begin();
	void Analyze();
	void End();

private:

  EnergyCorrection *phocorr;
  TLorentzVector CorrPhoton(TreeReader *fTR, int i, int mode);
	
	// file for histograms:
	TFile *fHistFile;

  bool PhotonID_EGM_10_006_Loose(TreeReader *fTR, int i);

  std::string fDataType_;
  bool isdata;

  TH1F *fHNumPU;
  TH1F *fHNumVtx;

  TH1F *fHsieie;

};
#endif