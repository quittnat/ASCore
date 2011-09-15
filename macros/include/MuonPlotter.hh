/*****************************************************************************
*   Collection of tools for producing plots for Muon Fake Rate Analysis      *
*                                                                            *
*                                                  (c) 2010 Benjamin Stieger *
*****************************************************************************/
#ifndef MuonPlotter_HH
#define MuonPlotter_HH

#include "helper/AnaClass.hh"
#include "helper/FPRatios.hh"
#include "helper/Monitor.hh"
#include "helper/Davismt2.h"

#include "TLorentzVector.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"


class MuonPlotter : public AnaClass{

public:
	// Binning
	static const int gNMuPtbins  = 7;
	static const int gNMuPt2bins = 7;
	static const int gNMuEtabins = 4;
	static const int gNElPtbins  = 5;
	static const int gNElPt2bins = 5;
	static const int gNElEtabins = 4;
	
	static const int gNNVrtxBins = 9;

	static double gNVrtxBins[gNNVrtxBins+1];

	static double gMuPtbins [gNMuPtbins+1];
	static double gMuPt2bins[gNMuPt2bins+1];
	static double gMuEtabins[gNMuEtabins+1];

	static double gElPtbins [gNElPtbins+1];
	static double gElPt2bins[gNElPt2bins+1];
	static double gElEtabins[gNElEtabins+1];

	static const int gNDiffHTBins  = 5;
	static const int gNDiffMETBins = 3;
	static const int gNDiffNJBins  = 5;
	static const int gNDiffMT2Bins = 3;

	static double gDiffHTBins [gNDiffHTBins+1];
	static double gDiffMETBins[gNDiffMETBins+1];
	static double gDiffNJBins [gNDiffNJBins+1];
	static double gDiffMT2Bins[gNDiffMT2Bins+1];

	static double gEChMisIDB;
	static double gEChMisIDB_E;
	static double gEChMisIDE;
	static double gEChMisIDE_E;

	// This enum has to correspond to the content of the samples.dat file
	enum gSample {
		sample_begin,
		DoubleMu1 = sample_begin, DoubleMu2, DoubleMu3, DoubleMu4,
		DoubleEle1, DoubleEle2, DoubleEle3, DoubleEle4,
		MuEG1, MuEG2, MuEG3, MuEG4,
		MuHad1, MuHad2, EleHad1, EleHad2,
		TTJetsSync,
		TTJets, TJets_t, TJets_tW, TJets_s, WJets, DYJets, DYJets50, DYJets10to50, GJets40, GJets100, GJets200,
		GVJets, WW, WZ, ZZ, VVTo4L, DPSWW, WpWp, WmWm, ttbarW,
		LM0, LM1, LM2, LM3, LM4, LM5, LM6, LM7, LM8, LM9, LM11, LM12, LM13, 
		QCDMuEnr10,
		QCD5,
		QCD15,
		QCD30,
		QCD50,
		QCD80,
		QCD120,
		QCD170,
		QCD300,
		QCD470,
		QCD600,
		QCD800,
		QCD1000,
		QCD1400,
		QCD1800,
		QCD50MG,
		QCD100MG,
		QCD250MG,
		QCD500MG,
		QCD1000MG,
		gNSAMPLES
	};
	enum gHiLoSwitch{
		HighPt,
		LowPt,
		TauChan
	};
	enum gFPSwitch{
		SigSup,
		ZDecay
	};
	enum gRegion {
		region_begin,
		Baseline = region_begin,
		Control,
		HT80MET100,
		HT200MET30,
		HT200MET120,
		HT400MET50,
		HT400MET120,
		gNREGIONS
	};
	enum gChannel {
		channels_begin,
		Muon = channels_begin,
		ElMu,
		Elec,
		gNCHANNELS
	};
	struct lepton{
		lepton(){};
		lepton(TLorentzVector vec, int ch, int ty, int ind){
			p = vec;
			charge = ch;
			type = ty;
			index = ind;
		};
		TLorentzVector p;
		int charge;
		int type; // -1(unknown), 0(mu), 1(ele)
		int index;
	};
	
	struct NumberSet{
		long nt2;
		long nt10;
		long nt01;
		long nt0;
		long nsst;
		long nssl;
		long nzl;
		long nzt;
	};

