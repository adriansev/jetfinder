#if !defined(__CINT__) || !defined(__CLING__)

// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>

// ROOT includes
#include "Riostream.h"
#include "Rtypes.h"
#include "TRint.h"
#include "TROOT.h"

#include "TSystem.h"
#include "TApplication.h"
#include "TString.h"
#include "TList.h"
#include "TEnv.h"
#include "TKey.h"
#include "TBits.h"
#include "TError.h"
#include "TNamed.h"
#include "TTask.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "TCollection.h"
#include "TChain.h"
#include "TObjString.h"
#include "TObjArray.h"

#include "TObject.h"
#include "TNamed.h"
#include "TTask.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisDataContainer.h"

#include "AliAnalysisTaskEmcal.h"
#include "AliAnalysisTaskEmcalJet.h"

// AliROOT includes
#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"

#include "AliVCluster.h"

#include "AliMultiInputEventHandler.h"
#include "AliLog.h"
#include "AliVEventHandler.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliESDInputHandler.h"
#include "AliInputEventHandler.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskTagCreator.h"

#include "AliTaskCDBconnect.h"

#include "AliAnalysisTaskRho.h"
#include "AliAnalysisTaskEMCALClusterizeFast.h"

#include "AliPhysicsSelectionTask.h"
#include "AliCentralitySelectionTask.h"

#include "AliEMCALTenderSupply.h"
#include "AliEmcalCompatTask.h"
#include "AliEMCALRecParam.h"
#include "AliEmcalJetTask.h"
#include "AliEmcalPhysicsSelection.h"
#include "AliEmcalClusterMaker.h"
#include "AliEmcalClusTrackMatcherTask.h"
#include "AliEMCALRecoUtils.h"

#include "AliClusterContainer.h"
#include "AliEmcalContainer.h"
#include "AliMCParticleContainer.h"
#include "AliParticleContainer.h"
#include "AliTrackContainer.h"
#include "AliJetContainer.h"

#include "AliHadCorrTask.h"

#include "AliAnalysisTaskEmcalJetCDF.h"
#include "AliAnalysisTaskEmcalJetSample.h"

// Macro locations
#include "../OADB/macros/AddTaskCentrality.C"
#include "../PWGPP/PilotTrain/AddTaskCDBconnect.C"

#include "../ANALYSIS/macros/train/AddAODHandler.C"
#include "../ANALYSIS/macros/train/AddESDHandler.C"
#include "../ANALYSIS/macros/train/AddMCHandler.C"

#include "../PWG/EMCAL/macros/CreateAODChain.C"
#include "../PWG/EMCAL/macros/CreateESDChain.C"

#include "../OADB/macros/AddTaskPhysicsSelection.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C"

#include "../PWG/EMCAL/macros/AddTaskEMCALTender.C"
#include "../PWG/EMCAL/macros/AddTaskClusterizerFast.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalClusterMaker.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalClusTrackMatcher.C"
#include "../PWG/EMCAL/macros/AddTaskHadCorr.C"

#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C"

#include "../PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDF.C"

#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C"

#include "InputData.C"

#endif
class AliESDInputHandler;
class AliAODInputHandler;
class AliVEvent;
class AliAnalysisManager;
class AliPhysicsSelectionTask;
class AliCentralitySelectionTask;
class AliEmcalCorrectionTask;
class AliEmcalJetTask;
class AliAnalysisGrid;

void LoadLibs ();
void LoadLibList ( const TString& list );
Bool_t LoadLibrary ( const TString& lib );

void LoadMacros();
void StartGridAnalysis(AliAnalysisManager* pMgr, const char* uniqueName, const char* cGridMode);
AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers,
    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, Int_t workerTTL, Bool_t isMC);

const UInt_t  kEMCAL      = AliEmcalPhysicsSelection::kEmcalOk;
const UInt_t  kMB         = AliVEvent::kAnyINT;
const UInt_t  kEMC        = AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE | AliVEvent::kEMCEGA;
const UInt_t  kEMC_noGA   = AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE;

const AliJetContainer::EJetType_t  fulljet = AliJetContainer::kFullJet;
const AliJetContainer::EJetType_t   chgjet = AliJetContainer::kChargedJet;

const AliEmcalJet::JetAcceptanceType acc_tpcfid   = AliEmcalJet::kTPCfid;      // kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kUser
const AliEmcalJet::JetAcceptanceType acc_emcalfid = AliEmcalJet::kEMCALfid;    // kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kUser

const AliJetContainer::EJetAlgo_t     antikt = AliJetContainer::antikt_algorithm;
const AliJetContainer::EJetAlgo_t         kt = AliJetContainer::kt_algorithm;
const AliJetContainer::ERecoScheme_t  recomb = AliJetContainer::pt_scheme;

