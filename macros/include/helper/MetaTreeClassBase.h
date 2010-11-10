//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov  2 13:01:29 2010 by ROOT version 5.25/04
// from TTree Analysis/AnalysisTree
// found on file: SSDLTrees/RA5Nov01/TTbarJets-madgraph/SSDLTree.root
//////////////////////////////////////////////////////////

#ifndef MetaTreeClassBase_h
#define MetaTreeClassBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class MetaTreeClassBase {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Run;
   Int_t           Event;
   Int_t           LumiSec;
   Float_t         ExtXSecLO;
   Float_t         IntXSec;
   Int_t           HLT_Mu9;
   Int_t           HLT_Mu11;
   Int_t           HLT_Mu15;
   Int_t           HLT_DoubleMu0;
   Int_t           HLT_DoubleMu3;
   Int_t           HLT_Jet15U;
   Int_t           HLT_Jet30U;
   Int_t           HLT_Jet50U;
   Int_t           HLT_Jet70U;
   Int_t           HLT_Jet100U;
   Int_t           HLT_HT100U;
   Int_t           HLT_HT120U;
   Int_t           HLT_HT140U;
   Int_t           HLT_HT150U;
   Int_t           HLT_Ele10_LW_L1R;
   Int_t           HLT_Ele10_SW_L1R;
   Int_t           HLT_Ele15_LW_L1R;
   Int_t           HLT_Ele15_SW_L1R;
   Int_t           HLT_Ele20_SW_L1R;
   Int_t           HLT_DoubleEle5_SW_L1R;
   Int_t           HLT_DoubleEle10_SW_L1R;
   Int_t           HLT_DoubleEle15_SW_L1R_v1;
   Int_t           HLT_Ele10_LW_EleId_L1R;
   Int_t           HLT_Ele10_SW_EleId_L1R;
   Int_t           HLT_Ele15_SW_CaloEleId_L1R;
   Int_t           HLT_Ele15_SW_EleId_L1R;
   Int_t           HLT_Ele17_SW_LooseEleId_L1R;
   Int_t           HLT_Ele17_SW_TightEleId_L1R;
   Int_t           HLT_Ele17_SW_CaloEleId_L1R;
   Int_t           HLT_Ele17_SW_EleId_L1R;
   Int_t           HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1;
   Int_t           HLT_Ele17_SW_TightEleIdIsol_L1R_v1;
   Int_t           HTL_GoodElEvent;
   Int_t           HTL_GoodElFakesEvent;
   Int_t           HTL_GoodElEvent_RA5;
   Int_t           HTL_GoodElEvent_TDL;
   Int_t           HTL_GoodMuEvent;
   Int_t           HTL_GoodHadronicEvent;
   Int_t           NMus;
   Float_t         MuPt[4];   //[NMus]
   Float_t         MuEta[4];   //[NMus]
   Float_t         MuPhi[4];   //[NMus]
   Int_t           MuCharge[4];   //[NMus]
   Int_t           MuTight[4];   //[NMus]
   Float_t         MuIso[4];   //[NMus]
   Float_t         MuIsoHybrid[4];   //[NMus]
   Float_t         MuDRJet[4];   //[NMus]
   Float_t         MuDRHardestJet[4];   //[NMus]
   Float_t         MuD0[4];   //[NMus]
   Float_t         MuDz[4];   //[NMus]
   Float_t         MuD0BS[4];   //[NMus]
   Float_t         MuDzBS[4];   //[NMus]
   Float_t         MuPtE[4];   //[NMus]
   Int_t           MuGenID[4];   //[NMus]
   Int_t           MuGenMoID[4];   //[NMus]
   Int_t           MuGenGMoID[4];   //[NMus]
   Int_t           MuGenType[4];   //[NMus]
   Int_t           MuGenMoType[4];   //[NMus]
   Int_t           MuGenGMoType[4];   //[NMus]
   Float_t         MuMT;
   Float_t         MuMinv;
   Int_t           NEls;
   Int_t           ElCh[4];   //[NEls]
   Int_t           ElChIsCons[4];   //[NEls]
   Int_t           ElChIsGenCons[4];   //[NEls]
   Float_t         ElPt[4];   //[NEls]
   Float_t         ElEta[4];   //[NEls]
   Float_t         ElPhi[4];   //[NEls]
   Float_t         ElD0[4];   //[NEls]
   Float_t         ElD0Err[4];   //[NEls]
   Float_t         ElEoverP[4];   //[NEls]
   Float_t         ElHoverE[4];   //[NEls]
   Float_t         ElSigmaIetaIeta[4];   //[NEls]
   Float_t         ElDeltaPhiSuperClusterAtVtx[4];   //[NEls]
   Float_t         ElDeltaEtaSuperClusterAtVtx[4];   //[NEls]
   Float_t         ElRelIso[4];   //[NEls]
   Float_t         ElDR04TkSumPt[4];   //[NEls]
   Float_t         ElDR04EcalRecHitSumEt[4];   //[NEls]
   Float_t         ElDR04HcalTowerSumEt[4];   //[NEls]
   Float_t         ElS4OverS1[4];   //[NEls]
   Float_t         ElConvPartnerTrkDist[4];   //[NEls]
   Float_t         ElConvPartnerTrkDCot[4];   //[NEls]
   Float_t         ElChargeMisIDProb[4];   //[NEls]
   Float_t         ElDRjet[4];   //[NEls]
   Float_t         ElDRhardestjet[4];   //[NEls]
   Int_t           ElGenID[4];   //[NEls]
   Int_t           ElGenMID[4];   //[NEls]
   Int_t           ElGenGMID[4];   //[NEls]
   Int_t           ElGenType[4];   //[NEls]
   Int_t           ElGenMType[4];   //[NEls]
   Int_t           ElGenGMType[4];   //[NEls]
   Int_t           ElTight[4];   //[NEls]
   Float_t         ElHybRelIso[4];   //[NEls]
   Float_t         ElMT[4];   //[NEls]
   Float_t         ElMInv;
   Float_t         ElMTInv;
   Float_t         HT;
   Float_t         SumEt;
   Float_t         tcMET;
   Float_t         pfMET;
   Float_t         MuCorrMET;
   Int_t           NJets;
   Float_t         JetPt[12];   //[NJets]
   Float_t         JetEta[12];   //[NJets]
   Float_t         JetPhi[12];   //[NJets]
   Float_t         dPhiMJ1;
   Float_t         dPhiMJ2;
   Float_t         R12;
   Float_t         R21;
   Float_t         R12plusR21;
   Int_t           NPhos;
   Float_t         PhoPt[4];   //[NPhos]
   Float_t         PhoEta[4];   //[NPhos]
   Float_t         PhoPhi[4];   //[NPhos]
   Float_t         PhoRelIso[4];   //[NPhos]
   Float_t         PhoDRjet[4];   //[NPhos]
   Float_t         PhoDRhardestjet[4];   //[NPhos]
   Float_t         AlphaT_h;
   Float_t         AlphaCT_h;
   Float_t         AlphaT;
   Float_t         AlphaCT;
   Float_t         AlphaT_new;
   Float_t         AlphaCT_new;
   Float_t         ElMT2_0;
   Float_t         ElMT2_50;
   Float_t         ElMT2_100;
   Float_t         ElMCT;
   Float_t         ElMCTparl;
   Float_t         ElMCTorth;
   Float_t         ElMT2orth_0;
   Float_t         ElMT2orth_50;
   Float_t         ElMT2orth_100;
   Float_t         MuMT2_0;
   Float_t         MuMT2_50;
   Float_t         MuMT2_100;
   Float_t         MuMCT;
   Float_t         MuMCTparl;
   Float_t         MuMCTorth;
   Float_t         MuMT2orth_0;
   Float_t         MuMT2orth_50;
   Float_t         MuMT2orth_100;
   Int_t           isSE_QCDLike;
   Int_t           SE_QCDLike_FakeElGenID;
   Float_t         SE_QCDLike_ElLoosePt;
   Float_t         SE_QCDLike_ElTightPt;
   Float_t         SE_QCDLike_ElLooseEta;
   Float_t         SE_QCDLike_ElTightEta;
   Int_t           isSE_AntiQCDLike;
   Int_t           SE_AntiQCDLike_FakeElGenID;
   Float_t         SE_AntiQCDLike_ElLoosePt;
   Float_t         SE_AntiQCDLike_ElTightPt;
   Float_t         SE_AntiQCDLike_ElLooseEta;
   Float_t         SE_AntiQCDLike_ElTightEta;
   Int_t           isDE_ZJetsLike;
   Float_t         DE_ZJetsLike_ElLoosePt;
   Float_t         DE_ZJetsLike_ElTightPt;
   Float_t         DE_ZJetsLike_ElLooseEta;
   Float_t         DE_ZJetsLike_ElTightEta;
   Float_t         DE_ZJetsLike_PromptElGenLoosePt;
   Float_t         DE_ZJetsLike_PromptElGenTightPt;
   Float_t         DE_ZJetsLike_PromptElGenLooseEta;
   Float_t         DE_ZJetsLike_PromptElGenTightEta;
   Int_t           isDE_WJetsLike;
   Int_t           DE_WJetsLike_FakeElGenID;
   Float_t         DE_WJetsLike_ElLoosePt;
   Float_t         DE_WJetsLike_ElTightPt;
   Float_t         DE_WJetsLike_ElLooseEta;
   Float_t         DE_WJetsLike_ElTightEta;
   Float_t         DE_WJetsLike_ElTightMT;
   Float_t         DE_WJetsLike_FakeElGenLoosePt;
   Float_t         DE_WJetsLike_FakeElGenTightPt;
   Float_t         DE_WJetsLike_FakeElGenLooseEta;
   Float_t         DE_WJetsLike_FakeElGenTightEta;
   Float_t         DE_WJetsLike_PromptElGenLoosePt;
   Float_t         DE_WJetsLike_PromptElGenTightPt;
   Float_t         DE_WJetsLike_PromptElGenLooseEta;
   Float_t         DE_WJetsLike_PromptElGenTightEta;
   Float_t         DE_WJetsLike_PromptElGenMT;
   Int_t           isDE_AntiWJetsLike;
   Int_t           DE_AntiWJetsLike_FakeElGenID;
   Float_t         DE_AntiWJetsLike_ElLoosePt;
   Float_t         DE_AntiWJetsLike_ElTightPt;
   Float_t         DE_AntiWJetsLike_ElLooseEta;
   Float_t         DE_AntiWJetsLike_ElTightEta;
   Float_t         DE_AntiWJetsLike_ElTightMT;
   Float_t         DE_AntiWJetsLike_FakeElGenLoosePt;
   Float_t         DE_AntiWJetsLike_FakeElGenTightPt;
   Float_t         DE_AntiWJetsLike_FakeElGenLooseEta;
   Float_t         DE_AntiWJetsLike_FakeElGenTightEta;
   Float_t         DE_AntiWJetsLike_PromptElGenLoosePt;
   Float_t         DE_AntiWJetsLike_PromptElGenTightPt;
   Float_t         DE_AntiWJetsLike_PromptElGenLooseEta;
   Float_t         DE_AntiWJetsLike_PromptElGenTightEta;
   Float_t         DE_AntiWJetsLike_PromptElGenMT;
   Int_t           isDE_SignalLike;
   Float_t         DE_Ntt_El1Pt;
   Float_t         DE_Ntt_El2Pt;
   Float_t         DE_Ntt_El1Eta;
   Float_t         DE_Ntt_El2Eta;
   Float_t         DE_Ntl_El1Pt;
   Float_t         DE_Ntl_El2Pt;
   Float_t         DE_Ntl_El1Eta;
   Float_t         DE_Ntl_El2Eta;
   Float_t         DE_Nlt_El1Pt;
   Float_t         DE_Nlt_El2Pt;
   Float_t         DE_Nlt_El1Eta;
   Float_t         DE_Nlt_El2Eta;
   Float_t         DE_Nll_El1Pt;
   Float_t         DE_Nll_El2Pt;
   Float_t         DE_Nll_El1Eta;
   Float_t         DE_Nll_El2Eta;

   // List of branches
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_LumiSec;   //!
   TBranch        *b_ExtXSecLO;   //!
   TBranch        *b_IntXSec;   //!
   TBranch        *b_HLT_Mu9;   //!
   TBranch        *b_HLT_Mu11;   //!
   TBranch        *b_HLT_Mu15;   //!
   TBranch        *b_HLT_DoubleMu0;   //!
   TBranch        *b_HLT_DoubleMu3;   //!
   TBranch        *b_HLT_Jet15U;   //!
   TBranch        *b_HLT_Jet30U;   //!
   TBranch        *b_HLT_Jet50U;   //!
   TBranch        *b_HLT_Jet70U;   //!
   TBranch        *b_HLT_Jet100U;   //!
   TBranch        *b_HLT_HT100U;   //!
   TBranch        *b_HLT_HT120U;   //!
   TBranch        *b_HLT_HT140U;   //!
   TBranch        *b_HLT_HT150U;   //!
   TBranch        *b_HLT_Ele10_LW_L1R;   //!
   TBranch        *b_HLT_Ele10_SW_L1R;   //!
   TBranch        *b_HLT_Ele15_LW_L1R;   //!
   TBranch        *b_HLT_Ele15_SW_L1R;   //!
   TBranch        *b_HLT_Ele20_SW_L1R;   //!
   TBranch        *b_HLT_DoubleEle5_SW_L1R;   //!
   TBranch        *b_HLT_DoubleEle10_SW_L1R;   //!
   TBranch        *b_HLT_DoubleEle15_SW_L1R_v1;   //!
   TBranch        *b_HLT_Ele10_LW_EleId_L1R;   //!
   TBranch        *b_HLT_Ele10_SW_EleId_L1R;   //!
   TBranch        *b_HLT_Ele15_SW_CaloEleId_L1R;   //!
   TBranch        *b_HLT_Ele15_SW_EleId_L1R;   //!
   TBranch        *b_HLT_Ele17_SW_LooseEleId_L1R;   //!
   TBranch        *b_HLT_Ele17_SW_TightEleId_L1R;   //!
   TBranch        *b_HLT_Ele17_SW_CaloEleId_L1R;   //!
   TBranch        *b_HLT_Ele17_SW_EleId_L1R;   //!
   TBranch        *b_HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1;   //!
   TBranch        *b_HLT_Ele17_SW_TightEleIdIsol_L1R_v1;   //!
   TBranch        *b_HTL_GoodElEvent;   //!
   TBranch        *b_HTL_GoodElFakesEvent;   //!
   TBranch        *b_HTL_GoodElEvent_RA5;   //!
   TBranch        *b_HTL_GoodElEvent_TDL;   //!
   TBranch        *b_HTL_GoodMuEvent;   //!
   TBranch        *b_HTL_GoodHadronicEvent;   //!
   TBranch        *b_NMus;   //!
   TBranch        *b_MuPt;   //!
   TBranch        *b_MuEta;   //!
   TBranch        *b_MuPhi;   //!
   TBranch        *b_MuCharge;   //!
   TBranch        *b_MuTight;   //!
   TBranch        *b_MuIso;   //!
   TBranch        *b_MuIsoHybrid;   //!
   TBranch        *b_MuDRJet;   //!
   TBranch        *b_MuDRHardestJet;   //!
   TBranch        *b_MuD0;   //!
   TBranch        *b_MuDz;   //!
   TBranch        *b_MuD0BS;   //!
   TBranch        *b_MuDzBS;   //!
   TBranch        *b_MuPtE;   //!
   TBranch        *b_MuGenID;   //!
   TBranch        *b_MuGenMoID;   //!
   TBranch        *b_MuGenGMoID;   //!
   TBranch        *b_MuGenType;   //!
   TBranch        *b_MuGenMoType;   //!
   TBranch        *b_MuGenGMoType;   //!
   TBranch        *b_MuMT;   //!
   TBranch        *b_MuMinv;   //!
   TBranch        *b_NEls;   //!
   TBranch        *b_ElCh;   //!
   TBranch        *b_ElChIsCons;   //!
   TBranch        *b_ElChIsGenCons;   //!
   TBranch        *b_ElPt;   //!
   TBranch        *b_ElEta;   //!
   TBranch        *b_ElPhi;   //!
   TBranch        *b_ElD0;   //!
   TBranch        *b_ElD0Err;   //!
   TBranch        *b_ElEoverP;   //!
   TBranch        *b_ElHoverE;   //!
   TBranch        *b_ElSigmaIetaIeta;   //!
   TBranch        *b_ElDeltaPhiSuperClusterAtVtx;   //!
   TBranch        *b_ElDeltaEtaSuperClusterAtVtx;   //!
   TBranch        *b_ElRelIso;   //!
   TBranch        *b_ElDR04TkSumPt;   //!
   TBranch        *b_ElDR04EcalRecHitSumEt;   //!
   TBranch        *b_ElDR04HcalTowerSumEt;   //!
   TBranch        *b_ElS4OverS1;   //!
   TBranch        *b_ElConvPartnerTrkDist;   //!
   TBranch        *b_ElConvPartnerTrkDCot;   //!
   TBranch        *b_ElChargeMisIDProb;   //!
   TBranch        *b_ElDRjet;   //!
   TBranch        *b_ElDRhardestjet;   //!
   TBranch        *b_ElGenID;   //!
   TBranch        *b_ElGenMID;   //!
   TBranch        *b_ElGenGMID;   //!
   TBranch        *b_ElGenType;   //!
   TBranch        *b_ElGenMType;   //!
   TBranch        *b_ElGenGMType;   //!
   TBranch        *b_ElTight;   //!
   TBranch        *b_ElHybRelIso;   //!
   TBranch        *b_ElMT;   //!
   TBranch        *b_ElMInv;   //!
   TBranch        *b_ElMTInv;   //!
   TBranch        *b_HT;   //!
   TBranch        *b_SumEt;   //!
   TBranch        *b_tcMET;   //!
   TBranch        *b_pfMET;   //!
   TBranch        *b_MuCorrMET;   //!
   TBranch        *b_NJets;   //!
   TBranch        *b_JetPt;   //!
   TBranch        *b_JetEta;   //!
   TBranch        *b_JetPhi;   //!
   TBranch        *b_dPhiMJ1;   //!
   TBranch        *b_dPhiMJ2;   //!
   TBranch        *b_R12;   //!
   TBranch        *b_R21;   //!
   TBranch        *b_R12plusR21;   //!
   TBranch        *b_NPhos;   //!
   TBranch        *b_PhoPt;   //!
   TBranch        *b_PhoEta;   //!
   TBranch        *b_PhoPhi;   //!
   TBranch        *b_PhoRelIso;   //!
   TBranch        *b_PhoDRjet;   //!
   TBranch        *b_PhoDRhardestjet;   //!
   TBranch        *b_AlphaT_h;   //!
   TBranch        *b_AlphaCT_h;   //!
   TBranch        *b_AlphaT;   //!
   TBranch        *b_AlphaCT;   //!
   TBranch        *b_AlphaT_new;   //!
   TBranch        *b_AlphaCT_new;   //!
   TBranch        *b_ElMT2_0;   //!
   TBranch        *b_ElMT2_50;   //!
   TBranch        *b_ElMT2_100;   //!
   TBranch        *b_ElMCT;   //!
   TBranch        *b_ElMCTparl;   //!
   TBranch        *b_ElMCTorth;   //!
   TBranch        *b_ElMT2orth_0;   //!
   TBranch        *b_ElMT2orth_50;   //!
   TBranch        *b_ElMT2orth_100;   //!
   TBranch        *b_MuMT2_0;   //!
   TBranch        *b_MuMT2_50;   //!
   TBranch        *b_MuMT2_100;   //!
   TBranch        *b_MuMCT;   //!
   TBranch        *b_MuMCTparl;   //!
   TBranch        *b_MuMCTorth;   //!
   TBranch        *b_MuMT2orth_0;   //!
   TBranch        *b_MuMT2orth_50;   //!
   TBranch        *b_MuMT2orth_100;   //!
   TBranch        *b_isSE_QCDLike;   //!
   TBranch        *b_SE_QCDLike_FakeElGenID;   //!
   TBranch        *b_SE_QCDLike_ElLoosePt;   //!
   TBranch        *b_SE_QCDLike_ElTightPt;   //!
   TBranch        *b_SE_QCDLike_ElLooseEta;   //!
   TBranch        *b_SE_QCDLike_ElTightEta;   //!
   TBranch        *b_isSE_AntiQCDLike;   //!
   TBranch        *b_SE_AntiQCDLike_FakeElGenID;   //!
   TBranch        *b_SE_AntiQCDLike_ElLoosePt;   //!
   TBranch        *b_SE_AntiQCDLike_ElTightPt;   //!
   TBranch        *b_SE_AntiQCDLike_ElLooseEta;   //!
   TBranch        *b_SE_AntiQCDLike_ElTightEta;   //!
   TBranch        *b_isDE_ZJetsLike;   //!
   TBranch        *b_DE_ZJetsLike_ElLoosePt;   //!
   TBranch        *b_DE_ZJetsLike_ElTightPt;   //!
   TBranch        *b_DE_ZJetsLike_ElLooseEta;   //!
   TBranch        *b_DE_ZJetsLike_ElTightEta;   //!
   TBranch        *b_DE_ZJetsLike_PromptElGenLoosePt;   //!
   TBranch        *b_DE_ZJetsLike_PromptElGenTightPt;   //!
   TBranch        *b_DE_ZJetsLike_PromptElGenLooseEta;   //!
   TBranch        *b_DE_ZJetsLike_PromptElGenTightEta;   //!
   TBranch        *b_isDE_WJetsLike;   //!
   TBranch        *b_DE_WJetsLike_FakeElGenID;   //!
   TBranch        *b_DE_WJetsLike_ElLoosePt;   //!
   TBranch        *b_DE_WJetsLike_ElTightPt;   //!
   TBranch        *b_DE_WJetsLike_ElLooseEta;   //!
   TBranch        *b_DE_WJetsLike_ElTightEta;   //!
   TBranch        *b_DE_WJetsLike_ElTightMT;   //!
   TBranch        *b_DE_WJetsLike_FakeElGenLoosePt;   //!
   TBranch        *b_DE_WJetsLike_FakeElGenTightPt;   //!
   TBranch        *b_DE_WJetsLike_FakeElGenLooseEta;   //!
   TBranch        *b_DE_WJetsLike_FakeElGenTightEta;   //!
   TBranch        *b_DE_WJetsLike_PromptElGenLoosePt;   //!
   TBranch        *b_DE_WJetsLike_PromptElGenTightPt;   //!
   TBranch        *b_DE_WJetsLike_PromptElGenLooseEta;   //!
   TBranch        *b_DE_WJetsLike_PromptElGenTightEta;   //!
   TBranch        *b_DE_WJetsLike_PromptElGenMT;   //!
   TBranch        *b_isDE_AntiWJetsLike;   //!
   TBranch        *b_DE_AntiWJetsLike_FakeElGenID;   //!
   TBranch        *b_DE_AntiWJetsLike_ElLoosePt;   //!
   TBranch        *b_DE_AntiWJetsLike_ElTightPt;   //!
   TBranch        *b_DE_AntiWJetsLike_ElLooseEta;   //!
   TBranch        *b_DE_AntiWJetsLike_ElTightEta;   //!
   TBranch        *b_DE_AntiWJetsLike_ElTightMT;   //!
   TBranch        *b_DE_AntiWJetsLike_FakeElGenLoosePt;   //!
   TBranch        *b_DE_AntiWJetsLike_FakeElGenTightPt;   //!
   TBranch        *b_DE_AntiWJetsLike_FakeElGenLooseEta;   //!
   TBranch        *b_DE_AntiWJetsLike_FakeElGenTightEta;   //!
   TBranch        *b_DE_AntiWJetsLike_PromptElGenLoosePt;   //!
   TBranch        *b_DE_AntiWJetsLike_PromptElGenTightPt;   //!
   TBranch        *b_DE_AntiWJetsLike_PromptElGenLooseEta;   //!
   TBranch        *b_DE_AntiWJetsLike_PromptElGenTightEta;   //!
   TBranch        *b_DE_AntiWJetsLike_PromptElGenMT;   //!
   TBranch        *b_isDE_SignalLike;   //!
   TBranch        *b_DE_Ntt_El1Pt;   //!
   TBranch        *b_DE_Ntt_El2Pt;   //!
   TBranch        *b_DE_Ntt_El1Eta;   //!
   TBranch        *b_DE_Ntt_El2Eta;   //!
   TBranch        *b_DE_Ntl_El1Pt;   //!
   TBranch        *b_DE_Ntl_El2Pt;   //!
   TBranch        *b_DE_Ntl_El1Eta;   //!
   TBranch        *b_DE_Ntl_El2Eta;   //!
   TBranch        *b_DE_Nlt_El1Pt;   //!
   TBranch        *b_DE_Nlt_El2Pt;   //!
   TBranch        *b_DE_Nlt_El1Eta;   //!
   TBranch        *b_DE_Nlt_El2Eta;   //!
   TBranch        *b_DE_Nll_El1Pt;   //!
   TBranch        *b_DE_Nll_El2Pt;   //!
   TBranch        *b_DE_Nll_El1Eta;   //!
   TBranch        *b_DE_Nll_El2Eta;   //!

   MetaTreeClassBase(TTree *tree=0);
   virtual ~MetaTreeClassBase();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MetaTreeClassBase_cxx
