import FWCore.ParameterSet.Config as cms

analyze = cms.EDFilter('NTupleProducer',
	# Main settings
	isRealData       = cms.bool(False),
	isFastSim        = cms.bool(False),
        isModelScan      = cms.bool(False),
	# Collections
	tag_muons        = cms.InputTag('muons'),
        tag_muonpfisosCustom = cms.VInputTag(''), # No defaults: set in _cfg.py
	tag_muisodeptk   = cms.InputTag('muons','muIsoDepositTk'),
	tag_muisodepec   = cms.InputTag('muons','ecal'),
	tag_muisodephc   = cms.InputTag('muons','hcal'),
	tag_electrons    = cms.InputTag('gsfElectrons'),
        tag_elepfisosCustom = cms.VInputTag(''), # No defaults: set in _cfg.py
        tag_elepfisosEvent  = cms.VInputTag(''), # No defaults: set in _cfg.py
	tag_jets         = cms.InputTag('ak5PFJets'),
	jetCorrs         = cms.string('ak5PFL1FastL2L3'),
	tag_btags        = cms.VInputTag(''), # No defaults: set in _cfg.py
	tag_partonmatch  = cms.InputTag('AK5PFbyValAlgo'),
	tag_rawcalomet   = cms.InputTag('met'),
	tag_tcmet        = cms.InputTag('tcMet'),
	tag_pfmet        = cms.InputTag('pfMet'),
	tag_corrcalomet  = cms.InputTag('metMuonJESCorAK5'),
	tag_genmet       = cms.InputTag('genMetTrue'),
	tag_vertex       = cms.InputTag('offlinePrimaryVertices'),
	tag_tracks       = cms.InputTag('generalTracks'),
	tag_photons      = cms.InputTag('photons'),
	tag_caltow       = cms.InputTag('towerMaker'),
	tag_EBrechits    = cms.InputTag('reducedEcalRecHitsEB'),
	tag_EErechits    = cms.InputTag('reducedEcalRecHitsEE'),
	tag_genpart      = cms.InputTag('genParticles'),
	tag_genjets      = cms.InputTag('ak5GenJets'),
	tag_l1trig       = cms.InputTag("gtDigis"),
	tag_hlttrigevent = cms.InputTag("hltTriggerSummaryAOD"),
	tag_srcRho       = cms.InputTag('kt6PFJets','rho'),
	tag_srcSigma       = cms.InputTag('kt6PFJets','sigma'),
        tag_srcRhoForIso = cms.InputTag('kt6PFJetsForIso','rho'),
	tag_pfphotonsProducer  = cms.InputTag("pfPhotonTranslator:pfphot"),
	tag_pfProducer = cms.InputTag("particleFlow"),
        tag_SC_barrel    = cms.InputTag("correctedHybridSuperClusters"),
        tag_SC_endcap    = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),

        tag_doVertexing = cms.bool(False), # overwritten from test/ntupleproducer_cfg.py
        tag_fTrackCollForVertexing = cms.InputTag("generalTracks"),
        tag_fallConversionsCollForVertexing = cms.InputTag("allConversions"),
        tag_perVtxMvaWeights = cms.string(""), # overwritten from test/ntupleproducer_cfg.py
        tag_perVtxMvaMethod = cms.string("BDTCat"),
        tag_perEvtMvaWeights = cms.string(""), # overwritten from test/ntupleproducer_cfg.py
        tag_perEvtMvaMethod = cms.string("evtBDTG"),

                         
	# Trigger paths to store the triggering object information of
        hlt_labels = cms.vstring('HLT_IsoMu24_eta2p1_v',
                                 'HLT_Ele27_WP80_v'),
	# Event Selection Criteria
	# Muons:
	sel_minmupt       = cms.double(5.0),
	sel_maxmueta      = cms.double(2.5),
	# Taus:
	sel_mintaupt      = cms.double(10.0),
	sel_maxtaueta     = cms.double(2.5),
	# Electrons:
	sel_minelpt       = cms.double(5.0),
	sel_maxeleta      = cms.double(2.5),
	# Jets:
	sel_mincorjpt     = cms.double(15.0),
	sel_minrawjpt     = cms.double(0.0),
	sel_maxjeta       = cms.double(10.0),
	sel_minjemfrac    = cms.double(0.0),
	# Tracks:
	sel_mintrkpt      = cms.double(1.0),
	sel_maxtrketa     = cms.double(10.0),
	sel_maxtrknchi2   = cms.double(1e15),
	sel_mintrknhits   = cms.int32(0),
	# Photons
	sel_minphopt      = cms.double(5.0),
	sel_maxphoeta     = cms.double(2.5),
        # SC
        sel_minSCraw      = cms.double(5.0),
	# GenLeptons
	sel_mingenleptpt  = cms.double(0.0),
	sel_maxgenlepteta = cms.double(100),
	# GenPhotons
	sel_mingenphotpt = cms.double(5.0),
	sel_maxgenphoteta = cms.double(2.5),
	# GenJets
	sel_mingenjetpt  = cms.double(10.0),
	sel_maxgenjeteta = cms.double(6.0),

	# EB rechits
        sel_fminebrechitE = cms.double(20.),

	# Additional collections
	jets    = cms.VPSet(),
        leptons = cms.VPSet(),
        pfCandidates = cms.VPSet(),

	# tag pile up distributions: replace empty strings in order to calculate in time and OOT pileup weights
	pu_data = cms.vstring('', ''), # replace this by cms.vstring('data_pileup.root', 'name_of_histo')
	pu_mc   = cms.vstring('', '')  # replace this by cms.vstring('mc_pileup.root'  , 'name_of_histo')

)



