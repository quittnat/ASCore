/*****************************************************************************
*   Collection of tools for producing plots for Muon Fake Rate Analysis      *
*                                                                            *
*                                                  (c) 2010 Benjamin Stieger *
*****************************************************************************/
#include "MuonPlotter.hh"
#include "helper/AnaClass.hh"
#include "helper/Utilities.hh"
#include "helper/FPRatios.hh"
#include "helper/FakeRatios.hh"
#include "helper/Monitor.hh"

#include "TLorentzVector.h"
#include "TGraphAsymmErrors.h"
#include "TEfficiency.h"

#include "TWbox.h"
#include "TMultiGraph.h"
#include "TGaxis.h"

#include <iostream>
#include <iomanip>
#include <time.h> // access to date/time

using namespace std;

//////////////////////////////////////////////////////////////////////////////////
// Global parameters:

static const float gMMU = 0.1057;
static const float gMEL = 0.0005;
static const float gMZ  = 91.2;

static const double gStatBetaAlpha = 1.;
static const double gStatBetaBeta  = 1.;

// Regions ///////////////////////////////////////////////////////////////////////
float MuonPlotter::Region::minMu1pt[2] = {20.,   5.};
float MuonPlotter::Region::minMu2pt[2] = {10.,   5.};
float MuonPlotter::Region::minEl1pt[2] = {20.,  10.};
float MuonPlotter::Region::minEl2pt[2] = {10.,  10.};

TString MuonPlotter::Region::sname [MuonPlotter::gNREGIONS] = {"HT80MET30", "HT80MET20to50", "HT80MET100", "HT200MET30", "HT200MET120", "HT400MET50", "HT400MET120", "HT400MET0",  "HT80METInv30", "HT0MET120"};
float MuonPlotter::Region::minHT   [MuonPlotter::gNREGIONS] = {        80.,             80.,          80.,         200.,          200.,         400.,          400.,         400.,            80.,          0.};
float MuonPlotter::Region::maxHT   [MuonPlotter::gNREGIONS] = {      7000.,           7000.,        7000.,        7000.,         7000.,        7000.,         7000.,        7000.,          7000.,       7000.};
float MuonPlotter::Region::minMet  [MuonPlotter::gNREGIONS] = {        30.,             20.,         100.,          30.,          120.,          50.,          120.,           0.,             0.,        120.};
float MuonPlotter::Region::maxMet  [MuonPlotter::gNREGIONS] = {      7000.,             50.,        7000.,        7000.,         7000.,        7000.,         7000.,        7000.,            30.,       7000.};
int   MuonPlotter::Region::minNjets[MuonPlotter::gNREGIONS] = {         2 ,              2 ,           2 ,           2 ,            2 ,           2 ,            2 ,           2 ,             2 ,          0 };

// Muon Binning //////////////////////////////////////////////////////////////////
double MuonPlotter::gMuPtbins [gNMuPtbins+1]  = {5., 10., 15., 20., 25., 35., 45, 60.};
double MuonPlotter::gMuPt2bins[gNMuPt2bins+1] = {5., 10., 15., 20., 25., 35., 45, 60.};
double MuonPlotter::gMuEtabins[gNMuEtabins+1] = {0., 1.0, 1.479, 2.0, 2.5};

// Electron Binning //////////////////////////////////////////////////////////////
double MuonPlotter::gElPtbins [gNElPtbins+1]  = {10., 15., 20., 25., 35., 55.};
double MuonPlotter::gElPt2bins[gNElPt2bins+1] = {10., 15., 20., 25., 35., 55.};
double MuonPlotter::gElEtabins[gNElEtabins+1] = {0., 1.0, 1.479, 2.0, 2.5};
//////////////////////////////////////////////////////////////////////////////////

// NVrtx Binning //////////////////////////////////////////////////////////////
double MuonPlotter::gNVrtxBins[gNNVrtxBins+1]  = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::gKinSelNames[gNKinSels] = {"LL", "TT", "Sig"};
TString MuonPlotter::KinPlots::var_name[MuonPlotter::gNKinVars] = {"HT", "MET", "NJets", "Pt1", "Pt2", "InvMassSF", "InvMassMM", "InvMassEE", "InvMassEM", "MT2"};
int     MuonPlotter::KinPlots::nbins[MuonPlotter::gNKinVars]    = { 20 ,   20 ,      8 ,   20 ,   20 ,        30  ,        30  ,        30  ,        30  ,   20 };
float   MuonPlotter::KinPlots::xmin[MuonPlotter::gNKinVars]     = {100.,    0.,      0.,   10.,   10.,        20. ,        20. ,        20. ,        20. ,    0.};
float   MuonPlotter::KinPlots::xmax[MuonPlotter::gNKinVars]     = {800.,  210.,      8.,  200.,  100.,       300. ,       300. ,       300. ,       300. ,  100.};
TString MuonPlotter::KinPlots::axis_label[MuonPlotter::gNKinVars] = {"H_{T} (GeV)",
	"E_{T}^{miss} (GeV)",
	"N_{Jets}",
	"P_{T} (l_{1}) (GeV)",
	"P_{T} (l_{2}) (GeV)",
	"m_{ll} (SF) (GeV)",
	"m_{#mu#mu} (GeV)",
	"m_{ee} (GeV)",
	"m_{ll} (OF) (GeV)",
	"M_{T2} (GeV)"};

//////////////////////////////////////////////////////////////////////////////////
double MuonPlotter::gDiffHTBins[gNDiffHTBins+1]   = { 0., 100., 200.,  300., 400., 900.};
double MuonPlotter::gDiffMETBins[gNDiffMETBins+1] = {30.,  60.,  90., 200.};
double MuonPlotter::gDiffNJBins[gNDiffNJBins+1]   = { 0.,   1.,   2.,    3.,   4.,   8.}; // fill NJets + 0.5 to hit the right bin
double MuonPlotter::gDiffMT2Bins[gNDiffMT2Bins+1] = { 0.,  25.,  50., 150.};
double MuonPlotter::gDiffPT1Bins[gNDiffPT1Bins+1] = { 20., 40., 60., 80., 100., 200.};
double MuonPlotter::gDiffPT2Bins[gNDiffPT2Bins+1] = { 10., 20., 30., 40.,  50., 100.};

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::DiffPredYields::var_name[MuonPlotter::gNDiffVars] = {"HT", "MET", "NJets", "MT2", "PT1", "PT2"};
int     MuonPlotter::DiffPredYields::nbins[MuonPlotter::gNDiffVars]    = {gNDiffHTBins, gNDiffMETBins, gNDiffNJBins, gNDiffMT2Bins, gNDiffPT1Bins, gNDiffPT2Bins};
double* MuonPlotter::DiffPredYields::bins[MuonPlotter::gNDiffVars]     = {gDiffHTBins,  gDiffMETBins,  gDiffNJBins,  gDiffMT2Bins,  gDiffPT1Bins,  gDiffPT2Bins};
TString MuonPlotter::DiffPredYields::axis_label[MuonPlotter::gNDiffVars] = {"H_{T} (GeV)",
	"E_{T}^{miss} (GeV)",
	"N_{Jets}",
	"M_{T2} (GeV)",
	"P_{T} (l_{1}) (GeV)",
	"P_{T} (l_{2}) (GeV)"};

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::FRatioPlots::var_name[MuonPlotter::gNRatioVars] = {"NJets",  "HT", "MaxJPt", "NVertices", "ClosJetPt", "AwayJetPt", "NBJets", "MET",  "MT"};
int     MuonPlotter::FRatioPlots::nbins[MuonPlotter::gNRatioVars]    = {     7 ,   20 ,      20 ,         9  ,        20  ,        20  ,       3 ,   10 ,   10 };
float   MuonPlotter::FRatioPlots::xmin[MuonPlotter::gNRatioVars]     = {     1.,   50.,      30.,         0. ,        30. ,        30. ,       0.,    0.,    0.};
float   MuonPlotter::FRatioPlots::xmax[MuonPlotter::gNRatioVars]     = {     8.,  500.,     300.,        18. ,       150. ,       300. ,       3.,   50.,  100.};

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::IsoPlots::sel_name[MuonPlotter::gNSels] = {"Base", "SigSup"};
int     MuonPlotter::IsoPlots::nbins[MuonPlotter::gNSels]    = {20, 20};


TString MuonPlotter::gEMULabel[2] = {"mu", "el"};
TString MuonPlotter::gChanLabel[3] = {"MM", "EM", "EE"};
TString MuonPlotter::gHiLoLabel[3] = {"HighPt", "LowPt", "TauChan"};

// Charge misid probability (from Hamed)
double MuonPlotter::gEChMisIDB   = 0.0002;
double MuonPlotter::gEChMisIDB_E = 0.0001;
double MuonPlotter::gEChMisIDE   = 0.0028;
double MuonPlotter::gEChMisIDE_E = 0.0004;

//____________________________________________________________________________
MuonPlotter::MuonPlotter(){
	// Default constructor, no samples are set
}

//____________________________________________________________________________
MuonPlotter::MuonPlotter(TString outputdir){
	// Explicit constructor with output directory
	setOutputDir(outputdir);
}

//____________________________________________________________________________
MuonPlotter::MuonPlotter(TString outputdir, TString outputfile){
	// Explicit constructor with output directory and output file
	setOutputDir(outputdir);
	setOutputFile(outputfile);
}

//____________________________________________________________________________
MuonPlotter::~MuonPlotter(){
	if(fOutputFile->IsOpen()) fOutputFile->Close();
	fChain = 0;
}

//____________________________________________________________________________
void MuonPlotter::init(TString filename){
	if(fVerbose > 0) cout << "------------------------------------" << endl;
	if(fVerbose > 0) cout << "Initializing MuonPlotter ... " << endl;
	if(fVerbose > 0) if(fChargeSwitch == 1) cout << " ... using opposite-sign charge selection" << endl;

	Util::SetStyle();
	// gROOT->SetStyle("Plain");
	readSamples(filename);
	readVarNames("anavarnames.dat");
	fOutputFileName = fOutputDir + "Yields.root";
	fLatex = new TLatex();
	fLatex->SetNDC(kTRUE);
	fLatex->SetTextColor(kBlack);
	fLatex->SetTextSize(0.04);

	fBinWidthScale = 10.; // Normalize Y axis to this binwidth
	fDoCounting = false; // Disable counters by default

	resetHypLeptons();
	initCutNames();

	fCurrentRun = -1;

	// Cuts:
	fC_minMu1pt = 20.;
	fC_minMu2pt = 10.;
	fC_minEl1pt = 20.;
	fC_minEl2pt = 10.;
	fC_minHT    = 80.;
	fC_minMet   = 30.;
	fC_maxHT    = 7000.;
	fC_maxMet   = 7000.;
	fC_minNjets = 2;

	fC_maxMet_Control = 20.;
	fC_maxMt_Control  = 20.;

	// Prevent root from adding histograms to current file
	TH1::AddDirectory(kFALSE);

	fMCBG.push_back(TTJets);
	fMCBG.push_back(TJets_t);
	fMCBG.push_back(TJets_tW);
	fMCBG.push_back(TJets_s);
	fMCBG.push_back(WJets);
	fMCBG.push_back(DYJets10to50);
	fMCBG.push_back(DYJets50);
	fMCBG.push_back(GJets40);
	fMCBG.push_back(GJets100);
	fMCBG.push_back(GJets200);
	fMCBG.push_back(WW);
	fMCBG.push_back(WZ);
	fMCBG.push_back(ZZ);
	// fMCBG.push_back(VVTo4L);
	fMCBG.push_back(GVJets);
	fMCBG.push_back(DPSWW);
	fMCBG.push_back(WWplus);
	fMCBG.push_back(WWminus);
	fMCBG.push_back(TTWplus);
	fMCBG.push_back(TTWminus);
	fMCBG.push_back(TTZplus);
	fMCBG.push_back(TTZminus);
	fMCBG.push_back(TTWWplus);
	fMCBG.push_back(TTWWminus);
	fMCBG.push_back(WWWplus);
	fMCBG.push_back(WWWminus);
	// fMCBG.push_back(WpWp);
	// fMCBG.push_back(WmWm);
	// fMCBG.push_back(ttbarW);

	// fMCBG.push_back(QCD5);
	// fMCBG.push_back(QCD15);
	// fMCBG.push_back(QCD30);
	// fMCBG.push_back(QCD50);
	// fMCBG.push_back(QCD80);
	// fMCBG.push_back(QCD120);
	// fMCBG.push_back(QCD170);
	// fMCBG.push_back(QCD300);
	// fMCBG.push_back(QCD470);
	// fMCBG.push_back(QCD600);
	// fMCBG.push_back(QCD800);
	// fMCBG.push_back(QCD1000);
	// fMCBG.push_back(QCD1400);
	// fMCBG.push_back(QCD1800);

	fMCBG.push_back(QCD50MG);
	fMCBG.push_back(QCD100MG);
	fMCBG.push_back(QCD250MG);
	fMCBG.push_back(QCD500MG);
	fMCBG.push_back(QCD1000MG);

	fMCBGSig = fMCBG;
	fMCBGSig.push_back(LM6);

	fMCBGMuEnr.push_back(TTJets);
	fMCBGMuEnr.push_back(TJets_t);
	fMCBGMuEnr.push_back(TJets_tW);
	fMCBGMuEnr.push_back(TJets_s);
	fMCBGMuEnr.push_back(WJets);
	fMCBGMuEnr.push_back(DYJets10to50);
	fMCBGMuEnr.push_back(DYJets50);
	fMCBGMuEnr.push_back(GJets40);
	fMCBGMuEnr.push_back(GJets100);
	fMCBGMuEnr.push_back(GJets200);
	fMCBGMuEnr.push_back(WW);
	fMCBGMuEnr.push_back(WZ);
	fMCBGMuEnr.push_back(ZZ);
	// fMCBGMuEnr.push_back(VVTo4L);
	fMCBGMuEnr.push_back(GVJets);
	fMCBGMuEnr.push_back(DPSWW);
	fMCBGMuEnr.push_back(WpWp);
	fMCBGMuEnr.push_back(WmWm);
	fMCBGMuEnr.push_back(ttbarW);
	fMCBGMuEnr.push_back(QCDMuEnr10);

	fMCBGMuEnrSig = fMCBGMuEnr;
	fMCBGMuEnrSig.push_back(LM6);

	fMCRareSM.push_back(WZ);
	fMCRareSM.push_back(ZZ);
	fMCRareSM.push_back(GVJets);
	fMCRareSM.push_back(DPSWW);
	fMCRareSM.push_back(WWplus);
	fMCRareSM.push_back(WWminus);
	fMCRareSM.push_back(TTWplus);
	fMCRareSM.push_back(TTWminus);
	fMCRareSM.push_back(TTZplus);
	fMCRareSM.push_back(TTZminus);
	fMCRareSM.push_back(TTWWplus);
	fMCRareSM.push_back(TTWWminus);
	fMCRareSM.push_back(WWWplus);
	fMCRareSM.push_back(WWWminus);
	// fMCRareSM.push_back(WpWp);
	// fMCRareSM.push_back(WmWm);
	// fMCRareSM.push_back(ttbarW);

	fMuData    .push_back(DoubleMu1);
	fMuData    .push_back(DoubleMu2);
	fMuData    .push_back(DoubleMu3);
	fMuData    .push_back(DoubleMu4);
	fMuHadData .push_back(MuHad1);
	fMuHadData .push_back(MuHad2);
	fEGData    .push_back(DoubleEle1);
	fEGData    .push_back(DoubleEle2);
	fEGData    .push_back(DoubleEle3);
	fEGData    .push_back(DoubleEle4);
	fEleHadData.push_back(EleHad1);
	fEleHadData.push_back(EleHad2);
	fMuEGData  .push_back(MuEG1);
	fMuEGData  .push_back(MuEG2);
	fMuEGData  .push_back(MuEG3);
	fMuEGData  .push_back(MuEG4);

	fHighPtData.push_back(DoubleMu1);
	fHighPtData.push_back(DoubleMu2);
	fHighPtData.push_back(DoubleMu3);
	fHighPtData.push_back(DoubleMu4);
	fHighPtData.push_back(DoubleEle1);
	fHighPtData.push_back(DoubleEle2);
	fHighPtData.push_back(DoubleEle3);
	fHighPtData.push_back(DoubleEle4);
	fHighPtData.push_back(MuEG1);
	fHighPtData.push_back(MuEG2);
	fHighPtData.push_back(MuEG3);
	fHighPtData.push_back(MuEG4);

	fLowPtData.push_back(MuHad1);
	fLowPtData.push_back(MuHad2);
	fLowPtData.push_back(EleHad1);
	fLowPtData.push_back(EleHad2);
	fLowPtData.push_back(MuEG1);
	fLowPtData.push_back(MuEG2);
	fLowPtData.push_back(MuEG3);
	fLowPtData.push_back(MuEG4);

	bookRatioHistos();
}
void MuonPlotter::InitMC(TTree *tree){
// Copied from MetaTreeClassBase, remove a few branches that are not in older version of minitrees

// The Init() function is called when the selector needs to initialize
// a new tree or chain. Typically here the branch addresses and branch
// pointers of the tree will be set.
// It is normally not necessary to make changes to the generated
// code, but the routine can be extended by the user if needed.
// Init() will be called many times when running on PROOF
// (once per file to be processed).

// Set branch addresses and branch pointers
	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("Run", &Run, &b_Run);
	fChain->SetBranchAddress("Event", &Event, &b_Event);
	fChain->SetBranchAddress("LumiSec", &LumiSec, &b_LumiSec);
	fChain->SetBranchAddress("Rho", &Rho, &b_Rho);
	fChain->SetBranchAddress("NVrtx", &NVrtx, &b_NVrtx);
	fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
	fChain->SetBranchAddress("NMus", &NMus, &b_NMus);
	fChain->SetBranchAddress("MuPt", MuPt, &b_MuPt);
	fChain->SetBranchAddress("MuEta", MuEta, &b_MuEta);
	fChain->SetBranchAddress("MuPhi", MuPhi, &b_MuPhi);
	fChain->SetBranchAddress("MuCharge", MuCharge, &b_MuCharge);
	fChain->SetBranchAddress("MuIso", MuIso, &b_MuIso);
	fChain->SetBranchAddress("MuD0", MuD0, &b_MuD0);
	fChain->SetBranchAddress("MuDz", MuDz, &b_MuDz);
	fChain->SetBranchAddress("MuPtE", MuPtE, &b_MuPtE);
	fChain->SetBranchAddress("MuGenID", MuGenID, &b_MuGenID);
	fChain->SetBranchAddress("MuGenMID", MuGenMID, &b_MuGenMID);
	fChain->SetBranchAddress("MuGenGMID", MuGenGMID, &b_MuGenGMID);
	fChain->SetBranchAddress("MuGenType", MuGenType, &b_MuGenType);
	fChain->SetBranchAddress("MuGenMType", MuGenMType, &b_MuGenMType);
	fChain->SetBranchAddress("MuGenGMType", MuGenGMType, &b_MuGenGMType);
	fChain->SetBranchAddress("MuMT", MuMT, &b_MuMT);
	fChain->SetBranchAddress("NEls", &NEls, &b_NEls);
	fChain->SetBranchAddress("ElCharge", ElCharge, &b_ElCharge);
	fChain->SetBranchAddress("ElChIsCons", ElChIsCons, &b_ElChIsCons);
	fChain->SetBranchAddress("ElPt", ElPt, &b_ElPt);
	fChain->SetBranchAddress("ElEta", ElEta, &b_ElEta);
	fChain->SetBranchAddress("ElPhi", ElPhi, &b_ElPhi);
	fChain->SetBranchAddress("ElD0", ElD0, &b_ElD0);
	fChain->SetBranchAddress("ElD0Err", ElD0Err, &b_ElD0Err);
	fChain->SetBranchAddress("ElDz", ElDz, &b_ElDz);
	fChain->SetBranchAddress("ElDzErr", ElDzErr, &b_ElDzErr);
	fChain->SetBranchAddress("ElRelIso", ElRelIso, &b_ElRelIso);
	fChain->SetBranchAddress("ElEcalRecHitSumEt", ElEcalRecHitSumEt, &b_ElEcalRecHitSumEt);
	fChain->SetBranchAddress("ElIsGoodElId_WP80", ElIsGoodElId_WP80, &b_ElIsGoodElId_WP80);
	fChain->SetBranchAddress("ElIsGoodElId_WP90", ElIsGoodElId_WP90, &b_ElIsGoodElId_WP90);
	fChain->SetBranchAddress("ElGenID", ElGenID, &b_ElGenID);
	fChain->SetBranchAddress("ElGenMID", ElGenMID, &b_ElGenMID);
	fChain->SetBranchAddress("ElGenGMID", ElGenGMID, &b_ElGenGMID);
	fChain->SetBranchAddress("ElGenType", ElGenType, &b_ElGenType);
	fChain->SetBranchAddress("ElGenMType", ElGenMType, &b_ElGenMType);
	fChain->SetBranchAddress("ElGenGMType", ElGenGMType, &b_ElGenGMType);
	fChain->SetBranchAddress("ElMT", ElMT, &b_ElMT);
	fChain->SetBranchAddress("tcMET", &tcMET, &b_tcMET);
	fChain->SetBranchAddress("tcMETPhi", &tcMETPhi, &b_tcMETPhi);
	fChain->SetBranchAddress("pfMET", &pfMET, &b_pfMET);
	fChain->SetBranchAddress("pfMETPhi", &pfMETPhi, &b_pfMETPhi);
	fChain->SetBranchAddress("NJets", &NJets, &b_NJets);
	fChain->SetBranchAddress("JetPt", JetPt, &b_JetPt);
	fChain->SetBranchAddress("JetEta", JetEta, &b_JetEta);
	fChain->SetBranchAddress("JetPhi", JetPhi, &b_JetPhi);
	fChain->SetBranchAddress("JetSSVHPBTag", JetSSVHPBTag, &b_JetSSVHPBTag);
	fChain->SetBranchAddress("JetArea", JetArea, &b_JetArea);
	Notify();
}
void MuonPlotter::readSamples(const char* filename){
	char buffer[200];
	ifstream IN(filename);

	char ParName[100], StringValue[1000];
	float ParValue;

	if(fVerbose > 2) cout << "------------------------------------" << endl;
	if(fVerbose > 2) cout << "Sample File  " << filename << endl;
	int counter(0);

	while( IN.getline(buffer, 200, '\n') ){
		// ok = false;
		if (buffer[0] == '#') continue; // Skip lines commented with '#'
		if( !strcmp(buffer, "SAMPLE")){
			Sample *s = new Sample();

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "Name\t%s", StringValue);
			s->name = TString(StringValue);

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "SName\t%s", StringValue);
			s->sname = TString(StringValue);

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "File\t%s", StringValue);
			s->location = TString(StringValue);

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "Lumi\t%f", &ParValue);
			s->lumi = ParValue;

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "DataMC\t%f", &ParValue);
			s->datamc = (int)ParValue;

			IN.getline(buffer, 200, '\n');
			sscanf(buffer, "Color\t%f", &ParValue);
			s->color = ParValue;

			if(fVerbose > 2){
				cout << " ---- " << endl;
				cout << "  New sample added: " << s->name << endl;
				cout << "   Sample no.  " << counter << endl;
				cout << "   Short name: " << s->sname << endl;
				cout << "   Lumi:       " << s->lumi << endl;
				cout << "   Color:      " << s->color << endl;
				cout << "   DataMC:     " << s->datamc << endl;
			}

			for(size_t hilo = 0; hilo < 2; ++hilo){
				for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
					Region *R = &(s->region[r][hilo]);
					for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
						Channel *C;
						if(c == Muon){
							C = &R->mm;
							C->name  = "Mu/Mu";
							C->sname = "MM";
						}
						if(c == Elec){
							C = &R->ee;
							C->name  = "El/El";
							C->sname = "EE";
						}
						if(c == ElMu){
							C = &R->em;
							C->name  = "El/Mu";
							C->sname = "EM";
						}
					}
				}
			}

			fSamples.push_back(s);
			fSampleMap[s->sname] = s;
			counter++;
		}
	}
	if(fVerbose > 2) cout << "------------------------------------" << endl;
}

//____________________________________________________________________________
const int     MuonPlotter::getNPtBins (gChannel chan){
	if(chan == Muon || chan == ElMu) return gNMuPtbins;
	if(chan == Elec) return gNElPtbins;
}
const double *MuonPlotter::getPtBins  (gChannel chan){
	if(chan == Muon || chan == ElMu) return gMuPtbins;
	if(chan == Elec) return gElPtbins;
}
const int     MuonPlotter::getNPt2Bins(gChannel chan){
	if(chan == Muon || chan == ElMu) return gNMuPt2bins;
	if(chan == Elec) return gNElPt2bins;
}
const double *MuonPlotter::getPt2Bins (gChannel chan){
	if(chan == Muon || chan == ElMu) return gMuPt2bins;
	if(chan == Elec) return gElPt2bins;
}
const int     MuonPlotter::getNEtaBins(gChannel chan){
	if(chan == Muon || chan == ElMu) return gNMuEtabins;
	if(chan == Elec)            return gNElEtabins;
}
const double *MuonPlotter::getEtaBins (gChannel chan){
	if(chan == Muon || chan == ElMu) return gMuEtabins;
	if(chan == Elec)            return gElEtabins;
}

//____________________________________________________________________________
void MuonPlotter::doAnalysis(){
	// sandBox();
	// return;

	if(readHistos(fOutputFileName) != 0) return;
	fLumiNorm = 2096.;
	printCutFlows(fOutputDir + "CutFlow.txt");

	// makePileUpPlots(); // loops on all data!
	// printSyncExercise();
	// makeIsoVsMETPlot(QCDMuEnr10);
	// makeIsoVsMETPlot(DoubleMu2);

	// printOrigins();
	// makeMuIsolationPlots(); // loops on TTbar sample
	// makeElIsolationPlots(); // loops on TTbar sample
	// makeNT2KinPlots();
	// makeMETvsHTPlot(fMuData, fEGData, fMuEGData, HighPt);
	// makeMETvsHTPlot(fMuHadData, fEleHadData, fMuEGData, LowPt);
	// makeMETvsHTPlotCustom();
	// makeMETvsHTPlotTau();
	// 
	// makeRatioPlots(Muon);
	// makeRatioPlots(Elec);
	// makeNTightLoosePlots(Muon);
	// makeNTightLoosePlots(Elec);

	// makeFRvsPtPlots(Muon, SigSup);
	// makeFRvsPtPlots(Elec, SigSup);
	// makeFRvsPtPlots(Muon, ZDecay);
	// makeFRvsPtPlots(Elec, ZDecay);
	// makeFRvsEtaPlots(Muon);
	// makeFRvsEtaPlots(Elec);

	// makeFRvsPtPlotsForPAS(Muon);
	// makeFRvsPtPlotsForPAS(Elec);
	// makeFRvsEtaPlotsForPAS(Muon);
	// makeFRvsEtaPlotsForPAS(Elec);

	// makeIntMCClosure(fOutputDir + "MCClosure.txt");	
	// makeTTbarClosure();

	makeAllIntPredictions();
	makeDiffPrediction();

	// cout << endl;
}

//____________________________________________________________________________
void MuonPlotter::sandBox(){
	fOutputSubDir = "sandbox/";
	vector<int> samples;
	// samples.push_back(DoubleMu1);
	samples.push_back(DoubleMu2);
	// samples.push_back(QCDMuEnr10);

	TH1D *hdphi1_da = new TH1D("hdphi1_da", "hdphi1_da", 20, 0., 3.1416);
	TH1D *hdphi2_da = new TH1D("hdphi2_da", "hdphi2_da", 20, 0., 3.1416);
	TH1D *hdphi1_mc = new TH1D("hdphi1_mc", "hdphi1_mc", 20, 0., 3.1416);
	TH1D *hdphi2_mc = new TH1D("hdphi2_mc", "hdphi2_mc", 20, 0., 3.1416);

	hdphi1_da->Sumw2();
	hdphi2_da->Sumw2();
	hdphi1_mc->Sumw2();
	hdphi2_mc->Sumw2();

	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];

		TTree *tree = S->getTree();
		tree->ResetBranchAddresses();
		if(S->datamc < 1) Init(tree);
		if(S->datamc > 0) InitMC(tree);
		for (Long64_t jentry=0; jentry<tree->GetEntriesFast();jentry++) {
			tree->GetEntry(jentry);
			printProgress(jentry, tree->GetEntriesFast(), S->name);

			if(singleMuTrigger() && isSigSupMuEvent()){
				setHypLepton1(0, Muon);
				int jind = getClosestJet(0, Muon);
				float dphi = Util::GetDeltaR(MuEta[0], JetEta[jind], MuPhi[0], JetPhi[jind]);
				if(getNJets() == 1){
					if(S->datamc == 0) hdphi1_da->Fill(dphi, singleMuPrescale());
					if(S->datamc > 0 ) hdphi1_mc->Fill(dphi);
				}
				if(getNJets() > 1){
					if(S->datamc == 0) hdphi2_da->Fill(dphi, singleMuPrescale());
					if(S->datamc > 0 ) hdphi2_mc->Fill(dphi);					
				}
			}
		}
		S->cleanUp();
	}

	printObject(hdphi1_da, "DR_1Jet_Data", "PEX");
	printObject(hdphi2_da, "DR_2Jet_Data", "PEX");
	// printObject(hdphi1_mc, "DPhi_1Jet_MC", "PEX");
	// printObject(hdphi2_mc, "DPhi_2Jet_MC", "PEX");
}

//____________________________________________________________________________
void MuonPlotter::makePileUpPlots(){
	fOutputSubDir = "PileUp/";
	bool write = true;

	TH1D *smu_nvertices, *dmu_nvertices, *sel_nvertices, *del_nvertices, *mue_nvertices;
	TH1D *mu_ntight, *mu_nloose, *mu_ratio, *el_ntight, *el_nloose, *el_ratio;

	if(!write){
		TFile *file = TFile::Open(fOutputDir + fOutputSubDir + "histos.root");
		smu_nvertices = (TH1D*)file->Get("smu_nvertices");
		dmu_nvertices = (TH1D*)file->Get("dmu_nvertices");
		sel_nvertices = (TH1D*)file->Get("sel_nvertices");
		del_nvertices = (TH1D*)file->Get("del_nvertices");
		mue_nvertices = (TH1D*)file->Get("mue_nvertices");
		mu_ratio      = (TH1D*)file->Get("mu_ratio");
		el_ratio      = (TH1D*)file->Get("el_ratio");
	}
	else{
		vector<gSample> samples;
		samples.push_back(DoubleMu1);
		samples.push_back(DoubleMu2);
		samples.push_back(DoubleMu3);
		samples.push_back(DoubleMu4);
		samples.push_back(MuEG1);
		samples.push_back(MuEG2);
		samples.push_back(MuEG3);
		samples.push_back(MuEG4);
		samples.push_back(DoubleEle1);
		samples.push_back(DoubleEle2);
		samples.push_back(DoubleEle3);
		samples.push_back(DoubleEle4);

		smu_nvertices = new TH1D("smu_nvertices", "smu_nvertices", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		dmu_nvertices = new TH1D("dmu_nvertices", "dmu_nvertices", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		sel_nvertices = new TH1D("sel_nvertices", "sel_nvertices", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		del_nvertices = new TH1D("del_nvertices", "del_nvertices", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		mue_nvertices = new TH1D("mue_nvertices", "mue_nvertices", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);

		mu_ntight = new TH1D("mu_ntight", "ntight", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		mu_nloose = new TH1D("mu_nloose", "nloose", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		mu_ratio  = new TH1D("mu_ratio",  "ratio",  FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		el_ntight = new TH1D("el_ntight", "ntight", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		el_nloose = new TH1D("el_nloose", "nloose", FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);
		el_ratio  = new TH1D("el_ratio",  "ratio",  FRatioPlots::nbins[3], FRatioPlots::xmin[3], FRatioPlots::xmax[3]);

		mu_ntight    ->Sumw2();
		mu_nloose    ->Sumw2();
		mu_ratio     ->Sumw2();
		el_ntight    ->Sumw2();
		el_nloose    ->Sumw2();
		el_ratio     ->Sumw2();
		smu_nvertices->Sumw2();
		dmu_nvertices->Sumw2();
		sel_nvertices->Sumw2();
		del_nvertices->Sumw2();
		mue_nvertices->Sumw2();
		mu_ntight    ->SetXTitle("N_{Vertices}");
		mu_nloose    ->SetXTitle("N_{Vertices}");
		mu_ratio     ->SetXTitle("N_{Vertices}");
		el_ntight    ->SetXTitle("N_{Vertices}");
		el_nloose    ->SetXTitle("N_{Vertices}");
		el_ratio     ->SetXTitle("N_{Vertices}");
		smu_nvertices->SetXTitle("N_{Vertices}");
		dmu_nvertices->SetXTitle("N_{Vertices}");
		sel_nvertices->SetXTitle("N_{Vertices}");
		del_nvertices->SetXTitle("N_{Vertices}");
		mue_nvertices->SetXTitle("N_{Vertices}");
		mu_ntight    ->SetYTitle("N_{Events}");
		mu_nloose    ->SetYTitle("N_{Events}");
		mu_ratio     ->SetYTitle("N_{Events}");
		el_ntight    ->SetYTitle("N_{Events}");
		el_nloose    ->SetYTitle("N_{Events}");
		el_ratio     ->SetYTitle("N_{Events}");
		smu_nvertices->SetYTitle("N_{Events}");
		dmu_nvertices->SetYTitle("N_{Events}");
		sel_nvertices->SetYTitle("N_{Events}");
		del_nvertices->SetYTitle("N_{Events}");
		mue_nvertices->SetYTitle("N_{Events}");
		mu_ntight    ->GetYaxis()->SetTitleOffset(1.2);
		mu_nloose    ->GetYaxis()->SetTitleOffset(1.2);
		mu_ratio     ->GetYaxis()->SetTitleOffset(1.2);
		el_ntight    ->GetYaxis()->SetTitleOffset(1.2);
		el_nloose    ->GetYaxis()->SetTitleOffset(1.2);
		el_ratio     ->GetYaxis()->SetTitleOffset(1.2);
		smu_nvertices->GetYaxis()->SetTitleOffset(1.2);
		dmu_nvertices->GetYaxis()->SetTitleOffset(1.2);
		sel_nvertices->GetYaxis()->SetTitleOffset(1.2);
		del_nvertices->GetYaxis()->SetTitleOffset(1.2);
		mue_nvertices->GetYaxis()->SetTitleOffset(1.2);

		for(size_t i = 0; i < samples.size(); ++i){
			Sample *S = fSamples[samples[i]];
			fCurrentSample = samples[i];
			resetHypLeptons();
			fDoCounting = false;

			TTree *tree = S->getTree();
			tree->ResetBranchAddresses();
			if(S->datamc < 1) Init(tree);
			if(S->datamc > 0) InitMC(tree);
			for (Long64_t jentry=0; jentry<tree->GetEntriesFast();jentry++) {
				tree->GetEntry(jentry);
				printProgress(jentry, tree->GetEntriesFast(), S->name);

				if(mumuSignalTrigger()) dmu_nvertices->Fill(NVrtx);
				if(elelSignalTrigger()) del_nvertices->Fill(NVrtx);
				if(elmuSignalTrigger()) mue_nvertices->Fill(NVrtx);
				if(singleMuTrigger()){
					smu_nvertices->Fill(NVrtx);
					if(isSigSupMuEvent()){
						if(isTightMuon(0)) mu_ntight->Fill(NVrtx);
						if(isLooseMuon(0)) mu_nloose->Fill(NVrtx);					
					}
				}
				resetHypLeptons();
				if(singleElTrigger()){
					sel_nvertices->Fill(NVrtx);
					if(isSigSupElEvent()){
						if(isTightElectron(0)) el_ntight->Fill(NVrtx);
						if(isLooseElectron(0)) el_nloose->Fill(NVrtx);
					}
				}
			}
			S->cleanUp();
		}

		mu_ratio->Divide(mu_ntight, mu_nloose, 1., 1., "B");
		mu_ratio->GetYaxis()->SetRangeUser(0., 0.4);
		el_ratio->Divide(el_ntight, el_nloose, 1., 1., "B");
		el_ratio->GetYaxis()->SetRangeUser(0., 0.4);

		dmu_nvertices->Scale(1./dmu_nvertices->Integral());
		smu_nvertices->Scale(1./smu_nvertices->Integral());
		del_nvertices->Scale(1./del_nvertices->Integral());
		sel_nvertices->Scale(1./sel_nvertices->Integral());
		mue_nvertices->Scale(1./mue_nvertices->Integral());
	}

	mu_ratio     ->SetYTitle("N_{Events} (Normalized)");
	el_ratio     ->SetYTitle("N_{Events} (Normalized)");
	smu_nvertices->SetYTitle("N_{Events} (Normalized)");
	dmu_nvertices->SetYTitle("N_{Events} (Normalized)");
	sel_nvertices->SetYTitle("N_{Events} (Normalized)");
	del_nvertices->SetYTitle("N_{Events} (Normalized)");
	mue_nvertices->SetYTitle("N_{Events} (Normalized)");

	mu_ratio     ->GetYaxis()->SetTitleOffset(1.3);
	el_ratio     ->GetYaxis()->SetTitleOffset(1.3);
	smu_nvertices->GetYaxis()->SetTitleOffset(1.3);
	dmu_nvertices->GetYaxis()->SetTitleOffset(1.3);
	sel_nvertices->GetYaxis()->SetTitleOffset(1.3);
	del_nvertices->GetYaxis()->SetTitleOffset(1.3);
	mue_nvertices->GetYaxis()->SetTitleOffset(1.3);

	// Color_t colors[6] = {1, 1, 1, 1, 1, 1};
	Color_t colors[5] = {31, 41, 51, 61, 81};
	// Color_t colors[6] = {1, 12, 39, 38, 32, 30};//, 29};
	Style_t styles[6] = {24, 25, 26, 27, 32, 30};//, 29};

	// dmu_nvertices->SetMarkerStyle(styles[0]);
	// smu_nvertices->SetMarkerStyle(styles[1]);
	// del_nvertices->SetMarkerStyle(styles[2]);
	// sel_nvertices->SetMarkerStyle(styles[3]);
	// mue_nvertices->SetMarkerStyle(styles[4]);
	// dmu_nvertices->SetMarkerColor(colors[0]);
	// smu_nvertices->SetMarkerColor(colors[1]);
	// del_nvertices->SetMarkerColor(colors[2]);
	// sel_nvertices->SetMarkerColor(colors[3]);
	// mue_nvertices->SetMarkerColor(colors[4]);
	dmu_nvertices->SetLineColor(colors[0]);
	smu_nvertices->SetLineColor(colors[1]);
	del_nvertices->SetLineColor(colors[2]);
	sel_nvertices->SetLineColor(colors[3]);
	mue_nvertices->SetLineColor(colors[4]);
	// dmu_nvertices->SetMarkerSize(1.5);
	// smu_nvertices->SetMarkerSize(1.5);
	// del_nvertices->SetMarkerSize(1.5);
	// sel_nvertices->SetMarkerSize(1.5);
	// mue_nvertices->SetMarkerSize(1.5);
	dmu_nvertices->SetLineWidth(2);
	smu_nvertices->SetLineWidth(2);
	del_nvertices->SetLineWidth(2);
	sel_nvertices->SetLineWidth(2);
	mue_nvertices->SetLineWidth(2);
	dmu_nvertices->SetFillStyle(0);
	smu_nvertices->SetFillStyle(0);
	del_nvertices->SetFillStyle(0);
	sel_nvertices->SetFillStyle(0);
	mue_nvertices->SetFillStyle(0);

	dmu_nvertices->GetYaxis()->SetRangeUser(0., 0.4);
	smu_nvertices->GetYaxis()->SetRangeUser(0., 0.4);
	del_nvertices->GetYaxis()->SetRangeUser(0., 0.4);
	sel_nvertices->GetYaxis()->SetRangeUser(0., 0.4);
	mue_nvertices->GetYaxis()->SetRangeUser(0., 0.4);

	mu_ratio->SetMarkerStyle(20);
	mu_ratio->SetMarkerColor(kBlue);
	mu_ratio->SetLineColor(kBlue);
	mu_ratio->SetMarkerSize(1.8);
	mu_ratio->SetLineWidth(2);
	el_ratio->SetMarkerStyle(21);
	el_ratio->SetMarkerColor(kRed);
	el_ratio->SetLineColor(kRed);
	el_ratio->SetMarkerSize(1.8);
	el_ratio->SetLineWidth(2);

	// printObject(mu_ratio, "MuRatio", "PEX");
	// printObject(el_ratio, "ElRatio", "PEX");
	// printObject(dmu_nvertices, "NVertices_DMu", "L");
	// printObject(smu_nvertices, "NVertices_SMu", "L");
	// printObject(del_nvertices, "NVertices_DEl", "L");
	// printObject(sel_nvertices, "NVertices_SEl", "L");
	// printObject(mue_nvertices, "NVertices_MuE", "L");

	// TLegend *leg = new TLegend(0.35,0.15,0.70,0.40);
	TLegend *leg = new TLegend(0.50,0.60,0.88,0.88);
	leg->AddEntry(dmu_nvertices, Form("DoubleMu Trig., Mean = %4.2f", dmu_nvertices->GetMean()),  "l");
	leg->AddEntry(smu_nvertices, Form("SingleMu Trig., Mean = %4.2f", smu_nvertices->GetMean()),  "l");
	leg->AddEntry(del_nvertices, Form("DoubleEle Trig., Mean = %4.2f", del_nvertices->GetMean()), "l");
	leg->AddEntry(sel_nvertices, Form("SingleEle Trig., Mean = %4.2f", sel_nvertices->GetMean()), "l");
	leg->AddEntry(mue_nvertices, Form("MuEle Trig., Mean = %4.2f", mue_nvertices->GetMean()),   "l");
	leg->AddEntry(mu_ratio, Form("TL Ratio (Muons)"), "p");
	leg->AddEntry(el_ratio, Form("TL Ratio (Electrons)"),  "p");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	// leg->SetTextSize(0.03);
	leg->SetBorderSize(0);

	TCanvas *c_temp = new TCanvas("C_temp", "HT vs MET in Data vs MC", 0, 0, 800, 600);
	c_temp->cd();
	dmu_nvertices->Draw("axis");
	dmu_nvertices->DrawCopy("hist L same");
	smu_nvertices->DrawCopy("hist L same");
	del_nvertices->DrawCopy("hist L same");
	sel_nvertices->DrawCopy("hist L same");
	mue_nvertices->DrawCopy("hist L same");
	mu_ratio->DrawCopy("PE X0 same");
	el_ratio->DrawCopy("PE X0 same");
	TGaxis *axis = new TGaxis(18, 0, 18, 0.4, 0, 0.4, 510, "+L");
	axis->SetLabelFont(42);
	axis->SetTitleFont(42);
	axis->SetTitleOffset(1.2);
	axis->SetTitle("TL Ratio");
	axis->Draw();
	leg->Draw();
	drawTopLine();
	// fLatex->DrawLatex(0.10,0.92, fSamples[sample]->name);
	// Util::PrintNoEPS( c_temp, "PileUp", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "PileUp", fOutputDir + fOutputSubDir);

	if(write){
		TFile *file = new TFile(fOutputDir + fOutputSubDir + "histos.root", "RECREATE");
		mu_ratio->Write(mu_ratio->GetName(), TObject::kWriteDelete);
		el_ratio->Write(el_ratio->GetName(), TObject::kWriteDelete);
		dmu_nvertices->Write(dmu_nvertices->GetName(), TObject::kWriteDelete);
		smu_nvertices->Write(smu_nvertices->GetName(), TObject::kWriteDelete);
		del_nvertices->Write(del_nvertices->GetName(), TObject::kWriteDelete);
		sel_nvertices->Write(sel_nvertices->GetName(), TObject::kWriteDelete);
		mue_nvertices->Write(mue_nvertices->GetName(), TObject::kWriteDelete);
		file->Close();
	}

	// Cleanup
	delete leg, c_temp;
	if(write) delete mu_ntight, mu_nloose, el_ntight, el_nloose, mu_ratio, el_ratio;
	else delete mu_ratio, el_ratio;
	delete dmu_nvertices, smu_nvertices, del_nvertices, sel_nvertices, mue_nvertices;
	fOutputSubDir = "";
}

//____________________________________________________________________________
void MuonPlotter::doLoop(){
	fDoCounting = true;
	TString eventfilename  = fOutputDir + "SSDLSignalEvents.txt";
	fOUTSTREAM.open(eventfilename.Data(), ios::trunc);

	TFile *pFile = new TFile(fOutputFileName, "RECREATE");

	// Sample loop
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		fCurrentSample = i;

		// if(i > DoubleMu1) break;

		bookHistos(S);

		TTree *tree = S->getTree();
		const bool isdata = (S->datamc)==0;

		// Stuff to execute for each sample BEFORE looping on the events
		initCounters(i);

		// Event loop
		tree->ResetBranchAddresses();
		// Init(tree);
		if(S->datamc == 0) Init(tree);
		else InitMC(tree);

		if (fChain == 0) return;
		Long64_t nentries = fChain->GetEntriesFast();
		Long64_t nbytes = 0, nb = 0;
		for (Long64_t jentry=0; jentry<nentries;jentry++) {
			printProgress(jentry, nentries, S->name);

			Long64_t ientry = LoadTree(jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(jentry);   nbytes += nb;

			if(fCurrentRun != Run){ // get trigger names for each new run
				fCurrentRun = Run;
			}

			fCounters[fCurrentSample][Muon].fill(fMMCutNames[0]);
			fCounters[fCurrentSample][ElMu].fill(fEMCutNames[0]);
			fCounters[fCurrentSample][Elec].fill(fEECutNames[0]);

			// Select mutually exclusive runs for Jet and MultiJet datasets
			if(!isGoodRun(i)) continue;

			fCounters[fCurrentSample][Muon].fill(fMMCutNames[1]);
			fCounters[fCurrentSample][ElMu].fill(fEMCutNames[1]);
			fCounters[fCurrentSample][Elec].fill(fEECutNames[1]);

			fillKinPlots(i, HighPt);
			fillKinPlots(i, LowPt);
			for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)) fillYields(S, r, HighPt);
			for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)) fillYields(S, r, LowPt);

			fillDiffYields(S);
			fillRatioPlots(S);
			fillMuIsoPlots(i);
			fillElIsoPlots(i);

		}

		// Stuff to execute for each sample AFTER looping on the events
		fillCutFlowHistos(i);

		writeHistos(S, pFile);
		writeSigGraphs(S, Muon, HighPt, pFile);
		writeSigGraphs(S, Muon, LowPt,  pFile);
		writeSigGraphs(S, Elec, HighPt, pFile);
		writeSigGraphs(S, Elec, LowPt,  pFile);
		writeSigGraphs(S, ElMu, HighPt, pFile);
		writeSigGraphs(S, ElMu, LowPt,  pFile);

		deleteHistos(S);
		S->cleanUp();
	}

	pFile->Write();
	pFile->Close();

	fOUTSTREAM.close();
	fDoCounting = false;
}

//____________________________________________________________________________
void MuonPlotter::makeMCClosurePlots(vector<int> samples){
	fillMuElRatios(samples); // Fill the ratios
	fixPRatios(); // puts p ratios to 1 whereever they are 0 (no entries)

	makeSSMuMuPredictionPlots(samples, false);
	makeSSElElPredictionPlots(samples, false);	
	makeSSElMuPredictionPlots(samples, false);

	makeNT012Plots(samples, Muon,     Baseline);
	makeNT012Plots(samples, Elec, Baseline);
	makeNT012Plots(samples, ElMu,      Baseline);
}
void MuonPlotter::makeNT012Plots(vector<int> mcsamples, gChannel chan, gRegion reg, gHiLoSwitch hilo){
	TString name;
	if(chan == Muon)     name = "MuMu";
	if(chan == Elec) name = "ElEl";
	if(chan == ElMu)      name = "ElMu";

	fOutputSubDir = name + "Predictions";

	THStack *hnt2_stack = new THStack(Form("%s_nt2_stack", name.Data()), "Observed Nt2");
	THStack *hnt1_stack = new THStack(Form("%s_nt1_stack", name.Data()), "Observed Nt1");
	THStack *hnt0_stack = new THStack(Form("%s_nt0_stack", name.Data()), "Observed Nt0");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D* hnt2[nmcsamples];
	TH1D* hnt1[nmcsamples];
	TH1D* hnt0[nmcsamples];

	for(size_t i = 0; i < mcsamples.size(); ++i){
		Sample *S = fSamples[mcsamples[i]];
		float scale = fLumiNorm / S->lumi;
		Channel *cha;
		if(chan == Muon)     cha = &S->region[reg][hilo].mm;
		if(chan == Elec) cha = &S->region[reg][hilo].ee;
		if(chan == ElMu)      cha = &S->region[reg][hilo].em;
		hnt2[i] = (TH1D*)(cha->nt20_pt->ProjectionX())->Clone();
		hnt1[i] = (TH1D*)(cha->nt10_pt->ProjectionX())->Clone();
		hnt0[i] = (TH1D*)(cha->nt00_pt->ProjectionX())->Clone();
		hnt2[i]->Scale(scale);
		hnt1[i]->Scale(scale);
		hnt0[i]->Scale(scale);

		hnt2[i]->SetFillColor(S->color);
		hnt1[i]->SetFillColor(S->color);
		hnt0[i]->SetFillColor(S->color);

		hnt2[i] = normHistBW(hnt2[i], fBinWidthScale);
		hnt1[i] = normHistBW(hnt1[i], fBinWidthScale);
		hnt0[i] = normHistBW(hnt0[i], fBinWidthScale);

		hnt2_stack->Add(hnt2[i]);
		hnt1_stack->Add(hnt1[i]);
		hnt0_stack->Add(hnt0[i]);
	}

	hnt2_stack->Draw();
	hnt2_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnt1_stack->Draw();
	hnt1_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnt0_stack->Draw();
	hnt0_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));

	TCanvas *c2 = new TCanvas(Form("%s_Nt2", name.Data()), "Observed Nt2", 0, 0, 800, 600);
	TCanvas *c1 = new TCanvas(Form("%s_Nt1", name.Data()), "Observed Nt1", 0, 0, 800, 600);
	TCanvas *c0 = new TCanvas(Form("%s_Nt0", name.Data()), "Observed Nt0", 0, 0, 800, 600);

	// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		int index = mcsamples[i];
		leg->AddEntry(hnt2[i], (fSamples[index]->sname).Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c2->cd();
	hnt2_stack->Draw("hist");
	leg->Draw();

	c1->cd();
	hnt1_stack->Draw("hist");
	leg->Draw();

	c0->cd();
	hnt0_stack->Draw("hist");
	leg->Draw();


	Util::PrintNoEPS(c2, name + "ObservedNt2", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c1, name + "ObservedNt1", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c0, name + "ObservedNt0", fOutputDir + fOutputSubDir, fOutputFile);
	delete c2, c1, c0;
	// for(size_t i = 0; i < nmcsamples; ++i){
	// 	delete hnt2[i];
	// 	delete hnt1[i];
	// 	delete hnt0[i];
	// }
	fOutputSubDir = "";
}
void MuonPlotter::makeNT012Plots(gChannel chan, vector<int> mcsamples, bool(MuonPlotter::*eventSelector)(int&, int&), TString tag){
	const bool read = false;

	THStack *hnt20_stack = new THStack("nt20_stack", "Observed Nt20");
	THStack *hnt10_stack = new THStack("nt10_stack", "Observed Nt10");
	THStack *hnt01_stack = new THStack("nt01_stack", "Observed Nt01");
	THStack *hnt00_stack = new THStack("nt00_stack", "Observed Nt00");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D *hnt20[nmcsamples];
	TH1D *hnt10[nmcsamples];
	TH1D *hnt01[nmcsamples];
	TH1D *hnt00[nmcsamples];

	if(read){
		hnt20_stack = (THStack*)fOutputFile->Get(fOutputDir + "/nt20_stack");
		hnt10_stack = (THStack*)fOutputFile->Get(fOutputDir + "/nt10_stack");
		hnt01_stack = (THStack*)fOutputFile->Get(fOutputDir + "/nt01_stack");
		hnt00_stack = (THStack*)fOutputFile->Get(fOutputDir + "/nt00_stack");
		TList *list20 = hnt20_stack->GetHists();
		TList *list10 = hnt10_stack->GetHists();
		TList *list01 = hnt01_stack->GetHists();
		TList *list00 = hnt00_stack->GetHists();
		if(list20->GetSize() != nmcsamples || list10->GetSize() != nmcsamples || list00->GetSize() != nmcsamples) return;
		for(size_t i = 0; i < nmcsamples; ++i){
			int index = mcsamples[i];
			hnt20[i] = (TH1D*)list20->At(i);
			hnt20[i]->SetFillColor(fSamples[index]->color);
			hnt10[i] = (TH1D*)list10->At(i);
			hnt10[i]->SetFillColor(fSamples[index]->color);
			hnt01[i] = (TH1D*)list01->At(i);
			hnt01[i]->SetFillColor(fSamples[index]->color);
			hnt00[i] = (TH1D*)list00->At(i);
			hnt00[i]->SetFillColor(fSamples[index]->color);
		}
	}

	if(!read){
		TTree *tree = NULL;
		for(size_t i = 0; i < mcsamples.size(); ++i){
			int index = mcsamples[i];
			tree = fSamples[index]->getTree();
			hnt20[i] = new TH1D(Form("nt20_%s", fSamples[index]->sname.Data()), "Observed Nt20", getNPt2Bins(Muon), getPt2Bins(Muon));
			hnt10[i] = new TH1D(Form("nt10_%s", fSamples[index]->sname.Data()), "Observed Nt10", getNPt2Bins(Muon), getPt2Bins(Muon));
			hnt01[i] = new TH1D(Form("nt01_%s", fSamples[index]->sname.Data()), "Observed Nt01", getNPt2Bins(Muon), getPt2Bins(Muon));
			hnt00[i] = new TH1D(Form("nt00_%s", fSamples[index]->sname.Data()), "Observed Nt00", getNPt2Bins(Muon), getPt2Bins(Muon));
			hnt20[i]->SetFillColor(fSamples[index]->color);
			hnt10[i]->SetFillColor(fSamples[index]->color);
			hnt01[i]->SetFillColor(fSamples[index]->color);
			hnt00[i]->SetFillColor(fSamples[index]->color);
			hnt20[i]->Sumw2();
			hnt10[i]->Sumw2();
			hnt01[i]->Sumw2();
			hnt00[i]->Sumw2();
			float scale = fLumiNorm / fSamples[index]->lumi;
			if(fSamples[index]->datamc == 0) scale = 1;
			tree->ResetBranchAddresses();
			Init(tree);
			if (fChain == 0) return;
			Long64_t nentries = fChain->GetEntriesFast();
			Long64_t nbytes = 0, nb = 0;
			for (Long64_t jentry=0; jentry<nentries;jentry++) {
				Long64_t ientry = LoadTree(jentry);
				if (ientry < 0) break;
				if(fVerbose > 1) printProgress(jentry, nentries, fSamples[index]->name);
				nb = fChain->GetEntry(jentry);   nbytes += nb;

				int ind1(-1), ind2(-1);
				if((*this.*eventSelector)(ind1, ind2) == false) continue;

				if(chan == Muon){
					if( isTightMuon(ind1) &&  isTightMuon(ind2)) hnt20[i]->Fill(MuPt[ind1], scale);
					if( isTightMuon(ind1) && !isTightMuon(ind2)) hnt10[i]->Fill(MuPt[ind1], scale);
					if(!isTightMuon(ind1) &&  isTightMuon(ind2)) hnt10[i]->Fill(MuPt[ind2], scale);
					if(!isTightMuon(ind1) && !isTightMuon(ind2)) hnt00[i]->Fill(MuPt[ind1], scale);
				}
				if(chan == Elec){
					if( isTightElectron(ind1) &&  isTightElectron(ind2)) hnt20[i]->Fill(ElPt[ind1], scale);
					if( isTightElectron(ind1) && !isTightElectron(ind2)) hnt10[i]->Fill(ElPt[ind1], scale);
					if(!isTightElectron(ind1) &&  isTightElectron(ind2)) hnt10[i]->Fill(ElPt[ind2], scale);
					if(!isTightElectron(ind1) && !isTightElectron(ind2)) hnt00[i]->Fill(ElPt[ind1], scale);
				}
				if(chan == ElMu){
					if( isTightMuon(ind1) &&  isTightElectron(ind2)) hnt20[i]->Fill(MuPt[ind1], scale);
					if( isTightMuon(ind1) && !isTightElectron(ind2)) hnt10[i]->Fill(MuPt[ind1], scale);
					if(!isTightMuon(ind1) &&  isTightElectron(ind2)) hnt01[i]->Fill(MuPt[ind1], scale);
					if(!isTightMuon(ind1) && !isTightElectron(ind2)) hnt00[i]->Fill(MuPt[ind1], scale);
				}
			}
			hnt20_stack->Add(hnt20[i]);
			hnt10_stack->Add(hnt10[i]);
			hnt01_stack->Add(hnt01[i]);
			hnt00_stack->Add(hnt00[i]);
			if(fVerbose > 1) cout << endl;
			fSamples[index]->cleanUp();
		}
	}

	hnt20_stack->Draw();
	hnt20_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnt10_stack->Draw();
	hnt10_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnt01_stack->Draw();
	hnt01_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnt00_stack->Draw();
	hnt00_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));

	TCanvas *c20 = new TCanvas("Nt20", "Observed Nt20", 0, 0, 800, 600);
	TCanvas *c10 = new TCanvas("Nt10", "Observed Nt10", 0, 0, 800, 600);
	TCanvas *c01 = new TCanvas("Nt01", "Observed Nt01", 0, 0, 800, 600);
	TCanvas *c00 = new TCanvas("Nt00", "Observed Nt00", 0, 0, 800, 600);

	// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		int index = mcsamples[i];
		leg->AddEntry(hnt20[i], fSamples[index]->sname.Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c20->cd();
	hnt20_stack->Draw("hist");
	leg->Draw();

	c10->cd();
	hnt10_stack->Draw("hist");
	leg->Draw();

	c01->cd();
	hnt01_stack->Draw("hist");
	leg->Draw();

	c00->cd();
	hnt00_stack->Draw("hist");
	leg->Draw();

	Util::PrintNoEPS(c20, tag + "ObservedNt20", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c10, tag + "ObservedNt10", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c01, tag + "ObservedNt01", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c00, tag + "ObservedNt00", fOutputDir + fOutputSubDir, fOutputFile);
}

//____________________________________________________________________________
void MuonPlotter::makeMuIsolationPlots(){
	fOutputSubDir = "MuIsoPlots/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TH1D    *hiso_data [gNSels];
	TH1D    *hiso_mc   [gNSels];
	TH1D	*hiso_ttbar[gNSels];
	THStack *hiso_mc_s [gNSels];

	TH1D    *hiso_data_pt [gNSels][gNMuPt2bins];
	TH1D    *hiso_mc_pt   [gNSels][gNMuPt2bins];
	TH1D    *hiso_ttbar_pt[gNSels][gNMuPt2bins];
	THStack *hiso_mc_pt_s [gNSels][gNMuPt2bins];

	TH1D    *hiso_data_nv [gNSels][gNNVrtxBins];
	TH1D    *hiso_mc_nv   [gNSels][gNNVrtxBins];
	TH1D    *hiso_ttbar_nv[gNSels][gNNVrtxBins];
	THStack *hiso_mc_nv_s [gNSels][gNNVrtxBins];

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	// Create histograms
	for(size_t i = 0; i < gNSels; ++i){
		hiso_data[i]  = new TH1D("MuIsoData_"          + IsoPlots::sel_name[i], "Muon Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
		hiso_mc[i]    = new TH1D("MuIsoMC_"            + IsoPlots::sel_name[i], "Muon Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
		hiso_ttbar[i] = new TH1D("MuIsoTTbar_"         + IsoPlots::sel_name[i], "Muon Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
		hiso_mc_s[i]  = new THStack("MuIsoMC_stacked_" + IsoPlots::sel_name[i], "Muon Isolation in MC for "    + IsoPlots::sel_name[i]);
		hiso_data[i]  ->Sumw2();
		hiso_mc[i]    ->Sumw2();
		hiso_ttbar[i] ->Sumw2();

		for(int k = 0; k < gNMuPt2bins; ++k){
			hiso_data_pt [i][k] = new TH1D(Form("MuIsoData_%s_Pt%d",          IsoPlots::sel_name[i].Data(), k), "Muon Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_pt   [i][k] = new TH1D(Form("MuIsoMC_%s_Pt%d",            IsoPlots::sel_name[i].Data(), k), "Muon Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_ttbar_pt[i][k] = new TH1D(Form("MuIsoTTbar_%s_Pt%d",         IsoPlots::sel_name[i].Data(), k), "Muon Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_pt_s [i][k] = new THStack(Form("MuIsoMC_stacked_%s_Pt%d", IsoPlots::sel_name[i].Data(), k), "Muon Isolation in MC for "    + IsoPlots::sel_name[i]);
			hiso_data_pt [i][k]->Sumw2();
			hiso_mc_pt   [i][k]->Sumw2();
			hiso_ttbar_pt[i][k]->Sumw2();
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			hiso_data_nv [i][k] = new TH1D(Form("MuIsoData_%s_NVtx%d",          IsoPlots::sel_name[i].Data(), k), "Muon Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_nv   [i][k] = new TH1D(Form("MuIsoMC_%s_NVtx%d",            IsoPlots::sel_name[i].Data(), k), "Muon Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_ttbar_nv[i][k] = new TH1D(Form("MuIsoTTbar_%s_NVtx%d",         IsoPlots::sel_name[i].Data(), k), "Muon Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_nv_s [i][k] = new THStack(Form("MuIsoMC_stacked_%s_NVtx%d", IsoPlots::sel_name[i].Data(), k), "Muon Isolation in MC for "    + IsoPlots::sel_name[i]);
			hiso_data_nv [i][k]->Sumw2();
			hiso_mc_nv   [i][k]->Sumw2();
			hiso_ttbar_nv[i][k]->Sumw2();
		}
	}

	////////////////////////////////////////////////////
	// Fill ttbar histos
	// Sample loop
	TTree *tree = fSamples[TTJets]->getTree();

	// Event loop
	tree->ResetBranchAddresses();
	InitMC(tree);

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		printProgress(jentry, nentries, fSamples[TTJets]->name);

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		int muind1(-1), muind2(-1);
		if(hasLooseMuons(muind1, muind2) < 1) continue;

		// Common event selections
		if(!passesJet50Cut()) continue; // make trigger 100% efficient

		// Common object selections
		if(!isLooseMuon(muind1)) continue;
		if(MuPt[muind1] < fC_minMu2pt) continue;
		if(MuPt[muind1] > gMuPt2bins[gNMuPt2bins]) continue;

		// Select genmatched fake muons
		if(isPromptMuon(muind1)) continue;

		////////////////////////////////////////////////////
		// MOST LOOSE SELECTION
		hiso_ttbar[0]->Fill(MuIso[muind1]);
		for(size_t k = 0; k < gNMuPt2bins; ++k){
			if(MuPt[muind1] < gMuPt2bins[k]) continue;
			if(MuPt[muind1] > gMuPt2bins[k+1]) continue;
			hiso_ttbar_pt[0][k]->Fill(MuIso[muind1]);
		}
		for(size_t k = 0; k < gNNVrtxBins; ++k){
			if(NVrtx < gNVrtxBins[k]) continue;
			if(NVrtx > gNVrtxBins[k+1]) continue;
			hiso_ttbar_nv[0][k]->Fill(MuIso[muind1]);
		}

		////////////////////////////////////////////////////
		// SIGNAL SUPPRESSED SELECTION
		if(isSigSupMuEvent()){
			hiso_ttbar[1]->Fill(MuIso[muind1]);
			for(size_t k = 0; k < gNMuPt2bins; ++k){
				if(MuPt[muind1] < gMuPt2bins[k]) continue;
				if(MuPt[muind1] > gMuPt2bins[k+1]) continue;
				hiso_ttbar_pt[1][k]->Fill(MuIso[muind1]);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				if(NVrtx < gNVrtxBins[k]) continue;
				if(NVrtx > gNVrtxBins[k+1]) continue;
				hiso_ttbar_nv[1][k]->Fill(MuIso[muind1]);
			}
		}
		// ////////////////////////////////////////////////////
		// // SIGNAL SELECTION
		// if(isSSLLMuEvent(muind1, muind2)){
		// 	int fakemu = muind1;
		// 	if(isPromptMuon(muind1) &&  isPromptMuon(muind2)) continue;
		// 	if(isPromptMuon(muind1) && !isPromptMuon(muind2)) fakemu = muind2;
		// 	
		// 	hiso_ttbar[1]->Fill(MuIso[fakemu]);
		// 	for(size_t k = 0; k < gNMuPt2bins; ++k){
		// 		if(MuPt[fakemu] < gMuPt2bins[k]) continue;
		// 		if(MuPt[fakemu] > gMuPt2bins[k+1]) continue;
		// 		hiso_ttbar_pt[1][k]->Fill(MuIso[fakemu]);
		// 	}
		// 	for(size_t k = 0; k < gNNVrtxBins; ++k){
		// 		if(NVrtx < gNVrtxBins[k]) continue;
		// 		if(NVrtx > gNVrtxBins[k+1]) continue;
		// 		hiso_ttbar_nv[1][k]->Fill(MuIso[fakemu]);
		// 	}
		// }
		////////////////////////////////////////////////////
	}
	fSamples[TTJets]->cleanUp();
	cout << endl;
	////////////////////////////////////////////////////


	// Create plots
	vector<int> mcsamples = fMCBGMuEnr;
	// vector<int> datasamples = fJMData;
	vector<int> datasamples = fMuData;

	for(size_t i = 0; i < gNSels; ++i){
		hiso_data[i]->SetXTitle(convertVarName("MuIso[0]"));
		hiso_data[i]->SetLineWidth(3);
		hiso_data[i]->SetLineColor(kBlack);
		hiso_data[i]->SetMarkerStyle(8);
		hiso_data[i]->SetMarkerColor(kBlack);
		hiso_data[i]->SetMarkerSize(1.2);

		hiso_ttbar[i]->SetXTitle(convertVarName("MuIso[0]"));
		hiso_ttbar[i]->SetLineWidth(3);
		hiso_ttbar[i]->SetLineColor(kRed);
		hiso_ttbar[i]->SetMarkerStyle(23);
		hiso_ttbar[i]->SetMarkerColor(kRed);
		hiso_ttbar[i]->SetMarkerSize(1.3);

		for(int k = 0; k < gNMuPt2bins; ++k){
			hiso_data_pt[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			hiso_data_pt[i][k]->SetLineWidth(3);
			hiso_data_pt[i][k]->SetLineColor(kBlack);
			hiso_data_pt[i][k]->SetMarkerStyle(8);
			hiso_data_pt[i][k]->SetMarkerColor(kBlack);
			hiso_data_pt[i][k]->SetMarkerSize(1.2);

			hiso_ttbar_pt[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			hiso_ttbar_pt[i][k]->SetLineWidth(3);
			hiso_ttbar_pt[i][k]->SetLineColor(kRed);
			hiso_ttbar_pt[i][k]->SetMarkerStyle(23);
			hiso_ttbar_pt[i][k]->SetMarkerColor(kRed);
			hiso_ttbar_pt[i][k]->SetMarkerSize(1.3);
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			hiso_data_nv[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			hiso_data_nv[i][k]->SetLineWidth(3);
			hiso_data_nv[i][k]->SetLineColor(kBlack);
			hiso_data_nv[i][k]->SetMarkerStyle(8);
			hiso_data_nv[i][k]->SetMarkerColor(kBlack);
			hiso_data_nv[i][k]->SetMarkerSize(1.2);

			hiso_ttbar_nv[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			hiso_ttbar_nv[i][k]->SetLineWidth(3);
			hiso_ttbar_nv[i][k]->SetLineColor(kRed);
			hiso_ttbar_nv[i][k]->SetMarkerStyle(23);
			hiso_ttbar_nv[i][k]->SetMarkerColor(kRed);
			hiso_ttbar_nv[i][k]->SetMarkerSize(1.3);
		}

		// Apply weights to MC histos
		for(size_t j = 0; j < gNSAMPLES; ++j){
			Sample *S = fSamples[j];
			float lumiscale = fLumiNorm / S->lumi;
			if(S->datamc == 0) continue;
			S->isoplots[0].hiso[i]->Scale(lumiscale);
			for(size_t k = 0; k < gNMuPt2bins; ++k){
				S->isoplots[0].hiso_pt[i][k]->Scale(lumiscale);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				S->isoplots[0].hiso_nv[i][k]->Scale(lumiscale);
			}
		}

		// Fill data histo
		for(size_t j = 0; j < datasamples.size(); ++j){
			Sample *S = fSamples[datasamples[j]];
			hiso_data[i]->Add(S->isoplots[0].hiso[i]);
			hiso_data[i]->SetXTitle(convertVarName("MuIso[0]"));
			for(int k = 0; k < gNMuPt2bins; ++k){
				hiso_data_pt[i][k]->Add(S->isoplots[0].hiso_pt[i][k]);
				hiso_data_pt[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				hiso_data_nv[i][k]->Add(S->isoplots[0].hiso_nv[i][k]);
				hiso_data_nv[i][k]->SetXTitle(convertVarName("MuIso[0]"));
			}
		}

		// Scale to get equal integrals
		float intscale(0.);
		float intscale_pt[gNMuPt2bins];
		float intscale_nv[gNNVrtxBins];
		for(size_t j = 0; j < mcsamples.size(); ++j){
			Sample *S = fSamples[mcsamples[j]];
			intscale += S->isoplots[0].hiso[i]->Integral();
			for(int k = 0; k < gNMuPt2bins; ++k){
				intscale_pt[k] += S->isoplots[0].hiso_pt[i][k]->Integral();
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				intscale_nv[k] += S->isoplots[0].hiso_nv[i][k]->Integral();
			}
		}
		intscale = hiso_data[i]->Integral() / intscale;
		for(size_t j = 0; j < gNMuPt2bins; ++j) intscale_pt[j] = hiso_data_pt[i][j]->Integral() / intscale_pt[j];
		for(size_t j = 0; j < gNNVrtxBins; ++j) intscale_nv[j] = hiso_data_nv[i][j]->Integral() / intscale_nv[j];

		for(size_t j = 0; j < mcsamples.size(); ++j){
			Sample *S = fSamples[mcsamples[j]];
			S->isoplots[0].hiso[i]->Scale(intscale);
			for(int k = 0; k < gNMuPt2bins; ++k) S->isoplots[0].hiso_pt[i][k]->Scale(intscale_pt[k]);
			for(int k = 0; k < gNNVrtxBins; ++k) S->isoplots[0].hiso_nv[i][k]->Scale(intscale_nv[k]);
		}
		hiso_ttbar[i]->Scale(hiso_data[i]->Integral() / hiso_ttbar[i]->Integral());
		for(int k = 0; k < gNMuPt2bins; ++k) hiso_ttbar_pt[i][k]->Scale(hiso_data_pt[i][k]->Integral() / hiso_ttbar_pt[i][k]->Integral());
		for(int k = 0; k < gNNVrtxBins; ++k) hiso_ttbar_nv[i][k]->Scale(hiso_data_nv[i][k]->Integral() / hiso_ttbar_nv[i][k]->Integral());


		// Fill MC stacks
		for(size_t j = 0; j < mcsamples.size();   ++j){
			Sample *S = fSamples[mcsamples[j]];
			hiso_mc  [i]->Add(S->isoplots[0].hiso[i]);
			hiso_mc_s[i]->Add(S->isoplots[0].hiso[i]);
			hiso_mc_s[i]->Draw("goff");
			hiso_mc_s[i]->GetXaxis()->SetTitle(convertVarName("MuIso[0]"));
			for(int k = 0; k < gNMuPt2bins; ++k){
				hiso_mc_pt  [i][k]->Add(S->isoplots[0].hiso_pt[i][k]);
				hiso_mc_pt_s[i][k]->Add(S->isoplots[0].hiso_pt[i][k]);
				hiso_mc_pt_s[i][k]->Draw("goff");
				hiso_mc_pt_s[i][k]->GetXaxis()->SetTitle(convertVarName("MuIso[0]"));
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				hiso_mc_nv  [i][k]->Add(S->isoplots[0].hiso_nv[i][k]);
				hiso_mc_nv_s[i][k]->Add(S->isoplots[0].hiso_nv[i][k]);
				hiso_mc_nv_s[i][k]->Draw("goff");
				hiso_mc_nv_s[i][k]->GetXaxis()->SetTitle(convertVarName("MuIso[0]"));
			}
		}

		double max1 = hiso_mc_s[i]->GetMaximum();
		double max2 = hiso_data[i]->GetMaximum();
		double max = max1>max2?max1:max2;
		hiso_mc_s[i]->SetMaximum(1.5*max);
		hiso_data[i]->SetMaximum(1.5*max);

		int bin0   = hiso_data[i]->FindBin(0.0);
		int bin015 = hiso_data[i]->FindBin(0.15) - 1; // bins start at lower edge...
		int bin1   = hiso_data[i]->FindBin(1.0)  - 1;
		float ratio_data  = hiso_data[i] ->Integral(bin0, bin015) / hiso_data[i] ->Integral(bin0, bin1);
		float ratio_mc    = hiso_mc[i]   ->Integral(bin0, bin015) / hiso_mc[i]   ->Integral(bin0, bin1);
		float ratio_ttbar = hiso_ttbar[i]->Integral(bin0, bin015) / hiso_ttbar[i]->Integral(bin0, bin1);

		TCanvas *c_temp = new TCanvas("MuIso" + IsoPlots::sel_name[i], "Muon Isolation in Data vs MC", 0, 0, 800, 600);
		c_temp->cd();

		TLegend *leg = new TLegend(0.15,0.65,0.40,0.88);
		// TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
		leg->AddEntry(hiso_data[i], "Data","p");
		leg->AddEntry(hiso_ttbar[i], "TTbar fake","p");
		for(size_t j = 0; j < mcsamples.size(); ++j) leg->AddEntry(fSamples[mcsamples[j]]->isoplots[0].hiso[i], fSamples[mcsamples[j]]->sname.Data(), "f");
		leg->SetFillStyle(0);
		leg->SetTextFont(42);
		leg->SetBorderSize(0);

		// gPad->SetLogy();
		hiso_mc_s[i]->Draw("hist");
		hiso_ttbar[i]->DrawCopy("PE X0 same");
		hiso_data[i]->DrawCopy("PE X0 same");
		leg->Draw();
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
		lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
		lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
		lat->SetTextColor(kRed);
		lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
		lat->SetTextColor(kBlack);


		// Util::PrintNoEPS(c_temp, "MuIso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir, NULL);
		Util::PrintPDF(c_temp, "MuIso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir);

		for(int k = 0; k < gNMuPt2bins; ++k){
			ratio_data  = hiso_data_pt[i][k] ->Integral(bin0, bin015) / hiso_data_pt[i][k] ->Integral(bin0, bin1);
			ratio_mc    = hiso_mc_pt[i][k]   ->Integral(bin0, bin015) / hiso_mc_pt[i][k]   ->Integral(bin0, bin1);
			ratio_ttbar = hiso_ttbar_pt[i][k]->Integral(bin0, bin015) / hiso_ttbar_pt[i][k]->Integral(bin0, bin1);

			TCanvas *c_temp = new TCanvas(Form("MuIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), "Muon Isolation in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			max1 = hiso_mc_pt_s[i][k]->GetMaximum();
			max2 = hiso_data_pt[i][k]->GetMaximum();
			max = max1>max2?max1:max2;
			hiso_mc_pt_s[i][k]->SetMaximum(1.5*max);
			hiso_data_pt[i][k]->SetMaximum(1.5*max);

			TLegend *leg_pt = new TLegend(0.15,0.65,0.40,0.88);
			// TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
			leg_pt->AddEntry(hiso_data_pt[i][k], "Data","p");
			leg_pt->AddEntry(hiso_ttbar_pt[i][k], "TTbar fake","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg_pt->AddEntry(fSamples[mcsamples[j]]->isoplots[0].hiso_pt[i][k], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg_pt->SetFillStyle(0);
			leg_pt->SetTextFont(42);
			leg_pt->SetBorderSize(0);

			// gPad->SetLogy();
			hiso_mc_pt_s[i][k]->Draw("hist");
			hiso_ttbar_pt[i][k]->DrawCopy("PE X0 same");
			hiso_data_pt[i][k]->DrawCopy("PE X0 same");
			leg_pt->Draw();
			lat->DrawLatex(0.20,0.92, Form("p_{T}(#mu) %3.0f - %3.0f GeV", getPt2Bins(Muon)[k], getPt2Bins(Muon)[k+1]));
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			// Util::PrintNoEPS(c_temp, Form("MuIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);
			Util::PrintPDF(c_temp, Form("MuIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir);
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			ratio_data  = hiso_data_nv[i][k] ->Integral(bin0, bin015) / hiso_data_nv[i][k] ->Integral(bin0, bin1);
			ratio_mc    = hiso_mc_nv[i][k]   ->Integral(bin0, bin015) / hiso_mc_nv[i][k]   ->Integral(bin0, bin1);
			ratio_ttbar = hiso_ttbar_nv[i][k]->Integral(bin0, bin015) / hiso_ttbar_nv[i][k]->Integral(bin0, bin1);

			TCanvas *c_temp = new TCanvas(Form("MuIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), "Muon Isolation in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			max1 = hiso_mc_nv_s[i][k]->GetMaximum();
			max2 = hiso_data_nv[i][k]->GetMaximum();
			max = max1>max2?max1:max2;
			hiso_mc_nv_s[i][k]->SetMaximum(1.5*max);
			hiso_data_nv[i][k]->SetMaximum(1.5*max);

			TLegend *leg_nv = new TLegend(0.15,0.65,0.40,0.88);
			// TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
			leg_nv->AddEntry(hiso_data_nv[i][k], "Data","p");
			leg_nv->AddEntry(hiso_ttbar_nv[i][k], "TTbar fake","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg_nv->AddEntry(fSamples[mcsamples[j]]->isoplots[0].hiso_nv[i][k], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg_nv->SetFillStyle(0);
			leg_nv->SetTextFont(42);
			leg_nv->SetBorderSize(0);

			// gPad->SetLogy();
			hiso_mc_nv_s[i][k]->Draw("hist");
			hiso_ttbar_nv[i][k]->DrawCopy("PE X0 same");
			hiso_data_nv[i][k]->DrawCopy("PE X0 same");
			leg_nv->Draw();
			lat->DrawLatex(0.20,0.92, Form("N_{Vrtx.} %2.0f - %2.0f", gNVrtxBins[k], gNVrtxBins[k+1]));
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			// Util::PrintNoEPS(c_temp, Form("MuIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);
			Util::PrintPDF(c_temp, Form("MuIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir);
		}
	}
}
void MuonPlotter::makeElIsolationPlots(){
	fOutputSubDir = "ElIsoPlots/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TH1D    *hiso_data [gNSels];
	TH1D    *hiso_mc   [gNSels];
	TH1D	*hiso_ttbar[gNSels];
	THStack *hiso_mc_s [gNSels];

	TH1D    *hiso_data_pt [gNSels][gNElPt2bins];
	TH1D    *hiso_mc_pt   [gNSels][gNElPt2bins];
	TH1D    *hiso_ttbar_pt[gNSels][gNElPt2bins];
	THStack *hiso_mc_pt_s [gNSels][gNElPt2bins];

	TH1D    *hiso_data_nv [gNSels][gNNVrtxBins];
	TH1D    *hiso_mc_nv   [gNSels][gNNVrtxBins];
	TH1D    *hiso_ttbar_nv[gNSels][gNNVrtxBins];
	THStack *hiso_mc_nv_s [gNSels][gNNVrtxBins];

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	// Create histograms
	for(size_t i = 0; i < gNSels; ++i){
		hiso_data[i]  = new TH1D("ElIsoData_"          + IsoPlots::sel_name[i], "Electron Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
		hiso_mc[i]    = new TH1D("ElIsoMC_"            + IsoPlots::sel_name[i], "Electron Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
		hiso_ttbar[i] = new TH1D("ElIsoTTbar_"         + IsoPlots::sel_name[i], "Electron Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
		hiso_mc_s[i]  = new THStack("ElIsoMC_stacked_" + IsoPlots::sel_name[i], "Electron Isolation in MC for "    + IsoPlots::sel_name[i]);
		hiso_data[i]  ->Sumw2();
		hiso_mc[i]    ->Sumw2();
		hiso_ttbar[i] ->Sumw2();

		for(int k = 0; k < gNElPt2bins; ++k){
			hiso_data_pt[i][k]  = new TH1D(Form("ElIsoData_%s_Pt%d",          IsoPlots::sel_name[i].Data(), k), "Electron Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
			hiso_mc_pt  [i][k]  = new TH1D(Form("ElIsoMC_%s_Pt%d",            IsoPlots::sel_name[i].Data(), k), "Electron Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
			hiso_ttbar_pt[i][k] = new TH1D(Form("ElIsoTTbar_%s_Pt%d",         IsoPlots::sel_name[i].Data(), k), "Electron Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_pt_s[i][k]  = new THStack(Form("ElIsoMC_stacked_%s_Pt%d", IsoPlots::sel_name[i].Data(), k), "Electron Isolation in MC for "    + IsoPlots::sel_name[i]);
			hiso_data_pt [i][k]->Sumw2();
			hiso_mc_pt   [i][k]->Sumw2();
			hiso_ttbar_pt[i][k]->Sumw2();
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			hiso_data_nv[i][k]  = new TH1D(Form("ElIsoData_%s_NVtx%d",          IsoPlots::sel_name[i].Data(), k), "Electron Isolation in Data for "  + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
			hiso_mc_nv  [i][k]  = new TH1D(Form("ElIsoMC_%s_NVtx%d",            IsoPlots::sel_name[i].Data(), k), "Electron Isolation in MC for "    + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.0);
			hiso_ttbar_nv[i][k] = new TH1D(Form("ElIsoTTbar_%s_NVtx%d",         IsoPlots::sel_name[i].Data(), k), "Electron Isolation in TTbar for " + IsoPlots::sel_name[i], IsoPlots::nbins[i], 0., 1.);
			hiso_mc_nv_s[i][k]  = new THStack(Form("ElIsoMC_stacked_%s_NVtx%d", IsoPlots::sel_name[i].Data(), k), "Electron Isolation in MC for "    + IsoPlots::sel_name[i]);
			hiso_data_nv [i][k]->Sumw2();
			hiso_mc_nv   [i][k]->Sumw2();
			hiso_ttbar_nv[i][k]->Sumw2();
		}
	}

	////////////////////////////////////////////////////
	// Fill ttbar histos
	// Sample loop
	TTree *tree = fSamples[TTJets]->getTree();

	// Event loop
	tree->ResetBranchAddresses();
	InitMC(tree);

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		printProgress(jentry, nentries, fSamples[TTJets]->name);

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		int elind1(-1), elind2(-1);
		if(hasLooseElectrons(elind1, elind2) < 1) continue;

		// Common event selections
		if(!passesJet50Cut()) continue; // make trigger 100% efficient

		// Common object selections
		if(!isLooseElectron(elind1)) continue;
		// if(ElIsGoodElId_WP80[elind1] != 1) return false; // apply tight ID for the iso plots?

		// Select genmatched fake muons
		if(ElGenMType[elind1] == 2 || ElGenMType[elind1] == 4) continue;
		// Exclude also conversions here?

		if(ElPt[elind1] < fC_minEl2pt) continue;
		if(ElPt[elind1] > gElPt2bins[gNElPt2bins]) continue;


		////////////////////////////////////////////////////
		// MOST LOOSE SELECTION
		hiso_ttbar[0]->Fill(ElRelIso[elind1]);
		for(size_t k = 0; k < gNElPt2bins; ++k){
			if(ElPt[elind1] < gElPt2bins[k]) continue;
			if(ElPt[elind1] > gElPt2bins[k+1]) continue;
			hiso_ttbar_pt[0][k]->Fill(ElRelIso[elind1]);
		}
		for(size_t k = 0; k < gNNVrtxBins; ++k){
			if(NVrtx < gNVrtxBins[k]) continue;
			if(NVrtx > gNVrtxBins[k+1]) continue;
			hiso_ttbar_nv[0][k]->Fill(ElRelIso[elind1]);
		}

		////////////////////////////////////////////////////
		// SIGNAL SUPPRESSED SELECTION
		if(isSigSupElEvent()){
			hiso_ttbar[1]->Fill(ElRelIso[elind1]);
			for(size_t k = 0; k < gNElPt2bins; ++k){
				if(ElPt[elind1] < gElPt2bins[k]) continue;
				if(ElPt[elind1] > gElPt2bins[k+1]) continue;
				hiso_ttbar_pt[1][k]->Fill(ElRelIso[elind1]);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				if(NVrtx < gNVrtxBins[k]) continue;
				if(NVrtx > gNVrtxBins[k+1]) continue;
				hiso_ttbar_nv[1][k]->Fill(ElRelIso[elind1]);
			}
		}
		////////////////////////////////////////////////////
	}
	cout << endl;
	fSamples[TTJets]->cleanUp();
	////////////////////////////////////////////////////

	// Create plots
	vector<int> mcsamples = fMCBG;
	vector<int> datasamples = fEGData;

	for(size_t i = 0; i < gNSels; ++i){
		hiso_data[i]->SetXTitle(convertVarName("ElRelIso[0]"));
		hiso_data[i]->SetLineWidth(3);
		hiso_data[i]->SetLineColor(kBlack);
		hiso_data[i]->SetMarkerStyle(8);
		hiso_data[i]->SetMarkerColor(kBlack);
		hiso_data[i]->SetMarkerSize(1.2);

		hiso_ttbar[i]->SetXTitle(convertVarName("ElRelIso[0]"));
		hiso_ttbar[i]->SetLineWidth(3);
		hiso_ttbar[i]->SetLineColor(kRed);
		hiso_ttbar[i]->SetMarkerStyle(23);
		hiso_ttbar[i]->SetMarkerColor(kRed);
		hiso_ttbar[i]->SetMarkerSize(1.3);

		for(int k = 0; k < gNElPt2bins; ++k){
			hiso_data_pt[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			hiso_data_pt[i][k]->SetLineWidth(3);
			hiso_data_pt[i][k]->SetLineColor(kBlack);
			hiso_data_pt[i][k]->SetMarkerStyle(8);
			hiso_data_pt[i][k]->SetMarkerColor(kBlack);
			hiso_data_pt[i][k]->SetMarkerSize(1.2);

			hiso_ttbar_pt[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			hiso_ttbar_pt[i][k]->SetLineWidth(3);
			hiso_ttbar_pt[i][k]->SetLineColor(kRed);
			hiso_ttbar_pt[i][k]->SetMarkerStyle(23);
			hiso_ttbar_pt[i][k]->SetMarkerColor(kRed);
			hiso_ttbar_pt[i][k]->SetMarkerSize(1.3);
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			hiso_data_nv[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			hiso_data_nv[i][k]->SetLineWidth(3);
			hiso_data_nv[i][k]->SetLineColor(kBlack);
			hiso_data_nv[i][k]->SetMarkerStyle(8);
			hiso_data_nv[i][k]->SetMarkerColor(kBlack);
			hiso_data_nv[i][k]->SetMarkerSize(1.2);

			hiso_ttbar_nv[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			hiso_ttbar_nv[i][k]->SetLineWidth(3);
			hiso_ttbar_nv[i][k]->SetLineColor(kRed);
			hiso_ttbar_nv[i][k]->SetMarkerStyle(23);
			hiso_ttbar_nv[i][k]->SetMarkerColor(kRed);
			hiso_ttbar_nv[i][k]->SetMarkerSize(1.3);
		}

		// Apply weights to MC histos
		for(size_t j = 0; j < gNSAMPLES; ++j){
			Sample *S = fSamples[j];
			float lumiscale = fLumiNorm / S->lumi;
			if(S->datamc == 0) continue;
			S->isoplots[1].hiso[i]->Scale(lumiscale);
			for(size_t k = 0; k < gNElPt2bins; ++k){
				S->isoplots[1].hiso_pt[i][k]->Scale(lumiscale);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				S->isoplots[1].hiso_nv[i][k]->Scale(lumiscale);
			}
		}

		// Fill data histo
		for(size_t j = 0; j < datasamples.size(); ++j){
			Sample *S = fSamples[datasamples[j]];
			hiso_data[i]->Add(S->isoplots[1].hiso[i]);
			hiso_data[i]->SetXTitle(convertVarName("ElRelIso[0]"));
			for(int k = 0; k < gNElPt2bins; ++k){
				hiso_data_pt[i][k]->Add(S->isoplots[1].hiso_pt[i][k]);
				hiso_data_pt[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				hiso_data_nv[i][k]->Add(S->isoplots[1].hiso_nv[i][k]);
				hiso_data_nv[i][k]->SetXTitle(convertVarName("ElRelIso[0]"));
			}
		}

		// Scale to get equal integrals
		float intscale(0.);
		float intscale_pt[gNElPt2bins];
		float intscale_nv[gNNVrtxBins];
		for(size_t j = 0; j < mcsamples.size();   ++j){
			Sample *S = fSamples[mcsamples[j]];
			intscale += S->isoplots[1].hiso[i]->Integral();
			for(int k = 0; k < gNElPt2bins; ++k){
				intscale_pt[k] += S->isoplots[1].hiso_pt[i][k]->Integral();
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				intscale_nv[k] += S->isoplots[1].hiso_nv[i][k]->Integral();
			}
		}
		intscale = hiso_data[i]->Integral() / intscale;
		for(size_t j = 0; j < gNElPt2bins; ++j) intscale_pt[j] = hiso_data_pt[i][j]->Integral() / intscale_pt[j];
		for(size_t j = 0; j < gNNVrtxBins; ++j) intscale_nv[j] = hiso_data_nv[i][j]->Integral() / intscale_nv[j];

		for(size_t j = 0; j < mcsamples.size();   ++j){
			Sample *S = fSamples[mcsamples[j]];			
			S->isoplots[1].hiso[i]->Scale(intscale);
			for(int k = 0; k < gNElPt2bins; ++k){
				S->isoplots[1].hiso_pt[i][k]->Scale(intscale_pt[k]);
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				S->isoplots[1].hiso_nv[i][k]->Scale(intscale_nv[k]);
			}
		}
		hiso_ttbar[i]->Scale(hiso_data[i]->Integral() / hiso_ttbar[i]->Integral());
		for(int k = 0; k < gNElPt2bins; ++k) hiso_ttbar_pt[i][k]->Scale(hiso_data_pt[i][k]->Integral() / hiso_ttbar_pt[i][k]->Integral());
		for(int k = 0; k < gNNVrtxBins; ++k) hiso_ttbar_nv[i][k]->Scale(hiso_data_nv[i][k]->Integral() / hiso_ttbar_nv[i][k]->Integral());

		// Fill MC stacks
		for(size_t j = 0; j < mcsamples.size();   ++j){
			Sample *S = fSamples[mcsamples[j]];			
			hiso_mc  [i]->Add(S->isoplots[1].hiso[i]);
			hiso_mc_s[i]->Add(S->isoplots[1].hiso[i]);
			hiso_mc_s[i]->Draw("goff");
			hiso_mc_s[i]->GetXaxis()->SetTitle(convertVarName("ElRelIso[0]"));
			for(int k = 0; k < gNElPt2bins; ++k){
				hiso_mc_pt  [i][k]->Add(S->isoplots[1].hiso_pt[i][k]);
				hiso_mc_pt_s[i][k]->Add(S->isoplots[1].hiso_pt[i][k]);
				hiso_mc_pt_s[i][k]->Draw("goff");
				hiso_mc_pt_s[i][k]->GetXaxis()->SetTitle(convertVarName("ElRelIso[0]"));
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				hiso_mc_nv  [i][k]->Add(S->isoplots[1].hiso_nv[i][k]);
				hiso_mc_nv_s[i][k]->Add(S->isoplots[1].hiso_nv[i][k]);
				hiso_mc_nv_s[i][k]->Draw("goff");
				hiso_mc_nv_s[i][k]->GetXaxis()->SetTitle(convertVarName("ElRelIso[0]"));
			}
		}


		double max1 = hiso_mc_s[i]->GetMaximum();
		double max2 = hiso_data[i]->GetMaximum();
		double max = max1>max2?max1:max2;

		hiso_mc_s[i]->SetMaximum(1.2*max);
		hiso_data[i]->SetMaximum(1.2*max);

		int bin0   = hiso_data[i]->FindBin(0.0);
		int bin015 = hiso_data[i]->FindBin(0.15) - 1; // bins start at lower edge...
		int bin1   = hiso_data[i]->FindBin(1.0)  - 1;
		float ratio_data  = hiso_data[i] ->Integral(bin0, bin015) / hiso_data[i] ->Integral(bin0, bin1);
		float ratio_mc    = hiso_mc[i]   ->Integral(bin0, bin015) / hiso_mc[i]   ->Integral(bin0, bin1);
		float ratio_ttbar = hiso_ttbar[i]->Integral(bin0, bin015) / hiso_ttbar[i]->Integral(bin0, bin1);

		TCanvas *c_temp = new TCanvas("ElIso" + IsoPlots::sel_name[i], "Electron Isolation in Data vs MC", 0, 0, 800, 600);
		c_temp->cd();

		TLegend *leg = new TLegend(0.70,0.30,0.90,0.68);
		// TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
		leg->AddEntry(hiso_data[i], "Data","p");
		leg->AddEntry(hiso_ttbar[i], "TTbar fake","p");
		for(size_t j = 0; j < mcsamples.size(); ++j) leg->AddEntry(fSamples[mcsamples[j]]->isoplots[1].hiso[i], fSamples[mcsamples[j]]->sname.Data(), "f");
		leg->SetFillStyle(0);
		leg->SetTextFont(42);
		leg->SetBorderSize(0);

		// gPad->SetLogy();
		hiso_mc_s[i]->Draw("hist");
		hiso_ttbar[i]->DrawCopy("PE X0 same");
		hiso_data[i]->DrawCopy("PE X0 same");
		leg->Draw();
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
		lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
		lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
		lat->SetTextColor(kRed);
		lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
		lat->SetTextColor(kBlack);

		// Util::PrintNoEPS(c_temp, "Iso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir, NULL);
		Util::PrintPDF(c_temp, "Iso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir);

		for(int k = 0; k < gNElPt2bins; ++k){
			double max1 = hiso_mc_pt_s[i][k]->GetMaximum();
			double max2 = hiso_data_pt[i][k]->GetMaximum();
			double max = max1>max2?max1:max2;

			hiso_mc_pt_s[i][k]->SetMaximum(1.2*max);
			hiso_data_pt[i][k]->SetMaximum(1.2*max);

			ratio_data = hiso_data_pt[i][k]  ->Integral(bin0, bin015) / hiso_data_pt[i][k] ->Integral(bin0, bin1);
			ratio_mc   = hiso_mc_pt[i][k]    ->Integral(bin0, bin015) / hiso_mc_pt[i][k]   ->Integral(bin0, bin1);
			ratio_ttbar = hiso_ttbar_pt[i][k]->Integral(bin0, bin015) / hiso_ttbar_pt[i][k]->Integral(bin0, bin1);

			TCanvas *c_temp = new TCanvas(Form("ElIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), "Electron Isolation in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			TLegend *leg_pt = new TLegend(0.70,0.30,0.90,0.68);
			// TLegend *leg_pt = new TLegend(0.75,0.60,0.89,0.88);
			leg_pt->AddEntry(hiso_data_pt[i][k], "Data","p");
			leg_pt->AddEntry(hiso_ttbar_pt[i][k], "TTbar fake","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg_pt->AddEntry(fSamples[mcsamples[j]]->isoplots[1].hiso_pt[i][k], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg_pt->SetFillStyle(0);
			leg_pt->SetTextFont(42);
			leg_pt->SetBorderSize(0);

			// gPad->SetLogy();
			hiso_mc_pt_s[i][k]->Draw("hist");
			hiso_ttbar_pt[i][k]->DrawCopy("PE X0 same");
			hiso_data_pt[i][k]->DrawCopy("PE X0 same");
			leg_pt->Draw();
			lat->DrawLatex(0.20,0.92, Form("p_{T}(e) %3.0f - %3.0f GeV", getPt2Bins(Elec)[k], getPt2Bins(Elec)[k+1]));
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			// Util::PrintNoEPS(c_temp, Form("ElIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);
			Util::PrintPDF(c_temp, Form("ElIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir);
		}
		for(int k = 0; k < gNNVrtxBins; ++k){
			double max1 = hiso_mc_nv_s[i][k]->GetMaximum();
			double max2 = hiso_data_nv[i][k]->GetMaximum();
			double max = max1>max2?max1:max2;

			hiso_mc_nv_s[i][k]->SetMaximum(1.2*max);
			hiso_data_nv[i][k]->SetMaximum(1.2*max);

			ratio_data = hiso_data_nv[i][k]  ->Integral(bin0, bin015) / hiso_data_nv[i][k] ->Integral(bin0, bin1);
			ratio_mc   = hiso_mc_nv[i][k]    ->Integral(bin0, bin015) / hiso_mc_nv[i][k]   ->Integral(bin0, bin1);
			ratio_ttbar = hiso_ttbar_nv[i][k]->Integral(bin0, bin015) / hiso_ttbar_nv[i][k]->Integral(bin0, bin1);

			TCanvas *c_temp = new TCanvas(Form("ElIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), "Electron Isolation in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			TLegend *leg_nv = new TLegend(0.70,0.30,0.90,0.68);
			// TLegend *leg_nv = new TLegend(0.75,0.60,0.89,0.88);
			leg_nv->AddEntry(hiso_data_nv[i][k], "Data","p");
			leg_nv->AddEntry(hiso_ttbar_nv[i][k], "TTbar fake","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg_nv->AddEntry(fSamples[mcsamples[j]]->isoplots[1].hiso_nv[i][k], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg_nv->SetFillStyle(0);
			leg_nv->SetTextFont(42);
			leg_nv->SetBorderSize(0);

			// gPad->SetLogy();
			hiso_mc_nv_s[i][k]->Draw("hist");
			hiso_ttbar_nv[i][k]->DrawCopy("PE X0 same");
			hiso_data_nv[i][k]->DrawCopy("PE X0 same");
			leg_nv->Draw();
			lat->DrawLatex(0.20,0.92, Form("N_{Vrtx.} %2.0f - %2.0f", gNVrtxBins[k], gNVrtxBins[k+1]));
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			// Util::PrintNoEPS(c_temp, Form("ElIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);
			Util::PrintPDF(c_temp, Form("ElIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir);
		}
	}
}
void MuonPlotter::makeNT2KinPlots(gHiLoSwitch hilo){
	TString selname[3] = {"LooseLoose", "TightTight", "Signal"};

	for(size_t s = 0; s < 3; ++s){ // loop on selections
		fOutputSubDir = "KinematicPlots/" + gHiLoLabel[hilo] + "/" + selname[s];
		char cmd[100];
		sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
		system(cmd);

		TH1D    *hvar_data[gNKinVars];
		TH1D    *hvar_mc  [gNKinVars];
		THStack *hvar_mc_s[gNKinVars];

		TLatex *lat = new TLatex();
		lat->SetNDC(kTRUE);
		lat->SetTextColor(kBlack);
		lat->SetTextSize(0.04);

		// Create histograms
		for(size_t i = 0; i < gNKinVars; ++i){
			hvar_data[i] = new TH1D("Data_"          + KinPlots::var_name[i], KinPlots::var_name[i] + " in Data", KinPlots::nbins[i], KinPlots::xmin[i], KinPlots::xmax[i]);
			hvar_mc[i]   = new TH1D("MC_"            + KinPlots::var_name[i], KinPlots::var_name[i] + " in MC"  , KinPlots::nbins[i], KinPlots::xmin[i], KinPlots::xmax[i]);
			hvar_mc_s[i] = new THStack("MC_stacked_" + KinPlots::var_name[i], KinPlots::var_name[i] + " in MC");
		}

		// Adjust overflow bins:
		for(size_t i = 0; i < gNKinVars; ++i){
			for(gSample j = sample_begin; j < gNSAMPLES; j=gSample(j+1)){
				Int_t nbins     = fSamples[j]->kinplots[s][hilo].hvar[i]->GetNbinsX();
				Double_t binc   = fSamples[j]->kinplots[s][hilo].hvar[i]->GetBinContent(nbins);
				Double_t overfl = fSamples[j]->kinplots[s][hilo].hvar[i]->GetBinContent(nbins+1);
				fSamples[j]->kinplots[s][hilo].hvar[i]->SetBinContent(nbins, binc + overfl);
			}
		}

		vector<int> mcsamples   = fMCBGMuEnr;
		vector<int> datasamples = fHighPtData;
		//////////////////////////////////////////////////////////
		// Make kin plots
		for(size_t i = 0; i < gNKinVars; ++i){
			// Create plots
			if(i != 6) mcsamples = fMCBG;

			hvar_data[i]->SetXTitle(KinPlots::axis_label[i]);
			hvar_data[i]->SetLineWidth(3);
			hvar_data[i]->SetLineColor(kBlack);
			hvar_data[i]->SetMarkerStyle(8);
			hvar_data[i]->SetMarkerColor(kBlack);
			hvar_data[i]->SetMarkerSize(1.2);

			// Scale by luminosity
			for(size_t j = 0; j < gNSAMPLES; ++j){
				float lumiscale = fLumiNorm / fSamples[j]->lumi;
				if(fSamples[j]->datamc == 0) continue;
				fSamples[j]->kinplots[s][hilo].hvar[i]->Scale(lumiscale);
			}

			// Fill data histo
			for(size_t j = 0; j < datasamples.size(); ++j){
				Sample *S = fSamples[datasamples[j]];
				hvar_data[i]->Add(S->kinplots[s][hilo].hvar[i]);
				hvar_data[i]->SetXTitle(KinPlots::axis_label[i]);
			}

			// Scale to get equal integrals
			float intscale(0.);
			for(size_t j = 0; j < mcsamples.size();   ++j){
				Sample *S = fSamples[mcsamples[j]];
				intscale += S->kinplots[s][hilo].hvar[i]->Integral();
			}
			intscale = hvar_data[i]->Integral() / intscale;

			for(size_t j = 0; j < mcsamples.size();   ++j){
				Sample *S = fSamples[mcsamples[j]];
				S->kinplots[s][hilo].hvar[i]->Scale(intscale);
			}

			// Fill MC stacks
			for(size_t j = 0; j < mcsamples.size();   ++j){
				Sample *S = fSamples[mcsamples[j]];
				hvar_mc  [i]->Add(S->kinplots[s][hilo].hvar[i]);
				hvar_mc_s[i]->Add(S->kinplots[s][hilo].hvar[i]);
				hvar_mc_s[i]->Draw("goff");
				hvar_mc_s[i]->GetXaxis()->SetTitle(KinPlots::axis_label[i]);
			}

			double max1 = hvar_mc_s[i]->GetMaximum();
			double max2 = hvar_data[i]->GetMaximum();
			double max = max1>max2?max1:max2;
			hvar_mc_s[i]->SetMaximum(5.*max);
			hvar_data[i]->SetMaximum(5.*max);
			// hvar_mc_s[i]->SetMaximum(1.5*max);
			// hvar_data[i]->SetMaximum(1.5*max);
			hvar_mc_s[i]->SetMinimum(0.5);
			hvar_data[i]->SetMinimum(0.5);

			TCanvas *c_temp = new TCanvas("C_" + KinPlots::var_name[i], KinPlots::var_name[i] + " in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			// TLegend *leg = new TLegend(0.15,0.50,0.40,0.88);
			// TLegend *leg = new TLegend(0.70,0.30,0.90,0.68);
			TLegend *leg = new TLegend(0.75,0.50,0.89,0.88);
			leg->AddEntry(hvar_data[i], "Data","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg->AddEntry(fSamples[mcsamples[j]]->kinplots[s][hilo].hvar[i], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg->SetFillStyle(0);
			leg->SetTextFont(42);
			leg->SetBorderSize(0);

			gPad->SetLogy();
			hvar_mc_s[i]->Draw("hist");
			hvar_data[i]->DrawCopy("PE X0 same");
			leg->Draw();
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
			lat->DrawLatex(0.11,0.92, selname[s]);

			if(i < 5)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");
			if(i == 5) lat->DrawLatex(0.31,0.92, "ee/#mu#mu");
			if(i == 6) lat->DrawLatex(0.31,0.92, "#mu#mu");
			if(i == 7) lat->DrawLatex(0.31,0.92, "ee");
			if(i == 8) lat->DrawLatex(0.31,0.92, "e#mu");
			if(i > 8)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");

			// Util::PrintNoEPS(c_temp, KinPlots::var_name[i], fOutputDir + fOutputSubDir, NULL);
			Util::PrintPDF(c_temp, KinPlots::var_name[i], fOutputDir + fOutputSubDir);
			delete c_temp;
			delete leg;
		}
	}	
}
void MuonPlotter::makeMETvsHTPlot(vector<int> mmsamples, vector<int> eesamples, vector<int> emsamples, gHiLoSwitch hilo){
	if(readSigGraphs(fOutputFileName) != 0) return;
	TString hiloname[2] = {"p_{T}(l_{1}/l_{2}) > 20/10 GeV", "p_{T}(#mu/e) > 5/10 GeV"};

	fOutputSubDir = "KinematicPlots/HTvsMET/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	const float htmax = 750.;

	// Create histograms
	TH2D *hmetvsht_da_mm = new TH2D("Data_HTvsMET_mm", "Data_HTvsMET_mm", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_ee = new TH2D("Data_HTvsMET_ee", "Data_HTvsMET_ee", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_em = new TH2D("Data_HTvsMET_em", "Data_HTvsMET_em", 100, 0., htmax, 100, 0., 400.);
	// TH2D *hmetvsht_da_mm = new TH2D("Data_HTvsMET_mm", "Data_HTvsMET_mm", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);
	// TH2D *hmetvsht_da_ee = new TH2D("Data_HTvsMET_ee", "Data_HTvsMET_ee", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);
	// TH2D *hmetvsht_da_em = new TH2D("Data_HTvsMET_em", "Data_HTvsMET_em", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);

	// vector<int> mcsamples   = fMCBGMuEnr;
	// const gSample sig = LM1;

	//////////////////////////////////////////////////////////
	// Make MET vs HT plot:
	hmetvsht_da_mm->SetMarkerStyle(8);
	hmetvsht_da_mm->SetMarkerColor(kBlack);
	hmetvsht_da_mm->SetMarkerSize(1.5);
	hmetvsht_da_mm->GetYaxis()->SetTitleOffset(1.4);

	hmetvsht_da_ee->SetMarkerStyle(21);
	hmetvsht_da_ee->SetMarkerColor(kRed);
	hmetvsht_da_ee->SetMarkerSize(1.4);
	hmetvsht_da_ee->GetYaxis()->SetTitleOffset(1.4);

	hmetvsht_da_em->SetMarkerStyle(23);
	hmetvsht_da_em->SetMarkerColor(kBlue);
	hmetvsht_da_em->SetMarkerSize(1.7  );
	hmetvsht_da_em->GetYaxis()->SetTitleOffset(1.4);

	// TGraphs:
	TMultiGraph *gmetvsht_da_mm = new TMultiGraph("HTvsMET_mm", "HTvsMET_mm");
	TMultiGraph *gmetvsht_da_ee = new TMultiGraph("HTvsMET_ee", "HTvsMET_ee");
	TMultiGraph *gmetvsht_da_em = new TMultiGraph("HTvsMET_em", "HTvsMET_em");

	// Fill data histo
	// for(size_t i = 0; i < mmsamples.size(); ++i) hmetvsht_da_mm->Add(fSamples[mmsamples[i]]->kinplots[s][hilo].hmetvsht);
	// for(size_t i = 0; i < eesamples.size(); ++i) hmetvsht_da_ee->Add(fSamples[eesamples[i]]->kinplots[s][hilo].hmetvsht);
	// for(size_t i = 0; i < emsamples.size(); ++i) hmetvsht_da_em->Add(fSamples[emsamples[i]]->kinplots[s][hilo].hmetvsht);

	for(size_t i = 0; i < mmsamples.size(); ++i) gmetvsht_da_mm->Add(fSamples[mmsamples[i]]->sigevents[Muon][hilo]);
	for(size_t i = 0; i < eesamples.size(); ++i) gmetvsht_da_ee->Add(fSamples[eesamples[i]]->sigevents[Elec][hilo]);
	for(size_t i = 0; i < emsamples.size(); ++i) gmetvsht_da_em->Add(fSamples[emsamples[i]]->sigevents[ElMu][hilo]);

	hmetvsht_da_mm->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_mm->SetYTitle(KinPlots::axis_label[1]);
	hmetvsht_da_ee->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_ee->SetYTitle(KinPlots::axis_label[1]);
	hmetvsht_da_em->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_em->SetYTitle(KinPlots::axis_label[1]);

	TLegend *leg = new TLegend(0.80,0.70,0.95,0.88);
	leg->AddEntry(hmetvsht_da_mm, "#mu#mu","p");
	leg->AddEntry(hmetvsht_da_ee, "ee","p");
	leg->AddEntry(hmetvsht_da_em, "e#mu","p");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetTextSize(0.05);
	leg->SetBorderSize(0);

	// Special effects:
	// const float lowerht = hilo==HighPt? 80.:200.;
	const float lowerht = hilo==HighPt? 0.:200.;

	TWbox *lowhtbox  = new TWbox(0., 0., lowerht, 400., kBlack, 0, 0);
	TWbox *lowmetbox = new TWbox(lowerht, 0., htmax,    30., kBlack, 0, 0);
	lowhtbox ->SetFillColor(12);
	lowmetbox->SetFillColor(12);
	lowhtbox ->SetFillStyle(3005);
	lowmetbox->SetFillStyle(3005);
	TLine *boxborder1 = new TLine(lowerht,30.,lowerht,400.);
	TLine *boxborder2 = new TLine(lowerht,30.,htmax,30.);
	boxborder1->SetLineWidth(1);
	boxborder2->SetLineWidth(1);
	boxborder1->SetLineColor(14);
	boxborder2->SetLineColor(14);

	TLine *sig1x = new TLine(lowerht, 100., lowerht, 400.); // met 100 ht 80
	TLine *sig1y = new TLine(lowerht, 100., htmax,   100.); 
	TLine *sig2x = new TLine(200., 120., 200.,  400.);      // met 120 ht 200
	TLine *sig2y = new TLine(200., 120., htmax, 120.);
	TLine *sig3x = new TLine(400.,  50., 400.,  400.);      // met 50  ht 400
	TLine *sig3y = new TLine(400.,  50., htmax,  50.);
	TLine *sig4x = new TLine(400., 120., 400.,  400.);      // met 120 ht 400
	TLine *sig4y = new TLine(400., 120., htmax, 120.);

	sig1x->SetLineWidth(2);
	sig1y->SetLineWidth(2);
	sig2x->SetLineWidth(2);
	sig2y->SetLineWidth(2);
	sig3x->SetLineWidth(1);
	sig3y->SetLineWidth(1);
	sig4x->SetLineWidth(3);
	sig4y->SetLineWidth(3);

	sig1x->SetLineStyle(3);
	sig1y->SetLineStyle(3);
	sig2x->SetLineStyle(2);
	sig2y->SetLineStyle(2);
	sig3x->SetLineStyle(1);
	sig3y->SetLineStyle(1);
	// sig4x->SetLineStyle(1);
	// sig4y->SetLineStyle(1);

	float legymax = hilo==HighPt?0.54:0.50;
	TLegend *regleg = new TLegend(0.70,0.37,0.88,legymax);
	regleg->AddEntry(sig4x, "Search Region 1","l");
	regleg->AddEntry(sig2x, "Search Region 2","l");
	regleg->AddEntry(sig3x, "Search Region 3","l");
	if(hilo != LowPt) regleg->AddEntry(sig1x, "Search Region 4","l");
	regleg->SetFillStyle(0);
	regleg->SetTextFont(42);
	regleg->SetTextSize(0.03);
	regleg->SetBorderSize(0);


	TCanvas *c_temp = new TCanvas("C_HTvsMET", "HT vs MET in Data vs MC", 0, 0, 600, 600);
	c_temp->cd();
	c_temp->SetRightMargin(0.03);
	c_temp->SetLeftMargin(0.13);

	hmetvsht_da_mm->DrawCopy("axis");

	lowhtbox ->Draw();
	lowmetbox->Draw();
	boxborder1->Draw();
	boxborder2->Draw();

	if(hilo != LowPt) sig1x->Draw();
	if(hilo != LowPt) sig1y->Draw();
	sig2x->Draw();
	sig2y->Draw();
	sig3x->Draw();
	sig3y->Draw();
	sig4x->Draw();
	sig4y->Draw();

	// Graphs
	gmetvsht_da_ee->Draw("P");
	gmetvsht_da_em->Draw("P");
	gmetvsht_da_mm->Draw("P");

	leg->Draw();
	// regleg->Draw();
	drawTopLine();
	TPaveText *pave = new TPaveText(0.16, 0.83, 0.55, 0.88, "NDC");
	pave->SetFillColor(0);
	pave->SetFillStyle(1001);
	pave->SetBorderSize(0);
	pave->SetMargin(0.05);
	pave->SetTextFont(42);
	pave->SetTextSize(0.04);
	pave->SetTextAlign(12);
	pave->AddText(hiloname[hilo]);
	pave->Draw();
	gPad->RedrawAxis();

	// Util::PrintNoEPS(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir);
	// Util::SaveAsMacro(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir);
	delete c_temp;
	delete leg, regleg;
	delete hmetvsht_da_mm, hmetvsht_da_ee, hmetvsht_da_em;//, hmetvsht_mc;
	delete gmetvsht_da_mm, gmetvsht_da_ee, gmetvsht_da_em;//, hmetvsht_mc;
}
void MuonPlotter::makeMETvsHTPlotCustom(){
	gHiLoSwitch hilo = LowPt;
	TString hiloname[2] = {"p_{T}(l_{1}/l_{2}) > 20/10 GeV", "p_{T}(#mu/e) > 5/10 GeV"};
	fOutputSubDir = "KinematicPlots/HTvsMET/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	const float htmax = 750.;

	// Create histograms
	TH2D *hmetvsht_da_mm = new TH2D("Data_HTvsMET_mm", "Data_HTvsMET_mm", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_ee = new TH2D("Data_HTvsMET_ee", "Data_HTvsMET_ee", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_em = new TH2D("Data_HTvsMET_em", "Data_HTvsMET_em", 100, 0., htmax, 100, 0., 400.);
	// TH2D *hmetvsht_da_mm = new TH2D("Data_HTvsMET_mm", "Data_HTvsMET_mm", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);
	// TH2D *hmetvsht_da_ee = new TH2D("Data_HTvsMET_ee", "Data_HTvsMET_ee", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);
	// TH2D *hmetvsht_da_em = new TH2D("Data_HTvsMET_em", "Data_HTvsMET_em", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);

	// vector<int> mcsamples   = fMCBGMuEnr;
	// const gSample sig = LM1;

	//////////////////////////////////////////////////////////
	// Make MET vs HT plot:
	hmetvsht_da_mm->SetMarkerStyle(8);
	hmetvsht_da_mm->SetMarkerColor(kBlack);
	hmetvsht_da_mm->SetMarkerSize(1.5);
	hmetvsht_da_mm->GetYaxis()->SetTitleOffset(1.4);

	hmetvsht_da_ee->SetMarkerStyle(21);
	hmetvsht_da_ee->SetMarkerColor(kRed);
	hmetvsht_da_ee->SetMarkerSize(1.4);
	hmetvsht_da_ee->GetYaxis()->SetTitleOffset(1.4);

	hmetvsht_da_em->SetMarkerStyle(23);
	hmetvsht_da_em->SetMarkerColor(kBlue);
	hmetvsht_da_em->SetMarkerSize(1.7  );
	hmetvsht_da_em->GetYaxis()->SetTitleOffset(1.4);

	// Define signa events here:
	const int nmmev = 23;
	const int nemev = 19;
	const int neeev = 7;
	float ht_mm [nmmev] = {319.601, 249.944, 585.687, 218.65 , 280.798, 209.26 , 228.398, 229.865, 264.248, 284.968, 219.79 , 210.096, 372.221, 257.237, 435.9  , 455.478, 224.574, 267.277, 646.496, 253.169, 220.158, 354.292, 592.905};
	float met_mm[nmmev] = {53.6233, 205.792, 35.0852, 39.8026, 73.8093, 58.0265, 48.3581, 30.1828, 120.754, 31.0167, 63.0605, 46.5073, 51.0459, 34.209 , 62.904 , 62.8467, 76.8364, 107.896, 143.069, 122.463, 78.2578, 42.3864, 58.09};
	float ht_em [nemev] = {318.618, 253.246, 320.556, 259.813, 324.42 , 377.818, 421.674, 290.427, 687.421, 296.215, 280.877, 247.511, 279.396, 347.603, 323.792, 428.888, 247.792, 261.28 , 255.075};
	float met_em[nemev] = {61.8755, 70.9379, 38.5762, 109.035, 56.9286, 121.968, 33.6699, 53.6195, 92.6551, 150.137, 100.28 , 64.1492, 78.0987, 77.7492, 67.6595, 55.3003, 53.2864, 72.1098, 30.7443};
	float ht_ee [neeev] = {294.182, 257.603, 561.344, 246.617, 264.046, 563.705, 253.773};
	float met_ee[neeev] = {56.5751, 64.2966, 30.083 , 108.323, 59.5336, 83.8312, 46.3871};

	// TGraphs:
	TGraph *gmetvsht_da_mm = new TGraph(nmmev, ht_mm, met_mm);
	TGraph *gmetvsht_da_em = new TGraph(nemev, ht_em, met_em);
	TGraph *gmetvsht_da_ee = new TGraph(neeev, ht_ee, met_ee);
	gmetvsht_da_mm->SetName("HTvsMET_mm");
	gmetvsht_da_em->SetName("HTvsMET_em");
	gmetvsht_da_ee->SetName("HTvsMET_ee");

	hmetvsht_da_mm->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_mm->SetYTitle(KinPlots::axis_label[1]);
	hmetvsht_da_ee->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_ee->SetYTitle(KinPlots::axis_label[1]);
	hmetvsht_da_em->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_em->SetYTitle(KinPlots::axis_label[1]);

	gmetvsht_da_mm->SetMarkerColor(kBlack);
	gmetvsht_da_mm->SetMarkerStyle(8);
	gmetvsht_da_mm->SetMarkerSize(1.5);
	gmetvsht_da_em->SetMarkerColor(kBlue);
	gmetvsht_da_em->SetMarkerStyle(23);
	gmetvsht_da_em->SetMarkerSize(1.5);
	gmetvsht_da_ee->SetMarkerColor(kRed);
	gmetvsht_da_ee->SetMarkerStyle(21);
	gmetvsht_da_ee->SetMarkerSize(1.5);

	TLegend *leg = new TLegend(0.80,0.70,0.95,0.88);
	leg->AddEntry(hmetvsht_da_mm, "#mu#mu","p");
	leg->AddEntry(hmetvsht_da_ee, "ee","p");
	leg->AddEntry(hmetvsht_da_em, "e#mu","p");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetTextSize(0.05);
	leg->SetBorderSize(0);

	// Special effects:
	const float lowerht = hilo==HighPt? 80.:200.;

	TWbox *lowhtbox  = new TWbox(0., 0., lowerht, 400., kBlack, 0, 0);
	TWbox *lowmetbox = new TWbox(lowerht, 0., htmax,    30., kBlack, 0, 0);
	lowhtbox ->SetFillColor(12);
	lowmetbox->SetFillColor(12);
	lowhtbox ->SetFillStyle(3005);
	lowmetbox->SetFillStyle(3005);
	TLine *boxborder1 = new TLine(lowerht,30.,lowerht,400.);
	TLine *boxborder2 = new TLine(lowerht,30.,htmax,30.);
	boxborder1->SetLineWidth(1);
	boxborder2->SetLineWidth(1);
	boxborder1->SetLineColor(14);
	boxborder2->SetLineColor(14);

	TLine *sig1x = new TLine(lowerht, 100., lowerht, 400.); // met 100 ht 80
	TLine *sig1y = new TLine(lowerht, 100., htmax,   100.); 
	TLine *sig2x = new TLine(200., 120., 200.,  400.);      // met 120 ht 200
	TLine *sig2y = new TLine(200., 120., htmax, 120.);
	TLine *sig3x = new TLine(400.,  50., 400.,  400.);      // met 50  ht 400
	TLine *sig3y = new TLine(400.,  50., htmax,  50.);
	TLine *sig4x = new TLine(400., 120., 400.,  400.);      // met 120 ht 400
	TLine *sig4y = new TLine(400., 120., htmax, 120.);

	sig1x->SetLineWidth(2);
	sig1y->SetLineWidth(2);
	sig2x->SetLineWidth(2);
	sig2y->SetLineWidth(2);
	sig3x->SetLineWidth(1);
	sig3y->SetLineWidth(1);
	sig4x->SetLineWidth(3);
	sig4y->SetLineWidth(3);

	sig1x->SetLineStyle(3);
	sig1y->SetLineStyle(3);
	sig2x->SetLineStyle(2);
	sig2y->SetLineStyle(2);
	sig3x->SetLineStyle(1);
	sig3y->SetLineStyle(1);
	// sig4x->SetLineStyle(1);
	// sig4y->SetLineStyle(1);

	TLegend *regleg = new TLegend(0.70,0.47,0.88,0.6);
	regleg->AddEntry(sig4x, "Search Region 1","l");
	regleg->AddEntry(sig2x, "Search Region 2","l");
	regleg->AddEntry(sig3x, "Search Region 3","l");
	if(hilo != LowPt) regleg->AddEntry(sig1x, "Search Region 4","l");
	regleg->SetFillStyle(0);
	regleg->SetTextFont(42);
	regleg->SetTextSize(0.03);
	regleg->SetBorderSize(0);


	TCanvas *c_temp = new TCanvas("C_HTvsMET", "HT vs MET in Data vs MC", 0, 0, 600, 600);
	c_temp->cd();
	c_temp->SetRightMargin(0.03);
	c_temp->SetLeftMargin(0.13);

	hmetvsht_da_mm->DrawCopy("axis");

	lowhtbox ->Draw();
	lowmetbox->Draw();
	boxborder1->Draw();
	boxborder2->Draw();

	if(hilo != LowPt) sig1x->Draw();
	if(hilo != LowPt) sig1y->Draw();
	sig2x->Draw();
	sig2y->Draw();
	sig3x->Draw();
	sig3y->Draw();
	sig4x->Draw();
	sig4y->Draw();

	// Graphs
	gmetvsht_da_ee->Draw("P");
	gmetvsht_da_em->Draw("P");
	gmetvsht_da_mm->Draw("P");

	leg->Draw();
	regleg->Draw();

	drawTopLine();
	TPaveText *pave = new TPaveText(0.16, 0.83, 0.55, 0.88, "NDC");
	pave->SetFillColor(0);
	pave->SetFillStyle(1001);
	pave->SetBorderSize(0);
	pave->SetMargin(0.05);
	pave->SetTextFont(42);
	pave->SetTextSize(0.04);
	pave->SetTextAlign(12);
	pave->AddText(hiloname[hilo]);
	pave->Draw();
	gPad->RedrawAxis();

	// Util::PrintNoEPS(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir);
	Util::SaveAsMacro(c_temp, "HTvsMET_" + gHiLoLabel[hilo], fOutputDir + fOutputSubDir);
	delete c_temp;
	delete leg, regleg;
	delete hmetvsht_da_mm, hmetvsht_da_ee, hmetvsht_da_em;//, hmetvsht_mc;
	delete gmetvsht_da_mm, gmetvsht_da_ee, gmetvsht_da_em;//, hmetvsht_mc;
}
void MuonPlotter::makeMETvsHTPlotTau(){
	fOutputSubDir = "KinematicPlots/HTvsMET/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	const float htmax = 750.;

	// Create histograms
	TH2D *hmetvsht_da_mt = new TH2D("Data_HTvsMET_mt", "Data_HTvsMET_mt", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_et = new TH2D("Data_HTvsMET_et", "Data_HTvsMET_et", 100, 0., htmax, 100, 0., 400.);
	TH2D *hmetvsht_da_tt = new TH2D("Data_HTvsMET_tt", "Data_HTvsMET_tt", 100, 0., htmax, 100, 0., 400.);

	//////////////////////////////////////////////////////////
	// Make MET vs HT plot:
	hmetvsht_da_mt->SetMarkerStyle(22);
	hmetvsht_da_mt->SetMarkerSize(1.8);
	hmetvsht_da_mt->SetMarkerColor(51);
	hmetvsht_da_mt->GetYaxis()->SetTitleOffset(1.4);

	hmetvsht_da_et->SetMarkerStyle(33);
	hmetvsht_da_et->SetMarkerSize(2.3);
	hmetvsht_da_et->SetMarkerColor(46);

	hmetvsht_da_tt->SetMarkerStyle(34);
	hmetvsht_da_tt->SetMarkerSize(1.8);
	hmetvsht_da_tt->SetMarkerColor(38);


	// these are the 5 SS tau-mu events with HT>350 and MET>80
	// const int nmtev = 5;
	// float a_mt_ht [nmtev] = {372.561, 368.64,  467.394, 605.398, 424.883};
	// float a_mt_met[nmtev] = {99.2257, 109.763, 144.284, 143.927, 91.4893};

	// updated numbers with 967/pb from Jul 6
	const int nmtev = 7;
	float a_mt_ht [nmtev] = {454.149, 380.437, 363.305, 549.702, 361.937, 355.314, 363.292};
	float a_mt_met[nmtev] = {144.284, 91.6271, 217.503, 143.927, 81.9502, 109.763, 89.3453};
	const int netev = 4;
	float a_et_ht [netev] = {421.656, 421.634, 537.589, 444.368};
	float a_et_met[netev] = {103.668, 93.5886, 83.3471, 194.835};

	TGraph *gmetvsht_da_mt = new TGraph(nmtev, a_mt_ht, a_mt_met);
	gmetvsht_da_mt->SetName("Data_HTvsMET_mt_graph");
	gmetvsht_da_mt->SetMarkerStyle(hmetvsht_da_mt->GetMarkerStyle());
	gmetvsht_da_mt->SetMarkerSize( hmetvsht_da_mt->GetMarkerSize());
	gmetvsht_da_mt->SetMarkerColor(hmetvsht_da_mt->GetMarkerColor());

	TGraph *gmetvsht_da_et = new TGraph(netev, a_et_ht, a_et_met);
	gmetvsht_da_et->SetName("Data_HTvsMET_et_graph");
	gmetvsht_da_et->SetMarkerStyle(hmetvsht_da_et->GetMarkerStyle());
	gmetvsht_da_et->SetMarkerSize( hmetvsht_da_et->GetMarkerSize());
	gmetvsht_da_et->SetMarkerColor(hmetvsht_da_et->GetMarkerColor());

	hmetvsht_da_mt->SetXTitle(KinPlots::axis_label[0]);
	hmetvsht_da_mt->SetYTitle(KinPlots::axis_label[1]);

	// TLegend *leg = new TLegend(0.80,0.82,0.95,0.88);
	TLegend *leg = new TLegend(0.80,0.70,0.95,0.88);
	leg->AddEntry(hmetvsht_da_mt, "#mu#tau","p");
	leg->AddEntry(hmetvsht_da_et, "e#tau","p");
	leg->AddEntry(hmetvsht_da_tt, "#tau#tau","p");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetTextSize(0.05);
	leg->SetBorderSize(0);

	// Special effects:
	const float lowerht = 350.;
	// HT350met80 and HT400met120).
	TWbox *lowhtbox  = new TWbox(0., 0., lowerht, 400., kBlack, 0, 0);
	TWbox *lowmetbox = new TWbox(lowerht, 0., htmax,    80., kBlack, 0, 0);
	lowhtbox ->SetFillColor(12);
	lowmetbox->SetFillColor(12);
	lowhtbox ->SetFillStyle(3005);
	lowmetbox->SetFillStyle(3005);
	TLine *boxborder1 = new TLine(lowerht, 80., lowerht, 400.);
	TLine *boxborder2 = new TLine(lowerht, 80., htmax,   80.);
	boxborder1->SetLineWidth(1);
	boxborder2->SetLineWidth(1);
	boxborder1->SetLineColor(14);
	boxborder2->SetLineColor(14);

	// TLine *sig1x = new TLine(lowerht, 100., lowerht, 400.);
	// TLine *sig1y = new TLine(lowerht, 100., htmax,   100.);
	// TLine *sig2x = new TLine(lowerht, 120., lowerht, 400.);
	// TLine *sig2y = new TLine(lowerht, 120., htmax,   120.);
	// TLine *sig3x = new TLine(400.,  50., 400.,  400.);
	// TLine *sig3y = new TLine(400.,  50., htmax,  50.);
	TLine *sig4x = new TLine(400., 120., 400.,  400.);
	TLine *sig4y = new TLine(400., 120., htmax, 120.);

	// sig1x->SetLineWidth(2);
	// sig1y->SetLineWidth(2);
	// sig2x->SetLineWidth(2);
	// sig2y->SetLineWidth(2);
	// sig3x->SetLineWidth(2);
	// sig3y->SetLineWidth(2);
	sig4x->SetLineWidth(3);
	sig4y->SetLineWidth(3);

	// sig1x->SetLineStyle(2);
	// sig1y->SetLineStyle(2);
	// sig2x->SetLineStyle(2);
	// sig2y->SetLineStyle(2);
	// sig3x->SetLineStyle(2);
	// sig3y->SetLineStyle(2);
	// sig4x->SetLineStyle(2);
	// sig4y->SetLineStyle(2);

	TLegend *regleg = new TLegend(0.70,0.60,0.88,0.65);
	regleg->AddEntry(sig4x, "Search Region 1","l");
	regleg->SetFillStyle(0);
	regleg->SetTextFont(42);
	regleg->SetTextSize(0.03);
	regleg->SetBorderSize(0);

	TCanvas *c_temp = new TCanvas("C_HTvsMET", "HT vs MET in Data vs MC", 0, 0, 600, 600);
	c_temp->cd();
	c_temp->SetRightMargin(0.03);
	c_temp->SetLeftMargin(0.13);

	hmetvsht_da_mt->Draw("axis");

	lowhtbox ->Draw();
	lowmetbox->Draw();
	boxborder1->Draw();
	boxborder2->Draw();

	// if(hilo != LowPt) sig1x->Draw();
	// if(hilo != LowPt) sig1y->Draw();
	// sig2x->Draw();
	// sig2y->Draw();
	// sig3x->Draw();
	// sig3y->Draw();
	sig4x->Draw();
	sig4y->Draw();

	// Graphs
	gmetvsht_da_mt->Draw("P");
	gmetvsht_da_et->Draw("P");

	leg->Draw();
	regleg->Draw();
	drawTopLine();
	TPaveText *pave = new TPaveText(0.16, 0.83, 0.55, 0.88, "NDC");
	pave->SetFillColor(0);
	pave->SetFillStyle(1001);
	pave->SetBorderSize(0);
	pave->SetMargin(0.05);
	pave->SetTextFont(42);
	pave->SetTextSize(0.04);
	pave->SetTextAlign(12);
	pave->AddText("p_{T}(#mu/e/#tau) > 5/10/15 GeV");
	pave->Draw();
	gPad->RedrawAxis();

	Util::PrintNoEPS( c_temp, "HTvsMET_TauChan", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "HTvsMET_TauChan", fOutputDir + fOutputSubDir);
	Util::SaveAsMacro(c_temp, "HTvsMET_TauChan", fOutputDir + fOutputSubDir);
	delete c_temp;
	delete leg, regleg;
	delete hmetvsht_da_mt;
	delete gmetvsht_da_mt;
	fOutputSubDir = "";
}
void MuonPlotter::makeFRvsPtPlots(gChannel chan, gFPSwitch fp){
	fOutputSubDir = "Ratios/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", getNEtaBins(chan), getEtaBins(chan));
	TH2D *h_dummy2 = new TH2D("dummy2", "dummy2", getNPt2Bins(chan), getPt2Bins(chan), getNEtaBins(chan), getEtaBins(chan));

	TH1D *h_ptratio_data = new TH1D("Ratio_data", "Tight/Loose Ratio in data", getNPt2Bins(chan), getPt2Bins(chan));
	TH1D *h_ptratio_mc   = new TH1D("Ratio_mc",   "Tight/Loose Ratio in MC",   getNPt2Bins(chan), getPt2Bins(chan));

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Elec){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	calculateRatio(datasamples, chan, fp, h_dummy2, h_ptratio_data, h_dummy1);
	calculateRatio(mcsamples,   chan, fp, h_dummy2, h_ptratio_mc,   h_dummy1);

	float maximum = 0.8;
	if(fp == ZDecay) maximum = 1.1;
	h_ptratio_data->SetMaximum(maximum);
	h_ptratio_mc  ->SetMaximum(maximum);
	h_ptratio_data->SetMinimum(0.0);
	h_ptratio_mc  ->SetMinimum(0.0);

	if(chan == Muon)     h_ptratio_mc->SetXTitle(convertVarName("MuPt[0]"));
	if(chan == Elec) h_ptratio_mc->SetXTitle(convertVarName("ElPt[0]"));
	h_ptratio_mc->GetYaxis()->SetTitleOffset(1.2);
	h_ptratio_mc->SetYTitle("N_{Tight}/N_{Loose}");

	h_ptratio_data->SetMarkerColor(kBlack);
	h_ptratio_data->SetMarkerStyle(20);
	h_ptratio_data->SetMarkerSize(1.3);
	h_ptratio_data->SetLineWidth(2);
	h_ptratio_data->SetLineColor(kBlack);
	h_ptratio_data->SetFillColor(kBlack);

	h_ptratio_mc  ->SetMarkerColor(kRed);
	h_ptratio_mc  ->SetMarkerStyle(20);
	h_ptratio_mc  ->SetMarkerSize(1.3);
	h_ptratio_mc  ->SetLineWidth(2);
	h_ptratio_mc  ->SetLineColor(kRed);
	h_ptratio_mc  ->SetFillColor(kRed);

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	TLegend *leg;
	if(fp == SigSup) leg = new TLegend(0.70,0.75,0.89,0.88);
	if(fp == ZDecay) leg = new TLegend(0.70,0.15,0.89,0.28);
	leg->AddEntry(h_ptratio_data, "Data","f");
	leg->AddEntry(h_ptratio_mc,   "MC",  "f");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	TCanvas *c_temp = new TCanvas("C_PtRatioPlot", "fRatio vs Pt in Data vs MC", 0, 0, 800, 600);
	c_temp->cd();

	h_ptratio_mc->DrawCopy("PE X0");
	h_ptratio_data->Draw("PE X0 same");
	leg->Draw();
	lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
	lat->DrawLatex(0.11,0.92, name);
	double ymean(0.), yrms(0.);
	getWeightedYMeanRMS(h_ptratio_data, ymean, yrms);
	lat->SetTextSize(0.03);
	lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));

	TString fpname = "F";
	if(fp == ZDecay) fpname = "P";

	// Util::PrintNoEPS( c_temp, fpname + "Ratio_" + name + "_Pt", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, fpname + "Ratio_" + name + "_Pt", fOutputDir + fOutputSubDir);
	delete h_ptratio_mc, h_ptratio_data;
	delete c_temp, lat, leg;
	fOutputSubDir = "";
}
void MuonPlotter::makeFRvsPtPlotsForPAS(gChannel chan){
	Util::SetTDRStyle();
	fOutputSubDir = "Ratios/forPAS/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", getNEtaBins(chan), getEtaBins(chan));
	TH2D *h_dummy2 = new TH2D("dummy2", "dummy2", getNPt2Bins(chan), getPt2Bins(chan), getNEtaBins(chan), getEtaBins(chan));

	TH1D *h_ratio_A2 = new TH1D("Ratio_data2", "Tight/Loose Ratio in data for A2", getNPt2Bins(chan), getPt2Bins(chan));
	TH1D *h_ratio_A1 = new TH1D("Ratio_data1", "Tight/Loose Ratio in data for A1", getNPt2Bins(chan), getPt2Bins(chan));

	vector<int> datasamples;

	if(chan == Muon)     datasamples = fMuData;
	if(chan == Elec) datasamples = fEGData;

	float A1MBins [5] = {0.31, 0.25, 0.21, 0.18, 0.20};
	float A1MBinsE[5] = {0.002, 0.002, 0.002, 0.003, 0.001};

	float A1EBins [5] = {0.22, 0.21, 0.14, 0.22, 0.35};
	float A1EBinsE[5] = {0.02, 0.02, 0.02, 0.02, 0.04};

	for(size_t i = 0; i < 5; ++i){
		if(chan == Muon){
			h_ratio_A1->SetBinContent(i+1, A1MBins[i]);
			h_ratio_A1->SetBinError(i+1,   A1MBinsE[i]);
		}
		if(chan == Elec){
			h_ratio_A1->SetBinContent(i+1, A1EBins[i]);
			h_ratio_A1->SetBinError(i+1,   A1EBinsE[i]);
		}
	}

	calculateRatio(datasamples, chan, SigSup, h_dummy2, h_ratio_A2, h_dummy1);

	h_ratio_A2->GetXaxis()->SetTitle("p_{T} (GeV)");
	h_ratio_A2->GetYaxis()->SetTitle("TL Ratio");
	h_ratio_A2->SetMarkerStyle(20);
	h_ratio_A2->SetMarkerSize(1.6);
	h_ratio_A2->SetMarkerColor(kBlue);
	h_ratio_A2->SetLineColor(kBlue);

	h_ratio_A1->GetXaxis()->SetTitle("p_{T} (GeV)");
	h_ratio_A1->GetYaxis()->SetTitle("TL Ratio");
	h_ratio_A1->SetMarkerStyle(23);
	h_ratio_A1->SetMarkerSize(1.8);
	h_ratio_A1->SetMarkerColor(kBlack);
	h_ratio_A1->SetLineColor(kBlack);

	h_ratio_A2->GetYaxis()->SetRangeUser(0., 0.7);
	h_ratio_A1->GetYaxis()->SetRangeUser(0., 0.7);

	TLegend *leg = new TLegend(0.21,0.58,0.47,0.78);
	leg->AddEntry(h_ratio_A1, "Method A1","p");
	leg->AddEntry(h_ratio_A2, "Method A2","p");
	leg->SetTextSize(0.05);
	// leg->SetTextFont(42);
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);	

	TCanvas *c_temp = new TCanvas();
	c_temp->cd();
	c_temp->SetRightMargin(0.05);

	h_ratio_A2->Draw("PE");
	h_ratio_A1->Draw("PE same");
	leg->Draw();
	TLatex lat;
	lat.SetNDC(kTRUE);
	lat.SetTextSize(0.05);
	lat.DrawLatex(0.23, 0.88, "CMS Preliminary");
	lat.DrawLatex(0.70, 0.88, name);
	lat.DrawLatex(0.23, 0.81, "L_{int.} = 0.98 fb^{-1},   #sqrt{s} = 7 TeV");

	Util::PrintNoEPS( c_temp, "FRatio_" + name + "_Pt_A1vsA2", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "FRatio_" + name + "_Pt_A1vsA2", fOutputDir + fOutputSubDir);
	Util::SaveAsMacro(c_temp, "FRatio_" + name + "_Pt_A1vsA2", fOutputDir + fOutputSubDir);
	delete h_ratio_A1, h_ratio_A2;
	delete c_temp;
	fOutputSubDir = "";
}
void MuonPlotter::makeFRvsEtaPlots(gChannel chan){
	fOutputSubDir = "Ratios/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", getNPt2Bins(chan), getPt2Bins(chan));
	TH2D *h_dummy2 = new TH2D("dummy2", "dummy2", getNPt2Bins(chan), getPt2Bins(chan), getNEtaBins(chan), getEtaBins(chan));

	TH1D *h_etaratio_data = new TH1D("Ratio_data", "Tight/Loose Ratio in data", getNEtaBins(chan), getEtaBins(chan));
	TH1D *h_etaratio_mc   = new TH1D("Ratio_mc",   "Tight/Loose Ratio in MC",   getNEtaBins(chan), getEtaBins(chan));
	// h_etaratio_data->Sumw2();
	// h_etaratio_mc  ->Sumw2();

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Elec){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	calculateRatio(datasamples, chan, SigSup, h_dummy2, h_dummy1, h_etaratio_data);
	calculateRatio(mcsamples,   chan, SigSup, h_dummy2, h_dummy1, h_etaratio_mc);

	h_etaratio_data->SetMaximum(0.4);
	h_etaratio_mc  ->SetMaximum(0.4);
	h_etaratio_data->SetMinimum(0.0);
	h_etaratio_mc  ->SetMinimum(0.0);

	if(chan == Muon)     h_etaratio_mc->SetXTitle(convertVarName("MuEta[0]"));
	if(chan == Elec) h_etaratio_mc->SetXTitle(convertVarName("ElEta[0]"));
	h_etaratio_mc->GetYaxis()->SetTitleOffset(1.2);
	h_etaratio_mc->SetYTitle("N_{Tight}/N_{Loose}");

	h_etaratio_data->SetMarkerColor(kBlack);
	h_etaratio_data->SetMarkerStyle(20);
	h_etaratio_data->SetMarkerSize(1.3);
	h_etaratio_data->SetLineWidth(2);
	h_etaratio_data->SetLineColor(kBlack);
	h_etaratio_data->SetFillColor(kBlack);

	h_etaratio_mc  ->SetMarkerColor(kRed);
	h_etaratio_mc  ->SetMarkerStyle(20);
	h_etaratio_mc  ->SetMarkerSize(1.3);
	h_etaratio_mc  ->SetLineWidth(2);
	h_etaratio_mc  ->SetLineColor(kRed);
	h_etaratio_mc  ->SetFillColor(kRed);

	// // h_etaratio_data->GetXaxis()->SetTitle("p_{T} (GeV)");
	// h_etaratio_data->GetXaxis()->SetTitle("#left|#eta#right|");
	// h_etaratio_data->GetYaxis()->SetTitle("TL Ratio");
	// h_etaratio_data->SetMarkerStyle(22);
	// h_etaratio_data->SetMarkerSize(1.4);
	// h_etaratio_data->SetMarkerColor(kBlack);
	// h_etaratio_data->SetLineColor(kBlack);
	// h_etaratio_data->GetYaxis()->SetRangeUser(0., 0.5);

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	TLegend *leg = new TLegend(0.70,0.75,0.89,0.88);
	leg->AddEntry(h_etaratio_data, "Data","f");
	leg->AddEntry(h_etaratio_mc,   "MC",  "f");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	TCanvas *c_temp = new TCanvas("C_EtaRatioPlot", "fRatio vs Eta in Data vs MC", 0, 0, 800, 600);
	// TCanvas *c_temp = new TCanvas();
	c_temp->cd();

	// gPad->SetLogy();
	// h_etaratio_data->Draw("PE");
	// TLatex lat;
	// lat.SetNDC(kTRUE);
	//     lat.SetTextSize(0.028);
	//     lat.DrawLatex(0.23, 0.88, "CMS Preliminary");
	//     lat.DrawLatex(0.23, 0.79, "#int L dt = XXX pb^{-1},   #sqrt{s} = 7 TeV");
	//     lat.DrawLatex(0.83, 0.88, name);

	h_etaratio_mc->DrawCopy("PE X0");
	h_etaratio_data->Draw("PE X0 same");
	leg->Draw();
	lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
	lat->DrawLatex(0.11,0.92, name);
	double ymean(0.), yrms(0.);
	getWeightedYMeanRMS(h_etaratio_data, ymean, yrms);
	lat->SetTextSize(0.03);
	lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));

	// Util::PrintNoEPS( c_temp, "FRatio_" + name + "_Eta", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "FRatio_" + name + "_Eta", fOutputDir + fOutputSubDir);
	delete h_etaratio_mc, h_etaratio_data;
	// delete c_temp;
	delete c_temp, lat, leg;
	fOutputSubDir = "";
}
void MuonPlotter::makeFRvsEtaPlotsForPAS(gChannel chan){
	Util::SetTDRStyle();
	fOutputSubDir = "Ratios/forPAS";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", getNPt2Bins(chan), getPt2Bins(chan));
	TH2D *h_dummy2 = new TH2D("dummy2", "dummy2", getNPt2Bins(chan), getPt2Bins(chan), getNEtaBins(chan), getEtaBins(chan));

	TH1D *h_ratio_A2 = new TH1D("Ratio_data2", "Tight/Loose Ratio in data for A2", getNEtaBins(chan), getEtaBins(chan));
	TH1D *h_ratio_A1 = new TH1D("Ratio_data1", "Tight/Loose Ratio in data for A1", getNEtaBins(chan), getEtaBins(chan));

	vector<int> datasamples;

	if(chan == Muon)     datasamples = fMuData;
	if(chan == Elec) datasamples = fEGData;

	calculateRatio(datasamples, chan, SigSup, h_dummy2, h_dummy1, h_ratio_A2);

	float A1MBins [4] = {0.2, 0.23, 0.25, 0.26};
	float A1MBinsE[4] = {0.001, 0.002, 0.002, 0.002};

	float A1EBins [4] = {0.2, 0.22, 0.23, 0.28};
	float A1EBinsE[4] = {0.01, 0.02, 0.02, 0.03};

	for(size_t i = 0; i < 4; ++i){
		if(chan == Muon){
			h_ratio_A1->SetBinContent(i+1, A1MBins[i]);
			h_ratio_A1->SetBinError(i+1,   A1MBinsE[i]);
		}
		if(chan == Elec){
			h_ratio_A1->SetBinContent(i+1, A1EBins[i]);
			h_ratio_A1->SetBinError(i+1,   A1EBinsE[i]);
		}
	}

	h_ratio_A2->SetMaximum(0.4);
	h_ratio_A2->SetMinimum(0.0);
	h_ratio_A1->SetMaximum(0.4);
	h_ratio_A1->SetMinimum(0.0);

	// h_ratio_data->GetXaxis()->SetTitle("p_{T} (GeV)");
	h_ratio_A2->GetXaxis()->SetTitle("#left|#eta#right|");
	h_ratio_A2->GetYaxis()->SetTitle("TL Ratio");
	h_ratio_A2->SetMarkerStyle(20);
	h_ratio_A2->SetMarkerSize(1.6);
	h_ratio_A2->SetMarkerColor(kBlue);
	h_ratio_A2->SetLineColor(kBlue);

	h_ratio_A1->GetXaxis()->SetTitle("#left|#eta#right|");
	h_ratio_A1->GetYaxis()->SetTitle("TL Ratio");
	h_ratio_A1->SetMarkerStyle(23);
	h_ratio_A1->SetMarkerSize(1.8);
	h_ratio_A1->SetMarkerColor(kBlack);
	h_ratio_A1->SetLineColor(kBlack);

	h_ratio_A2->GetYaxis()->SetRangeUser(0., 0.5);
	h_ratio_A1->GetYaxis()->SetRangeUser(0., 0.5);

	TLegend *leg = new TLegend(0.21,0.58,0.47,0.78);
	leg->AddEntry(h_ratio_A1, "Method A1","p");
	leg->AddEntry(h_ratio_A2, "Method A2","p");
	leg->SetTextSize(0.05);
	// leg->SetTextFont(42);
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);	

	TCanvas *c_temp = new TCanvas();
	c_temp->cd();
	c_temp->SetRightMargin(0.05);

	h_ratio_A2->Draw("PE");
	h_ratio_A1->Draw("PE same");
	leg->Draw();
	TLatex lat;
	lat.SetNDC(kTRUE);
	lat.SetTextSize(0.05);
	lat.DrawLatex(0.23, 0.88, "CMS Preliminary");
	lat.DrawLatex(0.70, 0.88, name);
	lat.DrawLatex(0.23, 0.81, "L_{int.} = 0.98 fb^{-1},   #sqrt{s} = 7 TeV");

	Util::PrintNoEPS( c_temp, "FRatio_" + name + "_Eta_A1vsA2", fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "FRatio_" + name + "_Eta_A1vsA2", fOutputDir + fOutputSubDir);
	Util::SaveAsMacro(c_temp, "FRatio_" + name + "_Eta_A1vsA2", fOutputDir + fOutputSubDir);
	delete h_ratio_A1, h_ratio_A2;
	delete c_temp;
	fOutputSubDir = "";
}
void MuonPlotter::makeRatioPlots(gChannel chan){
	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	fOutputSubDir = "Ratios/" + name + "/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Elec){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	// Customization
	TString axis_name[gNRatioVars] = {"N_{Jets}",  "H_{T} (GeV)", "P_{T}(Hardest Jet) (GeV)", "N_{Vertices}", "p_{T}(Closest Jet) (GeV)", "p_{T}(Away Jet) (GeV)", "N_{BJets}", "E_{T}^{miss} (GeV)", "m_{T} (GeV)"};

	for(size_t i = 0; i < gNRatioVars; ++i){
		TH1D *h_ratio_data = getFRatio(datasamples, chan, i);
		TH1D *h_ratio_mc   = getFRatio(mcsamples,   chan, i);
		h_ratio_data->SetName(Form("FRatio_%s_data", FRatioPlots::var_name[i].Data()));
		h_ratio_mc  ->SetName(Form("FRatio_%s_mc",   FRatioPlots::var_name[i].Data()));

		float max = 0.4;
		if(i==8) max = 1.0;
		h_ratio_data->SetMaximum(max);
		h_ratio_mc  ->SetMaximum(max);
		h_ratio_data->SetMinimum(0.0);
		h_ratio_mc  ->SetMinimum(0.0);

		h_ratio_data->SetXTitle(axis_name[i]);
		h_ratio_mc  ->SetXTitle(axis_name[i]);
		h_ratio_data->GetYaxis()->SetTitleOffset(1.2);
		h_ratio_mc  ->GetYaxis()->SetTitleOffset(1.2);
		h_ratio_data->SetYTitle("N_{Tight}/N_{Loose}");
		h_ratio_mc  ->SetYTitle("N_{Tight}/N_{Loose}");

		h_ratio_data->SetMarkerColor(kBlack);
		h_ratio_data->SetMarkerStyle(20);
		h_ratio_data->SetMarkerSize(1.3);
		h_ratio_data->SetLineWidth(2);
		h_ratio_data->SetLineColor(kBlack);
		h_ratio_data->SetFillColor(kBlack);

		h_ratio_mc  ->SetMarkerColor(kRed);
		h_ratio_mc  ->SetMarkerStyle(20);
		h_ratio_mc  ->SetMarkerSize(1.3);
		h_ratio_mc  ->SetLineWidth(2);
		h_ratio_mc  ->SetLineColor(kRed);
		h_ratio_mc  ->SetFillColor(kRed);

		TLatex *lat = new TLatex();
		lat->SetNDC(kTRUE);
		lat->SetTextColor(kBlack);
		lat->SetTextSize(0.04);

		TLegend *leg = new TLegend(0.70,0.75,0.89,0.88);
		leg->AddEntry(h_ratio_data, "Data","f");
		leg->AddEntry(h_ratio_mc,   "MC",  "f");
		leg->SetFillStyle(0);
		leg->SetTextFont(42);
		leg->SetBorderSize(0);

		TCanvas *c_temp = new TCanvas("C_Temp", "fRatio", 0, 0, 800, 600);
		c_temp->cd();

		// gPad->SetLogy();
		h_ratio_mc  ->DrawCopy("PE X0");
		h_ratio_data->DrawCopy("PE X0 same");
		leg->Draw();
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
		lat->DrawLatex(0.11,0.92, name);
		double ymean(0.), yrms(0.);
		getWeightedYMeanRMS(h_ratio_data, ymean, yrms);
		lat->SetTextSize(0.03);
		lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));

		// Util::PrintNoEPS(c_temp, "FRatio_" + name + "_" + FRatioPlots::var_name[i], fOutputDir + fOutputSubDir, NULL);
		Util::PrintPDF(  c_temp, "FRatio_" + name + "_" + FRatioPlots::var_name[i], fOutputDir + fOutputSubDir);
		delete c_temp, leg, lat;
		delete h_ratio_data, h_ratio_mc;
	}
	fOutputSubDir = "";
}
void MuonPlotter::makeNTightLoosePlots(gChannel chan){
	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Elec) name = "Electrons";

	fOutputSubDir = "Ratios/" + name + "/NTightLoose/";
	char cmd[100];
	sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	system(cmd);

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Elec){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	// Customization
	TString axis_name[gNRatioVars] = {"N_{Jets}",  "H_{T} (GeV)", "P_{T}(Hardest Jet) (GeV)", "N_{Vertices}", "p_{T}(Closest Jet) (GeV)", "p_{T}(Away Jet) (GeV)", "N_{BJets}", "E_{T}^{miss} (GeV)", "m_{T} (GeV)"};

	for(size_t i = 0; i < gNRatioVars; ++i){
		THStack *hsntight = new THStack(Form("NTight_%s", FRatioPlots::var_name[i].Data()), "Stack of tight");
		THStack *hsnloose = new THStack(Form("NLoose_%s", FRatioPlots::var_name[i].Data()), "Stack of loose");
		const unsigned int nmcsamples = mcsamples.size();

		// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
		TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
		for(size_t j = 0; j < mcsamples.size(); ++j){
			Sample *S = fSamples[mcsamples[j]];
			FRatioPlots *rat;
			if(chan == Muon)     rat = &S->ratioplots[0];
			if(chan == Elec) rat = &S->ratioplots[1];
			rat->ntight[i]->SetFillColor(S->color);
			rat->nloose[i]->SetFillColor(S->color);
			float scale = fLumiNorm / S->lumi;
			rat->ntight[i]->Scale(scale);
			rat->nloose[i]->Scale(scale);
			hsntight->Add(rat->ntight[i]);
			hsnloose->Add(rat->nloose[i]);

			if(rat->nloose[i]->Integral() < 1 ) continue;
			leg->AddEntry(rat->ntight[i], S->sname.Data(), "f");
		}
		hsntight->Draw();
		hsntight->GetXaxis()->SetTitle(axis_name[i]);
		hsnloose->Draw();
		hsnloose->GetXaxis()->SetTitle(axis_name[i]);
		leg->SetFillStyle(0);
		leg->SetTextFont(42);
		leg->SetBorderSize(0);

		TCanvas *c_tight = new TCanvas(Form("NTight_%s", FRatioPlots::var_name[i].Data()), "Tight Stack", 0, 0, 800, 600);
		TCanvas *c_loose = new TCanvas(Form("NLoose_%s", FRatioPlots::var_name[i].Data()), "Loose Stack", 0, 0, 800, 600);


		c_tight->cd();
		gPad->SetLogy();
		hsntight->Draw("hist");
		leg->Draw();

		c_loose->cd();
		gPad->SetLogy();
		hsnloose->Draw("hist");
		leg->Draw();

		Util::PrintNoEPS(c_tight, Form("NTight_%s", FRatioPlots::var_name[i].Data()), fOutputDir + fOutputSubDir, fOutputFile);
		Util::PrintNoEPS(c_loose, Form("NLoose_%s", FRatioPlots::var_name[i].Data()), fOutputDir + fOutputSubDir, fOutputFile);	
		delete hsntight, hsnloose, c_tight, c_loose, leg;
	}
	fOutputSubDir = "";
}

void MuonPlotter::makeIsoVsMETPlot(gSample sample){
	fOutputSubDir = "IsoVsMETPlots/";
	fCurrentSample = sample;
	fCurrentChannel = Muon;
	const int nisobins = 20;
	const int nmetbins = 7;
	float metbins[nmetbins+1] = {0., 5., 10., 15., 20., 30., 40., 1000.};
	float ratio[nmetbins];
	// Color_t colors[nmetbins] = {1, 1, 1, 1, 1};
	Color_t colors[nmetbins] = {1, 12, 39, 38, 32, 30, 29};
	// Color_t colors[nmetbins] = {52, 63, 74, 85, 96};
	Style_t styles[nmetbins] = {20, 21, 22, 23, 34, 33, 29};
	TH1F* hiso[nmetbins];

	TTree *tree = fSamples[sample]->getTree();
	for(int i = 0; i < nmetbins; ++i){
		TString name = Form("h%d",i+1);
		hiso[i] = new TH1F(name, Form("MET%.0fto%.0f", metbins[i], metbins[i+1]), nisobins, 0., 1.);
		hiso[i]->Sumw2();
		hiso[i]->SetLineWidth(1);
		hiso[i]->SetLineColor(colors[i]);
		hiso[i]->SetMarkerColor(colors[i]);
		hiso[i]->SetMarkerStyle(styles[i]);
		hiso[i]->SetMarkerSize(1.3);
		hiso[i]->SetFillStyle(0);
		hiso[i]->SetXTitle("RelIso(#mu)");
	}

	tree->ResetBranchAddresses();
	if(fSamples[sample]->datamc < 1) Init(tree);
	if(fSamples[sample]->datamc > 0) InitMC(tree);
	for (Long64_t jentry=0; jentry<tree->GetEntriesFast();jentry++) {
		tree->GetEntry(jentry);
		printProgress(jentry, tree->GetEntriesFast(), fSamples[sample]->name);
		if(singleMuTrigger() == false) continue;
		int mu1(-1), mu2(-1);
		if(hasLooseMuons(mu1, mu2) < 1) continue;
		setHypLepton1(mu1, Muon);
		if(!passesJet50Cut())           continue;
		if(!passesNJetCut(1))           continue;
		if(MuMT[mu1] > 20.)             continue;
		if(NMus > 1)                    continue;
		for(size_t j = 0; j < nmetbins; ++j) if(pfMET > metbins[j] && pfMET < metbins[j+1]) hiso[j]->Fill(MuIso[mu1], singleMuPrescale());
	}

	for(int i = 0; i < nmetbins; ++i){
		ratio[i] = hiso[i]->Integral(1, hiso[i]->FindBin(0.1499))/hiso[i]->Integral(1, nisobins);
		hiso[i]->Scale(1./hiso[i]->Integral());
	}

	TLegend *leg = new TLegend(0.35,0.15,0.70,0.40);
	// TLegend *leg = new TLegend(0.60,0.65,0.88,0.88);
	for(int i = 0; i < nmetbins; ++i) leg->AddEntry(hiso[i], Form("T/L = %5.2f: %.0f < E_{T}^{miss} < %.0f", ratio[i], metbins[i], metbins[i+1]), "p");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	// leg->SetTextSize(0.03);
	leg->SetBorderSize(0);

	TLine *line = new TLine(0.15, 0.00, 0.15, 0.08);
	line->SetLineStyle(3);

	TCanvas *c_temp = new TCanvas("C_HTvsMET", "HT vs MET in Data vs MC", 0, 0, 800, 600);
	c_temp->cd();
	hiso[0]->Draw("axis");
	hiso[0]->GetYaxis()->SetRangeUser(0., 0.08);
	for(int i = 0; i < nmetbins; ++i) hiso[i]->DrawCopy("PE X0 same");
	leg->Draw();
	line->Draw();
	fLatex->DrawLatex(0.10,0.92, fSamples[sample]->name);
	Util::PrintNoEPS( c_temp, "IsoVsMET_" + fSamples[sample]->sname, fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(   c_temp, "IsoVsMET_" + fSamples[sample]->sname, fOutputDir + fOutputSubDir);

	// Cleanup
	delete leg, c_temp;
	for(int i = 0; i < nmetbins; ++i) hiso[i]->Delete();
	fOutputSubDir = "";
	fSamples[sample]->cleanUp();
}

//____________________________________________________________________________
void MuonPlotter::produceRatio(gChannel chan, int sample, int index, bool(MuonPlotter::*eventSelector)(), bool(MuonPlotter::*objSelector)(int), TH2D *&h_2d, TH1D *&h_pt, TH1D *&h_eta, bool output){
	vector<int> samples; samples.push_back(sample);
	produceRatio(chan, samples, index, eventSelector, objSelector, h_2d, h_pt, h_eta, output);
}
void MuonPlotter::produceRatio(gChannel chan, vector<int> samples, int index, bool(MuonPlotter::*eventSelector)(), bool(MuonPlotter::*objSelector)(int), TH2D *&h_2d, TH1D *&h_pt, TH1D *&h_eta, bool output){
// Base function for production of all ratios
/*
TODO Fix treatment of statistical errors and luminosity scaling here!
*/
	gStyle->SetOptStat(0);
h_2d->Sumw2();
h_pt->Sumw2();
h_eta->Sumw2();
TString sname = "Mu";
TString fname = "Muon";
if(chan == Elec){
	sname = "El";
	fname = "Electron";
}
TH2D *H_ntight = new TH2D(Form("%sNTight", sname.Data()), Form("NTight %ss", fname.Data()), h_2d->GetNbinsX(), h_2d->GetXaxis()->GetXbins()->GetArray(), h_2d->GetNbinsY(),  h_2d->GetYaxis()->GetXbins()->GetArray());
TH2D *H_nloose = new TH2D(Form("%sNLoose", sname.Data()), Form("NLoose %ss", fname.Data()), h_2d->GetNbinsX(), h_2d->GetXaxis()->GetXbins()->GetArray(), h_2d->GetNbinsY(),  h_2d->GetYaxis()->GetXbins()->GetArray());
H_ntight->Sumw2();
H_nloose->Sumw2();

if(fVerbose>2) cout << "---------------" << endl;
for(size_t i = 0; i < samples.size(); ++i){
	int sample = samples[i];
	TTree *tree = fSamples[sample]->getTree();
	if(fVerbose>2) cout << "Producing ratios for " << fSamples[sample]->sname << endl;
	tree->ResetBranchAddresses();
	if(fSamples[sample]->datamc < 1) Init(tree);
	if(fSamples[sample]->datamc > 0) InitMC(tree);
	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();

	float scale = fLumiNorm / fSamples[sample]->lumi;
	if(fSamples[sample]->datamc == 0) scale = 1;

	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;
		printProgress(jentry, nentries, fSamples[sample]->name);

		if((*this.*eventSelector)() == false) continue;
		if((*this.*objSelector)(index) == false) continue;

		if(chan == Muon){
			if(isLooseMuon(index)) H_nloose->Fill(MuPt[index], MuEta[index], scale); // Tight or loose
			if(isTightMuon(index)) H_ntight->Fill(MuPt[index], MuEta[index], scale); // Tight
		}
		if(chan == Elec){
			if(isLooseElectron(index)) H_nloose->Fill(ElPt[index], ElEta[index], scale); // Tight or loose
			if(isTightElectron(index)) H_ntight->Fill(ElPt[index], ElEta[index], scale); // Tight
		}

	}
	cout << endl;
	fSamples[sample]->cleanUp();

	if(fVerbose>2) cout << " Tight entries so far: " << H_ntight->GetEntries() << " / " << H_ntight->Integral() << endl;
	if(fVerbose>2) cout << " Loose entries so far: " << H_nloose->GetEntries() << " / " << H_nloose->Integral() << endl;
	if(fVerbose>2) cout << "  Ratio: " << (double)H_ntight->GetEntries()/(double)H_nloose->GetEntries() << endl;
}
h_2d->Divide(H_ntight, H_nloose, 1., 1., "B"); // binomial, weights are ignored

TH1D *hloosept  = H_nloose->ProjectionX();
TH1D *hlooseeta = H_nloose->ProjectionY();
TH1D *htightpt  = H_ntight->ProjectionX();
TH1D *htighteta = H_ntight->ProjectionY();
h_pt ->Divide(htightpt,  hloosept,  1., 1., "B");
h_eta->Divide(htighteta, hlooseeta, 1., 1., "B");

h_pt ->SetXTitle(convertVarName(sname + "Pt[0]"));
h_eta->SetXTitle(convertVarName(sname + "Eta[0]"));
h_pt ->SetYTitle("# Tight / # Loose");
h_eta->SetYTitle("# Tight / # Loose");
h_2d->SetXTitle(convertVarName(sname + "Pt[0]"));
h_2d->SetYTitle(convertVarName(sname + "Eta[0]"));
TString name = "";
for(size_t i = 0; i < samples.size(); ++i){
	int sample = samples[i];
	name += h_2d->GetName();
	name += "_";
	name += fSamples[sample]->sname;
}
if(output){
	printObject(h_2d,  sname + "Ratio"    + name, "colz");
	printObject(h_pt,  sname + "RatioPt"  + name, "PE1");
	printObject(h_eta, sname + "RatioEta" + name, "PE1");
}
delete H_ntight, H_nloose, hloosept, hlooseeta, htightpt, htighteta;
}

//____________________________________________________________________________
TH1D* MuonPlotter::fillMuRatioPt(int sample, int muon, bool(MuonPlotter::*eventSelector)(), bool(MuonPlotter::*muonSelector)(int), bool output){
	vector<int> samples; samples.push_back(sample);
	return fillMuRatioPt(samples, muon, eventSelector, muonSelector, output);
}
TH1D* MuonPlotter::fillMuRatioPt(vector<int> samples, int muon, bool(MuonPlotter::*eventSelector)(), bool(MuonPlotter::*muonSelector)(int), bool output){
	gStyle->SetOptStat(0);
	TH2D *h_2d  = new TH2D("MuRatio",    "Ratio of tight to loose Muons vs Pt vs Eta", getNPt2Bins(Muon), getPt2Bins(Muon), getNEtaBins(Muon), getEtaBins(Muon));
	TH1D *h_pt  = new TH1D("MuRatioPt",  "Ratio of tight to loose Muons vs Pt",        getNPt2Bins(Muon), getPt2Bins(Muon));
	TH1D *h_eta = new TH1D("MuRatioEta", "Ratio of tight to loose Muons vs Eta",       getNEtaBins(Muon), getEtaBins(Muon));

	h_pt->SetXTitle(convertVarName("MuPt[0]"));
	h_pt ->SetYTitle("# Tight / # Loose");
	h_pt->GetYaxis()->SetTitleOffset(1.2);

	produceRatio(Muon, samples, muon, eventSelector, muonSelector, h_2d, h_pt, h_eta, output);
	return h_pt;
}
TH1D* MuonPlotter::fillMuRatioPt(vector<int> samples, int muon, bool(MuonPlotter::*eventSelector)(), bool(MuonPlotter::*muonSelector)(int), const int nptbins, const double* ptbins, const int netabins, const double* etabins, bool output){
	gStyle->SetOptStat(0);
	TH2D *h_2d  = new TH2D("MuRatio",    "Ratio of tight to loose Muons vs Pt vs Eta", nptbins, ptbins, netabins, etabins);
	TH1D *h_pt  = new TH1D("MuRatioPt",  "Ratio of tight to loose Muons vs Pt",        nptbins, ptbins);
	TH1D *h_eta = new TH1D("MuRatioEta", "Ratio of tight to loose Muons vs Eta",       netabins, etabins);

	h_pt->SetXTitle(convertVarName("MuPt[0]"));
	h_pt ->SetYTitle("# Tight / # Loose");
	h_pt->GetYaxis()->SetTitleOffset(1.2);

	produceRatio(Muon, samples, muon, eventSelector, muonSelector, h_2d, h_pt, h_eta, output);
	return h_pt;
}

//____________________________________________________________________________
void MuonPlotter::fillMuElRatios(vector<int> samples){
	calculateRatio(samples, Muon,     SigSup, fH2D_MufRatio, fH1D_MufRatioPt, fH1D_MufRatioEta);
	calculateRatio(samples, Muon,     ZDecay, fH2D_MupRatio, fH1D_MupRatioPt, fH1D_MupRatioEta);
	calculateRatio(samples, Elec, SigSup, fH2D_ElfRatio, fH1D_ElfRatioPt, fH1D_ElfRatioEta);
	calculateRatio(samples, Elec, ZDecay, fH2D_ElpRatio, fH1D_ElpRatioPt, fH1D_ElpRatioEta);
}

//____________________________________________________________________________
TH1D* MuonPlotter::fillMuRatioPt(int sample, gFPSwitch fp, bool output){
	vector<int> samples; samples.push_back(sample);
	return fillMuRatioPt(samples, fp);
}
TH1D* MuonPlotter::fillMuRatioPt(vector<int> samples, gFPSwitch fp, bool output){
	gStyle->SetOptStat(0);
	TH2D *h_2d  = new TH2D("MuRatio",    "Ratio of tight to loose Muons vs Pt vs Eta", getNPt2Bins(Muon), getPt2Bins(Muon), getNEtaBins(Muon), getEtaBins(Muon));
	TH1D *h_pt  = new TH1D("MuRatioPt",  "Ratio of tight to loose Muons vs Pt",        getNPt2Bins(Muon), getPt2Bins(Muon));
	TH1D *h_eta = new TH1D("MuRatioEta", "Ratio of tight to loose Muons vs Eta",       getNEtaBins(Muon), getEtaBins(Muon));

	h_pt->SetXTitle(convertVarName("MuPt[0]"));
	h_pt->SetYTitle("# Tight / # Loose");
	h_pt->GetYaxis()->SetTitleOffset(1.2);

	calculateRatio(samples, Muon, fp, h_2d, h_pt, h_eta, output);
	delete h_2d, h_eta;
	return h_pt;
};

//____________________________________________________________________________
TH1D* MuonPlotter::fillElRatioPt(int sample, gFPSwitch fp, bool output){
	vector<int> samples; samples.push_back(sample);
	return fillElRatioPt(samples, fp, output);
}
TH1D* MuonPlotter::fillElRatioPt(vector<int> samples, gFPSwitch fp, bool output){
	gStyle->SetOptStat(0);
	TH2D *h_2d  = new TH2D("ElRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Elec), getPt2Bins(Elec), getNEtaBins(Elec), getEtaBins(Elec));
	TH1D *h_pt  = new TH1D("ElRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Elec), getPt2Bins(Elec));
	TH1D *h_eta = new TH1D("ElRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Elec), getEtaBins(Elec));

	h_pt->SetXTitle(convertVarName("ElPt[0]"));
	h_pt->SetYTitle("# Tight / # Loose");
	h_pt->GetYaxis()->SetTitleOffset(1.2);

	calculateRatio(samples, Elec, fp, h_2d, h_pt, h_eta, output);
	return h_pt;
};

//____________________________________________________________________________
void MuonPlotter::calculateRatio(vector<int> samples, gChannel chan, gFPSwitch fp, TH2D*& h_2d, bool output){
	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", 1, 0.,1.);
	TH1D *h_dummy2 = new TH1D("dummy2", "dummy2", 1, 0.,1.);
	calculateRatio(samples, chan, fp, h_2d, h_dummy1, h_dummy2, output);
	delete h_dummy1, h_dummy2;
}
void MuonPlotter::calculateRatio(vector<int> samples, gChannel chan, gFPSwitch fp, TH2D*& h_2d, TH1D*& h_pt, TH1D*&h_eta, bool output){
/*
TODO Fix treatment of statistical errors and luminosity scaling here!
*/
	gStyle->SetOptStat(0);
h_2d->Sumw2();
h_pt->Sumw2();
h_eta->Sumw2();

TH2D *H_ntight = new TH2D("NTight", "NTight Muons", h_2d->GetNbinsX(), h_2d->GetXaxis()->GetXbins()->GetArray(), h_2d->GetNbinsY(),  h_2d->GetYaxis()->GetXbins()->GetArray());
TH2D *H_nloose = new TH2D("NLoose", "NLoose Muons", h_2d->GetNbinsX(), h_2d->GetXaxis()->GetXbins()->GetArray(), h_2d->GetNbinsY(),  h_2d->GetYaxis()->GetXbins()->GetArray());
H_ntight->Sumw2();
H_nloose->Sumw2();

getPassedTotal(samples, chan, fp, H_ntight, H_nloose, output);
h_2d->Divide(H_ntight, H_nloose, 1., 1., "B");

TH1D *hmuloosept  = H_nloose->ProjectionX();
TH1D *hmulooseeta = H_nloose->ProjectionY();
TH1D *hmutightpt  = H_ntight->ProjectionX();
TH1D *hmutighteta = H_ntight->ProjectionY();

h_pt ->Divide(hmutightpt,  hmuloosept,  1., 1., "B"); // binomial
h_eta->Divide(hmutighteta, hmulooseeta, 1., 1., "B"); // weights are ignored
delete H_ntight, H_nloose, hmuloosept, hmulooseeta, hmutightpt, hmutighteta;
TString name = "";
for(size_t i = 0; i < samples.size(); ++i){
	int sample = samples[i];
	name += h_2d->GetName();
	name += "_";
	name += fSamples[sample]->sname;
}
if(output){
	printObject(h_2d,  TString("Ratio")    + name, "colz");
	printObject(h_pt,  TString("RatioPt")  + name, "PE1");
	printObject(h_eta, TString("RatioEta") + name, "PE1");
}
}
void MuonPlotter::calculateRatio(vector<int> samples, gChannel chan, gFPSwitch fp, float &ratio, float &ratioe){
	double ntight(0.), nloose(0.);
	double ntighte2(0.), nloosee2(0.);
	vector<int> v_ntight, v_nloose;
	vector<float> v_scale;
	vector<TString> v_name;
	for(size_t i = 0; i < samples.size(); ++i){
		int index = samples[i];
		Sample *S = fSamples[index];

		int ntight_sam(0), nloose_sam(0);
		v_name.push_back(S->sname);

		float scale = fLumiNorm/S->lumi; // Normalize all
		if(S->datamc == 0) scale = 1;
		if(fp == SigSup){
			ntight += scale * S->numbers[Baseline][chan].nsst;
			nloose += scale * S->numbers[Baseline][chan].nssl;

			ntight_sam += S->numbers[Baseline][chan].nsst;
			nloose_sam += S->numbers[Baseline][chan].nssl;
		}
		if(fp == ZDecay){
			ntight += scale * S->numbers[Baseline][chan].nzt;
			nloose += scale * S->numbers[Baseline][chan].nzl;

			ntight_sam += S->numbers[Baseline][chan].nzt;
			nloose_sam += S->numbers[Baseline][chan].nzl;
		}
		v_ntight.push_back(ntight_sam);
		v_nloose.push_back(nloose_sam);
		v_scale.push_back(scale);
	}
	ratioWithBinomErrors(ntight, nloose, ratio, ratioe);
	// ratioWithPoissErrors(ntight, nloose, ratio, ratioe);
	if(fVerbose > 2){
		cout << "--------------------------------------------------------" << endl;
		TString s_forp;
		if(fp == SigSup) s_forp = "f Ratio";
		if(fp == ZDecay) s_forp = "p Ratio";
		TString s_channel;
		if(chan == Muon)     s_channel = "Muon";
		if(chan == Elec) s_channel = "Electron";
		cout << " Calling calculateRatio for " << s_forp << " in " << s_channel << " channel..." << endl;
		for(size_t i = 0; i < v_ntight.size(); ++i){
			cout << setw(9) << v_name[i] << ": ";
			cout << " Nt:    " << setw(8) << setprecision(2) << v_ntight[i];
			cout << " Nl:    " << setw(8) << setprecision(2) << v_nloose[i];
			cout << " Scale: " << v_scale[i] << endl;
		}
		cout << "--------------------------------------------------------" << endl;
		cout << " Total: ";
		cout << " Nt:    " << setw(8) << ntight;
		cout << " Nl:    " << setw(8) << nloose;
		cout << " Ratio: " << ratio << endl;
		cout << "--------------------------------------------------------" << endl;
	}
}
void MuonPlotter::calculateRatio(vector<int> samples, gChannel chan, gFPSwitch fp, float &ratio, float &ratioeup, float &ratioelow){
	// Careful, this method only takes integer numbers for passed/total events, therefore
	// only makes sense for application on data right now.
	int ntight(0), nloose(0);
	float ntighte2(0.), nloosee2(0.);
	vector<int> v_ntight, v_nloose;
	vector<TString> v_name;
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];

		int ntight_sam(0), nloose_sam(0);
		v_name.push_back(S->sname);

		float scale = fLumiNorm/S->lumi; // Normalize all
		if(S->datamc == 0) scale = 1;
		if(fp == SigSup){
			ntight += scale * S->numbers[Baseline][chan].nsst;
			nloose += scale * S->numbers[Baseline][chan].nssl;

			ntight_sam += S->numbers[Baseline][chan].nsst;
			nloose_sam += S->numbers[Baseline][chan].nssl;
		}
		if(fp == ZDecay){
			ntight += S->numbers[Baseline][chan].nzt;
			nloose += S->numbers[Baseline][chan].nzl;

			ntight_sam += S->numbers[Baseline][chan].nzt;
			nloose_sam += S->numbers[Baseline][chan].nzl;
		}
		v_ntight.push_back(ntight_sam);
		v_nloose.push_back(nloose_sam);
	}
	ratioWithAsymmCPErrors(ntight, nloose, ratio, ratioeup, ratioelow);
	if(fVerbose > 2){
		cout << "--------------------------------------------------------" << endl;
		TString s_forp;
		if(fp == SigSup) s_forp = "f Ratio";
		if(fp == ZDecay) s_forp = "p Ratio";
		TString s_channel;
		if(chan == Muon)     s_channel = "Muon";
		if(chan == Elec) s_channel = "Electron";
		cout << " Calling calculateRatio for " << s_forp << " in " << s_channel << " channel..." << endl;
		for(size_t i = 0; i < v_ntight.size(); ++i){
			cout << setw(9) << v_name[i] << ": ";
			cout << " Nt:    " << setw(8) << setprecision(2) << v_ntight[i];
			cout << " Nl:    " << setw(8) << setprecision(2) << v_nloose[i];
			cout << endl;
		}
		cout << "--------------------------------------------------------" << endl;
		cout << " Total: ";
		cout << " Nt:    " << setw(8) << ntight;
		cout << " Nl:    " << setw(8) << nloose;
		cout << " Ratio: " << ratio << " + " << ratioeup << " - " << ratioelow << endl;
		cout << "--------------------------------------------------------" << endl;
	}
}

//____________________________________________________________________________
void MuonPlotter::getPassedTotal(vector<int> samples, gChannel chan, gFPSwitch fp, TH2D*& h_passed, TH2D*& h_total, bool output, gHiLoSwitch hilo){
	// toggle: choose binning: 0: pt, default, 1: nvrtx, 2: closest jet pt
	if(fVerbose>2) cout << "---------------" << endl;
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];

		float scale = fLumiNorm / S->lumi;
		if(S->datamc == 0) scale = 1;

		Channel *C;
		if(chan == Muon)     C = &S->region[Baseline][hilo].mm;
		if(chan == Elec) C = &S->region[Baseline][hilo].ee;
		TH2D *ntight, *nloose;
		if(fp == SigSup){
			ntight = C->fntight;
			nloose = C->fnloose;
		} else if(fp == ZDecay){
			ntight = C->pntight;
			nloose = C->pnloose;
		}
		h_passed->Add(ntight, scale);
		h_total ->Add(nloose, scale);
	}
	TString name = "";
	for(size_t i = 0; i < samples.size(); ++i){
		int sample = samples[i];
		if(i > 0) name += "_";
		name += fSamples[sample]->sname;
	}
	if(output){
		printObject(h_passed, TString("Passed") + name, "colz");
		printObject(h_total,  TString("Total")  + name, "colz");
	}	
}

TH1D* MuonPlotter::getFRatio(vector<int> samples, gChannel chan, int ratiovar, bool output){
	gStyle->SetOptStat(0);

	TH1D *ntight = new TH1D("h_NTight",  "NTight",  FRatioPlots::nbins[ratiovar], FRatioPlots::xmin[ratiovar], FRatioPlots::xmax[ratiovar]);
	TH1D *nloose = new TH1D("h_NLoose",  "NLoose",  FRatioPlots::nbins[ratiovar], FRatioPlots::xmin[ratiovar], FRatioPlots::xmax[ratiovar]);
	TH1D *ratio  = new TH1D("h_TLRatio", "TLRatio", FRatioPlots::nbins[ratiovar], FRatioPlots::xmin[ratiovar], FRatioPlots::xmax[ratiovar]);
	ntight->Sumw2(); nloose->Sumw2(); ratio->Sumw2();

	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];

		float scale = fLumiNorm / S->lumi;
		if(S->datamc == 0) scale = 1;

		FRatioPlots *RP;
		if(chan == Muon)     RP = &S->ratioplots[0];
		if(chan == Elec) RP = &S->ratioplots[1];
		ntight->Add(RP->ntight[ratiovar], scale);
		nloose->Add(RP->nloose[ratiovar], scale);
	}

	ratio->Divide(ntight, nloose, 1., 1., "B");

	delete ntight, nloose;
	return ratio;
}

//____________________________________________________________________________
void MuonPlotter::ratioWithBinomErrors(float ntight, float nloose, float &ratio, float &error){
	ratio = ntight/nloose;
	error = TMath::Sqrt( ntight*(1.0-ntight/nloose) ) / nloose;                  // Binomial
}
void MuonPlotter::ratioWithPoissErrors(float ntight, float nloose, float &ratio, float &error){
	ratio = ntight/nloose;
	error = TMath::Sqrt( ntight*ntight*(nloose+ntight)/(nloose*nloose*nloose) ); // Poissonian	
}
void MuonPlotter::ratioWithAsymmCPErrors(int passed, int total, float &ratio, float &upper, float &lower){
	TEfficiency *eff = new TEfficiency("TempEfficiency", "TempEfficiency", 1, 0., 1.);
	eff->SetStatisticOption(TEfficiency::kFCP); // Frequentist Clopper Pearson = default
	eff->SetConfidenceLevel(0.683); // 1-sigma = default
	if( eff->SetTotalEvents(1, total) && eff->SetPassedEvents(1, passed) ){
		ratio = eff->GetEfficiency(1);
		upper = eff->GetEfficiencyErrorUp(1);
		lower = eff->GetEfficiencyErrorLow(1);
	}
	else{
		ratio = 1;
		upper = 1;
		lower = 0;
	};
	delete eff;
}

//____________________________________________________________________________
void MuonPlotter::makeSSMuMuPredictionPlots(vector<int> samples, bool output, gHiLoSwitch hilo){
	fOutputSubDir = "MuMuPredictions";
	// Need filled muon ratios before calling this function!

	// Prediction: /////////////////////////////////////////////////////////////////////
	TH1D *H_nsigpred = new TH1D("MuMuNsigpred", "Predicted N_sig in Pt1 bins",       getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nfppred  = new TH1D("MuMuNfppred",  "Predicted N_fp in Pt1 bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nffpred  = new TH1D("MuMuNffpred",  "Predicted N_ff in Pt1 bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nFpred   = new TH1D("MuMuNFpred",   "Total predicted fakes in Pt1 bins", getNPt2Bins(Muon),  getPt2Bins(Muon));
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm/S->lumi;
		Channel *C = &S->region[Baseline][hilo].mm;

		vector<TH1D*> prediction = MuMuFPPrediction(fH2D_MufRatio, fH2D_MupRatio, C->nt20_pt, C->nt10_pt, C->nt00_pt, output);
		H_nsigpred->Add(prediction[0], scale);
		H_nfppred ->Add(prediction[1], scale);
		H_nffpred ->Add(prediction[2], scale);
	}

	// Observation: ////////////////////////////////////////////////////////////////////
	TH1D *H_nsigobs  = new TH1D("Nsigobs", "Observed N_sig in Pt1 bins",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	vector<int> lm0sample; lm0sample.push_back(LM0);
	NObs(Muon, H_nsigobs, lm0sample, &MuonPlotter::isGenMatchedSUSYDiLepEvent);

	TH1D *H_nt2obs  = new TH1D("Nt2obs", "Observed N_t2 in Pt1 bins",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(Muon, H_nt2obs, samples);
	// NObs(Muon, H_nt2obs, samples, &MuonPlotter::isSSTTEvent);

	// THStack *HS_nt2obs = new THStack("Nt2obs_stacked", "Observed N_t2 in Pt1 bins");
	// NObs(Muon, HS_nt2obs, samples);

	TH1D *H_nt2obsSM = new TH1D("Nt2obsSM", "Observed N_t2 in Pt1 bins, ttbar only",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(Muon, H_nt2obsSM, fMCBG);
	// vector<int> ttbarsample; ttbarsample.push_back(TTbar);
	// NObs(Muon, H_nt2obsttbar, ttbarsample, &MuonPlotter::isSSTTEvent);	

	// Output
	H_nsigobs->SetXTitle(convertVarName("MuPt[0]"));
	H_nsigobs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	H_nFpred->Add(H_nfppred);
	H_nFpred->Add(H_nffpred);
	H_nFpred->SetXTitle(convertVarName("MuPt[0]"));
	H_nFpred->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obs->SetXTitle(convertVarName("MuPt[0]"));
	H_nt2obs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obsSM->SetXTitle(convertVarName("MuPt[0]"));
	H_nt2obsSM->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	// Normalize to binwidth
	H_nsigpred = normHistBW(H_nsigpred, fBinWidthScale);
	H_nsigobs  = normHistBW(H_nsigobs,  fBinWidthScale);
	H_nfppred  = normHistBW(H_nfppred,  fBinWidthScale);
	H_nffpred  = normHistBW(H_nffpred,  fBinWidthScale);
	H_nFpred   = normHistBW(H_nFpred,   fBinWidthScale);
	H_nt2obs   = normHistBW(H_nt2obs,   fBinWidthScale);
	H_nt2obsSM = normHistBW(H_nt2obsSM, fBinWidthScale);

	H_nt2obs->SetFillColor(kBlue);
	H_nt2obs->SetLineColor(kBlue);
	H_nt2obs->SetFillStyle(3004);
	H_nt2obs->SetLineWidth(2);

	H_nsigpred->SetFillColor(8);
	H_nsigpred->SetMarkerColor(8);
	H_nsigpred->SetMarkerStyle(20);
	H_nsigpred->SetLineColor(8);
	H_nsigpred->SetLineWidth(2);

	H_nfppred->SetFillColor(kRed);
	H_nfppred->SetMarkerColor(kRed);
	H_nfppred->SetMarkerStyle(20);
	H_nfppred->SetLineColor(kRed);
	H_nfppred->SetLineWidth(2);

	H_nffpred->SetFillColor(13);
	H_nffpred->SetMarkerColor(13);
	H_nffpred->SetLineColor(13);
	H_nffpred->SetMarkerStyle(20);
	H_nffpred->SetLineWidth(2);

	plotOverlay4H(H_nt2obs, "N_{ t2}", H_nsigpred, "N_{ pp}" , H_nfppred, "N_{ f p}", H_nffpred, "N_{ f f}");

	H_nsigpred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerStyle(20);

	plotPredOverlay2HWithRatio(H_nsigobs, "Observed N_{sig}", H_nsigpred, "Predicted N_{sig}");
	plotPredOverlay2HWithRatio(H_nt2obs,  "Observed N_{t2}",  H_nFpred,   "Predicted Fakes");
	plotPredOverlay3HWithRatio(H_nFpred,  "Predicted Fakes",  H_nt2obs,   "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets, LM0)", H_nt2obsSM, "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets)", false, false);
	fOutputSubDir = "";
}
void MuonPlotter::makeSSElElPredictionPlots(vector<int> samples, bool output, gHiLoSwitch hilo){
	fOutputSubDir = "ElElPredictions";
	// Need filled electron ratios before calling this function!

	// Prediction: /////////////////////////////////////////////////////////////////////
	TH1D *H_nsigpred = new TH1D("ElElNsigpred", "Predicted N_sig in Pt1 bins",       getNPt2Bins(Elec),  getPt2Bins(Elec));
	TH1D *H_nfppred  = new TH1D("ElElNfppred",  "Predicted N_fp in Pt1 bins",        getNPt2Bins(Elec),  getPt2Bins(Elec));
	TH1D *H_nffpred  = new TH1D("ElElNffpred",  "Predicted N_ff in Pt1 bins",        getNPt2Bins(Elec),  getPt2Bins(Elec));
	TH1D *H_nFpred   = new TH1D("ElElNFpred",   "Total predicted fakes in Pt1 bins", getNPt2Bins(Elec),  getPt2Bins(Elec));
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm/S->lumi;

		Channel *C = &S->region[Baseline][hilo].ee;
		vector<TH1D*> prediction = ElElFPPrediction(fH2D_ElfRatio, fH2D_ElpRatio, C->nt20_pt, C->nt10_pt, C->nt00_pt, output);
		H_nsigpred->Add(prediction[0], scale);
		H_nfppred ->Add(prediction[1], scale);
		H_nffpred ->Add(prediction[2], scale);
	}

	// Observation: ////////////////////////////////////////////////////////////////////
	TH1D *H_nsigobs  = new TH1D("Nsigobs", "Observed N_sig in Pt1 bins",  getNPt2Bins(Elec),  getPt2Bins(Elec));
	vector<int> lm0sample; lm0sample.push_back(LM0);
	NObs(Elec, H_nsigobs, lm0sample, &MuonPlotter::isGenMatchedSUSYEEEvent);

	TH1D *H_nt2obs  = new TH1D("Nt2obs", "Observed N_t2 in Pt1 bins",  getNPt2Bins(Elec),  getPt2Bins(Elec));
	NObs(Elec, H_nt2obs, samples);
	// NObs(H_nt2obs, samples, &MuonPlotter::isSSTTEvent);

	TH1D *H_nt2obsSM = new TH1D("Nt2obsSM", "Observed N_t2 in Pt1 bins, ttbar only",  getNPt2Bins(Elec),  getPt2Bins(Elec));
	// vector<int> ttbarsample; ttbarsample.push_back(TTbar);
	NObs(Elec, H_nt2obsSM, fMCBG);
	// NObs(Elec, H_nt2obsttbar, ttbarsample, &MuonPlotter::isSSTTEvent);	

	// Output
	H_nsigobs->SetXTitle(convertVarName("ElPt[0]"));
	H_nsigobs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	H_nFpred->Add(H_nfppred);
	H_nFpred->Add(H_nffpred);
	H_nFpred->SetXTitle(convertVarName("ElPt[0]"));
	H_nFpred->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obs->SetXTitle(convertVarName("ElPt[0]"));
	H_nt2obs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obsSM->SetXTitle(convertVarName("ElPt[0]"));
	H_nt2obsSM->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	// Normalize to binwidth
	H_nsigpred = normHistBW(H_nsigpred, fBinWidthScale);
	H_nsigobs  = normHistBW(H_nsigobs,  fBinWidthScale);
	H_nfppred  = normHistBW(H_nfppred,  fBinWidthScale);
	H_nffpred  = normHistBW(H_nffpred,  fBinWidthScale);
	H_nFpred   = normHistBW(H_nFpred,   fBinWidthScale);
	H_nt2obs   = normHistBW(H_nt2obs,   fBinWidthScale);
	H_nt2obsSM = normHistBW(H_nt2obsSM, fBinWidthScale);

	H_nt2obs->SetFillColor(kBlue);
	H_nt2obs->SetLineColor(kBlue);
	H_nt2obs->SetFillStyle(3004);
	H_nt2obs->SetLineWidth(2);

	H_nsigpred->SetFillColor(8);
	H_nsigpred->SetMarkerColor(8);
	H_nsigpred->SetMarkerStyle(20);
	H_nsigpred->SetLineColor(8);
	H_nsigpred->SetLineWidth(2);

	H_nfppred->SetFillColor(kRed);
	H_nfppred->SetMarkerColor(kRed);
	H_nfppred->SetMarkerStyle(20);
	H_nfppred->SetLineColor(kRed);
	H_nfppred->SetLineWidth(2);

	H_nffpred->SetFillColor(13);
	H_nffpred->SetMarkerColor(13);
	H_nffpred->SetLineColor(13);
	H_nffpred->SetMarkerStyle(20);
	H_nffpred->SetLineWidth(2);

	plotOverlay4H(H_nt2obs, "N_{ t2}", H_nsigpred, "N_{ pp}" , H_nfppred, "N_{ f p}", H_nffpred, "N_{ f f}");

	H_nsigpred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerStyle(20);

	plotPredOverlay2HWithRatio(H_nsigobs, "Observed N_{sig}", H_nsigpred, "Predicted N_{sig}");
	plotPredOverlay2HWithRatio(H_nt2obs, "Observed N_{t2}", H_nFpred, "Predicted Fakes");
	plotPredOverlay3HWithRatio(H_nFpred, "Predicted Fakes", H_nt2obs,  "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets, LM0)", H_nt2obsSM, "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets)", false, false);
	fOutputSubDir = "";
}
void MuonPlotter::makeSSElMuPredictionPlots(vector<int> samples, bool output, gHiLoSwitch hilo){
	fOutputSubDir = "ElMuPredictions";
	// Need filled electron and muon ratios before calling this function!

	// Prediction: /////////////////////////////////////////////////////////////////////
	TH1D *H_npppred = new TH1D("ElMuNpppred", "Predicted N_pp in MuPt bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nfppred = new TH1D("ElMuNfppred", "Predicted N_fp in MuPt bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_npfpred = new TH1D("ElMuNpfpred", "Predicted N_pf in MuPt bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nffpred = new TH1D("ElMuNffpred", "Predicted N_ff in MuPt bins",        getNPt2Bins(Muon),  getPt2Bins(Muon));
	TH1D *H_nFpred  = new TH1D("ElMuNFpred",  "Total predicted fakes in MuPt bins", getNPt2Bins(Muon),  getPt2Bins(Muon));
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm/S->lumi;

		Channel *C = &S->region[Baseline][hilo].em;
		if(fVerbose > 2) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
		if(fVerbose > 2) cout << "Calling FPRatios for " << S->sname << endl;
		vector<TH1D*> prediction = ElMuFPPrediction(fH2D_MufRatio, fH2D_MupRatio, fH2D_ElfRatio, fH2D_ElpRatio, C->nt20_pt, C->nt10_pt, C->nt01_pt, C->nt00_pt, output);
		H_npppred->Add(prediction[0], scale);
		H_nfppred->Add(prediction[1], scale);
		H_npfpred->Add(prediction[2], scale);
		H_nffpred->Add(prediction[3], scale);
	}

	// Observation: ////////////////////////////////////////////////////////////////////
	TH1D *H_nsigobs  = new TH1D("Nsigobs", "Observed N_sig in MuPt bins",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	vector<int> lm0sample; lm0sample.push_back(LM0);
	NObs(ElMu, H_nsigobs, lm0sample, &MuonPlotter::isGenMatchedSUSYEMuEvent);

	TH1D *H_nt2obs  = new TH1D("Nt2obs", "Observed N_t2 in MuPt bins",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(ElMu, H_nt2obs, samples);

	TH1D *H_nt2obsSM = new TH1D("Nt2obsSM", "Observed N_t2 in MuPt bins, ttbar only",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(ElMu, H_nt2obsSM, fMCBG);

	// Output
	H_nsigobs->SetXTitle(convertVarName("MuPt[0]"));
	H_nsigobs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	H_nFpred->Add(H_nfppred);
	H_nFpred->Add(H_npfpred);
	H_nFpred->Add(H_nffpred);
	H_nFpred->SetXTitle(convertVarName("MuPt[0]"));
	H_nFpred->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obs->SetXTitle(convertVarName("MuPt[0]"));
	H_nt2obs->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));
	H_nt2obsSM->SetXTitle(convertVarName("MuPt[0]"));
	H_nt2obsSM->SetYTitle(Form("Events / %2.0f GeV", fBinWidthScale));

	// Normalize to binwidth
	H_npppred  = normHistBW(H_npppred,  fBinWidthScale);
	H_nsigobs  = normHistBW(H_nsigobs,  fBinWidthScale);
	H_nfppred  = normHistBW(H_nfppred,  fBinWidthScale);
	H_npfpred  = normHistBW(H_npfpred,  fBinWidthScale);
	H_nffpred  = normHistBW(H_nffpred,  fBinWidthScale);
	H_nFpred   = normHistBW(H_nFpred,   fBinWidthScale);
	H_nt2obs   = normHistBW(H_nt2obs,   fBinWidthScale);
	H_nt2obsSM = normHistBW(H_nt2obsSM, fBinWidthScale);

	H_nt2obs->SetFillColor(kBlue);
	H_nt2obs->SetLineColor(kBlue);
	H_nt2obs->SetFillStyle(3004);
	H_nt2obs->SetLineWidth(2);

	int ppcolor = 8;
	H_npppred->SetFillColor(  ppcolor);
	H_npppred->SetMarkerColor(ppcolor);
	H_npppred->SetLineColor(  ppcolor);
	H_npppred->SetMarkerStyle(20);
	H_npppred->SetLineWidth(2);

	int fpcolor = 2;
	H_nfppred->SetFillColor(  fpcolor);
	H_nfppred->SetMarkerColor(fpcolor);
	H_nfppred->SetLineColor(  fpcolor);
	H_nfppred->SetMarkerStyle(20);
	H_nfppred->SetLineWidth(2);

	int pfcolor = 51;
	H_npfpred->SetFillColor(  pfcolor);
	H_npfpred->SetMarkerColor(pfcolor);
	H_npfpred->SetLineColor(  pfcolor);
	H_npfpred->SetMarkerStyle(20);
	H_npfpred->SetLineWidth(2);

	int ffcolor = 13;
	H_nffpred->SetFillColor(  ffcolor);
	H_nffpred->SetMarkerColor(ffcolor);
	H_nffpred->SetLineColor(  ffcolor);
	H_nffpred->SetMarkerStyle(20);
	H_nffpred->SetLineWidth(2);

	plotOverlay5H(H_nt2obs, "N_{ t2}", H_npppred, "N_{ pp}" , H_nfppred, "N_{ f p}", H_npfpred, "N_{ p f}", H_nffpred, "N_{ f f}");

	H_npppred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerColor(kRed);
	H_nFpred->SetMarkerStyle(20);
	plotPredOverlay2HWithRatio(H_nsigobs, "Observed N_{sig}", H_npppred, "Predicted N_{sig}");
	plotPredOverlay2HWithRatio(H_nt2obs,  "Observed N_{t2}",  H_nFpred,  "Predicted Fakes");
	plotPredOverlay3HWithRatio(H_nFpred,  "Predicted Fakes",  H_nt2obs,  "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets, LM0)", H_nt2obsSM, "Obs. N_{t2} (QCD, t#bar{t}+jets, V+jets)", false, false);
	fOutputSubDir = "";
}

//____________________________________________________________________________
void MuonPlotter::NObs(gChannel chan, TH1D *&hist, vector<int> samples, bool(MuonPlotter::*eventSelector)()){
/* This fills a histogram with the pt of the first muon for a given selection */
	hist->Sumw2();
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm / S->lumi;

		TTree *tree = S->getTree();
		tree->ResetBranchAddresses();
		Init(tree);
		if (fChain == 0) return;
		Long64_t nentries = fChain->GetEntriesFast();
		Long64_t nbytes = 0, nb = 0;
		for (Long64_t jentry=0; jentry<nentries;jentry++) {
			Long64_t ientry = LoadTree(jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(jentry);   nbytes += nb;
			printProgress(jentry, nentries, S->name);

			if((*this.*eventSelector)() == false) continue;

			if((chan == Muon || chan == ElMu) && NMus > 0) hist->Fill(MuPt[0], scale);
			if( chan == Elec             && NEls > 0) hist->Fill(ElPt[0], scale);
		}
		S->cleanUp();
	}	
}
void MuonPlotter::NObs(gChannel chan, TH1D *&hist, vector<int> samples, gRegion reg, gHiLoSwitch hilo){
	hist->Sumw2();
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm / S->lumi;
		Channel *C;
		if(chan == Muon)     C = &S->region[reg][hilo].mm;
		if(chan == ElMu)      C = &S->region[reg][hilo].em;
		if(chan == Elec) C = &S->region[reg][hilo].ee;
		hist->Add(C->nt20_pt->ProjectionX(), scale);
	}	
}
void MuonPlotter::NObs(gChannel chan, THStack *&stack, vector<int> samples, gRegion reg, gHiLoSwitch hilo){
	// hist->Sumw2();
	const int nsamples = samples.size();
	TH1D *hnt2[nsamples];
	for(size_t i = 0; i < nsamples; ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm / S->lumi;
		Channel *C;
		if(chan == Muon)     C = &S->region[reg][hilo].mm;
		if(chan == ElMu)      C = &S->region[reg][hilo].em;
		if(chan == Elec) C = &S->region[reg][hilo].ee;
		hnt2[i] = (TH1D*)C->nt20_pt->ProjectionX()->Clone(Form("Nt2_%s", S->sname.Data())); // not sure if i need the clone here...
		hnt2[i]->SetFillColor(S->color);
		hnt2[i]->Sumw2();
		hnt2[i]->Scale(scale);
		stack->Add(hnt2[i]);
	}	
}

//____________________________________________________________________________
void MuonPlotter::makeAllIntPredictions(){
	fOutputSubDir = Util::MakeOutputDir("IntPredictions");
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	// access a chararray containing the date with asctime(timeinfo)

	TString tablefilename = fOutputDir + fOutputSubDir + "Table2.tex";
	TString didarfilename = fOutputDir + fOutputSubDir + "forDidar.txt";
	fOUTSTREAM.open(tablefilename.Data(), ios::trunc);
	fOUTSTREAM << "==========================================================================================================" << endl;
	fOUTSTREAM << " Table 2 inputs from ETH Analysis" << endl;
	fOUTSTREAM << Form(" Generated on: %s ", asctime(timeinfo)) << endl;
	fOUTSTREAM << endl;

	fOUTSTREAM2.open(didarfilename.Data(), ios::trunc);
	fOUTSTREAM2 << "////////////////////////////////////////////////////////////////////" << endl;
	fOUTSTREAM2 << "// Plot inputs from ETH Analysis" << endl;
	fOUTSTREAM2 << Form("// Generated on: %s ", asctime(timeinfo)) << endl;
	fOUTSTREAM2 << "// Format is {ee, mm, em, total}" << endl;
	fOUTSTREAM2 << "// Errors are on sum of backgrounds" << endl;
	fOUTSTREAM2 << endl;
	for(size_t i = 0; i < gNREGIONS; ++i){
		TString outputname = fOutputDir + fOutputSubDir + "DataPred_" + Region::sname[i] + ".txt";
		makeIntPrediction(outputname, gRegion(i));
	}
	fOUTSTREAM.close();
	fOUTSTREAM2.close();
	fOutputSubDir = "";
}
void MuonPlotter::makeIntPrediction(TString filename, gRegion reg, gHiLoSwitch hilo){
	ofstream OUT(filename.Data(), ios::trunc);

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	vector<int> musamples;
	vector<int> elsamples;
	vector<int> emusamples;

	// TODO: Check these samples!
	musamples = fMuData;
	elsamples = fEGData;
	emusamples = fMuEGData;

	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;
	OUT << " Producing integrated predictions for region " << Region::sname[reg] << endl;
	OUT << "  scaling MC to " << fLumiNorm << " /pb" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_data(0.),  mufratio_data_e(0.);
	float mupratio_data(0.),  mupratio_data_e(0.);
	float elfratio_data(0.),  elfratio_data_e(0.);
	float elpratio_data(0.),  elpratio_data_e(0.);
	float mufratio_allmc(0.), mufratio_allmc_e(0.);
	float mupratio_allmc(0.), mupratio_allmc_e(0.);
	float elfratio_allmc(0.), elfratio_allmc_e(0.);
	float elpratio_allmc(0.), elpratio_allmc_e(0.);

	calculateRatio(fMuData, Muon, SigSup, mufratio_data, mufratio_data_e);
	calculateRatio(fMuData, Muon, ZDecay, mupratio_data, mupratio_data_e);

	calculateRatio(fEGData, Elec, SigSup, elfratio_data, elfratio_data_e);
	calculateRatio(fEGData, Elec, ZDecay, elpratio_data, elpratio_data_e);

	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	calculateRatio(fMCBG,      Elec, SigSup, elfratio_allmc, elfratio_allmc_e);
	calculateRatio(fMCBG,      Elec, ZDecay, elpratio_allmc, elpratio_allmc_e);

	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt2_mm(0.), nt10_mm(0.), nt0_mm(0.);
	float nt2_em(0.), nt10_em(0.), nt01_em(0.), nt0_em(0.);
	float nt2_ee(0.), nt10_ee(0.), nt0_ee(0.);

	// OS yields
	float nt2_ee_BB_os(0.), nt2_ee_EE_os(0.), nt2_ee_EB_os(0.);
	float nt2_em_BB_os(0.), nt2_em_EE_os(0.);

	for(size_t i = 0; i < musamples.size(); ++i){
		Sample *S = fSamples[musamples[i]];
		nt2_mm  += S->numbers[reg][Muon].nt2;
		nt10_mm += S->numbers[reg][Muon].nt10;
		nt0_mm  += S->numbers[reg][Muon].nt0;
	}
	for(size_t i = 0; i < emusamples.size(); ++i){
		Sample *S = fSamples[emusamples[i]];
		nt2_em  += S->numbers[reg][ElMu].nt2;
		nt10_em += S->numbers[reg][ElMu].nt10;
		nt01_em += S->numbers[reg][ElMu].nt01;
		nt0_em  += S->numbers[reg][ElMu].nt0;

		nt2_em_BB_os += S->region[reg][hilo].em.nt20_OS_BB_pt->GetEntries(); // ele in barrel
		nt2_em_EE_os += S->region[reg][hilo].em.nt20_OS_EE_pt->GetEntries(); // ele in endcal
	}
	for(size_t i = 0; i < elsamples.size(); ++i){
		Sample *S = fSamples[elsamples[i]];
		nt2_ee  += S->numbers[reg][Elec].nt2;
		nt10_ee += S->numbers[reg][Elec].nt10;
		nt0_ee  += S->numbers[reg][Elec].nt0;

		nt2_ee_BB_os += S->region[reg][hilo].ee.nt20_OS_BB_pt->GetEntries(); // both in barrel
		nt2_ee_EE_os += S->region[reg][hilo].ee.nt20_OS_EE_pt->GetEntries(); // both in endcal
		nt2_ee_EB_os += S->region[reg][hilo].ee.nt20_OS_EB_pt->GetEntries(); // one barrel, one endcap
	}

	///////////////////////////////////////////////////////////////////////////////////
	// PRINTOUT ///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	OUT << "---------------------------------------------------------------------------------------------------------" << endl;
	OUT << "         RATIOS  ||     Mu-fRatio      |     Mu-pRatio      ||     El-fRatio      |     El-pRatio      ||" << endl;
	OUT << "---------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "        allMC    ||";
	OUT << setw(7)  << setprecision(2) << mufratio_allmc << " +/- " << setw(7) << setprecision(2) << mufratio_allmc_e << " |";
	OUT << setw(7)  << setprecision(2) << mupratio_allmc << " +/- " << setw(7) << setprecision(2) << mupratio_allmc_e << " ||";
	OUT << setw(7)  << setprecision(2) << elfratio_allmc << " +/- " << setw(7) << setprecision(2) << elfratio_allmc_e << " |";
	OUT << setw(7)  << setprecision(2) << elpratio_allmc << " +/- " << setw(7) << setprecision(2) << elpratio_allmc_e << " ||";
	OUT << endl;
	OUT << setw(16) << "  data stat only ||";
	OUT << setw(7)  << setprecision(2) << mufratio_data  << " +/- " << setw(7) << setprecision(2) << mufratio_data_e  << " |";
	OUT << setw(7)  << setprecision(2) << mupratio_data  << " +/- " << setw(7) << setprecision(2) << mupratio_data_e  << " ||";
	OUT << setw(7)  << setprecision(2) << elfratio_data  << " +/- " << setw(7) << setprecision(2) << elfratio_data_e  << " |";
	OUT << setw(7)  << setprecision(2) << elpratio_data  << " +/- " << setw(7) << setprecision(2) << elpratio_data_e  << " ||";
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------------" << endl << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "         YIELDS  ||   Nt2   |   Nt1   |   Nt0   ||   Nt2   |   Nt10  |   Nt01  |   Nt0   ||   Nt2   |   Nt1   |   Nt0   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;

	float nt2sum_mm(0.), nt10sum_mm(0.), nt0sum_mm(0.);
	float nt2sum_em(0.), nt10sum_em(0.), nt01sum_em(0.), nt0sum_em(0.);
	float nt2sum_ee(0.), nt10sum_ee(0.), nt0sum_ee(0.);
	for(size_t i = 0; i < fMCBG.size(); ++i){
		Sample *S = fSamples[fMCBG[i]];
		float scale = fLumiNorm / S->lumi;
		nt2sum_mm  += scale*S->numbers[reg][Muon].nt2;
		nt10sum_mm += scale*S->numbers[reg][Muon].nt10;
		nt0sum_mm  += scale*S->numbers[reg][Muon].nt0;
		nt2sum_em  += scale*S->numbers[reg][ElMu].nt2;
		nt10sum_em += scale*S->numbers[reg][ElMu].nt10;
		nt01sum_em += scale*S->numbers[reg][ElMu].nt01;
		nt0sum_em  += scale*S->numbers[reg][ElMu].nt0;
		nt2sum_ee  += scale*S->numbers[reg][Elec].nt2;
		nt10sum_ee += scale*S->numbers[reg][Elec].nt10;
		nt0sum_ee  += scale*S->numbers[reg][Elec].nt0;

		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon].nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon].nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon].nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][ElMu].nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][ElMu].nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][ElMu].nt01 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][ElMu].nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Elec].nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Elec].nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Elec].nt0  << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "MC sum" << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_mm  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_mm ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_mm  ) << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_em  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_em ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt01sum_em ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_em  ) << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_ee  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_ee ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_ee  ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		if(S->datamc != 2) continue;
		float scale = fLumiNorm / S->lumi;
		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Muon].nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Muon].nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Muon].nt0  ) << " || ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][ElMu].nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][ElMu].nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][ElMu].nt01 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][ElMu].nt0  ) << " || ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Elec].nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Elec].nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", scale * S->numbers[reg][Elec].nt0  ) << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "data"  << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_mm ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_mm) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_mm ) << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_em ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_em) << " | ";
	OUT << setw(7) << Form("%4.0f", nt01_em) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_em ) << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_ee ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_ee) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_ee ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// PREDICTIONS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	FakeRatios *FR = new FakeRatios();
	FR->setNToyMCs(100); // speedup
	FR->setAddESyst(0.5);
	// FR->setNToyMCs(100);
	// FR->setAddESyst(0.0);

	// FR->setMFRatio(mufratio_data, 0.10);
	// FR->setEFRatio(elfratio_data, 0.10);
	// FR->setMPRatio(mupratio_data, 0.05);
	// FR->setEPRatio(elpratio_data, 0.05);
	FR->setMFRatio(mufratio_data, mufratio_data_e); // set error to pure statistical of ratio
	FR->setEFRatio(elfratio_data, elfratio_data_e);
	FR->setMPRatio(mupratio_data, mupratio_data_e);
	FR->setEPRatio(elpratio_data, elpratio_data_e);

	FR->setMMNtl(nt2_mm, nt10_mm, nt0_mm);
	FR->setEENtl(nt2_ee, nt10_ee, nt0_ee);
	FR->setEMNtl(nt2_em, nt10_em, nt01_em, nt0_em);


	OUT << "  PREDICTIONS (Data yields and ratios)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " Mu/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << FR->getMMNpp();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getMMNppEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getMMNppESyst() << " (syst)" << endl;
	OUT << "  Npf:           " << setw(7) << setprecision(3) << FR->getMMNpf();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getMMNpfEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getMMNpfESyst() << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << FR->getMMNff();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getMMNffEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getMMNffESyst() << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << FR->getMMTotFakes();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getMMTotEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getMMTotESyst() << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << FR->getEMNpp();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEMNppEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEMNppESyst() << " (syst)" << endl;
	OUT << "  Npf:           " << setw(7) << setprecision(3) << FR->getEMNpf();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEMNpfEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEMNpfESyst() << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << FR->getEMNfp();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEMNfpEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEMNfpESyst() << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << FR->getEMNff();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEMNffEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEMNffESyst() << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << FR->getEMTotFakes();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEMTotEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEMTotESyst() << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/E Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << FR->getEENpp();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEENppEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEENppESyst() << " (syst)" << endl;
	OUT << "  Npf:           " << setw(7) << setprecision(3) << FR->getEENpf();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEENpfEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEENpfESyst() << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << FR->getEENff();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEENffEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEENffESyst() << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << FR->getEETotFakes();
	OUT << " +/- "             << setw(7) << setprecision(3) << FR->getEETotEStat();
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << FR->getEETotESyst() << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// E-CHARGE MISID /////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt2_ee_chmid(0.), nt2_ee_chmid_e1(0.), nt2_ee_chmid_e2(0.);
	float nt2_em_chmid(0.), nt2_em_chmid_e1(0.), nt2_em_chmid_e2(0.);

	// Abbreviations
	float fb  = gEChMisIDB;
	float fbE = gEChMisIDB_E;
	float fe  = gEChMisIDE;
	float feE = gEChMisIDE_E;

	// Simple error propagation assuming error on number of events is sqrt(N)
	nt2_ee_chmid    = 2*fb*nt2_ee_BB_os + 2*fe*nt2_ee_EE_os + (fb+fe)*nt2_ee_EB_os;
	nt2_ee_chmid_e1 = sqrt( (4*fb*fb*FR->getEStat2(nt2_ee_BB_os)) + (4*fe*fe*FR->getEStat2(nt2_ee_EE_os)) + (fb+fe)*(fb+fe)*FR->getEStat2(nt2_ee_EB_os) ); // stat only
	nt2_ee_chmid_e2 = sqrt( (4*nt2_ee_BB_os*nt2_ee_BB_os*fbE*fbE) + (4*nt2_ee_EE_os*nt2_ee_EE_os*feE*feE) + (fbE*fbE+feE*feE)*nt2_ee_EB_os*nt2_ee_EB_os ); // syst only

	nt2_em_chmid    = fb*nt2_em_BB_os + fe*nt2_em_EE_os;
	nt2_em_chmid_e1 = sqrt( fb*fb*FR->getEStat2(nt2_em_BB_os) + fe*fe*FR->getEStat2(nt2_em_EE_os) );
	nt2_em_chmid_e2 = sqrt( nt2_em_BB_os*nt2_em_BB_os * fbE*fbE + nt2_em_EE_os*nt2_em_EE_os * feE*feE );

	///////////////////////////////////////////////////////////////////////////////////
	// PRINTOUT ///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	OUT << "--------------------------------------------------------------" << endl;
	OUT << "       E-ChMisID  ||       Barrel       |       Endcap      ||" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << "                  ||";
	OUT << setw(7)  << setprecision(2) << fb  << " +/- " << setw(7) << setprecision(3) << fbE  << " |";
	OUT << setw(7)  << setprecision(2) << fe  << " +/- " << setw(7) << setprecision(3) << feE  << " ||";
	OUT << endl;
	OUT << "--------------------------------------------------------------" << endl << endl;

	OUT << "-----------------------------------------------------------------------" << endl;
	OUT << "                 ||       E/Mu        ||             E/E             ||" << endl;
	OUT << "      OS-YIELDS  ||   N_B   |   N_E   ||   N_BB  |   N_EB  |   N_EE  ||" << endl;
	OUT << "-----------------------------------------------------------------------" << endl;

	float mc_os_em_bb_sum(0.), mc_os_em_ee_sum(0.);
	float mc_os_ee_bb_sum(0.), mc_os_ee_eb_sum(0.), mc_os_ee_ee_sum(0.);

	for(size_t i = 0; i < fMCBG.size(); ++i){
		Sample *S = fSamples[fMCBG[i]];
		float scale = fLumiNorm / S->lumi;

		mc_os_em_bb_sum += scale*S->region[reg][hilo].em.nt20_OS_BB_pt->GetEntries();
		mc_os_em_ee_sum += scale*S->region[reg][hilo].em.nt20_OS_EE_pt->GetEntries();
		mc_os_ee_bb_sum += scale*S->region[reg][hilo].ee.nt20_OS_BB_pt->GetEntries();
		mc_os_ee_eb_sum += scale*S->region[reg][hilo].ee.nt20_OS_EB_pt->GetEntries();
		mc_os_ee_ee_sum += scale*S->region[reg][hilo].ee.nt20_OS_EE_pt->GetEntries();

		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg][hilo].em.nt20_OS_BB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg][hilo].em.nt20_OS_EE_pt->GetEntries() << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg][hilo].ee.nt20_OS_BB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg][hilo].ee.nt20_OS_EB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg][hilo].ee.nt20_OS_EE_pt->GetEntries() << " || ";
		OUT << endl;
	}	
	OUT << "-----------------------------------------------------------------------" << endl;
	OUT << setw(16) << "MC sum" << " || ";
	OUT << setw(7) << Form("%5.1f", mc_os_em_bb_sum ) << " | ";
	OUT << setw(7) << Form("%5.1f", mc_os_em_ee_sum ) << " || ";
	OUT << setw(7) << Form("%5.1f", mc_os_ee_bb_sum ) << " | ";
	OUT << setw(7) << Form("%5.1f", mc_os_ee_eb_sum ) << " | ";
	OUT << setw(7) << Form("%5.1f", mc_os_ee_ee_sum ) << " || ";
	OUT << endl;
	OUT << "-----------------------------------------------------------------------" << endl;
	OUT << setw(16) << "data"  << " || ";
	OUT << setw(7) << Form("%5.0f", nt2_em_BB_os ) << " | ";
	OUT << setw(7) << Form("%5.0f", nt2_em_EE_os ) << " || ";
	OUT << setw(7) << Form("%5.0f", nt2_ee_BB_os ) << " | ";
	OUT << setw(7) << Form("%5.0f", nt2_ee_EB_os ) << " | ";
	OUT << setw(7) << Form("%5.0f", nt2_ee_EE_os ) << " || ";
	OUT << endl;
	OUT << "-----------------------------------------------------------------------" << endl;
	OUT << setw(16) << "pred. SS contr."  << " || ";
	OUT << setw(7) << Form("%6.4f",   fb   * nt2_em_BB_os ) << " | ";
	OUT << setw(7) << Form("%6.4f",   fe   * nt2_em_EE_os ) << " || ";
	OUT << setw(7) << Form("%6.4f", 2*fb   * nt2_ee_BB_os ) << " | ";
	OUT << setw(7) << Form("%6.4f", (fb+fe)* nt2_ee_EB_os ) << " | ";
	OUT << setw(7) << Form("%6.4f", 2*fe   * nt2_ee_EE_os ) << " || ";
	OUT << endl;
	OUT << "-----------------------------------------------------------------------" << endl << endl;
	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;


	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << "       SUMMARY   ||           Mu/Mu           ||           E/Mu            ||           E/E             ||" << endl;
	OUT << "==========================================================================================================" << endl;
	OUT << setw(16) << "pred. fakes"  << " || ";
	OUT << setw(5) << Form("%5.2f", FR->getMMTotFakes()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getMMTotEStat()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getMMTotESyst()) << " || ";
	OUT << setw(5) << Form("%5.2f", FR->getEMTotFakes()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getEMTotEStat()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getEMTotESyst()) << " || ";
	OUT << setw(5) << Form("%5.2f", FR->getEETotFakes()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getEETotEStat()) << " +/- ";
	OUT << setw(5) << Form("%5.2f", FR->getEETotESyst()) << " || " << endl;
	OUT << setw(16) << "pred. chmisid "  << " || ";
	OUT << "                          || ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid_e2) << " || ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid_e2) << " || " << endl;

	OUT << setw(16) << "Rare SM (MC)"  << " || ";
	float nt2_rare_mc_mm(0.),    nt2_rare_mc_em(0.),    nt2_rare_mc_ee(0.);
	float nt2_rare_mc_mm_e1(0.), nt2_rare_mc_em_e1(0.), nt2_rare_mc_ee_e1(0.);
	for(size_t i = 0; i < fMCRareSM.size(); ++i){
		Sample *S = fSamples[fMCRareSM[i]];
		float scale = fLumiNorm/S->lumi;
		nt2_rare_mc_mm += scale * S->numbers[reg][Muon].nt2;
		nt2_rare_mc_em += scale * S->numbers[reg][ElMu].nt2;
		nt2_rare_mc_ee += scale * S->numbers[reg][Elec].nt2;

		nt2_rare_mc_mm_e1 += scale*scale * FR->getEStat2(S->numbers[reg][Muon].nt2);
		nt2_rare_mc_em_e1 += scale*scale * FR->getEStat2(S->numbers[reg][ElMu].nt2);
		nt2_rare_mc_ee_e1 += scale*scale * FR->getEStat2(S->numbers[reg][Elec].nt2);
	}

	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_mm) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_mm_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", 0.5*nt2_rare_mc_mm) << " || ";
	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_em) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_em_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", 0.5*nt2_rare_mc_em) << " || ";
	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_ee) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_rare_mc_ee_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", 0.5*nt2_rare_mc_ee) << " || " << endl;
	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "tot. backgr. "  << " || ";
	// Just add different errors in quadrature (they are independent)
	float mm_tot_sqerr1 = FR->getMMTotEStat()*FR->getMMTotEStat() + nt2_rare_mc_mm_e1;
	float em_tot_sqerr1 = FR->getEMTotEStat()*FR->getEMTotEStat() + nt2_em_chmid_e1*nt2_em_chmid_e1 + nt2_rare_mc_em_e1;
	float ee_tot_sqerr1 = FR->getEETotEStat()*FR->getEETotEStat() + nt2_ee_chmid_e1*nt2_ee_chmid_e1 + nt2_rare_mc_ee_e1;
	float mm_tot_sqerr2 = FR->getMMTotESyst()*FR->getMMTotESyst() + 0.25*nt2_rare_mc_mm*nt2_rare_mc_mm;
	float em_tot_sqerr2 = FR->getEMTotESyst()*FR->getEMTotESyst() + nt2_em_chmid_e2*nt2_em_chmid_e2 + 0.25*nt2_rare_mc_em*nt2_rare_mc_em;
	float ee_tot_sqerr2 = FR->getEETotESyst()*FR->getEETotESyst() + nt2_ee_chmid_e2*nt2_ee_chmid_e2 + 0.25*nt2_rare_mc_ee*nt2_rare_mc_ee;
	OUT << setw(5) << Form("%5.2f", FR->getMMTotFakes() + nt2_rare_mc_mm) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(mm_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(mm_tot_sqerr2)) << " || ";
	OUT << setw(5) << Form("%5.2f", FR->getEMTotFakes() + nt2_em_chmid + nt2_rare_mc_em) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(em_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(em_tot_sqerr2)) << " || ";
	OUT << setw(5) << Form("%5.2f", FR->getEETotFakes() + nt2_ee_chmid + nt2_rare_mc_ee) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(ee_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(ee_tot_sqerr2)) << " || " << endl;
	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "observed"  << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_mm ) << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_em ) << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_ee ) << " || " << endl;
	OUT << "==========================================================================================================" << endl;
	OUT << setw(20) << "combined observed: ";
	OUT << setw(5) << left << Form("%2.0f", nt2_mm+nt2_em+nt2_ee ) << endl;
	OUT << setw(20) << "        predicted: ";
	float tot_pred        = FR->getTotFakes() + nt2_rare_mc_mm + nt2_em_chmid + nt2_rare_mc_em + nt2_ee_chmid + nt2_rare_mc_ee;
	float comb_tot_sqerr1 = FR->getTotEStat()*FR->getTotEStat() + nt2_rare_mc_mm_e1 + nt2_em_chmid_e1*nt2_em_chmid_e1 + nt2_rare_mc_em_e1 + nt2_ee_chmid_e1*nt2_ee_chmid_e1 + nt2_rare_mc_ee_e1;
	float comb_tot_sqerr2 = FR->getTotESyst()*FR->getTotESyst() + 0.25*nt2_rare_mc_mm*nt2_rare_mc_mm + 0.25*nt2_rare_mc_em*nt2_rare_mc_em + 0.25*nt2_rare_mc_ee*nt2_rare_mc_ee + nt2_em_chmid_e2*nt2_em_chmid_e2 + nt2_ee_chmid_e2*nt2_ee_chmid_e2;
	OUT << setw(5) << left << Form("%5.2f", tot_pred ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(comb_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(comb_tot_sqerr2)) << endl;
	OUT << "==========================================================================================================" << endl;
	OUT.close();

	///////////////////////////////////////////////////////////////////////////////////
	//  OUTPUT FOR PAS TABLE  /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	fOUTSTREAM << Region::sname[reg] << endl;
	fOUTSTREAM << Form("{\\bf predicted BG b} & {\\boldmath $%5.2f\\pm %5.2f$} & {\\boldmath $%5.2f \\pm %5.2f$} & {\\boldmath $%5.2f\\pm %5.2f$} & {\\boldmath $%5.2f\\pm %5.2f$} & \\\\ \n",
		FR->getEETotFakes() + nt2_ee_chmid + nt2_rare_mc_ee, sqrt(ee_tot_sqerr1 + ee_tot_sqerr2),
		FR->getMMTotFakes() + nt2_rare_mc_mm,                sqrt(mm_tot_sqerr1 + mm_tot_sqerr2),
		FR->getEMTotFakes() + nt2_em_chmid + nt2_rare_mc_em, sqrt(em_tot_sqerr1 + em_tot_sqerr2),
		tot_pred, sqrt(comb_tot_sqerr1 + comb_tot_sqerr2));
	fOUTSTREAM << Form("{\\bf observed} & {\\bf %2.0f} & {\\bf %2.0f} & {\\bf %2.0f} & {\\bf %2.0f}  & {\\bf XX} \\\\ \\hline \n", nt2_ee, nt2_mm, nt2_em, nt2_ee+nt2_mm+nt2_em);

	///////////////////////////////////////////////////////////////////////////////////
	//  OUTPUT FOR DIDARS PLOT  ///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	fOUTSTREAM2 << "// " + Region::sname[reg] << endl;
	fOUTSTREAM2 << Form("float %s_SS_ETH[4]    = {%6.3f, %6.3f, %6.3f, %6.3f }; \n", Region::sname[reg].Data(), nt2_rare_mc_ee, nt2_rare_mc_mm, nt2_rare_mc_em, nt2_rare_mc_ee + nt2_rare_mc_mm + nt2_rare_mc_em);
	fOUTSTREAM2 << Form("float %s_OS_ETH[4]    = {%6.3f, %6.3f, %6.3f, %6.3f }; \n", Region::sname[reg].Data(), nt2_ee_chmid, 0.00, nt2_em_chmid, nt2_ee_chmid + nt2_em_chmid);
	fOUTSTREAM2 << Form("float %s_PF_ETH[4]    = {%6.3f, %6.3f, %6.3f, %6.3f }; \n", Region::sname[reg].Data(), FR->getEENpf(), FR->getMMNpf(), FR->getEMNpf()+FR->getEMNfp(), FR->getEENpf() + FR->getMMNpf() + FR->getEMNpf() + FR->getEMNfp());
	fOUTSTREAM2 << Form("float %s_FF_ETH[4]    = {%6.3f, %6.3f, %6.3f, %6.3f }; \n", Region::sname[reg].Data(), FR->getEENff(), FR->getMMNff(), FR->getEMNff(), FR->getEENff()+FR->getMMNff()+FR->getEMNff());
	fOUTSTREAM2 << Form("float %s_Error_ETH[4] = {%6.3f, %6.3f, %6.3f, %6.3f }; \n", Region::sname[reg].Data(), sqrt(ee_tot_sqerr1 + ee_tot_sqerr2), sqrt(mm_tot_sqerr1 + mm_tot_sqerr2), sqrt(em_tot_sqerr1 + em_tot_sqerr2), sqrt(comb_tot_sqerr1 + comb_tot_sqerr2));
	fOUTSTREAM2 << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//  OUTPUT AS PLOT  ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	TH1D    *h_obs        = new TH1D("h_observed",   "Observed number of events",  3, 0., 3.);
	TH1D    *h_pred_sfake = new TH1D("h_pred_sfake", "Predicted single fakes", 3, 0., 3.);
	TH1D    *h_pred_dfake = new TH1D("h_pred_dfake", "Predicted double fakes", 3, 0., 3.);
	TH1D    *h_pred_chmid = new TH1D("h_pred_chmid", "Predicted charge mis id", 3, 0., 3.);
	TH1D    *h_pred_mc    = new TH1D("h_pred_mc",    "Predicted WW/WZ/ZZ", 3, 0., 3.);
	TH1D    *h_pred_tot   = new TH1D("h_pred_tot",   "Total Prediction", 3, 0., 3.);
	THStack *hs_pred      = new THStack("hs_predicted", "Predicted number of events");

	h_obs->SetMarkerColor(kBlack);
	h_obs->SetMarkerStyle(20);
	h_obs->SetMarkerSize(2.5);
	h_obs->SetLineWidth(2);
	h_obs->SetLineColor(kBlack);
	h_obs->SetFillColor(kBlack);

	h_pred_sfake->SetLineWidth(1);
	h_pred_dfake->SetLineWidth(1);
	h_pred_chmid->SetLineWidth(1);
	h_pred_mc   ->SetLineWidth(1);
	h_pred_sfake->SetLineColor(50);
	h_pred_sfake->SetFillColor(50);
	h_pred_dfake->SetLineColor(38);
	h_pred_dfake->SetFillColor(38);
	h_pred_chmid->SetLineColor(42);
	h_pred_chmid->SetFillColor(42);
	h_pred_mc   ->SetLineColor(31);
	h_pred_mc   ->SetFillColor(31);
	h_pred_tot  ->SetLineWidth(1);
	h_pred_tot  ->SetFillColor(kBlack);
	h_pred_tot  ->SetFillStyle(3013);

	// Add numbers:
	h_obs->SetBinContent(1, nt2_ee);
	h_obs->SetBinContent(2, nt2_mm);
	h_obs->SetBinContent(3, nt2_em);
	h_obs->SetBinError(1, FR->getEStat(nt2_ee)); // FIXME
	h_obs->SetBinError(2, FR->getEStat(nt2_mm)); // FIXME
	h_obs->SetBinError(3, FR->getEStat(nt2_em)); // FIXME

	h_pred_sfake->SetBinContent(1, FR->getEENpf());
	h_pred_sfake->SetBinContent(2, FR->getMMNpf());
	h_pred_sfake->SetBinContent(3, FR->getEMNpf()+FR->getEMNfp());
	h_pred_sfake->GetXaxis()->SetBinLabel(1, "ee");
	h_pred_sfake->GetXaxis()->SetBinLabel(2, "#mu#mu");
	h_pred_sfake->GetXaxis()->SetBinLabel(3, "e#mu");

	h_pred_dfake->SetBinContent(1, FR->getEENff());
	h_pred_dfake->SetBinContent(2, FR->getMMNff());
	h_pred_dfake->SetBinContent(3, FR->getEMNff());

	h_pred_chmid->SetBinContent(1, nt2_ee_chmid);
	h_pred_chmid->SetBinContent(2, 0.);
	h_pred_chmid->SetBinContent(3, nt2_em_chmid);

	h_pred_mc->SetBinContent(1, nt2_rare_mc_ee);
	h_pred_mc->SetBinContent(2, nt2_rare_mc_mm);
	h_pred_mc->SetBinContent(3, nt2_rare_mc_em);

	h_pred_tot->Add(h_pred_sfake);
	h_pred_tot->Add(h_pred_dfake);
	h_pred_tot->Add(h_pred_chmid);
	h_pred_tot->Add(h_pred_mc);
	h_pred_tot->SetBinError(1, sqrt(ee_tot_sqerr1 + ee_tot_sqerr2));
	h_pred_tot->SetBinError(2, sqrt(mm_tot_sqerr1 + mm_tot_sqerr2));
	h_pred_tot->SetBinError(3, sqrt(em_tot_sqerr1 + em_tot_sqerr2));

	hs_pred->Add(h_pred_sfake);
	hs_pred->Add(h_pred_dfake);
	hs_pred->Add(h_pred_chmid);
	hs_pred->Add(h_pred_mc);

	double max = h_obs->Integral();
	h_obs       ->SetMaximum(max>1?max+1:1.);
	h_pred_sfake->SetMaximum(max>1?max+1:1.);
	h_pred_dfake->SetMaximum(max>1?max+1:1.);
	h_pred_chmid->SetMaximum(max>1?max+1:1.);
	h_pred_mc   ->SetMaximum(max>1?max+1:1.);
	h_pred_tot  ->SetMaximum(max>1?max+1:1.);
	hs_pred     ->SetMaximum(max>1?max+1:1.);

	hs_pred->Draw("goff");
	hs_pred->GetXaxis()->SetBinLabel(1, "ee");
	hs_pred->GetXaxis()->SetBinLabel(2, "#mu#mu");
	hs_pred->GetXaxis()->SetBinLabel(3, "e#mu");
	hs_pred->GetXaxis()->SetLabelOffset(0.01);
	hs_pred->GetXaxis()->SetLabelFont(42);
	hs_pred->GetXaxis()->SetLabelSize(0.1);

	TLegend *leg = new TLegend(0.15,0.65,0.50,0.88);
	leg->AddEntry(h_obs,        "Observed","p");
	leg->AddEntry(h_pred_sfake, "Single Fakes","f");
	leg->AddEntry(h_pred_dfake, "Double Fakes","f");
	leg->AddEntry(h_pred_chmid, "Charge MisID","f");
	leg->AddEntry(h_pred_mc,    "Irreducible (MC)","f");
	leg->AddEntry(h_pred_tot,    "Total Uncertainty","f");
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	// leg->SetTextSize(0.05);
	leg->SetBorderSize(0);

	TCanvas *c_temp = new TCanvas("C_ObsPred", "Observed vs Predicted", 0, 0, 800, 600);
	c_temp->cd();

	hs_pred->Draw("hist");
	h_pred_tot->DrawCopy("0 E2 same");
	h_obs->DrawCopy("PE X0 same");
	leg->Draw();

	lat->SetTextSize(0.03);
	lat->DrawLatex(0.16,0.60, Form("H_{T} > %.0f GeV, N_{Jets} #geq %1d", Region::minHT[reg], Region::minNjets[reg]));
	if(reg != Control) lat->DrawLatex(0.16,0.55, Form("E_{T}^{miss} > %.0f GeV", Region::minMet[reg]));
	if(reg == Control) lat->DrawLatex(0.16,0.55, Form("E_{T}^{miss} > %.0f GeV, < %.0f GeV", Region::minMet[reg], Region::maxMet[reg]));
	drawTopLine();

	gPad->RedrawAxis();
	// Util::PrintNoEPS(c_temp, "ObsPred_" + Region::sname[reg], fOutputDir + fOutputSubDir, NULL);
	Util::PrintPDF(c_temp,   "ObsPred_" + Region::sname[reg], fOutputDir + fOutputSubDir);
	delete c_temp;	
	delete h_obs, h_pred_sfake, h_pred_dfake, h_pred_chmid, h_pred_mc, h_pred_tot, hs_pred;
	delete FR;
}

void MuonPlotter::makeDiffPrediction(){
	fOutputSubDir = "DiffPredictionPlots/";

	TLatex *lat = new TLatex();
	lat->SetNDC(kTRUE);
	lat->SetTextColor(kBlack);
	lat->SetTextSize(0.04);

	vector<int> musamples;
	vector<int> elsamples;
	vector<int> emusamples;

	musamples = fMuData;
	elsamples = fEGData;
	emusamples = fMuEGData;

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_data(0.),  mufratio_data_e(0.);
	float mupratio_data(0.),  mupratio_data_e(0.);
	float elfratio_data(0.),  elfratio_data_e(0.);
	float elpratio_data(0.),  elpratio_data_e(0.);

	calculateRatio(fMuData, Muon, SigSup, mufratio_data, mufratio_data_e);
	calculateRatio(fMuData, Muon, ZDecay, mupratio_data, mupratio_data_e);

	calculateRatio(fEGData, Elec, SigSup, elfratio_data, elfratio_data_e);
	calculateRatio(fEGData, Elec, ZDecay, elpratio_data, elpratio_data_e);

	// Loop on the different variables
	for(size_t j = 0; j < gNDiffVars; ++j){
		TString varname    = DiffPredYields::var_name[j];
		const int nbins    = DiffPredYields::nbins[j];
		const double *bins = DiffPredYields::bins[j];

		///////////////////////////////////////////////////////////////////////////////////
		// OBSERVATIONS ///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		TH1D *nt11 = new TH1D(Form("NT11_%s", varname.Data()), varname, nbins, bins); nt11->Sumw2();

		TH1D *nt11_mm = new TH1D(Form("NT11_MM_%s", varname.Data()), varname, nbins, bins); nt11_mm->Sumw2();
		TH1D *nt10_mm = new TH1D(Form("NT10_MM_%s", varname.Data()), varname, nbins, bins); nt10_mm->Sumw2();
		TH1D *nt01_mm = new TH1D(Form("NT01_MM_%s", varname.Data()), varname, nbins, bins); nt01_mm->Sumw2();
		TH1D *nt00_mm = new TH1D(Form("NT00_MM_%s", varname.Data()), varname, nbins, bins); nt00_mm->Sumw2();
		TH1D *nt11_ee = new TH1D(Form("NT11_EE_%s", varname.Data()), varname, nbins, bins); nt11_ee->Sumw2();
		TH1D *nt10_ee = new TH1D(Form("NT10_EE_%s", varname.Data()), varname, nbins, bins); nt10_ee->Sumw2();
		TH1D *nt01_ee = new TH1D(Form("NT01_EE_%s", varname.Data()), varname, nbins, bins); nt01_ee->Sumw2();
		TH1D *nt00_ee = new TH1D(Form("NT00_EE_%s", varname.Data()), varname, nbins, bins); nt00_ee->Sumw2();
		TH1D *nt11_em = new TH1D(Form("NT11_EM_%s", varname.Data()), varname, nbins, bins); nt11_em->Sumw2();
		TH1D *nt10_em = new TH1D(Form("NT10_EM_%s", varname.Data()), varname, nbins, bins); nt10_em->Sumw2();
		TH1D *nt01_em = new TH1D(Form("NT01_EM_%s", varname.Data()), varname, nbins, bins); nt01_em->Sumw2();
		TH1D *nt00_em = new TH1D(Form("NT00_EM_%s", varname.Data()), varname, nbins, bins); nt00_em->Sumw2();

		// OS yields
		TH1D *nt2_os_ee_bb = new TH1D(Form("NT2_OS_EE_BB_%s", varname.Data()), varname, nbins, bins); nt2_os_ee_bb->Sumw2();
		TH1D *nt2_os_ee_eb = new TH1D(Form("NT2_OS_EE_EB_%s", varname.Data()), varname, nbins, bins); nt2_os_ee_eb->Sumw2();
		TH1D *nt2_os_ee_ee = new TH1D(Form("NT2_OS_EE_EE_%s", varname.Data()), varname, nbins, bins); nt2_os_ee_ee->Sumw2();
		TH1D *nt2_os_em_bb = new TH1D(Form("NT2_OS_EM_BB_%s", varname.Data()), varname, nbins, bins); nt2_os_em_bb->Sumw2();
		TH1D *nt2_os_em_ee = new TH1D(Form("NT2_OS_EM_EE_%s", varname.Data()), varname, nbins, bins); nt2_os_em_ee->Sumw2();

		for(size_t i = 0; i < musamples.size(); ++i){
			Sample *S = fSamples[musamples[i]];
			nt11_mm->Add(S->diffyields[Muon].hnt11[j]);
			nt10_mm->Add(S->diffyields[Muon].hnt10[j]);
			nt01_mm->Add(S->diffyields[Muon].hnt01[j]);
			nt00_mm->Add(S->diffyields[Muon].hnt00[j]);
		}
		for(size_t i = 0; i < elsamples.size(); ++i){
			Sample *S = fSamples[elsamples[i]];
			nt11_ee->Add(S->diffyields[Elec].hnt11[j]);
			nt10_ee->Add(S->diffyields[Elec].hnt10[j]);
			nt01_ee->Add(S->diffyields[Elec].hnt01[j]);
			nt00_ee->Add(S->diffyields[Elec].hnt00[j]);

			nt2_os_ee_bb->Add(S->diffyields[Elec].hnt2_os_BB[j]);
			nt2_os_ee_eb->Add(S->diffyields[Elec].hnt2_os_EB[j]);
			nt2_os_ee_ee->Add(S->diffyields[Elec].hnt2_os_EE[j]);
		}
		for(size_t i = 0; i < emusamples.size(); ++i){
			Sample *S = fSamples[emusamples[i]];
			nt11_em->Add(S->diffyields[ElMu].hnt11[j]);
			nt10_em->Add(S->diffyields[ElMu].hnt10[j]);
			nt01_em->Add(S->diffyields[ElMu].hnt01[j]);
			nt00_em->Add(S->diffyields[ElMu].hnt00[j]);

			nt2_os_em_bb->Add(S->diffyields[ElMu].hnt2_os_BB[j]);
			nt2_os_em_ee->Add(S->diffyields[ElMu].hnt2_os_EE[j]);
		}

		nt11->Add(nt11_mm);
		nt11->Add(nt11_ee);
		nt11->Add(nt11_em);


		///////////////////////////////////////////////////////////////////////////////////
		// Errors /////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		// Save the squared sum of total error in the bin errors of these histos,
		// then apply square root before plotting
		TH1D *totbg    = new TH1D(Form("TotBG_%s",    varname.Data()), varname, nbins, bins); totbg   ->Sumw2();
		TH1D *totbg_mm = new TH1D(Form("TotBG_mm_%s", varname.Data()), varname, nbins, bins); totbg_mm->Sumw2();
		TH1D *totbg_em = new TH1D(Form("TotBG_em_%s", varname.Data()), varname, nbins, bins); totbg_em->Sumw2();
		TH1D *totbg_ee = new TH1D(Form("TotBG_ee_%s", varname.Data()), varname, nbins, bins); totbg_ee->Sumw2();

		///////////////////////////////////////////////////////////////////////////////////
		// MC Predictions /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		// TH1D *nt11_mc = new TH1D(Form("NT11_MC_%s", varname.Data()), varname, nbins, bins); nt11_mc->Sumw2();
		// 
		// TH1D *nt11_mm_mc = new TH1D(Form("NT11_MM_MC_%s", varname.Data()), varname, nbins, bins); nt11_mm_mc->Sumw2();
		// TH1D *nt11_ee_mc = new TH1D(Form("NT11_EE_MC_%s", varname.Data()), varname, nbins, bins); nt11_ee_mc->Sumw2();
		// TH1D *nt11_em_mc = new TH1D(Form("NT11_EM_MC_%s", varname.Data()), varname, nbins, bins); nt11_em_mc->Sumw2();
		// 
		// for(size_t i = 0; i < fMCBG.size(); ++i){
		// 	Sample *S = fSamples[fMCBG[i]];
		// 	float scale = fLumiNorm / S->lumi;
		// 	nt11_mm_mc->Add(S->diffyields[Muon].hnt11[j], scale);
		// 	nt11_ee_mc->Add(S->diffyields[Elec].hnt11[j], scale);
		// 	nt11_em_mc->Add(S->diffyields[ElMu].hnt11[j], scale);
		// }
		// 
		// nt11_mc->Add(nt11_mm_mc);
		// nt11_mc->Add(nt11_ee_mc);
		// nt11_mc->Add(nt11_em_mc);

		///////////////////////////////////////////////////////////////////////////////////
		// RARE SM MC /////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		TH1D *nt11_ss = new TH1D(Form("NT11_SS_%s", varname.Data()), varname, nbins, bins); nt11_ss->Sumw2();

		TH1D *nt11_mm_ss = new TH1D(Form("NT11_MM_SS_%s", varname.Data()), varname, nbins, bins); nt11_mm_ss->Sumw2();
		TH1D *nt11_ee_ss = new TH1D(Form("NT11_EE_SS_%s", varname.Data()), varname, nbins, bins); nt11_ee_ss->Sumw2();
		TH1D *nt11_em_ss = new TH1D(Form("NT11_EM_SS_%s", varname.Data()), varname, nbins, bins); nt11_em_ss->Sumw2();

		for(size_t i = 0; i < fMCRareSM.size(); ++i){
			FakeRatios *FR = new FakeRatios();
			Sample *S = fSamples[fMCRareSM[i]];
			float scale = fLumiNorm / S->lumi;
			nt11_mm_ss->Add(S->diffyields[Muon].hnt11[j], scale);
			nt11_ee_ss->Add(S->diffyields[Elec].hnt11[j], scale);
			nt11_em_ss->Add(S->diffyields[ElMu].hnt11[j], scale);

			// Errors
			for(size_t b = 0; b < nbins; ++b){
				float ss_mm = S->diffyields[Muon].hnt11[j]->GetBinContent(b+1);
				float ss_ee = S->diffyields[Elec].hnt11[j]->GetBinContent(b+1);
				float ss_em = S->diffyields[ElMu].hnt11[j]->GetBinContent(b+1);

				float esyst2_mm = 0.25 * ss_mm*ss_mm*scale*scale;
				float esyst2_ee = 0.25 * ss_ee*ss_ee*scale*scale;
				float esyst2_em = 0.25 * ss_em*ss_em*scale*scale;

				float estat2_mm = scale*scale*FR->getEStat2(ss_mm);
				float estat2_ee = scale*scale*FR->getEStat2(ss_ee);
				float estat2_em = scale*scale*FR->getEStat2(ss_em);

				float prev    = totbg   ->GetBinError(b+1);
				float prev_mm = totbg_mm->GetBinError(b+1);
				float prev_em = totbg_em->GetBinError(b+1);
				float prev_ee = totbg_ee->GetBinError(b+1);

				totbg   ->SetBinError(b+1, prev    + esyst2_mm + esyst2_ee + esyst2_em + estat2_mm + estat2_ee + estat2_em);
				totbg_mm->SetBinError(b+1, prev_mm + esyst2_mm + estat2_mm);
				totbg_em->SetBinError(b+1, prev_em + esyst2_em + estat2_em);
				totbg_ee->SetBinError(b+1, prev_ee + esyst2_ee + estat2_ee);
			}
			delete FR;
		}

		nt11_ss->Add(nt11_mm_ss);
		nt11_ss->Add(nt11_ee_ss);
		nt11_ss->Add(nt11_em_ss);

		totbg   ->Add(nt11_ss);
		totbg_mm->Add(nt11_mm_ss);
		totbg_em->Add(nt11_em_ss);
		totbg_ee->Add(nt11_ee_ss);

		///////////////////////////////////////////////////////////////////////////////////
		// FAKE PREDICTIONS ///////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		TH1D *nt11_sf = new TH1D(Form("NT11_SF_%s", varname.Data()), varname, nbins, bins); nt11_sf->Sumw2();
		TH1D *nt11_df = new TH1D(Form("NT11_DF_%s", varname.Data()), varname, nbins, bins); nt11_df->Sumw2();

		TH1D *nt11_mm_sf = new TH1D(Form("NT11_MM_SF_%s", varname.Data()), varname, nbins, bins); nt11_mm_sf->Sumw2();
		TH1D *nt11_ee_sf = new TH1D(Form("NT11_EE_SF_%s", varname.Data()), varname, nbins, bins); nt11_ee_sf->Sumw2();
		TH1D *nt11_em_sf = new TH1D(Form("NT11_EM_SF_%s", varname.Data()), varname, nbins, bins); nt11_em_sf->Sumw2();
		TH1D *nt11_mm_df = new TH1D(Form("NT11_MM_DF_%s", varname.Data()), varname, nbins, bins); nt11_mm_df->Sumw2();
		TH1D *nt11_ee_df = new TH1D(Form("NT11_EE_DF_%s", varname.Data()), varname, nbins, bins); nt11_ee_df->Sumw2();
		TH1D *nt11_em_df = new TH1D(Form("NT11_EM_DF_%s", varname.Data()), varname, nbins, bins); nt11_em_df->Sumw2();

		for(size_t i = 0; i < nbins; ++i){
			FakeRatios *FR = new FakeRatios();
			FR->setNToyMCs(100); // speedup
			FR->setAddESyst(0.5); // additional systematics

			FR->setMFRatio(mufratio_data, mufratio_data_e); // set error to pure statistical of ratio
			FR->setEFRatio(elfratio_data, elfratio_data_e);
			FR->setMPRatio(mupratio_data, mupratio_data_e);
			FR->setEPRatio(elpratio_data, elpratio_data_e);

			FR->setMMNtl(nt11_mm->GetBinContent(i+1), nt10_mm->GetBinContent(i+1) + nt01_mm->GetBinContent(i+1), nt00_mm->GetBinContent(i+1));
			FR->setEENtl(nt11_ee->GetBinContent(i+1), nt10_ee->GetBinContent(i+1) + nt01_ee->GetBinContent(i+1), nt00_ee->GetBinContent(i+1));
			FR->setEMNtl(nt11_em->GetBinContent(i+1), nt10_em->GetBinContent(i+1),  nt01_em->GetBinContent(i+1), nt00_em->GetBinContent(i+1));

			nt11_mm_sf->SetBinContent(i+1, FR->getMMNpf());
			nt11_ee_sf->SetBinContent(i+1, FR->getEENpf());
			nt11_em_sf->SetBinContent(i+1, FR->getEMNpf() + FR->getEMNfp());
			nt11_mm_df->SetBinContent(i+1, FR->getMMNff());
			nt11_ee_df->SetBinContent(i+1, FR->getEENff());
			nt11_em_df->SetBinContent(i+1, FR->getEMNff());

			// Errors
			float esyst2_mm  = FR->getMMTotESyst()*FR->getMMTotESyst();
			float esyst2_ee  = FR->getEETotESyst()*FR->getEETotESyst();
			float esyst2_em  = FR->getEMTotESyst()*FR->getEMTotESyst();
			float esyst2_tot = FR->getTotESyst()  *FR->getTotESyst();
			float estat2_mm  = FR->getMMTotEStat()*FR->getMMTotEStat();
			float estat2_ee  = FR->getEETotEStat()*FR->getEETotEStat();
			float estat2_em  = FR->getEMTotEStat()*FR->getEMTotEStat();
			float estat2_tot = FR->getTotEStat()  *FR->getTotEStat();

			float prev    = totbg   ->GetBinError(i+1);
			float prev_mm = totbg_mm->GetBinError(i+1);
			float prev_em = totbg_em->GetBinError(i+1);
			float prev_ee = totbg_ee->GetBinError(i+1);

			totbg   ->SetBinError(i+1, prev    + esyst2_tot + estat2_tot);
			totbg_mm->SetBinError(i+1, prev_mm + esyst2_mm + estat2_mm);
			totbg_em->SetBinError(i+1, prev_em + esyst2_em + estat2_em);
			totbg_ee->SetBinError(i+1, prev_ee + esyst2_ee + estat2_ee);

			delete FR;
		}

		nt11_sf->Add(nt11_mm_sf);
		nt11_sf->Add(nt11_ee_sf);
		nt11_sf->Add(nt11_em_sf);

		nt11_df->Add(nt11_mm_df);
		nt11_df->Add(nt11_ee_df);
		nt11_df->Add(nt11_em_df);

		totbg   ->Add(nt11_sf);
		totbg   ->Add(nt11_df);
		totbg_mm->Add(nt11_mm_sf);
		totbg_mm->Add(nt11_mm_df);
		totbg_em->Add(nt11_em_sf);
		totbg_em->Add(nt11_em_df);
		totbg_ee->Add(nt11_ee_sf);
		totbg_ee->Add(nt11_ee_df);

		///////////////////////////////////////////////////////////////////////////////////
		// E-CHARGE MISID /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		TH1D *nt11_cm = new TH1D(Form("NT11_CM_%s", varname.Data()), varname, nbins, bins); nt11_cm->Sumw2();

		TH1D *nt11_ee_cm = new TH1D(Form("NT11_EE_CM_%s", varname.Data()), varname, nbins, bins); nt11_ee_cm->Sumw2();
		TH1D *nt11_em_cm = new TH1D(Form("NT11_EM_CM_%s", varname.Data()), varname, nbins, bins); nt11_em_cm->Sumw2();

		// Abbreviations
		float fb  = gEChMisIDB;
		float fbE = gEChMisIDB_E;
		float fe  = gEChMisIDE;
		float feE = gEChMisIDE_E;

		for(size_t i = 0; i < nbins; ++i){
			float nt2_ee_BB_os = nt2_os_ee_bb->GetBinContent(i+1);
			float nt2_ee_EB_os = nt2_os_ee_eb->GetBinContent(i+1);
			float nt2_ee_EE_os = nt2_os_ee_ee->GetBinContent(i+1);
			float nt2_em_BB_os = nt2_os_em_bb->GetBinContent(i+1);
			float nt2_em_EE_os = nt2_os_em_ee->GetBinContent(i+1);

			// Errors
			FakeRatios *FR = new FakeRatios();

			// Simple error propagation assuming error on number of events is FR->getEStat2()
			nt11_ee_cm->SetBinContent(i+1, 2*fb*nt2_ee_BB_os + 2*fe*nt2_ee_EE_os + (fb+fe)*nt2_ee_EB_os);
			float nt11_ee_cm_e1 = sqrt( (4*fb*fb*FR->getEStat2(nt2_ee_BB_os)) + (4*fe*fe*FR->getEStat2(nt2_ee_EE_os)) + (fb+fe)*(fb+fe)*FR->getEStat2(nt2_ee_EB_os) ); // stat only
			float nt11_ee_cm_e2 = sqrt( (4*nt2_ee_BB_os*nt2_ee_BB_os*fbE*fbE) + (4*nt2_ee_EE_os*nt2_ee_EE_os*feE*feE) + (fbE*fbE+feE*feE)*nt2_ee_EB_os*nt2_ee_EB_os ); // syst only

			nt11_em_cm->SetBinContent(i+i, fb*nt2_em_BB_os + fe*nt2_em_EE_os);
			float nt11_em_cm_e1 = sqrt( fb*fb*FR->getEStat2(nt2_em_BB_os) + fe*fe*FR->getEStat2(nt2_em_EE_os) );
			float nt11_em_cm_e2 = sqrt( nt2_em_BB_os*nt2_em_BB_os * fbE*fbE + nt2_em_EE_os*nt2_em_EE_os * feE*feE );

			float esyst2_ee  = nt11_ee_cm_e2*nt11_ee_cm_e2;
			float esyst2_em  = nt11_em_cm_e2*nt11_em_cm_e2;
			float esyst2_tot = nt11_ee_cm_e2*nt11_ee_cm_e2 + nt11_em_cm_e2*nt11_em_cm_e2;
			float estat2_ee  = nt11_ee_cm_e1*nt11_ee_cm_e1;
			float estat2_em  = nt11_em_cm_e1*nt11_em_cm_e1;
			float estat2_tot = nt11_ee_cm_e1*nt11_ee_cm_e1 + nt11_em_cm_e1*nt11_em_cm_e1;

			float prev    = totbg   ->GetBinError(i+1);
			float prev_em = totbg_em->GetBinError(i+1);
			float prev_ee = totbg_ee->GetBinError(i+1);

			totbg   ->SetBinError(i+1, prev    + esyst2_tot + estat2_tot);
			totbg_em->SetBinError(i+1, prev_em + esyst2_em  + estat2_em);
			totbg_ee->SetBinError(i+1, prev_ee + esyst2_ee  + estat2_ee);
			delete FR;
		}

		nt11_cm->Add(nt11_ee_cm);
		nt11_cm->Add(nt11_em_cm);

		totbg   ->Add(nt11_cm);
		totbg_em->Add(nt11_em_cm);
		totbg_ee->Add(nt11_ee_cm);

		///////////////////////////////////////////////////////////////////////////////////
		// OUTPUT /////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		nt11->SetMarkerColor(kBlack);
		nt11->SetMarkerStyle(20);
		nt11->SetMarkerSize(2.0);
		nt11->SetLineWidth(2);
		nt11->SetLineColor(kBlack);
		nt11->SetFillColor(kBlack);
		nt11_mm->SetMarkerColor(kBlack);
		nt11_mm->SetMarkerStyle(20);
		nt11_mm->SetMarkerSize(2.0);
		nt11_mm->SetLineWidth(2);
		nt11_mm->SetLineColor(kBlack);
		nt11_mm->SetFillColor(kBlack);
		nt11_ee->SetMarkerColor(kBlack);
		nt11_ee->SetMarkerStyle(20);
		nt11_ee->SetMarkerSize(2.0);
		nt11_ee->SetLineWidth(2);
		nt11_ee->SetLineColor(kBlack);
		nt11_ee->SetFillColor(kBlack);
		nt11_em->SetMarkerColor(kBlack);
		nt11_em->SetMarkerStyle(20);
		nt11_em->SetMarkerSize(2.0);
		nt11_em->SetLineWidth(2);
		nt11_em->SetLineColor(kBlack);
		nt11_em->SetFillColor(kBlack);

		nt11_sf->SetLineWidth(1);
		nt11_df->SetLineWidth(1);
		nt11_ss->SetLineWidth(1);
		nt11_sf->SetLineColor(50);
		nt11_sf->SetFillColor(50);
		nt11_df->SetLineColor(38);
		nt11_df->SetFillColor(38);
		nt11_cm->SetLineColor(42);
		nt11_cm->SetFillColor(42);
		nt11_ss->SetLineColor(31);
		nt11_ss->SetFillColor(31);

		nt11_mm_sf->SetLineWidth(1);
		nt11_mm_df->SetLineWidth(1);
		nt11_mm_ss->SetLineWidth(1);
		nt11_mm_sf->SetLineColor(50);
		nt11_mm_sf->SetFillColor(50);
		nt11_mm_df->SetLineColor(38);
		nt11_mm_df->SetFillColor(38);
		nt11_mm_ss->SetLineColor(31);
		nt11_mm_ss->SetFillColor(31);

		nt11_ee_sf->SetLineWidth(1);
		nt11_ee_df->SetLineWidth(1);
		nt11_ee_ss->SetLineWidth(1);
		nt11_ee_sf->SetLineColor(50);
		nt11_ee_sf->SetFillColor(50);
		nt11_ee_df->SetLineColor(38);
		nt11_ee_df->SetFillColor(38);
		nt11_ee_cm->SetLineColor(42);
		nt11_ee_cm->SetFillColor(42);
		nt11_ee_ss->SetLineColor(31);
		nt11_ee_ss->SetFillColor(31);

		nt11_em_sf->SetLineWidth(1);
		nt11_em_df->SetLineWidth(1);
		nt11_em_ss->SetLineWidth(1);
		nt11_em_sf->SetLineColor(50);
		nt11_em_sf->SetFillColor(50);
		nt11_em_df->SetLineColor(38);
		nt11_em_df->SetFillColor(38);
		nt11_em_cm->SetLineColor(42);
		nt11_em_cm->SetFillColor(42);
		nt11_em_ss->SetLineColor(31);
		nt11_em_ss->SetFillColor(31);

		totbg   ->SetLineWidth(1);
		totbg_mm->SetLineWidth(1);
		totbg_em->SetLineWidth(1);
		totbg_ee->SetLineWidth(1);

		totbg   ->SetFillColor(kBlack);
		totbg_mm->SetFillColor(kBlack);
		totbg_em->SetFillColor(kBlack);
		totbg_ee->SetFillColor(kBlack);
		totbg   ->SetFillStyle(3013);
		totbg_mm->SetFillStyle(3013);
		totbg_em->SetFillStyle(3013);
		totbg_ee->SetFillStyle(3013);

		TH1D *nt11_pred_tot    = new TH1D(Form("NT11_PRED_%s",    varname.Data()), varname, nbins, bins); nt11_pred_tot->Sumw2();
		TH1D *nt11_mm_pred_tot = new TH1D(Form("NT11_MM_PRED_%s", varname.Data()), varname, nbins, bins); nt11_mm_pred_tot->Sumw2();
		TH1D *nt11_ee_pred_tot = new TH1D(Form("NT11_EE_PRED_%s", varname.Data()), varname, nbins, bins); nt11_ee_pred_tot->Sumw2();
		TH1D *nt11_em_pred_tot = new TH1D(Form("NT11_EM_PRED_%s", varname.Data()), varname, nbins, bins); nt11_em_pred_tot->Sumw2();

		THStack *nt11_tot    = new THStack("NT11_TotalBG", "NT11_TotalBG");
		THStack *nt11_mm_tot = new THStack("NT11_MM_TotalBG", "NT11_MM_TotalBG");
		THStack *nt11_ee_tot = new THStack("NT11_EE_TotalBG", "NT11_EE_TotalBG");
		THStack *nt11_em_tot = new THStack("NT11_EM_TotalBG", "NT11_EM_TotalBG");

		nt11_tot->Add(nt11_sf);
		nt11_tot->Add(nt11_df);
		nt11_tot->Add(nt11_ss);
		nt11_tot->Add(nt11_cm);

		nt11_mm_tot->Add(nt11_mm_sf);
		nt11_mm_tot->Add(nt11_mm_df);
		nt11_mm_tot->Add(nt11_mm_ss);

		nt11_ee_tot->Add(nt11_ee_sf);
		nt11_ee_tot->Add(nt11_ee_df);
		nt11_ee_tot->Add(nt11_ee_ss);
		nt11_ee_tot->Add(nt11_ee_cm);

		nt11_em_tot->Add(nt11_em_sf);
		nt11_em_tot->Add(nt11_em_df);
		nt11_em_tot->Add(nt11_em_ss);
		nt11_em_tot->Add(nt11_em_cm);

		nt11_tot->Draw("goff");
		nt11_tot->GetXaxis()->SetTitle(DiffPredYields::axis_label[j]);
		nt11_mm_tot->Draw("goff");
		nt11_mm_tot->GetXaxis()->SetTitle(DiffPredYields::axis_label[j]);
		nt11_ee_tot->Draw("goff");
		nt11_ee_tot->GetXaxis()->SetTitle(DiffPredYields::axis_label[j]);
		nt11_em_tot->Draw("goff");
		nt11_em_tot->GetXaxis()->SetTitle(DiffPredYields::axis_label[j]);

		nt11_tot   ->SetMinimum(0.5*nt11   ->GetMinimum());
		nt11_mm_tot->SetMinimum(0.5*nt11_mm->GetMinimum());
		nt11_ee_tot->SetMinimum(0.5*nt11_ee->GetMinimum());
		nt11_em_tot->SetMinimum(0.5*nt11_em->GetMinimum());

		double max = nt11->Integral();
		nt11    ->SetMaximum(max>1?max+1:1.);
		nt11_sf ->SetMaximum(max>1?max+1:1.);
		nt11_df ->SetMaximum(max>1?max+1:1.);
		nt11_cm ->SetMaximum(max>1?max+1:1.);
		nt11_ss ->SetMaximum(max>1?max+1:1.);
		nt11_tot->SetMaximum(max>1?max+1:1.);

		double max_mm = nt11_mm->Integral();
		nt11_mm    ->SetMaximum(max_mm>1?max_mm+1:1.);
		nt11_mm_sf ->SetMaximum(max_mm>1?max_mm+1:1.);
		nt11_mm_df ->SetMaximum(max_mm>1?max_mm+1:1.);
		nt11_mm_ss ->SetMaximum(max_mm>1?max_mm+1:1.);
		nt11_mm_tot->SetMaximum(max_mm>1?max_mm+1:1.);

		double max_ee = nt11_ee->Integral();
		nt11_ee    ->SetMaximum(max_ee>1?max_ee+1:1.);
		nt11_ee_sf ->SetMaximum(max_ee>1?max_ee+1:1.);
		nt11_ee_df ->SetMaximum(max_ee>1?max_ee+1:1.);
		nt11_ee_cm ->SetMaximum(max_ee>1?max_ee+1:1.);
		nt11_ee_ss ->SetMaximum(max_ee>1?max_ee+1:1.);
		nt11_ee_tot->SetMaximum(max_ee>1?max_ee+1:1.);

		double max_em = nt11_em->Integral();
		nt11_em    ->SetMaximum(max_em>1?max_em+1:1.);
		nt11_em_sf ->SetMaximum(max_em>1?max_em+1:1.);
		nt11_em_df ->SetMaximum(max_em>1?max_em+1:1.);
		nt11_em_cm ->SetMaximum(max_em>1?max_em+1:1.);
		nt11_em_ss ->SetMaximum(max_em>1?max_em+1:1.);
		nt11_em_tot->SetMaximum(max_em>1?max_em+1:1.);

		// Take square root of sum of squared errors:
		for(size_t i = 0; i < nbins; ++i){
			float prev    = totbg   ->GetBinError(i+1);
			float prev_mm = totbg_mm->GetBinError(i+1);
			float prev_em = totbg_em->GetBinError(i+1);
			float prev_ee = totbg_ee->GetBinError(i+1);

			totbg   ->SetBinError(i+1, sqrt(prev)   );
			totbg_mm->SetBinError(i+1, sqrt(prev_mm));
			totbg_em->SetBinError(i+1, sqrt(prev_em));
			totbg_ee->SetBinError(i+1, sqrt(prev_ee));
		}


		fOutputSubDir = "DiffPredictionPlots/";
		/////////////////////////////////////////////////////////////////
		TLegend *leg = new TLegend(0.60,0.67,0.90,0.88);
		leg->AddEntry(nt11,    "Observed","p");
		leg->AddEntry(nt11_sf, "Single Fakes","f");
		leg->AddEntry(nt11_df, "Double Fakes","f");
		leg->AddEntry(nt11_ss, "Irreducible (MC)","f");
		leg->AddEntry(nt11_cm, "Charge MisID","f");
		leg->AddEntry(totbg,   "Total Uncertainty","f");
		leg->SetFillStyle(0);
		leg->SetTextFont(42);
		leg->SetBorderSize(0);

		TCanvas *c_temp = new TCanvas("C_ObsPred_" + varname, "Observed vs Predicted", 0, 0, 800, 600);
		c_temp->cd();
		gPad->SetLogy();

		nt11_tot->Draw("hist");
		// nt11_error->DrawCopy("X0 E1 same");
		nt11->DrawCopy("PE X0 same");
		totbg->DrawCopy("0 E2 same");
		leg->Draw();
		lat->SetTextSize(0.04);
		lat->DrawLatex(0.55,0.92, "#mu#mu/ee/e#mu");
		lat->SetTextSize(0.03);
		if(j==0||j==2) lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV");
		if(j==1)       lat->DrawLatex(0.13,0.85, "H_{T} > 80 GeV, N_{Jets} > 1");
		if(j>2)        lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV, H_{T} > 80 GeV, N_{Jets} #geq 2");
		drawTopLine();

		gPad->RedrawAxis();
		Util::PrintPDF(c_temp, "ObsPred_" + varname, fOutputDir + fOutputSubDir);

		fOutputSubDir = "DiffPredictionPlots/IndividualChannels/";
		/////////////////////////////////////////////////////////////////
		TLegend *leg_mm = new TLegend(0.60,0.67,0.90,0.88);
		leg_mm->AddEntry(nt11_mm,    "Observed","p");
		leg_mm->AddEntry(nt11_mm_sf, "Single Fakes","f");
		leg_mm->AddEntry(nt11_mm_df, "Double Fakes","f");
		leg_mm->AddEntry(nt11_mm_ss, "Irreducible (MC)","f");
		leg_mm->AddEntry(totbg_mm,   "Total Uncertainty","f");
		leg_mm->SetFillStyle(0);
		leg_mm->SetTextFont(42);
		leg_mm->SetBorderSize(0);

		c_temp = new TCanvas("C_ObsPred_MM_" + varname, "Observed vs Predicted", 0, 0, 800, 600);
		c_temp->cd();
		gPad->SetLogy();

		nt11_mm_tot->Draw("hist");
		// nt11_error->DrawCopy("X0 E1 same");
		nt11_mm->DrawCopy("PE X0 same");
		totbg_mm->DrawCopy("0 E2 same");
		leg_mm->Draw();
		lat->SetTextSize(0.04);
		lat->DrawLatex(0.65,0.92, "#mu#mu");
		lat->SetTextSize(0.03);
		if(j==0||j==2) lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV");
		if(j==1)       lat->DrawLatex(0.13,0.85, "H_{T} > 80 GeV, N_{Jets} > 1");
		if(j>2)        lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV, H_{T} > 80 GeV, N_{Jets} #geq 2");
		drawTopLine();

		gPad->RedrawAxis();
		Util::PrintPDF(c_temp, varname + "_MM_ObsPred", fOutputDir + fOutputSubDir);

		/////////////////////////////////////////////////////////////////
		TLegend *leg_ee = new TLegend(0.60,0.67,0.90,0.88);
		leg_ee->AddEntry(nt11_ee,    "Observed","p");
		leg_ee->AddEntry(nt11_ee_sf, "Single Fakes","f");
		leg_ee->AddEntry(nt11_ee_df, "Double Fakes","f");
		leg_ee->AddEntry(nt11_ee_ss, "Irreducible (MC)","f");
		leg_ee->AddEntry(nt11_ee_cm, "Charge MisID","f");
		leg_ee->AddEntry(totbg_ee,   "Total Uncertainty","f");
		leg_ee->SetFillStyle(0);
		leg_ee->SetTextFont(42);
		leg_ee->SetBorderSize(0);

		c_temp = new TCanvas("C_ObsPred_EE_" + varname, "Observed vs Predicted", 0, 0, 800, 600);
		c_temp->cd();
		gPad->SetLogy();

		nt11_ee_tot->Draw("hist");
		// nt11_error->DrawCopy("X0 E1 same");
		nt11_ee->DrawCopy("PE X0 same");
		totbg_ee->DrawCopy("0 E2 same");
		leg_ee->Draw();
		lat->SetTextSize(0.04);
		lat->DrawLatex(0.65,0.92, "ee");
		lat->SetTextSize(0.03);
		if(j==0||j==2) lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV");
		if(j==1)       lat->DrawLatex(0.13,0.85, "H_{T} > 80 GeV, N_{Jets} > 1");
		if(j>2)        lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV, H_{T} > 80 GeV, N_{Jets} #geq 2");
		drawTopLine();

		gPad->RedrawAxis();
		Util::PrintPDF(c_temp, varname + "_EE_ObsPred", fOutputDir + fOutputSubDir);

		/////////////////////////////////////////////////////////////////
		TLegend *leg_em = new TLegend(0.60,0.67,0.90,0.88);
		leg_em->AddEntry(nt11_em,    "Observed","p");
		leg_em->AddEntry(nt11_em_sf, "Single Fakes","f");
		leg_em->AddEntry(nt11_em_df, "Double Fakes","f");
		leg_em->AddEntry(nt11_em_ss, "Irreducible (MC)","f");
		leg_em->AddEntry(nt11_em_cm, "Charge MisID","f");
		leg_em->AddEntry(totbg_em,   "Total Uncertainty","f");
		leg_em->SetFillStyle(0);
		leg_em->SetTextFont(42);
		leg_em->SetBorderSize(0);

		c_temp = new TCanvas("C_ObsPred_EM_" + varname, "Observed vs Predicted", 0, 0, 800, 600);
		c_temp->cd();
		gPad->SetLogy();

		nt11_em_tot->Draw("hist");
		totbg_em->DrawCopy("0 E2 same");
		nt11_em->DrawCopy("PE X0 same");
		leg_em->Draw();
		lat->SetTextSize(0.04);
		lat->DrawLatex(0.65,0.92, "e#mu");
		lat->SetTextSize(0.03);
		if(j==0||j==2) lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV");
		if(j==1)       lat->DrawLatex(0.13,0.85, "H_{T} > 80 GeV, N_{Jets} > 1");
		if(j>2)        lat->DrawLatex(0.13,0.85, "E_{T}^{miss} > 30 GeV, H_{T} > 80 GeV, N_{Jets} #geq 2");
		drawTopLine();

		gPad->RedrawAxis();
		Util::PrintPDF(c_temp, varname + "_EM_ObsPred", fOutputDir + fOutputSubDir);

		// Cleanup
		delete c_temp, leg, leg_mm, leg_em, leg_ee;
		delete nt11, nt11_mm, nt11_ee, nt11_em;
		delete nt10_mm, nt10_em, nt10_ee, nt01_mm, nt01_em, nt01_ee, nt00_mm, nt00_em, nt00_ee;
		delete nt2_os_ee_bb, nt2_os_ee_eb, nt2_os_ee_ee, nt2_os_em_bb, nt2_os_em_ee;
		delete nt11_ss, nt11_mm_ss, nt11_em_ss, nt11_ee_ss;
		delete nt11_sf, nt11_mm_sf, nt11_em_sf, nt11_ee_sf;
		delete nt11_df, nt11_mm_df, nt11_em_df, nt11_ee_df;
		delete nt11_cm, nt11_em_cm, nt11_ee_cm;
		// delete nt11_mc, nt11_mm_mc, nt11_ee_mc, nt11_em_mc;
		delete nt11_tot, nt11_mm_tot, nt11_ee_tot, nt11_em_tot;
		delete totbg, totbg_mm, totbg_em, totbg_ee;
	}
	fOutputSubDir = "";
}

void MuonPlotter::makeIntMCClosure(TString filename, gHiLoSwitch hilo){
	ofstream OUT(filename.Data(), ios::trunc);

	fLumiNorm = 1000.;
	const int nsamples = fMCBG.size();

	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;
	OUT << " Producing integrated predictions" << endl;
	OUT << "  scaling MC to " << fLumiNorm << " /pb" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mf(0.), mf_e(0.), mp(0.), mp_e(0.), ef(0.), ef_e(0.), ep(0.), ep_e(0.);

	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mf, mf_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mp, mp_e);
	calculateRatio(fMCBG,      Elec, SigSup, ef, ef_e);
	calculateRatio(fMCBG,      Elec, ZDecay, ep, ep_e);

	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	vector<float> ntt_mm, ntl_mm, nll_mm;
	vector<float> ntt_ee, ntl_ee, nll_ee;
	vector<float> ntt_em, ntl_em, nlt_em, nll_em;

	vector<float> npp_mm, npf_mm, nfp_mm, nff_mm;
	vector<float> npp_ee, npf_ee, nfp_ee, nff_ee;
	vector<float> npp_em, npf_em, nfp_em, nff_em;

	vector<float> npp_tt_mm, npf_tt_mm, nfp_tt_mm, nff_tt_mm;
	vector<float> npp_tt_ee, npf_tt_ee, nfp_tt_ee, nff_tt_ee;
	vector<float> npp_tt_em, npf_tt_em, nfp_tt_em, nff_tt_em;

	// OS yields
	vector<float> ntt_os_BB_ee, ntt_os_EE_ee, ntt_os_EB_ee;
	vector<float> ntt_os_BB_em, ntt_os_EE_em;
	// Charge misid
	vector<float> npp_tt_cm_ee, npp_cm_ee;
	vector<float> npp_tt_cm_em, npp_cm_em;

	vector<float> scales;
	vector<TString> names;
	for(size_t i = 0; i < fMCBG.size(); ++i){
		Sample *S = fSamples[fMCBG[i]];
		float scale = fLumiNorm / S->lumi;
		names.push_back(S->sname);
		scales.push_back(scale);
		ntt_mm.push_back(S->numbers[Baseline][Muon].nt2);
		ntl_mm.push_back(S->numbers[Baseline][Muon].nt10);
		nll_mm.push_back(S->numbers[Baseline][Muon].nt0);

		ntt_em.push_back(S->numbers[Baseline][ElMu].nt2);
		ntl_em.push_back(S->numbers[Baseline][ElMu].nt10);
		nlt_em.push_back(S->numbers[Baseline][ElMu].nt01);
		nll_em.push_back(S->numbers[Baseline][ElMu].nt0);

		ntt_ee.push_back(S->numbers[Baseline][Elec].nt2);
		ntl_ee.push_back(S->numbers[Baseline][Elec].nt10);
		nll_ee.push_back(S->numbers[Baseline][Elec].nt0);

		npp_mm.push_back(S->region[Baseline][hilo].mm.npp_pt->GetEntries());
		npf_mm.push_back(S->region[Baseline][hilo].mm.npf_pt->GetEntries());
		nfp_mm.push_back(S->region[Baseline][hilo].mm.nfp_pt->GetEntries());
		nff_mm.push_back(S->region[Baseline][hilo].mm.nff_pt->GetEntries());

		npp_em.push_back(S->region[Baseline][hilo].em.npp_pt->GetEntries());
		npf_em.push_back(S->region[Baseline][hilo].em.npf_pt->GetEntries());
		nfp_em.push_back(S->region[Baseline][hilo].em.nfp_pt->GetEntries());
		nff_em.push_back(S->region[Baseline][hilo].em.nff_pt->GetEntries());

		npp_ee.push_back(S->region[Baseline][hilo].ee.npp_pt->GetEntries());
		npf_ee.push_back(S->region[Baseline][hilo].ee.npf_pt->GetEntries());
		nfp_ee.push_back(S->region[Baseline][hilo].ee.nfp_pt->GetEntries());
		nff_ee.push_back(S->region[Baseline][hilo].ee.nff_pt->GetEntries());

		npp_tt_mm.push_back(S->region[Baseline][hilo].mm.nt2pp_pt->GetEntries());
		npf_tt_mm.push_back(S->region[Baseline][hilo].mm.nt2pf_pt->GetEntries());
		nfp_tt_mm.push_back(S->region[Baseline][hilo].mm.nt2fp_pt->GetEntries());
		nff_tt_mm.push_back(S->region[Baseline][hilo].mm.nt2ff_pt->GetEntries());

		npp_tt_em.push_back(S->region[Baseline][hilo].em.nt2pp_pt->GetEntries());
		npf_tt_em.push_back(S->region[Baseline][hilo].em.nt2pf_pt->GetEntries());
		nfp_tt_em.push_back(S->region[Baseline][hilo].em.nt2fp_pt->GetEntries());
		nff_tt_em.push_back(S->region[Baseline][hilo].em.nt2ff_pt->GetEntries());

		npp_tt_ee.push_back(S->region[Baseline][hilo].ee.nt2pp_pt->GetEntries());
		npf_tt_ee.push_back(S->region[Baseline][hilo].ee.nt2pf_pt->GetEntries());
		nfp_tt_ee.push_back(S->region[Baseline][hilo].ee.nt2fp_pt->GetEntries());
		nff_tt_ee.push_back(S->region[Baseline][hilo].ee.nt2ff_pt->GetEntries());

		ntt_os_BB_em.push_back(S->region[Baseline][hilo].em.nt20_OS_BB_pt->GetEntries()); // ele in barrel
		ntt_os_EE_em.push_back(S->region[Baseline][hilo].em.nt20_OS_EE_pt->GetEntries()); // ele in endcal
		ntt_os_BB_ee.push_back(S->region[Baseline][hilo].ee.nt20_OS_BB_pt->GetEntries()); // both in barrel
		ntt_os_EE_ee.push_back(S->region[Baseline][hilo].ee.nt20_OS_EE_pt->GetEntries()); // both in endcal
		ntt_os_EB_ee.push_back(S->region[Baseline][hilo].ee.nt20_OS_EB_pt->GetEntries()); // one barrel, one endcap

		npp_tt_cm_ee.push_back(scale*S->region[Baseline][hilo].ee.nt2pp_cm_pt->GetEntries());
		npp_cm_ee   .push_back(scale*S->region[Baseline][hilo].ee.npp_cm_pt->GetEntries());
		npp_tt_cm_em.push_back(scale*S->region[Baseline][hilo].em.nt2pp_cm_pt->GetEntries());
		npp_cm_em   .push_back(scale*S->region[Baseline][hilo].em.npp_cm_pt->GetEntries());
	}

	///////////////////////////////////////////////////////////////////////////////////
	// PREDICTIONS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	FakeRatios *FR = new FakeRatios();
	FR->setNToyMCs(100);
	FR->setAddESyst(0.0);

	///////////////////////////////////////////////////////////////////////////////////
	// Fiddle with ratios by hand...
	// mf = 0.03;
	// ef = 0.05;
	// mp = 0.95;
	// ep = 0.80;
	///////////////////////////////////////////////////////////////////////////////////

	FR->setMFRatio(mf, 0.05); // set error to pure statistical of ratio
	FR->setEFRatio(ef, 0.05);
	FR->setMPRatio(mp, 0.05);
	FR->setEPRatio(ep, 0.05);

	vector<float> npp_pred_mm,    npf_pred_mm,    nff_pred_mm;
	vector<float> npp_pred_mm_e1, npf_pred_mm_e1, nff_pred_mm_e1;
	vector<float> npp_pred_ee,    npf_pred_ee,    nff_pred_ee;
	vector<float> npp_pred_ee_e1, npf_pred_ee_e1, nff_pred_ee_e1;
	vector<float> npp_pred_em,    npf_pred_em,    nfp_pred_em,    nff_pred_em;
	vector<float> npp_pred_em_e1, npf_pred_em_e1, nfp_pred_em_e1, nff_pred_em_e1;

	vector<float> nF_pred_mm_e1, nF_pred_em_e1, nF_pred_ee_e1; // combined stat errors on fakes

	for(size_t i = 0; i < nsamples; ++i){
		FR->setMMNtl(ntt_mm[i], ntl_mm[i], nll_mm[i]);
		FR->setEENtl(ntt_ee[i], ntl_ee[i], nll_ee[i]);
		FR->setEMNtl(ntt_em[i], ntl_em[i], nlt_em[i], nll_em[i]);

		npp_pred_mm   .push_back(FR->getMMNpp());
		npp_pred_mm_e1.push_back(FR->getMMNppEStat());
		npf_pred_mm   .push_back(FR->getMMNpf());
		npf_pred_mm_e1.push_back(FR->getMMNpfEStat());
		nff_pred_mm   .push_back(FR->getMMNff());
		nff_pred_mm_e1.push_back(FR->getMMNffEStat());
		nF_pred_mm_e1 .push_back(FR->getMMTotEStat());

		npp_pred_ee   .push_back(FR->getEENpp());
		npp_pred_ee_e1.push_back(FR->getEENppEStat());
		npf_pred_ee   .push_back(FR->getEENpf());
		npf_pred_ee_e1.push_back(FR->getEENpfEStat());
		nff_pred_ee   .push_back(FR->getEENff());
		nff_pred_ee_e1.push_back(FR->getEENffEStat());
		nF_pred_ee_e1 .push_back(FR->getEETotEStat());

		npp_pred_em   .push_back(FR->getEMNpp());
		npp_pred_em_e1.push_back(FR->getEMNppEStat());
		npf_pred_em   .push_back(FR->getEMNpf());
		npf_pred_em_e1.push_back(FR->getEMNpfEStat());
		nfp_pred_em   .push_back(FR->getEMNfp());
		nfp_pred_em_e1.push_back(FR->getEMNfpEStat());
		nff_pred_em   .push_back(FR->getEMNff());
		nff_pred_em_e1.push_back(FR->getEMNffEStat());
		nF_pred_em_e1 .push_back(FR->getEMTotEStat());
	}

	// Charge MisID Predictions
	// Abbreviations
	float fb  = gEChMisIDB;
	float fbE = gEChMisIDB_E;
	float fe  = gEChMisIDE;
	float feE = gEChMisIDE_E;

	vector<float> ntt_cm_ee, ntt_cm_em;
	for(size_t i = 0; i < nsamples; ++i){
		ntt_cm_ee.push_back(2*fb*ntt_os_BB_ee[i] + 2*fe*ntt_os_EE_ee[i] + (fb+fe)*ntt_os_EB_ee[i]);
		ntt_cm_em.push_back(  fb*ntt_os_BB_em[i] +   fe*ntt_os_EE_em[i]);
	}

	// Sums
	float ntt_sum_mm(0.), ntl_sum_mm(0.), nll_sum_mm(0.);
	float ntt_sum_em(0.), ntl_sum_em(0.), nlt_sum_em(0.), nll_sum_em(0.);
	float ntt_sum_ee(0.), ntl_sum_ee(0.), nll_sum_ee(0.);

	float npp_sum_mm(0.), npf_sum_mm(0.), nff_sum_mm(0.);
	float npp_sum_em(0.), npf_sum_em(0.), nfp_sum_em(0.), nff_sum_em(0.);
	float npp_sum_ee(0.), npf_sum_ee(0.), nff_sum_ee(0.);

	float npp_pred_sum_mm(0.), npf_pred_sum_mm(0.), nff_pred_sum_mm(0.);
	float npp_pred_sum_em(0.), npf_pred_sum_em(0.), nfp_pred_sum_em(0.), nff_pred_sum_em(0.);
	float npp_pred_sum_ee(0.), npf_pred_sum_ee(0.), nff_pred_sum_ee(0.);

	float ntt_cm_sum_ee(0.), ntt_cm_sum_em(0.);

	float npp_cm_sum_ee(0.),    npp_cm_sum_em(0.);
	float npp_tt_cm_sum_ee(0.), npp_tt_cm_sum_em(0.);

	float ntt_diboson_mm(0.), ntt_diboson_em(0.), ntt_diboson_ee(0.);
	ntt_diboson_mm += fLumiNorm/fSamples[WW]->lumi*fSamples[WW]->numbers[Baseline][Muon].nt2;
	ntt_diboson_mm += fLumiNorm/fSamples[WZ]->lumi*fSamples[WZ]->numbers[Baseline][Muon].nt2;
	ntt_diboson_mm += fLumiNorm/fSamples[ZZ]->lumi*fSamples[ZZ]->numbers[Baseline][Muon].nt2;
	ntt_diboson_em += fLumiNorm/fSamples[WW]->lumi*fSamples[WW]->numbers[Baseline][ElMu].nt2;
	ntt_diboson_em += fLumiNorm/fSamples[WZ]->lumi*fSamples[WZ]->numbers[Baseline][ElMu].nt2;
	ntt_diboson_em += fLumiNorm/fSamples[ZZ]->lumi*fSamples[ZZ]->numbers[Baseline][ElMu].nt2;
	ntt_diboson_ee += fLumiNorm/fSamples[WW]->lumi*fSamples[WW]->numbers[Baseline][Elec].nt2;
	ntt_diboson_ee += fLumiNorm/fSamples[WZ]->lumi*fSamples[WZ]->numbers[Baseline][Elec].nt2;
	ntt_diboson_ee += fLumiNorm/fSamples[ZZ]->lumi*fSamples[ZZ]->numbers[Baseline][Elec].nt2;

	// Squared errors
	float npp_pred_sum_mm_e1(0.), npf_pred_sum_mm_e1(0.), nff_pred_sum_mm_e1(0.);
	float npp_pred_sum_em_e1(0.), npf_pred_sum_em_e1(0.), nfp_pred_sum_em_e1(0.), nff_pred_sum_em_e1(0.);
	float npp_pred_sum_ee_e1(0.), npf_pred_sum_ee_e1(0.), nff_pred_sum_ee_e1(0.);

	// Combined stat. errors
	float nF_pred_sum_mm_e1(0.), nF_pred_sum_em_e1(0.), nF_pred_sum_ee_e1(0.);

	for(size_t i = 0; i < nsamples; ++i){
		ntt_sum_mm         += scales[i] * ntt_mm[i];
		ntl_sum_mm         += scales[i] * ntl_mm[i];
		nll_sum_mm         += scales[i] * nll_mm[i];
		npp_sum_mm         += scales[i] * npp_mm[i];
		npf_sum_mm         += scales[i] * (npf_mm[i]+nfp_mm[i]);
		nff_sum_mm         += scales[i] * nff_mm[i];
		npp_pred_sum_mm    += scales[i] * npp_pred_mm[i];
		npf_pred_sum_mm    += scales[i] * npf_pred_mm[i];
		nff_pred_sum_mm    += scales[i] * nff_pred_mm[i];
		npp_pred_sum_mm_e1 += scales[i]*scales[i] * npp_pred_mm_e1[i]*npp_pred_mm_e1[i];
		npf_pred_sum_mm_e1 += scales[i]*scales[i] * npf_pred_mm_e1[i]*npf_pred_mm_e1[i];
		nff_pred_sum_mm_e1 += scales[i]*scales[i] * nff_pred_mm_e1[i]*nff_pred_mm_e1[i];
		nF_pred_sum_mm_e1  += scales[i]*scales[i] * nF_pred_mm_e1[i]*nF_pred_mm_e1[i];

		ntt_sum_ee         += scales[i] * ntt_ee[i];
		ntl_sum_ee         += scales[i] * ntl_ee[i];
		nll_sum_ee         += scales[i] * nll_ee[i];
		npp_sum_ee         += scales[i] * npp_ee[i];
		npf_sum_ee         += scales[i] * (npf_ee[i]+nfp_ee[i]);
		nff_sum_ee         += scales[i] * nff_ee[i];
		npp_pred_sum_ee    += scales[i] * npp_pred_ee[i];
		npf_pred_sum_ee    += scales[i] * npf_pred_ee[i];
		nff_pred_sum_ee    += scales[i] * nff_pred_ee[i];
		npp_pred_sum_ee_e1 += scales[i]*scales[i] * npp_pred_ee_e1[i]*npp_pred_ee_e1[i];
		npf_pred_sum_ee_e1 += scales[i]*scales[i] * npf_pred_ee_e1[i]*npf_pred_ee_e1[i];
		nff_pred_sum_ee_e1 += scales[i]*scales[i] * nff_pred_ee_e1[i]*nff_pred_ee_e1[i];
		nF_pred_sum_ee_e1  += scales[i]*scales[i] * nF_pred_ee_e1[i]*nF_pred_ee_e1[i];

		ntt_sum_em         += scales[i] * ntt_em[i];
		ntl_sum_em         += scales[i] * ntl_em[i];
		nlt_sum_em         += scales[i] * nlt_em[i];
		nll_sum_em         += scales[i] * nll_em[i];
		npp_sum_em         += scales[i] * npp_em[i];
		npf_sum_em         += scales[i] * npf_em[i];
		nfp_sum_em         += scales[i] * nfp_em[i];
		nff_sum_em         += scales[i] * nff_em[i];
		npp_pred_sum_em    += scales[i] * npp_pred_em[i];
		npf_pred_sum_em    += scales[i] * npf_pred_em[i];
		nfp_pred_sum_em    += scales[i] * nfp_pred_em[i];
		nff_pred_sum_em    += scales[i] * nff_pred_em[i];
		npp_pred_sum_em_e1 += scales[i]*scales[i] * npp_pred_em_e1[i]*npp_pred_em_e1[i];
		npf_pred_sum_em_e1 += scales[i]*scales[i] * npf_pred_em_e1[i]*npf_pred_em_e1[i];
		nfp_pred_sum_em_e1 += scales[i]*scales[i] * nfp_pred_em_e1[i]*nfp_pred_em_e1[i];
		nff_pred_sum_em_e1 += scales[i]*scales[i] * nff_pred_em_e1[i]*nff_pred_em_e1[i];
		nF_pred_sum_em_e1  += scales[i]*scales[i] * nF_pred_em_e1[i]*nF_pred_em_e1[i];

		ntt_cm_sum_ee   += scales[i] * ntt_cm_ee[i];
		ntt_cm_sum_em   += scales[i] * ntt_cm_em[i];

		npp_cm_sum_ee    += scales[i] * npp_cm_ee[i];
		npp_cm_sum_em    += scales[i] * npp_cm_em[i];
		npp_tt_cm_sum_ee += scales[i] * npp_tt_cm_ee[i];
		npp_tt_cm_sum_em += scales[i] * npp_tt_cm_em[i];
	}


	///////////////////////////////////////////////////////////////////////////////////
	// PRINTOUT ///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	OUT << "-------------------------------------------------------------------------------------------------" << endl;
	OUT << "         RATIOS  ||    Mu-fRatio     |    Mu-pRatio     ||    El-fRatio     |    El-pRatio     ||" << endl;
	OUT << "-------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "           MC    ||";
	OUT << setw(7)  << setprecision(2) << mf << " ± " << setw(7) << setprecision(2) << mf_e << " |";
	OUT << setw(7)  << setprecision(2) << mp << " ± " << setw(7) << setprecision(2) << mp_e << " ||";
	OUT << setw(7)  << setprecision(2) << ef << " ± " << setw(7) << setprecision(2) << ef_e << " |";
	OUT << setw(7)  << setprecision(2) << ep << " ± " << setw(7) << setprecision(2) << ep_e << " ||";
	OUT << endl;
	OUT << "-------------------------------------------------------------------------------------------------" << endl << endl;

	OUT << "==========================================================================================================================" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "          YIELDS ||   Nt2   |   Nt1   |   Nt0   ||   Nt2   |   Nt10  |   Nt01  |   Nt0   ||   Nt2   |   Nt1   |   Nt0   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << names[i] << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntt_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntl_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nll_mm[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntt_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntl_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nlt_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nll_em[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntt_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*ntl_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nll_ee[i]) << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Sum"  << " || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", ntl_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", nll_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", ntl_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nlt_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nll_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", ntl_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", nll_sum_ee) << " || ";
	OUT << endl;
	OUT << setw(16) << "Channels sum"  << " || ";
	OUT << Form("                    %6.3f ||                               %6.3f ||                     %6.3f || ",
		ntt_sum_mm+ntl_sum_mm+nll_sum_mm, ntt_sum_em+ntl_sum_em+nlt_sum_em+nll_sum_em, ntt_sum_ee+ntl_sum_ee+nll_sum_ee) << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "           TRUTH ||   Npp   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nff   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << names[i] << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*(npf_mm[i]+nfp_mm[i])) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_mm[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npf_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nfp_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_em[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npf_ee[i]+nfp_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_ee[i]) << " || ";
		OUT << endl;
	}
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Sum"  << " || ";
	OUT << setw(7) << Form("%6.3f", npp_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nfp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_sum_ee) << " || ";
	OUT << endl;
	OUT << setw(16) << "Channels sum"  << " || ";
	OUT << Form("                    %6.3f ||                               %6.3f ||                     %6.3f || ",
		npp_sum_mm+npf_sum_mm+nff_sum_mm, npp_sum_em+npf_sum_em+nfp_sum_em+nff_sum_em, npp_sum_ee+npf_sum_ee+nff_sum_ee) << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << endl;

	OUT << "==========================================================================================================================" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "     TRUTH IN TT ||   Npp   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nff   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << names[i] << " || ";
		OUT << setw(7)  << Form("%6.3f", mp*mp*scales[i]*npp_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", mp*mf*scales[i]*(npf_mm[i]+nfp_mm[i])) << " | ";
		OUT << setw(7)  << Form("%6.3f", mf*mf*scales[i]*nff_mm[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", mp*ep*scales[i]*npp_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", mp*ef*scales[i]*npf_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", mf*ep*scales[i]*nfp_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", mf*ef*scales[i]*nff_em[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", ep*ep*scales[i]*npp_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", ep*ef*scales[i]*npf_ee[i]+nfp_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", ef*ef*scales[i]*nff_ee[i]) << " || ";
		OUT << endl;
	}
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Npf Sum"  << " || ";
	OUT << setw(7) << Form("%6.3f", mp*mp*npp_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", mp*mf*npf_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*mf*nff_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", mp*ep*npp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mp*ef*npf_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*ep*nfp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*ef*nff_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", ep*ep*npp_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", ep*ef*npf_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", ef*ef*nff_sum_ee) << " || ";
	OUT << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << endl;

	OUT << "==========================================================================================================================" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "     PRED. IN TT ||   Npp   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nff   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << names[i] << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_pred_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npf_pred_mm[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_pred_mm[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_pred_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npf_pred_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nfp_pred_em[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_pred_em[i]) << " || ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npp_pred_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*npf_pred_ee[i]) << " | ";
		OUT << setw(7)  << Form("%6.3f", scales[i]*nff_pred_ee[i]) << " || ";
		OUT << endl;
	}
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Pred Sum"  << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nfp_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_ee) << " || ";
	OUT << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << endl;

	OUT << "=========================================================" << endl;
	OUT << "                 ||       E/Mu      ||       E/E       ||" << endl;
	OUT << "    CHARGE MISID ||  Pred  |  Truth ||  Pred  |  Truth ||" << endl;
	OUT << "---------------------------------------------------------" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << names[i] << " || ";
		OUT << setw(7)  << Form("%6.3f | %6.3f || ", scales[i]*ntt_cm_em[i], scales[i]*npp_tt_cm_em[i]);
		OUT << setw(7)  << Form("%6.3f | %6.3f || ", scales[i]*ntt_cm_ee[i], scales[i]*npp_tt_cm_ee[i]);
		OUT << endl;
	}	
	OUT << "---------------------------------------------------------" << endl;
	OUT << setw(16) << "Sum"  << " || ";
	OUT << setw(7)  << Form("%6.3f | %6.3f || ", ntt_cm_sum_em, npp_tt_cm_sum_em);
	OUT << setw(7)  << Form("%6.3f | %6.3f || ", ntt_cm_sum_ee, npp_tt_cm_sum_ee) << endl;
	OUT << "=========================================================" << endl;
	OUT << endl;

	OUT << "==========================================================================================================================" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "     PRED. IN TT ||   Npp   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nff   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Npf Truth"  << " || ";
	OUT << setw(7) << Form("%6.3f", mp*mp*npp_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", mp*mf*npf_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*mf*nff_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", mp*ep*npp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mp*ef*npf_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*ep*nfp_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", mf*ef*nff_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", ep*ep*npp_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", ep*ef*npf_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", ef*ef*nff_sum_ee) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "FR Prediction"  << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_mm) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_mm) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nfp_pred_sum_em) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_em) << " || ";
	OUT << setw(7) << Form("%6.3f", npp_pred_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_ee) << " | ";
	OUT << setw(7) << Form("%6.3f", nff_pred_sum_ee) << " || ";
	OUT << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << setw(16) << "Pred. Fakes"  << " || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_mm+nff_pred_sum_mm) << " |                   || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_em+nfp_pred_sum_em+nff_pred_sum_em) << " |                             || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_ee+npf_pred_sum_ee) << " |                   || ";
	OUT << endl;
	OUT << setw(16) << "Pred. Ch-MID"  << " ||         |                   || ";
	OUT << setw(7) << Form("%6.3f", ntt_cm_sum_em) << " |                             || ";
	OUT << setw(7) << Form("%6.3f", ntt_cm_sum_ee) << " |                   || ";
	OUT << endl;
	OUT << setw(16) << "Pred. DiBoson"  << " || ";
	OUT << setw(7) << Form("%6.3f", ntt_diboson_mm) << " |                   || ";
	OUT << setw(7) << Form("%6.3f", ntt_diboson_em) << " |                             || ";
	OUT << setw(7) << Form("%6.3f", ntt_diboson_ee) << " |                   || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Total BG Pred."  << " || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_mm+nff_pred_sum_mm+ntt_diboson_mm) << " |                   || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_em+nfp_pred_sum_em+nff_pred_sum_em+ntt_cm_sum_em+ntt_diboson_em) << " |                             || ";
	OUT << setw(7) << Form("%6.3f", npf_pred_sum_ee+npf_pred_sum_ee+ntt_cm_sum_ee+ntt_diboson_ee) << " |                   || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Observed"  << " || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_mm) << " |                   || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_em) << " |                             || ";
	OUT << setw(7) << Form("%6.3f", ntt_sum_ee) << " |                   || ";
	OUT << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << endl;
	OUT << "===================================================================================================================================" << endl;
	OUT << "  All predictions (Npp / Npf / (Nfp) / Nff):                                                                                      |" << endl;
	for(size_t i = 0; i < nsamples; ++i){
		OUT << setw(16) << left << names[i];
		OUT << Form("  MM || %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) |                          |",
			npp_pred_mm[i], npp_pred_mm_e1[i], npf_pred_mm[i], npf_pred_mm_e1[i], nff_pred_mm[i], nff_pred_mm_e1[i]) << endl;
		OUT << " scale = " << setw(7) << setprecision(2) << scales[i];
		OUT << Form("  EM || %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) |",
			npp_pred_em[i], npp_pred_em_e1[i], npf_pred_em[i], npf_pred_em_e1[i], nfp_pred_em[i], nfp_pred_em_e1[i], nff_pred_mm[i], nff_pred_em_e1[i]) << endl;
		OUT << Form("                  EE || %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) | %7.3f ± %7.3f (stat) |                          |",
			npp_pred_ee[i], npp_pred_ee_e1[i], npf_pred_ee[i], npf_pred_ee_e1[i], nff_pred_ee[i], nff_pred_ee_e1[i]) << endl;
	}
	OUT << "===================================================================================================================================" << endl;
	OUT << endl;

	OUT << "==========================================================================================================================" << endl;
	OUT << "  PREDICTIONS (in tt window)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " Mu/Mu Channel:" << endl;
	OUT << "  Npp*pp:        " <<  Form("%6.3f", npp_pred_sum_mm) << " ± " << Form("%6.3f", sqrt(npp_pred_sum_mm_e1)) << " (stat)" << endl;
	OUT << "  Nfp*fp:        " <<  Form("%6.3f", npf_pred_sum_mm) << " ± " << Form("%6.3f", sqrt(npf_pred_sum_mm_e1)) << " (stat)" << endl;
	OUT << "  Nff*ff:        " <<  Form("%6.3f", nff_pred_sum_mm) << " ± " << Form("%6.3f", sqrt(nff_pred_sum_mm_e1)) << " (stat)" << endl;
	OUT << "  Total fakes:   " <<  Form("%6.3f", npf_pred_sum_mm+nff_pred_sum_mm) << " ± " << Form("%6.3f", sqrt(nF_pred_sum_mm_e1)) << " (stat)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/Mu Channel:" << endl;
	OUT << "  Npp*pp:        " <<  Form("%6.3f", npp_pred_sum_em) << " ± " << Form("%6.3f", sqrt(npp_pred_sum_em_e1)) << " (stat)" << endl;
	OUT << "  Nfp*fp:        " <<  Form("%6.3f", npf_pred_sum_em) << " ± " << Form("%6.3f", sqrt(npf_pred_sum_em_e1)) << " (stat)" << endl;
	OUT << "  Npf*pf:        " <<  Form("%6.3f", nfp_pred_sum_em) << " ± " << Form("%6.3f", sqrt(nfp_pred_sum_em_e1)) << " (stat)" << endl;
	OUT << "  Nff*ff:        " <<  Form("%6.3f", nff_pred_sum_em) << " ± " << Form("%6.3f", sqrt(nff_pred_sum_em_e1)) << " (stat)" << endl;
	OUT << "  Total fakes:   " <<  Form("%6.3f", npf_pred_sum_em+nfp_pred_sum_em+nff_pred_sum_em) << " ± " << Form("%6.3f", sqrt(nF_pred_sum_em_e1)) << " (stat)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/E Channel:" << endl;
	OUT << "  Npp*pp:        " <<  Form("%6.3f", npp_pred_sum_ee) << " ± " << Form("%6.3f", sqrt(npp_pred_sum_ee_e1)) << " (stat)" << endl;
	OUT << "  Nfp*fp:        " <<  Form("%6.3f", npf_pred_sum_ee) << " ± " << Form("%6.3f", sqrt(npf_pred_sum_ee_e1)) << " (stat)" << endl;
	OUT << "  Nff*ff:        " <<  Form("%6.3f", nff_pred_sum_ee) << " ± " << Form("%6.3f", sqrt(nff_pred_sum_ee_e1)) << " (stat)" << endl;
	OUT << "  Total fakes:   " <<  Form("%6.3f", npf_pred_sum_ee+nff_pred_sum_ee) << " ± " << Form("%6.3f", sqrt(nF_pred_sum_ee_e1)) << " (stat)" << endl;
	OUT << "==========================================================================================================================" << endl;
	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;

	OUT.close();
	delete FR;
}
void MuonPlotter::makeTTbarClosure(){
	TString filename = "TTbarClosure.txt";
	ofstream OUT(fOutputDir + filename.Data(), ios::trunc);

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_allmc(0.), mufratio_allmc_e(0.);
	float mupratio_allmc(0.), mupratio_allmc_e(0.);
	float elfratio_allmc(0.), elfratio_allmc_e(0.);
	float elpratio_allmc(0.), elpratio_allmc_e(0.);

	vector<int> ttjets; ttjets.push_back(TTJets);
	// calculateRatio(ttjets, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	// calculateRatio(ttjets, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	// calculateRatio(ttjets, Elec, SigSup, elfratio_allmc, elfratio_allmc_e);
	// calculateRatio(ttjets, Elec, ZDecay, elpratio_allmc, elpratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	calculateRatio(fMCBG,      Elec, SigSup, elfratio_allmc, elfratio_allmc_e);
	calculateRatio(fMCBG,      Elec, ZDecay, elpratio_allmc, elpratio_allmc_e);

	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt_mm(0.), nl_mm(0.),  np_mm(0.), nf_mm(0.);
	float nt_em(0.), nl_em(0.),  np_em(0.), nf_em(0.);
	float nt_me(0.), nl_me(0.),  np_me(0.), nf_me(0.);
	float nt_ee(0.), nl_ee(0.),  np_ee(0.), nf_ee(0.);

	Sample *S = fSamples[TTJets];
	TTree *tree = S->getTree();
	fCurrentSample = TTJets;

	// Event loop
	tree->ResetBranchAddresses();
	// Init(tree);
	if(S->datamc == 0) Init(tree);
	else InitMC(tree);

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		printProgress(jentry, nentries, S->name);

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		int ind1(-1), ind2(-1);

		// MuMu
		fCurrentChannel = Muon;
		if(isSSLLMuEvent(ind1, ind2)){
			if(isPromptMuon(ind1)){
				if( isTightMuon(ind2)){
					nt_mm++;	
					if( isPromptMuon(ind2)) np_mm++;
					if(!isPromptMuon(ind2)) nf_mm++;
				}
				if(!isTightMuon(ind2))  nl_mm++;
			}
			else if(isPromptMuon(ind2)){
				if( isTightMuon(ind1)){
					nt_mm++;
					if( isPromptMuon(ind1)) np_mm++;
					if(!isPromptMuon(ind1)) nf_mm++;
				}
				if(!isTightMuon(ind1))  nl_mm++;				
			}			
		}


		// EE
		fCurrentChannel = Elec;
		if(isSSLLElEvent(ind1, ind2)){
			if(isPromptElectron(ind1)){
				if( isTightElectron(ind2)){
					nt_ee++;
					if( isPromptElectron(ind2)) np_ee++;
					if(!isPromptElectron(ind2)) nf_ee++;
				}
				if(!isTightElectron(ind2))  nl_ee++;
			}
			else if(isPromptElectron(ind2)){
				if( isTightElectron(ind1)){
					nt_ee++;
					if( isPromptElectron(ind1)) np_ee++;
					if(!isPromptElectron(ind1)) nf_ee++;
				}
				if(!isTightElectron(ind1))  nl_ee++;				
			}			
		}

		// EMu
		fCurrentChannel = ElMu;
		if(isSSLLElMuEvent(ind1, ind2)){
			if(isPromptElectron(ind2)){
				if( isTightMuon(ind1)){
					nt_em++;
					if( isPromptMuon(ind1)) np_em++;
					if(!isPromptMuon(ind1)) nf_em++;
				}
				if(!isTightMuon(ind1))  nl_em++;				
			}			
			else if(isPromptMuon(ind1)){
				if( isTightElectron(ind2)){
					nt_me++;
					if( isPromptElectron(ind2)) np_me++;
					if(!isPromptElectron(ind2)) nf_me++;
				}
				if(!isTightElectron(ind2))  nl_me++;
			}
		}
	}
	cout << endl;
	S->cleanUp();
	// Scale by luminosity:
	float scale = fLumiNorm / fSamples[TTJets]->lumi;
	nt_mm*=scale; nl_mm*=scale; np_mm*=scale; nf_mm*=scale;
	nt_em*=scale; nl_em*=scale; np_em*=scale; nf_em*=scale;
	nt_me*=scale; nl_me*=scale; np_me*=scale; nf_me*=scale;
	nt_ee*=scale; nl_ee*=scale; np_ee*=scale; nf_ee*=scale;	

	///////////////////////////////////////////////////////////////////////////////////
	// PREDICTIONS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	FPRatios *fMMFPRatios = new FPRatios();
	FPRatios *fMEFPRatios = new FPRatios();
	FPRatios *fEMFPRatios = new FPRatios();
	FPRatios *fEEFPRatios = new FPRatios();

	fMMFPRatios->SetVerbose(fVerbose);
	fMEFPRatios->SetVerbose(fVerbose);
	fEMFPRatios->SetVerbose(fVerbose);
	fEEFPRatios->SetVerbose(fVerbose);

	fMMFPRatios->SetMuFratios(mufratio_allmc, mufratio_allmc_e);
	fMMFPRatios->SetMuPratios(mupratio_allmc, mupratio_allmc_e);

	fEEFPRatios->SetElFratios(elfratio_allmc, elfratio_allmc_e);
	fEEFPRatios->SetElPratios(elpratio_allmc, elpratio_allmc_e);

	fEMFPRatios->SetMuFratios(mufratio_allmc, mufratio_allmc_e);
	fEMFPRatios->SetMuPratios(mupratio_allmc, mupratio_allmc_e);
	fMEFPRatios->SetElFratios(elfratio_allmc, elfratio_allmc_e);
	fMEFPRatios->SetElPratios(elpratio_allmc, elpratio_allmc_e);

	vector<double> vpt, veta;
	vpt.push_back(30.); // Fake pts and etas (first electron then muon)
	veta.push_back(0.);


	// MuMu Channel
	vector<double> v_nt_mm;
	v_nt_mm.push_back(nl_mm);
	v_nt_mm.push_back(nt_mm);

	fMMFPRatios->NevtTopol(0, 1, v_nt_mm);

	vector<double> MM_Nev   = fMMFPRatios->NevtPass(vpt, veta);
	vector<double> MM_Estat = fMMFPRatios->NevtPassErrStat();
	vector<double> MM_Esyst = fMMFPRatios->NevtPassErrSyst();

	// EM Channel
	vector<double> v_nt_em;
	v_nt_em.push_back(nl_em);
	v_nt_em.push_back(nt_em);
	fEMFPRatios->NevtTopol(0, 1, v_nt_em);
	vector<double> EM_Nev   = fEMFPRatios->NevtPass(vpt, veta);
	vector<double> EM_Estat = fEMFPRatios->NevtPassErrStat();
	vector<double> EM_Esyst = fEMFPRatios->NevtPassErrSyst();

	// ME Channel
	vector<double> v_nt_me;
	v_nt_me.push_back(nl_me);
	v_nt_me.push_back(nt_me);
	fMEFPRatios->NevtTopol(1, 0, v_nt_me);
	vector<double> ME_Nev   = fMEFPRatios->NevtPass(vpt, veta);
	vector<double> ME_Estat = fMEFPRatios->NevtPassErrStat();
	vector<double> ME_Esyst = fMEFPRatios->NevtPassErrSyst();


	// EE Channel
	vector<double> v_nt_ee;
	v_nt_ee.push_back(nl_ee);
	v_nt_ee.push_back(nt_ee);
	fEEFPRatios->NevtTopol(1, 0, v_nt_ee);
	vector<double> EE_Nev   = fEEFPRatios->NevtPass(vpt, veta);
	vector<double> EE_Estat = fEEFPRatios->NevtPassErrStat();
	vector<double> EE_Esyst = fEEFPRatios->NevtPassErrSyst();

	///////////////////////////////////////////////////////////////////////////////////
	// PRINTOUT ///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	OUT << "-----------------------------------------------------------------------------------------------------" << endl;
	OUT << "     RATIOS  ||     Mu-fRatio      |     Mu-pRatio      ||     El-fRatio      |     El-pRatio      ||" << endl;
	OUT << "-----------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(12) << "    allMC    ||";
	OUT << setw(7)  << setprecision(2) << mufratio_allmc << " +/- " << setw(7) << setprecision(2) << mufratio_allmc_e << " |";
	OUT << setw(7)  << setprecision(2) << mupratio_allmc << " +/- " << setw(7) << setprecision(2) << mupratio_allmc_e << " ||";
	OUT << setw(7)  << setprecision(2) << elfratio_allmc << " +/- " << setw(7) << setprecision(2) << elfratio_allmc_e << " |";
	OUT << setw(7)  << setprecision(2) << elpratio_allmc << " +/- " << setw(7) << setprecision(2) << elpratio_allmc_e << " ||";
	OUT << endl;
	OUT << "-----------------------------------------------------------------------------------------------------" << endl << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << "             ||       Mu/Mu       ||        E/Mu       ||       Mu/E        ||        E/E        ||" << endl;
	OUT << "             ||    Nt   |    Nl   ||    Nt   |   Nl    ||    Nt   |   Nl    ||    Nt   |   Nl    ||" << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(12) << "Observed" << " || ";
	OUT << setw(7)  << numbForm(nt_mm) << " | ";
	OUT << setw(7)  << numbForm(nl_mm) << " || ";
	OUT << setw(7)  << numbForm(nt_em) << " | ";
	OUT << setw(7)  << numbForm(nl_em) << " || ";
	OUT << setw(7)  << numbForm(nt_me) << " | ";
	OUT << setw(7)  << numbForm(nl_me) << " || ";
	OUT << setw(7)  << numbForm(nt_ee) << " | ";
	OUT << setw(7)  << numbForm(nl_ee) << " || ";
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(12) << "Truth ratio" << " || ";
	OUT << setw(17)  << numbForm(nt_mm/nl_mm) << " || ";
	OUT << setw(17)  << numbForm(nt_em/nl_em) << " || ";
	OUT << setw(17)  << numbForm(nt_me/nl_me) << " || ";
	OUT << setw(17)  << numbForm(nt_ee/nl_ee) << " || ";
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << "             ||    Np   |    Nf   ||    Np   |    Nf   ||    Np   |    Nf   ||    Np   |    Nf   ||" << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(12) << "MC Truth" << " || ";
	OUT << setw(7)  << numbForm(np_mm) << " | ";
	OUT << setw(7)  << numbForm(nf_mm) << " || ";
	OUT << setw(7)  << numbForm(np_em) << " | ";
	OUT << setw(7)  << numbForm(nf_em) << " || ";
	OUT << setw(7)  << numbForm(np_me) << " | ";
	OUT << setw(7)  << numbForm(nf_me) << " || ";
	OUT << setw(7)  << numbForm(np_ee) << " | ";
	OUT << setw(7)  << numbForm(nf_ee) << " || ";
	OUT << endl;
	OUT << setw(12) << "Predicted" << " || ";
	OUT << setw(7)  << numbForm(MM_Nev[0]) << " | ";
	OUT << setw(7)  << numbForm(MM_Nev[1]) << " || ";
	OUT << setw(7)  << numbForm(EM_Nev[0]) << " | ";
	OUT << setw(7)  << numbForm(EM_Nev[1]) << " || ";
	OUT << setw(7)  << numbForm(ME_Nev[0]) << " | ";
	OUT << setw(7)  << numbForm(ME_Nev[1]) << " || ";
	OUT << setw(7)  << numbForm(EE_Nev[0]) << " | ";
	OUT << setw(7)  << numbForm(EE_Nev[1]) << " || ";
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------" << endl;

	OUT.close();
	delete fMMFPRatios;
	delete fEMFPRatios;
	delete fMEFPRatios;
	delete fEEFPRatios;
}

void MuonPlotter::printSyncExercise(){
	fLumiNorm = 1000.;
	fDoCounting = false;

	fC_minMu1pt = 5.;
	fC_minMu2pt = 5.;
	fC_minEl1pt = 10.;
	fC_minEl2pt = 10.;
	fC_minHT    = 200.;
	fC_minMet   = 30.;
	fC_minNjets = 0;

	TString eventfilename = fOutputDir + "pass3_eth.txt";
	fOUTSTREAM.open(eventfilename.Data(), ios::trunc);

	Sample *S = fSamples[TTJetsSync];
	TTree *tree = S->getTree();
	fCurrentSample = TTJetsSync;

	// Counters
	int N_mumu(0), N_elel(0), N_elmu(0);

	// Event loop
	tree->ResetBranchAddresses();
	// Init(tree);
	if(S->datamc == 0) Init(tree);
	else InitMC(tree);

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		printProgress(jentry, nentries, S->name);

		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		// MuMu
		fCurrentChannel = Muon;
		int mu1(-1), mu2(-1);
		if(mumuSignalTrigger()){
			if(isSSLLMuEvent(mu1, mu2)){
				if(  isTightMuon(mu1) &&  isTightMuon(mu2) ){
					N_mumu++;
					fOUTSTREAM << " Mu/Mu in event ";
					fOUTSTREAM << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " Pt1 = " << setw(5) << Form("%6.2f", MuPt[mu1]);
					fOUTSTREAM << " D01 = " << setw(5) << Form("%6.3f", MuD0[mu1]);
					fOUTSTREAM << " Dz1 = " << setw(5) << Form("%6.3f", MuDz[mu1]);
					fOUTSTREAM << " Pt2 = " << setw(5) << Form("%6.2f", MuPt[mu2]);
					fOUTSTREAM << " D02 = " << setw(5) << Form("%6.3f", MuD0[mu2]);
					fOUTSTREAM << " Dz2 = " << setw(5) << Form("%6.3f", MuDz[mu2]);
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " NJET = " << setw(1) << Form("%1d", getNJets()) << endl;
				}
			}
		}
		resetHypLeptons();

		// EE
		fCurrentChannel = Elec;
		int el1(-1), el2(-1);
		if(elelSignalTrigger()){
			if( isSSLLElEvent(el1, el2) ){
				if(  isTightElectron(el1) &&  isTightElectron(el2) ){
					N_elel++;
					fOUTSTREAM << " El/El in event ";
					fOUTSTREAM << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " Pt1 = " << setw(5) << Form("%6.2f", ElPt[el1]);
					fOUTSTREAM << " D01 = " << setw(5) << Form("%6.3f", ElD0[el1]);
					fOUTSTREAM << " Dz1 = " << setw(5) << Form("%6.3f", ElDz[el1]);
					fOUTSTREAM << " Pt2 = " << setw(5) << Form("%6.2f", ElPt[el2]);
					fOUTSTREAM << " D02 = " << setw(5) << Form("%6.3f", MuD0[el2]);
					fOUTSTREAM << " Dz2 = " << setw(5) << Form("%6.3f", MuDz[el2]);
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " NJET = " << setw(1) << Form("%1d", getNJets()) << endl;
				}
			}
		}
		resetHypLeptons();

		// EMu
		fCurrentChannel = ElMu;
		int mu(-1), el(-1);
		if(elmuSignalTrigger()){
			if( isSSLLElMuEvent(mu, el) ){
				if(  isTightElectron(el) &&  isTightMuon(mu) ){
					N_elmu++;
					fOUTSTREAM << " El/Mu in event ";
					fOUTSTREAM << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " Pt1 = " << setw(5) << Form("%6.2f", MuPt[mu]);
					fOUTSTREAM << " D01 = " << setw(5) << Form("%6.3f", MuD0[mu]);
					fOUTSTREAM << " Dz1 = " << setw(5) << Form("%6.3f", MuDz[mu]);
					fOUTSTREAM << " Pt2 = " << setw(5) << Form("%6.2f", ElPt[el]);
					fOUTSTREAM << " D02 = " << setw(5) << Form("%6.3f", ElD0[el]);
					fOUTSTREAM << " Dz2 = " << setw(5) << Form("%6.3f", ElDz[el]);
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " NJET = " << setw(1) << Form("%1d", getNJets()) << endl;
				}
			}
		}
		resetHypLeptons();
	}
	cout << endl;

	float scale = fLumiNorm/S->lumi;

	// OUTPUT
	cout << endl;
	cout << "====================================================================" << endl;
	cout << " Event Yields " << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "        elel     |       mumu      |        elmu     |          tot" << endl;
	cout << setw(14) << left << " " << N_elel << " |";
	cout << setw(14) << left << " " << N_mumu << " |";
	cout << setw(14) << left << " " << N_elmu << " |";
	cout << setw(14) << left << " " << N_elel+N_mumu+N_elmu << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << " Event Yields scaled to 1/fb " << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << setw(6) << Form("%5.2f", scale * N_elel) << " +-";
	cout << setw(6) << Form("%5.2f", scale * sqrt(N_elel)) << " |";
	cout << setw(6) << Form("%5.2f", scale * N_mumu) << " +-";
	cout << setw(6) << Form("%5.2f", scale * sqrt(N_mumu)) << " |";
	cout << setw(6) << Form("%5.2f", scale * N_elmu) << " +-";
	cout << setw(6) << Form("%5.2f", scale * sqrt(N_elmu)) << " |";
	cout << setw(6) << Form("%5.2f", scale * (N_elel+N_mumu+N_elmu)) << " +-";
	cout << setw(6) << Form("%5.2f", scale * sqrt(N_elel+N_mumu+N_elmu)) << endl;
	cout << "====================================================================" << endl;
	cout << endl;
	cout << Form("| ETH/IFCA/Oviedo | %5.2f (%1d)  | %5.2f (%2d)   | %5.2f (%2d)   | %5.2f (%3d)   | [[%%ATTACHURL%%/pass3_eth.txt][yield.txt]] |"
		, scale*N_elel, N_elel
		, scale*N_mumu, N_mumu
		, scale*N_elmu, N_elmu
		, scale*(N_elel+N_mumu+N_elmu), N_elel+N_mumu+N_elmu) << endl;

	fOUTSTREAM.close();
	S->cleanUp();
	fOutputSubDir = "";
}

//____________________________________________________________________________
vector<TH1D*> MuonPlotter::MuMuFPPrediction(TH2D* H_fratio, TH2D* H_pratio, TH2D* H_nt2mes, TH2D* H_nt1mes, TH2D* H_nt0mes, bool output){
	///////////////////////////////////////////////////////////////////////////
	// Note: Careful, this is only a workaround at the moment, and is only
	//       really valid for one single eta bin!
	//       In the future we should rewrite the interface to FPRatios and
	//       pass it the ratios directly to have full control
	///////////////////////////////////////////////////////////////////////////
	vector<TH1D*> res;
	const int nptbins = getNPt2Bins(Muon);

	TH1D *H_dummy    = new TH1D("MuDummyhist","Dummy",                       H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nsigpred = new TH1D("MuNsigpred", "Predicted N_sig in Pt1 bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nfppred  = new TH1D("MuNfppred",  "Predicted N_fp in Pt1 bins",  H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nffpred  = new TH1D("MuNffpred",  "Predicted N_ff in Pt1 bins",  H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());	

	if(fVerbose > 2){
		cout << " Nt2 = " << H_nt2mes->GetEntries();
		cout << " Nt1 = " << H_nt1mes->GetEntries();
		cout << " Nt0 = " << H_nt0mes->GetEntries() << endl;
	}

	for(size_t i = 1; i <= nptbins; ++i){
		double pt1 = H_dummy->GetBinCenter(i);
		if(fVerbose > 2){
			cout << "=======================================" << endl;
			cout << "MuPt1 = " << pt1 << "  MufRatio = " << H_fratio->GetBinContent(i, 1) << "  MupRatio = " << H_pratio->GetBinContent(i, 1) << endl;
		}
		double eta1 = 0.0;
		double npppred(0.0), npppredEstat2(0.0), npppredEsyst2(0.0);
		double nfppred(0.0), nfppredEstat2(0.0), nfppredEsyst2(0.0);
		double nffpred(0.0), nffpredEstat2(0.0), nffpredEsyst2(0.0);
		for(size_t j = 1; j <= nptbins; ++j){
			if(fVerbose > 2) cout << " --------" << endl;
			double pt2 = H_dummy->GetBinCenter(j);

			double eta2 = 0.0;
			double nt2 = H_nt2mes->GetBinContent(i,j);
			double nt1 = H_nt1mes->GetBinContent(i,j);
			double nt0 = H_nt0mes->GetBinContent(i,j);

			if(fVerbose > 2) cout << "MuPt2 = " << pt2 << "  MufRatio = " << H_fratio->GetBinContent(j, 1) << "  MupRatio = " << H_pratio->GetBinContent(j, 1) << endl;
			if(fVerbose > 2) cout << "   nt2: " << nt2 << "  nt1: " << nt1 << "  nt0: " << nt0 << endl;

			FPRatios *fFPRatios = new FPRatios();
			fFPRatios->SetVerbose(fVerbose);
			fFPRatios->SetMuFratios(H_fratio);
			fFPRatios->SetMuPratios(H_pratio);
			vector<double> nt;
			nt.push_back(nt0); nt.push_back(nt1); nt.push_back(nt2);
			fFPRatios->NevtTopol(0, 2, nt);

			vector<double> vpt, veta;
			vpt.push_back(pt1); vpt.push_back(pt2);
			veta.push_back(eta1); veta.push_back(eta2);

			vector<double> nevFP = fFPRatios->NevtPass(vpt, veta);
			vector<double> nevFPEstat = fFPRatios->NevtPassErrStat();
			vector<double> nevFPEsyst = fFPRatios->NevtPassErrSyst();

			if(fVerbose > 2) cout << "   npp: " << nevFP[0] << "  nfp: " << nevFP[1] << "  nff: " << nevFP[2] << endl;
			npppred += nevFP[0];
			nfppred += nevFP[1];
			nffpred += nevFP[2];
			npppredEstat2 += nevFPEstat[0]*nevFPEstat[0];
			npppredEsyst2 += nevFPEsyst[0]*nevFPEsyst[0];
			nfppredEstat2 += nevFPEstat[1]*nevFPEstat[1];
			nfppredEsyst2 += nevFPEsyst[1]*nevFPEsyst[1];
			nffpredEstat2 += nevFPEstat[2]*nevFPEstat[2];
			nffpredEsyst2 += nevFPEsyst[2]*nevFPEsyst[2];
			delete fFPRatios;
		}
		Int_t bin = H_nsigpred->FindBin(pt1);
		H_nsigpred->SetBinContent(bin, npppred);
		H_nfppred ->SetBinContent(bin, nfppred);
		H_nffpred ->SetBinContent(bin, nffpred);
		H_nsigpred->SetBinError(bin, sqrt(npppredEstat2 + npppredEsyst2));
		H_nfppred ->SetBinError(bin, sqrt(nfppredEstat2 + nfppredEsyst2));
		H_nffpred ->SetBinError(bin, sqrt(nffpredEstat2 + nffpredEsyst2));
	}	

	if(fVerbose > 2) cout << " Predict " << H_nsigpred->Integral() << " signal events (Nsig = p^2*Npp) from this sample" << endl;
	if(fVerbose > 2) cout << " Predict " << H_nfppred->Integral() <<  " fake-prompt events (f*p*Nfp) from this sample" << endl;
	if(fVerbose > 2) cout << " Predict " << H_nffpred->Integral() <<  " fake-fake events (f*f*Nff) from this sample" << endl;
	// Output
	H_nsigpred->SetXTitle(convertVarName("MuPt[0]"));
	if(output){
		H_nt2mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt2mes->SetYTitle(convertVarName("MuPt[0]"));
		H_nt1mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt1mes->SetYTitle(convertVarName("MuPt[0]"));
		H_nt0mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt0mes->SetYTitle(convertVarName("MuPt[0]"));
		// if(H_nsigpred->GetMinimum() < 0) H_nsigpred->SetMaximum(0);
		// if(H_nsigpred->GetMinimum() > 0) H_nsigpred->SetMinimum(0);
		printObject(H_nsigpred, H_nsigpred->GetName());
		printObject(H_nt2mes, H_nt2mes->GetName(), "colz");
		printObject(H_nt1mes, H_nt1mes->GetName(), "colz");
		printObject(H_nt0mes, H_nt0mes->GetName(), "colz");
	}
	res.push_back(H_nsigpred);
	res.push_back(H_nfppred);
	res.push_back(H_nffpred);
	return res;
}
vector<TH1D*> MuonPlotter::ElElFPPrediction(TH2D* H_fratio, TH2D* H_pratio, TH2D* H_nt2mes, TH2D* H_nt1mes, TH2D* H_nt0mes, bool output){
	///////////////////////////////////////////////////////////////////////////
	// Note: Careful, this is only a workaround at the moment, and is only
	//       really valid for one single eta bin!
	//       In the future we should rewrite the interface to FPRatios and
	//       pass it the ratios directly to have full control
	///////////////////////////////////////////////////////////////////////////
	vector<TH1D*> res;
	const int nptbins = getNPt2Bins(Elec);

	TH1D *H_dummy    = new TH1D("ElDummyhist", "Dummy",                      H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nsigpred = new TH1D("ElNsigpred", "Predicted N_sig in Pt1 bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nfppred  = new TH1D("ElNfppred",  "Predicted N_fp in Pt1 bins",  H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nffpred  = new TH1D("ElNffpred",  "Predicted N_ff in Pt1 bins",  H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());	

	if(fVerbose > 2){
		cout << " Nt2 = " << H_nt2mes->GetEntries();
		cout << " Nt1 = " << H_nt1mes->GetEntries();
		cout << " Nt0 = " << H_nt0mes->GetEntries() << endl;
	}

	for(size_t i = 1; i <= nptbins; ++i){
		double pt1 = H_dummy->GetBinCenter(i);
		if(fVerbose > 2){
			cout << "=======================================" << endl;
			cout << "Pt1 = " << pt1 << endl;
		}
		double eta1 = 0.0;
		double npppred(0.0), npppredEstat2(0.0), npppredEsyst2(0.0);
		double nfppred(0.0), nfppredEstat2(0.0), nfppredEsyst2(0.0);
		double nffpred(0.0), nffpredEstat2(0.0), nffpredEsyst2(0.0);
		for(size_t j = 1; j <= nptbins; ++j){
			if(fVerbose > 2) cout << " --------" << endl;
			double pt2 = H_dummy->GetBinCenter(j);

			double eta2 = 0.0;
			double nt2 = H_nt2mes->GetBinContent(i,j);
			double nt1 = H_nt1mes->GetBinContent(i,j);
			double nt0 = H_nt0mes->GetBinContent(i,j);

			if(fVerbose > 2) cout << "   Pt2 = " << pt2 << endl;
			if(fVerbose > 2) cout << "   nt2: " << nt2 << "  nt1: " << nt1 << "  nt0: " << nt0 << endl;

			FPRatios *fFPRatios = new FPRatios();
			fFPRatios->SetVerbose(fVerbose);
			fFPRatios->SetElFratios(H_fratio);
			fFPRatios->SetElPratios(H_pratio);
			vector<double> nt;
			nt.push_back(nt0); nt.push_back(nt1); nt.push_back(nt2);
			fFPRatios->NevtTopol(2, 0, nt);

			vector<double> vpt, veta;
			vpt.push_back(pt1); vpt.push_back(pt2);
			veta.push_back(eta1); veta.push_back(eta2);

			vector<double> nevFP = fFPRatios->NevtPass(vpt, veta);
			vector<double> nevFPEstat = fFPRatios->NevtPassErrStat();
			vector<double> nevFPEsyst = fFPRatios->NevtPassErrSyst();
			npppred += nevFP[0];
			nfppred += nevFP[1];
			nffpred += nevFP[2];
			npppredEstat2 += nevFPEstat[0]*nevFPEstat[0];
			npppredEsyst2 += nevFPEsyst[0]*nevFPEsyst[0];
			nfppredEstat2 += nevFPEstat[1]*nevFPEstat[1];
			nfppredEsyst2 += nevFPEsyst[1]*nevFPEsyst[1];
			nffpredEstat2 += nevFPEstat[2]*nevFPEstat[2];
			nffpredEsyst2 += nevFPEsyst[2]*nevFPEsyst[2];
			delete fFPRatios;
		}
		Int_t bin = H_nsigpred->FindBin(pt1);
		H_nsigpred->SetBinContent(bin, npppred);
		H_nfppred ->SetBinContent(bin, nfppred);
		H_nffpred ->SetBinContent(bin, nffpred);
		H_nsigpred->SetBinError(bin, sqrt(npppredEstat2 + npppredEsyst2));
		H_nfppred ->SetBinError(bin, sqrt(nfppredEstat2 + nfppredEsyst2));
		H_nffpred ->SetBinError(bin, sqrt(nffpredEstat2 + nffpredEsyst2));
	}	

	if(fVerbose > 2) cout << " Predict " << H_nsigpred->Integral() << " signal events (Nsig = p^2*Npp) from this sample" << endl;
	if(fVerbose > 2) cout << " Predict " << H_nfppred->Integral() <<  " fake-prompt events (f*p*Nfp) from this sample" << endl;
	if(fVerbose > 2) cout << " Predict " << H_nffpred->Integral() <<  " fake-fake events (f*f*Nff) from this sample" << endl;
	// Output
	H_nsigpred->SetXTitle(convertVarName("ElPt[0]"));
	if(output){
		H_nt2mes->SetXTitle(convertVarName("ElPt[0]"));
		H_nt2mes->SetYTitle(convertVarName("ElPt[0]"));
		H_nt1mes->SetXTitle(convertVarName("ElPt[0]"));
		H_nt1mes->SetYTitle(convertVarName("ElPt[0]"));
		H_nt0mes->SetXTitle(convertVarName("ElPt[0]"));
		H_nt0mes->SetYTitle(convertVarName("ElPt[0]"));
		// if(H_nsigpred->GetMinimum() < 0) H_nsigpred->SetMaximum(0);
		// if(H_nsigpred->GetMinimum() > 0) H_nsigpred->SetMinimum(0);
		printObject(H_nsigpred, H_nsigpred->GetName());
		printObject(H_nt2mes, H_nt2mes->GetName(), "colz");
		printObject(H_nt1mes, H_nt1mes->GetName(), "colz");
		printObject(H_nt0mes, H_nt0mes->GetName(), "colz");
	}
	res.push_back(H_nsigpred);
	res.push_back(H_nfppred);
	res.push_back(H_nffpred);
	return res;
}
vector<TH1D*> MuonPlotter::ElMuFPPrediction(TH2D* H_mufratio, TH2D* H_mupratio, TH2D* H_elfratio, TH2D* H_elpratio, TH2D* H_nt2mes, TH2D* H_nt10mes, TH2D* H_nt01mes, TH2D* H_nt0mes, bool output){
	///////////////////////////////////////////////////////////////////////////
	// Note: Careful, this is only a workaround at the moment, and is only
	//       really valid for one single eta bin!
	//       In the future we should rewrite the interface to FPRatios and
	//       pass it the ratios directly to have full control
	///////////////////////////////////////////////////////////////////////////
	vector<TH1D*> res;

	TH1D *H_mudummy = new TH1D("ElMuDummy",   "Dummy1",                      H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray()); // x axis is muon
	TH1D *H_eldummy = new TH1D("MuElDummy",   "Dummy2",                      H_nt2mes->GetNbinsY(), H_nt2mes->GetYaxis()->GetXbins()->GetArray()); // y axis is electron
	TH1D *H_npppred = new TH1D("ElMuNpppred", "Predicted N_pp in MuPt bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nfppred = new TH1D("ElMuNfppred", "Predicted N_fp in MuPt bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_npfpred = new TH1D("ElMuNpfpred", "Predicted N_pf in MuPt bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());
	TH1D *H_nffpred = new TH1D("ElMuNffpred", "Predicted N_ff in MuPt bins", H_nt2mes->GetNbinsX(), H_nt2mes->GetXaxis()->GetXbins()->GetArray());	

	if(fVerbose > 2){
		cout << " Nt2  = " << H_nt2mes->GetEntries();
		cout << " Nt10 = " << H_nt10mes->GetEntries();
		cout << " Nt01 = " << H_nt01mes->GetEntries();
		cout << " Nt0  = " << H_nt0mes->GetEntries() << endl;
	}

	const double eta = 0.0;
	for(size_t i = 1; i <= H_mudummy->GetNbinsX(); ++i){ // loop on mu bins
		double mupt = H_mudummy->GetBinCenter(i);
		if(fVerbose > 2){
			cout << "=======================================" << endl;
			cout << "MuPt = " << mupt << "  MufRatio = " << H_mufratio->GetBinContent(i, 1) << "  MupRatio = " << H_mupratio->GetBinContent(i, 1) << endl;
		}
		double npppred(0.0), npppredEstat2(0.0), npppredEsyst2(0.0);
		double nfppred(0.0), nfppredEstat2(0.0), nfppredEsyst2(0.0);
		double npfpred(0.0), npfpredEstat2(0.0), npfpredEsyst2(0.0);
		double nffpred(0.0), nffpredEstat2(0.0), nffpredEsyst2(0.0);
		for(size_t j = 1; j <= H_eldummy->GetNbinsX(); ++j){ // loop on el bins
			if(fVerbose > 2) cout << " --------" << endl;
			double elpt = H_eldummy->GetBinCenter(j);

			double nt2  = H_nt2mes->GetBinContent(i,j);
			double nt10 = H_nt10mes->GetBinContent(i,j);
			double nt01 = H_nt01mes->GetBinContent(i,j);
			double nt0  = H_nt0mes->GetBinContent(i,j);

			if(fVerbose > 2) cout << "ElPt = " << elpt << "  ElfRatio = " << H_elfratio->GetBinContent(j, 1) << "  ElpRatio = " << H_elpratio->GetBinContent(j, 1) << endl;
			if(fVerbose > 2) cout << "   nt2: " << nt2 << "  nt10: " << nt10  << "  nt01: " << nt01 << "  nt0: " << nt0 << endl;

			FPRatios *fFPRatios = new FPRatios();
			fFPRatios->SetVerbose(fVerbose);
			fFPRatios->SetMuFratios(H_mufratio);
			fFPRatios->SetMuPratios(H_mupratio);
			fFPRatios->SetElFratios(H_elfratio);
			fFPRatios->SetElPratios(H_elpratio);
			vector<double> nt;
			nt.push_back(nt0);  // none pass
			nt.push_back(nt01); // e passes   >> Here I turn them around because FPRatios treats the first index as the electron,
			nt.push_back(nt10); // mu passes  >> and I have the opposite convention since muons rule electrons as everyone knows
			nt.push_back(nt2);  // both pass
			fFPRatios->NevtTopol(1, 1, nt);

			vector<double> vpt, veta;
			vpt.push_back(elpt); vpt.push_back(mupt);
			veta.push_back(eta); veta.push_back(eta);

			vector<double> nevFP      = fFPRatios->NevtPass(vpt, veta);
			vector<double> nevFPEstat = fFPRatios->NevtPassErrStat();
			vector<double> nevFPEsyst = fFPRatios->NevtPassErrSyst();
			npppred += nevFP[0];
			nfppred += nevFP[1];
			npfpred += nevFP[2];
			nffpred += nevFP[3];
			if(fVerbose > 2) cout << "   npp: " << nevFP[0] << "  nfp: " << nevFP[1]  << "  npf: " << nevFP[2] << "  nff: " << nevFP[3] << endl;
			npppredEstat2 += nevFPEstat[0]*nevFPEstat[0];
			npppredEsyst2 += nevFPEsyst[0]*nevFPEsyst[0];
			nfppredEstat2 += nevFPEstat[1]*nevFPEstat[1];
			nfppredEsyst2 += nevFPEsyst[1]*nevFPEsyst[1];
			npfpredEstat2 += nevFPEstat[2]*nevFPEstat[2];
			npfpredEsyst2 += nevFPEsyst[2]*nevFPEsyst[2];
			nffpredEstat2 += nevFPEstat[3]*nevFPEstat[3];
			nffpredEsyst2 += nevFPEsyst[3]*nevFPEsyst[3];
			delete fFPRatios;
		}
		Int_t bin = H_npppred->FindBin(mupt);
		H_npppred->SetBinContent(bin, npppred);
		H_nfppred->SetBinContent(bin, nfppred);
		H_npfpred->SetBinContent(bin, npfpred);
		H_nffpred->SetBinContent(bin, nffpred);
		H_npppred->SetBinError(bin, sqrt(npppredEstat2 + npppredEsyst2));
		H_nfppred->SetBinError(bin, sqrt(nfppredEstat2 + nfppredEsyst2));
		H_npfpred->SetBinError(bin, sqrt(npfpredEstat2 + npfpredEsyst2));
		H_nffpred->SetBinError(bin, sqrt(nffpredEstat2 + nffpredEsyst2));
	}	

	if(fVerbose > 2) cout << " Total: ";
	if(fVerbose > 2) cout << "   npp: " << H_npppred->Integral() << "  nfp: " << H_nfppred->Integral()  << "  npf: " << H_npfpred->Integral() << "  nff: " << H_nffpred->Integral() << endl;

	// Output
	H_npppred->SetXTitle(convertVarName("MuPt[0]"));
	if(output){
		H_nt2mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt2mes->SetYTitle(convertVarName("MuPt[0]"));
		H_nt10mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt10mes->SetYTitle(convertVarName("MuPt[0]"));
		H_nt01mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt01mes->SetYTitle(convertVarName("MuPt[0]"));
		H_nt0mes->SetXTitle(convertVarName("MuPt[0]"));
		H_nt0mes->SetYTitle(convertVarName("MuPt[0]"));
		printObject(H_npppred, H_npppred->GetName());
		printObject(H_nt2mes,  H_nt2mes->GetName(),  "colz");
		printObject(H_nt10mes, H_nt10mes->GetName(), "colz");
		printObject(H_nt01mes, H_nt01mes->GetName(), "colz");
		printObject(H_nt0mes,  H_nt0mes->GetName(),  "colz");
	}
	res.push_back(H_npppred);
	res.push_back(H_nfppred);
	res.push_back(H_npfpred);
	res.push_back(H_nffpred);
	return res;
}

//____________________________________________________________________________
void MuonPlotter::fillYields(Sample *S, gRegion reg, gHiLoSwitch hilo){
	///////////////////////////////////////////////////
	// Set custom event selections here:
	fC_minMu1pt = Region::minMu1pt[hilo];
	fC_minMu2pt = Region::minMu2pt[hilo];
	fC_minEl1pt = Region::minEl1pt[hilo];
	fC_minEl2pt = Region::minEl2pt[hilo];
	fC_minHT    = Region::minHT   [reg];
	fC_maxHT    = Region::maxHT   [reg];
	fC_minMet   = Region::minMet  [reg];
	fC_maxMet   = Region::maxMet  [reg];
	fC_minNjets = Region::minNjets[reg];

	///////////////////////////////////////////////////
	// SS YIELDS
	// MuMu Channel
	resetHypLeptons();
	fDoCounting = false;
	if(reg == Baseline && hilo == HighPt) fDoCounting = true;
	fCurrentChannel = Muon;
	float puweight = PUWeight;
	if(S->datamc == 4) puweight = 1; // fix for samples with no pileup
	int mu1(-1), mu2(-1);
	if(mumuSignalTrigger()){ // Trigger selection
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[2]);
		if(isSSLLMuEvent(mu1, mu2)){ // Same-sign loose-loose di muon event
			if(  isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Tight-tight
				if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[14]); // ... first muon passes tight cut
				if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[15]); // ... second muon passes tight cut
				if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[16]); // ... both muons pass tight cut
				S->region[reg][hilo].mm.nt20_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg][hilo].mm.nt20_eta->Fill(fabs(MuEta[mu1]), fabs(MuEta[mu2]), puweight);
				if(S->datamc == 0 && reg == Baseline && hilo == HighPt && (fCurrentSample != MuHad1 || fCurrentSample != MuHad2)){
					fOUTSTREAM << Form("%12s: MuMu - run %6.0d / ls %5.0d / ev %11.0d - HT(#J/#bJ) %6.2f(%1d/%1d) MET %6.2f MT2 %6.2f Pt1 %6.2f Pt2 %6.2f Charge %2d", S->sname.Data(), Run, LumiSec, Event, getHT(), getNJets(), getNBTags(), pfMET, getMT2(mu1,mu2,1), MuPt[mu1], MuPt[mu2], MuCharge[mu1]) << endl ;
				}
				if(S->datamc > 0 ){
					S->region[reg][hilo].mm.nt11_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
					if(isPromptMuon(mu1) && isPromptMuon(mu2)) S->region[reg][hilo].mm.nt2pp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(isPromptMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg][hilo].mm.nt2pf_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(  isFakeMuon(mu1) && isPromptMuon(mu2)) S->region[reg][hilo].mm.nt2fp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(  isFakeMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg][hilo].mm.nt2ff_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				}
			}
			if(  isTightMuon(mu1) && !isTightMuon(mu2) ){ // Tight-loose
				if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[14]); // ... first muon passes tight cut
				S->region[reg][hilo].mm.nt10_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg][hilo].mm.nt10_eta->Fill(fabs(MuEta[mu1]), fabs(MuEta[mu2]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].mm.nt10_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
			}
			if( !isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Loose-tight
				if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[15]); // ... second muon passes tight cut
				S->region[reg][hilo].mm.nt10_pt ->Fill(MuPt [mu2], MuPt [mu1], puweight); // tight one always in x axis; fill same again
				S->region[reg][hilo].mm.nt10_eta->Fill(fabs(MuEta[mu2]), fabs(MuEta[mu1]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].mm.nt10_origin->Fill(muIndexToBin(mu2)-0.5, muIndexToBin(mu1)-0.5, puweight);
			}
			if( !isTightMuon(mu1) && !isTightMuon(mu2) ){ // Loose-loose
				S->region[reg][hilo].mm.nt00_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg][hilo].mm.nt00_eta->Fill(fabs(MuEta[mu1]), fabs(MuEta[mu2]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].mm.nt00_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
			}
			if(S->datamc > 0){
				if(isPromptMuon(mu1) && isPromptMuon(mu2)) S->region[reg][hilo].mm.npp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(isPromptMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg][hilo].mm.npf_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(  isFakeMuon(mu1) && isPromptMuon(mu2)) S->region[reg][hilo].mm.nfp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(  isFakeMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg][hilo].mm.nff_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);			
			}
		}
		resetHypLeptons();
	}
	if(singleMuTrigger() && isSigSupMuEvent()){
		if( isTightMuon(0) ){
			S->region[reg][hilo].mm.fntight->Fill(MuPt[0], fabs(MuEta[0]), singleMuPrescale() * puweight);
			if(S->datamc > 0) S->region[reg][hilo].mm.sst_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
		if( isLooseMuon(0) ){
			S->region[reg][hilo].mm.fnloose->Fill(MuPt[0], fabs(MuEta[0]), singleMuPrescale() * puweight);
			if(S->datamc > 0) S->region[reg][hilo].mm.ssl_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
	}
	if(doubleMuTrigger() && isZMuMuEvent()){
		if( isTightMuon(0) ){
			S->region[reg][hilo].mm.pntight->Fill(MuPt[0], fabs(MuEta[0]), puweight);
			if(S->datamc > 0) S->region[reg][hilo].mm.zt_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
		if( isLooseMuon(0) ){
			S->region[reg][hilo].mm.pnloose->Fill(MuPt[0], fabs(MuEta[0]), puweight);
			if(S->datamc > 0) S->region[reg][hilo].mm.zl_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
	}				

	// EE Channel
	fCurrentChannel = Elec;
	int el1(-1), el2(-1);
	if(elelSignalTrigger()){
		if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[2]);
		if( isSSLLElEvent(el1, el2) ){
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[14]); // " ... first electron passes tight cut
				if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[15]); // " ... second electron passes tight cut
				if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[16]); // " ... both electrons pass tight cut
				S->region[reg][hilo].ee.nt20_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg][hilo].ee.nt20_eta->Fill(fabs(ElEta[el1]), fabs(ElEta[el2]), puweight);
				if(S->datamc == 0 && reg == Baseline && hilo == HighPt && (fCurrentSample != EleHad1 || fCurrentSample != EleHad2)){
					fOUTSTREAM << Form("%12s: ElEl - run %6.0d / ls %5.0d / ev %11.0d - HT(#J/#bJ) %6.2f(%1d/%1d) MET %6.2f MT2 %6.2f Pt1 %6.2f Pt2 %6.2f Charge %2d", S->sname.Data(), Run, LumiSec, Event, getHT(), getNJets(), getNBTags(), pfMET, getMT2(el1,el2,2), ElPt[el1], ElPt[el2], ElCharge[el1]) << endl ;
				}
				if(S->datamc > 0 ){
					S->region[reg][hilo].ee.nt11_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
					if(isPromptElectron(el1) && isPromptElectron(el2)){
						S->region[reg][hilo].ee.nt2pp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
						if(!isChargeMatchedElectron(el1) || !isChargeMatchedElectron(el2)){
							S->region[reg][hilo].ee.nt2pp_cm_pt->Fill(ElPt[el1], ElPt[el2], puweight);							
						}
					}
					if(isPromptElectron(el1) &&   isFakeElectron(el2)) S->region[reg][hilo].ee.nt2pf_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(  isFakeElectron(el1) && isPromptElectron(el2)) S->region[reg][hilo].ee.nt2fp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(  isFakeElectron(el1) &&   isFakeElectron(el2)) S->region[reg][hilo].ee.nt2ff_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				}
			}
			if(  isTightElectron(el1) && !isTightElectron(el2) ){ // Tight-loose
				if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[14]);
				S->region[reg][hilo].ee.nt10_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg][hilo].ee.nt10_eta->Fill(fabs(ElEta[el1]), fabs(ElEta[el2]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].ee.nt10_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
			}
			if( !isTightElectron(el1) &&  isTightElectron(el2) ){ // Loose-tight
				if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[15]);
				S->region[reg][hilo].ee.nt10_pt ->Fill(ElPt [el2], ElPt [el1], puweight); // tight one always in x axis; fill same again
				S->region[reg][hilo].ee.nt10_eta->Fill(fabs(ElEta[el2]), fabs(ElEta[el2]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].ee.nt10_origin->Fill(elIndexToBin(el2)-0.5, elIndexToBin(el1)-0.5, puweight);
			}
			if( !isTightElectron(el1) && !isTightElectron(el2) ){ // Loose-loose
				S->region[reg][hilo].ee.nt00_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg][hilo].ee.nt00_eta->Fill(fabs(ElEta[el1]), fabs(ElEta[el2]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].ee.nt00_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
			}
			if(S->datamc > 0 ){
				if(isPromptElectron(el1) && isPromptElectron(el2)){
					S->region[reg][hilo].ee.npp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(!isChargeMatchedElectron(el1) || !isChargeMatchedElectron(el2)){
						S->region[reg][hilo].ee.npp_cm_pt->Fill(ElPt[el1], ElPt[el2], puweight);							
					}
				}
				if(isPromptElectron(el1) &&   isFakeElectron(el2)) S->region[reg][hilo].ee.npf_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(  isFakeElectron(el1) && isPromptElectron(el2)) S->region[reg][hilo].ee.nfp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(  isFakeElectron(el1) &&   isFakeElectron(el2)) S->region[reg][hilo].ee.nff_pt->Fill(ElPt[el1], ElPt[el2], puweight);
			}
		}
		resetHypLeptons();
	}
	if(singleElTrigger() && isSigSupElEvent()){
		if( isTightElectron(0) ){
			S->region[reg][hilo].ee.fntight->Fill(ElPt[0], fabs(ElEta[0]), singleElPrescale() * puweight);
			if(S->datamc > 0) S->region[reg][hilo].ee.sst_origin->Fill(elIndexToBin(0)-0.5, puweight);
		}
		if( isLooseElectron(0) ){
			S->region[reg][hilo].ee.fnloose->Fill(ElPt[0], fabs(ElEta[0]), singleElPrescale() * puweight);
			if(S->datamc > 0) S->region[reg][hilo].ee.ssl_origin->Fill(elIndexToBin(0)-0.5, puweight);
		}
	}
	int elind;
	if(doubleElTrigger() && isZElElEvent(elind)){
		if( isTightElectron(elind) ){
			S->region[reg][hilo].ee.pntight->Fill(ElPt[elind], fabs(ElEta[elind]), puweight);
			if(S->datamc > 0) S->region[reg][hilo].ee.zt_origin->Fill(elIndexToBin(elind)-0.5, puweight);
		}
		if( isLooseElectron(elind) ){
			S->region[reg][hilo].ee.pnloose->Fill(ElPt[elind], fabs(ElEta[elind]), puweight);
			if(S->datamc > 0) S->region[reg][hilo].ee.zl_origin->Fill(elIndexToBin(elind)-0.5, puweight);
		}
	}

	// EMu Channel
	fCurrentChannel = ElMu;
	int mu(-1), el(-1);
	if(elmuSignalTrigger()){
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[2]);
		if( isSSLLElMuEvent(mu, el) ){
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[14]);
				if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[15]);
				if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[16]);
				S->region[reg][hilo].em.nt20_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg][hilo].em.nt20_eta->Fill(fabs(MuEta[mu]), fabs(ElEta[el]), puweight);
				if(S->datamc == 0 && reg == Baseline && hilo == HighPt){
					fOUTSTREAM << Form("%12s: ElMu - run %6.0d / ls %5.0d / ev %11.0d - HT(#J/#bJ) %6.2f(%1d/%1d) MET %6.2f MT2 %6.2f Pt1 %6.2f Pt2 %6.2f Charge %2d", S->sname.Data(), Run, LumiSec, Event, getHT(), getNJets(), getNBTags(), pfMET, getMT2(mu,el,3), MuPt[mu], ElPt[el], ElCharge[el]) << endl;
				}

				if(S->datamc > 0){
					S->region[reg][hilo].em.nt11_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
					if(isPromptMuon(mu) && isPromptElectron(el)) S->region[reg][hilo].em.nt2pp_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(isPromptMuon(mu) &&   isFakeElectron(el)) S->region[reg][hilo].em.nt2pf_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(  isFakeMuon(mu) && isPromptElectron(el)) S->region[reg][hilo].em.nt2fp_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(  isFakeMuon(mu) &&   isFakeElectron(el)) S->region[reg][hilo].em.nt2ff_pt->Fill(MuPt[mu], ElPt[el], puweight);
				}
			}
			if( !isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-loose
				if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[14]);
				S->region[reg][hilo].em.nt10_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg][hilo].em.nt10_eta->Fill(fabs(MuEta[mu]), fabs(ElEta[el]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].em.nt10_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if(  isTightElectron(el) && !isTightMuon(mu) ){ // Loose-tight
				if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[15]);
				S->region[reg][hilo].em.nt01_pt ->Fill(MuPt [mu], ElPt [el], puweight); // muon always in x axis for e/mu
				S->region[reg][hilo].em.nt01_eta->Fill(fabs(MuEta[mu]), fabs(ElEta[el]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].em.nt01_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if( !isTightElectron(0) && !isTightMuon(0) ){ // Loose-loose
				S->region[reg][hilo].em.nt00_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg][hilo].em.nt00_eta->Fill(fabs(MuEta[mu]), fabs(ElEta[el]), puweight);
				if(S->datamc > 0) S->region[reg][hilo].em.nt00_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if(S->datamc > 0){
				if(isPromptMuon(mu) && isPromptElectron(el)) S->region[reg][hilo].em.npp_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(isPromptMuon(mu) &&   isFakeElectron(el)) S->region[reg][hilo].em.npf_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(  isFakeMuon(mu) && isPromptElectron(el)) S->region[reg][hilo].em.nfp_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(  isFakeMuon(mu) &&   isFakeElectron(el)) S->region[reg][hilo].em.nff_pt->Fill(MuPt[mu], ElPt[el], puweight);
			}
		}
		resetHypLeptons();
	}

	///////////////////////////////////////////////////
	// OS YIELDS
	fDoCounting = false;
	fChargeSwitch = 1;

	// EE Channel
	fCurrentChannel = Elec;
	if(elelSignalTrigger()){
		if( isSSLLElEvent(el1, el2) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				if( isBarrelElectron(el1) &&  isBarrelElectron(el2)) S->region[reg][hilo].ee.nt20_OS_BB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(!isBarrelElectron(el1) && !isBarrelElectron(el2)) S->region[reg][hilo].ee.nt20_OS_EE_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if( isBarrelElectron(el1) && !isBarrelElectron(el2)) S->region[reg][hilo].ee.nt20_OS_EB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(!isBarrelElectron(el1) &&  isBarrelElectron(el2)) S->region[reg][hilo].ee.nt20_OS_EB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
			}
		}
		resetHypLeptons();
	}

	// EMu Channel
	fCurrentChannel = ElMu;
	if(elmuSignalTrigger()){
		if( isSSLLElMuEvent(mu, el) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				if( isBarrelElectron(el)) S->region[reg][hilo].em.nt20_OS_BB_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(!isBarrelElectron(el)) S->region[reg][hilo].em.nt20_OS_EE_pt->Fill(MuPt[mu], ElPt[el], puweight);
			}
		}
		resetHypLeptons();
	}
	fChargeSwitch = 0;
	fDoCounting = false;
	resetHypLeptons();
}
void MuonPlotter::fillDiffYields(Sample *S){
	///////////////////////////////////////////////////
	// Set custom event selections here:
	fC_minMu1pt = Region::minMu1pt[HighPt];
	fC_minMu2pt = Region::minMu2pt[HighPt];
	fC_minEl1pt = Region::minEl1pt[HighPt];
	fC_minEl2pt = Region::minEl2pt[HighPt];
	fC_minMet = 30.;
	fC_maxMet = 7000.;
	fC_maxHT  = 7000.;
	///////////////////////////////////////////////////
	// SS YIELDS
	// MuMu Channel
	resetHypLeptons();
	fDoCounting = false;
	fCurrentChannel = Muon;
	float puweight = PUWeight;
	if(S->datamc == 4) puweight = 1; // fix for samples with no pileup
	int mu1(-1), mu2(-1);
	if(mumuSignalTrigger()){ // Trigger selection
		fC_minHT  = 0.;
		fC_minNjets = 0;
		if(isSSLLMuEvent(mu1, mu2)){ // Same-sign loose-loose di muon event
			float HT  = getHT();
			float NJ  = getNJets() + 0.5;
			if(  isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Tight-tight
				S->diffyields[Muon].hnt11[0]->Fill(HT,    puweight);
				S->diffyields[Muon].hnt11[2]->Fill(NJ,    puweight);
			}
			if(  isTightMuon(mu1) && !isTightMuon(mu2) ){ // Tight-loose
				S->diffyields[Muon].hnt10[0]->Fill(HT,    puweight);
				S->diffyields[Muon].hnt10[2]->Fill(NJ,    puweight);
			}
			if( !isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Loose-tight
				S->diffyields[Muon].hnt01[0]->Fill(HT,    puweight);
				S->diffyields[Muon].hnt01[2]->Fill(NJ,    puweight);
			}
			if( !isTightMuon(mu1) && !isTightMuon(mu2) ){ // Loose-loose
				S->diffyields[Muon].hnt00[0]->Fill(HT,    puweight);
				S->diffyields[Muon].hnt00[2]->Fill(NJ,    puweight);
			}
		}
		resetHypLeptons();
		// Ask for 2 jets / 80 GeV HT when obtaining yields for met and mt2 plots
		fC_minHT =  80.;
		fC_minNjets = 2;
		if(isSSLLMuEvent(mu1, mu2)){ // Same-sign loose-loose di muon event
			float MT2 = getMT2(mu1, mu2, 1);
			if(  isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Tight-tight
				S->diffyields[Muon].hnt11[1]->Fill(pfMET, puweight);
				S->diffyields[Muon].hnt11[3]->Fill(MT2,   puweight);
				S->diffyields[Muon].hnt11[4]->Fill(MuPt[mu1], puweight);
				S->diffyields[Muon].hnt11[5]->Fill(MuPt[mu2], puweight);
			}
			if(  isTightMuon(mu1) && !isTightMuon(mu2) ){ // Tight-loose
				S->diffyields[Muon].hnt10[1]->Fill(pfMET, puweight);
				S->diffyields[Muon].hnt10[3]->Fill(MT2,   puweight);
				S->diffyields[Muon].hnt10[4]->Fill(MuPt[mu1], puweight);
				S->diffyields[Muon].hnt10[5]->Fill(MuPt[mu2], puweight);
			}
			if( !isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Loose-tight
				S->diffyields[Muon].hnt01[1]->Fill(pfMET, puweight);
				S->diffyields[Muon].hnt01[3]->Fill(MT2,   puweight);
				S->diffyields[Muon].hnt01[4]->Fill(MuPt[mu1], puweight);
				S->diffyields[Muon].hnt01[5]->Fill(MuPt[mu2], puweight);
			}
			if( !isTightMuon(mu1) && !isTightMuon(mu2) ){ // Loose-loose
				S->diffyields[Muon].hnt00[1]->Fill(pfMET, puweight);
				S->diffyields[Muon].hnt00[3]->Fill(MT2,   puweight);
				S->diffyields[Muon].hnt00[4]->Fill(MuPt[mu1], puweight);
				S->diffyields[Muon].hnt00[5]->Fill(MuPt[mu2], puweight);
			}
		}
		resetHypLeptons();
	}

	// EE Channel
	fCurrentChannel = Elec;
	int el1(-1), el2(-1);
	if(elelSignalTrigger()){
		fC_minHT  = 0.;
		fC_minNjets = 0;
		if( isSSLLElEvent(el1, el2) ){
			float HT  = getHT();
			float NJ  = getNJets() + 0.5;
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				S->diffyields[Elec].hnt11[0]->Fill(HT,    puweight);
				S->diffyields[Elec].hnt11[2]->Fill(NJ,    puweight);
			}
			if(  isTightElectron(el1) && !isTightElectron(el2) ){ // Tight-loose
				S->diffyields[Elec].hnt10[0]->Fill(HT,    puweight);
				S->diffyields[Elec].hnt10[2]->Fill(NJ,    puweight);
			}
			if( !isTightElectron(el1) &&  isTightElectron(el2) ){ // Loose-tight
				S->diffyields[Elec].hnt01[0]->Fill(HT,    puweight);
				S->diffyields[Elec].hnt01[2]->Fill(NJ,    puweight);
			}
			if( !isTightElectron(el1) && !isTightElectron(el2) ){ // Loose-loose
				S->diffyields[Elec].hnt00[0]->Fill(HT,    puweight);
				S->diffyields[Elec].hnt00[2]->Fill(NJ,    puweight);
			}
		}
		resetHypLeptons();
		// Ask for 2 jets / 80 GeV HT when obtaining yields for met and mt2 plots
		fC_minHT =  80.;
		fC_minNjets = 2;
		if( isSSLLElEvent(el1, el2) ){
			float MT2 = getMT2(el1, el2, 2);
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				S->diffyields[Elec].hnt11[1]->Fill(pfMET, puweight);
				S->diffyields[Elec].hnt11[3]->Fill(MT2,   puweight);
				S->diffyields[Elec].hnt11[4]->Fill(ElPt[el1], puweight);
				S->diffyields[Elec].hnt11[5]->Fill(ElPt[el2], puweight);
			}
			if(  isTightElectron(el1) && !isTightElectron(el2) ){ // Tight-loose
				S->diffyields[Elec].hnt10[1]->Fill(pfMET, puweight);
				S->diffyields[Elec].hnt10[3]->Fill(MT2,   puweight);
				S->diffyields[Elec].hnt10[4]->Fill(ElPt[el1], puweight);
				S->diffyields[Elec].hnt10[5]->Fill(ElPt[el2], puweight);
			}
			if( !isTightElectron(el1) &&  isTightElectron(el2) ){ // Loose-tight
				S->diffyields[Elec].hnt01[1]->Fill(pfMET, puweight);
				S->diffyields[Elec].hnt01[3]->Fill(MT2,   puweight);
				S->diffyields[Elec].hnt01[4]->Fill(ElPt[el1], puweight);
				S->diffyields[Elec].hnt01[5]->Fill(ElPt[el2], puweight);
			}
			if( !isTightElectron(el1) && !isTightElectron(el2) ){ // Loose-loose
				S->diffyields[Elec].hnt00[1]->Fill(pfMET, puweight);
				S->diffyields[Elec].hnt00[3]->Fill(MT2,   puweight);
				S->diffyields[Elec].hnt00[4]->Fill(ElPt[el1], puweight);
				S->diffyields[Elec].hnt00[5]->Fill(ElPt[el2], puweight);
			}
		}
		resetHypLeptons();
	}

	// EMu Channel
	fCurrentChannel = ElMu;
	int mu(-1), el(-1);
	if(elmuSignalTrigger()){
		fC_minHT  = 0.;
		fC_minNjets = 0;
		if( isSSLLElMuEvent(mu, el) ){
			float HT  = getHT();
			float NJ  = getNJets() + 0.5;
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				S->diffyields[ElMu].hnt11[0]->Fill(HT,    puweight);
				S->diffyields[ElMu].hnt11[2]->Fill(NJ,    puweight);
			}
			if( !isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-loose
				S->diffyields[ElMu].hnt10[0]->Fill(HT,    puweight);
				S->diffyields[ElMu].hnt10[2]->Fill(NJ,    puweight);
			}
			if(  isTightElectron(el) && !isTightMuon(mu) ){ // Loose-tight
				S->diffyields[ElMu].hnt01[0]->Fill(HT,    puweight);
				S->diffyields[ElMu].hnt01[2]->Fill(NJ,    puweight);
			}
			if( !isTightElectron(0) && !isTightMuon(0) ){ // Loose-loose
				S->diffyields[ElMu].hnt00[0]->Fill(HT,    puweight);
				S->diffyields[ElMu].hnt00[2]->Fill(NJ,    puweight);
			}
		}
		resetHypLeptons();
		fC_minHT =  80.;
		fC_minNjets = 2;		
		if( isSSLLElMuEvent(mu, el) ){
			float MT2 = getMT2(mu, el, 3);
			float ptmax = MuPt[mu];
			float ptmin = ElPt[el];
			if(ptmin > ptmax){
				ptmin = MuPt[mu];
				ptmax = ElPt[el];
			}
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				S->diffyields[ElMu].hnt11[1]->Fill(pfMET, puweight);
				S->diffyields[ElMu].hnt11[3]->Fill(MT2,   puweight);
				S->diffyields[ElMu].hnt11[4]->Fill(ptmax, puweight);
				S->diffyields[ElMu].hnt11[5]->Fill(ptmin, puweight);
			}
			if( !isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-loose
				S->diffyields[ElMu].hnt10[1]->Fill(pfMET, puweight);
				S->diffyields[ElMu].hnt10[3]->Fill(MT2,   puweight);
				S->diffyields[ElMu].hnt10[4]->Fill(ptmax, puweight);
				S->diffyields[ElMu].hnt10[5]->Fill(ptmin, puweight);
			}
			if(  isTightElectron(el) && !isTightMuon(mu) ){ // Loose-tight
				S->diffyields[ElMu].hnt01[1]->Fill(pfMET, puweight);
				S->diffyields[ElMu].hnt01[3]->Fill(MT2,   puweight);
				S->diffyields[ElMu].hnt01[4]->Fill(ptmax, puweight);
				S->diffyields[ElMu].hnt01[5]->Fill(ptmin, puweight);
			}
			if( !isTightElectron(0) && !isTightMuon(0) ){ // Loose-loose
				S->diffyields[ElMu].hnt00[1]->Fill(pfMET, puweight);
				S->diffyields[ElMu].hnt00[3]->Fill(MT2,   puweight);
				S->diffyields[ElMu].hnt00[4]->Fill(ptmax, puweight);
				S->diffyields[ElMu].hnt00[5]->Fill(ptmin, puweight);
			}
		}
		resetHypLeptons();
	}

	///////////////////////////////////////////////////
	// OS YIELDS
	fChargeSwitch = 1;

	// EE Channel
	fCurrentChannel = Elec;
	if(elelSignalTrigger()){
		fC_minHT    = 0.;
		fC_minNjets = 0;
		if( isSSLLElEvent(el1, el2) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				float HT  = getHT();
				float NJ  = getNJets() + 0.5;
				if( isBarrelElectron(el1) &&  isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_BB[0]->Fill(HT, puweight);
					S->diffyields[Elec].hnt2_os_BB[2]->Fill(NJ, puweight);
				}
				if(!isBarrelElectron(el1) && !isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EE[0]->Fill(HT, puweight);
					S->diffyields[Elec].hnt2_os_EE[2]->Fill(NJ, puweight);
				}
				if( isBarrelElectron(el1) && !isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EB[0]->Fill(HT, puweight);
					S->diffyields[Elec].hnt2_os_EB[2]->Fill(NJ, puweight);
				}
				if(!isBarrelElectron(el1) &&  isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EB[0]->Fill(HT, puweight);
					S->diffyields[Elec].hnt2_os_EB[2]->Fill(NJ, puweight);
				}
			}
		}
		resetHypLeptons();
		fC_minHT    = 80.;
		fC_minNjets = 2;
		if( isSSLLElEvent(el1, el2) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				float MT2 = getMT2(el1, el2, 2);
				if( isBarrelElectron(el1) &&  isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_BB[1]->Fill(pfMET, puweight);
					S->diffyields[Elec].hnt2_os_BB[3]->Fill(MT2  , puweight);
					S->diffyields[Elec].hnt2_os_BB[4]->Fill(ElPt[el1], puweight);
					S->diffyields[Elec].hnt2_os_BB[5]->Fill(ElPt[el2], puweight);
				}
				if(!isBarrelElectron(el1) && !isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EE[1]->Fill(pfMET, puweight);
					S->diffyields[Elec].hnt2_os_EE[3]->Fill(MT2  , puweight);
					S->diffyields[Elec].hnt2_os_EE[4]->Fill(ElPt[el1], puweight);
					S->diffyields[Elec].hnt2_os_EE[5]->Fill(ElPt[el2], puweight);
				}
				if( isBarrelElectron(el1) && !isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EB[1]->Fill(pfMET, puweight);
					S->diffyields[Elec].hnt2_os_EB[3]->Fill(MT2  , puweight);
					S->diffyields[Elec].hnt2_os_EB[4]->Fill(ElPt[el1], puweight);
					S->diffyields[Elec].hnt2_os_EB[5]->Fill(ElPt[el2], puweight);
				}
				if(!isBarrelElectron(el1) &&  isBarrelElectron(el2)){
					S->diffyields[Elec].hnt2_os_EB[1]->Fill(pfMET, puweight);
					S->diffyields[Elec].hnt2_os_EB[3]->Fill(MT2  , puweight);
					S->diffyields[Elec].hnt2_os_EB[4]->Fill(ElPt[el1], puweight);
					S->diffyields[Elec].hnt2_os_EB[5]->Fill(ElPt[el2], puweight);
				}
			}
		}
		resetHypLeptons();
	}

	// EMu Channel
	fCurrentChannel = ElMu;
	if(elmuSignalTrigger()){
		fC_minHT    = 0.;
		fC_minNjets = 0;
		if( isSSLLElMuEvent(mu, el) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				float HT  = getHT();
				float NJ  = getNJets() + 0.5;
				if( isBarrelElectron(el)){
					S->diffyields[ElMu].hnt2_os_BB[0]->Fill(HT, puweight);
					S->diffyields[ElMu].hnt2_os_BB[2]->Fill(NJ, puweight);
				}
				if(!isBarrelElectron(el)){
					S->diffyields[ElMu].hnt2_os_BB[0]->Fill(HT, puweight);
					S->diffyields[ElMu].hnt2_os_BB[2]->Fill(NJ, puweight);
				}
			}
		}
		resetHypLeptons();
		fC_minHT    = 80.;
		fC_minNjets = 2;
		if( isSSLLElMuEvent(mu, el) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				float MT2 = getMT2(mu, el, 3);
				float ptmax = MuPt[mu];
				float ptmin = ElPt[el];
				if(ptmin > ptmax){
					ptmin = MuPt[mu];
					ptmax = ElPt[el];
				}
				if( isBarrelElectron(el)){
					S->diffyields[ElMu].hnt2_os_BB[1]->Fill(pfMET, puweight);
					S->diffyields[ElMu].hnt2_os_BB[3]->Fill(MT2  , puweight);
					S->diffyields[ElMu].hnt2_os_BB[4]->Fill(ptmax, puweight);
					S->diffyields[ElMu].hnt2_os_BB[5]->Fill(ptmin, puweight);
				}
				if(!isBarrelElectron(el)){
					S->diffyields[ElMu].hnt2_os_EE[1]->Fill(pfMET, puweight);
					S->diffyields[ElMu].hnt2_os_EE[3]->Fill(MT2  , puweight);
					S->diffyields[ElMu].hnt2_os_EE[4]->Fill(ptmax, puweight);
					S->diffyields[ElMu].hnt2_os_EE[5]->Fill(ptmin, puweight);
				}
			}
		}
		resetHypLeptons();
	}
	fChargeSwitch = 0;
	resetHypLeptons();
}
void MuonPlotter::fillRatioPlots(Sample *S){
	resetHypLeptons();
	fDoCounting = false;
	fChargeSwitch = 0;
	float puweight = PUWeight;
	if(S->datamc == 4) puweight = 1; // fix for samples with no pileup

	// Reset event selections to baseline:
	fC_minMu1pt = Region::minMu1pt[HighPt];
	fC_minMu2pt = Region::minMu2pt[HighPt];
	fC_minEl1pt = Region::minEl1pt[HighPt];
	fC_minEl2pt = Region::minEl2pt[HighPt];

	fC_minHT    = Region::minHT   [Baseline];
	fC_minMet   = Region::minMet  [Baseline];
	fC_maxHT    = Region::maxHT   [Baseline];
	fC_maxMet   = Region::maxMet  [Baseline];
	fC_minNjets = Region::minNjets[Baseline];

	FRatioPlots *RP0 = &S->ratioplots[0];
	FRatioPlots *RP1 = &S->ratioplots[1];

	fCurrentChannel = Muon;
	if(singleMuTrigger()){
		if(isSigSupMuEvent()){
			if( isTightMuon(0) ){
				RP0->ntight[0]->Fill(getNJets(),               puweight);
				RP0->ntight[1]->Fill(getHT(),                  puweight);
				RP0->ntight[2]->Fill(getMaxJPt(),              puweight);
				RP0->ntight[3]->Fill(NVrtx,                    puweight);
				RP0->ntight[4]->Fill(getClosestJetPt(0, Muon), puweight);
				RP0->ntight[5]->Fill(getAwayJetPt(0, Muon),    puweight);
				RP0->ntight[6]->Fill(getNBTags(),              puweight);
			}
			if( isLooseMuon(0) ){
				RP0->nloose[0]->Fill(getNJets(),               puweight);
				RP0->nloose[1]->Fill(getHT(),                  puweight);
				RP0->nloose[2]->Fill(getMaxJPt(),              puweight);
				RP0->nloose[3]->Fill(NVrtx,                    puweight);
				RP0->nloose[4]->Fill(getClosestJetPt(0, Muon), puweight);
				RP0->nloose[5]->Fill(getAwayJetPt(0, Muon),    puweight);
				RP0->nloose[6]->Fill(getNBTags(),              puweight);
			}
		}
		fC_maxMet_Control = 1000.;
		if(isSigSupMuEvent()){
			if( isTightMuon(0) ){
				RP0->ntight[7]->Fill(pfMET,                    puweight);
			}
			if( isLooseMuon(0) ){
				RP0->nloose[7]->Fill(pfMET,                    puweight);
			}
		}		
		fC_maxMet_Control = 20.;
		fC_maxMt_Control = 1000.;
		if(isSigSupMuEvent()){
			if( isTightMuon(0) ){
				RP0->ntight[8]->Fill(MuMT[0],                  puweight);
			}
			if( isLooseMuon(0) ){
				RP0->nloose[8]->Fill(MuMT[0],                  puweight);
			}
		}		
		fC_maxMt_Control = 20.;
	}
	resetHypLeptons();
	if(singleElTrigger()){
		if(isSigSupElEvent()){
			if( isTightElectron(0) ){
				RP1->ntight[0]->Fill(getNJets(),                   puweight);
				RP1->ntight[1]->Fill(getHT(),                      puweight);
				RP1->ntight[2]->Fill(getMaxJPt(),                  puweight);
				RP1->ntight[3]->Fill(NVrtx,                        puweight);
				RP1->ntight[4]->Fill(getClosestJetPt(0, Elec), puweight);
				RP1->ntight[5]->Fill(getAwayJetPt(0, Elec),    puweight);
				RP1->ntight[6]->Fill(getNBTags(),                  puweight);
			}
			if( isLooseElectron(0) ){
				RP1->nloose[0]->Fill(getNJets(),                   puweight);
				RP1->nloose[1]->Fill(getHT(),                      puweight);
				RP1->nloose[2]->Fill(getMaxJPt(),                  puweight);
				RP1->nloose[3]->Fill(NVrtx,                        puweight);
				RP1->nloose[4]->Fill(getClosestJetPt(0, Elec), puweight);
				RP1->nloose[5]->Fill(getAwayJetPt(0, Elec),    puweight);
				RP1->nloose[6]->Fill(getNBTags(),                  puweight);
			}
		}
		fC_maxMet_Control = 1000.;
		if(isSigSupElEvent()){
			if( isTightElectron(0) ){
				RP1->ntight[7]->Fill(pfMET,                    puweight);
			}
			if( isLooseElectron(0) ){
				RP1->nloose[7]->Fill(pfMET,                    puweight);
			}
		}		
		fC_maxMet_Control = 20.;
		fC_maxMt_Control = 1000.;
		if(isSigSupElEvent()){
			if( isTightElectron(0) ){
				RP1->ntight[8]->Fill(ElMT[0],                  puweight);
			}
			if( isLooseElectron(0) ){
				RP1->nloose[8]->Fill(ElMT[0],                  puweight);
			}
		}		
		fC_maxMt_Control = 20.;
	}
	resetHypLeptons();
}
void MuonPlotter::fillKinPlots(gSample i, gHiLoSwitch hilo){
	resetHypLeptons();
	fDoCounting = false;
	Sample *S = fSamples[i];
	KinPlots *KP0 = &S->kinplots[0][hilo];
	KinPlots *KP1 = &S->kinplots[1][hilo];
	KinPlots *KP2 = &S->kinplots[2][hilo];
	int ind1(-1), ind2(-1);
	int mu(-1), el(-1); // for e/mu channel, easier readability

	float puweight = PUWeight;
	if(S->datamc == 4) puweight = 1; // fix for samples with no pileup

	///////////////////////////////////////////////////
	// Set custom event selections here:
	fC_minMu1pt = Region::minMu1pt[hilo];
	fC_minMu2pt = Region::minMu2pt[hilo];
	fC_minEl1pt = Region::minEl1pt[hilo];
	fC_minEl2pt = Region::minEl2pt[hilo];
	fC_minHT    = Region::minHT   [Baseline];
	fC_maxHT    = Region::maxHT   [Baseline];
	fC_minMet   = Region::minMet  [Baseline];
	fC_maxMet   = Region::maxMet  [Baseline];
	fC_minNjets = Region::minNjets[Baseline];
	if(hilo == LowPt) fC_minHT = 200.;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUMU CHANNEL:  //////////////////////////////////////////////////////////////////////////////////////
	if(mumuSignalTrigger() && abs(isSSLLEvent(ind1, ind2)) == 1){ // trigger && select loose mu/mu pair
		if(MuPt[ind1] > fC_minMu1pt && MuPt[ind2] > fC_minMu2pt){ // pt cuts

			// Fill histos
			KP0->hmetvsht->Fill(getHT(), pfMET, puweight);

			KP0->hvar[0]->Fill(getHT(),    puweight);
			KP0->hvar[1]->Fill(pfMET,      puweight);
			KP0->hvar[2]->Fill(getNJets(), puweight);
			KP0->hvar[3]->Fill(MuPt[ind1], puweight);
			KP0->hvar[4]->Fill(MuPt[ind2], puweight);
			TLorentzVector p1, p2;
			p1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
			p2.SetPtEtaPhiM(MuPt[ind2], MuEta[ind2], MuPhi[ind2], gMMU);
			float mass = (p1+p2).M();
			KP0->hvar[5]->Fill(mass, puweight); // SF
			KP0->hvar[6]->Fill(mass, puweight); // MM
			KP0->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);

			if(isTightMuon(ind1) && isTightMuon(ind2)){ // tight-tight
				KP1->hmetvsht->Fill(getHT(), pfMET, puweight);
				KP1->hvar[0]->Fill(getHT(),               puweight);
				KP1->hvar[1]->Fill(pfMET,                 puweight);
				KP1->hvar[2]->Fill(getNJets(),            puweight);
				KP1->hvar[3]->Fill(MuPt[ind1],            puweight);
				KP1->hvar[4]->Fill(MuPt[ind2],            puweight);
				KP1->hvar[5]->Fill(mass,                  puweight); // SF
				KP1->hvar[6]->Fill(mass,                  puweight); // MM
				KP1->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);
				if(isSSLLMuEvent(ind1, ind2)){ // signal region
					KP2->hmetvsht->Fill(getHT(), pfMET, puweight);
					if(hilo == HighPt){ // Store signal events
						fSigEv_HI_MM_HT .push_back(getHT());
						fSigEv_HI_MM_MET.push_back(pfMET);
					} else{
						fSigEv_LO_MM_HT .push_back(getHT());
						fSigEv_LO_MM_MET.push_back(pfMET);						
					}					

					KP2->hvar[0]->Fill(getHT(),    puweight);
					KP2->hvar[1]->Fill(pfMET,      puweight);
					KP2->hvar[2]->Fill(getNJets(), puweight);
					KP2->hvar[3]->Fill(MuPt[ind1], puweight);
					KP2->hvar[4]->Fill(MuPt[ind2], puweight);
					KP2->hvar[5]->Fill(mass,       puweight); // SF
					KP2->hvar[6]->Fill(mass,       puweight); // MM					
					KP2->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);
				}
			}
		}
		resetHypLeptons();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EE CHANNEL:  ////////////////////////////////////////////////////////////////////////////////////////
	else if(elelSignalTrigger() && abs(isSSLLEvent(ind1, ind2)) == 2){ // trigger && select loose e/e pair
		if(ElPt[ind1] > fC_minEl1pt && ElPt[ind2] > fC_minEl2pt){ // pt cuts
			// Fill histos
			KP0->hmetvsht->Fill(getHT(), pfMET, puweight);
			KP0->hvar[0]->Fill(getHT(),    puweight);
			KP0->hvar[1]->Fill(pfMET,      puweight);
			KP0->hvar[2]->Fill(getNJets(), puweight);
			KP0->hvar[3]->Fill(ElPt[ind1], puweight);
			KP0->hvar[4]->Fill(ElPt[ind2], puweight);
			TLorentzVector p1, p2;
			p1.SetPtEtaPhiM(ElPt[ind1], ElEta[ind1], ElPhi[ind1], gMEL);
			p2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);
			float mass = (p1+p2).M();
			KP0->hvar[5]->Fill(mass, puweight); // SF
			KP0->hvar[7]->Fill(mass, puweight); // MM
			KP0->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);
			if(isTightElectron(ind1) && isTightElectron(ind2)){ // tight-tight
				KP1->hmetvsht->Fill(getHT(), pfMET, puweight);
				KP1->hvar[0]->Fill(getHT(),               puweight);
				KP1->hvar[1]->Fill(pfMET,                 puweight);
				KP1->hvar[2]->Fill(getNJets(),            puweight);
				KP1->hvar[3]->Fill(ElPt[ind1],            puweight);
				KP1->hvar[4]->Fill(ElPt[ind2],            puweight);
				KP1->hvar[5]->Fill(mass,                  puweight); // SF
				KP1->hvar[7]->Fill(mass,                  puweight); // MM
				KP1->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);
				if(isSSLLElEvent(ind1, ind2)){
					KP2->hmetvsht->Fill(getHT(), pfMET, puweight);
					if(hilo == HighPt){ // Store signal events
						fSigEv_HI_EE_HT .push_back(getHT());
						fSigEv_HI_EE_MET.push_back(pfMET);
					} else{
						fSigEv_LO_EE_HT .push_back(getHT());
						fSigEv_LO_EE_MET.push_back(pfMET);						
					}
					KP2->hvar[0]->Fill(getHT(),               puweight);
					KP2->hvar[1]->Fill(pfMET,                 puweight);
					KP2->hvar[2]->Fill(getNJets(),            puweight);
					KP2->hvar[3]->Fill(ElPt[ind1],            puweight);
					KP2->hvar[4]->Fill(ElPt[ind2],            puweight);
					KP2->hvar[5]->Fill(mass,                  puweight); // SF
					KP2->hvar[7]->Fill(mass,                  puweight); // MM							
					KP2->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);
				}
			}
		}
		resetHypLeptons();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EMU CHANNEL:  ///////////////////////////////////////////////////////////////////////////////////////
	else if(elmuSignalTrigger() && abs(isSSLLEvent(mu, el)) == 3){ // trigger && select loose e/mu pair
		if( (MuPt[mu] > fC_minMu1pt && ElPt[el] > fC_minEl2pt) || (MuPt[mu] > fC_minMu2pt && ElPt[el] > fC_minEl1pt) ){
			// Fill histos
			KP0->hmetvsht->Fill(getHT(), pfMET, puweight);
			KP0->hvar[0]->Fill(getHT(),    puweight);
			KP0->hvar[1]->Fill(pfMET,      puweight);
			KP0->hvar[2]->Fill(getNJets(), puweight);
			float ptmax = MuPt[mu];
			float ptmin = ElPt[el];
			if(ptmin > ptmax){
				ptmin = MuPt[mu];
				ptmax = ElPt[el];
			}
			KP0->hvar[3]->Fill(ptmax, puweight);
			KP0->hvar[4]->Fill(ptmin, puweight);
			TLorentzVector p1, p2;
			p1.SetPtEtaPhiM(MuPt[mu], MuEta[mu], MuPhi[mu], gMMU);
			p2.SetPtEtaPhiM(ElPt[el], ElEta[el], ElPhi[el], gMEL);
			float mass = (p1+p2).M();
			KP0->hvar[8]->Fill(mass,                  puweight); // EM
			KP0->hvar[9]->Fill(getMT2(mu, el, 3), puweight);
			if(isTightMuon(mu) && isTightElectron(el)){ // tight-tight
				KP1->hmetvsht->Fill(getHT(), pfMET, puweight);
				KP1->hvar[0]->Fill(getHT(),               puweight);
				KP1->hvar[1]->Fill(pfMET,                 puweight);
				KP1->hvar[2]->Fill(getNJets(),            puweight);
				KP1->hvar[3]->Fill(ptmax,                 puweight);
				KP1->hvar[4]->Fill(ptmin,                 puweight);
				KP1->hvar[8]->Fill(mass,                  puweight); // EM
				KP1->hvar[9]->Fill(getMT2(mu, el, 3), puweight);
				if( isSSLLElMuEvent(mu, el) ){
					KP2->hmetvsht->Fill(getHT(), pfMET, puweight);
					if(hilo == HighPt){ // Store signal events
						fSigEv_HI_EM_HT .push_back(getHT());
						fSigEv_HI_EM_MET.push_back(pfMET);
					} else{
						fSigEv_LO_EM_HT .push_back(getHT());
						fSigEv_LO_EM_MET.push_back(pfMET);						
					}
					KP2->hvar[0]->Fill(getHT(),               puweight);
					KP2->hvar[1]->Fill(pfMET,                 puweight);
					KP2->hvar[2]->Fill(getNJets(),            puweight);
					KP2->hvar[3]->Fill(ptmax,                 puweight);
					KP2->hvar[4]->Fill(ptmin,                 puweight);
					KP2->hvar[8]->Fill(mass,                  puweight); // EM						
					KP2->hvar[9]->Fill(getMT2(mu, el, 3), puweight);
				}
			}
		}
	}
	resetHypLeptons();
	return;
}
void MuonPlotter::fillMuIsoPlots(gSample i){
	resetHypLeptons();
	fDoCounting = false;
	Sample *S = fSamples[i];
	IsoPlots *IP0 = &S->isoplots[0]; // mu

	// Reset event selections to baseline:
	fC_minMu1pt = Region::minMu1pt[HighPt];
	fC_minMu2pt = Region::minMu2pt[HighPt];
	fC_minEl1pt = Region::minEl1pt[HighPt];
	fC_minEl2pt = Region::minEl2pt[HighPt];

	fC_minHT    = Region::minHT   [Baseline];
	fC_maxHT    = Region::maxHT   [Baseline];
	fC_minMet   = Region::minMet  [Baseline];
	fC_maxMet   = Region::maxMet  [Baseline];
	fC_minNjets = Region::minNjets[Baseline];

	int muind1(-1), muind2(-1);
	if(hasLooseMuons(muind1, muind2) > 0){
		setHypLepton1(muind1, Muon);
		// Common trigger selection
		if(!singleMuTrigger()) return;
		float prescale = singleMuPrescale();
		float puweight = PUWeight;
		if(S->datamc == 4) puweight = 1; // fix for samples with no pileup
		float scale = prescale * puweight;

		// Common event selections
		if(!passesJet50Cut()) return; // make trigger 100% efficient

		// Common object selections
		if(!isLooseMuon(muind1)) return;
		if(MuPt[muind1] < fC_minMu2pt) return;
		if(MuPt[muind1] > gMuPt2bins[gNMuPt2bins]) return;

		////////////////////////////////////////////////////
		// MOST LOOSE SELECTION
		IP0->hiso[0]->Fill(MuIso[muind1], scale);
		for(size_t k = 0; k < gNMuPt2bins; ++k){
			if(MuPt[muind1] < gMuPt2bins[k]) continue;
			if(MuPt[muind1] > gMuPt2bins[k+1]) continue;
			IP0->hiso_pt[0][k]->Fill(MuIso[muind1], scale);
		}
		for(size_t k = 0; k < gNNVrtxBins; ++k){
			if(NVrtx <  gNVrtxBins[k]) continue;
			if(NVrtx >= gNVrtxBins[k+1]) continue;
			IP0->hiso_nv[0][k]->Fill(MuIso[muind1], scale);
		}

		////////////////////////////////////////////////////
		// SIGNAL SUPPRESSED SELECTION
		if(isSigSupMuEvent()){
			IP0->hiso[1]->Fill(MuIso[muind1], scale);
			for(size_t k = 0; k < gNMuPt2bins; ++k){
				if(MuPt[muind1] < gMuPt2bins[k]) continue;
				if(MuPt[muind1] > gMuPt2bins[k+1]) continue;
				IP0->hiso_pt[1][k]->Fill(MuIso[muind1], scale);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				if(NVrtx <  gNVrtxBins[k]) continue;
				if(NVrtx >= gNVrtxBins[k+1]) continue;
				IP0->hiso_nv[1][k]->Fill(MuIso[muind1], scale);
			}
		}
		resetHypLeptons();
		////////////////////////////////////////////////////
	}
	return;
}
void MuonPlotter::fillElIsoPlots(gSample i){
	resetHypLeptons();
	fDoCounting = false;
	Sample *S = fSamples[i];
	IsoPlots *IP = &S->isoplots[1]; // mu

	// Reset event selections to baseline:
	fC_minMu1pt = Region::minMu1pt[HighPt];
	fC_minMu2pt = Region::minMu2pt[HighPt];
	fC_minEl1pt = Region::minEl1pt[HighPt];
	fC_minEl2pt = Region::minEl2pt[HighPt];

	fC_minHT    = Region::minHT   [Baseline];
	fC_maxHT    = Region::maxHT   [Baseline];
	fC_minMet   = Region::minMet  [Baseline];
	fC_maxMet   = Region::maxMet  [Baseline];
	fC_minNjets = Region::minNjets[Baseline];

	int elind1(-1), elind2(-1);
	if(hasLooseElectrons(elind1, elind2) > 0){
		setHypLepton1(elind1, Elec);
		// Common trigger selection
		if(!singleElTrigger()) return;
		float prescale = singleElPrescale();
		float puweight = PUWeight;
		if(S->datamc == 4) puweight = 1; // fix for samples with no pileup
		float scale = prescale * puweight;

		// Common event selections
		if(!passesJet50Cut()) return; // make trigger 100% efficient

		// Common object selections
		if(!isLooseElectron(elind1)) return;
		// if(ElIsGoodElId_WP80[elind1] != 1) return false; // apply tight ID for the iso plots?

		if(ElPt[elind1] < fC_minEl2pt) return;
		if(ElPt[elind1] > gElPt2bins[gNElPt2bins]) return;

		////////////////////////////////////////////////////
		// MOST LOOSE SELECTION
		IP->hiso[0]->Fill(ElRelIso[elind1], scale);
		for(size_t k = 0; k < gNElPt2bins; ++k){
			if(ElPt[elind1] < gElPt2bins[k]) continue;
			if(ElPt[elind1] > gElPt2bins[k+1]) continue;
			IP->hiso_pt[0][k]->Fill(ElRelIso[elind1], scale);
		}
		for(size_t k = 0; k < gNNVrtxBins; ++k){
			if(NVrtx <  gNVrtxBins[k]) continue;
			if(NVrtx >= gNVrtxBins[k+1]) continue;
			IP->hiso_nv[0][k]->Fill(ElRelIso[elind1], scale);
		}

		////////////////////////////////////////////////////
		// SIGNAL SUPPRESSED SELECTION
		if(isSigSupElEvent()){
			IP->hiso[1]->Fill(ElRelIso[elind1], scale);
			for(size_t k = 0; k < gNElPt2bins; ++k){
				if(ElPt[elind1] < gElPt2bins[k]) continue;
				if(ElPt[elind1] > gElPt2bins[k+1]) continue;
				IP->hiso_pt[1][k]->Fill(ElRelIso[elind1], scale);
			}
			for(size_t k = 0; k < gNNVrtxBins; ++k){
				if(NVrtx <  gNVrtxBins[k]) continue;
				if(NVrtx >= gNVrtxBins[k+1]) continue;
				IP->hiso_nv[1][k]->Fill(ElRelIso[elind1], scale);
			}
		}
		////////////////////////////////////////////////////
		resetHypLeptons();
	}
	return;
}

//____________________________________________________________________________
void MuonPlotter::storeNumbers(Sample *S, gChannel chan, gRegion reg){
	Channel *C;
	if(chan == Muon)     C = &S->region[reg][HighPt].mm;
	if(chan == Elec) C = &S->region[reg][HighPt].ee;
	if(chan == ElMu)      C = &S->region[reg][HighPt].em;
	S->numbers[reg][chan].nt2  = C->nt20_pt->GetEntries();
	S->numbers[reg][chan].nt10 = C->nt10_pt->GetEntries();
	S->numbers[reg][chan].nt01 = C->nt01_pt->GetEntries();
	S->numbers[reg][chan].nt0  = C->nt00_pt->GetEntries();
	if(chan != ElMu){
		S->numbers[reg][chan].nsst = C->fntight->GetEntries();
		S->numbers[reg][chan].nssl = C->fnloose->GetEntries();
		S->numbers[reg][chan].nzt  = C->pntight->GetEntries();
		S->numbers[reg][chan].nzl  = C->pnloose->GetEntries();
	}
}

//____________________________________________________________________________
void MuonPlotter::initCutNames(){
	// Muon channel
	fMMCutNames.push_back("All events"); //                            = 0
	fMMCutNames.push_back(" ... is good run"); //                      = 1
	fMMCutNames.push_back(" ... passes triggers"); //                  = 2
	fMMCutNames.push_back(" ... has 1 loose muon"); //                 = 3
	fMMCutNames.push_back(" ... has 2 loose muons"); //                = 4
	fMMCutNames.push_back(" ... has same-sign muons"); //              = 5
	fMMCutNames.push_back(" ... passes Z veto"); //                    = 6
	fMMCutNames.push_back(" ... passes Minv veto"); //                 = 7
	fMMCutNames.push_back(" ... has one jet > 50 GeV"); //             = 8
	fMMCutNames.push_back(" ... passes NJets cut"); //                 = 9
	fMMCutNames.push_back(" ... passes HT cut"); //                    = 10
	fMMCutNames.push_back(" ... passes MET cut"); //                   = 11
	fMMCutNames.push_back(" ... second muon passes pt cut"); //        = 12
	fMMCutNames.push_back(" ... first muon passes pt cut"); //         = 13
	fMMCutNames.push_back(" ... first muon passes tight cut"); //      = 14
	fMMCutNames.push_back(" ... second muon passes tight cut"); //     = 15
	fMMCutNames.push_back(" ... both muons pass tight cut"); //        = 16

	// Electron channel
	fEECutNames.push_back("All events"); //                            = 0
	fEECutNames.push_back(" ... is good run"); //                      = 1
	fEECutNames.push_back(" ... passes triggers"); //                  = 2
	fEECutNames.push_back(" ... has 1 loose electron"); //             = 3
	fEECutNames.push_back(" ... has 2 loose electrons"); //            = 4
	fEECutNames.push_back(" ... has same-sign electrons"); //          = 5
	fEECutNames.push_back(" ... passes Z veto"); //                    = 6
	fEECutNames.push_back(" ... passes Minv veto"); //                 = 7
	fEECutNames.push_back(" ... has one jet > 50 GeV"); //             = 8
	fEECutNames.push_back(" ... passes NJets cut"); //                 = 9
	fEECutNames.push_back(" ... passes HT cut"); //                    = 10
	fEECutNames.push_back(" ... passes MET cut"); //                   = 11
	fEECutNames.push_back(" ... second electron passes pt cut"); //    = 12
	fEECutNames.push_back(" ... first electron passes pt cut"); //     = 13
	fEECutNames.push_back(" ... first electron passes tight cut"); //  = 14
	fEECutNames.push_back(" ... second electron passes tight cut"); // = 15
	fEECutNames.push_back(" ... both electrons pass tight cut"); //    = 16

	// E-Mu channel
	fEMCutNames.push_back("All events"); //                            = 0
	fEMCutNames.push_back(" ... is good run"); //                      = 1
	fEMCutNames.push_back(" ... passes triggers"); //                  = 2
	fEMCutNames.push_back(" ... has a loose muon"); //                 = 3
	fEMCutNames.push_back(" ... has a loose electron"); //             = 4
	fEMCutNames.push_back(" ... has both"); //                         = 5
	fEMCutNames.push_back(" ... has same-sign electron muon pair"); // = 6
	fEMCutNames.push_back(" ... passes Z veto"); //                    = 7
	fEMCutNames.push_back(" ... has one jet > 50 GeV"); //             = 8
	fEMCutNames.push_back(" ... passes NJets cut"); //                 = 9
	fEMCutNames.push_back(" ... passes HT cut"); //                    = 10
	fEMCutNames.push_back(" ... passes MET cut"); //                   = 11
	fEMCutNames.push_back(" ... muon passes pt cut"); //               = 12
	fEMCutNames.push_back(" ... electron passes pt cut"); //           = 13
	fEMCutNames.push_back(" ... muon passes tight cut"); //            = 14
	fEMCutNames.push_back(" ... electron passes tight cut"); //        = 15
	fEMCutNames.push_back(" ... both e and mu pass tight cuts"); //    = 16
}
void MuonPlotter::initCounters(gSample sample){
	fCounters[sample][Muon].fill(fMMCutNames[0],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[1],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[2],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[3],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[4],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[5],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[6],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[7],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[8],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[9],  0.);
	fCounters[sample][Muon].fill(fMMCutNames[10], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[11], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[12], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[13], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[14], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[15], 0.);
	fCounters[sample][Muon].fill(fMMCutNames[16], 0.);

	fCounters[sample][Elec].fill(fEECutNames[0],  0.);
	fCounters[sample][Elec].fill(fEECutNames[1],  0.);
	fCounters[sample][Elec].fill(fEECutNames[2],  0.);
	fCounters[sample][Elec].fill(fEECutNames[3],  0.);
	fCounters[sample][Elec].fill(fEECutNames[4],  0.);
	fCounters[sample][Elec].fill(fEECutNames[5],  0.);
	fCounters[sample][Elec].fill(fEECutNames[6],  0.);
	fCounters[sample][Elec].fill(fEECutNames[7],  0.);
	fCounters[sample][Elec].fill(fEECutNames[8],  0.);
	fCounters[sample][Elec].fill(fEECutNames[9],  0.);
	fCounters[sample][Elec].fill(fEECutNames[10], 0.);
	fCounters[sample][Elec].fill(fEECutNames[11], 0.);
	fCounters[sample][Elec].fill(fEECutNames[12], 0.);
	fCounters[sample][Elec].fill(fEECutNames[13], 0.);
	fCounters[sample][Elec].fill(fEECutNames[14], 0.);
	fCounters[sample][Elec].fill(fEECutNames[15], 0.);
	fCounters[sample][Elec].fill(fEECutNames[16], 0.);

	fCounters[sample][ElMu].fill(fEMCutNames[0],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[1],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[2],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[3],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[4],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[5],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[6],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[7],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[8],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[9],  0.);
	fCounters[sample][ElMu].fill(fEMCutNames[10], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[11], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[12], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[13], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[14], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[15], 0.);
	fCounters[sample][ElMu].fill(fEMCutNames[16], 0.);
}
void MuonPlotter::fillCutFlowHistos(gSample sample){
	Sample *S = fSamples[sample];

	for(int i=0; i<fMMCutNames.size(); i++) S->cutFlowHisto[Muon]->SetBinContent(i+1, fCounters[sample][Muon].counts(fMMCutNames[i]));
	for(int i=0; i<fEECutNames.size(); i++) S->cutFlowHisto[Elec]->SetBinContent(i+1, fCounters[sample][Elec].counts(fEECutNames[i]));
	for(int i=0; i<fEMCutNames.size(); i++) S->cutFlowHisto[ElMu]->SetBinContent(i+1, fCounters[sample][ElMu].counts(fEMCutNames[i]));	
}
void MuonPlotter::printCutFlow(gChannel chan, gSample indmin, gSample indmax){
	vector<string> names;
	if(chan == Muon) names = fMMCutNames;
	if(chan == Elec) names = fEECutNames;
	if(chan == ElMu) names = fEMCutNames;

	fOUTSTREAM << "------------------------------------------";
	for(int i = 0; i <= indmax-indmin; i++) fOUTSTREAM << "--------------";
	fOUTSTREAM << endl;

	fOUTSTREAM << " Cutname                                 | ";
	for(gSample i = indmin; i <= indmax; i=gSample(i+1)) fOUTSTREAM << setw(11) << fSamples[i]->sname << " | ";
	fOUTSTREAM << endl;

	fOUTSTREAM << "------------------------------------------";
	for(int i = 0; i <= indmax-indmin; i++) fOUTSTREAM << "--------------";
	fOUTSTREAM << endl;

	for( int c = 0; c < fMMCutNames.size(); c++ ){
		fOUTSTREAM << setw(40) << names[c] << " | ";

		for(gSample i = indmin; i <= indmax; i=gSample(i+1)){
			fOUTSTREAM << setw(11) << setprecision(11) << fSamples[i]->cutFlowHisto[chan]->GetBinContent(c+1) << " | ";
		}
		fOUTSTREAM << endl;
	}
	fOUTSTREAM << "------------------------------------------";
	for(int i = 0; i <= indmax-indmin; i++) fOUTSTREAM << "--------------";
	fOUTSTREAM << endl;	
}
void MuonPlotter::printCutFlows(TString filename){
	fOUTSTREAM.open(filename.Data(), ios::trunc);
	fOUTSTREAM << " Printing Cutflow for Mu/Mu channel..." << endl;
	// printCutFlow(Muon, DoubleMu1, EleHad2);
	printCutFlow(Muon, DoubleMu1, DoubleMu4);
	printCutFlow(Muon, TTJets, GJets200);
	printCutFlow(Muon, GVJets, ttbarW);
	printCutFlow(Muon, LM0, LM6);
	printCutFlow(Muon, LM7, LM13);
	printCutFlow(Muon, QCDMuEnr10, QCD470);
	printCutFlow(Muon, QCD600, QCD1000MG);
	fOUTSTREAM << endl << endl;

	fOUTSTREAM << " Printing Cutflow for E/Mu channel..." << endl;
	printCutFlow(ElMu, MuEG1, MuEG4);
	printCutFlow(ElMu, TTJets, GJets200);
	printCutFlow(ElMu, GVJets, ttbarW);
	printCutFlow(ElMu, LM0, LM6);
	printCutFlow(ElMu, LM7, LM13);
	printCutFlow(ElMu, QCDMuEnr10, QCD470);
	printCutFlow(ElMu, QCD600, QCD1000MG);
	fOUTSTREAM << endl << endl;

	fOUTSTREAM << " Printing Cutflow for E/E channel..." << endl;
	printCutFlow(Elec, DoubleEle1, DoubleEle4);
	printCutFlow(Elec, TTJets, GJets200);
	printCutFlow(Elec, GVJets, ttbarW);
	printCutFlow(Elec, LM0, LM6);
	printCutFlow(Elec, LM7, LM13);
	printCutFlow(Elec, QCDMuEnr10, QCD470);
	printCutFlow(Elec, QCD600, QCD1000MG);
	fOUTSTREAM << endl << endl;

	fOUTSTREAM.close();
}

//____________________________________________________________________________
void MuonPlotter::printYields(gChannel chan, float lumiscale){
	cout << setfill('-') << setw(97) << "-" << endl;
	TString name = "Mu/Mu";
	if(chan == Elec) name = "E/E";
	if(chan == ElMu) name = "E/Mu";
	cout << " Printing yields for " << name << " channel..." << endl;
	if(lumiscale > -1.0) cout << " Numbers scaled to " << lumiscale << " /pb" << endl;
	cout << "           Name |   Nt2   |   Nt10  |   Nt01  |   Nt0   |   Nsst  |   Nssl  |   NZ t  |   NZ l  |" << endl;
	cout << setfill('-') << setw(97) << "-" << endl;
	cout << setfill(' ');
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		NumberSet numbers = S->numbers[Baseline][chan];
		float scale = lumiscale / S->lumi;
		if(S->datamc == 0 || scale < 0) scale = 1;
		cout << setw(15) << S->sname << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nt2  << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nt10 << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nt01 << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nt0  << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nsst << " |"; 
		cout << setw(8)  << setprecision(3) << scale*numbers.nssl << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nzt  << " |";
		cout << setw(8)  << setprecision(3) << scale*numbers.nzl  << " |";
		cout << endl;
	}

	cout << setfill('-') << setw(97) << "-" << endl;
}
void MuonPlotter::printYieldsShort(float luminorm){
	vector<int> musamples;
	vector<int> elsamples;
	vector<int> emusamples;

	cout << "---------------------------------------------------" << endl;
	cout << "Printing yields" << endl;
	musamples = fMuData;
	elsamples = fEGData;
	emusamples = fMuEGData;

	float nt20[gNCHANNELS],    nt10[gNCHANNELS],    nt01[gNCHANNELS],    nt00[gNCHANNELS];

	// float nt2_mumu(0.),    nt10_mumu(0.),    nt0_mumu(0.);
	// float nt2_emu(0.),    nt10_emu(0.),    nt01_emu(0.),    nt0_emu(0.);
	// float nt2_ee(0.),    nt10_ee(0.),    nt0_ee(0.);

	for(size_t i = 0; i < musamples.size(); ++i){
		Sample *S = fSamples[musamples[i]];
		nt20[Muon]    += S->numbers[Baseline][Muon].nt2;
		nt10[Muon]    += S->numbers[Baseline][Muon].nt10;
		nt00[Muon]    += S->numbers[Baseline][Muon].nt0;
	}

	for(size_t i = 0; i < emusamples.size(); ++i){
		Sample *S = fSamples[emusamples[i]];
		nt20[ElMu]    += S->numbers[Baseline][ElMu].nt2;
		nt10[ElMu]    += S->numbers[Baseline][ElMu].nt10;
		nt01[ElMu]    += S->numbers[Baseline][ElMu].nt01;
		nt00[ElMu]    += S->numbers[Baseline][ElMu].nt0;
	}		

	for(size_t i = 0; i < elsamples.size(); ++i){
		Sample *S = fSamples[elsamples[i]];
		nt20[Elec]    += S->numbers[Baseline][Elec].nt2;
		nt10[Elec]    += S->numbers[Baseline][Elec].nt10;
		nt00[Elec]    += S->numbers[Baseline][Elec].nt0;
	}		

	// for(size_t i = 0; i < musamples.size(); ++i){
	// 	int index = musamples[i];
	// 	Channel *mumu = fSamples[index]->mm;
	// 	nt2_mumu  += mumu->numbers.nt2;
	// 	nt10_mumu += mumu->numbers.nt10;
	// 	nt0_mumu  += mumu->numbers.nt0;
	// 	nt2_mumu_e2  += mumu->numbers.nt2;
	// 	nt10_mumu_e2 += mumu->numbers.nt10;
	// 	nt0_mumu_e2  += mumu->numbers.nt0;
	// }		
	// for(size_t i = 0; i < emusamples.size(); ++i){
	// 	int index = emusamples[i];
	// 	Channel *emu = fSamples[index]->em;
	// 	nt2_emu  += emu->numbers.nt2;
	// 	nt10_emu += emu->numbers.nt10;
	// 	nt01_emu += emu->numbers.nt01;
	// 	nt0_emu  += emu->numbers.nt0;
	// 	nt2_emu_e2  += emu->numbers.nt2;
	// 	nt10_emu_e2 += emu->numbers.nt10;
	// 	nt01_emu_e2 += emu->numbers.nt01;
	// 	nt0_emu_e2  += emu->numbers.nt0;
	// }		
	// for(size_t i = 0; i < elsamples.size(); ++i){
	// 	int index = elsamples[i];
	// 	Channel *ee = fSamples[index]->ee;
	// 	nt2_ee  += ee->numbers.nt2;
	// 	nt10_ee += ee->numbers.nt10;
	// 	nt0_ee  += ee->numbers.nt0;
	// 	nt2_ee_e2  += ee->numbers.nt2;
	// 	nt10_ee_e2 += ee->numbers.nt10;
	// 	nt0_ee_e2  += ee->numbers.nt0;
	// }
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	cout << "          ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	cout << "  YIELDS  ||   Nt2   |   Nt1   |   Nt0   ||   Nt2   |   Nt10  |   Nt01  |   Nt0   ||   Nt2   |   Nt1   |   Nt0   ||" << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;

	float nt2sum_mumu(0.), nt10sum_mumu(0.), nt0sum_mumu(0.);
	float nt2sum_emu(0.), nt10sum_emu(0.), nt01sum_emu(0.), nt0sum_emu(0.);
	float nt2sum_ee(0.), nt10sum_ee(0.), nt0sum_ee(0.);
	for(size_t i = 0; i < fMCBG.size(); ++i){
		int index = fMCBG[i];
		Sample *S = fSamples[index];
		float scale = luminorm / S->lumi;
		if(luminorm < 0) scale = 1;
		nt2sum_mumu  += scale*S->numbers[Baseline][Muon]    .nt2;
		nt10sum_mumu += scale*S->numbers[Baseline][Muon]    .nt10;
		nt0sum_mumu  += scale*S->numbers[Baseline][Muon]    .nt0;
		nt2sum_emu   += scale*S->numbers[Baseline][ElMu]     .nt2;
		nt10sum_emu  += scale*S->numbers[Baseline][ElMu]     .nt10;
		nt01sum_emu  += scale*S->numbers[Baseline][ElMu]     .nt01;
		nt0sum_emu   += scale*S->numbers[Baseline][ElMu]     .nt0;
		nt2sum_ee    += scale*S->numbers[Baseline][Elec].nt2;
		nt10sum_ee   += scale*S->numbers[Baseline][Elec].nt10;
		nt0sum_ee    += scale*S->numbers[Baseline][Elec].nt0;

		cout << setw(9) << S->sname << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt0  << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][ElMu]     .nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][ElMu]     .nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][ElMu]     .nt01 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][ElMu]     .nt0  << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][Elec].nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Elec].nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Elec].nt0  << " || ";
		cout << endl;
	}
	if(luminorm > 0){
		cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
		cout << setw(9) << "MC sum" << " || ";
		cout << setw(7) << nt2sum_mumu  << " | ";
		cout << setw(7) << nt10sum_mumu << " | ";
		cout << setw(7) << nt0sum_mumu  << " || ";
		cout << setw(7) << nt2sum_emu   << " | ";
		cout << setw(7) << nt10sum_emu  << " | ";
		cout << setw(7) << nt01sum_emu  << " | ";
		cout << setw(7) << nt0sum_emu   << " || ";
		cout << setw(7) << nt2sum_ee    << " | ";
		cout << setw(7) << nt10sum_ee   << " | ";
		cout << setw(7) << nt0sum_ee    << " || ";
		cout << endl;
	}
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(9) << fSamples[LM0]->sname << " || ";
	float scale = luminorm / fSamples[LM0]->lumi;
	if(luminorm < 0) scale = 1;
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Muon]    .nt2  << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Muon]    .nt10 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Muon]    .nt0  << " || ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][ElMu]     .nt2  << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][ElMu]     .nt10 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][ElMu]     .nt01 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][ElMu]     .nt0  << " || ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Elec].nt2  << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Elec].nt10 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Elec].nt0  << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(9) << "data"  << " || ";
	cout << setw(7) << nt20[Muon]     << " | ";
	cout << setw(7) << nt10[Muon]     << " | ";
	cout << setw(7) << nt00[Muon]     << " || ";
	cout << setw(7) << nt20[ElMu]      << " | ";
	cout << setw(7) << nt10[ElMu]      << " | ";
	cout << setw(7) << nt01[ElMu]      << " | ";
	cout << setw(7) << nt00[ElMu]      << " || ";
	cout << setw(7) << nt20[Elec] << " | ";
	cout << setw(7) << nt10[Elec] << " | ";
	cout << setw(7) << nt00[Elec] << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;

}

//____________________________________________________________________________
void MuonPlotter::bookHistos(Sample *S){
	// Cut flow histos
	S->cutFlowHisto[Muon] = new TH1D("MMCutFlow", "MMCutFlow", fMMCutNames.size(), 0, fMMCutNames.size());
	S->cutFlowHisto[Elec] = new TH1D("EECutFlow", "EECutFlow", fEECutNames.size(), 0, fEECutNames.size());
	S->cutFlowHisto[ElMu] = new TH1D("EMCutFlow", "EMCutFlow", fEMCutNames.size(), 0, fEMCutNames.size());
	for(int i=0; i<fMMCutNames.size(); i++) S->cutFlowHisto[Muon]->GetXaxis()->SetBinLabel(i+1, TString(fMMCutNames[i]));
	for(int i=0; i<fEECutNames.size(); i++) S->cutFlowHisto[Elec]->GetXaxis()->SetBinLabel(i+1, TString(fEECutNames[i]));
	for(int i=0; i<fEMCutNames.size(); i++) S->cutFlowHisto[ElMu]->GetXaxis()->SetBinLabel(i+1, TString(fEMCutNames[i]));	

	// Histos for differential yields
	for(size_t k = 0; k < gNCHANNELS; ++k){
		TString name;
		for(size_t j = 0; j < gNDiffVars; ++j){
			name = Form("%s_%s_NT11_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt11[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt11[j]->SetFillColor(S->color);
			S->diffyields[k].hnt11[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt11[j]->Sumw2();
			name = Form("%s_%s_NT10_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt10[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt10[j]->SetFillColor(S->color);
			S->diffyields[k].hnt10[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt10[j]->Sumw2();
			name = Form("%s_%s_NT01_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt01[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt01[j]->SetFillColor(S->color);
			S->diffyields[k].hnt01[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt01[j]->Sumw2();
			name = Form("%s_%s_NT00_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt00[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt00[j]->SetFillColor(S->color);
			S->diffyields[k].hnt00[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt00[j]->Sumw2();

			if(k == Muon) continue;
			name = Form("%s_%s_NT11_OS_BB_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt2_os_BB[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt2_os_BB[j]->SetFillColor(S->color);
			S->diffyields[k].hnt2_os_BB[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt2_os_BB[j]->Sumw2();
			name = Form("%s_%s_NT11_OS_EE_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt2_os_EE[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt2_os_EE[j]->SetFillColor(S->color);
			S->diffyields[k].hnt2_os_EE[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt2_os_EE[j]->Sumw2();
			if(k == ElMu) continue;
			name = Form("%s_%s_NT11_OS_EB_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
			S->diffyields[k].hnt2_os_EB[j] = new TH1D(name, DiffPredYields::var_name[j].Data(), DiffPredYields::nbins[j], DiffPredYields::bins[j]);
			S->diffyields[k].hnt2_os_EB[j]->SetFillColor(S->color);
			S->diffyields[k].hnt2_os_EB[j]->SetXTitle(DiffPredYields::axis_label[j]);
			S->diffyields[k].hnt2_os_EB[j]->Sumw2();
		}
	}	

	// Kinematical histos
	for(size_t hilo = 0; hilo < 2; ++hilo){
		for(size_t k = 0; k < gNKinSels; ++k){
			TString name = Form("%s_%s_%s_HTvsMET", S->sname.Data(), gKinSelNames[k].Data(), gHiLoLabel[hilo].Data());
			S->kinplots[k][hilo].hmetvsht = new TH2D(name, "HTvsMET", KinPlots::nHTBins, KinPlots::HTmin, KinPlots::HTmax, KinPlots::nMETBins, KinPlots::METmin, KinPlots::METmax);
			for(size_t j = 0; j < gNKinVars; ++j){
				name = Form("%s_%s_%s_%s", S->sname.Data(), gKinSelNames[k].Data(), gHiLoLabel[hilo].Data(), KinPlots::var_name[j].Data());
				S->kinplots[k][hilo].hvar[j] = new TH1D(name, KinPlots::var_name[j].Data(), KinPlots::nbins[j], KinPlots::xmin[j], KinPlots::xmax[j]);
				S->kinplots[k][hilo].hvar[j]->SetFillColor(S->color);
				S->kinplots[k][hilo].hvar[j]->SetXTitle(KinPlots::axis_label[j]);
				S->kinplots[k][hilo].hvar[j]->Sumw2();
			}
		}
	}

	for(size_t l = 0; l < 2; ++l){
		// Isolation histos
		for(size_t j = 0; j < gNSels; ++j){
			TString name = Form("%s_%s_%siso", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[l].Data());
			S->isoplots[l].hiso[j] = new TH1D(name, Form("%siso", gEMULabel[l].Data()), IsoPlots::nbins[j], 0., 1.);
			S->isoplots[l].hiso[j]->SetFillColor(S->color);
			S->isoplots[l].hiso[j]->SetXTitle("RelIso");
			S->isoplots[l].hiso[j]->Sumw2();
			for(int k = 0; k < gNMuPt2bins; ++k){
				name = Form("%s_%s_%siso_pt%d", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[l].Data(), k);
				S->isoplots[l].hiso_pt[j][k] = new TH1D(name, Form("%siso_pt%d", gEMULabel[l].Data(), k), IsoPlots::nbins[j], 0., 1.);
				S->isoplots[l].hiso_pt[j][k]->SetFillColor(S->color);
				S->isoplots[l].hiso_pt[j][k]->SetXTitle("RelIso");
				S->isoplots[l].hiso_pt[j][k]->Sumw2();
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				name = Form("%s_%s_%siso_nv%d", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[l].Data(), k);
				S->isoplots[l].hiso_nv[j][k] = new TH1D(name, Form("%siso_nv%d", gEMULabel[l].Data(), k), IsoPlots::nbins[j], 0., 1.);
				S->isoplots[l].hiso_nv[j][k]->SetFillColor(S->color);
				S->isoplots[l].hiso_nv[j][k]->SetXTitle("RelIso");
				S->isoplots[l].hiso_nv[j][k]->Sumw2();
			}
		}

		// Ratio histos
		for(size_t j = 0; j < gNRatioVars; ++j){
			TString name = Form("%s_%s_ntight_%s", S->sname.Data(), gEMULabel[l].Data(), FRatioPlots::var_name[j].Data());
			S->ratioplots[l].ntight[j] = new TH1D(name, "ntight", FRatioPlots::nbins[j], FRatioPlots::xmin[j], FRatioPlots::xmax[j]);
			S->ratioplots[l].ntight[j]->SetFillColor(S->color);
			S->ratioplots[l].ntight[j]->SetXTitle(FRatioPlots::var_name[j]);
			S->ratioplots[l].ntight[j]->Sumw2();
			name = Form("%s_%s_nloose_%s", S->sname.Data(), gEMULabel[l].Data(), FRatioPlots::var_name[j].Data());
			S->ratioplots[l].nloose[j] = new TH1D(name, "nloose", FRatioPlots::nbins[j], FRatioPlots::xmin[j], FRatioPlots::xmax[j]);
			S->ratioplots[l].nloose[j]->SetFillColor(S->color);
			S->ratioplots[l].nloose[j]->SetXTitle(FRatioPlots::var_name[j]);
			S->ratioplots[l].nloose[j]->Sumw2();
		}
	}

	for(size_t hilo = 0; hilo < 2; ++hilo){
		for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
			Region *R = &S->region[r][hilo];
			for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
				Channel *C;
				if(c == Muon) C = &R->mm;
				if(c == Elec) C = &R->ee;
				if(c == ElMu) C = &R->em;
				TString rootname = S->sname + "_" + Region::sname[r] + "_" + C->sname + "_" + gHiLoLabel[hilo];
				// Yields common for all channels and data-mc:
				C->nt20_pt  = new TH2D(rootname + "_NT20_pt",  "NT20_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_pt ->Sumw2();
				C->nt10_pt  = new TH2D(rootname + "_NT10_pt",  "NT10_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt10_pt ->Sumw2();
				C->nt01_pt  = new TH2D(rootname + "_NT01_pt",  "NT01_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt01_pt ->Sumw2();
				C->nt00_pt  = new TH2D(rootname + "_NT00_pt",  "NT00_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt00_pt ->Sumw2();
				C->nt20_eta = new TH2D(rootname + "_NT20_eta", "NT20_eta", getNEtaBins(c), getEtaBins(c), getNEtaBins(c), getEtaBins(c)); C->nt20_eta->Sumw2();
				C->nt10_eta = new TH2D(rootname + "_NT10_eta", "NT10_eta", getNEtaBins(c), getEtaBins(c), getNEtaBins(c), getEtaBins(c)); C->nt10_eta->Sumw2();
				C->nt01_eta = new TH2D(rootname + "_NT01_eta", "NT01_eta", getNEtaBins(c), getEtaBins(c), getNEtaBins(c), getEtaBins(c)); C->nt01_eta->Sumw2();
				C->nt00_eta = new TH2D(rootname + "_NT00_eta", "NT00_eta", getNEtaBins(c), getEtaBins(c), getNEtaBins(c), getEtaBins(c)); C->nt00_eta->Sumw2();

				// MC truth info
				if(S->datamc > 0){
					C->npp_pt   = new TH2D(rootname + "_NPP_pt",   "NPP_pt",   getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->npp_pt->Sumw2();
					C->nfp_pt   = new TH2D(rootname + "_NFP_pt",   "NFP_pt",   getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nfp_pt->Sumw2();
					C->npf_pt   = new TH2D(rootname + "_NPF_pt",   "NPF_pt",   getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->npf_pt->Sumw2();
					C->nff_pt   = new TH2D(rootname + "_NFF_pt",   "NFF_pt",   getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nff_pt->Sumw2();
					C->nt2pp_pt = new TH2D(rootname + "_NT2PP_pt", "NT2PP_pt", getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt2pp_pt->Sumw2();
					C->nt2fp_pt = new TH2D(rootname + "_NT2FP_pt", "NT2FP_pt", getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt2fp_pt->Sumw2();
					C->nt2pf_pt = new TH2D(rootname + "_NT2PF_pt", "NT2PF_pt", getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt2pf_pt->Sumw2();
					C->nt2ff_pt = new TH2D(rootname + "_NT2FF_pt", "NT2FF_pt", getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt2ff_pt->Sumw2();

					C->nt11_origin = new TH2D(rootname + "_NT20_Origin",  "NT2Origin",  15, 0, 15, 15, 0, 15);
					C->nt10_origin = new TH2D(rootname + "_NT10_Origin",  "NT1Origin",  15, 0, 15, 15, 0, 15);
					C->nt01_origin = new TH2D(rootname + "_NT01_Origin",  "NT01Origin", 15, 0, 15, 15, 0, 15);
					C->nt00_origin = new TH2D(rootname + "_NT00_Origin",  "NT0Origin",  15, 0, 15, 15, 0, 15);
					label2OriginAxes(C->nt11_origin->GetXaxis(), C->nt11_origin->GetYaxis(), c);
					label2OriginAxes(C->nt10_origin->GetXaxis(), C->nt10_origin->GetYaxis(), c);
					label2OriginAxes(C->nt01_origin->GetXaxis(), C->nt01_origin->GetYaxis(), c);
					label2OriginAxes(C->nt00_origin->GetXaxis(), C->nt00_origin->GetYaxis(), c);					
				}

				// Charge misid truth
				if(c != Muon){
					C->npp_cm_pt   = new TH2D(rootname + "_NPP_CM_pt",   "NPP_CM_pt",   getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->npp_cm_pt->Sumw2();
					C->nt2pp_cm_pt = new TH2D(rootname + "_NT2PP_CM_pt", "NT2PP_CM_pt", getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt2pp_cm_pt->Sumw2();					
				}

					// OS Yields
				if(c == Elec){
					C->nt20_OS_BB_pt = new TH2D(rootname + "_NT20_OS_BB_pt",  "NT20_OS_BB_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_BB_pt ->Sumw2();
					C->nt20_OS_EE_pt = new TH2D(rootname + "_NT20_OS_EE_pt",  "NT20_OS_EE_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_EE_pt ->Sumw2();
					C->nt20_OS_EB_pt = new TH2D(rootname + "_NT20_OS_EB_pt",  "NT20_OS_EB_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_EB_pt ->Sumw2();
				}
				if(c == ElMu){
					C->nt20_OS_BB_pt = new TH2D(rootname + "_NT20_OS_BB_pt",  "NT20_OS_BB_pt",  getNPt2Bins(Muon), getPt2Bins(Muon), getNPt2Bins(Elec), getPt2Bins(Elec)); C->nt20_OS_BB_pt ->Sumw2();
					C->nt20_OS_EE_pt = new TH2D(rootname + "_NT20_OS_EE_pt",  "NT20_OS_EE_pt",  getNPt2Bins(Muon), getPt2Bins(Muon), getNPt2Bins(Elec), getPt2Bins(Elec)); C->nt20_OS_EE_pt ->Sumw2();
				}

				// Ratios
				if(c != ElMu){
					C->fntight  = new TH2D(rootname + "_fNTight",  "fNTight",  getNPt2Bins(c), getPt2Bins(c), getNEtaBins(c), getEtaBins(c)); C->fntight ->Sumw2();
					C->fnloose  = new TH2D(rootname + "_fNLoose",  "fNLoose",  getNPt2Bins(c), getPt2Bins(c), getNEtaBins(c), getEtaBins(c)); C->fnloose ->Sumw2();
					C->pntight  = new TH2D(rootname + "_pNTight",  "pNTight",  getNPt2Bins(c), getPt2Bins(c), getNEtaBins(c), getEtaBins(c)); C->pntight ->Sumw2();
					C->pnloose  = new TH2D(rootname + "_pNLoose",  "pNLoose",  getNPt2Bins(c), getPt2Bins(c), getNEtaBins(c), getEtaBins(c)); C->pnloose ->Sumw2();

					if(S->datamc > 0){
						C->sst_origin = new TH1D(rootname + "_fTOrigin", "fTOrigin", 15, 0, 15); C->sst_origin->Sumw2();
						C->ssl_origin = new TH1D(rootname + "_fLOrigin", "fLOrigin", 15, 0, 15); C->ssl_origin->Sumw2();
						C->zt_origin  = new TH1D(rootname + "_pTOrigin", "pTOrigin", 15, 0, 15); C->zt_origin ->Sumw2();
						C->zl_origin  = new TH1D(rootname + "_pLOrigin", "pLOrigin", 15, 0, 15); C->zl_origin ->Sumw2();
						labelOriginAxis(C->sst_origin->GetXaxis() , c);
						labelOriginAxis(C->ssl_origin->GetXaxis() , c);
						labelOriginAxis(C->zt_origin->GetXaxis()  , c);
						labelOriginAxis(C->zl_origin->GetXaxis()  , c);
					}
				}
			}
		}
	}
}
void MuonPlotter::deleteHistos(Sample *S){
	delete S->cutFlowHisto[Muon];
	delete S->cutFlowHisto[Elec];
	delete S->cutFlowHisto[ElMu];

	// Kinematical histos
	for(size_t hilo = 0; hilo < 2; ++hilo){
		for(size_t k = 0; k < gNKinSels; ++k){
			delete S->kinplots[k][hilo].hmetvsht;
			for(size_t j = 0; j < gNKinVars; ++j) delete S->kinplots[k][hilo].hvar[j];
		}
	}

	// Histos for differential yields
	for(size_t k = 0; k < gNCHANNELS; ++k){
		for(size_t j = 0; j < gNDiffVars; ++j){
			delete S->diffyields[k].hnt11[j];
			delete S->diffyields[k].hnt10[j];
			delete S->diffyields[k].hnt01[j];
			delete S->diffyields[k].hnt00[j];
			if(k == Muon) continue;
			delete S->diffyields[k].hnt2_os_BB[j];
			delete S->diffyields[k].hnt2_os_EE[j];
			if(k == ElMu) continue;
			delete S->diffyields[k].hnt2_os_EB[j];
		}
	}

	for(size_t l = 0; l < 2; ++l){
		// Isolation histos
		for(size_t j = 0; j < gNSels; ++j){
			delete S->isoplots[l].hiso[j];
			for(int k = 0; k < gNMuPt2bins; ++k){
				delete S->isoplots[l].hiso_pt[j][k];
			}
			for(int k = 0; k < gNNVrtxBins; ++k){
				delete S->isoplots[l].hiso_nv[j][k];
			}
		}

		// Ratio histos
		for(size_t j = 0; j < gNRatioVars; ++j){
			delete S->ratioplots[l].ntight[j];
			delete S->ratioplots[l].nloose[j];
		}
	}

	for(size_t hilo = 0; hilo < 2; ++hilo){
		for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
			Region *R = &S->region[r][hilo];
			for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
				Channel *C;
				if(c == Muon) C = &R->mm;
				if(c == Elec) C = &R->ee;
				if(c == ElMu) C = &R->em;

				delete C->nt20_pt;
				delete C->nt10_pt;
				delete C->nt01_pt;
				delete C->nt00_pt;
				delete C->nt20_eta;
				delete C->nt10_eta;
				delete C->nt01_eta;
				delete C->nt00_eta;

				// MC truth info
				if(S->datamc > 0){
					delete C->npp_pt;
					delete C->nfp_pt;
					delete C->npf_pt;
					delete C->nff_pt;
					delete C->nt2pp_pt;
					delete C->nt2fp_pt;
					delete C->nt2pf_pt;
					delete C->nt2ff_pt;

					delete C->nt11_origin;
					delete C->nt10_origin;
					delete C->nt01_origin;
					delete C->nt00_origin;
				}

				// Charge misid truth
				if(c != Muon){
					delete C->npp_cm_pt;
					delete C->nt2pp_cm_pt;
				}

					// OS Yields
				if(c == Elec){
					delete C->nt20_OS_BB_pt;
					delete C->nt20_OS_EE_pt;
					delete C->nt20_OS_EB_pt;
				}
				if(c == ElMu){
					delete C->nt20_OS_BB_pt;
					delete C->nt20_OS_EE_pt;
				}

				// Ratios
				if(c != ElMu){
					delete C->fntight;
					delete C->fnloose;
					delete C->pntight;
					delete C->pnloose;

					if(S->datamc > 0){
						delete C->sst_origin;
						delete C->ssl_origin;
						delete C->zt_origin;
						delete C->zl_origin;
					}
				}
			}
		}
	}
}
void MuonPlotter::writeHistos(Sample *S, TFile *pFile){
	pFile->cd();
	TDirectory* sdir = Util::FindOrCreate(S->sname, pFile);
	sdir->cd();

	TString temp;
	TDirectory *rdir;

	// Cut Flows
	S->cutFlowHisto[Muon]->Write(S->cutFlowHisto[Muon]->GetName(), TObject::kWriteDelete);
	S->cutFlowHisto[Elec]->Write(S->cutFlowHisto[Elec]->GetName(), TObject::kWriteDelete);
	S->cutFlowHisto[ElMu]->Write(S->cutFlowHisto[ElMu]->GetName(), TObject::kWriteDelete);

	// Histos for differential yields
	for(size_t k = 0; k < gNCHANNELS; ++k){
		temp = S->sname + "/DiffYields/";
		rdir = Util::FindOrCreate(temp, pFile);
		rdir->cd();
		for(size_t j = 0; j < gNDiffVars; ++j){
			S->diffyields[k].hnt11[j]->Write(S->diffyields[k].hnt11[j]->GetName(), TObject::kWriteDelete);
			S->diffyields[k].hnt10[j]->Write(S->diffyields[k].hnt10[j]->GetName(), TObject::kWriteDelete);
			S->diffyields[k].hnt01[j]->Write(S->diffyields[k].hnt01[j]->GetName(), TObject::kWriteDelete);
			S->diffyields[k].hnt00[j]->Write(S->diffyields[k].hnt00[j]->GetName(), TObject::kWriteDelete);
			if(k == Muon) continue;
			S->diffyields[k].hnt2_os_BB[j]->Write(S->diffyields[k].hnt2_os_BB[j]->GetName(), TObject::kWriteDelete);
			S->diffyields[k].hnt2_os_EE[j]->Write(S->diffyields[k].hnt2_os_EE[j]->GetName(), TObject::kWriteDelete);
			if(k == ElMu) continue;
			S->diffyields[k].hnt2_os_EB[j]->Write(S->diffyields[k].hnt2_os_EB[j]->GetName(), TObject::kWriteDelete);
		}
	}	

	// Kinematic histos
	for(size_t hilo = 0; hilo < 2; ++hilo){
		temp = S->sname + "/KinPlots/" + gHiLoLabel[hilo] + "/";
		rdir = Util::FindOrCreate(temp, pFile);
		rdir->cd();
		for(size_t k = 0; k < gNKinSels; ++k){
			KinPlots *kp = &S->kinplots[k][hilo];
			kp->hmetvsht->Write(kp->hmetvsht->GetName(), TObject::kWriteDelete);
			for(size_t j = 0; j < gNKinVars; ++j) kp->hvar[j]->Write(kp->hvar[j]->GetName(), TObject::kWriteDelete);
		}
	}

	// Isolation histos
	temp = S->sname + "/IsoPlots/";
	rdir = Util::FindOrCreate(temp, pFile);
	rdir->cd();
	for(size_t l = 0; l < 2; ++l){
		IsoPlots *ip = &S->isoplots[l];
		for(size_t j = 0; j < gNSels; ++j){
			ip->hiso[j]->Write(ip->hiso[j]->GetName(), TObject::kWriteDelete);
			for(int k = 0; k < gNMuPt2bins; ++k) ip->hiso_pt[j][k]->Write(ip->hiso_pt[j][k]->GetName(), TObject::kWriteDelete);
			for(int k = 0; k < gNNVrtxBins; ++k) ip->hiso_nv[j][k]->Write(ip->hiso_nv[j][k]->GetName(), TObject::kWriteDelete);
		}
	}

	// Ratio histos
	temp = S->sname + "/FRatioPlots/";
	rdir = Util::FindOrCreate(temp, pFile);
	rdir->cd();
	for(size_t l = 0; l < 2; ++l){
		FRatioPlots *rp = &S->ratioplots[l];
		for(size_t j = 0; j < gNRatioVars; ++j){
			rp->ntight[j]->Write(rp->ntight[j]->GetName(), TObject::kWriteDelete);
			rp->nloose[j]->Write(rp->nloose[j]->GetName(), TObject::kWriteDelete);
		}
	}

	// Yields
	for(size_t hilo = 0; hilo < 2; ++hilo){
		for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
			Region *R = &S->region[r][hilo];
			TString temp = S->sname + "/" + Region::sname[r] + "/" + gHiLoLabel[hilo] + "/";
			TDirectory* rdir = Util::FindOrCreate(temp, pFile);
			rdir->cd();

			for(gChannel ch = channels_begin; ch < gNCHANNELS; ch=gChannel(ch+1)){ // Loop over channels, mumu, emu, ee
				Channel *C;
				if(ch == Muon)     C = &R->mm;
				if(ch == Elec) C = &R->ee;
				if(ch == ElMu)      C = &R->em;
				C->nt20_pt    ->Write(C->nt20_pt    ->GetName(), TObject::kWriteDelete);
				C->nt10_pt    ->Write(C->nt10_pt    ->GetName(), TObject::kWriteDelete);
				C->nt01_pt    ->Write(C->nt01_pt    ->GetName(), TObject::kWriteDelete);
				C->nt00_pt    ->Write(C->nt00_pt    ->GetName(), TObject::kWriteDelete);
				C->nt20_eta   ->Write(C->nt20_eta   ->GetName(), TObject::kWriteDelete);
				C->nt10_eta   ->Write(C->nt10_eta   ->GetName(), TObject::kWriteDelete);
				C->nt01_eta   ->Write(C->nt01_eta   ->GetName(), TObject::kWriteDelete);
				C->nt00_eta   ->Write(C->nt00_eta   ->GetName(), TObject::kWriteDelete);

				if(ch == Elec || ch == ElMu){
					C->nt20_OS_BB_pt->Write(C->nt20_OS_BB_pt->GetName(), TObject::kWriteDelete);
					C->nt20_OS_EE_pt->Write(C->nt20_OS_EE_pt->GetName(), TObject::kWriteDelete);
					if(ch == Elec) C->nt20_OS_EB_pt->Write(C->nt20_OS_EB_pt->GetName(), TObject::kWriteDelete);					
				}

				if(S->datamc > 0){
					C->npp_pt     ->Write(C->npp_pt     ->GetName(), TObject::kWriteDelete);
					C->nfp_pt     ->Write(C->nfp_pt     ->GetName(), TObject::kWriteDelete);
					C->npf_pt     ->Write(C->npf_pt     ->GetName(), TObject::kWriteDelete);
					C->nff_pt     ->Write(C->nff_pt     ->GetName(), TObject::kWriteDelete);
					C->nt2pp_pt   ->Write(C->nt2pp_pt   ->GetName(), TObject::kWriteDelete);
					C->nt2fp_pt   ->Write(C->nt2fp_pt   ->GetName(), TObject::kWriteDelete);
					C->nt2pf_pt   ->Write(C->nt2pf_pt   ->GetName(), TObject::kWriteDelete);
					C->nt2ff_pt   ->Write(C->nt2ff_pt   ->GetName(), TObject::kWriteDelete);
					C->nt11_origin->Write(C->nt11_origin->GetName(), TObject::kWriteDelete);
					C->nt10_origin->Write(C->nt10_origin->GetName(), TObject::kWriteDelete);
					C->nt01_origin->Write(C->nt01_origin->GetName(), TObject::kWriteDelete);
					C->nt00_origin->Write(C->nt00_origin->GetName(), TObject::kWriteDelete);
					if(ch != Muon){
						C->npp_cm_pt  ->Write(C->npp_cm_pt  ->GetName(), TObject::kWriteDelete);
						C->nt2pp_cm_pt->Write(C->nt2pp_cm_pt->GetName(), TObject::kWriteDelete);						
					}
				}
				if(ch != ElMu){
					C->fntight    ->Write(C->fntight    ->GetName(), TObject::kWriteDelete);
					C->fnloose    ->Write(C->fnloose    ->GetName(), TObject::kWriteDelete);
					C->pntight    ->Write(C->pntight    ->GetName(), TObject::kWriteDelete);
					C->pnloose    ->Write(C->pnloose    ->GetName(), TObject::kWriteDelete);
					if(S->datamc > 0){
						C->sst_origin ->Write(C->sst_origin ->GetName(), TObject::kWriteDelete);
						C->ssl_origin ->Write(C->ssl_origin ->GetName(), TObject::kWriteDelete);
						C->zt_origin  ->Write(C->zt_origin  ->GetName(), TObject::kWriteDelete);
						C->zl_origin  ->Write(C->zl_origin  ->GetName(), TObject::kWriteDelete);						
					}
				}
			}
		}
	}
}
void MuonPlotter::writeSigGraphs(Sample *S, gChannel chan, gHiLoSwitch hilo, TFile *pFile){
	TString channame = "MM";
	if(chan == Elec) channame = "EE";
	if(chan == ElMu)      channame = "EM";
	vector<float> ht;
	vector<float> met;
	if(chan == Muon){
		if(hilo == HighPt){
			ht  = fSigEv_HI_MM_HT;
			met = fSigEv_HI_MM_MET;
		}
		if(hilo == LowPt){
			ht  = fSigEv_LO_MM_HT;
			met = fSigEv_LO_MM_MET;
		}
	}
	if(chan == Elec){
		if(hilo == HighPt){
			ht  = fSigEv_HI_EE_HT;
			met = fSigEv_HI_EE_MET;
		}
		if(hilo == LowPt){
			ht  = fSigEv_LO_EE_HT;
			met = fSigEv_LO_EE_MET;
		}
	}
	if(chan == ElMu){
		if(hilo == HighPt){
			ht  = fSigEv_HI_EM_HT;
			met = fSigEv_HI_EM_MET;
		}
		if(hilo == LowPt){
			ht  = fSigEv_LO_EM_HT;
			met = fSigEv_LO_EM_MET;
		}
	}

	const int nsig = ht.size();
	float ht_a [nsig];
	float met_a[nsig];
	for(size_t i = 0; i < ht.size(); ++i){
		ht_a[i] = ht[i];
		met_a[i] = met[i];
	}

	TGraph *sigevents = new TGraph(nsig, ht_a, met_a);
	sigevents->SetName(Form("%s_%s_%s_SigEvents", S->sname.Data(), channame.Data(), gHiLoLabel[hilo].Data()));

	pFile->cd();
	TString dirname = S->sname + "/SigGraphs/";
	TDirectory* dir = Util::FindOrCreate(dirname, pFile);
	dir->cd();
	sigevents->Write(sigevents->GetName(), TObject::kWriteDelete);
	delete sigevents;
}
int  MuonPlotter::readHistos(TString filename){
	TFile *pFile = TFile::Open(filename, "READ");
	if(pFile == NULL){
		cout << "File " << filename << " does not exist!" << endl;
		return 1;
	}

	pFile->cd();
	if(gNSAMPLES != fSamples.size()){
		cout << "Mismatch in number of samples! Help!" << endl;
		return 1;
	}

	TString getname;

	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];

		// Cut flow histos
		S->cutFlowHisto[Muon] = (TH1D*)pFile->Get(S->sname + "/MMCutFlow");
		S->cutFlowHisto[Elec] = (TH1D*)pFile->Get(S->sname + "/EECutFlow");
		S->cutFlowHisto[ElMu] = (TH1D*)pFile->Get(S->sname + "/EMCutFlow");

		// Histos for differential yields
		for(size_t k = 0; k < gNCHANNELS; ++k){
			TString name;
			for(size_t j = 0; j < gNDiffVars; ++j){
				getname = Form("%s_%s_NT11_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt11[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				getname = Form("%s_%s_NT10_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt10[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				getname = Form("%s_%s_NT01_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt01[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				getname = Form("%s_%s_NT00_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt00[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				if(k == Muon) continue;
				getname = Form("%s_%s_NT11_OS_BB_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt2_os_BB[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				getname = Form("%s_%s_NT11_OS_EE_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt2_os_EE[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
				if(k == ElMu) continue;
				getname = Form("%s_%s_NT11_OS_EB_%s", S->sname.Data(), DiffPredYields::var_name[j].Data(), gChanLabel[k].Data());
				S->diffyields[k].hnt2_os_EB[j] = (TH1D*)pFile->Get(S->sname + "/DiffYields/" + getname);
			}
		}	


		// Kinematic histos
		for(size_t hilo = 0; hilo < 2; ++hilo){
			for(size_t k = 0; k < gNKinSels; ++k){
				KinPlots *kp = &S->kinplots[k][hilo];
				getname = Form("%s_%s_%s_HTvsMET", S->sname.Data(), gKinSelNames[k].Data(), gHiLoLabel[hilo].Data());
				kp->hmetvsht = (TH2D*)pFile->Get(S->sname + "/KinPlots/" + gHiLoLabel[hilo] + "/" + getname);
				for(size_t j = 0; j < gNKinVars; ++j){
					getname = Form("%s_%s_%s_%s", S->sname.Data(), gKinSelNames[k].Data(), gHiLoLabel[hilo].Data(), KinPlots::var_name[j].Data());
					kp->hvar[j] = (TH1D*)pFile->Get(S->sname + "/KinPlots/" + gHiLoLabel[hilo] + "/" + getname);
					kp->hvar[j]->SetFillColor(S->color);
				}
			}
		}

		for(size_t lep = 0; lep < 2; ++lep){ // e-mu loop
			// Isolation histos
			IsoPlots *ip = &S->isoplots[lep];
			for(size_t j = 0; j < gNSels; ++j){
				getname = Form("%s_%s_%siso", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[lep].Data());
				ip->hiso[j] = (TH1D*)pFile->Get(S->sname + "/IsoPlots/" + getname);
				ip->hiso[j]->SetFillColor(S->color);
				for(int k = 0; k < gNMuPt2bins; ++k){
					getname = Form("%s_%s_%siso_pt%d", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[lep].Data(), k);
					ip->hiso_pt[j][k] = (TH1D*)pFile->Get(S->sname + "/IsoPlots/" + getname);
					ip->hiso_pt[j][k]->SetFillColor(S->color);
				}
				for(int k = 0; k < gNNVrtxBins; ++k){
					getname = Form("%s_%s_%siso_nv%d", S->sname.Data(), IsoPlots::sel_name[j].Data(), gEMULabel[lep].Data(), k);
					ip->hiso_nv[j][k] = (TH1D*)pFile->Get(S->sname + "/IsoPlots/" + getname);
					ip->hiso_nv[j][k]->SetFillColor(S->color);
				}
			}

			// Ratio histos
			FRatioPlots *rp = &S->ratioplots[lep];
			for(size_t j = 0; j < gNRatioVars; ++j){
				getname = Form("%s_%s_ntight_%s", S->sname.Data(), gEMULabel[lep].Data(), FRatioPlots::var_name[j].Data());
				rp->ntight[j] = (TH1D*)pFile->Get(S->sname + "/FRatioPlots/" + getname);
				getname = Form("%s_%s_nloose_%s", S->sname.Data(), gEMULabel[lep].Data(), FRatioPlots::var_name[j].Data());
				rp->nloose[j] = (TH1D*)pFile->Get(S->sname + "/FRatioPlots/" + getname);
			}
		}

		// Yields
		for(size_t hilo = 0; hilo < 2; ++hilo){
			for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){ // Loop over regions
				Region *R = &S->region[r][hilo];
				for(gChannel ch = channels_begin; ch < gNCHANNELS; ch=gChannel(ch+1)){ // Loop over channels, mumu, emu, ee
					Channel *C;
					if(ch == Muon)     C = &R->mm;
					if(ch == Elec) C = &R->ee;
					if(ch == ElMu)      C = &R->em;
					TString root = S->sname +"/"+ Region::sname[r] +"/"+ gHiLoLabel[hilo] +"/"+ S->sname +"_"+ Region::sname[r] +"_"+ C->sname +"_"+ gHiLoLabel[hilo];
					C->nt20_pt  = (TH2D*)pFile->Get(root + "_NT20_pt");
					C->nt10_pt  = (TH2D*)pFile->Get(root + "_NT10_pt");
					C->nt01_pt  = (TH2D*)pFile->Get(root + "_NT01_pt");
					C->nt00_pt  = (TH2D*)pFile->Get(root + "_NT00_pt");
					C->nt20_eta = (TH2D*)pFile->Get(root + "_NT20_eta");
					C->nt10_eta = (TH2D*)pFile->Get(root + "_NT10_eta");
					C->nt01_eta = (TH2D*)pFile->Get(root + "_NT01_eta");
					C->nt00_eta = (TH2D*)pFile->Get(root + "_NT00_eta");

					if(S->datamc > 0){
						C->npp_pt      = (TH2D*)pFile->Get(root + "_NPP_pt");
						C->nfp_pt      = (TH2D*)pFile->Get(root + "_NFP_pt");
						C->npf_pt      = (TH2D*)pFile->Get(root + "_NPF_pt");
						C->nff_pt      = (TH2D*)pFile->Get(root + "_NFF_pt");
						C->nt2pp_pt    = (TH2D*)pFile->Get(root + "_NT2PP_pt");
						C->nt2fp_pt    = (TH2D*)pFile->Get(root + "_NT2FP_pt");
						C->nt2pf_pt    = (TH2D*)pFile->Get(root + "_NT2PF_pt");
						C->nt2ff_pt    = (TH2D*)pFile->Get(root + "_NT2FF_pt");
						C->nt11_origin = (TH2D*)pFile->Get(root + "_NT20_Origin");
						C->nt10_origin = (TH2D*)pFile->Get(root + "_NT10_Origin");
						C->nt01_origin = (TH2D*)pFile->Get(root + "_NT01_Origin");
						C->nt00_origin = (TH2D*)pFile->Get(root + "_NT00_Origin");

						if(ch != Muon){
							C->npp_cm_pt   = (TH2D*)pFile->Get(root + "_NPP_CM_pt");
							C->nt2pp_cm_pt = (TH2D*)pFile->Get(root + "_NT2PP_CM_pt");						
						}
					}
					if(ch == Elec || ch == ElMu){
						C->nt20_OS_BB_pt = (TH2D*)pFile->Get(root + "_NT20_OS_BB_pt");
						C->nt20_OS_EE_pt = (TH2D*)pFile->Get(root + "_NT20_OS_EE_pt");
						if(ch == Elec) C->nt20_OS_EB_pt = (TH2D*)pFile->Get(root + "_NT20_OS_EB_pt");
					}

					if(ch != ElMu){
						C->fntight     = (TH2D*)pFile->Get(root + "_fNTight");
						C->fnloose     = (TH2D*)pFile->Get(root + "_fNLoose");
						C->pntight     = (TH2D*)pFile->Get(root + "_pNTight");
						C->pnloose     = (TH2D*)pFile->Get(root + "_pNLoose");
						if(S->datamc > 0){
							C->sst_origin  = (TH1D*)pFile->Get(root + "_fTOrigin");
							C->ssl_origin  = (TH1D*)pFile->Get(root + "_fLOrigin");
							C->zt_origin   = (TH1D*)pFile->Get(root + "_pTOrigin");
							C->zl_origin   = (TH1D*)pFile->Get(root + "_pLOrigin");
						}
					}
				}
				if(hilo == HighPt){
					storeNumbers(S, Muon,     r);
					storeNumbers(S, Elec, r);
					storeNumbers(S, ElMu,      r);					
				}
			}
		}
	}
	return 0;
}
int  MuonPlotter::readSigGraphs(TString filename){
	TFile *pFile = TFile::Open(filename, "READ");
	if(pFile == NULL){
		cout << "File " << filename << " does not exist!" << endl;
		return 1;
	}

	TString channame, getname;
	Color_t color[3] = {kBlack, kBlue, kRed};
	// Size_t  size [3] = {1.6, 1.8, 1.5};
	Size_t size = 1.5;
	Style_t style[3] = {8, 23, 21};

	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		for(size_t hilo = 0; hilo < 2; ++hilo){
			for(gChannel ch = channels_begin; ch < gNCHANNELS; ch=gChannel(ch+1)){ // Loop over channels, mumu, emu, ee
				if(ch == Muon)     channame = "MM";
				if(ch == Elec) channame = "EE";
				if(ch == ElMu)      channame = "EM";
				getname = Form("%s/SigGraphs/%s_%s_%s_SigEvents", S->sname.Data(), S->sname.Data(), channame.Data(), gHiLoLabel[hilo].Data());
				S->sigevents[ch][hilo] = (TGraph*)pFile->Get(getname);
				S->sigevents[ch][hilo]->SetMarkerColor(color[ch]);
				S->sigevents[ch][hilo]->SetMarkerStyle(style[ch]);
				S->sigevents[ch][hilo]->SetMarkerSize(size);
			}
		}
	}
	return 0;
}

//____________________________________________________________________________
void MuonPlotter::bookRatioHistos(){
	gStyle->SetOptStat(0);

	fH2D_MufRatio    = new TH2D("MufRatio",    "Ratio of tight to loose Muons vs Pt vs Eta", getNPt2Bins(Muon), getPt2Bins(Muon), getNEtaBins(Muon), getEtaBins(Muon));
	fH1D_MufRatioPt  = new TH1D("MufRatioPt",  "Ratio of tight to loose Muons vs Pt",        getNPt2Bins(Muon), getPt2Bins(Muon));
	fH1D_MufRatioEta = new TH1D("MufRatioEta", "Ratio of tight to loose Muons vs Eta",       getNEtaBins(Muon), getEtaBins(Muon));
	fH1D_MufRatioPt->SetXTitle(convertVarName("MuPt[0]"));
	fH1D_MufRatioEta->SetXTitle(convertVarName("MuEta[0]"));
	fH2D_MufRatio->SetXTitle(convertVarName("MuPt[0]"));
	fH2D_MufRatio->SetYTitle(convertVarName("MuEta[0]"));
	fH1D_MufRatioPt ->SetYTitle("# Tight / # Loose");
	fH1D_MufRatioEta->SetYTitle("# Tight / # Loose");
	fH1D_MufRatioPt->GetYaxis()->SetTitleOffset(1.2);
	fH1D_MufRatioEta->GetYaxis()->SetTitleOffset(1.2);

	fH2D_ElfRatio    = new TH2D("ElfRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Elec), getPt2Bins(Elec), getNEtaBins(Elec), getEtaBins(Elec));
	fH1D_ElfRatioPt  = new TH1D("ElfRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Elec), getPt2Bins(Elec));
	fH1D_ElfRatioEta = new TH1D("ElfRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Elec), getEtaBins(Elec));
	fH1D_ElfRatioPt->SetXTitle(convertVarName("ElPt[0]"));
	fH1D_ElfRatioEta->SetXTitle(convertVarName("ElEta[0]"));
	fH2D_ElfRatio->SetXTitle(convertVarName("ElPt[0]"));
	fH2D_ElfRatio->SetYTitle(convertVarName("ElEta[0]"));
	fH1D_ElfRatioPt ->SetYTitle("# Tight / # Loose");
	fH1D_ElfRatioEta->SetYTitle("# Tight / # Loose");
	fH1D_ElfRatioPt->GetYaxis()->SetTitleOffset(1.2);
	fH1D_ElfRatioEta->GetYaxis()->SetTitleOffset(1.2);

	fH2D_MupRatio    = new TH2D("MupRatio",    "Ratio of tight to loose Muons vs Pt vs Eta", getNPt2Bins(Muon), getPt2Bins(Muon), getNEtaBins(Muon), getEtaBins(Muon));
	fH1D_MupRatioPt  = new TH1D("MupRatioPt",  "Ratio of tight to loose Muons vs Pt",        getNPt2Bins(Muon), getPt2Bins(Muon));
	fH1D_MupRatioEta = new TH1D("MupRatioEta", "Ratio of tight to loose Muons vs Eta",       getNEtaBins(Muon), getEtaBins(Muon));
	fH1D_MupRatioPt->SetXTitle(convertVarName("MuPt[0]"));
	fH1D_MupRatioEta->SetXTitle(convertVarName("MuEta[0]"));
	fH2D_MupRatio->SetXTitle(convertVarName("MuPt[0]"));
	fH2D_MupRatio->SetYTitle(convertVarName("MuEta[0]"));
	fH1D_MupRatioPt ->SetYTitle("# Tight / # Loose");
	fH1D_MupRatioEta->SetYTitle("# Tight / # Loose");
	fH1D_MupRatioPt->GetYaxis()->SetTitleOffset(1.2);
	fH1D_MupRatioEta->GetYaxis()->SetTitleOffset(1.2);

	fH2D_ElpRatio    = new TH2D("ElpRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Elec), getPt2Bins(Elec), getNEtaBins(Elec), getEtaBins(Elec));
	fH1D_ElpRatioPt  = new TH1D("ElpRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Elec), getPt2Bins(Elec));
	fH1D_ElpRatioEta = new TH1D("ElpRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Elec), getEtaBins(Elec));
	fH1D_ElpRatioPt->SetXTitle(convertVarName("ElPt[0]"));
	fH1D_ElpRatioEta->SetXTitle(convertVarName("ElEta[0]"));
	fH2D_ElpRatio->SetXTitle(convertVarName("ElPt[0]"));
	fH2D_ElpRatio->SetYTitle(convertVarName("ElEta[0]"));
	fH1D_ElpRatioPt ->SetYTitle("# Tight / # Loose");
	fH1D_ElpRatioEta->SetYTitle("# Tight / # Loose");
	fH1D_ElpRatioPt->GetYaxis()->SetTitleOffset(1.2);
	fH1D_ElpRatioEta->GetYaxis()->SetTitleOffset(1.2);	
}
void MuonPlotter::fixPRatios(){
	// Checks if any bin of p ratio histo is empty and puts it to 1
	for(size_t i = 1; i <= fH2D_MupRatio->GetNbinsX(); ++i){
		for(size_t j = 1; j <= fH2D_MupRatio->GetNbinsY(); ++j){
			if(fH2D_MupRatio->GetBinContent(i,j) == 0.0){
				fH2D_MupRatio->SetBinContent(i,j, 1.0);
				fH2D_MupRatio->SetBinError(i,j, 0.5);
			}
		}
	}
	for(size_t i = 1; i <= fH2D_ElpRatio->GetNbinsX(); ++i){
		for(size_t j = 1; j <= fH2D_ElpRatio->GetNbinsY(); ++j){
			if(fH2D_ElpRatio->GetBinContent(i,j) == 0.0){
				fH2D_ElpRatio->SetBinContent(i,j, 1.0);
				fH2D_ElpRatio->SetBinError(i,j, 0.5);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Geninfo stuff
//____________________________________________________________________________
int MuonPlotter::muIndexToBin(int ind){
	// For the origin histograms
	// return the bin to fill for each id/type
	int id    = abs(MuGenID[ind]);
	int mid   = abs(MuGenMID[ind]);
	int mtype = abs(MuGenMType[ind]);
	if(id  != 13)                                 return 1; // mis id
	if(mid == 24)                                 return 2; // W
	if(mid == 23)                                 return 3; // Z
	if(mtype == 2)                                return 4; // tau
	if(mtype == 11 || mtype == 12 || mtype == 18) return 5; // light hadrons
	if(mtype == 13 || mtype == 19)                return 6; // strange hadrons
	if(mtype == 14 || mtype == 16 || mtype == 20) return 7; // charmed hadrons
	if(mtype == 15 || mtype == 17 || mtype == 21) return 8; // bottom hadrons
	if(mtype == 91 || mtype == 92)                return 9; // pythia strings
	return 15;                                              // uid
}
int MuonPlotter::elIndexToBin(int ind){
	// For the origin histograms
	// return the bin to fill for each id/type
	int id    = abs(ElGenID[ind]);
	int type  = abs(ElGenType[ind]);
	int mid   = abs(ElGenMID[ind]);
	int mtype = abs(ElGenMType[ind]);
	if(id  != 11){                                 // mis id
		if(type == 0 || type == 2)                 return 1;  // mis-match
		if(id == 22)                               return 2;  // gamma
		if(type == 11 || type == 12 || type == 13 ||
			type == 18 || type == 19)               return 3;  // Hadr. fake
		return 15;                                            // uid
	}
	if(mid == 24)                                  return 4;  // W
	if(mid == 23)                                  return 5;  // Z
	if(mtype == 2)                                 return 6;  // tau

	if(mtype == 11 || mtype == 12 || mtype == 18)  return 7;  // light hadrons
	if(mtype == 13 || mtype == 19)                 return 8;  // strange hadrons
	if(mtype == 14 || mtype == 16 || mtype == 20)  return 9;  // charmed hadrons
	if(mtype == 15 || mtype == 17 || mtype == 21)  return 10; // bottom hadrons
	if(mtype == 91 || mtype == 92)                 return 11; // pythia strings
	return 15;                                                // uid
}
TString MuonPlotter::muBinToLabel(int bin){
	// For the origin histograms
	// return the bin label for each bin
	switch( bin ){
		case 1:  return "Fake";
		case 2:  return "W";
		case 3:  return "Z";
		case 4:  return "#tau";
		case 5:  return "Light had.";
		case 6:  return "Strange had.";
		case 7:  return "Charmed had.";
		case 8:  return "Bottom had.";
		case 9:  return "QCD String";
		case 10: return "";
		case 11: return "";
		case 12: return "";
		case 13: return "";
		case 14: return "";
		case 15: return "Unidentified";
		default: return "?";
	}
}
TString MuonPlotter::elBinToLabel(int bin){
	// For the origin histograms
	// return the bin label for each bin
	switch( bin ){
		case 1:  return "Mismatch (#mu, #nu, etc.)";
		case 2:  return "Gamma fake / Conversion";
		case 3:  return "Hadronic fake";
		case 4:  return "W";
		case 5:  return "Z";
		case 6:  return "#tau";
		case 7:  return "Light had.";
		case 8:  return "Strange had.";
		case 9:  return "Charmed had.";
		case 10: return "Bottom had.";
		case 11: return "QCD string";
		case 12: return "";
		case 13: return "";
		case 14: return "";
		case 15: return "Unidentified";
		default: return "?";
	}
}
void MuonPlotter::labelOriginAxis(TAxis *axis, gChannel chan){
	if(chan == Muon){
		axis->SetTitle("#mu Origin");
		for(size_t i = 1; i <= 15; ++i){
			axis->SetBinLabel(i, muBinToLabel(i));
		}		
	}
	if(chan == Elec){
		axis->SetTitle("e Origin");
		for(size_t i = 1; i <= 15; ++i){
			axis->SetBinLabel(i, elBinToLabel(i));
		}
	}
	return;
}
void MuonPlotter::label2OriginAxes(TAxis *axis1, TAxis *axis2, gChannel chan){
	if(chan == Muon || chan == Elec){
		labelOriginAxis(axis1, chan);
		labelOriginAxis(axis2, chan);		
	}
	if(chan == ElMu){
		labelOriginAxis(axis1, Muon);
		labelOriginAxis(axis2, Elec);		
	}
	return;
}

void MuonPlotter::printOrigins(gRegion reg){
	TString filename = fOutputDir + "Origins.txt";
	fOUTSTREAM.open(filename.Data(), ios::trunc);
	printMuOriginTable(reg);
	fOUTSTREAM << endl << endl;
	printElOriginTable(reg);
	fOUTSTREAM << endl << endl;
	printEMuOriginTable(reg);
	fOUTSTREAM << endl;
	fOUTSTREAM.close();
}
void MuonPlotter::printMuOriginTable(gRegion reg){
	fOUTSTREAM << "-------------------------------------------" << endl;
	fOUTSTREAM << " Printing origins for Mu/Mu channel..." << endl;

	printMuOriginHeader("NT20");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		print2MuOriginsFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBGMuEnr, 2, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		print2MuOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBGMuEnr, 1, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		print2MuOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBGMuEnr, 0, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;

	printMuOriginHeader("SSTi");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBGMuEnr, 1, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("SSLo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBGMuEnr, 2, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("Z Ti");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 3, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBGMuEnr, 3, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("Z Lo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 4, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBGMuEnr, 4, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
}
void MuonPlotter::printElOriginTable(gRegion reg){
	fOUTSTREAM << "-------------------------------------------" << endl;
	fOUTSTREAM << " Printing origins for E/E channel..." << endl;

	printElOriginHeader("NT20");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		print2ElOriginsFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 2, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		print2ElOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 1, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		print2ElOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 0, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;

	printElOriginHeader("SSTi");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBG, 1, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("SSLo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBG, 2, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("Z Ti");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 3, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBG, 3, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("Z Lo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 4, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(fMCBG, 4, Elec, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl;
}
void MuonPlotter::printEMuOriginTable(gRegion reg){
	fOUTSTREAM << "-------------------------------------------" << endl;
	fOUTSTREAM << " Printing origins for E/Mu channel..." << endl;

	printEMuOriginHeader("NT20");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 2, ElMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 1, ElMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT01");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 10, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 10, ElMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(fMCBG, 0, ElMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
}

void MuonPlotter::printMuOriginHeader(TString name){
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << "    " << name;
	fOUTSTREAM << "            | Fakes   | W       | Z       | tau     | ud had. | s hadr. | c hadr. | b hadr. | strings | unid    |" << endl;
	fOUTSTREAM << "=========================================================================================================================" << endl;	
}
void MuonPlotter::printElOriginHeader(TString name){
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << "    " << name;
	fOUTSTREAM << "            |           Fakes             |                                  real electrons from...                                 |" << endl;
	fOUTSTREAM << "                    | Mismatc | Gam/Con | Hadron. | W       | Z       | tau     | ud had. | s hadr. | c hadr. | b hadr. | strings | unid    |" << endl;
	fOUTSTREAM << "=============================================================================================================================================" << endl;
}
void MuonPlotter::printEMuOriginHeader(TString name){
	TString descr = "";
	if(name == "NT20") descr = "both tight";
	if(name == "NT10") descr = "muon tight";
	if(name == "NT01") descr = "elec tight";
	if(name == "NT00") descr = "none tight";
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << "    " << name;
	fOUTSTREAM << "            |           Fakes             |                             real electrons/muons from...                                |" << endl;
	fOUTSTREAM << "      " << descr;
	fOUTSTREAM << "    | Mismatc | Gam/Con | Hadron. | W       | Z       | tau     | ud had. | s hadr. | c hadr. | b hadr. | strings | unid    |" << endl;
	fOUTSTREAM << "=============================================================================================================================================" << endl;
}

void MuonPlotter::printMuOriginFromSample(Sample *S, int toggle, gRegion reg, gHiLoSwitch hilo){
	if(S->datamc == 0 || S->datamc == 2 ) return;
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	Channel *C = &S->region[reg][hilo].mm;

	TH1D *histo;
	if(toggle == 1) histo = (TH1D*)C->sst_origin->Clone();
	if(toggle == 2) histo = (TH1D*)C->ssl_origin->Clone();
	if(toggle == 3) histo = (TH1D*)C->zt_origin->Clone();
	if(toggle == 4) histo = (TH1D*)C->zl_origin->Clone();

	if(histo->Integral() == 0.) return;
	histo->Scale(100./histo->Integral());

	fOUTSTREAM << setw(15) << S->sname;
	fOUTSTREAM << "     |";
	for(size_t i = 1; i <= 9; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;
}
void MuonPlotter::printElOriginFromSample(Sample *S, int toggle, gRegion reg, gHiLoSwitch hilo){
	if(S->datamc == 0 || S->datamc == 2 ) return;
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	Channel *C = &S->region[reg][hilo].ee;

	TH1D *histo;
	if(toggle == 1) histo = (TH1D*)C->sst_origin->Clone();
	if(toggle == 2) histo = (TH1D*)C->ssl_origin->Clone();
	if(toggle == 3) histo = (TH1D*)C->zt_origin->Clone();
	if(toggle == 4) histo = (TH1D*)C->zl_origin->Clone();

	if(histo->Integral() == 0.) return;
	histo->Scale(100./histo->Integral());

	fOUTSTREAM << setw(15) << S->sname;
	fOUTSTREAM << "     |";
	for(size_t i = 1; i <= 11; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;
}

void MuonPlotter::print2MuOriginsFromSample(Sample *S, int toggle, gRegion reg, gHiLoSwitch hilo){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2) return;
	Channel *C = &S->region[reg][hilo].mm;

	TH2D *histo2d;
	if(toggle == 0) histo2d = C->nt00_origin;
	if(toggle == 1) histo2d = C->nt10_origin;
	if(toggle == 2) histo2d = C->nt11_origin;

	TH1D *histo = histo2d->ProjectionX();
	if(histo->Integral() == 0.) return;
	histo->Scale(100./histo->Integral());

	fOUTSTREAM << setw(15) << S->sname;

	// first muon
	fOUTSTREAM << " mu1 |";
	for(size_t i = 1; i <= 9; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;

	// second muon
	fOUTSTREAM << "                mu2 |";
	histo = histo2d->ProjectionY();
	histo->Scale(100./histo->Integral());
	for(size_t i = 1; i <= 9; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;			
}
void MuonPlotter::print2ElOriginsFromSample(Sample *S, int toggle, gRegion reg, gHiLoSwitch hilo){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2) return;
	Channel *C = &S->region[reg][hilo].ee;

	TH2D *histo2d;
	if(toggle == 0) histo2d = C->nt00_origin;
	if(toggle == 1) histo2d = C->nt10_origin;
	if(toggle == 2) histo2d = C->nt11_origin;

	TH1D *histo = histo2d->ProjectionX();
	if(histo->Integral() == 0.) return;
	histo->Scale(100./histo->Integral());

	fOUTSTREAM << setw(15) << S->sname;

	// first electron
	fOUTSTREAM << " el1 |";
	for(size_t i = 1; i <= 11; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;

	// second electron
	fOUTSTREAM << "                el2 |";
	histo = histo2d->ProjectionY();
	histo->Scale(100./histo->Integral());
	for(size_t i = 1; i <= 11; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;			
}
void MuonPlotter::printEMuOriginsFromSample(Sample *S, int toggle, gRegion reg, gHiLoSwitch hilo){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2 && toggle != 10) return;
	Channel *C = &S->region[reg][hilo].em;

	TH2D *histo2d;
	if(toggle == 0)  histo2d = C->nt00_origin;
	if(toggle == 1)  histo2d = C->nt10_origin;
	if(toggle == 10) histo2d = C->nt01_origin;
	if(toggle == 2)  histo2d = C->nt11_origin;

	TH1D *histo = histo2d->ProjectionX();
	if(histo->Integral() == 0.) return;
	histo->Scale(100./histo->Integral());

	// muon
	fOUTSTREAM << setw(15) << S->sname;
	fOUTSTREAM << " mu  |         |         |";
	for(size_t i = 1; i <= 9; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;

	// electron
	fOUTSTREAM << "                el  |";
	histo = histo2d->ProjectionY();
	histo->Scale(100./histo->Integral());
	for(size_t i = 1; i <= 11; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histo->GetBinContent(15)) << " |" << endl;			
}

void MuonPlotter::printOriginSummary(vector<int> samples, int toggle, gChannel chan, gRegion reg, gHiLoSwitch hilo){
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	TH1D *histosum = new TH1D("SST_Origin_Sum", "SSTOrigin",  15, 0, 15);
	histosum->Sumw2();
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		Channel *C;
		if(chan == Muon)     C = &S->region[reg][hilo].mm;
		if(chan == Elec) C = &S->region[reg][hilo].ee;

		TH1D *histo;
		if(toggle == 1) histo = (TH1D*)C->sst_origin->Clone();
		if(toggle == 2) histo = (TH1D*)C->ssl_origin->Clone();
		if(toggle == 3) histo = (TH1D*)C->zt_origin->Clone();
		if(toggle == 4) histo = (TH1D*)C->zl_origin->Clone();

		float scale = fLumiNorm / S->lumi;
		histosum->Add(histo, scale);
	}
	histosum->Scale(100./histosum->Integral());
	fOUTSTREAM << " Weighted Sum       |";
	int ncols = 9;
	if(chan == Elec) ncols = 11;
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum->GetBinContent(15)) << " |" << endl;
	delete histosum;
}
void MuonPlotter::printOriginSummary2L(vector<int> samples, int toggle, gChannel chan, gRegion reg, gHiLoSwitch hilo){
	if(toggle != 0 && toggle != 1 && toggle != 10 && toggle != 2) return;
	TH1D *histosum1 = new TH1D("SST_Origin_Sum1", "SSTOrigin",  15, 0, 15);
	TH1D *histosum2 = new TH1D("SST_Origin_Sum2", "SSTOrigin",  15, 0, 15);
	histosum1->Sumw2();
	histosum2->Sumw2();
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		Channel *C;
		if(chan == Muon)     C = &S->region[reg][hilo].mm;
		if(chan == Elec) C = &S->region[reg][hilo].ee;
		if(chan == ElMu)      C = &S->region[reg][hilo].em;

		TH2D *histo2d;
		if(toggle == 0)  histo2d = C->nt00_origin;
		if(toggle == 1)  histo2d = C->nt10_origin;
		if(toggle == 10) histo2d = C->nt01_origin;
		if(toggle == 2)  histo2d = C->nt11_origin;

		float scale = fLumiNorm / S->lumi;
		histosum1->Add(histo2d->ProjectionX(), scale);
		histosum2->Add(histo2d->ProjectionY(), scale);
	}
	histosum1->Scale(100./histosum1->Integral());
	histosum2->Scale(100./histosum2->Integral());
	if(chan == Muon)     fOUTSTREAM << " Weighted Sum: mu1  |";
	if(chan == ElMu)      fOUTSTREAM << " Weighted Sum: muon |         |         |";
	if(chan == Elec) fOUTSTREAM << " Weighted Sum: el1  |";
	int ncols = 9;
	if(chan == Elec) ncols = 11;
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum1->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum1->GetBinContent(15)) << " |" << endl;

	if(chan == Muon)     fOUTSTREAM << "               mu2  |";
	if(chan == ElMu)      fOUTSTREAM << "               ele  |";
	if(chan == Elec) fOUTSTREAM << "               el2  |";
	if(chan == ElMu) ncols = 11;	
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum2->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum2->GetBinContent(15)) << " |" << endl;
	delete histosum1;
	delete histosum2;
}

//////////////////////////////////////////////////////////////////////////////
// Trigger stuff:
//____________________________________________________________________________
bool MuonPlotter::mumuSignalTrigger(){
	return ( doubleMuTrigger() || doubleMuHTTrigger() );
}
bool MuonPlotter::elelSignalTrigger(){
	return ( doubleElTrigger() || doubleElHTTrigger() );
}
bool MuonPlotter::elmuSignalTrigger(){
	return ( eMuTrigger() || eMuHTTrigger() );
}

//____________________________________________________________________________
// The following triggers will always return true for MC samples, and they
// will always return false if they are called in the 'wrong' dataset
bool MuonPlotter::singleMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	return (HLT_MU8_JET40 > 0);
}
float MuonPlotter::singleMuPrescale(){
	// Pretend MC samples have prescale 1.
	if(fSamples[fCurrentSample]->datamc > 0) return 1.;
	// Get the prescale factor for whichever of these triggers fired
	// Only correct if they are mutually exclusive!
	if(HLT_MU8_JET40_PS > 0) return HLT_MU8_JET40_PS;
	return 1;
}
bool MuonPlotter::singleElTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	return (HLT_ELE8_JET40 > 0);
}
float MuonPlotter::singleElPrescale(){
	// Pretend MC samples have prescale 1.
	if(fSamples[fCurrentSample]->datamc > 0) return 1.;
	// Get the prescale factor for whichever of these triggers fired
	// Only correct if they are mutually exclusive!
	if( HLT_ELE8_JET40_PS > 0 ) return HLT_ELE8_JET40_PS;
	return 1.;
}

bool MuonPlotter::doubleMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != DoubleMu1 &&
		fCurrentSample != DoubleMu2 &&
		fCurrentSample != DoubleMu3 &&
		fCurrentSample != DoubleMu4) return false;

	return ( (HLT_DOUBLEMU7 > 0) || 
		(HLT_MU13_MU8  > 0) );
}
bool MuonPlotter::doubleElTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleEle dataset!
	if(fCurrentSample != DoubleEle1 &&
		fCurrentSample != DoubleEle2 &&
		fCurrentSample != DoubleEle3 &&
		fCurrentSample != DoubleEle4) return false;

	return ( (HLT_ELE17_ELE8       > 0) ||
		(HLT_ELE17_ELE8_TIGHT > 0) );
}
bool MuonPlotter::doubleMuHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != MuHad1 && fCurrentSample != MuHad2) return false;

	return (HLT_DOUBLEMU3_HT160 > 0);
}
bool MuonPlotter::doubleElHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != EleHad1 && fCurrentSample != EleHad2) return false;

	return ( (HLT_DOUBLEELE8_HT160 > 0) ||
		(HLT_DOUBLEELE8_HT160_TIGHT > 0) );
}
bool MuonPlotter::eMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the MuEG dataset!
	if(fCurrentSample != MuEG1 &&
		fCurrentSample != MuEG2 &&
		fCurrentSample != MuEG3 &&
		fCurrentSample != MuEG4) return false;
	return ( (HLT_MU17_ELE8 > 0) || 
		(HLT_MU8_ELE17 > 0) );
}
bool MuonPlotter::eMuHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the MuEG dataset!
	if(fCurrentSample != MuEG1 &&
		fCurrentSample != MuEG2 &&
		fCurrentSample != MuEG3 &&
		fCurrentSample != MuEG4) return false;

	return ( (HLT_MU3_ELE8_HT160 > 0) ||
		(HLT_MU3_ELE8_HT160_TIGHT > 0) );
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions:
//____________________________________________________________________________
int MuonPlotter::getNJets(){
	int njets(0);
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i)) njets++;
	return njets;
}
int MuonPlotter::getNBTags(){
	int ntags(0);
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i) && JetSSVHPBTag[i] > 2.0) ntags++;
	return ntags;
}
float MuonPlotter::getHT(){
	float ht(0.);
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i)) ht += JetPt[i];
	return ht;
}
float MuonPlotter::getMT2(int ind1, int ind2, int toggle){
	// Calculate MT2 variable for two leptons and missing energy,
	// assuming zero testmass
	// Toggle switches between mumu (1), ee(2), emu(3)
	double pa[3];
	double pb[3];
	double pmiss[3];

	TLorentzVector pmet, pl1, pl2;

	if(toggle == 1){ // mumu
		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(MuPt[ind2], MuEta[ind2], MuPhi[ind2], gMMU);			
	}
	if(toggle == 2){ // ee
		pl1.SetPtEtaPhiM(ElPt[ind1], ElEta[ind1], ElPhi[ind1], gMEL);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);			
	}
	if(toggle == 3){ // emu
		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);			
	}

	pmet.SetPtEtaPhiM(pfMET, 0., pfMETPhi, 0.);
	pmiss[0] = 0.; // irrelevant
	pmiss[1] = pmet.Px();
	pmiss[2] = pmet.Py();

	pa[0] = 0.;
	pa[1] = pl1.Px();
	pa[2] = pl1.Py();

	pb[0] = 0.;
	pb[1] = pl2.Px();
	pb[2] = pl2.Py();

	Davismt2 *DavisMT2 = new Davismt2();
	DavisMT2->set_verbose(0);
	DavisMT2->set_momenta(pa, pb, pmiss);
	DavisMT2->set_mn(0.); // testmass
	double MT2 = DavisMT2->get_mt2();
	delete DavisMT2;
	return MT2;
}
int   MuonPlotter::getClosestJet(int ind, gChannel chan){
// Get index of the closest jet
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];

	float mindr = 999.;
	int cljetindex = -1;
	for(size_t i = 0; i < NJets; ++i){
		if(isGoodJet(i) == false) continue;
		float dr = Util::GetDeltaR(lepeta, JetEta[i], lepphi, JetPhi[i]);
		if(dr > mindr) continue;
		mindr = dr;
		cljetindex = i;
	}
	return cljetindex;
}
float MuonPlotter::getClosestJetPt(int ind, gChannel chan){
// Get the pt of the closest jet
	int jind = getClosestJet(ind, chan);
	if(jind > -1) return JetPt[jind];
	return -1;
}
float MuonPlotter::getClosestJetDR(int ind, gChannel chan){
// Get delta R to the closest jet
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];
	int jind = getClosestJet(ind, chan);
	if(jind > -1) return Util::GetDeltaR(lepeta, JetEta[jind], lepphi, JetPhi[jind]);
	return -1;
}
float MuonPlotter::getSecondClosestJetDR(int ind, gChannel chan){
// Get the pt of the closest jet
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];

	if(NJets < 2) return 10.;

	float mindr  = 888.;
	float mindr2 = 999.;
	for(size_t i = 0; i < NJets; ++i){
		float dr = Util::GetDeltaR(lepeta, JetEta[i], lepphi, JetPhi[i]);
		if(dr < mindr){
			mindr2 = mindr;
			mindr = dr;			
		}
		else if(dr < mindr2){
			mindr2 = dr;
		}
	}
	return mindr2;
}
float MuonPlotter::getAwayJetPt(int ind, gChannel chan){
// Get the pt of away jet
// DR > 0.1, choose hardest
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];

	if(NJets < 1) return 0.;

	float mindr  = 888.;
	for(size_t i = 0; i < NJets; ++i){
		if(Util::GetDeltaR(lepeta, JetEta[i], lepphi, JetPhi[i]) < 1.0) continue;
		if(!isGoodJet(i)) continue;
		return JetPt[i]; // assume sorted by pt, so this will return the hardest one
	}
	return 0.;
}
float MuonPlotter::getMaxJPt(){
	float maxpt(0.);
	for(size_t i = 0; i < NJets; ++i){
		if(!isGoodJet(i)) continue;
		if(JetPt[i] < maxpt) continue;
		maxpt = JetPt[i];
	}
	return maxpt;
}

void MuonPlotter::resetHypLeptons(){
	TLorentzVector vec(0., 0., 0., 0.);
	fHypLepton1 = lepton(vec, 0, -1, -1);
	fHypLepton2 = lepton(vec, 0, -1, -1);
}
void MuonPlotter::setHypLepton1(int index, gChannel chan){
	TLorentzVector vec(0., 0., 0., 0.);
	if(chan == Muon){
		vec.SetPtEtaPhiM(MuPt[index], MuEta[index], MuPhi[index], gMMU);
		fHypLepton1 = lepton(vec, MuCharge[index], 0, index);
	}
	else if(chan == Elec){
		vec.SetPtEtaPhiM(ElPt[index], ElEta[index], ElPhi[index], gMEL);
		fHypLepton1 = lepton(vec, ElCharge[index], 1, index);
	}
	else exit(-1);
}
void MuonPlotter::setHypLepton2(int index, gChannel chan){
	TLorentzVector vec(0., 0., 0., 0.);
	if(chan == Muon){
		vec.SetPtEtaPhiM(MuPt[index], MuEta[index], MuPhi[index], gMMU);
		fHypLepton2 = lepton(vec, MuCharge[index], 0, index);
	}
	else if(chan == Elec){
		vec.SetPtEtaPhiM(ElPt[index], ElEta[index], ElPhi[index], gMEL);
		fHypLepton2 = lepton(vec, ElCharge[index], 1, index);
	}
	else exit(-1);
}

//////////////////////////////////////////////////////////////////////////////
// Event Selections:
//____________________________________________________________________________
bool MuonPlotter::isGoodEvent(){
	// Global cuts?
	return true;
}
bool MuonPlotter::isGoodMuEvent(){
	// Ask for >0 loose muons, if 2 muons ask for second to be loose too
	// return number of loose muons
	if(!isGoodEvent()) return false;
	if(NMus < 1) return false;
	if(isLooseMuon(0) == false) return false;
	if(NMus > 1) if(isLooseMuon(1) == false) return false;
	return true;
}

int MuonPlotter::hasLooseMuons(int &mu1, int &mu2){
	// Returns the number of loose muons and fills their indices in mu1 and mu2
	// Assumes the muons are sorted by pt in the minitree
	vector<int> loosemus;
	mu1 = -1;
	mu2 = -1;
	for(size_t i = 0; i < NMus; ++i) if(isLooseMuon(i)) loosemus.push_back(i);
	if(loosemus.size() > 0) mu1 = loosemus[0];
	if(loosemus.size() > 1) mu2 = loosemus[1];
	return loosemus.size();
}
int MuonPlotter::hasLooseMuons(){
	int ind1(-1), ind2(-1);
	return hasLooseMuons(ind1, ind2);
}
int MuonPlotter::hasLooseElectrons(int &el1, int &el2){
	// Returns the number of loose electrons and fills their indices in el1 and el2
	// Assumes the electrons are sorted by pt in the minitree
	vector<int> looseels;
	el1 = -1;
	el2 = -1;
	for(size_t i = 0; i < NEls; ++i) if(isLooseElectron(i)) looseels.push_back(i);
	if(looseels.size() > 0) el1 = looseels[0];
	if(looseels.size() > 1) el2 = looseels[1];
	return looseels.size();
}
int MuonPlotter::hasLooseElectrons(){
	int ind1(-1), ind2(-1);
	return hasLooseElectrons(ind1, ind2);
}

//____________________________________________________________________________
int MuonPlotter::isSSLLEvent(int &ind1, int &ind2){
	// Check first if there is a tight-tight pair in the event
	int res = isSSEvent(ind1, &MuonPlotter::isTightMuon, ind2, &MuonPlotter::isTightElectron);
	if(res > 0) return res;
	return isSSEvent(ind1, &MuonPlotter::isLooseMuon, ind2, &MuonPlotter::isLooseElectron);
}
int MuonPlotter::isOSLLEvent(int &ind1, int &ind2){
	// Check first if there is a tight-tight pair in the event
	int res = isOSEvent(ind1, &MuonPlotter::isTightMuon, ind2, &MuonPlotter::isTightElectron);
	if(res > 0) return res;
	else return isOSEvent(ind1, &MuonPlotter::isLooseMuon, ind2, &MuonPlotter::isLooseElectron);
}
int MuonPlotter::isSSEvent(int &ind1, bool(MuonPlotter::*muonSelector)(int), int &ind2, bool(MuonPlotter::*eleSelector)(int)){
	// Looks for a SS pair of leptons with given object selectors
	// Return the channel: 0 = none found
	//                     1 / -1 = mu+mu+ / mu-mu- pair
	//                     2 / -2 = e+e+   / e-e-   pair
	//                     3 / -3 = mu+e+  / mu-e-  pair
	// The indices in the argument given are sorted by pt unless
	// it's a e/mu event when they are sorted such that the muon
	// is ind1

	// The pair selected is the one with hardest pt1 + pt2
	vector<lepton> tmp_Leptons_p;
	vector<lepton> tmp_Leptons_m;

	// First store all loose leptons in two vectors according to their charges
	TLorentzVector plep;
	for(size_t i = 0; i < NMus; ++i){
		if((*this.*muonSelector)(i) == false) continue;
		if(MuCharge[i] == 1 ){
			plep.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);
			lepton tmpLepton(plep, 1, 0, i);
			tmp_Leptons_p.push_back(tmpLepton);
		}
		if(MuCharge[i] == -1){
			plep.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);
			lepton tmpLepton(plep, -1, 0, i);
			tmp_Leptons_m.push_back(tmpLepton);
		}
	}
	for(size_t i = 0; i < NEls; ++i){
		if((*this.*eleSelector)(i) == false) continue;
		if(ElCharge[i] == 1 ){
			plep.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);
			lepton tmpLepton(plep, 1, 1, i);
			tmp_Leptons_p.push_back(tmpLepton);
		}
		if(ElCharge[i] == -1){
			plep.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);
			lepton tmpLepton(plep, -1, 1, i);
			tmp_Leptons_m.push_back(tmpLepton);
		}
	}

	// Check for at least one pair
	if(tmp_Leptons_m.size() < 2 && tmp_Leptons_p.size() < 2) return 0;

	/////////////////////////////////////////////////////////////////////////
	// Sort these vectors by type and pt
	vector<lepton> Leptons_p = sortLeptonsByTypeAndPt(tmp_Leptons_p);
	vector<lepton> Leptons_m = sortLeptonsByTypeAndPt(tmp_Leptons_m);

	// Proceed to select one ss pair
	double ptsum1(0.), ptsum2(0.);
	int typesum1(9), typesum2(9); // 0 for mm, 1 for em, 2 for ee
	int select(0); // switch between the two possible pairs
	if(Leptons_p.size() > 1){
		ptsum1   = Leptons_p[0].p.Pt() + Leptons_p[1].p.Pt();
		typesum1 = Leptons_p[0].type   + Leptons_p[1].type;
	}
	if(Leptons_m.size() > 1){
		ptsum2   = Leptons_m[0].p.Pt() + Leptons_m[1].p.Pt();
		typesum2 = Leptons_m[0].type   + Leptons_m[1].type;
	}
	// Selection logic:
	if(typesum1 < typesum2)                     select = 1;  // first pair had more muons
	if(typesum1 > typesum2)                     select = -1; // second pair has more muons
	if(typesum1 == typesum2 && ptsum1 > ptsum2) select = 1;  // both have same #muons, select by ptsum
	if(typesum1 == typesum2 && ptsum1 < ptsum2) select = -1;

	vector<lepton> selectedPair;
	if(select == 1){ // positive
		selectedPair.push_back(Leptons_p[0]);
		selectedPair.push_back(Leptons_p[1]);
	}
	if(select == -1){ // negative
		selectedPair.push_back(Leptons_m[0]);
		selectedPair.push_back(Leptons_m[1]);
	}
	/////////////////////////////////////////////////////////////////////////

	int result = 0;
	if(selectedPair[0].type == 0 && selectedPair[1].type == 0) result = 1; // mu/mu
	if(selectedPair[0].type == 1 && selectedPair[1].type == 1) result = 2; // el/el
	if(selectedPair[0].type == 0 && selectedPair[1].type == 1) result = 3; // mu/el
	result *= select; // Add charge to result

	// Return values
	ind1 = selectedPair[0].index;
	ind2 = selectedPair[1].index;
	return result;
}
int MuonPlotter::isOSEvent(int &ind1, bool(MuonPlotter::*muonSelector)(int), int &ind2, bool(MuonPlotter::*eleSelector)(int)){
	// Looks for a OS pair of leptons with given object selectors
	// Return the channel: 0 = none found
	//                     1 / -1 = mu+mu- / mu-mu+ pair
	//                     2 / -2 = e+e-   / e-e+   pair
	//                     3 / -3 = mu+e-  / mu-e+  pair
	// The indices in the argument given are sorted by pt unless
	// it's a e/mu event when they are sorted such that the muon
	// is ind1
	// Return value has sign of harder of the two, or the mu if
	// it's a mu/e pair
	// The event selected is the one with hardest pt1 + pt2
	vector<lepton> tmp_Leptons;

	// First store all loose leptons in a vector
	for(size_t i = 0; i < NMus; ++i){
		if((*this.*muonSelector)(i) == false) continue;
		TLorentzVector pmu;
		pmu.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);
		lepton tmpLepton(pmu, MuCharge[i], 0, i);
		tmp_Leptons.push_back(tmpLepton);
	}
	for(size_t i = 0; i < NEls; ++i){
		if((*this.*eleSelector)(i) == false) continue;
		TLorentzVector p;
		p.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);
		lepton tmpLepton(p, ElCharge[i], 1, i);
		tmp_Leptons.push_back(tmpLepton);
	}

	// Sort these vector by their flavor and their pt
	vector<lepton> v_Leptons;
	v_Leptons = sortLeptonsByTypeAndPt(tmp_Leptons);

	// Proceed to select one os pair
	if(v_Leptons.size() < 2) return 0;

	vector<lepton> selectedPair;
	selectedPair.push_back(v_Leptons[0]);
	for(size_t i = 1; i < v_Leptons.size(); ++i){ // look for the next lep with opp. charge
		if(selectedPair[0].charge == v_Leptons[i].charge) continue;
		selectedPair.push_back(v_Leptons[i]);
		break;
	}
	if(selectedPair.size() < 2) return 0;

	int result = 0;
	if(selectedPair[0].type == 0 && selectedPair[1].type == 0) result = 1; // mu/mu
	if(selectedPair[0].type == 1 && selectedPair[1].type == 1) result = 2; // el/el
	if(selectedPair[0].type == 0 && selectedPair[1].type == 1) result = 3; // mu/el
	result *= selectedPair[0].charge; // Add charge to result

	// Result
	ind1 = selectedPair[0].index;
	ind2 = selectedPair[1].index;
	return result;
}

//____________________________________________________________________________
bool momentumComparator(MuonPlotter::lepton i, MuonPlotter::lepton j){ return (i.p.Pt()>j.p.Pt()); }
bool momentumAndTypeComparator(MuonPlotter::lepton i, MuonPlotter::lepton j){
	// If e/mu, return the muon irrespective of type
	if(i.type != j.type) return i.type < j.type;
	return momentumComparator(i,j);
}
vector<MuonPlotter::lepton> MuonPlotter::sortLeptonsByPt(vector<lepton>& leptons){
	vector<lepton> theLep = leptons;
	sort (theLep.begin(), theLep.end(), momentumComparator);
	return theLep;
}
vector<MuonPlotter::lepton> MuonPlotter::sortLeptonsByTypeAndPt(vector<lepton>& leptons){
	vector<lepton> theLep = leptons;
	sort (theLep.begin(), theLep.end(), momentumAndTypeComparator);
	return theLep;
}

//____________________________________________________________________________
bool MuonPlotter::passesNJetCut(int cut){
	return getNJets() >= cut;
}
bool MuonPlotter::passesJet50Cut(){
	// Return true if event contains one good jet with pt > 50
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i, 50)) return true;
	return false;
}

//____________________________________________________________________________
bool MuonPlotter::passesHTCut(float min, float max){
	return (getHT() >= min && getHT() < max);
}
bool MuonPlotter::passesMETCut(float min, float max){
	return (pfMET >= min && pfMET < max);
}
bool MuonPlotter::passesZVeto(bool(MuonPlotter::*muonSelector)(int), bool(MuonPlotter::*eleSelector)(int), float dm){
// Checks if any combination of opposite sign, same flavor leptons (e or mu)
// has invariant mass closer than dm to the Z mass, returns true if none found
// Default for dm is 15 GeV
	if(NMus > 1){
		for(size_t i = 0; i < NMus-1; ++i){
			if((*this.*muonSelector)(i)){
				TLorentzVector pmu1, pmu2;
				pmu1.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);

				// Second muon
				for(size_t j = i+1; j < NMus; ++j){ 
					if((*this.*muonSelector)(j) && (MuCharge[i] != MuCharge[j]) ){
						pmu2.SetPtEtaPhiM(MuPt[j], MuEta[j], MuPhi[j], gMMU);
						if(fabs((pmu1+pmu2).M() - gMZ) < dm) return false;
					}
				}
			}
		}
	}

	if(NEls > 1){
		for(size_t i = 0; i < NEls-1; ++i){
			if((*this.*eleSelector)(i)){
				TLorentzVector pel1, pel2;
				pel1.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);

				// Second electron
				for(size_t j = i+1; j < NEls; ++j){
					if((*this.*eleSelector)(j) && (ElCharge[i] != ElCharge[j]) ){
						pel2.SetPtEtaPhiM(ElPt[j], ElEta[j], ElPhi[j], gMEL);
						if(fabs((pel1+pel2).M() - gMZ) < dm) return false;
					}
				}
			}
		}		
	}
	return true;
}
bool MuonPlotter::passesZVeto(float dm){
	// return passesZVeto(&MuonPlotter::isTightMuon, &MuonPlotter::isTightElectron, dm);
	return passesZVeto(&MuonPlotter::isLooseMuon, &MuonPlotter::isLooseElectron, dm);
}
bool MuonPlotter::passesMllEventVeto(float cut){
// Checks if any combination of opposite sign, same flavor tight leptons (e or mu)
// has invariant mass smaller than cut, returns true if none found
	if(NMus > 1){
		// First muon
		for(size_t i = 0; i < NMus-1; ++i){
			if(isTightMuon(i)){
				TLorentzVector pmu1, pmu2;
				pmu1.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);

				// Second muon
				for(size_t j = i+1; j < NMus; ++j){ 
					if(isTightMuon(j)){
						pmu2.SetPtEtaPhiM(MuPt[j], MuEta[j], MuPhi[j], gMMU);
						if((pmu1+pmu2).M() < cut) return false;
					}
				}
			}
		}
	}

	if(NEls > 1){ 
	// First electron
		for(size_t i = 0; i < NEls-1; ++i){
			if(isTightElectron(i)){
				TLorentzVector pel1, pel2;
				pel1.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);

				// Second electron
				for(size_t j = i+1; j < NEls; ++j){
					if(isTightElectron(j)){
						pel2.SetPtEtaPhiM(ElPt[j], ElEta[j], ElPhi[j], gMEL);
						if((pel1+pel2).M() < cut) return false;
					}
				}
			}
		}
	}
	return true;
}
bool MuonPlotter::passesMllEventVeto(int ind1, int ind2, int toggle, float cut){
	// Calculate invariant mass of pair, return false if it's smaller than cut
	TLorentzVector pmet, pl1, pl2;

	if(toggle == 1){ // mumu
		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(MuPt[ind2], MuEta[ind2], MuPhi[ind2], gMMU);			
	}
	if(toggle == 2){ // ee
		pl1.SetPtEtaPhiM(ElPt[ind1], ElEta[ind1], ElPhi[ind1], gMEL);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);			
	}
	if(toggle == 3){ // emu
		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);			
	}
	if( (pl1+pl2).M() < cut ) return false;
	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isGoodRun(gSample sample){
	// Select runs such that JetB and MultiJet datasets are mutually exclusive
	// if(gSample(sample) == JMB)      if(Run > 147195) return false;
	// if(gSample(sample) == MultiJet) if(Run < 147196) return false;
	// if(sample == JMB)      if(Run > 148058) return false;
	// if(sample == MultiJet) if(Run < 148822) return false;
	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isSigSupMuEvent(){
	int mu1(-1), mu2(-1);
	if(hasLooseMuons(mu1, mu2) < 1) return false;
	setHypLepton1(mu1, Muon);
	if(!passesJet50Cut())           return false;
	if(!passesNJetCut(1))           return false;
	// if(!passesNJetCut(fC_minNjets)) return false;
	if(MuMT[0] > fC_maxMt_Control)  return false;
	if(pfMET > fC_maxMet_Control)   return false;
	if(NMus > 1)                    return false;
	return true;
}
bool MuonPlotter::isZMuMuEvent(){
	int mu1(-1), mu2(-1);
	if(hasLooseMuons(mu1, mu2) < 2)  return false;
	// if(!isTightMuon(0) && !isTightMuon(1)) return false; // at least one tight

	if(MuCharge[mu1] == MuCharge[mu2]) return false; // os

	// Z mass window cut
	TLorentzVector p1, p2;
	p1.SetPtEtaPhiM(MuPt[mu1], MuEta[mu1], MuPhi[mu1], 0.1057);
	p2.SetPtEtaPhiM(MuPt[mu2], MuEta[mu2], MuPhi[mu2], 0.1057);
	double m = (p1+p2).M();
	if(fabs(gMZ - m) > 15.) return false;

	setHypLepton1(mu1, Muon);
	setHypLepton2(mu2, Muon);

	if(pfMET > 20.) return false;

	if(!passesNJetCut(2)) return false;
	return true;
}
bool MuonPlotter::isSigSupElEvent(){
	int el1(-1), el2(-1);
	if(hasLooseElectrons(el1, el2) < 1) return false;
	setHypLepton1(el1, Elec);
	if(!passesJet50Cut())               return false;
	if(!passesNJetCut(1))               return false;
	// if(!passesNJetCut(fC_minNjets))     return false;
	if(ElMT[0] > fC_maxMt_Control)      return false;
	if(pfMET > fC_maxMet_Control)       return false;
	if(NEls > 1)                        return false;
	return true;
}
bool MuonPlotter::isZElElEvent(int &elind){
	int el1(-1), el2(-1);
	if(hasLooseElectrons(el1, el2) < 2)  return false;
	if(!isTightElectron(el1) && !isTightElectron(el2)) return false; // at least one tight

	if(ElCharge[el1] == ElCharge[el2]) return false; // os

	// Z mass window cut
	TLorentzVector p1, p2;
	p1.SetPtEtaPhiM(ElPt[el1], ElEta[el1], ElPhi[el1], gMEL);
	p2.SetPtEtaPhiM(ElPt[el2], ElEta[el2], ElPhi[el2], gMEL);
	double m = (p1+p2).M();
	if(fabs(gMZ - m) > 15.) return false;

	setHypLepton1(el1, Elec);
	setHypLepton2(el2, Elec);


	// If only the harder one tight or both tight, return the softer one
	// If only the softer one tight, return the harder one
	elind = el2;
	if(isTightElectron(el2) && !isTightElectron(el1)) elind = el1;

	if(pfMET > 20.) return false;
	if(!passesNJetCut(2)) return false;
	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isGenMatchedSUSYDiLepEvent(){
	int ind1(-1), ind2(-1);
	return isGenMatchedSUSYDiLepEvent(ind1, ind2);
}
bool MuonPlotter::isGenMatchedSUSYDiLepEvent(int &mu1, int &mu2){
	if(!isSSLLMuEvent(mu1, mu2)) return false;
	if(!isTightMuon(mu1) || !isTightMuon(mu2)) return false;
	if(isPromptMuon(mu1) && isPromptMuon(mu2)) return true;
	return false;
}
bool MuonPlotter::isGenMatchedSUSYEEEvent(){
	int ind1(-1), ind2(-1);
	if(!isSSLLElEvent(ind1, ind2)) return false;
	if(!isTightElectron(ind1) || !isTightElectron(ind2)) return false;
	if(isPromptElectron(ind1) && isPromptElectron(ind2)) return true;
	return false;
}
bool MuonPlotter::isGenMatchedSUSYEMuEvent(){
	int muind(-1), elind(-1);
	if(!isSSLLElMuEvent(muind, elind)) return false;
	if(!isTightElectron(elind) || !isTightMuon(muind)) return false;
	if(isPromptMuon(muind) && isPromptElectron(elind)) return true;
	return false;
}

//____________________________________________________________________________
bool MuonPlotter::isSSLLMuEvent(int& mu1, int& mu2){
	// This should include all the cuts for the final selection
	int nmus = hasLooseMuons(mu1, mu2);
	if(nmus < 1) return false; // >0 loose muons
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[3]);
	if(nmus < 2) return false; // >1 loose muons
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[4]);

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(mu1, mu2)) != 1) return false;
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[5]);
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(mu1, mu2)) != 1) return false;
	}

	if(fChargeSwitch == 0){
		// Need to remove the Z veto in OS case, since these OS events are exactly what 
		// contributes to the SS yield. They would NOT fire the Z veto, since they are
		// misidd as SS events
		if(!passesZVeto()) return false; // no Zs in event
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[6]);
	}

	if(!passesMllEventVeto(mu1, mu2, 1, 5.)) return false; // no low mass OSSF pairs
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[7]);

	// Define hypothesis leptons
	setHypLepton1(mu1, Muon);
	setHypLepton1(mu2, Muon);

	// if(!passesJet50Cut()) return false; // one jet with pt > 50
	// if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[8]);

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[9]);

	if(!passesHTCut(fC_minHT, fC_maxHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[10]);

	if(!passesMETCut(fC_minMet, fC_maxMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[11]);

	if(!isGoodSecMuon(mu2)) return false; // pt cuts
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[12]);

	if(!isGoodPrimMuon(mu1)) return false;
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(fMMCutNames[13]);

	return true;
}
bool MuonPlotter::isSSLLElEvent(int& el1, int& el2){
	// This should include all the cuts for the final selection
	int nels = hasLooseElectrons(el1, el2);
	if(nels < 1) return false; // >0 eles;
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[3]);
	if(nels < 2) return false; // >1 eles
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[4]);

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(el1, el2)) != 2) return false;
		if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[5]);
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(el1, el2)) != 2) return false;
	}

	if(fChargeSwitch == 0){
		// Need to remove the Z veto in OS case, since these OS events are exactly what 
		// contributes to the SS yield. They would NOT fire the Z veto, since they are
		// misidd as SS events
		if(!passesZVeto()) return false; // no Zs in event
		if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[6]);
	}

	if(!passesMllEventVeto(el1, el2, 2, 5.)) return false; // no low mass OSSF pairs
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[7]);

	// Define hypothesis leptons
	setHypLepton1(el1, Elec);
	setHypLepton1(el2, Elec);

	// if(!passesJet50Cut()) return false;
	// if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[8]);

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[9]);

	if(!passesHTCut(fC_minHT, fC_maxHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[10]);

	if(!passesMETCut(fC_minMet, fC_maxMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[11]);

	if(!isGoodSecElectron(el2)) return false; // pt cuts
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[12]);

	if(!isGoodPrimElectron(el1)) return false;
	if(fDoCounting) fCounters[fCurrentSample][Elec].fill(fEECutNames[13]);

	return true;
}
bool MuonPlotter::isSSLLElMuEvent(int& mu, int& el){
	// This should include all the cuts for the final selection
	int nmus = hasLooseMuons(mu, el);
	if(nmus > 0 && fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[3]);
	int nels = hasLooseElectrons(el, mu);
	if(nels > 0 && fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[4]);
	if(nels < 1 || nmus < 1) return false;
	if(nels > 0 && fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[5]);

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(mu, el)) != 3) return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[6]);
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(mu, el)) != 3) return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[6]);
	}

	if(fChargeSwitch == 0){
		// Need to remove the Z veto in OS case, since these OS events are exactly what 
		// contributes to the SS yield. They would NOT fire the Z veto, since they are
		// misidd as SS events
		if(!passesZVeto()) return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[7]);
	}

	// Define hypothesis leptons
	setHypLepton1(mu, Muon);
	setHypLepton1(el, Elec);

	// if(!passesJet50Cut()) return false;
	// if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[8]);

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[9]);

	if(!passesHTCut(fC_minHT, fC_maxHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[10]);

	if(!passesMETCut(fC_minMet, fC_maxMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[11]);

	if(MuPt[mu] > ElPt[el]){
		if(!isGoodPrimMuon(mu))    return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[12]);
		if(!isGoodSecElectron(el)) return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[13]);
	}
	else if(MuPt[mu] < ElPt[el]){
		if(!isGoodPrimElectron(el)) return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[13]);
		if(!isGoodSecMuon(mu))      return false;
		if(fDoCounting) fCounters[fCurrentSample][ElMu].fill(fEMCutNames[12]);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// Object selections:
//////////////////////////////////////////////////////////////////////////////
// Muons
//____________________________________________________________________________
bool MuonPlotter::isGoodMuon(int muon, float ptcut){
	if(muon >= NMus) return false; // Sanity check
	if(ptcut < 0.) ptcut = fC_minMu2pt;
	if(MuPt[muon] < ptcut) return false;
	if(MuPtE[muon]/MuPt[muon] > 0.1) return false;
	return true;
}
bool MuonPlotter::isLooseMuon(int muon){
	if(isGoodMuon(muon) == false)  return false;
	if(MuIso[muon] > 1.00) return false;
	return true;
}
bool MuonPlotter::isTightMuon(int muon){
	if(isGoodMuon(muon) == false)  return false;
	if(isLooseMuon(muon) == false) return false;
	if(MuIso[muon] > 0.15) return false;
	// if(MuIso[muon] > 0.1) return false;
	return true;
}
bool MuonPlotter::isGoodPrimMuon(int muon, float ptcut){
	if(ptcut < 0.) ptcut = fC_minMu1pt;
	if(isLooseMuon(muon) == false) return false;
	if(MuPt[muon] < ptcut) return false;
	return true;
}
bool MuonPlotter::isGoodSecMuon(int muon, float ptcut){
	if(ptcut < 0.) ptcut = fC_minMu2pt;
	if(isLooseMuon(muon) == false) return false;
	if(MuPt[muon] < ptcut) return false;
	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isFakeMuon(int muon){
	if(isLooseMuon(muon) == false) return false;
	if(MuGenMType[muon] == 2)  return false;
	if(MuGenMType[muon] == 4)  return false;
	return true;
}
bool MuonPlotter::isPromptMuon(int muon){
	if(isLooseMuon(muon) == false) return false;
	if(abs(MuGenID[muon]) != 13) return false; // not matched to mu
	if(MuGenMType[muon] == 4) return true; // W/Z -> mu
	if(abs(MuGenMID[muon]) == 15 && MuGenGMType[muon] == 4) return true; // W/Z -> tau -> mu
	if(MuGenMType[muon] == 9) return true; // susy particle
	return false;
}
bool MuonPlotter::isChargeMatchedMuon(int mu){
	if(MuGenID[mu] ==  13) return MuCharge[mu] < 0; // muon (-)
	if(MuGenID[mu] == -13) return MuCharge[mu] > 0; // anti-muon (+)
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// Electrons
//____________________________________________________________________________
bool MuonPlotter::isGoodElectron(int ele, float ptcut){
// Some common electron ID cuts
	if(ele >= NEls) return false; // Sanity check
	if(ptcut < 0.) ptcut = fC_minEl2pt;
	if(ElPt[ele] < ptcut) return false;

	if(ElEcalRecHitSumEt[ele]/ElPt[ele] > 0.2) return false; // when using "CaloIsoVL" triggers

	// Reject electrons closer than 0.1 in DR to tight muons
	for(size_t i = 0; i < NMus; ++i){
		if(!isTightMuon(i)) continue;
		if(Util::GetDeltaR(MuEta[i], ElEta[ele], MuPhi[i], ElPhi[ele]) > 0.1 ) continue;
		return false;
	}
	return true;
}
bool MuonPlotter::isLooseElectron(int ele){
	// All electrons are already loose in the high-pt selection (hybiso)
	if(isGoodElectron(ele) == false) return false;
	if( fabs(ElEta[ele]) < 1.479 ) if(ElRelIso[ele] > 1.00) return false;
	else                           if(ElRelIso[ele] > 0.60) return false;		
	if(ElChIsCons[ele] != 1) return false;
	return true;
}
bool MuonPlotter::isTightElectron(int ele){
	if(!isLooseElectron(ele))       return false;
	if(ElIsGoodElId_WP80[ele] != 1) return false;

	if(ElRelIso[ele]    > 0.15) return false;
	// if(ElRelIso[ele]    > 0.1) return false;

	return true;
}
bool MuonPlotter::isGoodPrimElectron(int ele, float ptcut){
	if(ptcut < 0.) ptcut = fC_minEl1pt;
	if(isLooseElectron(ele) == false) return false;
	if(ElPt[ele] < ptcut) return false;
	return true;
}
bool MuonPlotter::isGoodSecElectron(int ele, float ptcut){
	if(ptcut < 0.) ptcut = fC_minEl2pt;
	if(isLooseElectron(ele) == false) return false;
	if(ElPt[ele] < ptcut) return false;
	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isFakeElectron(int ele){
	if(isLooseElectron(ele) == false) return false;
	if(ElGenMType[ele] == 2) return false;
	if(ElGenMType[ele] == 4) return false;
	return true;
}
bool MuonPlotter::isPromptElectron(int ele){
	if(isLooseElectron(ele) == false) return false;
	if(abs(ElGenID[ele]) != 11) return false; // not matched to e
	if(ElGenMType[ele] == 4) return true; // W/Z -> el
	if(abs(ElGenMID[ele]) == 15 && ElGenGMType[ele] == 4) return true; // W/Z -> tau -> el
	if(ElGenMType[ele] == 9) return true; // susy particle
	return false;
}
bool MuonPlotter::isChargeMatchedElectron(int ele){
	if(ElGenID[ele] ==  11) return ElCharge[ele] < 0; // electron
	if(ElGenID[ele] == -11) return ElCharge[ele] > 0; // positron
	return true;
}

bool MuonPlotter::isBarrelElectron(int ele){
	// true if in barrel, false if in endcap
	if(fabs(ElEta[ele]) < 1.479 ) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// Jets
//____________________________________________________________________________
bool MuonPlotter::isGoodJet(int jet, float pt){
	if(jet >= NJets) return false; // Sanity check
	float minDR = 0.4;

	// Remove jets close to hypothesis leptons
	if(fHypLepton1.index > -1) if(Util::GetDeltaR(fHypLepton1.p.Eta(), JetEta[jet], fHypLepton1.p.Phi(), JetPhi[jet]) < minDR) return false;
	if(fHypLepton2.index > -1) if(Util::GetDeltaR(fHypLepton2.p.Eta(), JetEta[jet], fHypLepton2.p.Phi(), JetPhi[jet]) < minDR) return false;

	// Remove jets close to all tight leptons
	for(size_t imu = 0; imu < NMus; ++imu){
		if(!isTightMuon(imu)) continue;
		if(!isGoodSecMuon(imu)) continue; // pt > 10
		if(Util::GetDeltaR(MuEta[imu], JetEta[jet], MuPhi[imu], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	for(size_t iel = 0; iel < NEls; ++iel){
		if(!isTightElectron(iel)) continue;
		if(!isGoodSecElectron(iel)) continue;
		if(Util::GetDeltaR(ElEta[iel], JetEta[jet], ElPhi[iel], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	if(JetPt[jet] < pt) return false;
	return true;
}


//____________________________________________________________________________
void MuonPlotter::drawTopLine(){
	fLatex->SetTextFont(62);
	fLatex->SetTextSize(0.05);
	fLatex->DrawLatex(0.13,0.92, "CMS Preliminary");	
	fLatex->SetTextFont(42);
	fLatex->SetTextSize(0.04);
	fLatex->DrawLatex(0.70,0.92, Form("L_{int.} = %2.1f fb^{-1}", fLumiNorm/1000.));
	// fLatex->DrawLatex(0.70,0.92, Form("L_{int.} = %4.0f pb^{-1}", fLumiNorm));
}
