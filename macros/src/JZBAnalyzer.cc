#include "JZBAnalyzer.hh"
#include "JZBAnalysis.hh"

#include "base/TreeAnalyzerBase.hh"
#include "base/TreeReader.hh"

using namespace std;

JZBAnalyzer::JZBAnalyzer(std::vector<std::string>& fileList, std::string dataType, bool fullCleaning, bool isModelScan, bool makeSmall, bool doGenInfo)  
   : TreeAnalyzerBase(fileList) {
  f_isModelScan=isModelScan;
  f_doGenInfo=doGenInfo;
  if(whichanalysis!=2)   fJZBAnalysis = new JZBAnalysis(fTR,dataType,fullCleaning,isModelScan,makeSmall,doGenInfo);
//  if(whichanalysis!=1)   fJZBPFAnalysis = new JZBPFAnalysis(fTR,dataType,fullCleaning,isModelScan,makeSmall);
}

JZBAnalyzer::~JZBAnalyzer() {
  if(whichanalysis!=2) 	delete fJZBAnalysis;
//  if(whichanalysis!=1) 	delete fJZBPFAnalysis;
}

// Method for looping over the tree
void JZBAnalyzer::Loop(){
  Long64_t nentries = fTR->GetEntries();
  cout << " total events in ntuples = " << fTR->GetEntries() << endl;
  
  // loop over all ntuple entries
  if(fMaxEvents==-1)nentries=fTR->GetEntries();
  if(fMaxEvents>0)nentries=fMaxEvents;
  
  Long64_t jentry=0;
  for ( fTR->ToBegin();!(fTR->AtEnd()) && (jentry<fMaxEvents || fMaxEvents<0);++(*fTR) ) {
    PrintProgress(jentry++);
    if ( fCurRun != fTR->Run ) {
      fCurRun = fTR->Run;
      if(whichanalysis!=2) fJZBAnalysis->BeginRun(fCurRun);
//      if(whichanalysis!=1) fJZBPFAnalysis->BeginRun(fCurRun);
      skipRun = false;
      if ( !CheckRun() ) skipRun = true;
    }
    // Check if new lumi is in JSON file
    if ( !skipRun && fCurLumi != fTR->LumiSection ) {
      fCurLumi = fTR->LumiSection;
      skipLumi = false; // Re-initialise
      if ( !CheckRunLumi() ) skipLumi = true;
    }
    if ( !(skipRun || skipLumi) ) {
      if(whichanalysis!=2) fJZBAnalysis->Analyze();
//      if(whichanalysis!=1) fJZBPFAnalysis->Analyze();
    }
  }
}

TFile *fHistFile;

// Method called before starting the event loop
void JZBAnalyzer::BeginJob(string fdata_PileUp, string fmc_PileUp){
  fHistFile = new TFile(outputFileName_.c_str(), "RECREATE");
  //	Note: the next two lines are commented out because we are now saving in the analysis routine anymore but at the "analyzer level"
  //	fJZBAnalysis->outputFileName_ = outputFileName_;
  //	fJZBPFAnalysis->outputFileName_ = outputFileName_;
  if(whichanalysis!=2) fJZBAnalysis->SetVerbose(fVerbose);
//  if(whichanalysis!=1) fJZBPFAnalysis->SetVerbose(fVerbose);
  //if(whichanalysis!=2) fJZBAnalysis->SetPileUpSrc(fdata_PileUp, fmc_PileUp);
  if(whichanalysis!=2) fJZBAnalysis->SetPileUp3DSrc(fdata_PileUp, fmc_PileUp);
//  if(whichanalysis!=1) fJZBPFAnalysis->SetPileUpSrc(fdata_PileUp, fmc_PileUp);
  if(whichanalysis!=2) fJZBAnalysis->Begin(fHistFile);
//  if(whichanalysis!=1) fJZBPFAnalysis->Begin(fHistFile);
}

// Method called after finishing the event loop
void JZBAnalyzer::EndJob(){
  if(whichanalysis!=2) fJZBAnalysis->End(fHistFile);
//  if(whichanalysis!=1) fJZBPFAnalysis->End(fHistFile);
  fHistFile->Close();
}