const AliEmcalJet::JetAcceptanceType acc_chgjets  = acc_emcalfid;
const AliEmcalJet::JetAcceptanceType acc_fulljets = acc_emcalfid;

const UInt_t kSel_chg = kEMC_noGA;
const UInt_t kSel_full = kEMC_noGA;
const UInt_t kSel_tasks = kEMC_noGA;

const Bool_t  bDoChargedJets = 1;
const Bool_t  bDoFullJets    = 1;

enum eDataType { kAod, kEsd };

TString     ListLibs      = "";
TString     ListLibsExtra = "";

//______________________________________________________________________________
AliAnalysisManager* runEMCalJetSampleTask(
    const char   *cDataType      = "AOD",                    // set the analysis type, AOD or ESD
    const char   *cRunPeriod     = "LHC11c",                 // set the run period
    const char   *cLocalFiles    = "data.txt",               // set the local list file
    const UInt_t  iNumEvents     = 1234567890,               // number of events to be analyzed
    const UInt_t  kPhysSel       = kMB,                      // physics selection
    Int_t         iStartAnalysis = 1,
    const UInt_t  iNumFiles      = 100,                     // number of files analyzed locally
    const char   *cGridMode      = "test"
)
{
  const char   *cTaskName      = "CDFJets";            // sets name of analysis manager

  TString sRunPeriod(cRunPeriod);
  sRunPeriod.ToLower();

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;

  Bool_t bIsRun2 = kFALSE;
  if (sRunPeriod.Length() == 6 && sRunPeriod.BeginsWith("lhc15")) { bIsRun2 = kTRUE; }

  if (sRunPeriod == "lhc10h" || sRunPeriod == "lhc11h" || sRunPeriod == "lhc15o") { iBeamType = AliAnalysisTaskEmcal::kAA; }
  else
  if (sRunPeriod == "lhc12g" || sRunPeriod == "lhc13b" || sRunPeriod == "lhc13c" ||
      sRunPeriod == "lhc13d" || sRunPeriod == "lhc13e" || sRunPeriod == "lhc13f")||
      sRunPeriod == "lhc16q" || sRunPeriod == "lhc16r" || sRunPeriod == "lhc16s" || sRunPeriod == "lhc16t" )
  { iBeamType = AliAnalysisTaskEmcal::kpA; }

  if (iBeamType != AliAnalysisTaskEmcal::kpp) { std::cout << "Comment this line if you want to run on Pb" << std::endl; gApplication->Terminate();}

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(sRunPeriod);
  Printf("Default track cut period set to: %s", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  LoadLibs();
  namespace CDF = NS_AliAnalysisTaskEmcalJetCDF;

  Bool_t   bDoEmcalCorrections  = kFALSE;
  if (bDoFullJets) { bDoEmcalCorrections  = kTRUE; }

  const Double_t kHadCorrF            = 2.;

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

  LoadMacros();

  // Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);

  if (iDataType == kAod)
    { AliAODInputHandler* pAODHandler = AddAODHandler(); }
  else
    { AliESDInputHandler* pESDHandler = AddESDHandler(); }

  // CDBconnect task
  AliTaskCDBconnect *taskCDB = AddTaskCDBconnect();
  taskCDB->SetFallBackToRaw(kTRUE);
  if (!taskCDB) { cout << "--------->>>>  taskCDB :: could not connect!!!! CHECK CVMFS" << endl; return NULL;}

  // Physics selection task
  if (iDataType == kEsd) { AliPhysicsSelectionTask *pPhysSelTask = AddTaskPhysicsSelection(); }

  // Centrality task
  if (iDataType == kEsd && iBeamType != AliAnalysisTaskEmcal::kpp) {
    AliCentralitySelectionTask *pCentralityTask = AddTaskCentrality(kTRUE);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  if (bDoEmcalCorrections) {
    // Configuration of the Correction Task is handled via a YAML file, which is setup below
    // NOTE: Calling this function is equivalent to the AddTask macro, just the function is defined in the class.
    //       The AddTask macro still exists for use on the LEGO train
    AliEmcalCorrectionTask * correctionTask = AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask();
    correctionTask->SelectCollisionCandidates(kPhysSel);
    correctionTask->SetForceBeamType(iBeamType);

    // Configure and initialize
    correctionTask->SetUserConfigurationFilename("PWGJE_SEV_Config.yaml");
    correctionTask->Initialize();
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
    pChJet02Task->SelectCollisionCandidates(kSel_chg);

    AliEmcalJetTask *pChJet04Task = AddTaskEmcalJet("usedefault", "", antikt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet04Task->SelectCollisionCandidates(kSel_chg);
    }

  // Full jet analysis
  if (bDoFullJets) {
    AliEmcalJetTask *pFuJet02Task = AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.2, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet02Task->SelectCollisionCandidates(kSel_full);
    pFuJet02Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

    AliEmcalJetTask *pFuJet04Task = AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.4, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet04Task->SelectCollisionCandidates(kSel_full);
    pFuJet04Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }

  // Sample task
  AliAnalysisTaskEmcalJetSample* sampleTask = AddTaskEmcalJetSample("usedefault", "usedefault", "usedefault", "new");
  sampleTask->SelectCollisionCandidates(kSel_tasks);

  sampleTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
  sampleTask->SetHistoBins(600, 0, 300);
  sampleTask->GetClusterContainer(0)->SetClusECut(0.);
  sampleTask->GetClusterContainer(0)->SetClusPtCut(0.);
  sampleTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
  sampleTask->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
  sampleTask->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

  //###   CDF task
  AliAnalysisTaskEmcalJetCDF* anaTaskCDF = CDF::AddTaskEmcalJetCDF ( "usedefault", "usedefault", "usedefault", "CDF" );
  anaTaskCDF->SelectCollisionCandidates(kSel_tasks);

  anaTaskCDF->GetParticleContainer(0)->SetParticlePtCut(0.15);
  anaTaskCDF->GetClusterContainer(0)->SetClusECut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusPtCut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
  anaTaskCDF->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
  anaTaskCDF->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

  if (bDoFullJets) {
    AliJetContainer* jetCont02full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
    AliJetContainer* jetCont04full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");

    if (iBeamType != AliAnalysisTaskEmcal::kpp) {
      jetCont02full_sample->SetRhoName(sRhoFuName);
      jetCont02full_sample->SetPercAreaCut(0.6);
      jetCont04full_sample->SetRhoName(sRhoFuName);
      jetCont04full_sample->SetPercAreaCut(0.6);
      }

    AliJetContainer* jetCont02full_cdf_bin1  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin1,    1.,   5., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin2  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin2,    5.,  10., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin3  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin3,   10.,  15., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin4  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin4,   15.,  20., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin5  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin5,   20.,  25., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin6  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin6,   25.,  30., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin7  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin7,   30.,  35., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin8  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin8,   35.,  40., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin9  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin9,   40.,  45., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin10 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin10,  45.,  50., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin11 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin11,  50.,  55., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin12 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin12,  55.,  60., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin13 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin13,  60.,  65., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin14 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin14,  65.,  70., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin15 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin15,  70.,  75., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin16 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin16,  75.,  80., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin17 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin17,  80.,  85., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin18 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin18,  85.,  90., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin19 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin19,  90.,  95., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin20 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin20,  95., 100., 0, 2);
    AliJetContainer* jetCont02full_cdf_bin21 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_bin21, 100., 500., 0, 2);
    AliJetContainer* jetCont02full_cdf_all   = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont02full_cdf_all,     1., 500., 0, 2);

    AliJetContainer* jetCont04full_cdf_bin1  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin1,    1.,   5., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin2  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin2,    5.,  10., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin3  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin3,   10.,  15., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin4  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin4,   15.,  20., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin5  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin5,   20.,  25., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin6  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin6,   25.,  30., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin7  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin7,   30.,  35., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin8  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin8,   35.,  40., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin9  = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin9,   40.,  45., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin10 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin10,  45.,  50., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin11 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin11,  50.,  55., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin12 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin12,  55.,  60., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin13 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin13,  60.,  65., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin14 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin14,  65.,  70., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin15 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin15,  70.,  75., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin16 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin16,  75.,  80., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin17 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin17,  80.,  85., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin18 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin18,  85.,  90., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin19 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin19,  90.,  95., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin20 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin20,  95., 100., 0, 2);
    AliJetContainer* jetCont04full_cdf_bin21 = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_bin21, 100., 500., 0, 2);
    AliJetContainer* jetCont04full_cdf_all   = anaTaskCDF->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet"); CDF::jetContSetParams (jetCont04full_cdf_all,     1., 500., 0, 2);
    }

  if (bDoChargedJets) {
    AliJetContainer* jetCont02chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
    AliJetContainer* jetCont04chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");

    if (iBeamType != AliAnalysisTaskEmcal::kpp) {
      jetCont02chg_sample->SetRhoName(sRhoChName);
      jetCont02chg_sample->SetPercAreaCut(0.6);
      jetCont04chg_sample->SetRhoName(sRhoChName);
      jetCont04chg_sample->SetPercAreaCut(0.6);
      }

    AliJetContainer* jetCont02chg_cdf_bin1  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin1,    1.,   5., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin2  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin2,    5.,  10., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin3  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin3,   10.,  15., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin4  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin4,   15.,  20., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin5  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin5,   20.,  25., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin6  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin6,   25.,  30., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin7  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin7,   30.,  35., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin8  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin8,   35.,  40., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin9  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin9,   40.,  45., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin10 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin10,  45.,  50., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin11 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin11,  50.,  55., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin12 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin12,  55.,  60., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin13 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin13,  60.,  65., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin14 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin14,  65.,  70., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin15 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin15,  70.,  75., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin16 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin16,  75.,  80., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin17 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin17,  80.,  85., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin18 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin18,  85.,  90., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin19 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin19,  90.,  95., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin20 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin20,  95., 100., 0, 0);
    AliJetContainer* jetCont02chg_cdf_bin21 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_bin21, 100., 500., 0, 0);
    AliJetContainer* jetCont02chg_cdf_all   = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont02chg_cdf_all,     1., 500., 0, 0);


    AliJetContainer* jetCont04chg_cdf_bin1  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin1,    1.,   5., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin2  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin2,    5.,  10., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin3  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin3,   10.,  15., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin4  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin4,   15.,  20., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin5  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin5,   20.,  25., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin6  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin6,   25.,  30., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin7  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin7,   30.,  35., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin8  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin8,   35.,  40., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin9  = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin9,   40.,  45., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin10 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin10,  45.,  50., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin11 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin11,  50.,  55., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin12 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin12,  55.,  60., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin13 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin13,  60.,  65., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin14 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin14,  65.,  70., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin15 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin15,  70.,  75., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin16 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin16,  75.,  80., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin17 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin17,  80.,  85., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin18 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin18,  85.,  90., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin19 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin19,  90.,  95., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin20 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin20,  95., 100., 0, 0);
    AliJetContainer* jetCont04chg_cdf_bin21 = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_bin21, 100., 500., 0, 0);
    AliJetContainer* jetCont04chg_cdf_all   = anaTaskCDF->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet"); CDF::jetContSetParams (jetCont04chg_cdf_all,     1., 500., 0, 0);
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

