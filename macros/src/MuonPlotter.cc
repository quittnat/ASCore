/*****************************************************************************
*   Collection of tools for producing plots for Muon Fake Rate Analysis      *
*                                                                            *
*                                                  (c) 2010 Benjamin Stieger *
*****************************************************************************/
#include "MuonPlotter.hh"
#include "helper/AnaClass.hh"
#include "helper/Utilities.hh"
#include "helper/FPRatios.hh"
#include "helper/Monitor.hh"

#include "TLorentzVector.h"
#include "TGraphAsymmErrors.h"
#include "TEfficiency.h"

#include <iomanip>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////
// Global parameters:

static const float gMMU = 0.1057;
static const float gMEL = 0.0005;
static const float gMZ  = 91.2;

static const TEfficiency::EStatOption gStatOpt = TEfficiency::kBBayesian; // kBBayesian (alpha, beta), kBUniform (1,1), kBJeffrey (0.5, 0.5)
static const double gStatBetaAlpha = 1.;
static const double gStatBetaBeta  = 1.;

// Regions ///////////////////////////////////////////////////////////////////////
TString MuonPlotter::Region::sname [MuonPlotter::gNREGIONS] = {"BaseLine", "HT200MET30", "HT400MET50", "HT400MET120", "HT200MET120", "HT80MET100"};
float MuonPlotter::Region::minMu1pt[MuonPlotter::gNREGIONS] = {       20.,          20.,          20.,           20.,           20.,          20.};
float MuonPlotter::Region::minMu2pt[MuonPlotter::gNREGIONS] = {       10.,          10.,          10.,           10.,           10.,          10.};
float MuonPlotter::Region::minEl1pt[MuonPlotter::gNREGIONS] = {       20.,          20.,          20.,           20.,           20.,          20.};
float MuonPlotter::Region::minEl2pt[MuonPlotter::gNREGIONS] = {       10.,          10.,          10.,           10.,           10.,          10.};
float MuonPlotter::Region::minHT   [MuonPlotter::gNREGIONS] = {       80.,         200.,         400.,          400.,          200.,          80.};
float MuonPlotter::Region::minMet  [MuonPlotter::gNREGIONS] = {       30.,          30.,          50.,          120.,          120.,         100.};
int   MuonPlotter::Region::minNjets[MuonPlotter::gNREGIONS] = {       2  ,          2  ,          2  ,           2  ,           2  ,          2  };


// Muon Binning //////////////////////////////////////////////////////////////////
double MuonPlotter::gMuPtbins [gNMuPtbins+1]  = {10., 20., 30., 40., 50., 60.};
double MuonPlotter::gMuPt2bins[gNMuPt2bins+1] = {10., 20., 30., 40., 50., 60.};
double MuonPlotter::gMuEtabins[gNMuEtabins+1] = {-2.4, -1.5, -1.0, -0.5, 0., 0.5, 1.0, 1.5, 2.4};

// Electron Binning //////////////////////////////////////////////////////////////
double MuonPlotter::gElPtbins [gNElPtbins+1]  = {10., 20., 30., 40., 50., 60.};
double MuonPlotter::gElPt2bins[gNElPt2bins+1] = {10., 20., 30., 40., 50., 60.};
double MuonPlotter::gElEtabins[gNElEtabins+1] = {-2.4, -1.5, -0.85, 0., 0.85, 1.5, 2.4};
//////////////////////////////////////////////////////////////////////////////////

// NVrtx Binning //////////////////////////////////////////////////////////////
double MuonPlotter::gNVrtxBins[gNNVrtxBins+1]  = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
// Jet Pt Binning //////////////////////////////////////////////////////////////
double MuonPlotter::gJetPtBins[gNJetPtBins+1]  = {30., 40., 50., 60., 70., 80., 80., 90., 100., 120., 150.};
//////////////////////////////////////////////////////////////////////////////////

TString MuonPlotter::gEMULabel[2] = {"mu", "el"};
//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::gKinSelNames[gNKinSels] = {"LL", "TT", "Sig"};
TString MuonPlotter::KinPlots::var_name[MuonPlotter::gNKinVars] = {"HT", "MET", "NJets", "Pt1", "Pt2", "InvMassSF", "InvMassMM", "InvMassEE", "InvMassEM", "MT2"};
int     MuonPlotter::KinPlots::nbins[MuonPlotter::gNKinVars]    = { 20 ,   20 ,      8 ,   20 ,   20 ,        30  ,        30  ,        30  ,        30  ,   20 };
float   MuonPlotter::KinPlots::xmin[MuonPlotter::gNKinVars]     = {100.,    0.,      0.,   10.,   10.,        20. ,        20. ,        20. ,        20. ,    0.};
float   MuonPlotter::KinPlots::xmax[MuonPlotter::gNKinVars]     = {800.,  210.,      8.,  200.,  100.,       300. ,       300. ,       300. ,       300. ,  100.};
TString MuonPlotter::KinPlots::axis_label[MuonPlotter::gNKinVars] = {"H_{T} [GeV]",
                                                                     "ME_{T} [GeV]",
                                                                     "N_{Jets}",
                                                                     "P_{T} (l_{1}) [GeV]",
                                                                     "P_{T} (l_{2}) [GeV]",
                                                                     "m_{ll} (SF) [GeV]",
                                                                     "m_{#mu#mu} [GeV]",
                                                                     "m_{ee} [GeV]",
                                                                     "m_{ll} (OF) [GeV]",
                                                                     "M_{T2} [GeV]"};

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::gHWWSelNames[gNHWWSels] = {"", "AllCuts", "Barrel"};
TString MuonPlotter::HWWPlots::var_name[MuonPlotter::gNHWWVars] = {"MET", "NJets", "Pt1", "Pt2", "InvMassSF", "InvMassMM", "InvMassEE", "InvMassEM", "DPhi", "MT2"};
int     MuonPlotter::HWWPlots::nbins[MuonPlotter::gNHWWVars]    = {  20 ,      4 ,   20 ,   20 ,        30  ,        30  ,        30  ,        30  ,   20  ,   20 };
float   MuonPlotter::HWWPlots::xmin[MuonPlotter::gNHWWVars]     = {   0.,      0.,   10.,   10.,        20. ,        20. ,        20. ,        20. ,    0. ,    0.};
float   MuonPlotter::HWWPlots::xmax[MuonPlotter::gNHWWVars]     = { 200.,      4.,  200.,  100.,       100. ,       100. ,       100. ,       100. ,  3.15 ,  100.};
TString MuonPlotter::HWWPlots::axis_label[MuonPlotter::gNHWWVars] = {"ME_{T} [GeV]",
                                                                     "N_{Jets}",
                                                                     "P_{T} (l_{1}) [GeV]",
                                                                     "P_{T} (l_{2}) [GeV]",
                                                                     "m_{ll} (SF) [GeV]",
                                                                     "m_{#mu#mu} [GeV]",
                                                                     "m_{ee} [GeV]",
                                                                     "m_{ll} (OF) [GeV]",
                                                                     "#Delta#Phi_{ll}",
                                                                     "M_{T2} [GeV]"};

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::FRatioPlots::var_name[MuonPlotter::gNRatioVars] = {"NJets",  "HT", "MaxJPt", "NVertices", "ClosJetPt", "AwayJetPt"};
int     MuonPlotter::FRatioPlots::nbins[MuonPlotter::gNRatioVars]    = {     7 ,   20 ,      20 ,         9  ,        20  ,        20  };
float   MuonPlotter::FRatioPlots::xmin[MuonPlotter::gNRatioVars]     = {     1.,   50.,      30.,         0. ,        30. ,        30. };
float   MuonPlotter::FRatioPlots::xmax[MuonPlotter::gNRatioVars]     = {     8.,  500.,     300.,        18. ,       150. ,       300. };

//////////////////////////////////////////////////////////////////////////////////
TString MuonPlotter::IsoPlots::sel_name[MuonPlotter::gNSels] = {"Base", "SigSup"};
int     MuonPlotter::IsoPlots::nbins[MuonPlotter::gNSels]    = {20, 20};

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
}

//____________________________________________________________________________
void MuonPlotter::init(TString filename){
	if(fVerbose > 0) cout << "------------------------------------" << endl;
	if(fVerbose > 0) cout << "Initializing MuonPlotter ... " << endl;
	if(fVerbose > 0) if(fChargeSwitch == 1) cout << " ... using opposite-sign charge selection" << endl;

	Util::SetStyle();
	readSamples(filename);
	readVarNames("anavarnames.dat");
	fOutputFileName = fOutputDir + "Yields.root";

	fBinWidthScale = 10.; // Normalize Y axis to this binwidth
	fDoCounting = false; // Disable counters by default

	fCurrentRun = -1;

	// Cuts:
	fC_minMu1pt = 20.;
	fC_minMu2pt = 10.;
	fC_minEl1pt = 20.;
	fC_minEl2pt = 10.;
	fC_minHT    = 80.;
	fC_minMet   = 30.;
	fC_minNjets = 2;

	fMCSamples.push_back(fSampleMap["TTJets"]);
	fMCSamples.push_back(fSampleMap["SingleT_t"]);
	fMCSamples.push_back(fSampleMap["SingleT_tW"]);
	fMCSamples.push_back(fSampleMap["SingleT_s"]);
	fMCSamples.push_back(fSampleMap["WJets"]);
	fMCSamples.push_back(fSampleMap["DYJets"]);
	
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
	// fMCBG.push_back(WW);
	// fMCBG.push_back(WZ);
	// fMCBG.push_back(ZZ);
	fMCBG.push_back(VVTo4L);
	fMCBG.push_back(GVJets);

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

	// fMCBG.push_back(SSWWDPS);
	// fMCBG.push_back(SSWWSPSPos);
	// fMCBG.push_back(SSWWSPSNeg);
	fMCBGSig = fMCBG;
	fMCBGSig.push_back(LM0);

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
	// fMCBGMuEnr.push_back(WW);
	// fMCBGMuEnr.push_back(WZ);
	// fMCBGMuEnr.push_back(ZZ);
	fMCBGMuEnr.push_back(VVTo4L);
	fMCBGMuEnr.push_back(GVJets);
	fMCBGMuEnr.push_back(QCDMuEnr10);

	fMCBGMuEnrSig = fMCBGMuEnr;
	fMCBGMuEnrSig.push_back(LM0);

	// fMuData.push_back(DoubleMu1);
	fMuData.push_back(DoubleMu2);
	// fEGData.push_back(DoubleEle1);
	fEGData.push_back(DoubleEle2);
	// fMuEGData.push_back(MuEG1);
	fMuEGData.push_back(MuEG2);

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
			TFile *f = TFile::Open(StringValue);
			s->file = f;
			s->tree = (TTree*)f->Get("Analysis");
			if(s->tree == NULL){ cout << " Tree not found in file!" << endl; break; }

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
				cout << "   File:       " << (s->file)->GetName() << endl;
				cout << "   Events:     " << s->tree->GetEntries() << endl;
				cout << "   Lumi:       " << s->lumi << endl;
				cout << "   Color:      " << s->color << endl;
				cout << "   DataMC:     " << s->datamc << endl;
			}

			for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
				Region *R = &(s->region[r]);
				for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
					Channel *C;
					if(c == Muon){
						C = &R->mm;
						C->name  = "Mu/Mu";
						C->sname = "MM";
					}
					if(c == Electron){
						C = &R->ee;
						C->name  = "El/El";
						C->sname = "EE";
					}
					if(c == EMu){
						C = &R->em;
						C->name  = "El/Mu";
						C->sname = "EM";
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
	if(chan == Muon || chan == EMu) return gNMuPtbins;
	if(chan == Electron) return gNElPtbins;
}
const double *MuonPlotter::getPtBins  (gChannel chan){
	if(chan == Muon || chan == EMu) return gMuPtbins;
	if(chan == Electron) return gElPtbins;
}
const int     MuonPlotter::getNPt2Bins(gChannel chan){
	if(chan == Muon || chan == EMu) return gNMuPt2bins;
	if(chan == Electron) return gNElPt2bins;
}
const double *MuonPlotter::getPt2Bins (gChannel chan){
	if(chan == Muon || chan == EMu) return gMuPt2bins;
	if(chan == Electron) return gElPt2bins;
}
const int     MuonPlotter::getNEtaBins(gChannel chan){
	if(chan == Muon || chan == EMu) return gNMuEtabins;
	if(chan == Electron)            return gNElEtabins;
}
const double *MuonPlotter::getEtaBins (gChannel chan){
	if(chan == Muon || chan == EMu) return gMuEtabins;
	if(chan == Electron)            return gElEtabins;
}

//____________________________________________________________________________
void MuonPlotter::doAnalysis(){
	// sandBox();
	// printSyncExercise();

	if(readHistos(fOutputFileName) != 0) return;
	// // fLumiNorm = 43.5;
	// // fLumiNorm = 152.9;
	// // fLumiNorm = 191.1;
	// fLumiNorm = 410.;
	fLumiNorm = 1000.;
	// 
	makeTTbarClosure();
	// makeHWWPlots();
	// 
	// printOrigins();
	// makeMuIsolationPlots();
	// makeElIsolationPlots();
	// makeNT2KinPlots();
	// 
	// makeRatioPlots(Muon);
	// makeRatioPlots(Electron);
	// 
	// makeFRvsPtPlots(Muon,     SigSup);
	// makeFRvsPtPlots(Electron, SigSup);
	// makeFRvsPtPlots(Muon,     ZDecay);
	// makeFRvsPtPlots(Electron, ZDecay);
	// makeFRvsEtaPlots(Muon);
	// makeFRvsEtaPlots(Electron);
	// 
	// makeMufRatioPlots(false);
	// makeMupRatioPlots(false);
	// makeElfRatioPlots(false);
	// makeElpRatioPlots(false);
	// 
	// for(size_t i = 0; i < gNREGIONS; ++i){
	// 	TString outputname = fOutputDir + "DataPred_" + Region::sname[i] + ".txt";
	// 	makeIntPrediction(outputname, gRegion(i));
	// }

	// makeIntMCClosure( fOutputDir + "MCClosure.txt");	
	// makeMCClosurePlots(fMCBG);
	cout << endl;
}

//____________________________________________________________________________
void MuonPlotter::sandBox(){
	// TFile *pFile = TFile::Open(fOutputDir + "InvMass.root", "RECREATE");
	// fLumiNorm = 191.1;
	// 
	// TH1D *h_minv_sf = new TH1D("h_minv_sf", "Inv. Mass of same flavor dileptons",     1000, 0., 700.);
	// TH1D *h_minv_of = new TH1D("h_minv_sf", "Inv. Mass of opposite flavor dileptons", 1000, 0., 700.);
	// TH1D *h_minv_ee = new TH1D("h_minv_ee", "Inv. Mass of ee pairs",                  1000, 0., 700.);
	// TH1D *h_minv_mm = new TH1D("h_minv_mm", "Inv. Mass of mumu pairs",                1000, 0., 700.);
	// 
	// vector<int> samples;
	// samples.push_back(DoubleMu1);
	// samples.push_back(DoubleMu2);
	// samples.push_back(DoubleEle1);
	// samples.push_back(DoubleEle2);
	// samples.push_back(MuEG1);
	// samples.push_back(MuEG2);
	// 
	// for(size_t samp = 0; samp < samples.size(); ++samp){
	// 	Sample *S = fSamples[samp];
	// 	fCurrentSample = gSample(samp);
	// 	TTree *tree = S->tree;
	// 
	// 	// Event loop
	// 	tree->ResetBranchAddresses();
	// 	// Init(tree);
	// 	if(S->datamc == 0) Init(tree);
	// 	else InitMC(tree);
	// 
	// 	if (fChain == 0) return;
	// 	Long64_t nentries = fChain->GetEntriesFast();
	// 	Long64_t nbytes = 0, nb = 0;
	// 	for (Long64_t jentry=0; jentry<nentries;jentry++) {
	// 		printProgress(jentry, nentries, S->name);
	// 
	// 		Long64_t ientry = LoadTree(jentry);
	// 		if (ientry < 0) break;
	// 		nb = fChain->GetEntry(jentry);   nbytes += nb;
	// 
	// 		int ind1(-1), ind2(-1);
	// 		int type(0);
	// 		TLorentzVector pl1, pl2;
	// 		type = abs(isOSLLEvent(ind1, ind2));
	// 		if(type == 0) continue; //nothing found
	// 		if(type == 1){ // mumu
	// 			if(!mumuSignalTrigger()) continue;
	// 			pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
	// 			pl2.SetPtEtaPhiM(MuPt[ind2], MuEta[ind2], MuPhi[ind2], gMMU);
	// 			float mass = (pl1 + pl2).M();
	// 			h_minv_sf->Fill(mass);
	// 			h_minv_mm->Fill(mass);
	// 		}
	// 		if(type == 2){ // ee
	// 			if(!elelSignalTrigger()) continue;
	// 			pl1.SetPtEtaPhiM(ElPt[ind1], ElEta[ind1], ElPhi[ind1], gMEL);
	// 			pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);
	// 			float mass = (pl1 + pl2).M();
	// 			h_minv_sf->Fill(mass);
	// 			h_minv_ee->Fill(mass);
	// 		}
	// 		if(type == 3){ // emu
	// 			if(!elmuSignalTrigger()) continue;
	// 			pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
	// 			pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);
	// 			float mass = (pl1 + pl2).M();
	// 			h_minv_of->Fill(mass);
	// 		}
	// 	}
	// 	cout << endl;
	// }
	// 
	// h_minv_of->SetXTitle("m_{ll} [GeV]");
	// h_minv_sf->SetXTitle("m_{ll} [GeV]");
	// h_minv_mm->SetXTitle("m_{ll} [GeV]");
	// h_minv_ee->SetXTitle("m_{ll} [GeV]");
	// 
	// h_minv_sf->SetMarkerColor(kBlack);
	// h_minv_sf->SetMarkerStyle(20);
	// h_minv_sf->SetMarkerSize(1.0);
	// h_minv_sf->SetLineWidth(2);
	// h_minv_sf->SetLineColor(kBlack);
	// h_minv_sf->SetFillColor(kBlack);
	// 
	// h_minv_of->SetMarkerColor(kRed);
	// h_minv_of->SetMarkerStyle(20);
	// h_minv_of->SetMarkerSize(0.7);
	// h_minv_of->SetLineWidth(2);
	// h_minv_of->SetLineColor(kRed);
	// h_minv_of->SetFillColor(kRed);
	// 
	// h_minv_mm->SetLineWidth(1);
	// h_minv_mm->SetLineColor(kBlue);
	// h_minv_mm->SetFillColor(kBlue);
	// h_minv_mm->SetFillStyle(3004);
	// 
	// h_minv_ee->SetLineWidth(1);
	// h_minv_ee->SetLineColor(kGreen);
	// h_minv_ee->SetFillColor(kGreen);
	// h_minv_ee->SetFillStyle(3005);
	// 
	// printObject(h_minv_of, "Minv_OF", "Inv Mass OF Pairs", "PE X0", true);
	// printObject(h_minv_sf, "Minv_SF", "Inv Mass SF Pairs", "PE X0", true);
	// printObject(h_minv_mm, "Minv_MM", "Inv Mass MM Pairs", "hist",  true);
	// printObject(h_minv_ee, "Minv_EE", "Inv Mass EE Pairs", "hist",  true);
	// 
	// TLatex *lat = new TLatex();
	// lat->SetNDC(kTRUE);
	// lat->SetTextColor(kBlack);
	// lat->SetTextSize(0.04);
	// 
	// TLegend *leg = new TLegend(0.70,0.70,0.89,0.88);
	// leg->AddEntry(h_minv_sf, "SF Pairs", "p");
	// leg->AddEntry(h_minv_mm, "MM Pairs", "f");
	// leg->AddEntry(h_minv_ee, "EE Pairs", "f");
	// leg->AddEntry(h_minv_of, "OF Pairs", "p");
	// leg->SetFillStyle(0);
	// leg->SetTextFont(42);
	// leg->SetBorderSize(0);
	// 
	// THStack *stack = new THStack("hstack", "Stack");
	// stack->Add(h_minv_mm);
	// stack->Add(h_minv_ee);
	// 
	// TCanvas *c1 = new TCanvas("Minv", "Minv", 0, 0, 800, 600);
	// c1->cd();
	// gPad->SetLogy();
	// h_minv_sf->DrawCopy("PE X0");
	// h_minv_of->DrawCopy("PE X0 same");
	// stack->Draw("hist same");
	// // h_minv_mm->DrawCopy("hist same");
	// // h_minv_ee->DrawCopy("hist same");
	// leg->Draw();
	// gPad->RedrawAxis();
	// 
	// lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
	// // lat->DrawLatex(0.11,0.92, name);
	// 
	// Util::PrintNoEPS(c1, "MinvCombined", fOutputDir + fOutputSubDir, pFile);
	// 
	// delete h_minv_of, h_minv_sf, h_minv_mm, h_minv_ee;
	// delete c1;
	// delete lat, lat;
	// pFile->Close();
	// fOutputSubDir = "";
}

