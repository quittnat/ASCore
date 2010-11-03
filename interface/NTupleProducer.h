// -*- C++ -*-
//
// Package:    NTupleProducer
// Class:      NTupleProducer
//
/* class NTupleProducer
   NTupleProducer.h
   AnalysisExamples/NTupleProducer/src/NTupleProducer.h
   Description: Template to produce NTuples for ETH SUSY Analysis

   Implementation:

*/
//
// Original Author:  Benjamin Stieger
//         Created:  Wed Sep  2 16:43:05 CET 2009
// $Id: NTupleProducer.h,v 1.66 2010/10/28 16:44:42 fronga Exp $
//
//


// system include files
#include <vector>
#include <string>

// ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

// Framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// Data formats
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
	
// Helpers
#include "Math/VectorUtil.h"

// Local classes
#include "DiLeptonAnalysis/NTupleProducer/interface/JetFillerReco.h"



typedef math::XYZTLorentzVector LorentzVector;

class NTupleProducer : public edm::EDAnalyzer {
public:
  explicit NTupleProducer(const edm::ParameterSet&);
  ~NTupleProducer();

  virtual void beginJob(); //336  beginJob(const edm::EventSetup&)
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();
  virtual void endRun(const edm::Run&, const edm::EventSetup&);
  vector<const reco::GenParticle*> matchRecoCand(const reco::RecoCandidate *Cand, const edm::Event& iEvent);
  const reco::GenJet* matchJet(const reco::Jet *jet, const edm::Event& iEvent);
  void switchDouble(double &, double &);
  void switchInt(int &, int &);
  void resetDouble(double *v, unsigned int size = 20);
  void resetInt(int *v, unsigned int size = 20);
  void resetTree();

private:

  const reco::Track* getElectronTrack(const reco::GsfElectron& gsfElectron, const float minFracSharedHits);
  std::pair<double, double> getConversionInfo(math::XYZTLorentzVector trk1_p4,int trk1_q, float trk1_d0,math::XYZTLorentzVector trk2_p4,int trk2_q, float trk2_d0,float bFieldAtOrigin);
  reco::TrackRef getConversionPartnerTrack(const reco::GsfElectron& gsfElectron, const edm::Handle<reco::TrackCollection>& track_h, const float bFieldAtOrigin, double& Dist, double& DCot, const float maxAbsDist = 0.02, const float maxAbsDCot = 0.02, const float minFracSharedHits = 0.45);

  virtual void ElectronDuplicate(vector<const SuperCluster*> elecPtr, vector<const GsfTrack*> trckPtr);
  virtual void PhotonElectronDuplicate(vector<const SuperCluster*>, vector<const SuperCluster*>);
  virtual void ElJetOverlap(vector<const Jet*> jets, vector<const SuperCluster*> electrons, edm::Handle<CaloTowerCollection> calotowers);
  virtual void PhotonJetOverlap(vector<const Jet*> jets, vector<const SuperCluster*> electrons, edm::Handle<CaloTowerCollection> calotowers);
  virtual bool IsEMObjectInJet(const SuperCluster* theElecSC, vector<CaloTowerPtr> jetCaloRefs, edm::Handle<CaloTowerCollection> calotowers, math::XYZVector* sharedMomentum);
  virtual bool EMCaloTowerWindow(const SuperCluster* superCluster, float & phimin, float & phimax, float & etamin, float & etamax);
  virtual float CaloTowerSizePhi(float eta);
  virtual float CaloTowerSizeEta(float eta);
  virtual bool IsInPhiWindow(float phi, float phimin, float phimax);
  virtual float DeltaPhiSigned(float v1, float v2);
  virtual float GetPhiMin(float phi1, float phi2);
  virtual float GetPhiMax(float phi1, float phi2);

  typedef pair<int,double> OrderPair;
  struct IndexByPt {
    const bool operator()(const OrderPair& j1, const OrderPair& j2 ) const {
      return j1.second > j2.second;
    }
  };
	

  // ----------member data ---------------------------
  edm::Service<TFileService> fTFileService;

  std::vector<JetFillerBase*> jetFillers;

  bool fIsRealData;
  bool fIsPat;
  int fNTotEvents;
  int fNFillTree;