cout << "END of runEMCalJetSampleTask.C" << endl;
return pMgr;
}

void LoadMacros()
{
  // Aliroot macros
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGPP/PilotTrain/AddTaskCDBconnect.C");

  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEMCALTender.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskClusterizerFast.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusterMaker.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusTrackMatcher.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskHadCorr.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C");
}

//######################################################################################################################################
void StartGridAnalysis(AliAnalysisManager* pMgr, const char* uniqueName, const char* cGridMode) {
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

//######################################################################################################################################
AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers,
    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, Int_t workerTTL, Bool_t isMC) {

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
  plugin->SetAliPhysicsVersion("vAN-20170622-1");

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

  if ( ListLibs.Length() )       { plugin->SetAdditionalLibs     ( ListLibs.Data() ); }
  if ( ListLibsExtra.Length() )  { plugin->SetAdditionalRootLibs ( ListLibsExtra.Data() ); }

  return plugin;
}

//______________________________________________________________________________
void LoadLibs ()
  {
  TString list_fj         = "CGAL fastjet siscone siscone_spherical fastjetplugins fastjettools fastjetcontribfragile";
  TString list_alicejets  = "PWGJEEMCALJetTasks";

  LoadLibList (list_fj);
  LoadLibList (list_alicejets);

  ::Info ( "EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS" );
  }