//____________________________________________________________________________
void MuonPlotter::doLoop(){
	fDoCounting = true;
	TString eventfilename  = fOutputDir + "SSDLSignalEvents.txt";
	TString eventfilename2 = fOutputDir + "HWWSignalEvents.txt";
	fOUTSTREAM.open(eventfilename.Data(), ios::trunc);
	fOUTSTREAM2.open(eventfilename2.Data(), ios::trunc);

	TFile *pFile = new TFile(fOutputFileName, "RECREATE");

	// Sample loop
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		fCurrentSample = i;
		
		bookHistos(S);
		
		// if(i > DoubleMu2) continue;
		
		TTree *tree = S->tree;
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
			
			fCounters[fCurrentSample][Muon]    .fill("All events");
			fCounters[fCurrentSample][EMu]     .fill("All events");
			fCounters[fCurrentSample][Electron].fill("All events");

			// Select mutually exclusive runs for Jet and MultiJet datasets
			if(!isGoodRun(i)) continue;

			fCounters[fCurrentSample][Muon]    .fill(" ... is good run");
			fCounters[fCurrentSample][EMu]     .fill(" ... is good run");
			fCounters[fCurrentSample][Electron].fill(" ... is good run");

			for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)) fillYields(S, r);

			// Revert event selections to baseline:
			fC_minMu1pt = Region::minMu1pt[Baseline];
			fC_minMu2pt = Region::minMu2pt[Baseline];
			fC_minEl1pt = Region::minEl1pt[Baseline];
			fC_minEl2pt = Region::minEl2pt[Baseline];
			fC_minHT    = Region::minHT   [Baseline];
			fC_minMet   = Region::minMet  [Baseline];
			fC_minNjets = Region::minNjets[Baseline];

			fillRatioPlots(S);
			fillHWWPlots(S);
			fillKinPlots(i);
			fillMuIsoPlots(i);
			fillElIsoPlots(i);
		}
		cout << endl;

		// Stuff to execute for each sample AFTER looping on the events
		writeHistos(S, pFile);
		deleteHistos(S);
	}

	pFile->Write();
	pFile->Close();

	fOUTSTREAM.close();
	fOUTSTREAM2.close();
	printCutFlows(fOutputDir + "CutFlow.txt");
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
	makeNT012Plots(samples, Electron, Baseline);
	makeNT012Plots(samples, EMu,      Baseline);
}
void MuonPlotter::makeNT012Plots(vector<int> mcsamples, gChannel chan, gRegion reg){
	TString name;
	if(chan == Muon)     name = "MuMu";
	if(chan == Electron) name = "ElEl";
	if(chan == EMu)      name = "ElMu";

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
		if(chan == Muon)     cha = &S->region[reg].mm;
		if(chan == Electron) cha = &S->region[reg].ee;
		if(chan == EMu)      cha = &S->region[reg].em;
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
			tree = fSamples[index]->tree;
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
				if(chan == Electron){
					if( isTightElectron(ind1) &&  isTightElectron(ind2)) hnt20[i]->Fill(ElPt[ind1], scale);
					if( isTightElectron(ind1) && !isTightElectron(ind2)) hnt10[i]->Fill(ElPt[ind1], scale);
					if(!isTightElectron(ind1) &&  isTightElectron(ind2)) hnt10[i]->Fill(ElPt[ind2], scale);
					if(!isTightElectron(ind1) && !isTightElectron(ind2)) hnt00[i]->Fill(ElPt[ind1], scale);
				}
				if(chan == EMu){
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
void MuonPlotter::makeMufRatioPlots(bool output){
	if(fVerbose>1) cout << "MuonPlotter::makeMufRatioPlots() ..." << endl;
	fOutputSubDir = "Ratios/Muon";
	TH1D *h_fdata1 = fillMuRatioPt(fMuData,    SigSup, output); // DoubleMu dataset
	// TH1D *h_fdata2 = fillMuRatioPt(DoubleMu1, SigSup, output); // temp, for compilation
	TH1D *h_fallmc = fillMuRatioPt(fMCBGMuEnr, SigSup, output);      // QCD MC
	TH1D *h_fttbar = fillMuRatioPt(TTJets,   0, &MuonPlotter::isSigSupMuEvent, &MuonPlotter::isFakeMuon); // TTbarJets MC
	h_fdata1->SetName("MufRatioData");
	// h_fdata2->SetName("MufRatioDataMu");
	h_fttbar->SetName("MufRatioTTbar");
	h_fallmc->SetName("MufRatioAllMC");

	setPlottingRange(h_fdata1, h_fttbar, h_fallmc);

	// h_fdata1->SetMinimum(0.);
	// h_fdata2->SetMinimum(0.);
	// h_fttbar->SetMinimum(0.);
	// h_fallmc->SetMinimum(0.);

	h_fdata1->SetMarkerColor(kBlack);
	// h_fdata2->SetMarkerColor(kBlue);
	h_fttbar->SetMarkerColor(kBlue);
	h_fallmc->SetMarkerColor(kRed);

	h_fdata1->SetMarkerStyle(20);
	// h_fdata2->SetMarkerStyle(20);
	h_fttbar->SetMarkerStyle(20);
	h_fallmc->SetMarkerStyle(20);

	// h_fdata1->SetMarkerSize(2);
	// h_fdata2->SetMarkerSize(2);
	// h_fttbar->SetMarkerSize(2);
	// h_fallmc->SetMarkerSize(2);

	h_fdata1->SetLineWidth(2);
	// h_fdata2->SetLineWidth(2);
	h_fttbar->SetLineWidth(2);
	h_fallmc->SetLineWidth(2);

	h_fdata1->SetLineColor(kBlack);
	// h_fdata2->SetLineColor(kBlue);
	h_fttbar->SetLineColor(kBlue);
	h_fallmc->SetLineColor(kRed);

	h_fdata1->SetFillColor(kBlack);
	// h_fdata2->SetFillColor(kBlue);
	h_fttbar->SetFillColor(kBlue);
	h_fallmc->SetFillColor(kRed);

	plotRatioOverlay3H(h_fdata1, "Data (Jet, L = 21.7 pb^{-1})", h_fttbar, "t#bar{t} Fake GenMatch", h_fallmc, "QCD, t#bar{t}+jets, V+jets");
	// setPlottingRange(h_fdata1, h_fdata2);
	// plotRatioOverlay2H(h_fdata1, "Data (Jet, L = 21.7 pb^{-1})", h_fdata2, "Data (Muon, L = 21.3 pb^{-1})");
	// plotRatioOverlay3H(h_fdata2, "Data (Mu, L = 21.3 pb^{-1})", h_fttbar, "t#bar{t} Fake GenMatch", h_fallmc, "QCD, t#bar{t}+jets, V+jets");

	vector<int> mcsamples = fMCBGSig;
	THStack *hntight_stack = new THStack("MufTightStack", "Stack of tight muons in sig. supp. selection");
	THStack *hnloose_stack = new THStack("MufLooseStack", "Stack of loose muons in sig. supp. selection");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D *hntight[nmcsamples];
	TH1D *hnloose[nmcsamples];

	for(size_t i = 0; i < mcsamples.size(); ++i){
		Sample *S = fSamples[mcsamples[i]];
		Channel *cha = &S->region[Baseline].mm;
		hntight[i] = cha->fntight->ProjectionX();
		hnloose[i] = cha->fnloose->ProjectionX();
		hntight[i]->SetFillColor(S->color);
		hnloose[i]->SetFillColor(S->color);
		float scale = fLumiNorm / S->lumi;
		hntight[i]->Scale(scale);
		hnloose[i]->Scale(scale);
		hntight_stack->Add(hntight[i]);
		hnloose_stack->Add(hnloose[i]);
	}
	hntight_stack->Draw();
	hntight_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnloose_stack->Draw();
	hnloose_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));

	TCanvas *c_tight = new TCanvas("MufStackTight", "Tight Stack", 0, 0, 800, 600);
	TCanvas *c_loose = new TCanvas("MufStackLoose", "Loose Stack", 0, 0, 800, 600);

	// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		int index = mcsamples[i];
		leg->AddEntry(hntight[i], fSamples[index]->sname.Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c_tight->cd();
	gPad->SetLogy();
	hntight_stack->Draw("hist");
	leg->Draw();

	c_loose->cd();
	gPad->SetLogy();
	hnloose_stack->Draw("hist");
	leg->Draw();

	Util::PrintNoEPS(c_tight, "MufRatioTightStack", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c_loose, "MufRatioLooseStack", fOutputDir + fOutputSubDir, fOutputFile);	
	fOutputSubDir = "";
}
void MuonPlotter::makeElfRatioPlots(bool output){
	if(fVerbose>1) cout << "MuonPlotter::makeElfRatioPlots() ..." << endl;
	fOutputSubDir = "Ratios/Electron";
	TH1D *h_fdata1 = fillElRatioPt(fEGData, SigSup, output);      // JetMET Dataset (Single Electron Selection)
	// TH1D *h_fdata2 = fillElRatioPt(DoubleEle1, SigSup, output); // temp for compilation
	TH1D *h_fallmc = fillElRatioPt(fMCBG,   SigSup, output);      // QCD MC
	h_fdata1->SetName("ElfRatioData");
	// h_fdata2->SetName("ElfRatioDataEl");
	h_fallmc->SetName("ElfRatioAllMC");

	setPlottingRange(h_fdata1, h_fallmc);
	// setPlottingRange(h_fdata1, h_fdata2, h_fallmc);

	// h_fdata1->SetMinimum(0.);
	// h_fdata2->SetMinimum(0.);
	// h_fallmc->SetMinimum(0.);

	h_fdata1->SetMarkerColor(kBlack);
	// h_fdata2->SetMarkerColor(kBlue);
	h_fallmc->SetMarkerColor(kRed);

	h_fdata1->SetMarkerStyle(20);
	// h_fdata2->SetMarkerStyle(20);
	h_fallmc->SetMarkerStyle(20);

	// h_fdata1->SetMarkerSize(2);
	// h_fdata2->SetMarkerSize(2);
	// h_fallmc->SetMarkerSize(2);

	h_fdata1->SetLineWidth(2);
	// h_fdata2->SetLineWidth(2);
	h_fallmc->SetLineWidth(2);

	h_fdata1->SetLineColor(kBlack);
	// h_fdata2->SetLineColor(kBlue);
	h_fallmc->SetLineColor(kRed);

	h_fdata1->SetFillColor(kBlack);
	// h_fdata2->SetFillColor(kBlue);
	h_fallmc->SetFillColor(kRed);

	plotRatioOverlay2H(h_fdata1, "Data (Jet, L = 35 pb^{-1})", h_fallmc, "QCD, t#bar{t}+jets, V+jets");
	// setPlottingRange(h_fdata1, h_fdata2);
	// plotRatioOverlay2H(h_fdata1, "Data (Jet, L = 35 pb^{-1})", h_fdata2, "Data (EGamma, L = 35 pb^{-1})");

	vector<int> mcsamples = fMCBGSig;
	THStack *hntight_stack = new THStack("ElfTightStack", "Stack of tight electrons in sig. supp. selection");
	THStack *hnloose_stack = new THStack("ElfLooseStack", "Stack of loose electrons in sig. supp. selection");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D *hntight[nmcsamples];
	TH1D *hnloose[nmcsamples];

	for(size_t i = 0; i < mcsamples.size(); ++i){
		Sample *S = fSamples[mcsamples[i]];
		Channel *cha = &S->region[Baseline].ee;
		hntight[i] = cha->fntight->ProjectionX();
		hnloose[i] = cha->fnloose->ProjectionX();
		hntight[i]->SetFillColor(S->color);
		hnloose[i]->SetFillColor(S->color);
		float scale = fLumiNorm / S->lumi;
		hntight[i]->Scale(scale);
		hnloose[i]->Scale(scale);
		hntight_stack->Add(hntight[i]);
		hnloose_stack->Add(hnloose[i]);
	}
	hntight_stack->Draw();
	hntight_stack->GetXaxis()->SetTitle(convertVarName("ElPt[0]"));
	hnloose_stack->Draw();
	hnloose_stack->GetXaxis()->SetTitle(convertVarName("ElPt[0]"));

	TCanvas *c_tight = new TCanvas("ElfStackTight", "Tight Stack", 0, 0, 800, 600);
	TCanvas *c_loose = new TCanvas("ElfStackLoose", "Loose Stack", 0, 0, 800, 600);

	// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		Sample *S = fSamples[mcsamples[i]];
		leg->AddEntry(hntight[i], S->sname.Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c_tight->cd();
	// gPad->SetLogy();
	hntight_stack->Draw("hist");
	leg->Draw();

	c_loose->cd();
	// gPad->SetLogy();
	hnloose_stack->Draw("hist");
	leg->Draw();

	Util::PrintNoEPS(c_tight, "ElfRatioTightStack", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c_loose, "ElfRatioLooseStack", fOutputDir + fOutputSubDir, fOutputFile);	
	fOutputSubDir = "";
}
void MuonPlotter::makeMupRatioPlots(bool output){
	if(fVerbose>1) cout << "MuonPlotter::makeMupRatioPlots() ..." << endl;
	fOutputSubDir = "Ratios/Muon";
	TH1D *h_pdata  = fillMuRatioPt(fMuData,    ZDecay, output);
	TH1D *h_pallmc = fillMuRatioPt(fMCBGMuEnr, ZDecay, output);
	TH1D *h_pttbar = fillMuRatioPt(TTJets, 0, &MuonPlotter::isGoodMuEvent, &MuonPlotter::isPromptMuon); // TTbar
	h_pdata ->SetName("MupRatioData");
	h_pttbar->SetName("MupRatioTTbar");
	h_pallmc->SetName("MupRatioAllMC");

	setPlottingRange(h_pdata, h_pttbar, h_pallmc);

	h_pdata ->Draw("goff");
	h_pttbar->Draw("goff");
	h_pallmc->Draw("goff");

	// h_pdata ->SetMinimum(0.);
	// h_pttbar->SetMinimum(0.);
	// h_pallmc->SetMinimum(0.);
	// h_pttbar->SetMaximum(1.3);
	// h_pallmc->SetMaximum(1.3);

	h_pdata ->SetLineWidth(2);
	h_pttbar->SetLineWidth(2);
	h_pallmc->SetLineWidth(2);

	h_pdata ->SetMarkerColor(kBlack);
	h_pttbar->SetMarkerColor(kBlue);
	h_pallmc->SetMarkerColor(kRed);

	h_pdata ->SetMarkerStyle(20);
	h_pttbar->SetMarkerStyle(20);
	h_pallmc->SetMarkerStyle(20);

	// h_pdata ->SetMarkerSize(2);
	// h_pttbar->SetMarkerSize(2);
	// h_pallmc->SetMarkerSize(2);

	h_pdata ->SetLineColor(kBlack);
	h_pttbar->SetLineColor(kBlue);
	h_pallmc->SetLineColor(kRed);

	h_pdata ->SetFillColor(kBlack);
	h_pttbar->SetFillColor(kBlue);
	h_pallmc->SetFillColor(kRed);

	h_pdata ->SetDrawOption("E1");
	h_pttbar->SetDrawOption("E1");
	h_pallmc->SetDrawOption("E1");

	// plotRatioOverlay3H(h_pdata, "Data (Jet, L = 21.7 pb^{-1})", h_pttbar, "t#bar{t} Prompt GenMatch", h_pallmc, "QCD, t#bar{t}+jets, V+jets");
	plotRatioOverlay3H(h_pdata, "Data (Mu, L = 21.3 pb^{-1})", h_pttbar, "t#bar{t} Prompt GenMatch", h_pallmc, "QCD, t#bar{t}+jets, V+jets");

	vector<int> mcsamples = fMCBGSig;
	THStack *hntight_stack = new THStack("MupTightStack", "Stack of tight muons in Z decay selection");
	THStack *hnloose_stack = new THStack("MupLooseStack", "Stack of loose muons in Z decay selection");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D *hntight[nmcsamples];
	TH1D *hnloose[nmcsamples];

	for(size_t i = 0; i < mcsamples.size(); ++i){
		Sample *S = fSamples[mcsamples[i]];
		Channel *cha = &S->region[Baseline].mm;
		hntight[i] = cha->pntight->ProjectionX();
		hnloose[i] = cha->pnloose->ProjectionX();
		hntight[i]->SetFillColor(S->color);
		hnloose[i]->SetFillColor(S->color);
		float scale = fLumiNorm / S->lumi;
		hntight[i]->Scale(scale);
		hnloose[i]->Scale(scale);
		hntight_stack->Add(hntight[i]);
		hnloose_stack->Add(hnloose[i]);
	}
	hntight_stack->Draw();
	hntight_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));
	hnloose_stack->Draw();
	hnloose_stack->GetXaxis()->SetTitle(convertVarName("MuPt[0]"));

	TCanvas *c_tight = new TCanvas("MupStackTight", "Tight Stack", 0, 0, 800, 600);
	TCanvas *c_loose = new TCanvas("MupStackLoose", "Loose Stack", 0, 0, 800, 600);

	TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	// TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		Sample *S = fSamples[mcsamples[i]];
		leg->AddEntry(hntight[i], S->sname.Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c_tight->cd();
	// gPad->SetLogy();
	hntight_stack->Draw("hist");
	leg->Draw();

	c_loose->cd();
	// gPad->SetLogy();
	hnloose_stack->Draw("hist");
	leg->Draw();

	Util::PrintNoEPS(c_tight, "MupRatioTightStack", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c_loose, "MupRatioLooseStack", fOutputDir + fOutputSubDir, fOutputFile);	
	fOutputSubDir = "";
}
void MuonPlotter::makeElpRatioPlots(bool output){
	if(fVerbose>1) cout << "MuonPlotter::makeElpRatioPlots() ..." << endl;
	fOutputSubDir = "Ratios/Electron";
	TH1D *h_pdata  = fillElRatioPt(fEGData, ZDecay, output);
	TH1D *h_pallmc = fillElRatioPt(fMCBG,   ZDecay, output);
	h_pdata ->SetName("ElpRatioData");
	h_pallmc->SetName("ElpRatioAllMC");

	setPlottingRange(h_pdata, h_pallmc);

	h_pdata ->Draw("goff");
	h_pallmc->Draw("goff");

	// h_pdata ->SetMinimum(0.);
	// h_pallmc->SetMinimum(0.);

	h_pdata ->SetLineWidth(2);
	h_pallmc->SetLineWidth(2);

	h_pdata ->SetMarkerColor(kBlack);
	h_pallmc->SetMarkerColor(kRed);

	h_pdata ->SetMarkerStyle(20);
	h_pallmc->SetMarkerStyle(20);

	h_pdata ->SetLineColor(kBlack);
	h_pallmc->SetLineColor(kRed);

	h_pdata ->SetFillColor(kBlack);
	h_pallmc->SetFillColor(kRed);

	h_pdata ->SetDrawOption("E1");
	h_pallmc->SetDrawOption("E1");

	setPlottingRange(h_pdata, h_pallmc);
	plotRatioOverlay2H(h_pdata, "Data (EG, L = 35 pb^{-1})", h_pallmc, "QCD, t#bar{t}+jets, V+jets");

	vector<int> mcsamples = fMCBGSig;
	THStack *hntight_stack = new THStack("ElpTightStack", "Stack of tight electrons in Z decay selection");
	THStack *hnloose_stack = new THStack("ElpLooseStack", "Stack of loose electrons in Z decay selection");
	const unsigned int nmcsamples = mcsamples.size();
	TH1D *hntight[nmcsamples];
	TH1D *hnloose[nmcsamples];

	for(size_t i = 0; i < mcsamples.size(); ++i){
		Sample *S = fSamples[mcsamples[i]];
		Channel *cha = &S->region[Baseline].ee;
		hntight[i] = cha->pntight->ProjectionX();
		hnloose[i] = cha->pnloose->ProjectionX();
		hntight[i]->SetFillColor(S->color);
		hnloose[i]->SetFillColor(S->color);
		float scale = fLumiNorm / S->lumi;
		hntight[i]->Scale(scale);
		hnloose[i]->Scale(scale);
		hntight_stack->Add(hntight[i]);
		hnloose_stack->Add(hnloose[i]);
	}
	hntight_stack->Draw();
	hntight_stack->GetXaxis()->SetTitle(convertVarName("ElPt[0]"));
	hnloose_stack->Draw();
	hnloose_stack->GetXaxis()->SetTitle(convertVarName("ElPt[0]"));

	TCanvas *c_tight = new TCanvas("ElpStackTight", "Tight Stack", 0, 0, 800, 600);
	TCanvas *c_loose = new TCanvas("ElpStackLoose", "Loose Stack", 0, 0, 800, 600);

	// TLegend *leg = new TLegend(0.13,0.60,0.38,0.88);
	TLegend *leg = new TLegend(0.75,0.60,0.89,0.88);
	for(size_t i = 0; i < nmcsamples; ++i){
		Sample *S = fSamples[mcsamples[i]];
		leg->AddEntry(hntight[i], S->sname.Data(), "f");
	}
	leg->SetFillStyle(0);
	leg->SetTextFont(42);
	leg->SetBorderSize(0);

	c_tight->cd();
	// gPad->SetLogy();
	hntight_stack->Draw("hist");
	leg->Draw();

	c_loose->cd();
	// gPad->SetLogy();
	hnloose_stack->Draw("hist");
	leg->Draw();

	Util::PrintNoEPS(c_tight, "ElpRatioTightStack", fOutputDir + fOutputSubDir, fOutputFile);
	Util::PrintNoEPS(c_loose, "ElpRatioLooseStack", fOutputDir + fOutputSubDir, fOutputFile);	
	fOutputSubDir = "";
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
	TTree *tree = fSamples[TTJets]->tree;

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
		if(MuGenMType[muind1] == 2 || MuGenMType[muind1] == 4) continue;

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
		////////////////////////////////////////////////////
	}
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
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.2f pb^{-1}", fLumiNorm));
		lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
		lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
		lat->SetTextColor(kRed);
		lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
		lat->SetTextColor(kBlack);
		

		Util::PrintNoEPS(c_temp, "Iso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir, NULL);

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
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.2f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			Util::PrintNoEPS(c_temp, Form("MuIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);				
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
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.2f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data}  = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}   = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			Util::PrintNoEPS(c_temp, Form("MuIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);				
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
	TTree *tree = fSamples[TTJets]->tree;

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
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.1f pb^{-1}", fLumiNorm));
		lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
		lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
		lat->SetTextColor(kRed);
		lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
		lat->SetTextColor(kBlack);

		Util::PrintNoEPS(c_temp, "Iso" + IsoPlots::sel_name[i], fOutputDir + fOutputSubDir, NULL);	

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
			lat->DrawLatex(0.20,0.92, Form("p_{T}(e) %3.0f - %3.0f GeV", getPt2Bins(Electron)[k], getPt2Bins(Electron)[k+1]));
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.1f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			Util::PrintNoEPS(c_temp, Form("ElIso%s_pt_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);				
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
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = %4.1f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.75,0.85, Form("R^{T/L}_{Data} = %4.2f", ratio_data));
			lat->DrawLatex(0.75,0.80, Form("R^{T/L}_{MC}  = %4.2f", ratio_mc));
			lat->SetTextColor(kRed);
			lat->DrawLatex(0.75,0.75, Form("R^{T/L}_{TTbar} = %4.2f", ratio_ttbar));
			lat->SetTextColor(kBlack);

			Util::PrintNoEPS(c_temp, Form("ElIso%s_nv_%d", IsoPlots::sel_name[i].Data(), k), fOutputDir + fOutputSubDir, NULL);				
		}
	}
}

void MuonPlotter::makeNT2KinPlots(){
	TString selname[3] = {"LooseLoose", "TightTight", "Signal"};

	for(size_t s = 0; s < 3; ++s){ // loop on selections
		fOutputSubDir = "KinematicPlots/" + selname[s];
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
				Int_t nbins     = fSamples[j]->kinplots[s].hvar[i]->GetNbinsX();
				Double_t binc   = fSamples[j]->kinplots[s].hvar[i]->GetBinContent(nbins);
				Double_t overfl = fSamples[j]->kinplots[s].hvar[i]->GetBinContent(nbins+1);
				fSamples[j]->kinplots[s].hvar[i]->SetBinContent(nbins, binc + overfl);
			}
		}

		for(size_t i = 0; i < gNKinVars; ++i){
			// Create plots
			vector<int> mcsamples = fMCBGMuEnr;
			if(i != 6) mcsamples = fMCBG;
			vector<int> datasamples;
			datasamples.push_back(DoubleMu1);
			datasamples.push_back(DoubleMu2);
			datasamples.push_back(DoubleEle1);
			datasamples.push_back(DoubleEle2);
			datasamples.push_back(MuEG1);
			datasamples.push_back(MuEG2);

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
				fSamples[j]->kinplots[s].hvar[i]->Scale(lumiscale);
			}

			// Fill data histo
			for(size_t j = 0; j < datasamples.size(); ++j){
				Sample *S = fSamples[datasamples[j]];
				hvar_data[i]->Add(S->kinplots[s].hvar[i]);
				hvar_data[i]->SetXTitle(KinPlots::axis_label[i]);
			}

			// // Scale to get equal integrals
			// float intscale(0.);
			// for(size_t j = 0; j < mcsamples.size();   ++j){
			// 	intscale += hvar[i][mcsamples[j]]->Integral();
			// }
			// intscale = hvar_data[i]->Integral() / intscale;
			// 
			// for(size_t j = 0; j < mcsamples.size();   ++j){
			// 	hvar[i][mcsamples[j]]->Scale(intscale);
			// }

			// Fill MC stacks
			for(size_t j = 0; j < mcsamples.size();   ++j){
				Sample *S = fSamples[mcsamples[j]];
				hvar_mc  [i]->Add(S->kinplots[s].hvar[i]);
				hvar_mc_s[i]->Add(S->kinplots[s].hvar[i]);
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
			for(size_t j = 0; j < mcsamples.size(); ++j) leg->AddEntry(fSamples[mcsamples[j]]->kinplots[s].hvar[i], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg->SetFillStyle(0);
			leg->SetTextFont(42);
			leg->SetBorderSize(0);

			gPad->SetLogy();
			hvar_mc_s[i]->Draw("hist");
			hvar_data[i]->DrawCopy("PE X0 same");
			leg->Draw();
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.11,0.92, selname[s]);

			if(i < 5)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");
			if(i == 5) lat->DrawLatex(0.31,0.92, "ee/#mu#mu");
			if(i == 6) lat->DrawLatex(0.31,0.92, "#mu#mu");
			if(i == 7) lat->DrawLatex(0.31,0.92, "ee");
			if(i == 8) lat->DrawLatex(0.31,0.92, "e#mu");
			if(i > 8)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");

			Util::PrintNoEPS(c_temp, KinPlots::var_name[i], fOutputDir + fOutputSubDir, NULL);
			delete c_temp;
			delete leg;
		}
	}
}

void MuonPlotter::makeFRvsPtPlots(gChannel chan, gFPSwitch fp){
	fOutputSubDir = "Ratios/";
	char cmd[100];
    sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
    system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Electron) name = "Electrons";

	TH1D *h_dummy1 = new TH1D("dummy1", "dummy1", getNEtaBins(chan), getEtaBins(chan));
	TH2D *h_dummy2 = new TH2D("dummy2", "dummy2", getNPt2Bins(chan), getPt2Bins(chan), getNEtaBins(chan), getEtaBins(chan));

	TH1D *h_ptratio_data = new TH1D("Ratio_data", "Tight/Loose Ratio in data", getNPt2Bins(chan), getPt2Bins(chan));
	TH1D *h_ptratio_mc   = new TH1D("Ratio_mc",   "Tight/Loose Ratio in MC",   getNPt2Bins(chan), getPt2Bins(chan));
	// h_ptratio_data->Sumw2();
	// h_ptratio_mc  ->Sumw2();

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Electron){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	calculateRatio(datasamples, chan, fp, h_dummy2, h_ptratio_data, h_dummy1);
	calculateRatio(mcsamples,   chan, fp, h_dummy2, h_ptratio_mc,   h_dummy1);

	float maximum = 0.4;
	if(fp == ZDecay) maximum = 1.1;
	h_ptratio_data->SetMaximum(maximum);
	h_ptratio_mc  ->SetMaximum(maximum);
	h_ptratio_data->SetMinimum(0.0);
	h_ptratio_mc  ->SetMinimum(0.0);

	if(chan == Muon)     h_ptratio_mc->SetXTitle(convertVarName("MuPt[0]"));
	if(chan == Electron) h_ptratio_mc->SetXTitle(convertVarName("ElPt[0]"));
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

	// gPad->SetLogy();
	h_ptratio_mc->DrawCopy("PE X0");
	h_ptratio_data->Draw("PE X0 same");
	leg->Draw();
	lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
	lat->DrawLatex(0.11,0.92, name);
	double ymean(0.), yrms(0.);
	getWeightedYMeanRMS(h_ptratio_data, ymean, yrms);
	lat->SetTextSize(0.03);
	lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));

	TString fpname = "f";
	if(fp == ZDecay) fpname = "p";
	
	Util::PrintNoEPS(c_temp, fpname + "PtRatio_" + name, fOutputDir + fOutputSubDir, NULL);
	delete h_ptratio_mc, h_ptratio_data;
	delete c_temp, lat, leg;
	fOutputSubDir = "";
}
void MuonPlotter::makeFRvsEtaPlots(gChannel chan){
	fOutputSubDir = "Ratios/";
	char cmd[100];
    sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
    system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Electron) name = "Electrons";

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
	if(chan == Electron){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	calculateRatio(datasamples, chan, SigSup, h_dummy2, h_dummy1, h_etaratio_data);
	calculateRatio(mcsamples,   chan, SigSup, h_dummy2, h_dummy1, h_etaratio_mc);

	h_etaratio_data->SetMaximum(0.3);
	h_etaratio_mc  ->SetMaximum(0.3);
	h_etaratio_data->SetMinimum(0.0);
	h_etaratio_mc  ->SetMinimum(0.0);

	if(chan == Muon)     h_etaratio_mc->SetXTitle(convertVarName("MuEta[0]"));
	if(chan == Electron) h_etaratio_mc->SetXTitle(convertVarName("ElEta[0]"));
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
	c_temp->cd();

	// gPad->SetLogy();
	h_etaratio_mc->DrawCopy("PE X0");
	h_etaratio_data->Draw("PE X0 same");
	leg->Draw();
	lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
	lat->DrawLatex(0.11,0.92, name);
	double ymean(0.), yrms(0.);
	getWeightedYMeanRMS(h_etaratio_data, ymean, yrms);
	lat->SetTextSize(0.03);
	lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));
	
	Util::PrintNoEPS(c_temp, "EtaRatio_" + name, fOutputDir + fOutputSubDir, NULL);
	delete h_etaratio_mc, h_etaratio_data;
	delete c_temp, lat, leg;
	fOutputSubDir = "";
}
void MuonPlotter::makeRatioPlots(gChannel chan){
	fOutputSubDir = "Ratios/";
	char cmd[100];
    sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
    system(cmd);

	TString name;
	if(chan == Muon)     name = "Muons";
	if(chan == Electron) name = "Electrons";

	vector<int> datasamples;
	vector<int> mcsamples;

	if(chan == Muon){
		datasamples = fMuData;
		mcsamples   = fMCBGMuEnr;
	}
	if(chan == Electron){
		datasamples = fEGData;
		mcsamples   = fMCBG;
	}

	// Customization
	TString axis_name[gNRatioVars] = {"N_{Jets}",  "H_{T} [GeV]", "P_{T}(Hardest Jet) [GeV]", "N_{Vertices}", "P_{T}(Closest Jet) [GeV]", "P_{T}(Away Jet) [GeV]"};

	for(size_t i = 0; i < gNRatioVars; ++i){
		TH1D *h_ratio_data = getFRatio(datasamples, chan, i);
		TH1D *h_ratio_mc   = getFRatio(mcsamples,   chan, i);
		h_ratio_data->SetName(Form("FRatio_%s_data", FRatioPlots::var_name[i].Data()));
		h_ratio_mc  ->SetName(Form("FRatio_%s_mc",   FRatioPlots::var_name[i].Data()));

		h_ratio_data->SetMaximum(0.3);
		h_ratio_mc  ->SetMaximum(0.3);
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
		lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
		lat->DrawLatex(0.11,0.92, name);
		double ymean(0.), yrms(0.);
		getWeightedYMeanRMS(h_ratio_data, ymean, yrms);
		lat->SetTextSize(0.03);
		lat->DrawLatex(0.25,0.92, Form("Mean ratio: %4.2f #pm %4.2f", ymean, yrms));
	
		Util::PrintNoEPS(c_temp, "FRatio_" + name + "_" + FRatioPlots::var_name[i], fOutputDir + fOutputSubDir, NULL);
		delete c_temp, leg, lat;
		delete h_ratio_data, h_ratio_mc;
	}
	fOutputSubDir = "";
}