  static const int gMaxnmus     = 30;
  static const int gMaxneles    = 20;
  static const int gMaxnjets    = 100;
  static const int gMaxntrks    = 500;
  static const int gMaxnphos    = 50;
  static const int gMaxngenlept = 100;
  static const int gMaxnvrtx    = 25;

  edm::InputTag fMuonTag;
  edm::InputTag fElectronTag;
  string fEleIdWP;
  edm::InputTag fMuIsoDepTkTag;
  edm::InputTag fMuIsoDepECTag;
  edm::InputTag fMuIsoDepHCTag;
  edm::InputTag fJetTag;
  string fJetCorrs;
  edm::InputTag fBtag1Tag;
  edm::InputTag fBtag2Tag;
  edm::InputTag fBtag3Tag;
  edm::InputTag fBtag4Tag;
  edm::InputTag fJetTracksTag;
  edm::InputTag fJetIDTag;
  edm::InputTag fMET1Tag;
  edm::InputTag fMET2Tag;
  edm::InputTag fMET3Tag;
  edm::InputTag fMET4Tag;
  edm::InputTag fMET5Tag;
  edm::InputTag fMET6Tag;
  edm::InputTag fVertexTag;
  edm::InputTag fTrackTag;
  edm::InputTag fPhotonTag;
  edm::InputTag fCalTowTag;
  edm::InputTag fEBRecHitsTag;
  edm::InputTag fEERecHitsTag;
  edm::InputTag fGenPartTag;
  edm::InputTag fGenJetTag;
  edm::InputTag fL1TriggerTag;
  edm::InputTag fHLTTriggerTag;
  edm::InputTag fHLTTrigEventTag;
  edm::InputTag fHBHENoiseResultTag;

  double fMinmupt;
  double fMaxmueta;
  double fMinelpt;
  double fMaxeleta;
  double fMincorjpt;
  double fMinrawjpt;
  double fMaxjeta;
  double fMinjemfrac;
  double fMintrkpt;
  double fMaxtrketa;
  double fMaxtrknchi2;
  int	fMintrknhits;

  double fMinphopt;
  double fMaxphoeta;
	
  double fMingenleptpt; 
  double fMaxgenlepteta;

  double fJUNC_px_match[gMaxnjets];
  double fJUNC_py_match[gMaxnjets];
  double fJUNC_pz_match[gMaxnjets];

  TH1I *fHhltstat; // Added to keep track of trigger names
  TH1I *fHl1physstat;
  TH1I *fHl1techstat;
  bool fFirstevent;

  // Trigger stuff
  string fProcessName; // process name of (HLT) process for which to get HLT configuration
  HLTConfigProvider fHltConfig;

  ////////////////////////////////////////////////////////
  // Trees:
  TTree *fRunTree;

  int fRTrunnumber;
  double fRTextxslo;
  double fRTextxsnlo;
  double fRTintxs;

  double fRTMinmupt;
  double fRTMaxmueta;
  double fRTMinelpt;
  double fRTMaxeleta;
  double fRTMinjpt;
  double fRTMinrawjpt;
  double fRTMaxjeta;
  double fRTMinjemfrac;

  double fRTMintrkpt;
  double fRTMaxtrketa;
  double fRTMaxtrknchi2;
  int fRTMintrknhits;

  double fRTMinphopt;
  double fRTMaxphoeta;

  int fRTmaxnmu;
  int fRTmaxnel;
  int fRTmaxnjet;
  int fRTmaxntrk;
  int fRTmaxnphot;

  TTree *fEventTree;

  // General event information
  int fTrunnumber;
  int fTeventnumber;
  int fTlumisection;
  double fTpthat;
  int fTsigprocid;
  double fTpdfscalePDF;
  int fTpdfid1;
  int fTpdfid2;
  double fTpdfx1;
  double fTpdfx2;
  double fTpdfxPDF1;
  double fTpdfxPDF2;
  double fTextxslo;
  double fTintxs;
  double fTweight;
  int fTHBHENoiseFlag;

  int fTgoodvtx;
  double fTprimvtxx;
  double fTprimvtxy;
  double fTprimvtxz;
  double fTprimvtxrho;
  double fTprimvtxxE;
  double fTprimvtxyE;
  double fTprimvtxzE;
  double fTpvtxznchi2;
  int    fTpvtxndof;
  int    fTpvtxisfake;
  double fTpvtxptsum;

  double fTbeamspotx;
  double fTbeamspoty;
  double fTbeamspotz;