	struct Channel{ // like mumu, emu, ee
		TString name;
		TString sname;
		TH2D *nt20_pt; // pt1 vs pt2
		TH2D *nt10_pt;
		TH2D *nt01_pt; // only filled for e/mu
		TH2D *nt00_pt;
		TH2D *nt20_eta; // eta1 vs eta2
		TH2D *nt10_eta;
		TH2D *nt01_eta;
		TH2D *nt00_eta;

		TH2D *fntight; // pt vs eta
		TH2D *fnloose; 
		TH2D *pntight; // pt vs eta
		TH2D *pnloose;

		// Gen matched yields: t = tight, p = prompt, etc.
		TH2D *npp_pt; // overall pp/fp/.., binned in pt1 vs pt2
		TH2D *npp_cm_pt; // charge misid
		TH2D *nfp_pt;
		TH2D *npf_pt; // only filled for e/mu
		TH2D *nff_pt;
		TH2D *nt2pp_pt; // pp/fp/.. in tt window, binned in pt1 vs pt2
		TH2D *nt2pp_cm_pt; // charge misid
		TH2D *nt2fp_pt;
		TH2D *nt2pf_pt; // only filled for e/mu
		TH2D *nt2ff_pt;

		// Origin histos
		TH2D *nt11_origin;
		TH2D *nt10_origin;
		TH2D *nt01_origin;
		TH2D *nt00_origin;
		TH1D *sst_origin;
		TH1D *ssl_origin;
		TH1D *zt_origin;
		TH1D *zl_origin;

		// OS Yields
		// Only filled for electrons
		// For e/mu channel, use only BB and EE to count e's in barrel and endcaps
		TH2D *nt20_OS_BB_pt; // binned in pt1 vs pt2
		TH2D *nt20_OS_EE_pt; // BB = barrel/barrel, EE = endcap/endcap
		TH2D *nt20_OS_EB_pt; // EB = barrel/endcap
	};
	
	struct Region{
		static TString sname[gNREGIONS];
		// Two different pt cuts
		static float minMu1pt[2];
		static float minMu2pt[2];
		static float minEl1pt[2];
		static float minEl2pt[2];
		// Custom selections for every region
		static float minHT   [gNREGIONS];
		static float maxHT   [gNREGIONS];
		static float minMet  [gNREGIONS];
		static float maxMet  [gNREGIONS];
		static int   minNjets[gNREGIONS];
		
		Channel mm;
		Channel em;
		Channel ee;
	};
	
	// static const int gNRatioVars = 8;
	static const int gNRatioVars = 9;
	struct FRatioPlots{
		static TString var_name[gNRatioVars];
		static int nbins[gNRatioVars];
		static float xmin[gNRatioVars];
		static float xmax[gNRatioVars];
		TH1D *ntight[gNRatioVars];
		TH1D *nloose[gNRatioVars];
	};
	
	static const int gNKinVars = 10;
	struct KinPlots{
		static TString var_name[gNKinVars];
		static TString axis_label[gNKinVars];
		static int nbins[gNKinVars];
		static float xmin[gNKinVars];
		static float xmax[gNKinVars];
		TH1D *hvar[gNKinVars];
		TH2D *hmetvsht;
		static const int nMETBins = 100;
		static const int nHTBins = 200;
		static const float METmin = 0.;
		static const float METmax = 400.;
		static const float HTmin = 0.;
		static const float HTmax = 1000.;
	};
	
	static const int gNSels = 2;
	struct IsoPlots{
		static TString sel_name[gNSels];
		static int nbins[gNSels];
		TH1D *hiso[gNSels];
		TH1D *hiso_pt[gNSels][gNMuPt2bins];
		TH1D *hiso_nv[gNSels][gNNVrtxBins];
	};
	
	static const int gNDiffVars = 4;
	struct DiffPredYields{
		static TString var_name[gNDiffVars];
		static TString axis_label[gNDiffVars];
		static int nbins[gNDiffVars];
		static double* bins[gNDiffVars];

		TH1D *hnt11[gNDiffVars]; // SS yields
		TH1D *hnt10[gNDiffVars];
		TH1D *hnt01[gNDiffVars];
		TH1D *hnt00[gNDiffVars];

		TH1D *hnt2_os_BB[gNDiffVars]; // OS yields
		TH1D *hnt2_os_EB[gNDiffVars];
		TH1D *hnt2_os_EE[gNDiffVars];
	};
	