void MuonPlotter::makeHWWPlots(){
	TString selname[3] = {"NoCuts", "N1Cuts", "N1CutsBarrel"};

	for(size_t s = 0; s < 2; ++s){ // loop on selections
		fOutputSubDir = "HWWPlots/" + selname[s];
		char cmd[100];
	    sprintf(cmd,"mkdir -p %s%s", fOutputDir.Data(), fOutputSubDir.Data());
	    system(cmd);
		
		TH1D    *hvar_data[gNHWWVars];
		TH1D    *hvar_mc  [gNHWWVars];
		THStack *hvar_mc_s[gNHWWVars];

		TLatex *lat = new TLatex();
		lat->SetNDC(kTRUE);
		lat->SetTextColor(kBlack);
		lat->SetTextSize(0.04);

		// Create histograms
		for(size_t i = 0; i < gNHWWVars; ++i){
			hvar_data[i] = new TH1D("Data_"          + HWWPlots::var_name[i], HWWPlots::var_name[i] + " in Data", HWWPlots::nbins[i], HWWPlots::xmin[i], HWWPlots::xmax[i]);
			hvar_mc[i]   = new TH1D("MC_"            + HWWPlots::var_name[i], HWWPlots::var_name[i] + " in MC"  , HWWPlots::nbins[i], HWWPlots::xmin[i], HWWPlots::xmax[i]);
			hvar_mc_s[i] = new THStack("MC_stacked_" + HWWPlots::var_name[i], HWWPlots::var_name[i] + " in MC");
		}

		// Adjust overflow bins:
		for(size_t i = 0; i < gNHWWVars; ++i){
			for(gSample j = sample_begin; j < gNSAMPLES; j=gSample(j+1)){
				Int_t nbins     = fSamples[j]->hwwplots[s].hvar[i]->GetNbinsX();
				Double_t binc   = fSamples[j]->hwwplots[s].hvar[i]->GetBinContent(nbins);
				Double_t overfl = fSamples[j]->hwwplots[s].hvar[i]->GetBinContent(nbins+1);
				fSamples[j]->hwwplots[s].hvar[i]->SetBinContent(nbins, binc + overfl);
			}
		}

		for(size_t i = 0; i < gNHWWVars; ++i){
			// Create plots
			vector<int> mcsamples = fMCBGMuEnr;
			if(i != 6) mcsamples = fMCBG;
			vector<int> datasamples;
			datasamples.push_back(DoubleMu1);
			datasamples.push_back(DoubleMu2);
			datasamples.push_back(DoubleEle1);
			datasamples.push_back(DoubleEle2);
			datasamples.push_back(MuEG1);
			datasamples.push_back(MuEG2);

			hvar_data[i]->SetXTitle(HWWPlots::axis_label[i]);
			hvar_data[i]->SetLineWidth(3);
			hvar_data[i]->SetLineColor(kBlack);
			hvar_data[i]->SetMarkerStyle(8);
			hvar_data[i]->SetMarkerColor(kBlack);
			hvar_data[i]->SetMarkerSize(1.2);

			// Scale by luminosity
			for(size_t j = 0; j < gNSAMPLES; ++j){
				float lumiscale = fLumiNorm / fSamples[j]->lumi;
				if(fSamples[j]->datamc == 0) continue;
				fSamples[j]->hwwplots[s].hvar[i]->Scale(lumiscale);
			}

			// Fill data histo
			for(size_t j = 0; j < datasamples.size(); ++j){
				Sample *S = fSamples[datasamples[j]];
				hvar_data[i]->Add(S->hwwplots[s].hvar[i]);
				hvar_data[i]->SetXTitle(HWWPlots::axis_label[i]);
			}

			// Scale to get equal integrals
			float intscale(0.);
			for(size_t j = 0; j < mcsamples.size();   ++j){
				intscale += fSamples[mcsamples[j]]->hwwplots[s].hvar[i]->Integral();
			}
			intscale = hvar_data[i]->Integral() / intscale;
			
			for(size_t j = 0; j < mcsamples.size();   ++j){
				fSamples[mcsamples[j]]->hwwplots[s].hvar[i]->Scale(intscale);
			}

			// Fill MC stacks
			for(size_t j = 0; j < mcsamples.size();   ++j){
				Sample *S = fSamples[mcsamples[j]];
				hvar_mc  [i]->Add(S->hwwplots[s].hvar[i]);
				hvar_mc_s[i]->Add(S->hwwplots[s].hvar[i]);
				hvar_mc_s[i]->Draw("goff");
				hvar_mc_s[i]->GetXaxis()->SetTitle(HWWPlots::axis_label[i]);
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

			TCanvas *c_temp = new TCanvas("C_" + HWWPlots::var_name[i], HWWPlots::var_name[i] + " in Data vs MC", 0, 0, 800, 600);
			c_temp->cd();

			// TLegend *leg = new TLegend(0.15,0.50,0.40,0.88);
			// TLegend *leg = new TLegend(0.70,0.30,0.90,0.68);
			TLegend *leg = new TLegend(0.75,0.50,0.89,0.88);
			leg->AddEntry(hvar_data[i], "Data","p");
			for(size_t j = 0; j < mcsamples.size(); ++j) leg->AddEntry(fSamples[mcsamples[j]]->hwwplots[s].hvar[i], fSamples[mcsamples[j]]->sname.Data(), "f");
			leg->SetFillStyle(0);
			leg->SetTextFont(42);
			leg->SetBorderSize(0);

			gPad->SetLogy();
			hvar_mc_s[i]->Draw("hist");
			hvar_data[i]->DrawCopy("PE X0 same");
			leg->Draw();
			lat->DrawLatex(0.70,0.92, Form("L_{int.} = ~%4.1f pb^{-1}", fLumiNorm));
			lat->DrawLatex(0.11,0.92, selname[s]);

			if(i < 4)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");
			if(i == 4) lat->DrawLatex(0.31,0.92, "ee/#mu#mu");
			if(i == 5) lat->DrawLatex(0.31,0.92, "#mu#mu");
			if(i == 6) lat->DrawLatex(0.31,0.92, "ee");
			if(i == 7) lat->DrawLatex(0.31,0.92, "e#mu");
			if(i > 7)  lat->DrawLatex(0.31,0.92, "ee/e#mu/#mu#mu");

			Util::PrintNoEPS(c_temp, HWWPlots::var_name[i], fOutputDir + fOutputSubDir, NULL);
			delete c_temp;
			delete leg;
		}
	}
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
	if(chan == Electron){
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
		TTree *tree = fSamples[sample]->tree;
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
			if(chan == Electron){
				if(isLooseElectron(index)) H_nloose->Fill(ElPt[index], ElEta[index], scale); // Tight or loose
				if(isTightElectron(index)) H_ntight->Fill(ElPt[index], ElEta[index], scale); // Tight
			}

		}
		cout << endl;

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
		printObject(h_2d,  sname + "Ratio"    + name, "Muon Fake Ratio vs pt/eta", "colz");
		printObject(h_pt,  sname + "RatioPt"  + name, "Muon Fake Ratio vs pt",     "PE1");
		printObject(h_eta, sname + "RatioEta" + name, "Muon Fake Ratio vs eta",    "PE1");
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
	calculateRatio(samples, Electron, SigSup, fH2D_ElfRatio, fH1D_ElfRatioPt, fH1D_ElfRatioEta);
	calculateRatio(samples, Electron, ZDecay, fH2D_ElpRatio, fH1D_ElpRatioPt, fH1D_ElpRatioEta);
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
	TH2D *h_2d  = new TH2D("ElRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Electron), getPt2Bins(Electron), getNEtaBins(Electron), getEtaBins(Electron));
	TH1D *h_pt  = new TH1D("ElRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Electron), getPt2Bins(Electron));
	TH1D *h_eta = new TH1D("ElRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Electron), getEtaBins(Electron));

	h_pt->SetXTitle(convertVarName("ElPt[0]"));
	h_pt->SetYTitle("# Tight / # Loose");
	h_pt->GetYaxis()->SetTitleOffset(1.2);

	calculateRatio(samples, Electron, fp, h_2d, h_pt, h_eta, output);
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
		printObject(h_2d,  TString("Ratio")    + name, "Fake Ratio vs pt/eta", "colz");
		printObject(h_pt,  TString("RatioPt")  + name, "Fake Ratio vs pt",     "PE1");
		printObject(h_eta, TString("RatioEta") + name, "Fake Ratio vs eta",    "PE1");
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
		if(chan == Electron) s_channel = "Electron";
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
		if(chan == Electron) s_channel = "Electron";
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
void MuonPlotter::getPassedTotal(vector<int> samples, gChannel chan, gFPSwitch fp, TH2D*& h_passed, TH2D*& h_total, bool output){
	// toggle: choose binning: 0: pt, default, 1: nvrtx, 2: closest jet pt
	if(fVerbose>2) cout << "---------------" << endl;
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];

		float scale = fLumiNorm / S->lumi;
		if(S->datamc == 0) scale = 1;

		Channel *C;
		if(chan == Muon)     C = &S->region[Baseline].mm;
		if(chan == Electron) C = &S->region[Baseline].ee;
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
		printObject(h_passed, TString("Passed") + name, "Passed vs pt vs eta", "colz");
		printObject(h_total,  TString("Total")  + name, "Total vs pt vs eta",  "colz");
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
		if(chan == Electron) RP = &S->ratioplots[1];
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
void MuonPlotter::makeSSMuMuPredictionPlots(vector<int> samples, bool output){
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
		Channel *C = &S->region[Baseline].mm;

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
void MuonPlotter::makeSSElElPredictionPlots(vector<int> samples, bool output){
	fOutputSubDir = "ElElPredictions";
	// Need filled electron ratios before calling this function!

	// Prediction: /////////////////////////////////////////////////////////////////////
	TH1D *H_nsigpred = new TH1D("ElElNsigpred", "Predicted N_sig in Pt1 bins",       getNPt2Bins(Electron),  getPt2Bins(Electron));
	TH1D *H_nfppred  = new TH1D("ElElNfppred",  "Predicted N_fp in Pt1 bins",        getNPt2Bins(Electron),  getPt2Bins(Electron));
	TH1D *H_nffpred  = new TH1D("ElElNffpred",  "Predicted N_ff in Pt1 bins",        getNPt2Bins(Electron),  getPt2Bins(Electron));
	TH1D *H_nFpred   = new TH1D("ElElNFpred",   "Total predicted fakes in Pt1 bins", getNPt2Bins(Electron),  getPt2Bins(Electron));
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm/S->lumi;

		Channel *C = &S->region[Baseline].ee;
		vector<TH1D*> prediction = ElElFPPrediction(fH2D_ElfRatio, fH2D_ElpRatio, C->nt20_pt, C->nt10_pt, C->nt00_pt, output);
		H_nsigpred->Add(prediction[0], scale);
		H_nfppred ->Add(prediction[1], scale);
		H_nffpred ->Add(prediction[2], scale);
	}

	// Observation: ////////////////////////////////////////////////////////////////////
	TH1D *H_nsigobs  = new TH1D("Nsigobs", "Observed N_sig in Pt1 bins",  getNPt2Bins(Electron),  getPt2Bins(Electron));
	vector<int> lm0sample; lm0sample.push_back(LM0);
	NObs(Electron, H_nsigobs, lm0sample, &MuonPlotter::isGenMatchedSUSYEEEvent);

	TH1D *H_nt2obs  = new TH1D("Nt2obs", "Observed N_t2 in Pt1 bins",  getNPt2Bins(Electron),  getPt2Bins(Electron));
	NObs(Electron, H_nt2obs, samples);
	// NObs(H_nt2obs, samples, &MuonPlotter::isSSTTEvent);

	TH1D *H_nt2obsSM = new TH1D("Nt2obsSM", "Observed N_t2 in Pt1 bins, ttbar only",  getNPt2Bins(Electron),  getPt2Bins(Electron));
	// vector<int> ttbarsample; ttbarsample.push_back(TTbar);
	NObs(Electron, H_nt2obsSM, fMCBG);
	// NObs(Electron, H_nt2obsttbar, ttbarsample, &MuonPlotter::isSSTTEvent);	

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
void MuonPlotter::makeSSElMuPredictionPlots(vector<int> samples, bool output){
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

		Channel *C = &S->region[Baseline].em;
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
	NObs(EMu, H_nsigobs, lm0sample, &MuonPlotter::isGenMatchedSUSYEMuEvent);

	TH1D *H_nt2obs  = new TH1D("Nt2obs", "Observed N_t2 in MuPt bins",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(EMu, H_nt2obs, samples);

	TH1D *H_nt2obsSM = new TH1D("Nt2obsSM", "Observed N_t2 in MuPt bins, ttbar only",  getNPt2Bins(Muon),  getPt2Bins(Muon));
	NObs(EMu, H_nt2obsSM, fMCBG);

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

		TTree *tree = S->tree;
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

			if((chan == Muon || chan == EMu) && NMus > 0) hist->Fill(MuPt[0], scale);
			if( chan == Electron             && NEls > 0) hist->Fill(ElPt[0], scale);
		}
	}	
}
void MuonPlotter::NObs(gChannel chan, TH1D *&hist, vector<int> samples, gRegion reg){
	hist->Sumw2();
	for(size_t i = 0; i < samples.size(); ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm / S->lumi;
		Channel *C;
		if(chan == Muon)     C = &S->region[reg].mm;
		if(chan == EMu)      C = &S->region[reg].em;
		if(chan == Electron) C = &S->region[reg].ee;
		hist->Add(C->nt20_pt->ProjectionX(), scale);
	}	
}
void MuonPlotter::NObs(gChannel chan, THStack *&stack, vector<int> samples, gRegion reg){
	// hist->Sumw2();
	const int nsamples = samples.size();
	TH1D *hnt2[nsamples];
	for(size_t i = 0; i < nsamples; ++i){
		Sample *S = fSamples[samples[i]];
		float scale = fLumiNorm / S->lumi;
		Channel *C;
		if(chan == Muon)     C = &S->region[reg].mm;
		if(chan == EMu)      C = &S->region[reg].em;
		if(chan == Electron) C = &S->region[reg].ee;
		hnt2[i] = (TH1D*)C->nt20_pt->ProjectionX()->Clone(Form("Nt2_%s", S->sname.Data())); // not sure if i need the clone here...
		hnt2[i]->SetFillColor(S->color);
		hnt2[i]->Sumw2();
		hnt2[i]->Scale(scale);
		stack->Add(hnt2[i]);
	}	
}

//____________________________________________________________________________
void MuonPlotter::makeIntPrediction(TString filename, gRegion reg){
	ofstream OUT(filename.Data(), ios::trunc);

	vector<int> musamples;
	vector<int> elsamples;
	vector<int> emusamples;

	// TODO: Check these samples!
	musamples = fMuData;
	elsamples = fEGData;
	emusamples = fMuEGData;

	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;
	OUT << " Producing integrated predictions" << endl;
	OUT << "  scaling MC to " << fLumiNorm << " /pb" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_data(0.),  mufratio_data_e(0.), mufratio_data_eup(0.);
	float mupratio_data(0.),  mupratio_data_e(0.), mupratio_data_eup(0.);
	float elfratio_data(0.),  elfratio_data_e(0.), elfratio_data_eup(0.);
	float elpratio_data(0.),  elpratio_data_e(0.), elpratio_data_eup(0.);
	float mufratio_allmc(0.), mufratio_allmc_e(0.);
	float mupratio_allmc(0.), mupratio_allmc_e(0.);
	float elfratio_allmc(0.), elfratio_allmc_e(0.);
	float elpratio_allmc(0.), elpratio_allmc_e(0.);

	calculateRatio(fMuData, Muon, SigSup, mufratio_data, mufratio_data_e);
	calculateRatio(fMuData, Muon, ZDecay, mupratio_data, mupratio_data_e);

	calculateRatio(fEGData, Electron, SigSup, elfratio_data, elfratio_data_e);
	calculateRatio(fEGData, Electron, ZDecay, elpratio_data, elpratio_data_e);

	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	calculateRatio(fMCBG,      Electron, SigSup, elfratio_allmc, elfratio_allmc_e);
	calculateRatio(fMCBG,      Electron, ZDecay, elpratio_allmc, elpratio_allmc_e);

	///////////////////////////////////////////////////////////////////////////////////
	// SYSTEMATICS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float olderror(0.), newerror(0.);
	// add here relative errors on the ratios
	float syst_m_p           = 0.10 * mupratio_data;
	float syst_e_p           = 0.10 * elpratio_data;
	float syst_eta_m_f       = 0.10 * mufratio_data;
	float syst_eta_e_f       = 0.10 * elfratio_data;
	float syst_nv_m_f        = 0.08 * mufratio_data;
	float syst_nv_e_f        = 0.15 * elfratio_data;
	float syst_intrinsic_m_f = 0.10 * mufratio_data;
	float syst_intrinsic_e_f = 0.10 * elfratio_data;

	// // add here absolute errors on the ratios
	// float syst_m_p           = 0.05;
	// float syst_e_p           = 0.05;
	// float syst_eta_m_f       = 0.02;
	// float syst_eta_e_f       = 0.01;
	// float syst_nv_m_f        = 0.02;
	// float syst_nv_e_f        = 0.04;
	// float syst_intrinsic_m_f = 0.03;
	// float syst_intrinsic_e_f = 0.05;

	// Add errors to ratios:
	// Mu f:
	olderror = mufratio_data_e;
	newerror = sqrt(olderror*olderror
	              + syst_eta_m_f*syst_eta_m_f
	              + syst_nv_m_f*syst_nv_m_f
	              + syst_intrinsic_m_f*syst_intrinsic_m_f);
	float mufratio_data_e_new = newerror;
	
	// El f:
	olderror = elfratio_data_e;
	newerror = sqrt(olderror*olderror
	              + syst_eta_e_f*syst_eta_e_f
	              + syst_nv_e_f*syst_nv_e_f
	              + syst_intrinsic_e_f*syst_intrinsic_e_f);
	float elfratio_data_e_new = newerror;
	
	// Mu p:
	olderror = mupratio_data_e;
	newerror = sqrt(olderror*olderror + syst_m_p*syst_m_p);
	float mupratio_data_e_new = newerror;
	
	// El p:
	olderror = elpratio_data_e;
	newerror = sqrt(olderror*olderror + syst_e_p*syst_m_p);
	float elpratio_data_e_new = newerror;
	///////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt2_mumu(0.),    nt10_mumu(0.),    nt0_mumu(0.);
	float nt2_mumu_e2(0.), nt10_mumu_e2(0.), nt0_mumu_e2(0.);
	float nt2_emu(0.),     nt10_emu(0.),     nt01_emu(0.),    nt0_emu(0.);
	float nt2_emu_e2(0.),  nt10_emu_e2(0.),  nt01_emu_e2(0.), nt0_emu_e2(0.);	
	float nt2_ee(0.),      nt10_ee(0.),      nt0_ee(0.);
	float nt2_ee_e2(0.),   nt10_ee_e2(0.),   nt0_ee_e2(0.);

	// OS yields
	float nt2_ee_BB_os(0.),    nt2_ee_EE_os(0.),    nt2_ee_EB_os(0.);
	float nt2_em_BB_os(0.),    nt2_em_EE_os(0.);

	for(size_t i = 0; i < musamples.size(); ++i){
		Sample *S = fSamples[musamples[i]];
		nt2_mumu     += S->numbers[reg][Muon].nt2;
		nt10_mumu    += S->numbers[reg][Muon].nt10;
		nt0_mumu     += S->numbers[reg][Muon].nt0;
		nt2_mumu_e2  += S->numbers[reg][Muon].nt2;
		nt10_mumu_e2 += S->numbers[reg][Muon].nt10;
		nt0_mumu_e2  += S->numbers[reg][Muon].nt0;
	}		
	for(size_t i = 0; i < emusamples.size(); ++i){
		Sample *S = fSamples[emusamples[i]];
		nt2_emu     += S->numbers[reg][EMu].nt2;
		nt10_emu    += S->numbers[reg][EMu].nt10;
		nt01_emu    += S->numbers[reg][EMu].nt01;
		nt0_emu     += S->numbers[reg][EMu].nt0;
		nt2_emu_e2  += S->numbers[reg][EMu].nt2;
		nt10_emu_e2 += S->numbers[reg][EMu].nt10;
		nt01_emu_e2 += S->numbers[reg][EMu].nt01;
		nt0_emu_e2  += S->numbers[reg][EMu].nt0;

		nt2_em_BB_os    += S->region[reg].em.nt20_OS_BB_pt->GetEntries(); // ele in barrel
		nt2_em_EE_os    += S->region[reg].em.nt20_OS_EE_pt->GetEntries(); // ele in endcal
	}		
	for(size_t i = 0; i < elsamples.size(); ++i){
		Sample *S = fSamples[elsamples[i]];
		nt2_ee     += S->numbers[reg][Electron].nt2;
		nt10_ee    += S->numbers[reg][Electron].nt10;
		nt0_ee     += S->numbers[reg][Electron].nt0;
		nt2_ee_e2  += S->numbers[reg][Electron].nt2;
		nt10_ee_e2 += S->numbers[reg][Electron].nt10;
		nt0_ee_e2  += S->numbers[reg][Electron].nt0;

		nt2_ee_BB_os += S->region[reg].ee.nt20_OS_BB_pt->GetEntries(); // both in barrel
		nt2_ee_EE_os += S->region[reg].ee.nt20_OS_EE_pt->GetEntries(); // both in endcal
		nt2_ee_EB_os += S->region[reg].ee.nt20_OS_EB_pt->GetEntries(); // one barrel, one endcap
	}		

	///////////////////////////////////////////////////////////////////////////////////
	// PREDICTIONS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	FPRatios *fMMFPRatios = new FPRatios();
	FPRatios *fEMFPRatios  = new FPRatios();
	FPRatios *fEEFPRatios   = new FPRatios();

	fMMFPRatios->SetVerbose(fVerbose);
	fEMFPRatios ->SetVerbose(fVerbose);
	fEEFPRatios  ->SetVerbose(fVerbose);

	fMMFPRatios->SetMuFratios(mufratio_data, mufratio_data_e_new);
	fMMFPRatios->SetMuPratios(mupratio_data, mupratio_data_e_new);

	fEEFPRatios  ->SetElFratios(elfratio_data, elfratio_data_e_new);
	fEEFPRatios  ->SetElPratios(elpratio_data, elpratio_data_e_new);

	fEMFPRatios ->SetFratios(elfratio_data, elfratio_data_e_new, mufratio_data, mufratio_data_e_new);
	fEMFPRatios ->SetPratios(elpratio_data, elpratio_data_e_new, mupratio_data, mupratio_data_e_new);


	// MuMu Channel
	vector<double> nt_mumu;
	nt_mumu.push_back(nt0_mumu);
	nt_mumu.push_back(nt10_mumu); // mu passes
	nt_mumu.push_back(nt2_mumu);

	fMMFPRatios->NevtTopol(0, 2, nt_mumu);

	vector<double> vpt, veta;
	vpt.push_back(30.); vpt.push_back(30.); // Fake pts and etas (first electron then muon)
	veta.push_back(0.); veta.push_back(0.);

	vector<double> MuMu_Nev   = fMMFPRatios->NevtPass(vpt, veta);
	vector<double> MuMu_Estat = fMMFPRatios->NevtPassErrStat();
	vector<double> MuMu_Esyst = fMMFPRatios->NevtPassErrSyst();

	// EMu Channel
	vector<double> nt_emu;
	nt_emu.push_back(nt0_emu);
	nt_emu.push_back(nt01_emu); // e passes
	nt_emu.push_back(nt10_emu); // mu passes
	nt_emu.push_back(nt2_emu);

	fEMFPRatios->NevtTopol(1, 1, nt_emu);

	vector<double> EMu_Nev      = fEMFPRatios->NevtPass(vpt, veta);
	vector<double> EMu_Estat = fEMFPRatios->NevtPassErrStat();
	vector<double> EMu_Esyst = fEMFPRatios->NevtPassErrSyst();

	// EE Channel
	vector<double> nt_ee;
	nt_ee.push_back(nt0_ee);
	nt_ee.push_back(nt10_ee); // el passes
	nt_ee.push_back(nt2_ee);

	fEEFPRatios->NevtTopol(2, 0, nt_ee);

	vector<double> EE_Nev   = fEEFPRatios->NevtPass(vpt, veta);
	vector<double> EE_Estat = fEEFPRatios->NevtPassErrStat();
	vector<double> EE_Esyst = fEEFPRatios->NevtPassErrSyst();

	float mm_tot_fakes    = MuMu_Nev[1]+MuMu_Nev[2];
	float mm_tot_fakes_e1 = TMath::Sqrt(MuMu_Estat[1]*MuMu_Estat[1] + MuMu_Estat[2]*MuMu_Estat[2]);
	float mm_tot_fakes_e2 = TMath::Sqrt(MuMu_Esyst[1]*MuMu_Esyst[1] + MuMu_Esyst[2]*MuMu_Esyst[2]);
	float em_tot_fakes    = EMu_Nev[1]+EMu_Nev[2]+EMu_Nev[3];
	float em_tot_fakes_e1 = TMath::Sqrt(EMu_Estat[1]*EMu_Estat[1] + EMu_Estat[2]*EMu_Estat[2] + EMu_Estat[3]*EMu_Estat[3]);
	float em_tot_fakes_e2 = TMath::Sqrt(EMu_Esyst[1]*EMu_Esyst[1] + EMu_Esyst[2]*EMu_Esyst[2] + EMu_Esyst[3]*EMu_Esyst[3]);
	float ee_tot_fakes    = EE_Nev[1]+EE_Nev[2];
	float ee_tot_fakes_e1 = TMath::Sqrt(EE_Estat[1]*EE_Estat[1] + EE_Estat[2]*EE_Estat[2]);
	float ee_tot_fakes_e2 = TMath::Sqrt(EE_Esyst[1]*EE_Esyst[1] + EE_Esyst[2]*EE_Esyst[2]);

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
	OUT << setw(16) << "    data no syst ||";
	OUT << setw(7)  << setprecision(2) << mufratio_data  << " +/- " << setw(7) << setprecision(2) << mufratio_data_e  << " |";
	OUT << setw(7)  << setprecision(2) << mupratio_data  << " +/- " << setw(7) << setprecision(2) << mupratio_data_e  << " ||";
	OUT << setw(7)  << setprecision(2) << elfratio_data  << " +/- " << setw(7) << setprecision(2) << elfratio_data_e  << " |";
	OUT << setw(7)  << setprecision(2) << elpratio_data  << " +/- " << setw(7) << setprecision(2) << elpratio_data_e  << " ||";
	OUT << endl;
	OUT << setw(16) << "        data     ||";
	OUT << setw(7)  << setprecision(2) << mufratio_data  << " +/- " << setw(7) << setprecision(2) << mufratio_data_e_new  << " |";
	OUT << setw(7)  << setprecision(2) << mupratio_data  << " +/- " << setw(7) << setprecision(2) << mupratio_data_e_new  << " ||";
	OUT << setw(7)  << setprecision(2) << elfratio_data  << " +/- " << setw(7) << setprecision(2) << elfratio_data_e_new  << " |";
	OUT << setw(7)  << setprecision(2) << elpratio_data  << " +/- " << setw(7) << setprecision(2) << elpratio_data_e_new  << " ||";
	OUT << endl;
	OUT << "---------------------------------------------------------------------------------------------------------" << endl << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "         YIELDS  ||   Nt2   |   Nt1   |   Nt0   ||   Nt2   |   Nt10  |   Nt01  |   Nt0   ||   Nt2   |   Nt1   |   Nt0   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;

	float nt2sum_mumu(0.), nt10sum_mumu(0.), nt0sum_mumu(0.);
	float nt2sum_emu(0.), nt10sum_emu(0.), nt01sum_emu(0.), nt0sum_emu(0.);
	float nt2sum_ee(0.), nt10sum_ee(0.), nt0sum_ee(0.);
	for(size_t i = 0; i < fMCBG.size(); ++i){
		Sample *S = fSamples[fMCBG[i]];
		float scale = fLumiNorm / S->lumi;
		nt2sum_mumu  += scale*S->numbers[reg][Muon]    .nt2;
		nt10sum_mumu += scale*S->numbers[reg][Muon]    .nt10;
		nt0sum_mumu  += scale*S->numbers[reg][Muon]    .nt0;
		nt2sum_emu   += scale*S->numbers[reg][EMu]     .nt2;
		nt10sum_emu  += scale*S->numbers[reg][EMu]     .nt10;
		nt01sum_emu  += scale*S->numbers[reg][EMu]     .nt01;
		nt0sum_emu   += scale*S->numbers[reg][EMu]     .nt0;
		nt2sum_ee    += scale*S->numbers[reg][Electron].nt2;
		nt10sum_ee   += scale*S->numbers[reg][Electron].nt10;
		nt0sum_ee    += scale*S->numbers[reg][Electron].nt0;

		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon]    .nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon]    .nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Muon]    .nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][EMu]     .nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][EMu]     .nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][EMu]     .nt01 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][EMu]     .nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Electron].nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Electron].nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[reg][Electron].nt0  << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "MC sum" << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_mumu  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_mumu ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_mumu  ) << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_emu   ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_emu  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt01sum_emu  ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_emu   ) << " || ";
	OUT << setw(7) << Form("%5.1f", nt2sum_ee    ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt10sum_ee   ) << " | ";
	OUT << setw(7) << Form("%5.1f", nt0sum_ee    ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		if(S->datamc < 2) continue;
		float scale = fLumiNorm / S->lumi;
		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Muon]    .nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Muon]    .nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Muon]    .nt0  ) << " || ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][EMu]     .nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][EMu]     .nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][EMu]     .nt01 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][EMu]     .nt0  ) << " || ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Electron].nt2  ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Electron].nt10 ) << " | ";
		OUT << setw(7)  << Form("%5.2f", fLumiNorm / S->lumi * S->numbers[reg][Electron].nt0  ) << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "data"  << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_mumu ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_mumu) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_mumu ) << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_emu  ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_emu ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt01_emu ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_emu  ) << " || ";
	OUT << setw(7) << Form("%4.0f", nt2_ee   ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt10_ee  ) << " | ";
	OUT << setw(7) << Form("%4.0f", nt0_ee   ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "tot. pred. fakes"  << " || ";
	OUT << setw(7) << Form("%5.2f", mm_tot_fakes ) << " | ";
	OUT << setw(7) << "        |";
	OUT << setw(7) << "         || ";
	OUT << setw(7) << Form("%5.2f", em_tot_fakes ) << " | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        || ";
	OUT << setw(7) << Form("%5.2f", ee_tot_fakes ) << " | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl;
	OUT << "  PREDICTIONS" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " Mu/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << MuMu_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << MuMu_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[1] << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << MuMu_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[2] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << mm_tot_fakes;
	OUT << " +/- "             << setw(7) << setprecision(3) << mm_tot_fakes_e1;
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << mm_tot_fakes_e2 << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << EMu_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << EMu_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[1] << " (syst)" << endl;
	OUT << "  Npf:           " << setw(7) << setprecision(3) << EMu_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[2] << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << EMu_Nev[3];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[3];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[3] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << em_tot_fakes;
	OUT << " +/- "             << setw(7) << setprecision(3) << em_tot_fakes_e1;
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << em_tot_fakes_e2 << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/E Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << EE_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << EE_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[1] << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << EE_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[2] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << ee_tot_fakes;
	OUT << " +/- "             << setw(7) << setprecision(3) << ee_tot_fakes_e1;
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << ee_tot_fakes_e2 << " (syst)" << endl;
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
	nt2_ee_chmid_e1 = sqrt( (4*fb*fb*nt2_ee_BB_os) + (4*fe*fe*nt2_ee_EE_os) + (fb+fe)*(fb+fe)*nt2_ee_EB_os ); // stat only
	nt2_ee_chmid_e2 = sqrt( (4*nt2_ee_BB_os*nt2_ee_BB_os*fbE*fbE) + (4*nt2_ee_EE_os*nt2_ee_EE_os*feE*feE) + (fbE*fbE+feE*feE)*nt2_ee_EB_os*nt2_ee_EB_os ); // syst only

	nt2_em_chmid    = fb*nt2_em_BB_os + fe*nt2_em_EE_os;
	nt2_em_chmid_e1 = sqrt( fb*fb*nt2_em_BB_os + fe*fe*nt2_em_EE_os );
	nt2_em_chmid_e2 = sqrt( nt2_em_BB_os*nt2_em_BB_os * fbE*fbE + nt2_em_EE_os*nt2_em_EE_os * feE*feE );


	// nt2_ee_chmid_e = 2*sqrt( (nt2_ee_BB_os*fbE)*(nt2_ee_BB_os*fbE) + fb*fb*nt2_ee_BB_os 
	//                        + (nt2_ee_EE_os*feE)*(nt2_ee_EE_os*feE) + fe*fe*nt2_ee_EE_os
	// 			           + 0.25*( (fbE*fbE + feE*feE)*(nt2_ee_EB_os*nt2_ee_EB_os) + (fe+fb)*(fe+fb)*nt2_ee_EB_os) );
	// nt2_em_chmid_e = sqrt( (fbE*fbE * nt2_em_BB_os*nt2_em_BB_os) + fb*fb*nt2_em_BB_os
	//                      + (feE*feE * nt2_em_EE_os*nt2_em_EE_os) + fe*fe*nt2_em_EE_os );

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
		
		mc_os_em_bb_sum += scale*S->region[reg].em.nt20_OS_BB_pt->GetEntries();
		mc_os_em_ee_sum += scale*S->region[reg].em.nt20_OS_EE_pt->GetEntries();
		mc_os_ee_bb_sum += scale*S->region[reg].ee.nt20_OS_BB_pt->GetEntries();
		mc_os_ee_eb_sum += scale*S->region[reg].ee.nt20_OS_EB_pt->GetEntries();
		mc_os_ee_ee_sum += scale*S->region[reg].ee.nt20_OS_EE_pt->GetEntries();

		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg].em.nt20_OS_BB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg].em.nt20_OS_EE_pt->GetEntries() << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg].ee.nt20_OS_BB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg].ee.nt20_OS_EB_pt->GetEntries() << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->region[reg].ee.nt20_OS_EE_pt->GetEntries() << " || ";
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
	OUT << setw(7) << Form("%5.3f",   fb   * nt2_em_BB_os ) << " | ";
	OUT << setw(7) << Form("%5.3f",   fe   * nt2_em_EE_os ) << " || ";
	OUT << setw(7) << Form("%5.3f", 2*fb   * nt2_ee_BB_os ) << " | ";
	OUT << setw(7) << Form("%5.3f", (fb+fe)* nt2_ee_EB_os ) << " | ";
	OUT << setw(7) << Form("%5.3f", 2*fe   * nt2_ee_EE_os ) << " || ";
	OUT << endl;
	OUT << "-----------------------------------------------------------------------" << endl << endl;
	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;


	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << "       SUMMARY   ||           Mu/Mu           ||           E/Mu            ||           E/E             ||" << endl;
	OUT << "==========================================================================================================" << endl;
	OUT << setw(16) << "pred. fakes"  << " || ";
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes_e2) << " || ";
	OUT << setw(5) << Form("%5.2f", em_tot_fakes   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", em_tot_fakes_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", em_tot_fakes_e2) << " || ";
	OUT << setw(5) << Form("%5.2f", ee_tot_fakes   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", ee_tot_fakes_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", ee_tot_fakes_e2) << " || " << endl;
	OUT << setw(16) << "pred. chmisid "  << " || ";
	OUT << "                          || ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_em_chmid_e2) << " || ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", nt2_ee_chmid_e2) << " || " << endl;
	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "tot. backgr. "  << " || ";
	float em_tot_sqerr1 = em_tot_fakes_e1*em_tot_fakes_e1 + nt2_em_chmid_e1*nt2_em_chmid_e1;
	float em_tot_sqerr2 = em_tot_fakes_e2*em_tot_fakes_e2 + nt2_em_chmid_e2*nt2_em_chmid_e2;
	float ee_tot_sqerr1 = ee_tot_fakes_e1*ee_tot_fakes_e1 + nt2_ee_chmid_e1*nt2_ee_chmid_e1;
	float ee_tot_sqerr2 = ee_tot_fakes_e2*ee_tot_fakes_e2 + nt2_ee_chmid_e2*nt2_ee_chmid_e2;
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes_e1) << " +/- ";
	OUT << setw(5) << Form("%5.2f", mm_tot_fakes_e2) << " || ";
	OUT << setw(5) << Form("%5.2f", em_tot_fakes + nt2_em_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(em_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(em_tot_sqerr2)) << " || ";
	OUT << setw(5) << Form("%5.2f", ee_tot_fakes + nt2_ee_chmid   ) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(ee_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(ee_tot_sqerr2)) << " || " << endl;
	OUT << "----------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "observed"  << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_mumu ) << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_emu  ) << " || ";
	OUT << setw(25) << left << Form("%2.0f", nt2_ee   ) << " || " << endl;
	OUT << "==========================================================================================================" << endl;
	OUT << setw(20) << "combined observed: ";
	OUT << setw(5) << left << Form("%2.0f", nt2_mumu+nt2_emu+nt2_ee ) << endl;
	OUT << setw(20) << "        predicted: ";
	OUT << setw(5) << left << Form("%5.2f", mm_tot_fakes +  em_tot_fakes + nt2_em_chmid + ee_tot_fakes + nt2_ee_chmid ) << " +/- ";
	float comb_tot_sqerr1 = mm_tot_fakes_e1*mm_tot_fakes_e1 + em_tot_sqerr1 + ee_tot_sqerr1;
	float comb_tot_sqerr2 = mm_tot_fakes_e2*mm_tot_fakes_e2 + em_tot_sqerr2 + ee_tot_sqerr2;
	OUT << setw(5) << Form("%5.2f", sqrt(comb_tot_sqerr1)) << " +/- ";
	OUT << setw(5) << Form("%5.2f", sqrt(comb_tot_sqerr2)) << endl;
	OUT << "==========================================================================================================" << endl;

	OUT.close();
	delete fMMFPRatios;
	delete fEMFPRatios;
	delete fEEFPRatios;
}
void MuonPlotter::makeIntMCClosure(TString filename){
	ofstream OUT(filename.Data(), ios::trunc);

	fLumiNorm = 1000.;
	vector<int> musamples;
	vector<int> elsamples;
	vector<int> emusamples;

	// TODO: Check these samples!
	// musamples = fMCBGMuEnr;
	musamples.push_back(TTJets);
	musamples.push_back(WJets);
	musamples.push_back(DYJets);
	musamples.push_back(GJets40);
	musamples.push_back(GJets100);
	musamples.push_back(GJets200);
	musamples.push_back(WW);
	musamples.push_back(WZ);
	musamples.push_back(ZZ);
	musamples.push_back(QCD50MG);
	musamples.push_back(QCD100MG);
	musamples.push_back(QCD250MG);
	musamples.push_back(QCD500MG);
	musamples.push_back(QCD1000MG);

	elsamples.push_back(TTJets);
	elsamples.push_back(WJets);
	elsamples.push_back(DYJets);
	elsamples.push_back(GJets40);
	elsamples.push_back(GJets100);
	elsamples.push_back(GJets200);
	elsamples.push_back(WW);
	elsamples.push_back(WZ);
	elsamples.push_back(ZZ);
	elsamples.push_back(QCD50MG);
	elsamples.push_back(QCD100MG);
	elsamples.push_back(QCD250MG);
	elsamples.push_back(QCD500MG);
	elsamples.push_back(QCD1000MG);

	emusamples.push_back(TTJets);
	emusamples.push_back(WJets);
	emusamples.push_back(DYJets);
	emusamples.push_back(GJets40);
	emusamples.push_back(GJets100);
	emusamples.push_back(GJets200);
	emusamples.push_back(WW);
	emusamples.push_back(WZ);
	emusamples.push_back(ZZ);
	emusamples.push_back(QCD50MG);
	emusamples.push_back(QCD100MG);
	emusamples.push_back(QCD250MG);
	emusamples.push_back(QCD500MG);
	emusamples.push_back(QCD1000MG);

	// musamples = fMCBG;
	// elsamples = fMCBG;
	// emusamples = fMCBG;

	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;
	OUT << " Producing integrated predictions" << endl;
	OUT << "  scaling MC to " << fLumiNorm << " /pb" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_allmc(0.), mufratio_allmc_e(0.);
	float mupratio_allmc(0.), mupratio_allmc_e(0.);
	float elfratio_allmc(0.), elfratio_allmc_e(0.);
	float elpratio_allmc(0.), elpratio_allmc_e(0.);

	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	calculateRatio(fMCBG,      Electron, SigSup, elfratio_allmc, elfratio_allmc_e);
	calculateRatio(fMCBG,      Electron, ZDecay, elpratio_allmc, elpratio_allmc_e);


	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt2_mumu(0.),    nt10_mumu(0.),    nt0_mumu(0.);
	float nt2_mumu_e2(0.), nt10_mumu_e2(0.), nt0_mumu_e2(0.);
	float nt2_emu(0.),     nt10_emu(0.),     nt01_emu(0.),    nt0_emu(0.);
	float nt2_emu_e2(0.),  nt10_emu_e2(0.),  nt01_emu_e2(0.), nt0_emu_e2(0.);	
	float nt2_ee(0.),      nt10_ee(0.),      nt0_ee(0.);
	float nt2_ee_e2(0.),   nt10_ee_e2(0.),   nt0_ee_e2(0.);

	float nt2pp_mumu(0.), nt2pf_mumu(0.), nt2ff_mumu(0.);
	float nt2pp_emu(0.),  nt2pf_emu(0.),  nt2fp_emu(0.),   nt2ff_emu(0.);
	float nt2pp_ee(0.),   nt2pf_ee(0.),   nt2ff_ee(0.);
	float npp_mumu(0.),   npf_mumu(0.),   nff_mumu(0.);
	float npp_emu(0.),    npf_emu(0.),    nfp_emu(0.),   nff_emu(0.);
	float npp_ee(0.),     npf_ee(0.),     nff_ee(0.);

	float nt2pp_cm_ee(0.);
	float npp_cm_ee(0.);

	for(size_t i = 0; i < musamples.size(); ++i){
		Sample *S = fSamples[musamples[i]];
		float scale = fLumiNorm / S->lumi;
		nt2_mumu     += scale*S->numbers[Baseline][Muon].nt2;
		nt10_mumu    += scale*S->numbers[Baseline][Muon].nt10;
		nt0_mumu     += scale*S->numbers[Baseline][Muon].nt0;
		nt2_mumu_e2  += scale*S->numbers[Baseline][Muon].nt2;
		nt10_mumu_e2 += scale*S->numbers[Baseline][Muon].nt10;
		nt0_mumu_e2  += scale*S->numbers[Baseline][Muon].nt0;

		nt2pp_mumu += scale*S->region[Baseline].mm.nt2pp_pt->GetEntries();
		nt2pf_mumu += scale*S->region[Baseline].mm.nt2pf_pt->GetEntries();
		nt2pf_mumu += scale*S->region[Baseline].mm.nt2fp_pt->GetEntries();
		nt2ff_mumu += scale*S->region[Baseline].mm.nt2ff_pt->GetEntries();
		npp_mumu   += scale*S->region[Baseline].mm.npp_pt->GetEntries();
		npf_mumu   += scale*S->region[Baseline].mm.npf_pt->GetEntries();
		npf_mumu   += scale*S->region[Baseline].mm.nfp_pt->GetEntries();
		nff_mumu   += scale*S->region[Baseline].mm.nff_pt->GetEntries();
	}
	for(size_t i = 0; i < emusamples.size(); ++i){
		Sample *S = fSamples[emusamples[i]];
		float scale = fLumiNorm / S->lumi;
		nt2_emu     += scale*S->numbers[Baseline][EMu].nt2;
		nt10_emu    += scale*S->numbers[Baseline][EMu].nt10;
		nt01_emu    += scale*S->numbers[Baseline][EMu].nt01;
		nt0_emu     += scale*S->numbers[Baseline][EMu].nt0;
		nt2_emu_e2  += scale*S->numbers[Baseline][EMu].nt2;
		nt10_emu_e2 += scale*S->numbers[Baseline][EMu].nt10;
		nt01_emu_e2 += scale*S->numbers[Baseline][EMu].nt01;
		nt0_emu_e2  += scale*S->numbers[Baseline][EMu].nt0;

		nt2pp_emu += scale*S->region[Baseline].em.nt2pp_pt->GetEntries();
		nt2pf_emu += scale*S->region[Baseline].em.nt2pf_pt->GetEntries();
		nt2fp_emu += scale*S->region[Baseline].em.nt2fp_pt->GetEntries();
		nt2ff_emu += scale*S->region[Baseline].em.nt2ff_pt->GetEntries();
		npp_emu   += scale*S->region[Baseline].em.npp_pt->GetEntries();
		npf_emu   += scale*S->region[Baseline].em.npf_pt->GetEntries();
		nfp_emu   += scale*S->region[Baseline].em.nfp_pt->GetEntries();
		nff_emu   += scale*S->region[Baseline].em.nff_pt->GetEntries();
	}		
	for(size_t i = 0; i < elsamples.size(); ++i){
		Sample *S = fSamples[elsamples[i]];
		float scale = fLumiNorm / S->lumi;
		nt2_ee     += scale*S->numbers[Baseline][Electron].nt2;
		nt10_ee    += scale*S->numbers[Baseline][Electron].nt10;
		nt0_ee     += scale*S->numbers[Baseline][Electron].nt0;
		nt2_ee_e2  += scale*S->numbers[Baseline][Electron].nt2;
		nt10_ee_e2 += scale*S->numbers[Baseline][Electron].nt10;
		nt0_ee_e2  += scale*S->numbers[Baseline][Electron].nt0;

		nt2pp_ee += scale*S->region[Baseline].ee.nt2pp_pt->GetEntries();
		nt2pf_ee += scale*S->region[Baseline].ee.nt2pf_pt->GetEntries();
		nt2pf_ee += scale*S->region[Baseline].ee.nt2fp_pt->GetEntries();
		nt2ff_ee += scale*S->region[Baseline].ee.nt2ff_pt->GetEntries();
		npp_ee   += scale*S->region[Baseline].ee.npp_pt->GetEntries();
		npf_ee   += scale*S->region[Baseline].ee.npf_pt->GetEntries();
		npf_ee   += scale*S->region[Baseline].ee.nfp_pt->GetEntries();
		nff_ee   += scale*S->region[Baseline].ee.nff_pt->GetEntries();

		nt2pp_cm_ee += scale*S->region[Baseline].ee.nt2pp_cm_pt->GetEntries();
		npp_cm_ee   += scale*S->region[Baseline].ee.npp_cm_pt->GetEntries();
	}		

	///////////////////////////////////////////////////////////////////////////////////
	// PREDICTIONS ////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	FPRatios *fMMFPRatios = new FPRatios();
	FPRatios *fEMFPRatios  = new FPRatios();
	FPRatios *fEEFPRatios   = new FPRatios();

	fMMFPRatios->SetVerbose(fVerbose);
	fEMFPRatios ->SetVerbose(fVerbose);
	fEEFPRatios  ->SetVerbose(fVerbose);

	fMMFPRatios->SetMuFratios(mufratio_allmc, mufratio_allmc_e);
	fMMFPRatios->SetMuPratios(mupratio_allmc, mupratio_allmc_e);

	fEEFPRatios  ->SetElFratios(elfratio_allmc, elfratio_allmc_e);
	fEEFPRatios  ->SetElPratios(elpratio_allmc, elpratio_allmc_e);

	fEMFPRatios ->SetFratios(elfratio_allmc, elfratio_allmc_e, mufratio_allmc, mufratio_allmc_e);
	fEMFPRatios ->SetPratios(elpratio_allmc, elpratio_allmc_e, mupratio_allmc, mupratio_allmc_e);


	// MuMu Channel
	vector<double> nt_mumu;
	nt_mumu.push_back(nt0_mumu);
	nt_mumu.push_back(nt10_mumu); // mu passes
	nt_mumu.push_back(nt2_mumu);

	fMMFPRatios->NevtTopol(0, 2, nt_mumu);

	vector<double> vpt, veta;
	vpt.push_back(30.); vpt.push_back(30.); // Fake pts and etas (first electron then muon)
	veta.push_back(0.); veta.push_back(0.);

	vector<double> MuMu_Nev   = fMMFPRatios->NevtPass(vpt, veta);
	vector<double> MuMu_Estat = fMMFPRatios->NevtPassErrStat();
	vector<double> MuMu_Esyst = fMMFPRatios->NevtPassErrSyst();

	// EMu Channel
	vector<double> nt_emu;
	nt_emu.push_back(nt0_emu);
	nt_emu.push_back(nt01_emu); // e passes
	nt_emu.push_back(nt10_emu); // mu passes
	nt_emu.push_back(nt2_emu);

	fEMFPRatios->NevtTopol(1, 1, nt_emu);

	vector<double> EMu_Nev   = fEMFPRatios->NevtPass(vpt, veta);
	vector<double> EMu_Estat = fEMFPRatios->NevtPassErrStat();
	vector<double> EMu_Esyst = fEMFPRatios->NevtPassErrSyst();

	// EE Channel
	vector<double> nt_ee;
	nt_ee.push_back(nt0_ee);
	nt_ee.push_back(nt10_ee); // el passes
	nt_ee.push_back(nt2_ee);

	fEEFPRatios->NevtTopol(2, 0, nt_ee);

	vector<double> EE_Nev   = fEEFPRatios->NevtPass(vpt, veta);
	vector<double> EE_Estat = fEEFPRatios->NevtPassErrStat();
	vector<double> EE_Esyst = fEEFPRatios->NevtPassErrSyst();

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
	OUT << "---------------------------------------------------------------------------------------------------------" << endl << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "         YIELDS  ||   Nt2   |   Nt1   |   Nt0   ||   Nt2   |   Nt10  |   Nt01  |   Nt0   ||   Nt2   |   Nt1   |   Nt0   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;

	float nt2sum_mumu(0.), nt10sum_mumu(0.), nt0sum_mumu(0.);
	float nt2sum_emu(0.), nt10sum_emu(0.), nt01sum_emu(0.), nt0sum_emu(0.);
	float nt2sum_ee(0.), nt10sum_ee(0.), nt0sum_ee(0.);
	for(size_t i = 0; i < fMCBG.size(); ++i){
		Sample *S = fSamples[fMCBG[i]];
		float scale = fLumiNorm / S->lumi;
		nt2sum_mumu  += scale*S->numbers[Baseline][Muon]    .nt2;
		nt10sum_mumu += scale*S->numbers[Baseline][Muon]    .nt10;
		nt0sum_mumu  += scale*S->numbers[Baseline][Muon]    .nt0;
		nt2sum_emu   += scale*S->numbers[Baseline][EMu]     .nt2;
		nt10sum_emu  += scale*S->numbers[Baseline][EMu]     .nt10;
		nt01sum_emu  += scale*S->numbers[Baseline][EMu]     .nt01;
		nt0sum_emu   += scale*S->numbers[Baseline][EMu]     .nt0;
		nt2sum_ee    += scale*S->numbers[Baseline][Electron].nt2;
		nt10sum_ee   += scale*S->numbers[Baseline][Electron].nt10;
		nt0sum_ee    += scale*S->numbers[Baseline][Electron].nt0;

		OUT << setw(16) << S->sname << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Muon]    .nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Muon]    .nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Muon]    .nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][EMu]     .nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][EMu]     .nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][EMu]     .nt01 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][EMu]     .nt0  << " || ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Electron].nt2  << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Electron].nt10 << " | ";
		OUT << setw(7)  << setprecision(2) << scale*S->numbers[Baseline][Electron].nt0  << " || ";
		OUT << endl;
	}	
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Observed in MC"  << " || ";
	OUT << setw(7) << Form("%5.2f", nt2_mumu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt10_mumu) << " | ";
	OUT << setw(7) << Form("%5.2f", nt0_mumu ) << " || ";
	OUT << setw(7) << Form("%5.2f", nt2_emu  ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt10_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt01_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt0_emu  ) << " || ";
	OUT << setw(7) << Form("%5.2f", nt2_ee   ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt10_ee  ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt0_ee   ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "tot. pred. fakes"  << " || ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[1]+MuMu_Nev[2] ) << " | ";
	OUT << setw(7) << "        |";
	OUT << setw(7) << "         || ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[1]+EMu_Nev[2]+EMu_Nev[3] ) << " | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        || ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[1]+EE_Nev[2] ) << " | ";
	OUT << setw(7) << "        | ";
	OUT << setw(7) << "        || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << "                 ||            Mu/Mu            ||                   E/Mu                ||             E/E             ||" << endl;
	OUT << "         YIELDS  ||   Npp   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nfp   |   Nff   ||   Npp   |   Npf   |   Nff   ||" << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Observed in MC"  << " || ";
	OUT << setw(7) << Form("%5.2f", npp_mumu ) << " | ";
	OUT << setw(7) << Form("%5.2f", npf_mumu) << " | ";
	OUT << setw(7) << Form("%5.2f", nff_mumu ) << " || ";
	OUT << setw(7) << Form("%5.2f", npp_emu  ) << " | ";
	OUT << setw(7) << Form("%5.2f", npf_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nfp_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nff_emu  ) << " || ";
	OUT << setw(7) << Form("%5.2f", npp_ee   ) << " | ";
	OUT << setw(7) << Form("%5.2f", npf_ee  ) << " | ";
	OUT << setw(7) << Form("%5.2f", nff_ee   ) << " || ";
	OUT << endl;
	OUT << " ee charge misid ||         |         |         ||         |         |         |         || ";
	OUT << setw(7) << Form("%5.2f", npp_cm_ee) << " |         |         ||" << endl;
	OUT << setw(16) << "Predicted"  << " || ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[0] / (mupratio_allmc*mupratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[1] / (mupratio_allmc*mufratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[2] / (mufratio_allmc*mufratio_allmc)) << " || ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[0]  / (mupratio_allmc*elpratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[1]  / (mufratio_allmc*elpratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[2]  / (mupratio_allmc*elfratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[3]  / (mufratio_allmc*elfratio_allmc)) << " || ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[0]   / (elpratio_allmc*elpratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[1]   / (elpratio_allmc*elfratio_allmc)) << " | ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[2]   / (elfratio_allmc*elfratio_allmc)) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << setw(16) << "Observed in TT"  << " || ";
	OUT << setw(7) << Form("%5.2f", nt2pp_mumu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2pf_mumu) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2ff_mumu ) << " || ";
	OUT << setw(7) << Form("%5.2f", nt2pp_emu  ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2pf_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2fp_emu ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2ff_emu  ) << " || ";
	OUT << setw(7) << Form("%5.2f", nt2pp_ee   ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2pf_ee  ) << " | ";
	OUT << setw(7) << Form("%5.2f", nt2ff_ee   ) << " || ";
	OUT << endl;
	OUT << " ee charge misid ||         |         |         ||         |         |         |         || ";
	OUT << setw(7) << Form("%5.2f", nt2pp_cm_ee) << " |         |         ||" << endl;
	OUT << setw(16) << "Predicted in TT"  << " || ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[0] ) << " | ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[1] ) << " | ";
	OUT << setw(7) << Form("%5.2f", MuMu_Nev[2] ) << " || ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[0]  ) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[1]  ) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[2]  ) << " | ";
	OUT << setw(7) << Form("%5.2f", EMu_Nev[3]  ) << " || ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[0]   ) << " | ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[1]   ) << " | ";
	OUT << setw(7) << Form("%5.2f", EE_Nev[2]   ) << " || ";
	OUT << endl;
	OUT << "--------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl;


	OUT << "  PREDICTIONS (in tt window)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " Mu/Mu Channel:" << endl;
	OUT << "  Npp*pp:        " << setw(7) << setprecision(3) << MuMu_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp*fp:        " << setw(7) << setprecision(3) << MuMu_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[1] << " (syst)" << endl;
	OUT << "  Nff*ff:        " << setw(7) << setprecision(3) << MuMu_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[2] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << MuMu_Nev[1]+MuMu_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << TMath::Sqrt(MuMu_Estat[1]*MuMu_Estat[1] + MuMu_Estat[2]*MuMu_Estat[2]);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << TMath::Sqrt(MuMu_Esyst[1]*MuMu_Esyst[1] + MuMu_Esyst[2]*MuMu_Esyst[2]) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/Mu Channel:" << endl;
	OUT << "  Npp*pp:        " << setw(7) << setprecision(3) << EMu_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp*fp:        " << setw(7) << setprecision(3) << EMu_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[1] << " (syst)" << endl;
	OUT << "  Npf*pf:        " << setw(7) << setprecision(3) << EMu_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[2] << " (syst)" << endl;
	OUT << "  Nff*ff:        " << setw(7) << setprecision(3) << EMu_Nev[3];
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[3];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[3] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << EMu_Nev[1]+EMu_Nev[2]+EMu_Nev[3];
	OUT << " +/- "             << setw(7) << setprecision(3) << TMath::Sqrt(EMu_Estat[1]*EMu_Estat[1] + EMu_Estat[2]*EMu_Estat[2] + EMu_Estat[3]*EMu_Estat[3]);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << TMath::Sqrt(EMu_Esyst[1]*EMu_Esyst[1] + EMu_Esyst[2]*EMu_Esyst[2] + EMu_Esyst[3]*EMu_Esyst[3]) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/E Channel:" << endl;
	OUT << "  Npp*pp:        " << setw(7) << setprecision(3) << EE_Nev[0];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[0];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[0] << " (syst)" << endl;
	OUT << "  Nfp*fp:        " << setw(7) << setprecision(3) << EE_Nev[1];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[1];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[1] << " (syst)" << endl;
	OUT << "  Nff*ff:        " << setw(7) << setprecision(3) << EE_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[2];
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[2] << " (syst)" << endl;
	OUT << "  Total fakes:   " << setw(7) << setprecision(3) << EE_Nev[1]+EE_Nev[2];
	OUT << " +/- "             << setw(7) << setprecision(3) << TMath::Sqrt(EE_Estat[1]*EE_Estat[1] + EE_Estat[2]*EE_Estat[2]);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << TMath::Sqrt(EE_Esyst[1]*EE_Esyst[1] + EE_Esyst[2]*EE_Esyst[2]) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << "  PREDICTIONS (in entire window)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " Mu/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << MuMu_Nev[0]   / (mupratio_allmc*mupratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[0] / (mupratio_allmc*mupratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[0] / (mupratio_allmc*mupratio_allmc) << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << MuMu_Nev[1]   / (mupratio_allmc*mufratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[1] / (mupratio_allmc*mufratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[1] / (mupratio_allmc*mufratio_allmc) << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << MuMu_Nev[2]   / (mufratio_allmc*mufratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << MuMu_Estat[2] / (mufratio_allmc*mufratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << MuMu_Esyst[2] / (mufratio_allmc*mufratio_allmc) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/Mu Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << EMu_Nev[0]   / (mupratio_allmc*elpratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[0] / (mupratio_allmc*elpratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[0] / (mupratio_allmc*elpratio_allmc) << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << EMu_Nev[1]   / (mufratio_allmc*elpratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[1] / (mufratio_allmc*elpratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[1] / (mufratio_allmc*elpratio_allmc) << " (syst)" << endl;
	OUT << "  Npf:           " << setw(7) << setprecision(3) << EMu_Nev[2]   / (mupratio_allmc*elfratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[2] / (mupratio_allmc*elfratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[2] / (mupratio_allmc*elfratio_allmc) << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << EMu_Nev[3]   / (mufratio_allmc*elfratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EMu_Estat[3] / (mufratio_allmc*elfratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EMu_Esyst[3] / (mufratio_allmc*elfratio_allmc) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << " E/E Channel:" << endl;
	OUT << "  Npp:           " << setw(7) << setprecision(3) << EE_Nev[0]   / (elpratio_allmc*elpratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[0] / (elpratio_allmc*elpratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[0] / (elpratio_allmc*elpratio_allmc) << " (syst)" << endl;
	OUT << "  Nfp:           " << setw(7) << setprecision(3) << EE_Nev[1]   / (elpratio_allmc*elfratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[1] / (elpratio_allmc*elfratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[1] / (elpratio_allmc*elfratio_allmc) << " (syst)" << endl;
	OUT << "  Nff:           " << setw(7) << setprecision(3) << EE_Nev[2]   / (elfratio_allmc*elfratio_allmc);
	OUT << " +/- "             << setw(7) << setprecision(3) << EE_Estat[2] / (elfratio_allmc*elfratio_allmc);
	OUT << " (stat) +/- "      << setw(7) << setprecision(3) << EE_Esyst[2] / (elfratio_allmc*elfratio_allmc) << " (syst)" << endl;
	OUT << "--------------------------------------------------------------" << endl;
	OUT << "/////////////////////////////////////////////////////////////////////////////" << endl;
	
	OUT.close();
	delete fMMFPRatios;
	delete fEMFPRatios;
	delete fEEFPRatios;
}

void MuonPlotter::makeTTbarClosure(){
	// TString filename = "TTbarClosure.txt";
	// ofstream OUT(fOutputDir + filename.Data(), ios::trunc);

	///////////////////////////////////////////////////////////////////////////////////
	// RATIOS /////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float mufratio_allmc(0.), mufratio_allmc_e(0.);
	float mupratio_allmc(0.), mupratio_allmc_e(0.);
	float elfratio_allmc(0.), elfratio_allmc_e(0.);
	float elpratio_allmc(0.), elpratio_allmc_e(0.);

	calculateRatio(fMCBGMuEnr, Muon,     SigSup, mufratio_allmc, mufratio_allmc_e);
	calculateRatio(fMCBGMuEnr, Muon,     ZDecay, mupratio_allmc, mupratio_allmc_e);
	calculateRatio(fMCBG,      Electron, SigSup, elfratio_allmc, elfratio_allmc_e);
	calculateRatio(fMCBG,      Electron, ZDecay, elpratio_allmc, elpratio_allmc_e);

	///////////////////////////////////////////////////////////////////////////////////
	// OBSERVATIONS ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	float nt_mm(0.), nl_mm(0.),  np_mm(0.), nf_mm(0.);
	float nt_em(0.), nl_em(0.),  np_em(0.), nf_em(0.);
	float nt_me(0.), nl_me(0.),  np_me(0.), nf_me(0.);
	float nt_ee(0.), nl_ee(0.),  np_ee(0.), nf_ee(0.);

	Sample *S = fSamples[TTJets];
	TTree *tree = S->tree;
	fCurrentSample = TTJets;
	float scale = fLumiNorm / S->lumi;

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
					scale*nt_mm++;	
					if( isPromptMuon(ind2)) scale*np_mm++;
					if(!isPromptMuon(ind2)) scale*nf_mm++;
				}
				if(!isTightMuon(ind2))  scale*nl_mm++;
			}
			else if(isPromptMuon(ind2)){
				if( isTightMuon(ind1)){
					scale*nt_mm++;
					if( isPromptMuon(ind1)) scale*np_mm++;
					if(!isPromptMuon(ind1)) scale*nf_mm++;
				}
				if(!isTightMuon(ind1))  scale*nl_mm++;				
			}			
		}
		
		
		// EE
		fCurrentChannel = Electron;
		if(isSSLLElEvent(ind1, ind2)){
			if(isPromptElectron(ind1)){
				if( isTightElectron(ind2)){
					scale*nt_ee++;
					if( isPromptElectron(ind2)) scale*np_ee++;
					if(!isPromptElectron(ind2)) scale*nf_ee++;
				}
				if(!isTightElectron(ind2))  scale*nl_ee++;
			}
			else if(isPromptElectron(ind2)){
				if( isTightElectron(ind1)){
					scale*nt_ee++;
					if( isPromptElectron(ind1)) scale*np_ee++;
					if(!isPromptElectron(ind1)) scale*nf_ee++;
				}
				if(!isTightElectron(ind1))  scale*nl_ee++;				
			}			
		}
		
		// EMu
		fCurrentChannel = EMu;
		if(isSSLLElMuEvent(ind1, ind2)){
			if(isPromptElectron(ind2)){
				if( isTightMuon(ind1)){
					scale*nt_em++;
					if( isPromptMuon(ind1)) scale*np_em++;
					if(!isPromptMuon(ind1)) scale*nf_em++;
				}
				if(!isTightMuon(ind1))  scale*nl_em++;				
			}			
			else if(isPromptMuon(ind1)){
				if( isTightElectron(ind2)){
					scale*nt_me++;
					if( isPromptElectron(ind2)) scale*np_me++;
					if(!isPromptElectron(ind2)) scale*nf_me++;
				}
				if(!isTightElectron(ind2))  scale*nl_me++;
			}
		}
	}
	cout << endl;
	
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
	cout << "---------------------------------------------------------------------------------------------------------" << endl;
	cout << "         RATIOS  ||     Mu-fRatio      |     Mu-pRatio      ||     El-fRatio      |     El-pRatio      ||" << endl;
	cout << "---------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(16) << "        allMC    ||";
	cout << setw(7)  << setprecision(2) << mufratio_allmc << " +/- " << setw(7) << setprecision(2) << mufratio_allmc_e << " |";
	cout << setw(7)  << setprecision(2) << mupratio_allmc << " +/- " << setw(7) << setprecision(2) << mupratio_allmc_e << " ||";
	cout << setw(7)  << setprecision(2) << elfratio_allmc << " +/- " << setw(7) << setprecision(2) << elfratio_allmc_e << " |";
	cout << setw(7)  << setprecision(2) << elpratio_allmc << " +/- " << setw(7) << setprecision(2) << elpratio_allmc_e << " ||";
	cout << endl;
	cout << "---------------------------------------------------------------------------------------------------------" << endl << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	cout << "                 ||       Mu/Mu       ||        E/Mu       ||       Mu/E        ||        E/E        ||" << endl;
	cout << "                 ||    Nt   |    Nl   ||    Nt   |   Nl    ||    Nt   |   Nl    ||    Nt   |   Nl    ||" << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(16) << "Observed" << " || ";
	cout << setw(7)  << numbForm(nt_mm) << " | ";
	cout << setw(7)  << numbForm(nl_mm) << " || ";
	cout << setw(7)  << numbForm(nt_em) << " | ";
	cout << setw(7)  << numbForm(nl_em) << " || ";
	cout << setw(7)  << numbForm(nt_me) << " | ";
	cout << setw(7)  << numbForm(nl_me) << " || ";
	cout << setw(7)  << numbForm(nt_ee) << " | ";
	cout << setw(7)  << numbForm(nl_ee) << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	cout << "                 ||    Np   |    Nf   ||    Np   |    Nf   ||    Np   |    Nf   ||    Np   |    Nf   ||" << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(16) << "MC Truth" << " || ";
	cout << setw(7)  << numbForm(np_mm) << " | ";
	cout << setw(7)  << numbForm(nf_mm) << " || ";
	cout << setw(7)  << numbForm(np_em) << " | ";
	cout << setw(7)  << numbForm(nf_em) << " || ";
	cout << setw(7)  << numbForm(np_me) << " | ";
	cout << setw(7)  << numbForm(nf_me) << " || ";
	cout << setw(7)  << numbForm(np_ee) << " | ";
	cout << setw(7)  << numbForm(nf_ee) << " || ";
	cout << endl;
	cout << setw(16) << "Predicted" << " || ";
	cout << setw(7)  << numbForm(MM_Nev[0]) << " | ";
	cout << setw(7)  << numbForm(MM_Nev[1]) << " || ";
	cout << setw(7)  << numbForm(EM_Nev[0]) << " | ";
	cout << setw(7)  << numbForm(EM_Nev[1]) << " || ";
	cout << setw(7)  << numbForm(ME_Nev[0]) << " | ";
	cout << setw(7)  << numbForm(ME_Nev[1]) << " || ";
	cout << setw(7)  << numbForm(EE_Nev[0]) << " | ";
	cout << setw(7)  << numbForm(EE_Nev[1]) << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	
	// OUT.close();
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

	TString eventfilename = fOutputDir + "pass8_eth.txt";
	fOUTSTREAM.open(eventfilename.Data(), ios::trunc);

	Sample *S = fSamples[TTJetsSync];
	TTree *tree = S->tree;
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
		
		
		// EE
		fCurrentChannel = Electron;
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
		
		// EMu
		fCurrentChannel = EMu;
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
	cout << Form("| ETH/IFCA/Oviedo | %5.2f (%1d)  | %5.2f (%2d)   | %5.2f (%2d)   | %5.2f (%3d)   | Ran on 1084783 total events, [[%%ATTACHURL%%/pass8_eth.txt][yield.txt]] |"
		, scale*N_elel, N_elel
		, scale*N_mumu, N_mumu
		, scale*N_elmu, N_elmu
		, scale*(N_elel+N_mumu+N_elmu), N_elel+N_mumu+N_elmu) << endl;

	fOUTSTREAM.close();
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
		printObject(H_nsigpred, H_nsigpred->GetName(), H_nsigpred->GetTitle());
		printObject(H_nt2mes, H_nt2mes->GetName(), H_nt2mes->GetTitle(), "colz");
		printObject(H_nt1mes, H_nt1mes->GetName(), H_nt1mes->GetTitle(), "colz");
		printObject(H_nt0mes, H_nt0mes->GetName(), H_nt0mes->GetTitle(), "colz");
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
	const int nptbins = getNPt2Bins(Electron);

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
		printObject(H_nsigpred, H_nsigpred->GetName(), H_nsigpred->GetTitle());
		printObject(H_nt2mes, H_nt2mes->GetName(), H_nt2mes->GetTitle(), "colz");
		printObject(H_nt1mes, H_nt1mes->GetName(), H_nt1mes->GetTitle(), "colz");
		printObject(H_nt0mes, H_nt0mes->GetName(), H_nt0mes->GetTitle(), "colz");
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
		printObject(H_npppred, H_npppred->GetName(), H_npppred->GetTitle());
		printObject(H_nt2mes,  H_nt2mes->GetName(),  H_nt2mes->GetTitle(),  "colz");
		printObject(H_nt10mes, H_nt10mes->GetName(), H_nt10mes->GetTitle(), "colz");
		printObject(H_nt01mes, H_nt01mes->GetName(), H_nt01mes->GetTitle(), "colz");
		printObject(H_nt0mes,  H_nt0mes->GetName(),  H_nt0mes->GetTitle(),  "colz");
	}
	res.push_back(H_npppred);
	res.push_back(H_nfppred);
	res.push_back(H_npfpred);
	res.push_back(H_nffpred);
	return res;
}

//____________________________________________________________________________
void MuonPlotter::fillYields(Sample *S, gRegion reg){
	///////////////////////////////////////////////////
	// Set custom event selections here:
	fC_minMu1pt = Region::minMu1pt[reg];
	fC_minMu2pt = Region::minMu2pt[reg];
	fC_minEl1pt = Region::minEl1pt[reg];
	fC_minEl2pt = Region::minEl2pt[reg];
	fC_minHT    = Region::minHT   [reg];
	fC_minMet   = Region::minMet  [reg];
	fC_minNjets = Region::minNjets[reg];

	///////////////////////////////////////////////////
	// SS YIELDS
	// MuMu Channel
	fDoCounting = true;
	fCurrentChannel = Muon;
	float puweight = PUWeight;
	int mu1(-1), mu2(-1);
	if(mumuSignalTrigger()){ // Trigger selection
		if(isSSLLMuEvent(mu1, mu2)){ // Same-sign loose-loose di muon event
			if(  isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Tight-tight
				fCounters[fCurrentSample][Muon].fill(" ... first muon passes tight cut");
				fCounters[fCurrentSample][Muon].fill(" ... second muon passes tight cut");
				fCounters[fCurrentSample][Muon].fill(" ... both muons pass tight cut");
				S->region[reg].mm.nt20_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg].mm.nt20_eta->Fill(MuEta[mu1], MuEta[mu2], puweight);
				if(S->datamc == 0){
					fOUTSTREAM << " Mu/Mu Tight-Tight event in run ";
					fOUTSTREAM << setw(7) << Run;
					fOUTSTREAM << " event " << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " in dataset " << setw(12) << S->sname;
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " MT2 = " << setw(5) << Form("%6.2f", getMT2(mu1, mu2, 1)) << endl;
				}
				if(S->datamc > 0 ){
					S->region[reg].mm.nt11_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
					if(isPromptMuon(mu1) && isPromptMuon(mu2)) S->region[reg].mm.nt2pp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(isPromptMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg].mm.nt2pf_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(  isFakeMuon(mu1) && isPromptMuon(mu2)) S->region[reg].mm.nt2fp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
					if(  isFakeMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg].mm.nt2ff_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				}
			}
			if(  isTightMuon(mu1) && !isTightMuon(mu2) ){ // Tight-loose
				fCounters[fCurrentSample][Muon].fill(" ... first muon passes tight cut");
				S->region[reg].mm.nt10_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg].mm.nt10_eta->Fill(MuEta[mu1], MuEta[mu2], puweight);
				if(S->datamc > 0) S->region[reg].mm.nt10_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
			}
			if( !isTightMuon(mu1) &&  isTightMuon(mu2) ){ // Loose-tight
				fCounters[fCurrentSample][Muon].fill(" ... second muon passes tight cut");
				S->region[reg].mm.nt10_pt ->Fill(MuPt [mu2], MuPt [mu1], puweight); // tight one always in x axis; fill same again
				S->region[reg].mm.nt10_eta->Fill(MuEta[mu2], MuEta[mu1], puweight);
				if(S->datamc > 0) S->region[reg].mm.nt10_origin->Fill(muIndexToBin(mu2)-0.5, muIndexToBin(mu1)-0.5, puweight);
			}
			if( !isTightMuon(mu1) && !isTightMuon(mu2) ){ // Loose-loose
				S->region[reg].mm.nt00_pt ->Fill(MuPt [mu1], MuPt [mu2], puweight);
				S->region[reg].mm.nt00_eta->Fill(MuEta[mu1], MuEta[mu2], puweight);
				if(S->datamc > 0) S->region[reg].mm.nt00_origin->Fill(muIndexToBin(mu1)-0.5, muIndexToBin(mu2)-0.5, puweight);
			}
			if(S->datamc > 0){
				if(isPromptMuon(mu1) && isPromptMuon(mu2)) S->region[reg].mm.npp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(isPromptMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg].mm.npf_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(  isFakeMuon(mu1) && isPromptMuon(mu2)) S->region[reg].mm.nfp_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);
				if(  isFakeMuon(mu1) &&   isFakeMuon(mu2)) S->region[reg].mm.nff_pt->Fill(MuPt[mu1], MuPt[mu2], puweight);			
			}
		}
	}
	if(singleMuTrigger() && isSigSupMuEvent()){
		if( isTightMuon(0) ){
			S->region[reg].mm.fntight->Fill(MuPt[0], MuEta[0], singleMuPrescale() * puweight);
			if(S->datamc > 0) S->region[reg].mm.sst_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
		if( isLooseMuon(0) ){
			S->region[reg].mm.fnloose->Fill(MuPt[0], MuEta[0], singleMuPrescale() * puweight);
			if(S->datamc > 0) S->region[reg].mm.ssl_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
	}
	if(doubleMuTrigger() && isZMuMuEvent()){
		if( isTightMuon(0) ){
			S->region[reg].mm.pntight->Fill(MuPt[0], MuEta[0], puweight);
			if(S->datamc > 0) S->region[reg].mm.zt_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
		if( isLooseMuon(0) ){
			S->region[reg].mm.pnloose->Fill(MuPt[0], MuEta[0], puweight);
			if(S->datamc > 0) S->region[reg].mm.zl_origin->Fill(muIndexToBin(0)-0.5, puweight);
		}
	}				

	// EE Channel
	fCurrentChannel = Electron;
	int el1(-1), el2(-1);
	if(elelSignalTrigger()){
		if( isSSLLElEvent(el1, el2) ){
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				fCounters[fCurrentSample][Electron].fill(" ... first electron passes tight cut");
				fCounters[fCurrentSample][Electron].fill(" ... second electron passes tight cut");
				fCounters[fCurrentSample][Electron].fill(" ... both electrons pass tight cut");
				S->region[reg].ee.nt20_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg].ee.nt20_eta->Fill(ElEta[el1], ElEta[el2], puweight);
				if(S->datamc == 0){
					fOUTSTREAM << " E/E Tight-Tight event in run   ";
					fOUTSTREAM << setw(7) << Run;
					fOUTSTREAM << " event " << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " in dataset " << setw(12) << S->sname;
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " MT2 = " << setw(5) << Form("%6.2f", getMT2(el1, el2, 2)) << endl;
				}
				if(S->datamc > 0 ){
					S->region[reg].ee.nt11_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
					if(isPromptElectron(el1) && isPromptElectron(el2)){
						S->region[reg].ee.nt2pp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
						if(!isChargeMatchedElectron(el1) || !isChargeMatchedElectron(el2)){
							S->region[reg].ee.nt2pp_cm_pt->Fill(ElPt[el1], ElPt[el2], puweight);							
						}
					}
					if(isPromptElectron(el1) &&   isFakeElectron(el2)) S->region[reg].ee.nt2pf_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(  isFakeElectron(el1) && isPromptElectron(el2)) S->region[reg].ee.nt2fp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(  isFakeElectron(el1) &&   isFakeElectron(el2)) S->region[reg].ee.nt2ff_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				}
			}
			if(  isTightElectron(el1) && !isTightElectron(el2) ){ // Tight-loose
				fCounters[fCurrentSample][Electron].fill(" ... first electron passes tight cut");
				S->region[reg].ee.nt10_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg].ee.nt10_eta->Fill(ElEta[el1], ElEta[el2], puweight);
				if(S->datamc > 0) S->region[reg].ee.nt10_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
			}
			if( !isTightElectron(el1) &&  isTightElectron(el2) ){ // Loose-tight
				fCounters[fCurrentSample][Electron].fill(" ... second electron passes tight cut");
				S->region[reg].ee.nt10_pt ->Fill(ElPt [el2], ElPt [el1], puweight); // tight one always in x axis; fill same again
				S->region[reg].ee.nt10_eta->Fill(ElEta[el2], ElEta[el1], puweight);
				if(S->datamc > 0) S->region[reg].ee.nt10_origin->Fill(elIndexToBin(el2)-0.5, elIndexToBin(el1)-0.5, puweight);
			}
			if( !isTightElectron(el1) && !isTightElectron(el2) ){ // Loose-loose
				S->region[reg].ee.nt00_pt ->Fill(ElPt [el1], ElPt [el2], puweight);
				S->region[reg].ee.nt00_eta->Fill(ElEta[el1], ElEta[el2], puweight);
				if(S->datamc > 0) S->region[reg].ee.nt00_origin->Fill(elIndexToBin(el1)-0.5, elIndexToBin(el2)-0.5, puweight);
			}
			if(S->datamc > 0 ){
				if(isPromptElectron(el1) && isPromptElectron(el2)){
					S->region[reg].ee.npp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
					if(!isChargeMatchedElectron(el1) || !isChargeMatchedElectron(el2)){
						S->region[reg].ee.npp_cm_pt->Fill(ElPt[el1], ElPt[el2], puweight);							
					}
				}
				if(isPromptElectron(el1) &&   isFakeElectron(el2)) S->region[reg].ee.npf_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(  isFakeElectron(el1) && isPromptElectron(el2)) S->region[reg].ee.nfp_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(  isFakeElectron(el1) &&   isFakeElectron(el2)) S->region[reg].ee.nff_pt->Fill(ElPt[el1], ElPt[el2], puweight);
			}
		}
	}
	if(singleElTrigger() && isSigSupElEvent()){
		if( isTightElectron(0) ){
			S->region[reg].ee.fntight->Fill(ElPt[0], ElEta[0], singleElPrescale() * puweight);
			if(S->datamc > 0) S->region[reg].ee.sst_origin->Fill(elIndexToBin(0)-0.5, puweight);
		}
		if( isLooseElectron(0) ){
			S->region[reg].ee.fnloose->Fill(ElPt[0], ElEta[0], singleElPrescale() * puweight);
			if(S->datamc > 0) S->region[reg].ee.ssl_origin->Fill(elIndexToBin(0)-0.5, puweight);
		}
	}
	int elind;
	if(doubleElTrigger() && isZElElEvent(elind)){
		if( isTightElectron(elind) ){
			S->region[reg].ee.pntight->Fill(ElPt[elind], ElEta[elind], puweight);
			if(S->datamc > 0) S->region[reg].ee.zt_origin->Fill(elIndexToBin(elind)-0.5, puweight);
		}
		if( isLooseElectron(elind) ){
			S->region[reg].ee.pnloose->Fill(ElPt[elind], ElEta[elind], puweight);
			if(S->datamc > 0) S->region[reg].ee.zl_origin->Fill(elIndexToBin(elind)-0.5, puweight);
		}
	}

	// EMu Channel
	fCurrentChannel = EMu;
	int mu(-1), el(-1);
	if(elmuSignalTrigger()){
		if( isSSLLElMuEvent(mu, el) ){
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				fCounters[fCurrentSample][EMu].fill(" ... muon passes tight cut");
				fCounters[fCurrentSample][EMu].fill(" ... electron passes tight cut");
				fCounters[fCurrentSample][EMu].fill(" ... both e and mu pass tight cuts");
				S->region[reg].em.nt20_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg].em.nt20_eta->Fill(MuEta[mu], ElEta[el], puweight);
				if(S->datamc == 0){
					fOUTSTREAM << " E/Mu Tight-Tight event in run  ";
					fOUTSTREAM << setw(7) << Run;
					fOUTSTREAM << " event " << setw(11) << Event;
					fOUTSTREAM << " lumisection " << setw(4) << LumiSec;
					fOUTSTREAM << " in dataset " << setw(12) << S->sname;
					fOUTSTREAM << " HT  = " << setw(5) << Form("%6.2f", getHT());
					fOUTSTREAM << " MET = " << setw(5) << Form("%6.2f", pfMET);
					fOUTSTREAM << " MT2 = " << setw(5) << Form("%6.2f", getMT2(mu, el, 2)) << endl;
				}
				if(S->datamc > 0){
					S->region[reg].em.nt11_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
					if(isPromptMuon(mu) && isPromptElectron(el)) S->region[reg].em.nt2pp_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(isPromptMuon(mu) &&   isFakeElectron(el)) S->region[reg].em.nt2pf_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(  isFakeMuon(mu) && isPromptElectron(el)) S->region[reg].em.nt2fp_pt->Fill(MuPt[mu], ElPt[el], puweight);
					if(  isFakeMuon(mu) &&   isFakeElectron(el)) S->region[reg].em.nt2ff_pt->Fill(MuPt[mu], ElPt[el], puweight);
				}
			}
			if( !isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-loose
				fCounters[fCurrentSample][EMu].fill(" ... muon passes tight cut");
				S->region[reg].em.nt10_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg].em.nt10_eta->Fill(MuEta[mu], ElEta[el], puweight);
				if(S->datamc > 0) S->region[reg].em.nt10_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if(  isTightElectron(el) && !isTightMuon(mu) ){ // Loose-tight
				fCounters[fCurrentSample][EMu].fill(" ... electron passes tight cut");
				S->region[reg].em.nt01_pt ->Fill(MuPt [mu], ElPt [el], puweight); // muon always in x axis for e/mu
				S->region[reg].em.nt01_eta->Fill(MuEta[mu], ElEta[el], puweight);
				if(S->datamc > 0) S->region[reg].em.nt01_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if( !isTightElectron(0) && !isTightMuon(0) ){ // Loose-loose
				S->region[reg].em.nt00_pt ->Fill(MuPt [mu], ElPt [el], puweight);
				S->region[reg].em.nt00_eta->Fill(MuEta[mu], ElEta[el], puweight);
				if(S->datamc > 0) S->region[reg].em.nt00_origin->Fill(muIndexToBin(mu)-0.5, elIndexToBin(el)-0.5, puweight);
			}
			if(S->datamc > 0){
				if(isPromptMuon(mu) && isPromptElectron(el)) S->region[reg].em.npp_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(isPromptMuon(mu) &&   isFakeElectron(el)) S->region[reg].em.npf_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(  isFakeMuon(mu) && isPromptElectron(el)) S->region[reg].em.nfp_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(  isFakeMuon(mu) &&   isFakeElectron(el)) S->region[reg].em.nff_pt->Fill(MuPt[mu], ElPt[el], puweight);
			}
		}
	}

	///////////////////////////////////////////////////
	// OS YIELDS
	fDoCounting = false;
	fChargeSwitch = 1;

	// EE Channel
	fCurrentChannel = Electron;
	if(elelSignalTrigger()){
		if( isSSLLElEvent(el1, el2) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el1) &&  isTightElectron(el2) ){ // Tight-tight
				if( isBarrelElectron(el1) &&  isBarrelElectron(el2)) S->region[reg].ee.nt20_OS_BB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(!isBarrelElectron(el1) && !isBarrelElectron(el2)) S->region[reg].ee.nt20_OS_EE_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if( isBarrelElectron(el1) && !isBarrelElectron(el2)) S->region[reg].ee.nt20_OS_EB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
				if(!isBarrelElectron(el1) &&  isBarrelElectron(el2)) S->region[reg].ee.nt20_OS_EB_pt->Fill(ElPt[el1], ElPt[el2], puweight);
			}
		}
	}

	// EMu Channel
	fCurrentChannel = EMu;
	if(elmuSignalTrigger()){
		if( isSSLLElMuEvent(mu, el) ){ // this selects now OS events with the exact same cuts
			if(  isTightElectron(el) &&  isTightMuon(mu) ){ // Tight-tight
				if( isBarrelElectron(el)) S->region[reg].em.nt20_OS_BB_pt->Fill(MuPt[mu], ElPt[el], puweight);
				if(!isBarrelElectron(el)) S->region[reg].em.nt20_OS_EE_pt->Fill(MuPt[mu], ElPt[el], puweight);
			}
		}
	}
	fChargeSwitch = 0;
}
void MuonPlotter::fillRatioPlots(Sample *S){
	fDoCounting = false;
	fChargeSwitch = 0;
	float puweight = PUWeight;

	FRatioPlots *RP0 = &S->ratioplots[0];
	FRatioPlots *RP1 = &S->ratioplots[1];

	fCurrentChannel = Muon;
	if(singleMuTrigger() && isSigSupMuEvent()){
		if( isTightMuon(0) ){
			RP0->ntight[0]->Fill(getNJets(),               puweight);
			RP0->ntight[1]->Fill(getHT(),                  puweight);
			RP0->ntight[2]->Fill(getMaxJPt(),              puweight);
			RP0->ntight[3]->Fill(NVrtx,                    puweight);
			RP0->ntight[4]->Fill(getClosestJetPt(0, Muon), puweight);
			RP0->ntight[5]->Fill(getAwayJetPt(0, Muon),    puweight);
		}
		if( isLooseMuon(0) ){
			RP0->nloose[0]->Fill(getNJets(),               puweight);
			RP0->nloose[1]->Fill(getHT(),                  puweight);
			RP0->nloose[2]->Fill(getMaxJPt(),              puweight);
			RP0->nloose[3]->Fill(NVrtx,                    puweight);
			RP0->nloose[4]->Fill(getClosestJetPt(0, Muon), puweight);
			RP0->nloose[5]->Fill(getAwayJetPt(0, Muon),    puweight);
		}
	}
	if(singleElTrigger() && isSigSupElEvent()){
		if( isTightElectron(0) ){
			RP1->ntight[0]->Fill(getNJets(),                   puweight);
			RP1->ntight[1]->Fill(getHT(),                      puweight);
			RP1->ntight[2]->Fill(getMaxJPt(),                  puweight);
			RP1->ntight[3]->Fill(NVrtx,                        puweight);
			RP1->ntight[4]->Fill(getClosestJetPt(0, Electron), puweight);
			RP1->ntight[5]->Fill(getAwayJetPt(0, Electron),    puweight);
		}
		if( isLooseElectron(0) ){
			RP1->nloose[0]->Fill(getNJets(),                   puweight);
			RP1->nloose[1]->Fill(getHT(),                      puweight);
			RP1->nloose[2]->Fill(getMaxJPt(),                  puweight);
			RP1->nloose[3]->Fill(NVrtx,                        puweight);
			RP1->nloose[4]->Fill(getClosestJetPt(0, Electron), puweight);
			RP1->nloose[5]->Fill(getAwayJetPt(0, Electron),    puweight);
		}
	}
	
}
void MuonPlotter::fillHWWPlots(Sample *S){
	fDoCounting = false;
	fChargeSwitch = 0;
	float puweight = PUWeight;

	HWWPlots *HWW0 = &S->hwwplots[0]; // all
	HWWPlots *HWW1 = &S->hwwplots[1]; // N-1
	int ind1(-1), ind2(-1);

	// Cuts
	// Loose
	// const float dphimax = TMath::Pi()/3.;
	// const float pt1min  = 20.;
	// const float pt2min  = 20.;
	// const float etamax  = 3.0; // disabled
	// Tight
	const float dphimax = TMath::Pi()/2.;
	const float pt1min  = 30.;
	const float pt2min  = 30.;
	const float etamax  = 1.5; // barrel only

	const float metmin  = 30.;
	const float mllmin  = 12.;
	const float mllmax  = 60.;

	TLorentzVector pl1, pl2;
	int type = abs(isOSLLEvent(ind1, ind2));
	if(type == 0) return; //nothing found
	if(type == 1){ // mumu
		if(!mumuSignalTrigger()) return;
		if(!passesAddLepVeto(ind1, ind2, 1)) return;
		if(!isTightMuon(ind1) || !isTightMuon(ind2)) return;
		
		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(MuPt[ind2], MuEta[ind2], MuPhi[ind2], gMMU);
		float mass = (pl1+pl2).M();
		float dphi = Util::DeltaPhi(MuPhi[ind1], MuPhi[ind2]);
		
		HWW0->hvar[0]->Fill(pfMET,      puweight);
		HWW0->hvar[1]->Fill(getNJets(), puweight);
		HWW0->hvar[2]->Fill(MuPt[ind1], puweight);
		HWW0->hvar[3]->Fill(MuPt[ind2], puweight);
		HWW0->hvar[4]->Fill(mass,       puweight);
		HWW0->hvar[5]->Fill(mass,       puweight);
		HWW0->hvar[8]->Fill(dphi,       puweight);
		HWW0->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);

		if(fabs(MuEta[ind1]) > etamax || fabs(MuEta[ind2]) > etamax) return;
		if(                  getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[0]->Fill(pfMET,      puweight);
		if(pfMET > metmin &&                    MuPt[ind1] > pt1min && MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[1]->Fill(getNJets(), puweight);
		if(pfMET > metmin && getNJets() == 0 &&                        MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[2]->Fill(MuPt[ind1], puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min &&                        mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[3]->Fill(MuPt[ind2], puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min &&                                   dphi < dphimax) HWW1->hvar[4]->Fill(mass,       puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min &&                                   dphi < dphimax) HWW1->hvar[5]->Fill(mass,       puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax                  ) HWW1->hvar[8]->Fill(dphi,       puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);

		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && MuPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax){
			if(S->datamc == 0){
				fOUTSTREAM2 << " Mu/Mu event in run ";
				fOUTSTREAM2 << setw(7) << Run;
				fOUTSTREAM2 << " event " << setw(11) << Event;
				fOUTSTREAM2 << " lumisection " << setw(4) << LumiSec;
				fOUTSTREAM2 << " in dataset " << setw(12) << S->sname;
				fOUTSTREAM2 << " DPhi = " << setw(5) << Form("%6.2f", dphi);
				fOUTSTREAM2 << " MET  = " << setw(5) << Form("%6.2f", pfMET);
				fOUTSTREAM2 << " Mass = " << setw(5) << Form("%6.2f", mass);
				fOUTSTREAM2 << " Pt1  = " << setw(5) << Form("%6.2f", MuPt[ind1]);
				fOUTSTREAM2 << " Pt2  = " << setw(5) << Form("%6.2f", MuPt[ind2]);
				fOUTSTREAM2 << " MT2  = " << setw(5) << Form("%6.2f", getMT2(ind1, ind2, 1)) << endl;
			}
		}
		
	}
	if(type == 2){ // ee
		if(!elelSignalTrigger()) return;
		if(!passesAddLepVeto(ind1, ind2, 2)) return;
		if(!isTightElectron(ind1) || !isTightElectron(ind2)) return;

		pl1.SetPtEtaPhiM(ElPt[ind1], ElEta[ind1], ElPhi[ind1], gMEL);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);
		float mass = (pl1+pl2).M();
		float dphi = Util::DeltaPhi(ElPhi[ind1], ElPhi[ind2]);

		HWW0->hvar[0]->Fill(pfMET,      puweight);
		HWW0->hvar[1]->Fill(getNJets(), puweight);
		HWW0->hvar[2]->Fill(ElPt[ind1], puweight);
		HWW0->hvar[3]->Fill(ElPt[ind2], puweight);
		HWW0->hvar[4]->Fill(mass,       puweight);
		HWW0->hvar[6]->Fill(mass,       puweight);
		HWW0->hvar[8]->Fill(dphi,       puweight);
		HWW0->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);

		if(fabs(ElEta[ind1]) > etamax || fabs(ElEta[ind2]) > etamax) return;
		if(                  getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[0]->Fill(pfMET,      puweight);
		if(pfMET > metmin &&                    ElPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[1]->Fill(getNJets(), puweight);
		if(pfMET > metmin && getNJets() == 0 &&                        ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[2]->Fill(ElPt[ind1], puweight);
		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min &&                        mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[3]->Fill(ElPt[ind2], puweight);
		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min &&                                   dphi < dphimax) HWW1->hvar[4]->Fill(mass,       puweight);
		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min &&                                   dphi < dphimax) HWW1->hvar[6]->Fill(mass,       puweight);
		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax                  ) HWW1->hvar[8]->Fill(dphi,       puweight);
		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);

		if(pfMET > metmin && getNJets() == 0 && ElPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax){
			if(S->datamc == 0){
				fOUTSTREAM2 << " El/El event in run ";
				fOUTSTREAM2 << setw(7) << Run;
				fOUTSTREAM2 << " event " << setw(11) << Event;
				fOUTSTREAM2 << " lumisection " << setw(4) << LumiSec;
				fOUTSTREAM2 << " in dataset " << setw(12) << S->sname;
				fOUTSTREAM2 << " DPhi = " << setw(5) << Form("%6.2f", dphi);
				fOUTSTREAM2 << " MET  = " << setw(5) << Form("%6.2f", pfMET);
				fOUTSTREAM2 << " Mass = " << setw(5) << Form("%6.2f", mass);
				fOUTSTREAM2 << " Pt1  = " << setw(5) << Form("%6.2f", ElPt[ind1]);
				fOUTSTREAM2 << " Pt2  = " << setw(5) << Form("%6.2f", ElPt[ind2]);
				fOUTSTREAM2 << " MT2  = " << setw(5) << Form("%6.2f", getMT2(ind1, ind2, 2)) << endl;
			}
		}
	}
	if(type == 3){ // emu
		if(!elmuSignalTrigger()) return;
		if(!passesAddLepVeto(ind1, ind2, 3)) return;
		if(!isTightMuon(ind1) || !isTightElectron(ind2)) return;

		pl1.SetPtEtaPhiM(MuPt[ind1], MuEta[ind1], MuPhi[ind1], gMMU);
		pl2.SetPtEtaPhiM(ElPt[ind2], ElEta[ind2], ElPhi[ind2], gMEL);
		float mass = (pl1+pl2).M();
		float dphi = Util::DeltaPhi(MuPhi[ind1], ElPhi[ind2]);

		HWW0->hvar[0]->Fill(pfMET,      puweight);
		HWW0->hvar[1]->Fill(getNJets(), puweight);
		HWW0->hvar[2]->Fill(MuPt[ind1], puweight);
		HWW0->hvar[3]->Fill(ElPt[ind2], puweight);
		HWW0->hvar[7]->Fill(mass,       puweight);
		HWW0->hvar[8]->Fill(dphi,       puweight);
		HWW0->hvar[9]->Fill(getMT2(ind1, ind2, 3), puweight);

		if(fabs(MuEta[ind1]) > etamax || fabs(ElEta[ind2]) > etamax) return;
		if(                  getNJets() == 0 && MuPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[0]->Fill(pfMET,      puweight);
		if(pfMET > metmin &&                    MuPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[1]->Fill(getNJets(), puweight);
		if(pfMET > metmin && getNJets() == 0 &&                        ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[2]->Fill(MuPt[ind1], puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min &&                        mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[3]->Fill(ElPt[ind2], puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && ElPt[ind2] > pt2min &&                                   dphi < dphimax) HWW1->hvar[7]->Fill(mass,       puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax                  ) HWW1->hvar[8]->Fill(dphi,       puweight);
		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax) HWW1->hvar[9]->Fill(getMT2(ind1, ind2, 3), puweight);

		if(pfMET > metmin && getNJets() == 0 && MuPt[ind1] > pt1min && ElPt[ind2] > pt2min && mass > mllmin && mass < mllmax && dphi < dphimax){
			if(S->datamc == 0){
				fOUTSTREAM2 << " El/Mu event in run ";
				fOUTSTREAM2 << setw(7) << Run;
				fOUTSTREAM2 << " event " << setw(11) << Event;
				fOUTSTREAM2 << " lumisection " << setw(4) << LumiSec;
				fOUTSTREAM2 << " in dataset " << setw(12) << S->sname;
				fOUTSTREAM2 << " DPhi = " << setw(5) << Form("%6.2f", dphi);
				fOUTSTREAM2 << " MET  = " << setw(5) << Form("%6.2f", pfMET);
				fOUTSTREAM2 << " Mass = " << setw(5) << Form("%6.2f", mass);
				fOUTSTREAM2 << " Pt1  = " << setw(5) << Form("%6.2f", MuPt[ind1]);
				fOUTSTREAM2 << " Pt2  = " << setw(5) << Form("%6.2f", ElPt[ind2]);
				fOUTSTREAM2 << " MT2  = " << setw(5) << Form("%6.2f", getMT2(ind1, ind2, 3)) << endl;
			}
		}
		
	}
}

void MuonPlotter::fillKinPlots(gSample i){
	fDoCounting = false;
	Sample *S = fSamples[i];
	KinPlots *KP0 = &S->kinplots[0];
	KinPlots *KP1 = &S->kinplots[1];
	KinPlots *KP2 = &S->kinplots[2];
	int ind1(-1), ind2(-1);
	int mu(-1), el(-1); // for e/mu channel, easier readability

	float puweight = PUWeight;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MUMU CHANNEL:  //////////////////////////////////////////////////////////////////////////////////////
	// if( ( S->datamc == 0 && doubleMuTrigger() && (i == DoubleMu1 || i == DoubleMu2) ) || S->datamc > 0){ // trigger
	if(doubleMuTrigger() && abs(isSSLLEvent(ind1, ind2)) == 1){ // trigger && select loose mu/mu pair
		// if(abs(isSSLLEvent(ind1, ind2)) == 1){ // select loose mu/mu pair
			if(MuPt[ind1] > 20. && MuPt[ind2] > 10.){ // 20/10 pt cuts

				// Fill histos
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
					KP1->hvar[0]->Fill(getHT(),               puweight);
					KP1->hvar[1]->Fill(pfMET,                 puweight);
					KP1->hvar[2]->Fill(getNJets(),            puweight);
					KP1->hvar[3]->Fill(MuPt[ind1],            puweight);
					KP1->hvar[4]->Fill(MuPt[ind2],            puweight);
					KP1->hvar[5]->Fill(mass,                  puweight); // SF
					KP1->hvar[6]->Fill(mass,                  puweight); // MM
					KP1->hvar[9]->Fill(getMT2(ind1, ind2, 1), puweight);
					if(isSSLLMuEvent(ind1, ind2)){ // signal region
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
		// }
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EE CHANNEL:  ////////////////////////////////////////////////////////////////////////////////////////
	// if( ( S->datamc == 0 && doubleElTrigger() && ( i==DoubleEle1 || i==DoubleEle2 ) ) || S->datamc > 0){ // trigger
	else if(doubleElTrigger() && abs(isSSLLEvent(ind1, ind2)) == 2){ // trigger && select loose e/e pair\
		// 'else' to avoid duplicate MC events, will prefer mumu over ee over emu
		// if(!doubleMuTrigger()                   || S->datamc > 0){ // exclusive trigger selection
			// if(abs(isSSLLEvent(ind1, ind2)) == 2){ // select loose e/e pair
				if(ElPt[ind1] > 20. && ElPt[ind2] > 10.){ // 20/10 pt cuts
					// Fill histos
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
						KP1->hvar[0]->Fill(getHT(),               puweight);
						KP1->hvar[1]->Fill(pfMET,                 puweight);
						KP1->hvar[2]->Fill(getNJets(),            puweight);
						KP1->hvar[3]->Fill(ElPt[ind1],            puweight);
						KP1->hvar[4]->Fill(ElPt[ind2],            puweight);
						KP1->hvar[5]->Fill(mass,                  puweight); // SF
						KP1->hvar[7]->Fill(mass,                  puweight); // MM
						KP1->hvar[9]->Fill(getMT2(ind1, ind2, 2), puweight);
						if(isSSLLElEvent(ind1, ind2)){
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
			// }
		// }
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EMU CHANNEL:  ///////////////////////////////////////////////////////////////////////////////////////
	// if( ( S->datamc == 0 && eMuTrigger() && (i == MuEG1 || i == MuEG2) ) || S->datamc > 0){ // trigger
	else if(eMuTrigger() && abs(isSSLLEvent(mu, el)) == 3){ // trigger && select loose e/mu pair
		// 'else' to avoid duplicate MC events, will prefer mumu over ee over emu
		if( (MuPt[mu] > 20. && ElPt[el] > 10.) || (MuPt[mu] > 10. && ElPt[el] > 20.) ){
			// Fill histos
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
				KP1->hvar[0]->Fill(getHT(),               puweight);
				KP1->hvar[1]->Fill(pfMET,                 puweight);
				KP1->hvar[2]->Fill(getNJets(),            puweight);
				KP1->hvar[3]->Fill(ptmax,                 puweight);
				KP1->hvar[4]->Fill(ptmin,                 puweight);
				KP1->hvar[8]->Fill(mass,                  puweight); // EM
				KP1->hvar[9]->Fill(getMT2(mu, el, 3), puweight);
				if( isSSLLElMuEvent(mu, el) ){
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
	return;
}
void MuonPlotter::fillMuIsoPlots(gSample i){
	fDoCounting = false;
	Sample *S = fSamples[i];
	IsoPlots *IP0 = &S->isoplots[0]; // mu

	int muind1(-1), muind2(-1);
	if(hasLooseMuons(muind1, muind2) > 0){
		// Common trigger selection
		if(!singleMuTrigger()) return;
		float prescale = singleMuPrescale();
		float puweight = PUWeight;
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
		////////////////////////////////////////////////////
	}
	return;
}
void MuonPlotter::fillElIsoPlots(gSample i){
	fDoCounting = false;
	Sample *S = fSamples[i];
	IsoPlots *IP = &S->isoplots[1]; // mu

	int elind1(-1), elind2(-1);
	if(hasLooseElectrons(elind1, elind2) > 0){
		// Common trigger selection
		if(!singleElTrigger()) return;
		float prescale = singleElPrescale();
		float puweight = PUWeight;
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
	}
	return;
}

//____________________________________________________________________________
void MuonPlotter::storeNumbers(Sample *S, gChannel chan, gRegion reg){
	Channel *C;
	if(chan == Muon)     C = &S->region[reg].mm;
	if(chan == Electron) C = &S->region[reg].ee;
	if(chan == EMu)      C = &S->region[reg].em;
	S->numbers[reg][chan].nt2  = C->nt20_pt->GetEntries();
	S->numbers[reg][chan].nt10 = C->nt10_pt->GetEntries();
	S->numbers[reg][chan].nt01 = C->nt01_pt->GetEntries();
	S->numbers[reg][chan].nt0  = C->nt00_pt->GetEntries();
	if(chan != EMu){
		S->numbers[reg][chan].nsst = C->fntight->GetEntries();
		S->numbers[reg][chan].nssl = C->fnloose->GetEntries();
		S->numbers[reg][chan].nzt  = C->pntight->GetEntries();
		S->numbers[reg][chan].nzl  = C->pnloose->GetEntries();
	}
}

//____________________________________________________________________________
void MuonPlotter::initCounters(gSample sample){
	fCounters[sample][Muon]    .fill("All events",                             0.);
	fCounters[sample][Muon]    .fill(" ... is good run",                       0.);
	fCounters[sample][Muon]    .fill(" ... passes triggers",                   0.);
	fCounters[sample][Muon]    .fill(" ... has one jet > 50 GeV",              0.);
	fCounters[sample][Muon]    .fill(" ... passes NJets cut",                  0.);
	fCounters[sample][Muon]    .fill(" ... passes HT cut",                     0.);
	fCounters[sample][Muon]    .fill(" ... passes MET cut",                    0.);
	fCounters[sample][Muon]    .fill(" ... has 1 loose muon",                  0.);
	fCounters[sample][Muon]    .fill(" ... has 2 loose muons",                 0.);
	fCounters[sample][Muon]    .fill(" ... passes Z veto",                     0.);
	fCounters[sample][Muon]    .fill(" ... passes Minv veto",                  0.);
	if(fChargeSwitch == 0) fCounters[sample][Muon]    .fill(" ... has same-sign muons",     0.);
	if(fChargeSwitch == 1) fCounters[sample][Muon]    .fill(" ... has opposite-sign muons", 0.);
	fCounters[sample][Muon]    .fill(" ... second muon passes pt cut",         0.);
	fCounters[sample][Muon]    .fill(" ... first muon passes pt cut",          0.);
	fCounters[sample][Muon]    .fill(" ... first muon passes tight cut",       0.);
	fCounters[sample][Muon]    .fill(" ... second muon passes tight cut",      0.);
	fCounters[sample][Muon]    .fill(" ... both muons pass tight cut",         0.);

	fCounters[sample][EMu]     .fill("All events",                             0.);
	fCounters[sample][EMu]     .fill(" ... is good run",                       0.);
	fCounters[sample][EMu]     .fill(" ... passes triggers",                   0.);
	fCounters[sample][EMu]     .fill(" ... has one jet > 50 GeV",              0.);
	fCounters[sample][EMu]     .fill(" ... passes NJets cut",                  0.);
	fCounters[sample][EMu]     .fill(" ... passes HT cut",                     0.);
	fCounters[sample][EMu]     .fill(" ... passes MET cut",                    0.);
	fCounters[sample][EMu]     .fill(" ... has a loose muon",                  0.);
	fCounters[sample][EMu]     .fill(" ... has a loose electron",              0.);
	fCounters[sample][EMu]     .fill(" ... has both",                          0.);
	fCounters[sample][EMu]     .fill(" ... passes Z veto",                     0.);
	fCounters[sample][EMu]     .fill(" ... passes Minv veto",                  0.);
	if(fChargeSwitch == 0) fCounters[sample][EMu]     .fill(" ... has same-sign electron muon pair", 0.);
	if(fChargeSwitch == 1) fCounters[sample][EMu]     .fill(" ... has opp.-sign electron muon pair", 0.);
	fCounters[sample][EMu]     .fill(" ... muon passes pt cut",                0.);
	fCounters[sample][EMu]     .fill(" ... electron passes pt cut",            0.);
	fCounters[sample][EMu]     .fill(" ... muon passes tight cut",             0.);
	fCounters[sample][EMu]     .fill(" ... electron passes tight cut",         0.);
	fCounters[sample][EMu]     .fill(" ... both e and mu pass tight cuts",     0.);

	fCounters[sample][Electron].fill("All events",                             0.);
	fCounters[sample][Electron].fill(" ... is good run",                       0.);
	fCounters[sample][Electron].fill(" ... passes triggers",                   0.);
	fCounters[sample][Electron].fill(" ... has one jet > 50 GeV",              0.);
	fCounters[sample][Electron].fill(" ... passes NJets cut",                  0.);
	fCounters[sample][Electron].fill(" ... passes HT cut",                     0.);
	fCounters[sample][Electron].fill(" ... passes MET cut",                    0.);
	fCounters[sample][Electron].fill(" ... has 1 loose electron",              0.);
	fCounters[sample][Electron].fill(" ... has 2 loose electrons",             0.);
	fCounters[sample][Electron].fill(" ... passes Z veto",                     0.);
	fCounters[sample][Electron].fill(" ... passes Minv veto",                  0.);
	if(fChargeSwitch == 0) fCounters[sample][Electron].fill(" ... has same-sign electrons",     0.);
	if(fChargeSwitch == 1) fCounters[sample][Electron].fill(" ... has opposite-sign electrons", 0.);
	fCounters[sample][Electron].fill(" ... second electron passes pt cut",     0.);
	fCounters[sample][Electron].fill(" ... first electron passes pt cut",      0.);
	fCounters[sample][Electron].fill(" ... first electron passes tight cut",   0.);
	fCounters[sample][Electron].fill(" ... second electron passes tight cut",  0.);
	fCounters[sample][Electron].fill(" ... both electrons pass tight cut",     0.);
}
void MuonPlotter::printCutFlow(gChannel chan, int indmin, int indmax){
	vector<string>::const_iterator countit;
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i < indmin || i > indmax) continue;
		fOUTSTREAM << setw(9) << fSamples[i]->sname;
		fOUTSTREAM << " | ";
	}
	fOUTSTREAM << endl;
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][chan].countNames.begin(); countit != fCounters[0][chan].countNames.end(); ++countit ){
		fOUTSTREAM << setw(40) << *countit;
		fOUTSTREAM << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(i < indmin || i > indmax) continue;
			fOUTSTREAM << setw(9) << setprecision(3) << fCounters[i][chan].counts(*countit) << " | ";
		}
		fOUTSTREAM << endl;
	}
}
void MuonPlotter::printCutFlows(TString filename){
	fOUTSTREAM.open(filename.Data(), ios::trunc);
	fOUTSTREAM << " Printing Cutflow for Mu/Mu channel..." << endl;
	// printCutFlow(Muon, DoubleMu1, EleHad2);
	printCutFlow(Muon, DoubleMu1, MuEG2);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Muon, TTJets, GJets200);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Muon, LM0, LM13);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Muon, QCDMuEnr10, QCD470);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Muon, QCD600, QCD1000MG);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << endl << endl;
	
	fOUTSTREAM << " Printing Cutflow for E/Mu channel..." << endl;
	// printCutFlow(EMu, DoubleMu1, EleHad2);
	printCutFlow(EMu, DoubleMu1, MuEG2);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(EMu, TTJets, GJets200);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(EMu, LM0, LM13);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(EMu, QCDMuEnr10, QCD470);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(EMu, QCD600, QCD1000MG);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << endl << endl;
	
	fOUTSTREAM << " Printing Cutflow for E/E channel..." << endl;
	// printCutFlow(Electron, DoubleMu1, EleHad2);
	printCutFlow(Electron, DoubleMu1, MuEG2);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Electron, TTJets, GJets200);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Electron, LM0, LM13);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Electron, QCDMuEnr10, QCD470);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printCutFlow(Electron, QCD600, QCD1000MG);
	fOUTSTREAM << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	fOUTSTREAM << endl << endl;

	fOUTSTREAM.close();
}
void MuonPlotter::printCutFlowsOld(TString filename){
	// Remove existing cutflow file
	// char cmd[100];
	// sprintf(cmd,"rm -f %s", filename.Data());
	// system(cmd);

	ofstream OUT(filename.Data(), ios::trunc);
	vector<string>::const_iterator countit;

	OUT << " Printing Cutflow for Mu/Mu channel..." << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc > 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][Muon].countNames.begin(); countit != fCounters[0][Muon].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc > 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][Muon].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc == 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][Muon].countNames.begin(); countit != fCounters[0][Muon].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc == 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][Muon].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl << endl;
	
	
	OUT << " Printing Cutflow for E/Mu channel..." << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc > 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][EMu].countNames.begin(); countit != fCounters[0][EMu].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc > 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][EMu].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc == 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][EMu].countNames.begin(); countit != fCounters[0][EMu].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc == 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][EMu].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << endl << endl;
	OUT << " Printing Cutflow for E/E channel..." << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc > 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][Electron].countNames.begin(); countit != fCounters[0][Electron].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc > 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][Electron].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT << " Cutname                                 | ";
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(fSamples[i]->datamc == 0) continue;
		OUT << setw(16) << fSamples[i]->sname;
		OUT << " | ";
	}
	OUT << endl;
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

	for( countit = fCounters[0][Electron].countNames.begin(); countit != fCounters[0][Electron].countNames.end(); ++countit ){
		OUT << setw(40) << *countit;
		OUT << " | ";

		for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
			if(fSamples[i]->datamc == 0) continue;
			OUT << setw(16) << setprecision(3) << fCounters[i][Electron].counts(*countit) << " | ";
		}
		OUT << endl;
	}
	OUT << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	OUT.close();
}