  int fTnvrtx;
  double fTvrtxx[gMaxnvrtx];
  double fTvrtxy[gMaxnvrtx];
  double fTvrtxz[gMaxnvrtx];
  double fTvrtxxE[gMaxnvrtx];
  double fTvrtxyE[gMaxnvrtx];
  double fTvrtxzE[gMaxnvrtx];
  double fTvrtxndof[gMaxnvrtx];
  double fTvrtxchi2[gMaxnvrtx];
  double fTvrtxntrks[gMaxnvrtx];
  double fTvrtxsumpt[gMaxnvrtx];
  int fTvrtxisfake[gMaxnvrtx];

  int fTNCaloTowers;

  // Trigger
  static const unsigned int gMaxhltbits = 200;
  static const unsigned int gMaxl1physbits = 128;
  static const unsigned int gMaxl1techbits = 64;
  int fTHLTres[gMaxhltbits];
  int fTL1physres[gMaxl1physbits];
  int fTL1techres[gMaxl1techbits];
  int fTHLTprescale[gMaxhltbits];
  std::vector<std::string> fTHLTmenu;
  std::vector<std::string> fTL1physmenu;

  static const unsigned int gMaxhltnobjs  = 10;
  std::vector<std::string> fTHltLabels; // HLT Paths to store the triggering objects of
  unsigned int fTNpaths;
  unsigned int fTNHLTobjects;
  int**    fTHLTObjectID;
  double** fTHLTObjectPt;
  double** fTHLTObjectEta;
  double** fTHLTObjectPhi;

  // Flags
  int fTgoodevent;         // 0 for good events, 1 for bad events
  int fTflagmaxmuexc;      // Found more than 20 muons in event (0 is good, 1 is bad)
  int fTflagmaxelexc;      // Found more than 20 electrons in event
  int fTflagmaxujetexc;    // Found more than 50 jets in event
  int fTflagmaxjetexc;     // Found more than 50 uncorrected jets in event
  int fTflagmaxtrkexc;     // Found more than 500 tracks in event
  int fTflagmaxphoexc;     // Found more than 500 photons in event
  int fTflagmaxgenleptexc; // Found more than 100 genleptons in event
  int fTflagmaxvrtxexc;    // Found more than 25 vertices in event
  
  // GenLeptons
  
  int fTngenleptons;      
  int fTGenLeptonId[gMaxngenlept];      
  double fTGenLeptonPt[gMaxngenlept];      
  double fTGenLeptonEta[gMaxngenlept];     
  double fTGenLeptonPhi[gMaxngenlept];     
  int fTGenLeptonMId[gMaxngenlept];     
  int fTGenLeptonMStatus[gMaxngenlept]; 
  double fTGenLeptonMPt[gMaxngenlept];     
  double fTGenLeptonMEta[gMaxngenlept];    
  double fTGenLeptonMPhi[gMaxngenlept];    
  int fTGenLeptonGMId[gMaxngenlept];    
  int fTGenLeptonGMStatus[gMaxngenlept];
  double fTGenLeptonGMPt[gMaxngenlept];    
  double fTGenLeptonGMEta[gMaxngenlept];   
  double fTGenLeptonGMPhi[gMaxngenlept]; 
	
	

  // Muons:
  int fTnmu;
  int fTnmutot; // before preselection
  int fTnglobalmu;
  int fTntrackermu;
  int fTgoodmu[gMaxnmus];
  int fTmuIsIso[gMaxnmus];
  int fTmuIsGM[gMaxnmus];
  int fTmuIsTM[gMaxnmus];
  double fTmupx[gMaxnmus];
  double fTmupy[gMaxnmus];
  double fTmupz[gMaxnmus];
  double fTmue[gMaxnmus];
  double fTmuet[gMaxnmus];
  double fTmupt[gMaxnmus];
  double fTmuptE[gMaxnmus];
  double fTmueta[gMaxnmus];
  double fTmuphi[gMaxnmus];
  int fTmucharge[gMaxnmus];

  // - Isolation Variables
  double fTmuiso[gMaxnmus];
  double fTmuIso03sumPt[gMaxnmus];
  double fTmuIso03emEt[gMaxnmus];
  double fTmuIso03hadEt[gMaxnmus];
  double fTmuIso03emVetoEt[gMaxnmus];
  double fTmuIso03hadVetoEt[gMaxnmus];
  double fTmuIso05sumPt[gMaxnmus];
  double fTmuIso05emEt[gMaxnmus];
  double fTmuIso05hadEt[gMaxnmus];
  double fTmueecal[gMaxnmus];
  double fTmuehcal[gMaxnmus];

