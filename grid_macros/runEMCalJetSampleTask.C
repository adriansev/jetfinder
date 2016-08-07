// runEMCalJetAnalysisNew.C

class AliESDInputHandler;
class AliAODInputHandler;
class AliVEvent;
class AliAnalysisManager;
class AliPhysicsSelectionTask;
class AliCentralitySelectionTask;
class AliEmcalSetupTask;
class AliAnalysisGrid;

void LoadMacros();
void StartGridAnalysis(AliAnalysisManager* pMgr, const char* uniqueName, const char* cGridMode);
AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers,
    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, Int_t workerTTL, Bool_t isMC);

const UInt_t  kEMCAL   = AliEmcalPhysicsSelection::kEmcalOk;
const UInt_t  kMB      = AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral;

//______________________________________________________________________________
AliAnalysisManager* runEMCalJetSampleTask(
    const char   *cDataType      = "AOD",                    // set the analysis type, AOD or ESD
    const char   *cRunPeriod     = "LHC11c",                 // set the run period
    const char   *cLocalFiles    = "data.txt",               // set the local list file
    const UInt_t  iNumEvents     = 1234567890,               // number of events to be analyzed
    const UInt_t  kPhysSel       = kMB,                      // physics selection
    const char   *cTaskName      = "EMCalJetAna",            // sets name of analysis manager
    const Bool_t  bDoChargedJets = kTRUE,
    const Bool_t  bDoFullJets    = kTRUE,
    const char   *cOCDBpath      = "uselocal",                 // change to "raw://" if running on the grid
    // 0 = only prepare the analysis manager but do not start the analysis
    // 1 = prepare the analysis manager and start the analysis
    // 2 = launch a grid analysis
    Int_t         iStartAnalysis = 1,
    const UInt_t  iNumFiles      = 6000,                                     // number of files analyzed locally
    const char   *cGridMode      = "test"
)
{
  TString sRunPeriod(cRunPeriod);
  sRunPeriod.ToLower();

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;

  Bool_t bIsRun2 = kFALSE;
  if (sRunPeriod.Length() == 6 && sRunPeriod.BeginsWith("lhc15")) { bIsRun2 = kTRUE; }

  if (sRunPeriod == "lhc10h" || sRunPeriod == "lhc11h" || sRunPeriod == "lhc15o") { iBeamType = AliAnalysisTaskEmcal::kAA; }
  else
  if (sRunPeriod == "lhc12g" || sRunPeriod == "lhc13b" || sRunPeriod == "lhc13c" ||
      sRunPeriod == "lhc13d" || sRunPeriod == "lhc13e" || sRunPeriod == "lhc13f")
    { iBeamType = AliAnalysisTaskEmcal::kpA; }

  if (iBeamType != AliAnalysisTaskEmcal::kpp) { std::cout << "Comment this line if you want to run on Pb" << std::endl; gApplication->Terminate();}

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(sRunPeriod);

  Printf("Default track cut period set to: %s", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  const Bool_t   bDoTender            = kTRUE;
  const Bool_t   bDoHadCorr           = kTRUE;
  const Double_t kClusECut            = 0.30;
  const Double_t kTrackPtCut          = 0.15;
  const Double_t kJetPtCut            = 1.;
  const Double_t kHadCorrF            = 2.;

  enum eDataType { kAod, kEsd };

  eDataType iDataType;
  if (!strcmp(cDataType, "ESD")) { iDataType = kEsd; }
  else 
  if (!strcmp(cDataType, "AOD")) { iDataType = kAod; }
  else
    {
    Printf("Incorrect data type option, check third argument of run macro.");
    Printf("datatype = AOD or ESD");
    return 0;
    }

  Printf("%s analysis chosen.", cDataType);

  TString sLocalFiles(cLocalFiles);
  if (iStartAnalysis == 1) {
    if (sLocalFiles == "") { Printf("You need to provide the list of local files!"); return 0; }
    Printf("Setting local analysis for %d files from list %s, max events = %d", iNumFiles, sLocalFiles.Data(), iNumEvents);
    }

  const AliJetContainer::EJetType_t  fulljet = AliJetContainer::kFullJet;
  const AliJetContainer::EJetType_t   chgjet = AliJetContainer::kChargedJet;

  const AliJetContainer::JetAcceptanceType acc_tpc   = AliJetContainer::kTPCfid;      // kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kUser
  const AliJetContainer::JetAcceptanceType acc_emcal = AliJetContainer::kEMCALfid;    // kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kUser

  const AliJetContainer::EJetAlgo_t     antikt = AliJetContainer::antikt_algorithm;
  const AliJetContainer::EJetAlgo_t         kt = AliJetContainer::kt_algorithm;
  const AliJetContainer::ERecoScheme_t  recomb = AliJetContainer::pt_scheme;

  LoadMacros();

  // Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);

  if (iDataType == kAod)
    { AliAODInputHandler* pAODHandler = AddAODHandler(); }
  else 
    { AliESDInputHandler* pESDHandler = AddESDHandler(); }

  // Physics selection task
  if (iDataType == kEsd) { AliPhysicsSelectionTask *pPhysSelTask = AddTaskPhysicsSelection(); }

  // Centrality task
  if (iDataType == kEsd && iBeamType != AliAnalysisTaskEmcal::kpp) {
    AliCentralitySelectionTask *pCentralityTask = AddTaskCentrality(kTRUE);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  // Setup task
  if (bDoFullJets || iDataType == kEsd) {
    AliEmcalSetupTask *pSetupTask = AddTaskEmcalSetup();
    pSetupTask->SelectCollisionCandidates(AliVEvent::kAny);
    pSetupTask->SetOcdbPath(cOCDBpath);
    }

  if (bDoTender) {
    // Only cell energy/time recalibration (and bad channel) is switched on
    Bool_t   bCalibEnergy    = kTRUE;
    Bool_t   bCalibTime      = kTRUE;
    Bool_t   bRemBC          = kTRUE;
    Bool_t   bUpdateCellOnly = kTRUE;

    // All these parameters are irrelevant for the tender
    const char *cPass        = 0;
    Bool_t   bDistBC         = kFALSE;
    Bool_t   bRecalibClus    = kFALSE;
    Bool_t   bRecalcClusPos  = kFALSE;
    Bool_t   bNonLinearCorr  = kFALSE;
    Bool_t   bRemExoticCell  = kFALSE;
    Bool_t   bRemExoticClus  = kFALSE;
    Bool_t   bFidRegion      = kFALSE;
    UInt_t   iNonLinFunct    = AliEMCALRecoUtils::kNoCorrection;
    Bool_t   bReclusterize   = kFALSE;
    Float_t  fSeedThresh     = 0.1;      // 100 MeV
    Float_t  fCellThresh     = 0.05;     // 50 MeV
    UInt_t   iClusterizer    = AliEMCALRecParam::kClusterizerv2;
    Bool_t   bTrackMatch     = kFALSE;


    AliAnalysisTaskSE *pTenderTask = AddTaskEMCALTender(bDistBC, bRecalibClus, bRecalcClusPos, bNonLinearCorr, bRemExoticCell, bRemExoticClus,
        bFidRegion, bCalibEnergy, bCalibTime, bRemBC, iNonLinFunct, bReclusterize, fSeedThresh,
        fCellThresh, iClusterizer, bTrackMatch, bUpdateCellOnly, 0, 1e6, 1e6, cPass);
    pTenderTask->SelectCollisionCandidates(kPhysSel);

    // Time cuts are switched off at cell level
    Float_t  fEMCtimeMin     = -50e-6;
    Float_t  fEMCtimeMax     =  50e-6;
    Float_t  fEMCtimeCut     =  1e6;

    AliAnalysisTaskEMCALClusterizeFast *pClusterizerTask = AddTaskClusterizerFast("ClusterizerFast", "", "", iClusterizer,
        0.05, 0.1, fEMCtimeMin, fEMCtimeMax, fEMCtimeCut,
        kFALSE, kFALSE, AliAnalysisTaskEMCALClusterizeFast::kFEEData);
    pClusterizerTask->SelectCollisionCandidates(kPhysSel);

    bRemExoticClus  = kTRUE;
    iNonLinFunct    = AliEMCALRecoUtils::kBeamTestCorrected;

    AliEmcalClusterMaker *pClusterMakerTask = AddTaskEmcalClusterMaker(iNonLinFunct, bRemExoticClus, "usedefault", "", 0., kTRUE);
    pClusterMakerTask->GetClusterContainer(0)->SetClusPtCut(0.);
    pClusterMakerTask->GetClusterContainer(0)->SetClusECut(0.);
    pClusterMakerTask->SelectCollisionCandidates(kPhysSel);
    }

  // Cluster-track matcher task
  AliEmcalClusTrackMatcherTask *pMatcherTask = AddTaskEmcalClusTrackMatcher("usedefault", "usedefault", 0.1, kFALSE, kTRUE, kTRUE, kTRUE);
  pMatcherTask->SelectCollisionCandidates(kPhysSel);
  pMatcherTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
  pMatcherTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.15);
  pMatcherTask->GetClusterContainer(0)->SetClusECut(0.);
  pMatcherTask->GetClusterContainer(0)->SetClusPtCut(0.);

  if (iDataType == kEsd) { pMatcherTask->SetDoPropagation(kTRUE); }

  if (bDoHadCorr) {
    // Hadronic correction task
    AliHadCorrTask *pHadCorrTask = AddTaskHadCorr("usedefault", "usedefault", "", kHadCorrF, 0.15, 0.030, 0.015, 0, kTRUE, kTRUE);
    pHadCorrTask->SelectCollisionCandidates(kPhysSel);
    pHadCorrTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.15);
    pHadCorrTask->GetClusterContainer(0)->SetClusECut(0);
    pHadCorrTask->GetClusterContainer(0)->SetClusPtCut(0.);
    }

  // Background
  TString sRhoChName;
  TString sRhoFuName;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) {
    sRhoChName = "Rho";
    sRhoFuName = "Rho_Scaled";

    AliEmcalJetTask *pKtChJetTask = AddTaskEmcalJet("usedefault", "", kt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 0., kFALSE, kFALSE);
    pKtChJetTask->SelectCollisionCandidates(kPhysSel);

    AliAnalysisTaskRho* pRhoTask = AddTaskRhoNew("usedefault", "usedefault", sRhoChName, 0.4);
    pRhoTask->SetExcludeLeadJets(2);
    pRhoTask->SelectCollisionCandidates(kPhysSel);

    if (bDoFullJets) {
      TString sFuncPath = "alien:///alice/cern.ch/user/s/saiola/LHC11h_ScaleFactorFunctions.root";
      TString sFuncName = "LHC11h_HadCorr20_ClustersV2";
      pRhoTask->LoadRhoFunction(sFuncPath, sFuncName);
      }
    }

  // Charged jet analysis
  if (bDoChargedJets) {
    AliEmcalJetTask *pChJet02Task = AddTaskEmcalJet("usedefault", "", antikt, 0.2, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet02Task->SelectCollisionCandidates(kPhysSel);

    AliEmcalJetTask *pChJet04Task = AddTaskEmcalJet("usedefault", "", antikt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet04Task->SelectCollisionCandidates(kPhysSel);
    }

  // Full jet analysis
  if (bDoFullJets) {
    AliEmcalJetTask *pFuJet02Task = AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.2, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet02Task->SelectCollisionCandidates(kPhysSel);
    pFuJet02Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

    AliEmcalJetTask *pFuJet04Task = AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.4, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet04Task->SelectCollisionCandidates(kPhysSel);
    pFuJet04Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }

  // Sample task
  AliAnalysisTaskEmcalJetSample *sampleTask = 0;
  sampleTask = AddTaskEmcalJetSample("usedefault", "usedefault", "usedefault");
  sampleTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
  sampleTask->SelectCollisionCandidates(kPhysSel);
  sampleTask->SetHistoBins(600, 0, 300);
  sampleTask->GetClusterContainer(0)->SetClusECut(0.);
  sampleTask->GetClusterContainer(0)->SetClusPtCut(0.);
  sampleTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
  sampleTask->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
  sampleTask->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

  //###   CDF task
  AliAnalysisTaskEmcalJetCDF* anaTaskCDF = 0;
  anaTaskCDF = dynamic_cast<AliAnalysisTaskEmcalJetCDF*>NS_AliAnalysisTaskEmcalJetCDF::AddTaskEmcalJetCDF ( "usedefault", "usedefault", "usedefault", "CDF" );
  anaTaskCDF->GetParticleContainer(0)->SetParticlePtCut(0.15);
  anaTaskCDF->SelectCollisionCandidates(kPhysSel);
  anaTaskCDF->GetClusterContainer(0)->SetClusECut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusPtCut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
  anaTaskCDF->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

  if (bDoFullJets) {
    AliJetContainer* jetCont02full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet");
    AliJetContainer* jetCont04full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet");

    AliJetContainer* jetCont02full_cdf_bin1  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin1, 1., 5., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin2  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin2, 5., 10., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin3  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin3, 10., 15., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin4  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin4, 15., 20., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin5  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin5, 20., 25., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin6  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin6, 25., 30., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin7  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin7, 30., 35., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin8  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin8, 35., 40., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin9  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin9, 40., 45., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin10 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin10, 45., 50., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin11 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin11, 50., 55., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin12 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin12, 55., 60., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin13 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin13, 60., 65., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin14 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin14, 65., 70., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin15 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_bin15, 70., 500., 0, 2);
    AliJetContainer* jetCont02full_cdf_all   = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02full_cdf_all, 1., 500., 0, 2);

    AliJetContainer* jetCont04full_cdf_bin1  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin1, 1., 5., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin2  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin2, 5., 10., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin3  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin3, 10., 15., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin4  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin4, 15., 20., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin5  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin5, 20., 25., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin6  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin6, 25., 30., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin7  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin7, 30., 35., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin8  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin8, 35., 40., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin9  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin9, 40., 45., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin10 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin10, 45., 50., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin11 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin11, 50., 55., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin12 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin12, 55., 60., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin13 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin13, 60., 65., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin14 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin14, 65., 70., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin15 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_bin15, 70., 500., 0, 2);
    AliJetContainer* jetCont04full_cdf_all   = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04full_cdf_all, 1., 500., 0, 2);

    }

  if (bDoChargedJets) {
    AliJetContainer* jetCont02chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet");
    AliJetContainer* jetCont04chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet");

    AliJetContainer* jetCont02chg_cdf_bin1  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin1, 1., 5., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin2  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin2, 5., 10., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin3  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin3, 10., 15., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin4  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin4, 15., 20., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin5  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin5, 20., 25., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin6  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin6, 25., 30., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin7  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin7, 30., 35., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin8  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin8, 35., 40., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin9  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin9, 40., 45., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin10 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin10, 45., 50., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin11 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin11, 50., 55., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin12 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin12, 55., 60., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin13 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin13, 60., 65., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin14 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin14, 65., 70., 0, 2);
    AliJetContainer* jetCont02chg_cdf_bin15 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_bin15, 70., 500., 0, 2);
    AliJetContainer* jetCont02chg_cdf_all   = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_emcal, "Jet"); jetContSetPropreties (jetCont02chg_cdf_all, 1., 500., 0, 2);

    AliJetContainer* jetCont04chg_cdf_bin1  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin1, 1., 5., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin2  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin2, 5., 10., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin3  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin3, 10., 15., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin4  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin4, 15., 20., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin5  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin5, 20., 25., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin6  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin6, 25., 30., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin7  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin7, 30., 35., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin8  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin8, 35., 40., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin9  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin9, 40., 45., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin10 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin10, 45., 50., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin11 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin11, 50., 55., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin12 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin12, 55., 60., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin13 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin13, 60., 65., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin14 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin14, 65., 70., 0, 2);
    AliJetContainer* jetCont04chg_cdf_bin15 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_bin15, 70., 500., 0, 2);
    AliJetContainer* jetCont04chg_cdf_all   = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_emcal, "Jet"); jetContSetPropreties (jetCont04chg_cdf_all, 1., 500., 0, 2);

    }

  TObjArray *pTopTasks = pMgr->GetTasks();
  for (Int_t i = 0; i < pTopTasks->GetEntries(); ++i) {
    AliAnalysisTaskSE *pTask = dynamic_cast<AliAnalysisTaskSE*>(pTopTasks->At(i));
    if (!pTask) continue;
    if (pTask->InheritsFrom("AliAnalysisTaskEmcal")) {
      AliAnalysisTaskEmcal *pTaskEmcal = static_cast<AliAnalysisTaskEmcal*>(pTask);
      Printf("Setting beam type %d for task %s", iBeamType, pTaskEmcal->GetName());
      pTaskEmcal->SetForceBeamType(iBeamType);
      }
    }

  if (!pMgr->InitAnalysis()) return 0;
  pMgr->PrintStatus();