//____________________________________________________________________________
void MuonPlotter::printYields(gChannel chan, float lumiscale){
	cout << setfill('-') << setw(97) << "-" << endl;
	TString name = "Mu/Mu";
	if(chan == Electron) name = "E/E";
	if(chan == EMu) name = "E/Mu";
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
		nt20[EMu]    += S->numbers[Baseline][EMu].nt2;
		nt10[EMu]    += S->numbers[Baseline][EMu].nt10;
		nt01[EMu]    += S->numbers[Baseline][EMu].nt01;
		nt00[EMu]    += S->numbers[Baseline][EMu].nt0;
	}		

	for(size_t i = 0; i < elsamples.size(); ++i){
		Sample *S = fSamples[elsamples[i]];
		nt20[Electron]    += S->numbers[Baseline][Electron].nt2;
		nt10[Electron]    += S->numbers[Baseline][Electron].nt10;
		nt00[Electron]    += S->numbers[Baseline][Electron].nt0;
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
		nt2sum_emu   += scale*S->numbers[Baseline][EMu]     .nt2;
		nt10sum_emu  += scale*S->numbers[Baseline][EMu]     .nt10;
		nt01sum_emu  += scale*S->numbers[Baseline][EMu]     .nt01;
		nt0sum_emu   += scale*S->numbers[Baseline][EMu]     .nt0;
		nt2sum_ee    += scale*S->numbers[Baseline][Electron].nt2;
		nt10sum_ee   += scale*S->numbers[Baseline][Electron].nt10;
		nt0sum_ee    += scale*S->numbers[Baseline][Electron].nt0;

		cout << setw(9) << S->sname << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Muon]    .nt0  << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][EMu]     .nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][EMu]     .nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][EMu]     .nt01 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][EMu]     .nt0  << " || ";
		cout << setw(7) << scale*S->numbers[Baseline][Electron].nt2  << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Electron].nt10 << " | ";
		cout << setw(7) << scale*S->numbers[Baseline][Electron].nt0  << " || ";
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
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][EMu]     .nt2  << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][EMu]     .nt10 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][EMu]     .nt01 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][EMu]     .nt0  << " || ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Electron].nt2  << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Electron].nt10 << " | ";
	cout << setw(7) << scale * fSamples[LM0]->numbers[Baseline][Electron].nt0  << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(9) << "data"  << " || ";
	cout << setw(7) << nt20[Muon]     << " | ";
	cout << setw(7) << nt10[Muon]     << " | ";
	cout << setw(7) << nt00[Muon]     << " || ";
	cout << setw(7) << nt20[EMu]      << " | ";
	cout << setw(7) << nt10[EMu]      << " | ";
	cout << setw(7) << nt01[EMu]      << " | ";
	cout << setw(7) << nt00[EMu]      << " || ";
	cout << setw(7) << nt20[Electron] << " | ";
	cout << setw(7) << nt10[Electron] << " | ";
	cout << setw(7) << nt00[Electron] << " || ";
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;

}