  // - Impact Parameters
  double fTmud0bs[gMaxnmus];
  double fTmud0pv[gMaxnmus];
  double fTmud0E[gMaxnmus];
  double fTmudzbs[gMaxnmus];
  double fTmudzpv[gMaxnmus];
  double fTmudzE[gMaxnmus];

  // - MuID Variables
  double fTmunchi2[gMaxnmus];
  int fTmunglhits[gMaxnmus];
  int fTmunmuhits[gMaxnmus];
  int fTmuntkhits[gMaxnmus];
  int fTmunpxhits[gMaxnmus];
  double fTmuinntknchi2[gMaxnmus];
  int fTmunmatches[gMaxnmus];
  int fTmunchambers[gMaxnmus];
  double fTmucalocomp[gMaxnmus];
  double fTmusegmcomp[gMaxnmus];

  int fTmuIsGMPT[gMaxnmus];
  int fTmuIsGMTkChiComp[gMaxnmus];
  int fTmuIsGMStaChiComp[gMaxnmus];
  int fTmuIsGMTkKinkTight[gMaxnmus];
  int fTmuIsAllStaMuons[gMaxnmus];
  int fTmuIsAllTrkMuons[gMaxnmus];
  int fTmuIsTrkMuArb[gMaxnmus];
  int fTmuIsAllArb[gMaxnmus];
  int fTmuIsTMLastStationLoose[gMaxnmus];
  int fTmuIsTMLastStationTight[gMaxnmus];
  int fTmuIsTM2DCompLoose[gMaxnmus];
  int fTmuIsTM2DCompTight[gMaxnmus];
  int fTmuIsTMOneStationLoose[gMaxnmus];
  int fTmuIsTMOneStationTight[gMaxnmus];
  int fTmuIsTMLSOPL[gMaxnmus];
  int fTmuIsTMLastStationAngLoose[gMaxnmus];
  int fTmuIsTMLastStationAngTight[gMaxnmus];
  int fTmuIsTMOneStationAngTight[gMaxnmus];
  int fTmuIsTMOneStationAngLoose[gMaxnmus];

  double fTmuoutmomx[gMaxnmus];
  double fTmuoutmomy[gMaxnmus];
  double fTmuoutmomz[gMaxnmus];
  double fTmuoutmomphi[gMaxnmus];
  double fTmuoutmometa[gMaxnmus];
  double fTmuoutmomtheta[gMaxnmus];
  double fTmuoutposrad[gMaxnmus];
  double fTmuoutposx[gMaxnmus];
  double fTmuoutposy[gMaxnmus];
  double fTmuoutposz[gMaxnmus];

  // - Gen Info:
  int    fTGenMuId[gMaxnmus];
  int    fTGenMuStatus[gMaxnmus];
  int    fTGenMuCharge[gMaxnmus];
  double fTGenMuPt[gMaxnmus];
  double fTGenMuEta[gMaxnmus];
  double fTGenMuPhi[gMaxnmus];
  double fTGenMuE[gMaxnmus];
  int    fTGenMuMId[gMaxnmus];
  int    fTGenMuMStatus[gMaxnmus];
  int    fTGenMuMCharge[gMaxnmus];
  double fTGenMuMPt[gMaxnmus];
  double fTGenMuMEta[gMaxnmus];
  double fTGenMuMPhi[gMaxnmus];
  double fTGenMuME[gMaxnmus];
  int    fTGenMuGMId[gMaxnmus];
  int    fTGenMuGMStatus[gMaxnmus];
  int    fTGenMuGMCharge[gMaxnmus];
  double fTGenMuGMPt[gMaxnmus];
  double fTGenMuGMEta[gMaxnmus];
  double fTGenMuGMPhi[gMaxnmus];
  double fTGenMuGME[gMaxnmus];


