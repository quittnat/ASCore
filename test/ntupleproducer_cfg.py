import FWCore.ParameterSet.Config as cms
import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("NTupleProducer")

### Message Logger #############################################################
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.categories.append('NTP')
process.MessageLogger.cerr.NTP = cms.untracked.PSet(
    limit = cms.untracked.int32(-1),
    reportEvery = cms.untracked.int32(1)
    )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

### Parsing of command line parameters #############################################
### (type of run: data, MC; reconstruction: RECO, PAT, PF) #####################
options = VarParsing.VarParsing ('standard')					# set 'standard'  options
options.register ('runon',										# register 'runon' option
                  'data',										# the default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,         # string, int, or float
                  "Type of sample to run on: data (default), MC35X, MC31X")
options.register ('recoType',									# register 'recoType' option
                  'RECO',										# the default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,         # string, int, or float
                  "Type of reconstruction to use: RECO (default), PAT, PF")
# get and parse the command line arguments
options.parseArguments()

# set NTupleProducer defaults (override the output, files and maxEvents parameter)
options.files= '/store/data/Commissioning10/MinimumBias/RAW-RECO/v9/000/135/735/FAB17A5D-4465-DF11-8DBF-00E08178C031.root'
options.maxEvents = -1 # If it is different from -1, string "_numEventXX" will be added to the output file name
options.output='NTupleProducer_36X_'+options.runon+'_'+options.recoType+'.root'

### Running conditions #########################################################
# See https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions
# to check what cff to use
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
if options.runon=='data':
    # CMSSW_3_6_X:
    process.GlobalTag.globaltag = "GR_R_36X_V11::All"
else:
    # CMSSW_3_6_X:
    process.GlobalTag.globaltag = "START36_V9::All"

### b-tagging ##################################################################
# Simple SV (+IP) based algos
process.load("RecoBTag.Configuration.RecoBTag_cff")
process.load("RecoBTag.SecondaryVertex.simpleSecondaryVertex3TrkES_cfi")
process.load("RecoBTag.SecondaryVertex.simpleSecondaryVertexHighPurBJetTags_cfi")
process.load("RecoBTag.SecondaryVertex.simpleSecondaryVertexHighEffBJetTags_cfi")

### Input/Output ###############################################################
# Input
process.source = cms.Source("PoolSource",
      fileNames = cms.untracked.vstring(options.files)
#Enable if you see duplicate error      duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
# Output
process.TFileService = cms.Service("TFileService",
# keep track of the type of data source and reco type in the ntuple file name
	fileName = cms.string(options.output),  
	closeFileFast = cms.untracked.bool(True)
)

#### Parameterisation for Jet Corrections and JES ME Corrections ###############
recoJet_src = "ak5CaloJets"
genJet_src = "ak5GenJets"

# Jet ID: add the ones you want
process.load('RecoJets.Configuration.JetIDProducers_cff')
process.recoJetIdSequence = cms.Sequence( process.ak5JetID )

#### Jet Corrections ###########################################################
# See https://twiki.cern.ch/twiki/bin/view/CMS/WorkBookJetEnergyCorrections
# to check what cff to use
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.jecCorSequence = cms.Sequence(
    process.ak5CaloJetsL2L3*process.ak5PFJetsL2L3
    )
### NB: also check the analysis input below.

### JES MET Corrections ########################################################
from JetMETCorrections.Type1MET.MetType1Corrections_cff import metJESCorAK5CaloJet

process.metMuonJESCorAK5 = metJESCorAK5CaloJet.clone()
process.metMuonJESCorAK5.inputUncorJetsLabel = recoJet_src
#[FR: already default] process.metMuonJESCorAK5.corrector = "ak5CaloL2L3"
process.metMuonJESCorAK5.inputUncorMetLabel = "corMetGlobalMuons"
process.metCorSequence = cms.Sequence(process.metMuonJESCorAK5)

### Cleaning ###################################################################
# flag HB/HE noise
process.load('CommonTools/RecoAlgos/HBHENoiseFilterResultProducer_cfi')
process.cleaning = cms.Sequence(process.HBHENoiseFilterResultProducer)

# See for example DPGAnalysis/Skims/python/MinBiasPDSkim_cfg.py
if options.runon=='data':
    # require physics declared
    process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
    process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'

    # require scraping filter
    process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                        applyfilter = cms.untracked.bool(True),
                                        debugOn = cms.untracked.bool(False),
                                        numtrack = cms.untracked.uint32(10),
                                        thresh = cms.untracked.double(0.2)
                                        )

    # configure HLT
    process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
    process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
    process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
    process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')

    # require good primary vertex
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                               vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                               minimumNDOF = cms.uint32(4) ,
                                               maxAbsZ = cms.double(15),
                                               maxd0 = cms.double(2)
                                               )
    # Cleaning path
    process.cleaning *= cms.Sequence(
        process.hltLevel1GTSeed*
        process.scrapingVeto*
        process.hltPhysicsDeclared*
        process.primaryVertexFilter
        )

    
### Analysis configuration #####################################################
process.load("DiLeptonAnalysis.NTupleProducer.ntupleproducer_cfi")
process.analyze.isRealData = cms.untracked.bool(options.runon=='data')
# Synchronise with Jet configuration above (defaults to ak5)
process.analyze.tag_jets  = recoJet_src
process.analyze.jetCorrs  = 'ak5CaloL2L3'
process.analyze.tag_jetID = 'ak5JetID'
# Add some jet collections
process.analyze.jets = (
    # PF jets
    cms.PSet( prefix = cms.untracked.string('PF'),
              tag = cms.untracked.InputTag('ak5PFJets'),
              sel_minpt  = process.analyze.sel_mincorjpt,
              sel_maxeta = process.analyze.sel_maxjeta,
              corrections = cms.string('ak5PFL2L3'),
              ),
    # Calo jets (for cross-check)
    cms.PSet( prefix = cms.untracked.string('CA'),
              tag = cms.untracked.InputTag('ak5CaloJets'),
              sel_minpt  = process.analyze.sel_mincorjpt,
              sel_maxeta = process.analyze.sel_maxjeta,
              corrections = cms.string('ak5CaloL2L3'),
              ),
    )

#### Path ######################################################################
process.mybtag = cms.Sequence(	process.simpleSecondaryVertexHighPurBJetTags
								* process.simpleSecondaryVertexHighEffBJetTags )

process.p = cms.Path(
    process.cleaning
    + process.jecCorSequence
    + process.recoJetIdSequence
    + process.metCorSequence
    + process.mybtag
    + process.analyze
    )