//____________________________________________________________________________
void MuonPlotter::bookHistos(Sample *S){
	// Kinematical histos
	for(size_t k = 0; k < gNKinSels; ++k){
		for(size_t j = 0; j < gNKinVars; ++j){
			TString name = Form("%s_%s_%s", S->sname.Data(), gKinSelNames[k].Data(), KinPlots::var_name[j].Data());
			S->kinplots[k].hvar[j] = new TH1D(name, KinPlots::var_name[j].Data(), KinPlots::nbins[j], KinPlots::xmin[j], KinPlots::xmax[j]);
			S->kinplots[k].hvar[j]->SetFillColor(S->color);
			S->kinplots[k].hvar[j]->SetXTitle(KinPlots::axis_label[j]);
			S->kinplots[k].hvar[j]->Sumw2();
		}
	}

	// HWW histos
	TString hwwlabels[gNHWWSels] = {"", "AllCuts", "Barrel"};
	for(size_t k = 0; k < gNHWWSels; ++k){
		for(size_t j = 0; j < gNHWWVars; ++j){
			TString name = Form("%s_%s_%s", S->sname.Data(), gHWWSelNames[k].Data(), HWWPlots::var_name[j].Data());
			S->hwwplots[k].hvar[j] = new TH1D(name, HWWPlots::var_name[j].Data(), HWWPlots::nbins[j], HWWPlots::xmin[j], HWWPlots::xmax[j]);
			S->hwwplots[k].hvar[j]->SetFillColor(S->color);
			S->hwwplots[k].hvar[j]->SetXTitle(HWWPlots::axis_label[j]);
			S->hwwplots[k].hvar[j]->Sumw2();
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
			TString name = Form("%s_%s_ntight-%s", S->sname.Data(), gEMULabel[l].Data(), FRatioPlots::var_name[j].Data());
			S->ratioplots[l].ntight[j] = new TH1D(name, "ntight", FRatioPlots::nbins[j], FRatioPlots::xmin[j], FRatioPlots::xmax[j]);
			S->ratioplots[l].ntight[j]->SetFillColor(S->color);
			S->ratioplots[l].ntight[j]->SetXTitle(FRatioPlots::var_name[j]);
			S->ratioplots[l].ntight[j]->Sumw2();
			name = Form("%s_%s_nloose-%s", S->sname.Data(), gEMULabel[l].Data(), FRatioPlots::var_name[j].Data());
			S->ratioplots[l].nloose[j] = new TH1D(name, "nloose", FRatioPlots::nbins[j], FRatioPlots::xmin[j], FRatioPlots::xmax[j]);
			S->ratioplots[l].nloose[j]->SetFillColor(S->color);
			S->ratioplots[l].nloose[j]->SetXTitle(FRatioPlots::var_name[j]);
			S->ratioplots[l].nloose[j]->Sumw2();
		}
	}

	for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
		Region *R = &S->region[r];
		for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
			Channel *C;
			if(c == Muon)     C = &R->mm;
			if(c == Electron) C = &R->ee;
			if(c == EMu)      C = &R->em;
			TString rootname = S->sname + "_" + Region::sname[r] + "_" + C->sname;
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
			if(c == Electron){
				C->nt20_OS_BB_pt = new TH2D(rootname + "_NT20_OS_BB_pt",  "NT20_OS_BB_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_BB_pt ->Sumw2();
                   C->nt20_OS_EE_pt = new TH2D(rootname + "_NT20_OS_EE_pt",  "NT20_OS_EE_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_EE_pt ->Sumw2();
                   C->nt20_OS_EB_pt = new TH2D(rootname + "_NT20_OS_EB_pt",  "NT20_OS_EB_pt",  getNPt2Bins(c), getPt2Bins(c), getNPt2Bins(c), getPt2Bins(c)); C->nt20_OS_EB_pt ->Sumw2();
			}
			if(c == EMu){
				C->nt20_OS_BB_pt = new TH2D(rootname + "_NT20_OS_BB_pt",  "NT20_OS_BB_pt",  getNPt2Bins(Muon), getPt2Bins(Muon), getNPt2Bins(Electron), getPt2Bins(Electron)); C->nt20_OS_BB_pt ->Sumw2();
                   C->nt20_OS_EE_pt = new TH2D(rootname + "_NT20_OS_EE_pt",  "NT20_OS_EE_pt",  getNPt2Bins(Muon), getPt2Bins(Muon), getNPt2Bins(Electron), getPt2Bins(Electron)); C->nt20_OS_EE_pt ->Sumw2();
			}

			// Ratios
			if(c != EMu){
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
void MuonPlotter::deleteHistos(Sample *S){
	// Kinematical histos
	for(size_t k = 0; k < gNKinSels; ++k) for(size_t j = 0; j < gNKinVars; ++j) delete S->kinplots[k].hvar[j];

	// HWW histos
	for(size_t k = 0; k < gNHWWSels; ++k) for(size_t j = 0; j < gNHWWVars; ++j) delete S->hwwplots[k].hvar[j];

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

	for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
		Region *R = &S->region[r];
		for(gChannel c = channels_begin; c < gNCHANNELS; c=gChannel(c+1)){
			Channel *C;
			if(c == Muon)     C = &R->mm;
			if(c == Electron) C = &R->ee;
			if(c == EMu)      C = &R->em;

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
			if(c == Electron){
				delete C->nt20_OS_BB_pt;
				delete C->nt20_OS_EE_pt;
				delete C->nt20_OS_EB_pt;
			}
			if(c == EMu){
				delete C->nt20_OS_BB_pt;
				delete C->nt20_OS_EE_pt;
			}

			// Ratios
			if(c != EMu){
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
void MuonPlotter::writeHistos(Sample *S, TFile *pFile){
	pFile->cd();
	TDirectory* sdir = Util::FindOrCreate(S->sname, pFile);
	sdir->cd();

	// Kinematic histos
	TString temp = S->sname + "/KinPlots/";
	TDirectory* rdir = Util::FindOrCreate(temp, pFile);
	rdir->cd();
	for(size_t k = 0; k < gNKinSels; ++k){
		KinPlots *kp = &S->kinplots[k];
		for(size_t j = 0; j < gNKinVars; ++j) kp->hvar[j]->Write(kp->hvar[j]->GetName(), TObject::kWriteDelete);
	}

	// HWW histos
	temp = S->sname + "/HWWPlots/";
	rdir = Util::FindOrCreate(temp, pFile);
	rdir->cd();
	for(size_t k = 0; k < gNHWWSels; ++k){
		HWWPlots *hww = &S->hwwplots[k];
		for(size_t j = 0; j < gNHWWVars; ++j) hww->hvar[j]->Write(hww->hvar[j]->GetName(), TObject::kWriteDelete);
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
	for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){
		Region *R = &S->region[r];
		TString temp = S->sname + "/" + Region::sname[r];
		TDirectory* rdir = Util::FindOrCreate(temp, pFile);
		rdir->cd();

		for(gChannel ch = channels_begin; ch < gNCHANNELS; ch=gChannel(ch+1)){ // Loop over channels, mumu, emu, ee
			Channel *C;
			if(ch == Muon)     C = &R->mm;
			if(ch == Electron) C = &R->ee;
			if(ch == EMu)      C = &R->em;
			C->nt20_pt    ->Write(C->nt20_pt    ->GetName(), TObject::kWriteDelete);
			C->nt10_pt    ->Write(C->nt10_pt    ->GetName(), TObject::kWriteDelete);
			C->nt01_pt    ->Write(C->nt01_pt    ->GetName(), TObject::kWriteDelete);
			C->nt00_pt    ->Write(C->nt00_pt    ->GetName(), TObject::kWriteDelete);
			C->nt20_eta   ->Write(C->nt20_eta   ->GetName(), TObject::kWriteDelete);
			C->nt10_eta   ->Write(C->nt10_eta   ->GetName(), TObject::kWriteDelete);
			C->nt01_eta   ->Write(C->nt01_eta   ->GetName(), TObject::kWriteDelete);
			C->nt00_eta   ->Write(C->nt00_eta   ->GetName(), TObject::kWriteDelete);
			
			if(ch == Electron || ch == EMu){
				C->nt20_OS_BB_pt->Write(C->nt20_OS_BB_pt->GetName(), TObject::kWriteDelete);
				C->nt20_OS_EE_pt->Write(C->nt20_OS_EE_pt->GetName(), TObject::kWriteDelete);
				if(ch == Electron) C->nt20_OS_EB_pt->Write(C->nt20_OS_EB_pt->GetName(), TObject::kWriteDelete);					
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
			if(ch != EMu){
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

		// Kinematic histos
		for(size_t k = 0; k < gNKinSels; ++k){
			KinPlots *kp = &S->kinplots[k];
			for(size_t j = 0; j < gNKinVars; ++j){
				getname = Form("%s_%s_%s", S->sname.Data(), gKinSelNames[k].Data(), KinPlots::var_name[j].Data());
				kp->hvar[j] = (TH1D*)pFile->Get(S->sname + "/KinPlots/" + getname);
				kp->hvar[j]->SetFillColor(S->color);
			}
		}

		// HWW histos
		for(size_t k = 0; k < gNHWWSels; ++k){
			HWWPlots *hwwp = &S->hwwplots[k];
			for(size_t j = 0; j < gNHWWVars; ++j){
				getname = Form("%s_%s_%s", S->sname.Data(), gHWWSelNames[k].Data(), HWWPlots::var_name[j].Data());
				hwwp->hvar[j] = (TH1D*)pFile->Get(S->sname + "/HWWPlots/" + getname);
				hwwp->hvar[j]->SetFillColor(S->color);
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
				getname = Form("%s_%s_ntight-%s", S->sname.Data(), gEMULabel[lep].Data(), FRatioPlots::var_name[j].Data());
				rp->ntight[j] = (TH1D*)pFile->Get(S->sname + "/FRatioPlots/" + getname);
				getname = Form("%s_%s_nloose-%s", S->sname.Data(), gEMULabel[lep].Data(), FRatioPlots::var_name[j].Data());
				rp->nloose[j] = (TH1D*)pFile->Get(S->sname + "/FRatioPlots/" + getname);
			}
		}

		// Yields
		for(gRegion r = region_begin; r < gNREGIONS; r=gRegion(r+1)){ // Loop over regions
			Region *R = &S->region[r];
			for(gChannel ch = channels_begin; ch < gNCHANNELS; ch=gChannel(ch+1)){ // Loop over channels, mumu, emu, ee
				Channel *C;
				if(ch == Muon)     C = &R->mm;
				if(ch == Electron) C = &R->ee;
				if(ch == EMu)      C = &R->em;
				TString root = S->sname + "/" + Region::sname[r] + "/" + S->sname + "_" + Region::sname[r] + "_" + C->sname;
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
				if(ch == Electron || ch == EMu){
					C->nt20_OS_BB_pt = (TH2D*)pFile->Get(root + "_NT20_OS_BB_pt");
					C->nt20_OS_EE_pt = (TH2D*)pFile->Get(root + "_NT20_OS_EE_pt");
					if(ch == Electron) C->nt20_OS_EB_pt = (TH2D*)pFile->Get(root + "_NT20_OS_EB_pt");
				}

				if(ch != EMu){
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
			storeNumbers(S, Muon,     r);
			storeNumbers(S, Electron, r);
			storeNumbers(S, EMu,      r);
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

	fH2D_ElfRatio    = new TH2D("ElfRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Electron), getPt2Bins(Electron), getNEtaBins(Electron), getEtaBins(Electron));
	fH1D_ElfRatioPt  = new TH1D("ElfRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Electron), getPt2Bins(Electron));
	fH1D_ElfRatioEta = new TH1D("ElfRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Electron), getEtaBins(Electron));
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

	fH2D_ElpRatio    = new TH2D("ElpRatio",    "Ratio of tight to loose Electrons vs Pt vs Eta", getNPt2Bins(Electron), getPt2Bins(Electron), getNEtaBins(Electron), getEtaBins(Electron));
	fH1D_ElpRatioPt  = new TH1D("ElpRatioPt",  "Ratio of tight to loose Electrons vs Pt",        getNPt2Bins(Electron), getPt2Bins(Electron));
	fH1D_ElpRatioEta = new TH1D("ElpRatioEta", "Ratio of tight to loose Electrons vs Eta",       getNEtaBins(Electron), getEtaBins(Electron));
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
// Geninfo stuff:
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
	if(chan == Electron){
		axis->SetTitle("e Origin");
		for(size_t i = 1; i <= 15; ++i){
			axis->SetBinLabel(i, elBinToLabel(i));
		}
	}
	return;
}
void MuonPlotter::label2OriginAxes(TAxis *axis1, TAxis *axis2, gChannel chan){
	if(chan == Muon || chan == Electron){
		labelOriginAxis(axis1, chan);
		labelOriginAxis(axis2, chan);		
	}
	if(chan == EMu){
		labelOriginAxis(axis1, Muon);
		labelOriginAxis(axis2, Electron);		
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
	printOriginSummary2L(2, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		print2MuOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(1, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		print2MuOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(0, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;

	printMuOriginHeader("SSTi");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(1, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("SSLo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(2, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("Z Ti");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 3, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(3, Muon, reg);
	fOUTSTREAM << "=========================================================================================================================" << endl << endl;
	printMuOriginHeader("Z Lo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		if(i > QCDMuEnr10) continue;
		printMuOriginFromSample(fSamples[i], 4, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(4, Muon, reg);
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
	printOriginSummary2L(2, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		print2ElOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(1, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		print2ElOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(0, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;

	printElOriginHeader("SSTi");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(1, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("SSLo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 2, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(2, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("Z Ti");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 3, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(3, Electron, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printElOriginHeader("Z Lo");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printElOriginFromSample(fSamples[i], 4, reg);
	}
	fOUTSTREAM << "-------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary(4, Electron, reg);
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
	printOriginSummary2L(2, EMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT10");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 1, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(1, EMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT01");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 10, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(10, EMu, reg);
	fOUTSTREAM << "=============================================================================================================================================" << endl << endl;
	printEMuOriginHeader("NT00");
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		printEMuOriginsFromSample(fSamples[i], 0, reg);
	}
	fOUTSTREAM << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	printOriginSummary2L(0, EMu, reg);
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

void MuonPlotter::printMuOriginFromSample(Sample *S, int toggle, gRegion reg){
	if(S->datamc == 0 || S->datamc == 2 ) return;
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	Channel *C = &S->region[reg].mm;

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
void MuonPlotter::printElOriginFromSample(Sample *S, int toggle, gRegion reg){
	if(S->datamc == 0 || S->datamc == 2 ) return;
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	Channel *C = &S->region[reg].ee;

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

void MuonPlotter::print2MuOriginsFromSample(Sample *S, int toggle, gRegion reg){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2) return;
	Channel *C = &S->region[reg].mm;

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
void MuonPlotter::print2ElOriginsFromSample(Sample *S, int toggle, gRegion reg){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2) return;
	Channel *C = &S->region[reg].ee;

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
void MuonPlotter::printEMuOriginsFromSample(Sample *S, int toggle, gRegion reg){
	if(S->datamc == 0) return;
	if(toggle != 0 && toggle != 1 && toggle != 2 && toggle != 10) return;
	Channel *C = &S->region[reg].em;

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

void MuonPlotter::printOriginSummary(int toggle, gChannel chan, gRegion reg){
	if(toggle != 1 && toggle != 2 && toggle != 3 && toggle != 4) return;
	TH1D *histosum = new TH1D("SST_Origin_Sum", "SSTOrigin",  15, 0, 15);
	histosum->Sumw2();
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		// if(S->sname == "QCD_5to15") continue;
		if(chan == Muon && i > QCDMuEnr10) continue;
		if(S->datamc == 0 || S->datamc == 2) continue; // no data or signal
		Channel *C;
		if(chan == Muon)     C = &S->region[reg].mm;
		if(chan == Electron) C = &S->region[reg].ee;

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
	if(chan == Electron) ncols = 11;
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum->GetBinContent(15)) << " |" << endl;
	delete histosum;
}
void MuonPlotter::printOriginSummary2L(int toggle, gChannel chan, gRegion reg){
	if(toggle != 0 && toggle != 1 && toggle != 10 && toggle != 2) return;
	TH1D *histosum1 = new TH1D("SST_Origin_Sum1", "SSTOrigin",  15, 0, 15);
	TH1D *histosum2 = new TH1D("SST_Origin_Sum2", "SSTOrigin",  15, 0, 15);
	histosum1->Sumw2();
	histosum2->Sumw2();
	for(gSample i = sample_begin; i < gNSAMPLES; i=gSample(i+1)){
		Sample *S = fSamples[i];
		if(S->datamc == 0 || S->datamc == 2) continue; // no data or signal
		Channel *C;
		if(chan == Muon)     C = &S->region[reg].mm;
		if(chan == Electron) C = &S->region[reg].ee;
		if(chan == EMu)      C = &S->region[reg].em;

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
	if(chan == EMu)      fOUTSTREAM << " Weighted Sum: muon |         |         |";
	if(chan == Electron) fOUTSTREAM << " Weighted Sum: el1  |";
	int ncols = 9;
	if(chan == Electron) ncols = 11;
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum1->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum1->GetBinContent(15)) << " |" << endl;

	if(chan == Muon)     fOUTSTREAM << "               mu2  |";
	if(chan == EMu)      fOUTSTREAM << "               ele  |";
	if(chan == Electron) fOUTSTREAM << "               el2  |";
	if(chan == EMu) ncols = 11;	
	for(size_t i = 1; i <= ncols; ++i)	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum2->GetBinContent(i)) << " |";
	fOUTSTREAM << setw(6)  << setprecision(3) << Form(" %6.2f%%", histosum2->GetBinContent(15)) << " |" << endl;
	delete histosum1;
	delete histosum2;
}

//////////////////////////////////////////////////////////////////////////////
// Trigger stuff:
//____________________________________________________________________________
bool MuonPlotter::mumuSignalTrigger(){
	return doubleMuTrigger();
	// return doubleMuHTTrigger();
}
bool MuonPlotter::elelSignalTrigger(){
	return doubleElTrigger();
	// return doubleElHTTrigger();
}
bool MuonPlotter::elmuSignalTrigger(){
	return eMuTrigger();
	// return eMuHTTrigger();
}

//____________________________________________________________________________
// The following triggers will always return true for MC samples, and they
// will always return false if they are called in the 'wrong' dataset
bool MuonPlotter::singleMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	return ( (HLT_Mu8_Jet40_v1 > 0) || 
	         (HLT_Mu8_Jet40_v2 > 0) ||
	         (HLT_Mu8_Jet40_v3 > 0) ||
	         (HLT_Mu8_Jet40_v4 > 0) ||
	         (HLT_Mu8_Jet40_v5 > 0) ||
	         (HLT_Mu8_Jet40_v6 > 0) );
}
float MuonPlotter::singleMuPrescale(){
	// Pretend MC samples have prescale 1.
	if(fSamples[fCurrentSample]->datamc > 0) return 1.;
	// Get the prescale factor for whichever of these triggers fired
	// Only correct if they are mutually exclusive!
	if( HLT_Mu8_Jet40_v1 > 0 ) return HLT_Mu8_Jet40_v1_PS;
	if( HLT_Mu8_Jet40_v2 > 0 ) return HLT_Mu8_Jet40_v2_PS;
	if( HLT_Mu8_Jet40_v3 > 0 ) return HLT_Mu8_Jet40_v3_PS;
	if( HLT_Mu8_Jet40_v4 > 0 ) return HLT_Mu8_Jet40_v4_PS;
	if( HLT_Mu8_Jet40_v5 > 0 ) return HLT_Mu8_Jet40_v5_PS;
	if( HLT_Mu8_Jet40_v6 > 0 ) return HLT_Mu8_Jet40_v6_PS;
	return 1.;
}
bool MuonPlotter::singleElTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	return ( (HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v1 > 0) || 
	         (HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v2 > 0) ||
	         (HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v3 > 0) ||
	         (HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v4 > 0) ||
	         (HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v5 > 0) );
}
float MuonPlotter::singleElPrescale(){
	// Pretend MC samples have prescale 1.
	if(fSamples[fCurrentSample]->datamc > 0) return 1.;
	// Get the prescale factor for whichever of these triggers fired
	// Only correct if they are mutually exclusive!
	if( HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v1 > 0 ) return HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v1_PS;
	if( HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v2 > 0 ) return HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v2_PS;
	if( HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v3 > 0 ) return HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v3_PS;
	if( HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v4 > 0 ) return HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v4_PS;
	if( HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v5 > 0 ) return HLT_Ele8_CaloIdL_CaloIsoVL_Jet40_v5_PS;
	return 1.;
}

bool MuonPlotter::doubleMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != DoubleMu1 && fCurrentSample != DoubleMu2) return false;

	return ( (HLT_DoubleMu6_v1 > 0) || 
	         (HLT_DoubleMu6_v2 > 0) ||
	         (HLT_DoubleMu6_v3 > 0) ||
	         (HLT_DoubleMu7_v1 > 0) ||
	         (HLT_DoubleMu7_v2 > 0) ||
	         (HLT_DoubleMu7_v3 > 0) );
}
bool MuonPlotter::doubleElTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != DoubleEle1 && fCurrentSample != DoubleEle2) return false;

	return ( (HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v1 > 0) || 
	         (HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v2 > 0) ||
	         (HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v3 > 0) ||
	         (HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v4 > 0) ||
	         (HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v5 > 0) );
}
bool MuonPlotter::doubleMuHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != DoubleMu1 && fCurrentSample != DoubleMu2) return false;

	return ( (HLT_DoubleMu3_HT160_v2 > 0) || 
	         (HLT_DoubleMu3_HT160_v3 > 0) ||
	         (HLT_DoubleMu3_HT150_v1 > 0) ||
	         (HLT_DoubleMu3_HT150_v2 > 0) ||
	         (HLT_DoubleMu3_HT150_v3 > 0) );
}
bool MuonPlotter::doubleElHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != DoubleEle1 && fCurrentSample != DoubleEle2) return false;

	return ( (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT160_v1 > 0) || 
	         (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT160_v2 > 0) ||
	         (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT160_v3 > 0) ||
	         (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT150_v1 > 0) ||
	         (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT150_v2 > 0) ||
	         (HLT_DoubleEle8_CaloIdL_TrkIdVL_HT150_v3 > 0) );
}
bool MuonPlotter::eMuTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != MuEG1 && fCurrentSample != MuEG2) return false;
	return ( (HLT_Mu17_Ele8_CaloIdL_v1 > 0) || 
	         (HLT_Mu17_Ele8_CaloIdL_v2 > 0) ||
	         (HLT_Mu17_Ele8_CaloIdL_v3 > 0) ||
	         (HLT_Mu17_Ele8_CaloIdL_v4 > 0) ||
	         (HLT_Mu17_Ele8_CaloIdL_v5 > 0) ||
	         (HLT_Mu8_Ele17_CaloIdL_v1 > 0) ||
	         (HLT_Mu8_Ele17_CaloIdL_v2 > 0) ||
	         (HLT_Mu8_Ele17_CaloIdL_v3 > 0) ||
	         (HLT_Mu8_Ele17_CaloIdL_v4 > 0) ||
	         (HLT_Mu8_Ele17_CaloIdL_v5 > 0) );
}
bool MuonPlotter::eMuHTTrigger(){
	// Pretend MC samples always fire trigger
	if(fSamples[fCurrentSample]->datamc > 0) return true;
	// Only apply this on the DoubleMu dataset!
	if(fCurrentSample != MuEG1 && fCurrentSample != MuEG2) return false;

	return ( (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT160_v1 > 0) || 
	         (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT160_v2 > 0) ||
	         (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT160_v3 > 0) ||
	         (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT150_v1 > 0) ||
	         (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT150_v2 > 0) ||
	         (HLT_Mu3_Ele8_CaloIdL_TrkIdVL_HT150_v3 > 0) );
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions:
//____________________________________________________________________________
int MuonPlotter::getNJets(){
	int njets(0);
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i)) njets++;
	return njets;
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
float MuonPlotter::getClosestJetPt(int ind, gChannel chan){
// Get the pt of the closest jet
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];
	
	float mindr = 999.;
	float cljetpt = 0.;
	for(size_t i = 0; i < NJets; ++i){
		float dr = Util::GetDeltaR(lepeta, JetEta[i], lepphi, JetPhi[i]);
		// if(dr < 0.05) continue;
		if(dr > mindr) continue;
		mindr = dr;
		cljetpt = JetPt[i];
	}
	return cljetpt;
}
float MuonPlotter::getClosestJetDR(int ind, gChannel chan){
// Get the pt of the closest jet
	float lepeta = (chan==Muon)?MuEta[ind]:ElEta[ind];
	float lepphi = (chan==Muon)?MuPhi[ind]:ElPhi[ind];
	
	float mindr = 999.;
	for(size_t i = 0; i < NJets; ++i){
		float dr = Util::GetDeltaR(lepeta, JetEta[i], lepphi, JetPhi[i]);
		if(dr > mindr) continue;
		mindr = dr;
	}
	return mindr;
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
	if(int res = isSSEvent(ind1, &MuonPlotter::isTightMuon, ind2, &MuonPlotter::isTightElectron)) return res;
	return isSSEvent(ind1, &MuonPlotter::isLooseMuon, ind2, &MuonPlotter::isLooseElectron);
}
int MuonPlotter::isOSLLEvent(int &ind1, int &ind2){
	// Check first if there is a tight-tight pair in the event
	if(int res = isOSEvent(ind1, &MuonPlotter::isTightMuon, ind2, &MuonPlotter::isTightElectron)) return res;
	return isOSEvent(ind1, &MuonPlotter::isLooseMuon, ind2, &MuonPlotter::isLooseElectron);
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

// HWW Event selections:
//____________________________________________________________________________
bool MuonPlotter::passesAddLepVeto(int ind1, int ind2, int toggle){
	if(toggle == 1){ // muons
		for(size_t i = 0; i < NMus; ++i) if(i != ind1 && i != ind2 && isTightMuon(i)) return false;
		for(size_t i = 0; i < NEls; ++i) if(isTightElectron(i)) return false;
	}
	if(toggle == 2){ // electrons
		for(size_t i = 0; i < NMus; ++i) if(isTightMuon(i)) return false;
		for(size_t i = 0; i < NEls; ++i) if(i != ind1 && i != ind2 && isTightElectron(i)) return false;
	}
	if(toggle == 3){ // emu
		for(size_t i = 0; i < NMus; ++i) if(i != ind1 && isTightMuon(i))     return false;
		for(size_t i = 0; i < NEls; ++i) if(i != ind2 && isTightElectron(i)) return false;
	}
	return true;
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
bool MuonPlotter::passesNJetCut_LooseLep(int cut){
	// This is TIGHTER than passesNJetCut, so can be called on top of the other
	int njets(0);
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet_LooseLep(i)) njets++;
	return njets>=cut;
}
bool MuonPlotter::passesJet50Cut(){
	// Return true if event contains one good jet with pt > 50
	for(size_t i = 0; i < NJets; ++i) if(isGoodJet(i, 50)) return true;
	return false;
}

//____________________________________________________________________________
bool MuonPlotter::passesHTCut(float cut){
	return getHT() >= cut;
}
bool MuonPlotter::passesMETCut(float cut){
	if(pfMET < cut) return false;
	return true;
}
bool MuonPlotter::passesZVeto(int ind1, int ind2, gChannel chan, float dm){
// Checks if there is any combination of opposite sign, same flavor tight leptons
// with invariant mass closer than dm to the Z mass, returns true if none found
// Default for dm is 15 GeV
// Ignores the pair given by ind1 and ind2

	// In EMu case, the given pair is irrelevant
	if(chan == EMu) return passesZVeto(dm);

	if(NMus > 1){
		unsigned start = 0;
		// First muon
		for(size_t i = start; i < NMus-1; ++i){
			if(chan == Muon && i == ind1) continue;
			if(isTightMuon(i)){
				TLorentzVector pmu1, pmu2;
				pmu1.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);

				// Second muon
				for(size_t j = i+1; j < NMus; ++j){ 
					if(chan == Muon && j == ind2) continue;
					if(isTightMuon(j) && (MuCharge[i] != MuCharge[j]) ){
						pmu2.SetPtEtaPhiM(MuPt[j], MuEta[j], MuPhi[j], gMMU);
						if(fabs((pmu1+pmu2).M() - gMZ) < dm) return false;
					}
				}
			}
		}
	}

	if(NEls > 1){
		unsigned start = 0;
		// if(fCurrentChannel == Electron) start = 1; // For ee, ignore first e
		// First electron
		for(size_t i = start; i < NEls-1; ++i){
			if(chan == Electron && i == ind1) continue;
			if(isTightElectron(i)){
				TLorentzVector pel1, pel2;
				pel1.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);

				// Second electron
				for(size_t j = i+1; j < NEls; ++j){
					if(chan == Electron && j == ind2) continue;
					if(isTightElectron(j) && (ElCharge[i] != ElCharge[j]) ){
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
// Checks if any combination of opposite sign, same flavor tight leptons (e or mu)
// has invariant mass closer than dm to the Z mass, returns true if none found
// Default for dm is 15 GeV

	if(NMus > 1){
		unsigned start = 0;
		// if(fCurrentChannel == Muon) start = 1; // For mumu, ignore first mu
		// First muon
		for(size_t i = start; i < NMus-1; ++i){
			if(isTightMuon(i)){
				TLorentzVector pmu1, pmu2;
				pmu1.SetPtEtaPhiM(MuPt[i], MuEta[i], MuPhi[i], gMMU);

				// Second muon
				for(size_t j = i+1; j < NMus; ++j){ 
					if(isTightMuon(j) && (MuCharge[i] != MuCharge[j]) ){
						pmu2.SetPtEtaPhiM(MuPt[j], MuEta[j], MuPhi[j], gMMU);
						if(fabs((pmu1+pmu2).M() - gMZ) < dm) return false;
					}
				}
			}
		}
	}

	if(NEls > 1){
		unsigned start = 0;
		// if(fCurrentChannel == Electron) start = 1; // For ee, ignore first e
		// First electron
		for(size_t i = start; i < NEls-1; ++i){
			if(isTightElectron(i)){
				TLorentzVector pel1, pel2;
				pel1.SetPtEtaPhiM(ElPt[i], ElEta[i], ElPhi[i], gMEL);

				// Second electron
				for(size_t j = i+1; j < NEls; ++j){
					if(isTightElectron(j) && (ElCharge[i] != ElCharge[j]) ){
						pel2.SetPtEtaPhiM(ElPt[j], ElEta[j], ElPhi[j], gMEL);
						if(fabs((pel1+pel2).M() - gMZ) < dm) return false;
					}
				}
			}
		}		
	}
	return true;
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
	if(!passesJet50Cut())           return false;
	if(MuMT[0] > 20.)               return false;
	if(pfMET > 20.)                 return false;
	if(NMus > 1)                    return false;
	return true;
}
bool MuonPlotter::isZMuMuEvent(){
	if(!passesNJetCut_LooseLep(2)) return false;
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

	if(pfMET > 20.) return false;

	return true;
}

//____________________________________________________________________________
bool MuonPlotter::isSigSupElEvent(){
	int el1(-1), el2(-1);
	if(hasLooseElectrons(el1, el2) < 1)  return false;
	if(!passesJet50Cut()) return false;
	if(ElMT[0] > 20.) return false;
	if(pfMET > 20.)   return false;
	if(NEls > 1)      return false;
	return true;
}
bool MuonPlotter::isZElElEvent(int &elind){
	if(!passesNJetCut_LooseLep(2)) return false;
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

	if(pfMET > 20.) return false;

	// If only the harder one tight or both tight, return the softer one
	// If only the softer one tight, return the harder one
	elind = el2;
	if(isTightElectron(el2) && !isTightElectron(el1)) elind = el1;
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
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes triggers");

	if(!passesJet50Cut()) return false; // one jet with pt > 50
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... has one jet > 50 GeV");

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes NJets cut");

	if(!passesHTCut(fC_minHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes HT cut");

	if(!passesMETCut(fC_minMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes MET cut");

	int nmus = hasLooseMuons(mu1, mu2);
	if(nmus < 1) return false; // >0 loose muons
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... has 1 loose muon");
	if(nmus < 2) return false; // >1 loose muons
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... has 2 loose muons");

	if(fChargeSwitch == 0){
		if(!passesZVeto()) return false; // no Zs in event
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes Z veto");
	}

	if(!passesMllEventVeto(mu1, mu2, 1, 5.)) return false; // no low mass OSSF pairs
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... passes Minv veto");

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(mu1, mu2)) != 1) return false;
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... has same-sign muons");
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(mu1, mu2)) != 1) return false;
		if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... has opposite-sign muons");		
	}

	if(!isGoodSecMuon(mu2)) return false; // pt cuts
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... second muon passes pt cut");

	if(!isGoodPrimMuon(mu1)) return false;
	if(fDoCounting) fCounters[fCurrentSample][Muon].fill(" ... first muon passes pt cut");		

	return true;
}
bool MuonPlotter::isSSLLElEvent(int& el1, int& el2){
	// This should include all the cuts for the final selection
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes triggers");

	if(!passesJet50Cut()) return false;
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... has one jet > 50 GeV");

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes NJets cut");

	if(!passesHTCut(fC_minHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes HT cut");

	if(!passesMETCut(fC_minMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes MET cut");

	int nels = hasLooseElectrons(el1, el2);
	if(nels < 1) return false; // >0 eles;
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... has 1 loose electron");
	if(nels < 2) return false; // >1 eles
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... has 2 loose electrons");

	if(fChargeSwitch == 0){
		if(!passesZVeto()) return false; // no Zs in event
		if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes Z veto");
	}

	if(!passesMllEventVeto(el1, el2, 2, 5.)) return false; // no low mass OSSF pairs
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... passes Minv veto");

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(el1, el2)) != 2) return false;
		if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... has same-sign electrons");
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(el1, el2)) != 2) return false;
		if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... has opposite-sign electrons");
	}

	if(!isGoodSecElectron(el2)) return false; // pt cuts
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... second electron passes pt cut");

	if(!isGoodPrimElectron(el1)) return false;
	if(fDoCounting) fCounters[fCurrentSample][Electron].fill(" ... first electron passes pt cut");

	return true;
}
bool MuonPlotter::isSSLLElMuEvent(int& mu, int& el){
	// This should include all the cuts for the final selection
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes triggers");

	if(!passesJet50Cut()) return false;
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has one jet > 50 GeV");

	if(!passesNJetCut(fC_minNjets) ) return false;    // njets cut
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes NJets cut");

	if(!passesHTCut(fC_minHT) )  return false;    // ht cut
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes HT cut");

	if(!passesMETCut(fC_minMet) ) return false;    // met cut
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes MET cut");

	int nmus = hasLooseMuons(mu, el);
	if(nmus > 0 && fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has a loose muon");
	int nels = hasLooseElectrons(el, mu);
	if(nels > 0 && fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has a loose electron");
	if(nels < 1 || nmus < 1) return false;
	if(nels > 0 && fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has both");

	if(fChargeSwitch == 0){
		if(!passesZVeto())       return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes Z veto");
	}

	// if(!passesMllEventVeto(mu, el, 3, 5.)) return false; // no low mass OSSF pairs
	if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... passes Minv veto");

	if(fChargeSwitch == 0){
		if(abs(isSSLLEvent(mu, el)) != 3) return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has same-sign electron muon pair");
	}
	if(fChargeSwitch == 1){
		if(abs(isOSLLEvent(mu, el)) != 3) return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... has opp.-sign electron muon pair");
	}

	if(MuPt[mu] > ElPt[el]){
		if(!isGoodPrimMuon(mu))    return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... muon passes pt cut");
		if(!isGoodSecElectron(el)) return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... electron passes pt cut");
	}
	else if(MuPt[mu] < ElPt[el]){
		if(!isGoodPrimElectron(el)) return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... electron passes pt cut");
		if(!isGoodSecMuon(mu))      return false;
		if(fDoCounting) fCounters[fCurrentSample][EMu].fill(" ... muon passes pt cut");
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

	// if(ElEcalRecHitSumEt[ele]/ElPt[ele] > 0.2) return false; // when using "CaloIsoVL" triggers

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
	for(size_t imu = 0; imu < NMus; ++imu){
		if(!isTightMuon(imu)) continue;
		// if(!isLooseMuon(imu)) continue;
		if(!isGoodSecMuon(imu)) continue; // pt > 10
		if(Util::GetDeltaR(MuEta[imu], JetEta[jet], MuPhi[imu], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	for(size_t iel = 0; iel < NEls; ++iel){
		if(!isTightElectron(iel)) continue;
		// if(!isLooseElectron(iel)) continue;
		if(!isGoodSecElectron(iel)) continue;
		if(Util::GetDeltaR(ElEta[iel], JetEta[jet], ElPhi[iel], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	if(JetPt[jet] < pt) return false;
	return true;
}
bool MuonPlotter::isGoodJet_LooseLep(int jet){
	if(jet >= NJets) return false; // Sanity check
	float minDR = 0.4;
	for(size_t imu = 0; imu < NMus; ++imu){
		if(!isLooseMuon(imu)) continue;
		if(!isGoodSecMuon(imu)) continue;
		if(Util::GetDeltaR(MuEta[imu], JetEta[jet], MuPhi[imu], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	for(size_t iel = 0; iel < NEls; ++iel){
		if(!isLooseElectron(iel)) continue;
		if(!isGoodSecElectron(iel)) continue;
		if(Util::GetDeltaR(ElEta[iel], JetEta[jet], ElPhi[iel], JetPhi[jet]) > minDR ) continue;
		return false;
	}
	return true;
}