  // Electrons:
  int fTneles;
  int fTnelestot; // before preselection
  int fTgoodel[gMaxneles];
  int fTeIsIso[gMaxneles];
  int fTeChargeMisIDProb[gMaxneles];
  // Kinematics
  double fTepx[gMaxneles];
  double fTepy[gMaxneles];
  double fTepz[gMaxneles];
  double fTept[gMaxneles];
  double fTeptE[gMaxneles];
  double fTee[gMaxneles];
  double fTeet[gMaxneles];
  double fTeeta[gMaxneles];
  double fTephi[gMaxneles];
  double fTetheta[gMaxneles];
  double fTesceta[gMaxneles];
  // Impact parameter
  double fTed0bs[gMaxneles];
  double fTed0pv[gMaxneles];
  double fTed0E[gMaxneles];
  double fTedzbs[gMaxneles];
  double fTedzpv[gMaxneles];
  double fTedzE[gMaxneles];
  // Isolation
  double fTeiso03[gMaxneles];
  double fTeiso04[gMaxneles];
  double fTdr03tksumpt[gMaxneles];
  double fTdr04tksumpt[gMaxneles];
  double fTdr03ecalrechitsumet[gMaxneles];
  double fTdr04ecalrechitsumet[gMaxneles];
  double fTdr03hcaltowersumet[gMaxneles];
  double fTdr04hcaltowersumet[gMaxneles];
  double fTenchi2[gMaxneles];
  // Electron ID
  double fTeIDMva[gMaxneles];
  int fTeIDTight[gMaxneles];
  int fTeIDLoose[gMaxneles];
  int fTeIDRobustTight[gMaxneles];
  int fTeIDRobustLoose[gMaxneles];
  int fTeIDsimpleWPrelIso[gMaxneles];
  int fTeIDsimpleWP95relIso[gMaxneles];
  int fTeIDsimpleWP80relIso[gMaxneles];
  int fTecharge[gMaxneles];
  int fTeCInfoIsGsfCtfCons[gMaxneles];
  int fTeCInfoIsGsfCtfScPixCons[gMaxneles];
  int fTeCInfoIsGsfScPixCons[gMaxneles];
  int fTeCInfoScPixCharge[gMaxneles];
  double fTeClosestCtfTrackpt[gMaxneles];
  double fTeClosestCtfTracketa[gMaxneles];
  double fTeClosestCtfTrackphi[gMaxneles];
  int fTeClosestCtfTrackcharge[gMaxneles];
  int fTeInGap[gMaxneles];  // seed crystal next to a gap
  int fTeEcalDriven[gMaxneles];
  int fTeTrackerDriven[gMaxneles];
  int fTeBasicClustersSize[gMaxneles];
  double fTefbrem[gMaxneles];
  double fTeHcalOverEcal[gMaxneles];
  double fTeE1x5[gMaxneles];                      // 5x5 arround seed
  double fTeE5x5[gMaxneles];                      // 5x5 arround seed
  double fTeE2x5Max[gMaxneles];                   // 2x5 arround seed
  double fTeSigmaIetaIeta[gMaxneles];             // shower shape covariance
  double fTeDeltaPhiSeedClusterAtCalo[gMaxneles]; // Dphi (seed-track) at calo from p_out
  double fTeDeltaEtaSeedClusterAtCalo[gMaxneles]; // outermost track state extrapolated at calo
  double fTeDeltaPhiSuperClusterAtVtx[gMaxneles]; // Dphi (sc-track) at calo extrapolated from p_in
  double fTeDeltaEtaSuperClusterAtVtx[gMaxneles]; // Deta (sc-track) at calo extrapolated from p_in
  double fTecaloenergy[gMaxneles];                // caloEnergy() = supercluster energy 99.9% of the time
  double fTetrkmomatvtx[gMaxneles];               // trackMomentumAtVtx().R()
  double fTeESuperClusterOverP[gMaxneles];        // Esc/Pin
  int fTeNumberOfMissingInnerHits[gMaxneles];
  int fTeIsInJet[gMaxneles];
  double fTeSharedPx[gMaxneles];
  double fTeSharedPy[gMaxneles];
  double fTeSharedPz[gMaxneles];
  double fTeSharedEnergy[gMaxneles];
  int    fTeDupEl[gMaxneles];
  double fTeConvPartTrackDist[gMaxneles];
  double fTeConvPartTrackDCot[gMaxneles];
  double fTeConvPartTrackPt[gMaxneles];
  double fTeConvPartTrackEta[gMaxneles];
  double fTeConvPartTrackPhi[gMaxneles];
  double fTeConvPartTrackCharge[gMaxneles];
  // Spike cleaning
  int fTeScSeedSeverity[gMaxneles];
  double fTeE1OverE9[gMaxneles];
  double fTeS4OverS1[gMaxneles];
  