	static const int gNKinSels = 3;
	static TString gKinSelNames[gNKinSels];
	static TString gEMULabel[2];
	static TString gChanLabel[gNCHANNELS];
	static TString gHiLoLabel[3];

	class Sample{
	public:
		Sample(){};
		~Sample(){};
		
		TString name;
		TString sname;
		TString location;
		TFile *file;
		TTree *tree;
		float lumi;
		int color;
		int datamc; // 0: Data, 1: SM MC, 2: Signal MC, 3: rare MC
		TH1D *cutFlowHisto[gNCHANNELS];
		Region region[gNREGIONS][2];
		DiffPredYields diffyields[gNCHANNELS];
		NumberSet numbers[gNREGIONS][gNCHANNELS]; // summary of integrated numbers
		KinPlots kinplots[gNKinSels][2]; // tt and ll and signal for both low and high pt analysis
		IsoPlots isoplots[2]; // e and mu
		FRatioPlots ratioplots[2]; // e and mu
		TGraph *sigevents[gNCHANNELS][2];

		TTree* getTree(){
			file = TFile::Open(location);
			tree = (TTree*)file->Get("Analysis");
			return tree;
		}
		
		void cleanUp(){
			tree = NULL;
			file->Close();
		}
	};
	
	MuonPlotter();
	MuonPlotter(TString);
	MuonPlotter(TString, TString);
	virtual ~MuonPlotter();

	inline void setCharge(int charge){ fChargeSwitch = charge; };

	virtual void init(TString filename = "samples.dat");
	virtual void InitMC(TTree*); // remove a few branches
	virtual void readSamples(const char* filename = "samples.dat");

	virtual void doAnalysis();
	virtual void doLoop();
	
	virtual void sandBox();
	
	//////////////////////////////
	// Plots
	void makeMuIsolationPlots();
	void makeElIsolationPlots();
	
	void makeNT2KinPlots(gHiLoSwitch = HighPt);
	void makeMETvsHTPlot(vector<int>, vector<int>, vector<int>, gHiLoSwitch = HighPt);
	void makeMETvsHTPlotCustom();
	void makeMETvsHTPlotTau();
	
	void makeFRvsPtPlots(gChannel, gFPSwitch);
	void makeFRvsEtaPlots(gChannel);
	void makeFRvsPtPlotsForPAS(gChannel);
	void makeFRvsEtaPlotsForPAS(gChannel);
	void makeRatioPlots(gChannel);
	void makeNTightLoosePlots(gChannel);
	
	void makeIsoVsMETPlot(gSample);
	void makePileUpPlots();
	
	void makeMCClosurePlots(vector<int>);
	void makeDataClosurePlots();
	void makeNT012Plots(vector<int>, gChannel, gRegion = Baseline, gHiLoSwitch = HighPt);
	void makeNT012Plots(gChannel, vector<int>, bool(MuonPlotter::*)(int&, int&), TString = "");

	void makeIntPrediction(TString, gRegion, gHiLoSwitch = HighPt);
	void makeDiffPrediction();
	void makeIntMCClosure(TString, gHiLoSwitch = HighPt);	
	void makeTTbarClosure();
	
	void printSyncExercise();
	
	//////////////////////////////
	// Fake ratios
	// Produce from tree, with given selections:
	void produceRatio(gChannel, int, int, bool(MuonPlotter::*)(), bool(MuonPlotter::*)(int), TH2D*&, TH1D*&, TH1D*&, bool = false);
	void produceRatio(gChannel, vector<int>, int, bool(MuonPlotter::*)(), bool(MuonPlotter::*)(int), TH2D*&, TH1D*&, TH1D*&, bool = false);

	TH1D* fillMuRatioPt(int, int, bool(MuonPlotter::*)(), bool(MuonPlotter::*)(int), bool = false);
	TH1D* fillMuRatioPt(vector<int>, int, bool(MuonPlotter::*)(), bool(MuonPlotter::*)(int), bool = false);
	TH1D* fillMuRatioPt(vector<int>, int, bool(MuonPlotter::*)(), bool(MuonPlotter::*)(int), const int, const double*, const int, const double*, bool = false);

	// Calculate from pre stored numbers, with fixed selections:
	void fillMuElRatios(vector<int>);