//______________________________________________________________________________
void LoadLibList ( const TString& list )
    {
    TObjArray* arr = list.Tokenize(" ");
    TObjString *objstr = NULL;

    TIter next(arr);
    while ( (objstr=(TObjString*)next()) )
        {
        TString module = objstr->GetString();
        module.Prepend("lib");
        if ( !LoadLibrary (module) ) { gApplication->Terminate(); }
        }
    delete arr;
    }

//______________________________________________________________________________
Bool_t LoadLibrary ( const TString& lib  )
    {
    Int_t result = -999 ;

    if ( !lib.Length() ) { ::Error ( "AnalysisCDFjets::LoadLibrary", "Empty module name" ); return kFALSE; }

    if ( lib.EndsWith ( ".so" ) ) { lib.Remove ( lib.Index (".so") ); }

    cout << "loading module :" << lib.Data() << " ... " ;

    result = gSystem->Load ( lib.Data() );
    if ( result < 0 ) { ::Error ( "EmcalJetCDF::LoadLibrary", "Could not load library %s", lib.Data() ); return kFALSE; }
    cout << result << endl;

    TString lib_in_list = "";
    lib_in_list = lib + ".so "; // blank after .so

    ListLibs      += lib_in_list;
    ListLibsExtra += lib_in_list;

    return kTRUE;
    }


// kate: indent-mode none; indent-width 2; replace-tabs on;