  // - Gen Info:
  int    fTGenElId[gMaxneles];
  int    fTGenElStatus[gMaxneles];
  int    fTGenElCharge[gMaxneles];
  double fTGenElPt[gMaxneles];
  double fTGenElEta[gMaxneles];
  double fTGenElPhi[gMaxneles];
  double fTGenElE[gMaxneles];
  int    fTGenElMId[gMaxneles];
  int    fTGenElMStatus[gMaxneles];
  int    fTGenElMCharge[gMaxneles];
  double fTGenElMPt[gMaxneles];
  double fTGenElMEta[gMaxneles];
  double fTGenElMPhi[gMaxneles];
  double fTGenElME[gMaxneles];
  int    fTGenElGMId[gMaxneles];
  int    fTGenElGMStatus[gMaxneles];
  int    fTGenElGMCharge[gMaxneles];
  double fTGenElGMPt[gMaxneles];
  double fTGenElGMEta[gMaxneles];
  double fTGenElGMPhi[gMaxneles];
  double fTGenElGME[gMaxneles];

  // Photons:
  int fTnphotons;
  int fTnphotonstot; // before preselection
  int fTgoodphoton[gMaxnphos];
  int fTPhotIsIso[gMaxnphos];
  double fTPhotPt[gMaxnphos];  
  double fTPhotPx[gMaxnphos];  
  double fTPhotPy[gMaxnphos];  
  double fTPhotPz[gMaxnphos];  
  double fTPhotEta[gMaxnphos];
  double fTPhotPhi[gMaxnphos];
  double fTPhotEnergy[gMaxnphos];
  double fTPhotIso03Ecal[gMaxnphos];
  double fTPhotIso03Hcal[gMaxnphos];
  double fTPhotIso03TrkSolid[gMaxnphos];
  double fTPhotIso03TrkHollow[gMaxnphos];
  double fTPhotIso03[gMaxnphos];
  double fTPhotIso04Ecal[gMaxnphos];
  double fTPhotIso04Hcal[gMaxnphos];
  double fTPhotIso04TrkSolid[gMaxnphos];
  double fTPhotIso04TrkHollow[gMaxnphos];
  double fTPhotIso04[gMaxnphos];
  double fTPhotcaloPosX[gMaxnphos];
  double fTPhotcaloPosY[gMaxnphos];
  double fTPhotcaloPosZ[gMaxnphos];
  double fTPhotHoverE[gMaxnphos];
  double fTPhotH1overE[gMaxnphos];
  double fTPhotH2overE[gMaxnphos];
  double fTPhotSigmaIetaIeta[gMaxnphos];
  int    fTPhotHasPixSeed[gMaxnphos];
  int    fTPhotHasConvTrks[gMaxnphos];
  int    fTPhotIsInJet[gMaxnphos];
  int    fTPhotDupEl[gMaxnphos];
  double fTPhotSharedPx[gMaxnphos];
  double fTPhotSharedPy[gMaxnphos];
  double fTPhotSharedPz[gMaxnphos];
  double fTPhotSharedEnergy[gMaxnphos];
  // Spike cleaning
  int    fTPhotScSeedSeverity[gMaxnphos];
  double fTPhotE1OverE9[gMaxnphos];
  double fTPhotS4OverS1[gMaxnphos];
	