	TH1D* fillMuRatioPt(int, gFPSwitch, bool = false);
	TH1D* fillMuRatioPt(vector<int>, gFPSwitch, bool = false);
	TH1D* fillElRatioPt(int, gFPSwitch, bool = false);
	TH1D* fillElRatioPt(vector<int>, gFPSwitch, bool = false);

	void calculateRatio(vector<int>, gChannel, gFPSwitch, TH2D*&, bool = false);
	void calculateRatio(vector<int>, gChannel, gFPSwitch, TH2D*&, TH1D*&, TH1D*&, bool = false);
	void calculateRatio(vector<int>, gChannel, gFPSwitch, float&, float&);
	void calculateRatio(vector<int>, gChannel, gFPSwitch, float&, float&, float&);
	
	void getPassedTotal(vector<int>,  gChannel, gFPSwitch, TH2D*&, TH2D*&, bool = false, gHiLoSwitch = HighPt);
	TH1D* getFRatio(vector<int>, gChannel, int = 0, bool = false);

	void ratioWithBinomErrors(float, float, float&, float&);
	void ratioWithPoissErrors(float, float, float&, float&);
	void ratioWithAsymmCPErrors(int, int, float&, float&, float&);

	//////////////////////////////
	// Predictions
	void makeSSMuMuPredictionPlots(vector<int>, bool = false, gHiLoSwitch = HighPt);
	void makeSSElElPredictionPlots(vector<int>, bool = false, gHiLoSwitch = HighPt);
	void makeSSElMuPredictionPlots(vector<int>, bool = false, gHiLoSwitch = HighPt);
	void NObs(gChannel, TH1D *&, vector<int>, bool(MuonPlotter::*)());
	void NObs(gChannel, TH1D *&, vector<int>, gRegion = Baseline, gHiLoSwitch = HighPt);
	void NObs(gChannel, THStack *&, vector<int>, gRegion = Baseline, gHiLoSwitch = HighPt);
	vector<TH1D*> MuMuFPPrediction(TH2D* fratio, TH2D* pratio, TH2D* nt2, TH2D* nt1, TH2D* nt0, bool output = false);
	vector<TH1D*> ElElFPPrediction(TH2D* fratio, TH2D* pratio, TH2D* nt2, TH2D* nt1, TH2D* nt0, bool output = false);
	vector<TH1D*> ElMuFPPrediction(TH2D* mufratio, TH2D* mupratio, TH2D* elfratio, TH2D* elpratio,  TH2D* nt2, TH2D* nt10, TH2D* nt01, TH2D* nt0, bool output = false);

	void storeNumbers(Sample*, gChannel, gRegion);
	
	// Cutflow
	void initCutNames();
	void initCounters(gSample);
	void fillCutFlowHistos(gSample);
	void printCutFlow(gChannel, gSample, gSample);
	void printCutFlows(TString);
	
	void printYields(gChannel, float = -1.0);
	void printYieldsShort(float = -1);

	//////////////////////////////
	// Fillers
	void fillYields(Sample*, gRegion, gHiLoSwitch);
	void fillDiffYields(Sample*);
	void fillRatioPlots(Sample*);
	
	void fillMuIsoPlots(gSample);
	void fillElIsoPlots(gSample);
	void fillKinPlots(gSample, gHiLoSwitch);
	
	//////////////////////////////
	// I/O
	void bookHistos(Sample*);
	void deleteHistos(Sample*);
	void writeHistos(Sample*, TFile*);
	void writeSigGraphs(Sample*, gChannel, gHiLoSwitch, TFile*);
	int readHistos(TString);
	int readSigGraphs(TString);
	
	void bookRatioHistos();
	void fixPRatios();
	
	// Geninfo stuff:
	int muIndexToBin(int);
	int elIndexToBin(int);
	TString muBinToLabel(int);
	TString elBinToLabel(int);
	void labelOriginAxis(TAxis*, gChannel);
	void label2OriginAxes(TAxis*, TAxis*, gChannel);
	
	void printOrigins(gRegion = Baseline);
	void printMuOriginTable(gRegion = Baseline);
	void printMuOriginHeader(TString);
	void printMuOriginFromSample(Sample*, int, gRegion = Baseline, gHiLoSwitch = HighPt);
	void print2MuOriginsFromSample(Sample*, int, gRegion = Baseline, gHiLoSwitch = HighPt);