MetaTreeClassBase::MetaTreeClassBase(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("SSDLTrees/RA5Nov01/TTbarJets-madgraph/SSDLTree.root");
      if (!f) {
         f = new TFile("SSDLTrees/RA5Nov01/TTbarJets-madgraph/SSDLTree.root");
      }
      tree = (TTree*)gDirectory->Get("Analysis");

   }
   Init(tree);
}

MetaTreeClassBase::~MetaTreeClassBase()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MetaTreeClassBase::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MetaTreeClassBase::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void MetaTreeClassBase::Init(TTree *tree)
{
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
   fChain->SetBranchAddress("ExtXSecLO", &ExtXSecLO, &b_ExtXSecLO);
   fChain->SetBranchAddress("IntXSec", &IntXSec, &b_IntXSec);
   fChain->SetBranchAddress("HLT_Mu9", &HLT_Mu9, &b_HLT_Mu9);
   fChain->SetBranchAddress("HLT_Mu11", &HLT_Mu11, &b_HLT_Mu11);
   fChain->SetBranchAddress("HLT_Mu15", &HLT_Mu15, &b_HLT_Mu15);
   fChain->SetBranchAddress("HLT_DoubleMu0", &HLT_DoubleMu0, &b_HLT_DoubleMu0);
   fChain->SetBranchAddress("HLT_DoubleMu3", &HLT_DoubleMu3, &b_HLT_DoubleMu3);
   fChain->SetBranchAddress("HLT_Jet15U", &HLT_Jet15U, &b_HLT_Jet15U);
   fChain->SetBranchAddress("HLT_Jet30U", &HLT_Jet30U, &b_HLT_Jet30U);
   fChain->SetBranchAddress("HLT_Jet50U", &HLT_Jet50U, &b_HLT_Jet50U);
   fChain->SetBranchAddress("HLT_Jet70U", &HLT_Jet70U, &b_HLT_Jet70U);
   fChain->SetBranchAddress("HLT_Jet100U", &HLT_Jet100U, &b_HLT_Jet100U);
   fChain->SetBranchAddress("HLT_HT100U", &HLT_HT100U, &b_HLT_HT100U);
   fChain->SetBranchAddress("HLT_HT120U", &HLT_HT120U, &b_HLT_HT120U);
   fChain->SetBranchAddress("HLT_HT140U", &HLT_HT140U, &b_HLT_HT140U);
   fChain->SetBranchAddress("HLT_HT150U", &HLT_HT150U, &b_HLT_HT150U);
   fChain->SetBranchAddress("HLT_Ele10_LW_L1R", &HLT_Ele10_LW_L1R, &b_HLT_Ele10_LW_L1R);
   fChain->SetBranchAddress("HLT_Ele10_SW_L1R", &HLT_Ele10_SW_L1R, &b_HLT_Ele10_SW_L1R);
   fChain->SetBranchAddress("HLT_Ele15_LW_L1R", &HLT_Ele15_LW_L1R, &b_HLT_Ele15_LW_L1R);
   fChain->SetBranchAddress("HLT_Ele15_SW_L1R", &HLT_Ele15_SW_L1R, &b_HLT_Ele15_SW_L1R);
   fChain->SetBranchAddress("HLT_Ele20_SW_L1R", &HLT_Ele20_SW_L1R, &b_HLT_Ele20_SW_L1R);
   fChain->SetBranchAddress("HLT_DoubleEle5_SW_L1R", &HLT_DoubleEle5_SW_L1R, &b_HLT_DoubleEle5_SW_L1R);
   fChain->SetBranchAddress("HLT_DoubleEle10_SW_L1R", &HLT_DoubleEle10_SW_L1R, &b_HLT_DoubleEle10_SW_L1R);
   fChain->SetBranchAddress("HLT_DoubleEle15_SW_L1R_v1", &HLT_DoubleEle15_SW_L1R_v1, &b_HLT_DoubleEle15_SW_L1R_v1);
   fChain->SetBranchAddress("HLT_Ele10_LW_EleId_L1R", &HLT_Ele10_LW_EleId_L1R, &b_HLT_Ele10_LW_EleId_L1R);
   fChain->SetBranchAddress("HLT_Ele10_SW_EleId_L1R", &HLT_Ele10_SW_EleId_L1R, &b_HLT_Ele10_SW_EleId_L1R);
   fChain->SetBranchAddress("HLT_Ele15_SW_CaloEleId_L1R", &HLT_Ele15_SW_CaloEleId_L1R, &b_HLT_Ele15_SW_CaloEleId_L1R);
   fChain->SetBranchAddress("HLT_Ele15_SW_EleId_L1R", &HLT_Ele15_SW_EleId_L1R, &b_HLT_Ele15_SW_EleId_L1R);
   fChain->SetBranchAddress("HLT_Ele17_SW_LooseEleId_L1R", &HLT_Ele17_SW_LooseEleId_L1R, &b_HLT_Ele17_SW_LooseEleId_L1R);
   fChain->SetBranchAddress("HLT_Ele17_SW_TightEleId_L1R", &HLT_Ele17_SW_TightEleId_L1R, &b_HLT_Ele17_SW_TightEleId_L1R);
   fChain->SetBranchAddress("HLT_Ele17_SW_CaloEleId_L1R", &HLT_Ele17_SW_CaloEleId_L1R, &b_HLT_Ele17_SW_CaloEleId_L1R);
   fChain->SetBranchAddress("HLT_Ele17_SW_EleId_L1R", &HLT_Ele17_SW_EleId_L1R, &b_HLT_Ele17_SW_EleId_L1R);
   fChain->SetBranchAddress("HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1", &HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1, &b_HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1);
   fChain->SetBranchAddress("HLT_Ele17_SW_TightEleIdIsol_L1R_v1", &HLT_Ele17_SW_TightEleIdIsol_L1R_v1, &b_HLT_Ele17_SW_TightEleIdIsol_L1R_v1);
   fChain->SetBranchAddress("HTL_GoodElEvent", &HTL_GoodElEvent, &b_HTL_GoodElEvent);
   fChain->SetBranchAddress("HTL_GoodElFakesEvent", &HTL_GoodElFakesEvent, &b_HTL_GoodElFakesEvent);
   fChain->SetBranchAddress("HTL_GoodElEvent_RA5", &HTL_GoodElEvent_RA5, &b_HTL_GoodElEvent_RA5);
   fChain->SetBranchAddress("HTL_GoodElEvent_TDL", &HTL_GoodElEvent_TDL, &b_HTL_GoodElEvent_TDL);
   fChain->SetBranchAddress("HTL_GoodMuEvent", &HTL_GoodMuEvent, &b_HTL_GoodMuEvent);
   fChain->SetBranchAddress("HTL_GoodHadronicEvent", &HTL_GoodHadronicEvent, &b_HTL_GoodHadronicEvent);
   fChain->SetBranchAddress("NMus", &NMus, &b_NMus);
   fChain->SetBranchAddress("MuPt", MuPt, &b_MuPt);
   fChain->SetBranchAddress("MuEta", MuEta, &b_MuEta);
   fChain->SetBranchAddress("MuPhi", MuPhi, &b_MuPhi);
   fChain->SetBranchAddress("MuCharge", MuCharge, &b_MuCharge);
   fChain->SetBranchAddress("MuTight", MuTight, &b_MuTight);
   fChain->SetBranchAddress("MuIso", MuIso, &b_MuIso);
   fChain->SetBranchAddress("MuIsoHybrid", MuIsoHybrid, &b_MuIsoHybrid);
   fChain->SetBranchAddress("MuDRJet", MuDRJet, &b_MuDRJet);
   fChain->SetBranchAddress("MuDRHardestJet", MuDRHardestJet, &b_MuDRHardestJet);
   fChain->SetBranchAddress("MuD0", MuD0, &b_MuD0);
   fChain->SetBranchAddress("MuDz", MuDz, &b_MuDz);
   fChain->SetBranchAddress("MuD0BS", MuD0BS, &b_MuD0BS);
   fChain->SetBranchAddress("MuDzBS", MuDzBS, &b_MuDzBS);
   fChain->SetBranchAddress("MuPtE", MuPtE, &b_MuPtE);
   fChain->SetBranchAddress("MuGenID", MuGenID, &b_MuGenID);
   fChain->SetBranchAddress("MuGenMoID", MuGenMoID, &b_MuGenMoID);
   fChain->SetBranchAddress("MuGenGMoID", MuGenGMoID, &b_MuGenGMoID);
   fChain->SetBranchAddress("MuGenType", MuGenType, &b_MuGenType);
   fChain->SetBranchAddress("MuGenMoType", MuGenMoType, &b_MuGenMoType);
   fChain->SetBranchAddress("MuGenGMoType", MuGenGMoType, &b_MuGenGMoType);
   fChain->SetBranchAddress("MuMT", &MuMT, &b_MuMT);
   fChain->SetBranchAddress("MuMinv", &MuMinv, &b_MuMinv);
   fChain->SetBranchAddress("NEls", &NEls, &b_NEls);
   fChain->SetBranchAddress("ElCh", ElCh, &b_ElCh);
   fChain->SetBranchAddress("ElChIsCons", ElChIsCons, &b_ElChIsCons);
   fChain->SetBranchAddress("ElChIsGenCons", ElChIsGenCons, &b_ElChIsGenCons);
   fChain->SetBranchAddress("ElPt", ElPt, &b_ElPt);
   fChain->SetBranchAddress("ElEta", ElEta, &b_ElEta);
   fChain->SetBranchAddress("ElPhi", ElPhi, &b_ElPhi);
   fChain->SetBranchAddress("ElD0", ElD0, &b_ElD0);
   fChain->SetBranchAddress("ElD0Err", ElD0Err, &b_ElD0Err);
   fChain->SetBranchAddress("ElEoverP", ElEoverP, &b_ElEoverP);
   fChain->SetBranchAddress("ElHoverE", ElHoverE, &b_ElHoverE);
   fChain->SetBranchAddress("ElSigmaIetaIeta", ElSigmaIetaIeta, &b_ElSigmaIetaIeta);
   fChain->SetBranchAddress("ElDeltaPhiSuperClusterAtVtx", ElDeltaPhiSuperClusterAtVtx, &b_ElDeltaPhiSuperClusterAtVtx);
   fChain->SetBranchAddress("ElDeltaEtaSuperClusterAtVtx", ElDeltaEtaSuperClusterAtVtx, &b_ElDeltaEtaSuperClusterAtVtx);
   fChain->SetBranchAddress("ElRelIso", ElRelIso, &b_ElRelIso);
   fChain->SetBranchAddress("ElDR04TkSumPt", ElDR04TkSumPt, &b_ElDR04TkSumPt);
   fChain->SetBranchAddress("ElDR04EcalRecHitSumEt", ElDR04EcalRecHitSumEt, &b_ElDR04EcalRecHitSumEt);
   fChain->SetBranchAddress("ElDR04HcalTowerSumEt", ElDR04HcalTowerSumEt, &b_ElDR04HcalTowerSumEt);
   fChain->SetBranchAddress("ElS4OverS1", ElS4OverS1, &b_ElS4OverS1);
   fChain->SetBranchAddress("ElConvPartnerTrkDist", ElConvPartnerTrkDist, &b_ElConvPartnerTrkDist);
   fChain->SetBranchAddress("ElConvPartnerTrkDCot", ElConvPartnerTrkDCot, &b_ElConvPartnerTrkDCot);
   fChain->SetBranchAddress("ElChargeMisIDProb", ElChargeMisIDProb, &b_ElChargeMisIDProb);
   fChain->SetBranchAddress("ElDRjet", ElDRjet, &b_ElDRjet);
   fChain->SetBranchAddress("ElDRhardestjet", ElDRhardestjet, &b_ElDRhardestjet);
   fChain->SetBranchAddress("ElGenID", ElGenID, &b_ElGenID);
   fChain->SetBranchAddress("ElGenMID", ElGenMID, &b_ElGenMID);
   fChain->SetBranchAddress("ElGenGMID", ElGenGMID, &b_ElGenGMID);
   fChain->SetBranchAddress("ElGenType", ElGenType, &b_ElGenType);
   fChain->SetBranchAddress("ElGenMType", ElGenMType, &b_ElGenMType);
   fChain->SetBranchAddress("ElGenGMType", ElGenGMType, &b_ElGenGMType);
   fChain->SetBranchAddress("ElTight", ElTight, &b_ElTight);
   fChain->SetBranchAddress("ElHybRelIso", ElHybRelIso, &b_ElHybRelIso);
   fChain->SetBranchAddress("ElMT", ElMT, &b_ElMT);
   fChain->SetBranchAddress("ElMInv", &ElMInv, &b_ElMInv);
   fChain->SetBranchAddress("ElMTInv", &ElMTInv, &b_ElMTInv);
   fChain->SetBranchAddress("HT", &HT, &b_HT);
   fChain->SetBranchAddress("SumEt", &SumEt, &b_SumEt);
   fChain->SetBranchAddress("tcMET", &tcMET, &b_tcMET);
   fChain->SetBranchAddress("pfMET", &pfMET, &b_pfMET);
   fChain->SetBranchAddress("MuCorrMET", &MuCorrMET, &b_MuCorrMET);
   fChain->SetBranchAddress("NJets", &NJets, &b_NJets);
   fChain->SetBranchAddress("JetPt", JetPt, &b_JetPt);
   fChain->SetBranchAddress("JetEta", JetEta, &b_JetEta);
   fChain->SetBranchAddress("JetPhi", JetPhi, &b_JetPhi);
   fChain->SetBranchAddress("dPhiMJ1", &dPhiMJ1, &b_dPhiMJ1);
   fChain->SetBranchAddress("dPhiMJ2", &dPhiMJ2, &b_dPhiMJ2);
   fChain->SetBranchAddress("R12", &R12, &b_R12);
   fChain->SetBranchAddress("R21", &R21, &b_R21);
   fChain->SetBranchAddress("R12plusR21", &R12plusR21, &b_R12plusR21);
   fChain->SetBranchAddress("NPhos", &NPhos, &b_NPhos);
   fChain->SetBranchAddress("PhoPt", PhoPt, &b_PhoPt);
   fChain->SetBranchAddress("PhoEta", PhoEta, &b_PhoEta);
   fChain->SetBranchAddress("PhoPhi", PhoPhi, &b_PhoPhi);
   fChain->SetBranchAddress("PhoRelIso", PhoRelIso, &b_PhoRelIso);
   fChain->SetBranchAddress("PhoDRjet", PhoDRjet, &b_PhoDRjet);
   fChain->SetBranchAddress("PhoDRhardestjet", PhoDRhardestjet, &b_PhoDRhardestjet);
   fChain->SetBranchAddress("AlphaT_h", &AlphaT_h, &b_AlphaT_h);
   fChain->SetBranchAddress("AlphaCT_h", &AlphaCT_h, &b_AlphaCT_h);
   fChain->SetBranchAddress("AlphaT", &AlphaT, &b_AlphaT);
   fChain->SetBranchAddress("AlphaCT", &AlphaCT, &b_AlphaCT);
   fChain->SetBranchAddress("AlphaT_new", &AlphaT_new, &b_AlphaT_new);
   fChain->SetBranchAddress("AlphaCT_new", &AlphaCT_new, &b_AlphaCT_new);
   fChain->SetBranchAddress("ElMT2_0", &ElMT2_0, &b_ElMT2_0);
   fChain->SetBranchAddress("ElMT2_50", &ElMT2_50, &b_ElMT2_50);
   fChain->SetBranchAddress("ElMT2_100", &ElMT2_100, &b_ElMT2_100);
   fChain->SetBranchAddress("ElMCT", &ElMCT, &b_ElMCT);
   fChain->SetBranchAddress("ElMCTparl", &ElMCTparl, &b_ElMCTparl);
   fChain->SetBranchAddress("ElMCTorth", &ElMCTorth, &b_ElMCTorth);
   fChain->SetBranchAddress("ElMT2orth_0", &ElMT2orth_0, &b_ElMT2orth_0);
   fChain->SetBranchAddress("ElMT2orth_50", &ElMT2orth_50, &b_ElMT2orth_50);
   fChain->SetBranchAddress("ElMT2orth_100", &ElMT2orth_100, &b_ElMT2orth_100);
   fChain->SetBranchAddress("MuMT2_0", &MuMT2_0, &b_MuMT2_0);
   fChain->SetBranchAddress("MuMT2_50", &MuMT2_50, &b_MuMT2_50);
   fChain->SetBranchAddress("MuMT2_100", &MuMT2_100, &b_MuMT2_100);
   fChain->SetBranchAddress("MuMCT", &MuMCT, &b_MuMCT);
   fChain->SetBranchAddress("MuMCTparl", &MuMCTparl, &b_MuMCTparl);
   fChain->SetBranchAddress("MuMCTorth", &MuMCTorth, &b_MuMCTorth);
   fChain->SetBranchAddress("MuMT2orth_0", &MuMT2orth_0, &b_MuMT2orth_0);
   fChain->SetBranchAddress("MuMT2orth_50", &MuMT2orth_50, &b_MuMT2orth_50);
   fChain->SetBranchAddress("MuMT2orth_100", &MuMT2orth_100, &b_MuMT2orth_100);
   fChain->SetBranchAddress("isSE_QCDLike", &isSE_QCDLike, &b_isSE_QCDLike);
   fChain->SetBranchAddress("SE_QCDLike_FakeElGenID", &SE_QCDLike_FakeElGenID, &b_SE_QCDLike_FakeElGenID);
   fChain->SetBranchAddress("SE_QCDLike_ElLoosePt", &SE_QCDLike_ElLoosePt, &b_SE_QCDLike_ElLoosePt);
   fChain->SetBranchAddress("SE_QCDLike_ElTightPt", &SE_QCDLike_ElTightPt, &b_SE_QCDLike_ElTightPt);
   fChain->SetBranchAddress("SE_QCDLike_ElLooseEta", &SE_QCDLike_ElLooseEta, &b_SE_QCDLike_ElLooseEta);
   fChain->SetBranchAddress("SE_QCDLike_ElTightEta", &SE_QCDLike_ElTightEta, &b_SE_QCDLike_ElTightEta);
   fChain->SetBranchAddress("isSE_AntiQCDLike", &isSE_AntiQCDLike, &b_isSE_AntiQCDLike);
   fChain->SetBranchAddress("SE_AntiQCDLike_FakeElGenID", &SE_AntiQCDLike_FakeElGenID, &b_SE_AntiQCDLike_FakeElGenID);
   fChain->SetBranchAddress("SE_AntiQCDLike_ElLoosePt", &SE_AntiQCDLike_ElLoosePt, &b_SE_AntiQCDLike_ElLoosePt);
   fChain->SetBranchAddress("SE_AntiQCDLike_ElTightPt", &SE_AntiQCDLike_ElTightPt, &b_SE_AntiQCDLike_ElTightPt);
   fChain->SetBranchAddress("SE_AntiQCDLike_ElLooseEta", &SE_AntiQCDLike_ElLooseEta, &b_SE_AntiQCDLike_ElLooseEta);
   fChain->SetBranchAddress("SE_AntiQCDLike_ElTightEta", &SE_AntiQCDLike_ElTightEta, &b_SE_AntiQCDLike_ElTightEta);
   fChain->SetBranchAddress("isDE_ZJetsLike", &isDE_ZJetsLike, &b_isDE_ZJetsLike);
   fChain->SetBranchAddress("DE_ZJetsLike_ElLoosePt", &DE_ZJetsLike_ElLoosePt, &b_DE_ZJetsLike_ElLoosePt);
   fChain->SetBranchAddress("DE_ZJetsLike_ElTightPt", &DE_ZJetsLike_ElTightPt, &b_DE_ZJetsLike_ElTightPt);
   fChain->SetBranchAddress("DE_ZJetsLike_ElLooseEta", &DE_ZJetsLike_ElLooseEta, &b_DE_ZJetsLike_ElLooseEta);
   fChain->SetBranchAddress("DE_ZJetsLike_ElTightEta", &DE_ZJetsLike_ElTightEta, &b_DE_ZJetsLike_ElTightEta);
   fChain->SetBranchAddress("DE_ZJetsLike_PromptElGenLoosePt", &DE_ZJetsLike_PromptElGenLoosePt, &b_DE_ZJetsLike_PromptElGenLoosePt);
   fChain->SetBranchAddress("DE_ZJetsLike_PromptElGenTightPt", &DE_ZJetsLike_PromptElGenTightPt, &b_DE_ZJetsLike_PromptElGenTightPt);
   fChain->SetBranchAddress("DE_ZJetsLike_PromptElGenLooseEta", &DE_ZJetsLike_PromptElGenLooseEta, &b_DE_ZJetsLike_PromptElGenLooseEta);
   fChain->SetBranchAddress("DE_ZJetsLike_PromptElGenTightEta", &DE_ZJetsLike_PromptElGenTightEta, &b_DE_ZJetsLike_PromptElGenTightEta);
   fChain->SetBranchAddress("isDE_WJetsLike", &isDE_WJetsLike, &b_isDE_WJetsLike);
   fChain->SetBranchAddress("DE_WJetsLike_FakeElGenID", &DE_WJetsLike_FakeElGenID, &b_DE_WJetsLike_FakeElGenID);
   fChain->SetBranchAddress("DE_WJetsLike_ElLoosePt", &DE_WJetsLike_ElLoosePt, &b_DE_WJetsLike_ElLoosePt);
   fChain->SetBranchAddress("DE_WJetsLike_ElTightPt", &DE_WJetsLike_ElTightPt, &b_DE_WJetsLike_ElTightPt);
   fChain->SetBranchAddress("DE_WJetsLike_ElLooseEta", &DE_WJetsLike_ElLooseEta, &b_DE_WJetsLike_ElLooseEta);
   fChain->SetBranchAddress("DE_WJetsLike_ElTightEta", &DE_WJetsLike_ElTightEta, &b_DE_WJetsLike_ElTightEta);
   fChain->SetBranchAddress("DE_WJetsLike_ElTightMT", &DE_WJetsLike_ElTightMT, &b_DE_WJetsLike_ElTightMT);
   fChain->SetBranchAddress("DE_WJetsLike_FakeElGenLoosePt", &DE_WJetsLike_FakeElGenLoosePt, &b_DE_WJetsLike_FakeElGenLoosePt);
   fChain->SetBranchAddress("DE_WJetsLike_FakeElGenTightPt", &DE_WJetsLike_FakeElGenTightPt, &b_DE_WJetsLike_FakeElGenTightPt);
   fChain->SetBranchAddress("DE_WJetsLike_FakeElGenLooseEta", &DE_WJetsLike_FakeElGenLooseEta, &b_DE_WJetsLike_FakeElGenLooseEta);
   fChain->SetBranchAddress("DE_WJetsLike_FakeElGenTightEta", &DE_WJetsLike_FakeElGenTightEta, &b_DE_WJetsLike_FakeElGenTightEta);
   fChain->SetBranchAddress("DE_WJetsLike_PromptElGenLoosePt", &DE_WJetsLike_PromptElGenLoosePt, &b_DE_WJetsLike_PromptElGenLoosePt);
   fChain->SetBranchAddress("DE_WJetsLike_PromptElGenTightPt", &DE_WJetsLike_PromptElGenTightPt, &b_DE_WJetsLike_PromptElGenTightPt);
   fChain->SetBranchAddress("DE_WJetsLike_PromptElGenLooseEta", &DE_WJetsLike_PromptElGenLooseEta, &b_DE_WJetsLike_PromptElGenLooseEta);
   fChain->SetBranchAddress("DE_WJetsLike_PromptElGenTightEta", &DE_WJetsLike_PromptElGenTightEta, &b_DE_WJetsLike_PromptElGenTightEta);
   fChain->SetBranchAddress("DE_WJetsLike_PromptElGenMT", &DE_WJetsLike_PromptElGenMT, &b_DE_WJetsLike_PromptElGenMT);
   fChain->SetBranchAddress("isDE_AntiWJetsLike", &isDE_AntiWJetsLike, &b_isDE_AntiWJetsLike);
   fChain->SetBranchAddress("DE_AntiWJetsLike_FakeElGenID", &DE_AntiWJetsLike_FakeElGenID, &b_DE_AntiWJetsLike_FakeElGenID);
   fChain->SetBranchAddress("DE_AntiWJetsLike_ElLoosePt", &DE_AntiWJetsLike_ElLoosePt, &b_DE_AntiWJetsLike_ElLoosePt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_ElTightPt", &DE_AntiWJetsLike_ElTightPt, &b_DE_AntiWJetsLike_ElTightPt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_ElLooseEta", &DE_AntiWJetsLike_ElLooseEta, &b_DE_AntiWJetsLike_ElLooseEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_ElTightEta", &DE_AntiWJetsLike_ElTightEta, &b_DE_AntiWJetsLike_ElTightEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_ElTightMT", &DE_AntiWJetsLike_ElTightMT, &b_DE_AntiWJetsLike_ElTightMT);
   fChain->SetBranchAddress("DE_AntiWJetsLike_FakeElGenLoosePt", &DE_AntiWJetsLike_FakeElGenLoosePt, &b_DE_AntiWJetsLike_FakeElGenLoosePt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_FakeElGenTightPt", &DE_AntiWJetsLike_FakeElGenTightPt, &b_DE_AntiWJetsLike_FakeElGenTightPt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_FakeElGenLooseEta", &DE_AntiWJetsLike_FakeElGenLooseEta, &b_DE_AntiWJetsLike_FakeElGenLooseEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_FakeElGenTightEta", &DE_AntiWJetsLike_FakeElGenTightEta, &b_DE_AntiWJetsLike_FakeElGenTightEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_PromptElGenLoosePt", &DE_AntiWJetsLike_PromptElGenLoosePt, &b_DE_AntiWJetsLike_PromptElGenLoosePt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_PromptElGenTightPt", &DE_AntiWJetsLike_PromptElGenTightPt, &b_DE_AntiWJetsLike_PromptElGenTightPt);
   fChain->SetBranchAddress("DE_AntiWJetsLike_PromptElGenLooseEta", &DE_AntiWJetsLike_PromptElGenLooseEta, &b_DE_AntiWJetsLike_PromptElGenLooseEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_PromptElGenTightEta", &DE_AntiWJetsLike_PromptElGenTightEta, &b_DE_AntiWJetsLike_PromptElGenTightEta);
   fChain->SetBranchAddress("DE_AntiWJetsLike_PromptElGenMT", &DE_AntiWJetsLike_PromptElGenMT, &b_DE_AntiWJetsLike_PromptElGenMT);
   fChain->SetBranchAddress("isDE_SignalLike", &isDE_SignalLike, &b_isDE_SignalLike);
   fChain->SetBranchAddress("DE_Ntt_El1Pt", &DE_Ntt_El1Pt, &b_DE_Ntt_El1Pt);
   fChain->SetBranchAddress("DE_Ntt_El2Pt", &DE_Ntt_El2Pt, &b_DE_Ntt_El2Pt);
   fChain->SetBranchAddress("DE_Ntt_El1Eta", &DE_Ntt_El1Eta, &b_DE_Ntt_El1Eta);
   fChain->SetBranchAddress("DE_Ntt_El2Eta", &DE_Ntt_El2Eta, &b_DE_Ntt_El2Eta);
   fChain->SetBranchAddress("DE_Ntl_El1Pt", &DE_Ntl_El1Pt, &b_DE_Ntl_El1Pt);
   fChain->SetBranchAddress("DE_Ntl_El2Pt", &DE_Ntl_El2Pt, &b_DE_Ntl_El2Pt);
   fChain->SetBranchAddress("DE_Ntl_El1Eta", &DE_Ntl_El1Eta, &b_DE_Ntl_El1Eta);
   fChain->SetBranchAddress("DE_Ntl_El2Eta", &DE_Ntl_El2Eta, &b_DE_Ntl_El2Eta);
   fChain->SetBranchAddress("DE_Nlt_El1Pt", &DE_Nlt_El1Pt, &b_DE_Nlt_El1Pt);
   fChain->SetBranchAddress("DE_Nlt_El2Pt", &DE_Nlt_El2Pt, &b_DE_Nlt_El2Pt);
   fChain->SetBranchAddress("DE_Nlt_El1Eta", &DE_Nlt_El1Eta, &b_DE_Nlt_El1Eta);
   fChain->SetBranchAddress("DE_Nlt_El2Eta", &DE_Nlt_El2Eta, &b_DE_Nlt_El2Eta);
   fChain->SetBranchAddress("DE_Nll_El1Pt", &DE_Nll_El1Pt, &b_DE_Nll_El1Pt);
   fChain->SetBranchAddress("DE_Nll_El2Pt", &DE_Nll_El2Pt, &b_DE_Nll_El2Pt);
   fChain->SetBranchAddress("DE_Nll_El1Eta", &DE_Nll_El1Eta, &b_DE_Nll_El1Eta);
   fChain->SetBranchAddress("DE_Nll_El2Eta", &DE_Nll_El2Eta, &b_DE_Nll_El2Eta);
   Notify();
}

Bool_t MetaTreeClassBase::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MetaTreeClassBase::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MetaTreeClassBase::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MetaTreeClassBase_cxx