  // Jets:
  int fTnjets;
  int fTnjetstot; // before preselection
  int fTgoodjet[gMaxnjets];
  double fTjpx[gMaxnjets];
  double fTjpy[gMaxnjets];
  double fTjpz[gMaxnjets];
  double fTje[gMaxnjets];
  double fTjet[gMaxnjets];
  double fTjpt[gMaxnjets];
  double fTjeta[gMaxnjets];
  double fTjphi[gMaxnjets];
  double fTjemfrac[gMaxnjets];
  int fTjNconstituents[gMaxnjets];
  double fTjID_HPD[gMaxnjets];
  double fTjID_RBX[gMaxnjets];
  double fTjID_n90Hits[gMaxnjets];
  double fTjID_resEMF[gMaxnjets];
  double fTjID_HCALTow[gMaxnjets];
  double fTjID_ECALTow[gMaxnjets];
  double fTJEtaRms[gMaxnjets];
  double fTJPhiRms[gMaxnjets];
  double fTjbTagProbTkCntHighEff[gMaxnjets];
  double fTjbTagProbTkCntHighPur[gMaxnjets];
  double fTjbTagProbSimpSVHighEff[gMaxnjets];
  double fTjbTagProbSimpSVHighPur[gMaxnjets];
  double fTjChfrac[gMaxnjets];
  double fTjEfracHadr[gMaxnjets];
  double fTjMass[gMaxnjets];
  int fTjnAssoTracks[gMaxnjets];
  double fTjtrk1px[gMaxnjets];
  double fTjtrk1py[gMaxnjets];
  double fTjtrk1pz[gMaxnjets];
  double fTjtrk2px[gMaxnjets];
  double fTjtrk2py[gMaxnjets];
  double fTjtrk2pz[gMaxnjets];
  double fTjtrk3px[gMaxnjets];
  double fTjtrk3py[gMaxnjets];
  double fTjtrk3pz[gMaxnjets];
  double fTjEcorr[gMaxnjets];
  double fTjeMinDR[gMaxnjets];
  double fTjetVtxx[gMaxnjets];
  double fTjetVtxy[gMaxnjets];
  double fTjetVtxz[gMaxnjets];
  double fTjetVtxExx[gMaxnjets];
  double fTjetVtxEyx[gMaxnjets];
  double fTjetVtxEyy[gMaxnjets];
  double fTjetVtxEzy[gMaxnjets];
  double fTjetVtxEzz[gMaxnjets];
  double fTjetVtxEzx[gMaxnjets];
  double fTjetVtxNChi2[gMaxnjets];
  double fTjetGenPt[gMaxnjets];
  double fTjetGenEta[gMaxnjets];
  double fTjetGenPhi[gMaxnjets];
  double fTjetGenE[gMaxnjets];
  double fTjetGenemE[gMaxnjets];
  double fTjetGenhadE[gMaxnjets];
  double fTjetGeninvE[gMaxnjets];

  // Tracks:
  int fTntracks;
  int fTntrackstot; // before preselection
  int fTgoodtrk[gMaxntrks];
  double fTtrkpt[gMaxntrks]; // this is actually charge*pt
  double fTtrketa[gMaxntrks];
  double fTtrkphi[gMaxntrks];
  double fTtrknchi2[gMaxntrks];
  double fTtrknhits[gMaxntrks];

  // (M)E(T):
  double fTTrkPtSumx;
  double fTTrkPtSumy;
  double fTTrkPtSumphi;
  double fTTrkPtSum;
  double fTSumEt;
  double fTECALSumEt;
  double fTHCALSumEt;
  double fTECALEsumx;
  double fTECALEsumy;
  double fTECALEsumz;
  double fTECALMETphi;
  double fTECALMETeta;
  double fTECALMET;
  double fTHCALEsumx;
  double fTHCALEsumy;
  double fTHCALEsumz;
  double fTHCALMETphi;
  double fTHCALMETeta;
  double fTHCALMET;
  double fTRawMET;
  double fTRawMETpx;
  double fTRawMETpy;
  double fTRawMETphi;
  double fTRawMETemEtFrac;
  double fTRawMETemEtInEB;
  double fTRawMETemEtInEE;
  double fTRawMETemEtInHF;
  double fTRawMEThadEtFrac;
  double fTRawMEThadEtInHB;
  double fTRawMEThadEtInHE;
  double fTRawMEThadEtInHF;
  double fTRawMETSignificance;
  double fTMuCorrMET;
  double fTMuCorrMETpx;
  double fTMuCorrMETpy;
  double fTMuCorrMETphi;
  double fTGenMET;
  double fTGenMETpx;
  double fTGenMETpy;
  double fTGenMETphi;
  double fTTCMET;
  double fTTCMETpx;
  double fTTCMETpy;
  double fTTCMETphi;
  double fTTCMETSignificance;
  double fTMuJESCorrMET;
  double fTMuJESCorrMETpx;
  double fTMuJESCorrMETpy;
  double fTMuJESCorrMETphi;
  double fTPFMET;
  double fTPFMETpx;
  double fTPFMETpy;
  double fTPFMETphi;
  double fTPFMETSignificance;
  double fTMETR12;
  double fTMETR21;
  ////////////////////////////////////////////////////////
};