	void printElOriginTable(gRegion = Baseline);
	void printElOriginHeader(TString);
	void printElOriginFromSample(Sample*, int, gRegion = Baseline, gHiLoSwitch = HighPt);
	void print2ElOriginsFromSample(Sample*, int, gRegion = Baseline, gHiLoSwitch = HighPt);

	void printEMuOriginTable(gRegion = Baseline);
	void printEMuOriginHeader(TString);
	void printEMuOriginsFromSample(Sample*, int, gRegion = Baseline, gHiLoSwitch = HighPt);

	void printOriginSummary(vector<int>, int, gChannel, gRegion = Baseline, gHiLoSwitch = HighPt);
	void printOriginSummary2L(vector<int>, int, gChannel, gRegion = Baseline, gHiLoSwitch = HighPt);

	// Trigger selections:
	virtual bool  singleMuTrigger();
	virtual float singleMuPrescale();
	virtual bool  singleElTrigger();
	virtual float singleElPrescale();

	virtual bool mumuSignalTrigger();
	virtual bool elelSignalTrigger();
	virtual bool elmuSignalTrigger();

	virtual bool doubleMuTrigger();
	virtual bool doubleElTrigger();
	virtual bool eMuTrigger();

	virtual bool doubleMuHTTrigger();
	virtual bool doubleElHTTrigger();
	virtual bool eMuHTTrigger();
	
	virtual bool isGoodRun(gSample);

	// Event and Object selectors:
	virtual int getNJets();
	virtual int getNBTags();
	virtual float getHT();
	virtual float getMT2(int, int, int);
	virtual int   getClosestJet(int, gChannel);
	virtual float getClosestJetPt(int, gChannel);
	virtual float getClosestJetDR(int, gChannel);
	virtual float getSecondClosestJetDR(int, gChannel);
	virtual float getAwayJetPt(int, gChannel);
	virtual float getMaxJPt();
	
	virtual int isSSLLEvent(int&, int&);
	virtual int isOSLLEvent(int&, int&);
	virtual int isSSEvent(int&, bool(MuonPlotter::*)(int), int&, bool(MuonPlotter::*)(int));
	virtual int isOSEvent(int&, bool(MuonPlotter::*)(int), int&, bool(MuonPlotter::*)(int));
	vector<lepton> sortLeptonsByPt(vector<lepton> &leptons);
	vector<lepton> sortLeptonsByTypeAndPt(vector<lepton> &leptons);

	virtual bool isGoodEvent();
	virtual bool isGoodMuEvent();
	virtual int hasLooseMuons(int&, int&);
	virtual int hasLooseMuons();
	virtual int hasLooseElectrons(int&, int&);
	virtual int hasLooseElectrons();
	virtual bool passesNJetCut(int=2);
	virtual bool passesJet50Cut();
	
	virtual bool passesHTCut(float, float = 7000.);
	virtual bool passesMETCut(float = -1., float = 7000.);
	virtual bool passesZVeto(bool(MuonPlotter::*)(int), bool(MuonPlotter::*)(int), float = 15.); // cut with mZ +/- cut value and specified obj selectors
	virtual bool passesZVeto(float = 15.); // cut with mZ +/- cut value
	virtual bool passesMllEventVeto(float = 5.);
	virtual bool passesMllEventVeto(int, int, int, float = 5.);

	virtual bool isSigSupMuEvent();
	virtual bool isZMuMuEvent();

	virtual bool isSigSupElEvent();
	virtual bool isZElElEvent(int&);

	virtual bool isGenMatchedSUSYDiLepEvent();
	virtual bool isGenMatchedSUSYDiLepEvent(int&, int&);

	virtual bool isGenMatchedSUSYEEEvent();
	virtual bool isGenMatchedSUSYEMuEvent();

	virtual bool isSSLLMuEvent(int&, int&);
	virtual bool isSSLLElEvent(int&, int&);
	virtual bool isSSLLElMuEvent(int&, int&);

	virtual bool isGoodMuon(int, float = -1.);
	virtual bool isLooseMuon(int);
	virtual bool isTightMuon(int);
	virtual bool isGoodPrimMuon(int, float = -1.);
	virtual bool isGoodSecMuon(int, float = -1.);

	virtual bool isFakeMuon(int);
	virtual bool isPromptMuon(int);
	virtual bool isChargeMatchedMuon(int);

