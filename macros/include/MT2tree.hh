#ifndef MT2tree_hh
#define MT2tree_hh

#include "TObject.h"
#include "TLorentzVector.h"

enum {m_jetSize = 20, m_eleSize = 10, m_muoSize = 10};

class MT2Misc : public TObject {

public:
  MT2Misc();
  virtual ~MT2Misc();

  void Reset();
  
  Int_t    Run;
  Int_t    Event;
  Int_t    LumiSection;
  Float_t  Weight;
  Int_t    LeptConfig;
  Bool_t   IsCleanMultiJetEvent;
  Bool_t   IsCleanJetEvent;
  Int_t    R12R21;
  Int_t    NJetsPt50Eta25;
  Double_t PseudoJetMT2;
  Double_t PseudoJetMCT;
  Double_t PseudoJet1Pt;
  Double_t PseudoJet2Pt;
  Double_t PseudojetAlphaT;
  Double_t Vectorsumpt;
  Double_t PFMET;
  Double_t PFMETphi;
  Double_t PFMETsign;
  Double_t LeadingJetEta;
  Double_t DPhiJ1Met;
  Double_t DPhiJ2Met;
  Double_t PseudoJetMT2AxisdPhi;
  Double_t R1221min;
  Double_t MPT_sel;
  Double_t MPT;
  Double_t MHT;
  Double_t HT;
  Double_t DPhiMhtMpt;

  ClassDef(MT2Misc, 1)
};

class MT2Jet : public TObject {

public:
  MT2Jet();
  virtual ~MT2Jet();

  void Reset();
  void SetLV(const TLorentzVector v);

  TLorentzVector lv;

  Double_t bTagProbTCHE;
  Double_t bTagProbTCHP;
  Double_t bTagProbSSVHE;
  Double_t bTagProbSSVHP;

  Bool_t isPFIDLoose;
  Bool_t isPFIDMedium;
  Bool_t isPFIDTight;

  Double_t ChHadFrac;
  Double_t NeuHadFrac;
  Double_t ChEmFrac;
  Int_t    ChMult;
  Int_t    NeuMult;
  Int_t    NConstituents;

  Int_t    inHemisphere;

  ClassDef(MT2Jet, 1)
};

class MT2tree : public TObject {

public:
  MT2tree();
  virtual ~MT2tree();

  void Reset();

  void SetNJets (int n);
  void SetNEles (int n);
  void SetNMuons(int n);
  
  // My functions here
  Double_t PseudoJetDPhi ();
  Double_t PseudoJetAngle();
  Double_t JetsDPhi(int j1=1, int j2=0);
  Double_t MetJetDPhi(int ijet = 0);
  Double_t MinMetJetDPhi();
  Int_t    MinMetJetDPhiIndex();
  Double_t GetMT2(double testmass=0 , bool massive=false);
  Double_t GetMCT(bool massive=false);

  MT2Misc misc;
  Int_t   NJets;
  Int_t   NEles;
  Int_t   NMuons;
  MT2Jet         jet[m_jetSize];
  TLorentzVector ele[m_eleSize];
  TLorentzVector muo[m_muoSize];
  TLorentzVector pfmet[2];
  TLorentzVector pseudoJets[2];

  
  ClassDef(MT2tree, 1)
};

#endif