//  pMgr->SetUseProgressBar(kTRUE, 250);

  TFile *pOutFile = new TFile("train.root","RECREATE");
  pOutFile->cd();
  pMgr->Write();
  pOutFile->Close();
  delete pOutFile;

  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateESDChain.C");

  if (iStartAnalysis == 1) { // start local analysis
    TChain* pChain = 0;
    if (iDataType == kAod)
      { pChain = CreateAODChain(sLocalFiles.Data(), iNumFiles, 0, kFALSE); }
    else 
      { pChain = CreateESDChain(sLocalFiles.Data(), iNumFiles, 0, kFALSE); }

    // start analysis
    Printf("Starting Analysis...");
    pMgr->StartAnalysis("local", pChain, iNumEvents);
    }
  else 
  if (iStartAnalysis == 2) { StartGridAnalysis(pMgr, cTaskName, cGridMode); } // start grid analysis

return pMgr;
}

void LoadMacros()
{
  // Aliroot macros
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalSetup.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEMCALTender.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskClusterizerFast.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusterMaker.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusTrackMatcher.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskHadCorr.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C");
}

void StartGridAnalysis(AliAnalysisManager* pMgr, const char* uniqueName, const char* cGridMode)
{
  Int_t maxFilesPerWorker = 4;
  Int_t workerTTL = 7200;
  const char* runNumbers = "180720";
  const char* pattern = "pass2/AOD/*/AliAOD.root";
  const char* gridDir = "/alice/data/2012/LHC12c";
  const char* additionalCXXs = "";
  const char* additionalHs = "";

  AliAnalysisGrid *plugin = CreateAlienHandler(uniqueName, gridDir, cGridMode, runNumbers, pattern, additionalCXXs, additionalHs, maxFilesPerWorker, workerTTL, kFALSE);
  pMgr->SetGridHandler(plugin);

  // start analysis
   Printf("Starting GRID Analysis...");
   pMgr->SetDebugLevel(0);
   pMgr->StartAnalysis("grid");
}

AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers,
    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, Int_t workerTTL, Bool_t isMC)
{
  TDatime currentTime;
  TString tmpName(uniqueName);

  // Only add current date and time when not in terminate mode! In this case the exact name has to be supplied by the user
  if (strcmp(gridMode, "terminate")) {
    tmpName += "_";
    tmpName += currentTime.GetDate();
    tmpName += "_";
    tmpName += currentTime.GetTime();
  }

  TString macroName("");
  TString execName("");
  TString jdlName("");
  macroName = Form("%s.C", tmpName.Data());
  execName = Form("%s.sh", tmpName.Data());
  jdlName = Form("%s.jdl", tmpName.Data());

  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetOverwriteMode();
  plugin->SetRunMode(gridMode);

  // Here you can set the (Ali)PHYSICS version you want to use
  plugin->SetAliPhysicsVersion("vAN-20160203-1");

  plugin->SetGridDataDir(gridDir); // e.g. "/alice/sim/LHC10a6"
  plugin->SetDataPattern(pattern); //dir structure in run directory

  if (!isMC) plugin->SetRunPrefix("000");

  plugin->AddRunList(runNumbers);

  plugin->SetGridWorkingDir(Form("work/%s",tmpName.Data()));
  plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output

  plugin->SetAnalysisSource(additionalCode.Data());

  plugin->SetDefaultOutputs(kTRUE);
  plugin->SetAnalysisMacro(macroName.Data());
  plugin->SetSplitMaxInputFileNumber(maxFilesPerWorker);
  plugin->SetExecutable(execName.Data());
  plugin->SetTTL(workerTTL);
  plugin->SetInputFormat("xml-single");
  plugin->SetJDLName(jdlName.Data());
  plugin->SetPrice(1);
  plugin->SetSplitMode("se");

  // merging via jdl
  plugin->SetMergeViaJDL(kTRUE);
  plugin->SetOneStageMerging(kFALSE);
  plugin->SetMaxMergeStages(2);

  return plugin;
}

//______________________________________________________________________________
AliJetContainer* jetContSetPropreties (AliJetContainer* jetCont,
                                      Float_t jetptmin = 1.,
                                      Float_t jetptmax = 500.,
                                      Float_t jetareacutperc = 0.,
                                      Int_t leadhadtype = 2,
                                      Int_t nLeadJets = 1,
                                      Float_t mintrackpt = 0.15,
                                      Float_t maxtrackpt = 1000.
                                      )
{
  if (!jetCont) { return NULL; }
  jetCont->SetJetPtCut ( jetptmin );
  jetCont->SetJetPtCutMax ( jetptmax );
  jetCont->SetPercAreaCut ( jetareacutperc );
  jetCont->SetLeadingHadronType ( leadhadtype ); // Int_t fLeadingHadronType;  0 = charged, 1 = neutral, 2 = both
  jetCont->SetNLeadingJets(nLeadJets);
  jetCont->SetMinTrackPt(mintrackpt);
  jetCont->SetMaxTrackPt(maxtrackpt);

  return jetCont;
}

// kate: indent-mode none; indent-width 2; replace-tabs on;