	virtual bool isGoodElectron(int, float = -1.);
	virtual bool isLooseElectron(int);
	virtual bool isTightElectron(int);
	virtual bool isGoodPrimElectron(int, float = -1.);
	virtual bool isGoodSecElectron(int, float = -1.);

	virtual bool isFakeElectron(int);
	virtual bool isPromptElectron(int);
	virtual bool isChargeMatchedElectron(int);

	virtual bool isBarrelElectron(int);

	virtual bool isGoodJet(int, float = 30.);
	
	virtual void drawTopLine();

	float fC_minHT;
	float fC_minMet;
	float fC_maxHT;
	float fC_maxMet;
	int   fC_minNjets;
	float fC_minMu1pt;
	float fC_minMu2pt;
	float fC_minEl1pt;
	float fC_minEl2pt;
	float fC_maxMet_Control;
	float fC_maxMt_Control;
	
	void resetHypLeptons();
	void setHypLepton1(int, gChannel);
	void setHypLepton2(int, gChannel);
	lepton fHypLepton1;
	lepton fHypLepton2;
	
	const int     getNPtBins (gChannel);
	const double *getPtBins  (gChannel);
	const int     getNPt2Bins(gChannel);
	const double *getPt2Bins (gChannel);
	const int     getNEtaBins(gChannel);
	const double *getEtaBins (gChannel);
	
	const double *getDiffPredBins(int);
	
	Monitor fCounters[gNSAMPLES][3];
	vector<string> fMMCutNames;
	vector<string> fEMCutNames;
	vector<string> fEECutNames;
	bool fDoCounting;
	gSample fCurrentSample;
	gChannel fCurrentChannel;
	int fCurrentRun;
	ofstream fOUTSTREAM, fOUTSTREAM2;

	int fChargeSwitch;    // 0 for SS, 1 for OS

	vector<int> fMCBG;    // SM background MC samples
	vector<int> fMCBGSig; // SM background + LM0 signal samples
	vector<int> fMCBGMuEnr;    // SM background MC samples with Muon enriched QCD
	vector<int> fMCBGMuEnrSig; // SM background + LM0 signal samples with Muon enriched QCD
	vector<int> fMCRareSM; // Rare SM backgrounds
	vector<int> fMuData;  // Muon data samples
	vector<int> fEGData;  // EG data samples
	vector<int> fMuEGData;  // MuEG dataset
	vector<int> fMuHadData;  // Muon data samples
	vector<int> fEleHadData;  // EG data samples
	vector<int> fHighPtData;  // All high pt triggered data
	vector<int> fLowPtData;   // All lepton cross HT triggered data
	
	float fLumiNorm;      // Normalize everything to this luminosity
	float fBinWidthScale; // Normalize bin contents to this width

	vector<Sample*>::iterator fS;
	vector<Sample*> fSamples;
	vector<Sample*> fMCSamples;
	map<TString, Sample*> fSampleMap;	// Mapping of sample to name
	
	vector<float> fSigEv_HI_MM_HT;
	vector<float> fSigEv_HI_MM_MET;
	vector<float> fSigEv_LO_MM_HT;
	vector<float> fSigEv_LO_MM_MET;
	vector<float> fSigEv_HI_EE_HT;
	vector<float> fSigEv_HI_EE_MET;
	vector<float> fSigEv_LO_EE_HT;
	vector<float> fSigEv_LO_EE_MET;
	vector<float> fSigEv_HI_EM_HT;
	vector<float> fSigEv_HI_EM_MET;
	vector<float> fSigEv_LO_EM_HT;
	vector<float> fSigEv_LO_EM_MET;
	
	
	TFile *fStorageFile;
	TString fOutputFileName;
	TLatex *fLatex;
	
	
	TH2D *fH2D_MufRatio;
	TH1D *fH1D_MufRatioPt;
	TH1D *fH1D_MufRatioEta;
	TH2D *fH2D_MupRatio;
	TH1D *fH1D_MupRatioPt;
	TH1D *fH1D_MupRatioEta;

	TH2D *fH2D_ElfRatio;
	TH1D *fH1D_ElfRatioPt;
	TH1D *fH1D_ElfRatioEta;
	TH2D *fH2D_ElpRatio;
	TH1D *fH1D_ElpRatioPt;
	TH1D *fH1D_ElpRatioEta;
	
	private:
	
};

#endif
