// Main steering variables just below

#if ( !defined(__CINT__) && !defined(__CLING__) )
#include "Rtypes.h"
#include "TString.h"
#endif

const UInt_t  iNumFiles      = 1;            // number of files analyzed locally
const UInt_t  iNumEvents     = 100;     // number of events to be analyzed

const Bool_t  bDoChargedJets = kTRUE;
const Bool_t  bDoFullJets    = kTRUE;

const Bool_t  bDoSample = kTRUE;
const Bool_t  bDoCDF    = kFALSE;

TString     kGridDataSet             = "pp_lhc16q_aod";
Bool_t      kSkipTerminate           = kTRUE;          // Do not call Terminate

#if ( !defined(__CINT__) && !defined(__CLING__) )
// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>

// ROOT includes
#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"

#include "TSystem.h"
#include "TApplication.h"
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
#include "AliEmcalCorrectionTask.h"

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

// Forward declarations
void LoadLibs ();
void LoadLibList ( const TString& list );
Bool_t LoadLibrary ( const TString& lib );

void LoadMacros();
AliAnalysisAlien *CreateAlienHandler ( const char *gridMode );
TString GetInputDataPath ( const TString& file_list);
TString GetPeriod ( const TString& file_path);
TString GetPass ( const TString& file_path);
const char* pwd() { return gSystem->WorkingDirectory(); }
const char* basedir() { return gSystem->BaseName( pwd() ); }
//############################################################

TString     ListLibs      = "";
TString     ListLibsExtra = "";

const AliAnalysisTaskEmcal::EDataType_t kAod = AliAnalysisTaskEmcal::kAOD;
const AliAnalysisTaskEmcal::EDataType_t kEsd = AliAnalysisTaskEmcal::kESD;

const AliEmcalPhysicsSelection::EOfflineEmcalTypes  kEMCAL  = AliEmcalPhysicsSelection::kEmcalOk;
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

//______________________________________________________________________________
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//        AliEN plugin variables
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//______________________________________________________________________________
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

TString     kWorkDir                 = "emcalcdf";    // AliEn work dir; relative to AliEn $HOME
TString     kTrainName               = "sevjets";     // *CHANGE ME* (no blancs or special characters)
TString     kJobTag                  = "cdfjet";   // *CHANGE ME*

TString     kPluginExecutableCommand =
                                      "aliroot -l -b -q";
//                                       "root.exe -l -b -q";

Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

TString     kAPIVersion              = "V1.1x";
TString     kRootVersion             = "v5-34-30-alice8-5";
TString     kAliRootVersion          = "v5-09-20a-1";
TString     kAliPhysicsVersion       = "vAN-20171212-1";

TString     kPackage1                = "boost::v1.59.0-7";
TString     kPackage2                = "cgal::v4.6.3-10";
TString     kPackage3                = "fastjet::v3.2.1_1.024-alice1-6";
TString     kPackage4                = "GEANT3::v2-4-9";
// TString     kPackage5                = "jemalloc::v3.6.0";

Int_t       kGridFilesPerJob         = 20;             // Maximum number of files per job (gives size of AOD)
Int_t       kGridMaxMergeFiles       = 100;            // Number of files merged in a chunk grid run range
Int_t       kMaxInitFailed           = 10 ;            // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

Int_t       kTTL                     = 28800 ;         // Time To Live
Int_t       kMergeTTL                = 1800 ;          // Time To Live merging

TString     kGridOutdir              = "output";          // AliEn output directory. If blank will become output_<kTrainName>
TString     kGridSubWorkDir          = "";             // sub working directory not to confuse different run xmls
TString     kGridExtraAliendirLevel  = "";             // sub working directory not to confuse different run xmls

// FILES USED IN MACRO
TString     kCommonOutputFileName    = "AnalysisResults.root";

// == grid plugin files rules
TString     kGridExtraFiles          = ""; // LIBRARIES files that will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

TString     kAlirootMode             = "ALIROOT";     // STEERBase,ESD,AOD,ANALYSIS,ANALYSISalice (default aliroot mode)
//  alirootMode="ALIROOT";   // $ALICE_ROOT/macros/loadlibs.C
//  alirootMode="REC";       // $ALICE_ROOT/macros/loadlibsrec.C
//  alirootMode="SIM";       // $ALICE_ROOT/macros/loadlibssim.C
//  alirootMode="TRAIN";     // $ALICE_ROOT/macros/loadlibstrain.C (not working yet)
//  alirootMode="CUSTOM";    // nothing is loaded, but aliroot variables are set (not working yet)

//--------------------
//   PROOF SETTINGS
//--------------------
TString kAAF        =
//                         "asevcenc@alice-caf.cern.ch" ;
//    "asevcenc@skaf.saske.sk";
      "";

Int_t   kProofReset = 0  ; Int_t   kWorkers    = 20 ; Int_t   kCores      = 8  ;
//------------------------------------------------------------------------------

using namespace std;

//______________________________________________________________________________
AliAnalysisManager* runEMCalJetSampleTask(
    const char*   cDataType      = "AOD",                    // set the analysis type, AOD or ESD
    Int_t         iStartAnalysis = 1,                        // 1 - local analysis; 2- grid plugin
    const char*   cGridMode      = "test"
) {

  const UInt_t  kPhysSel       = kMB;            // physics selection
  const char*   cLocalFiles    = "data.txt";     // set the local list file
  const char*   cTaskName      = "CDFJets";      // sets name of analysis manager

  // Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);
  AliAnalysisAlien* plugin = NULL;

  if (iStartAnalysis == 2) {  // start grid analysis
    plugin = CreateAlienHandler(cGridMode);
    if ( !plugin ) { ::Error ( "runEMCalJetSampleTask.C - StartGridAnalysis", "plugin invalid" ); return NULL; }
    pMgr->SetGridHandler(plugin);

    gROOT->LoadMacro("InputData.C");
    InputData(kGridDataSet);
    }

  TString sRunPeriod, pass ;

  if (iStartAnalysis == 1)  {
    TString input_path = GetInputDataPath (cLocalFiles);
    sRunPeriod = GetPeriod (input_path);
    pass   = GetPass (input_path);
    }
  else
  if (iStartAnalysis == 2 ) {
    TString datadir = plugin->GetGridDataDir();
    sRunPeriod = GetPeriod (datadir);

    TString datapat = plugin->GetDataPattern();
    pass = GetPass(datapat).ReplaceAll("*","");
    }
  else
    { std::cout << "Analysis mode unknown : it should be 1 for local and 2 for GRID mode" << std::endl; return NULL; }

  sRunPeriod.ToLower();
  cout  << "Detected data period : " << sRunPeriod.Data() << endl;
  cout  << "Detected pass name : " << pass.Data() << endl;

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;
  Bool_t bIsRun2 = kFALSE;

  if (sRunPeriod.Length() == 6 && (sRunPeriod.BeginsWith("lhc15") || sRunPeriod.BeginsWith("lhc16") || sRunPeriod.BeginsWith("lhc17")) ) { bIsRun2 = kTRUE; }

  if (sRunPeriod == "lhc10h" || sRunPeriod == "lhc11h" || sRunPeriod == "lhc15o") { iBeamType = AliAnalysisTaskEmcal::kAA; }

  if (sRunPeriod == "lhc12g" || sRunPeriod == "lhc13b" || sRunPeriod == "lhc13c" || sRunPeriod == "lhc13d" || sRunPeriod == "lhc13e" || sRunPeriod == "lhc13f" ||
      sRunPeriod == "lhc16q" || sRunPeriod == "lhc16r" || sRunPeriod == "lhc16s" || sRunPeriod == "lhc16t" )
    { iBeamType = AliAnalysisTaskEmcal::kpA; }

//   if (iBeamType != AliAnalysisTaskEmcal::kpp) { std::cout << "Comment this line if you want to run on Pb" << std::endl; gApplication->Terminate();}

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(sRunPeriod);
  Printf("Default track cut period set to: %s", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  LoadLibs();
  namespace CDF = NS_AliAnalysisTaskEmcalJetCDF;

  Bool_t   bDoEmcalCorrections  = kFALSE;
  if (bDoFullJets) { bDoEmcalCorrections  = kTRUE; }

  const Double_t kHadCorrF            = 2.;

  AliAnalysisTaskEmcal::EDataType_t iDataType = AliAnalysisTaskEmcal::kAOD;
  if (!std::strcmp(cDataType, "ESD")) { iDataType = AliAnalysisTaskEmcal::kESD; }

  Printf("%s analysis chosen.", cDataType);

  TString sLocalFiles(cLocalFiles);
  if (iStartAnalysis == 1) {
    if (sLocalFiles == "") { Printf("You need to provide the list of local files!"); return 0; }
    Printf("Setting local analysis for %d files from list %s, max events = %d", iNumFiles, sLocalFiles.Data(), iNumEvents);
    }

  LoadMacros();

  AliAODInputHandler* pAODHandler = NULL;
  AliESDInputHandler* pESDHandler = NULL;

  if (iDataType == AliAnalysisTaskEmcal::kESD)
    { pESDHandler = AddESDHandler(); }
  else
    { pAODHandler = AddAODHandler(); }

  // CDBconnect task
  AliTaskCDBconnect *taskCDB = AddTaskCDBconnect();
  taskCDB->SetFallBackToRaw(kTRUE);
  if (!taskCDB) { std::cout << "--------->>>>  taskCDB :: could not connect!!!! CHECK CVMFS" << std::endl; return NULL;}

  // Physics selection task
  if (iDataType == AliAnalysisTaskEmcal::kESD) { AliPhysicsSelectionTask* pPhysSelTask = AddTaskPhysicsSelection(); }

  // Centrality task
  if ( (iDataType == AliAnalysisTaskEmcal::kESD) && (iBeamType != AliAnalysisTaskEmcal::kpp) ) {
    AliCentralitySelectionTask *pCentralityTask = AddTaskCentrality(kTRUE);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  if (bDoEmcalCorrections) {
    // Configuration of the Correction Task is handled via a YAML file, which is setup below
    // NOTE: Calling this function is equivalent to the AddTask macro, just the function is defined in the class.
    //       The AddTask macro still exists for use on the LEGO train
    AliEmcalCorrectionTask * correctionTask = AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask();
    correctionTask->SelectCollisionCandidates(kPhysSel);
    correctionTask->SetForceBeamType((AliEmcalCorrectionTask::BeamType)iBeamType);

    // Configure and initialize
    correctionTask->SetUserConfigurationFilename("PWGJE_SEV_Config.yaml");
    correctionTask->Initialize();
    }

  // Background
  TString sRhoChName;
  TString sRhoFuName;
  if ( iBeamType != AliAnalysisTaskEmcal::kpp ) {
    sRhoChName = "Rho";
    sRhoFuName = "Rho_Scaled";

    AliEmcalJetTask* pKtChJetTask = AddTaskEmcalJet("usedefault", "", kt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 0., kFALSE, kFALSE);
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
  AliAnalysisTaskEmcalJetSample* sampleTask = NULL;
  if (bDoSample) {
    sampleTask = AliAnalysisTaskEmcalJetSample::AddTaskEmcalJetSample("usedefault", "usedefault", "usedefault", "new");
    sampleTask->SelectCollisionCandidates(kSel_tasks);

    sampleTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
    sampleTask->SetHistoBins(600, 0, 300);
    sampleTask->GetClusterContainer(0)->SetClusECut(0.);
    sampleTask->GetClusterContainer(0)->SetClusPtCut(0.);
    sampleTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
    sampleTask->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
    sampleTask->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }

  //###   CDF task
  AliAnalysisTaskEmcalJetCDF* anaTaskCDF = NULL;
  if (bDoCDF) {
    anaTaskCDF = CDF::AddTaskEmcalJetCDF ( "usedefault", "usedefault", "usedefault", "CDF" );
    anaTaskCDF->SelectCollisionCandidates(kSel_tasks);

    anaTaskCDF->GetParticleContainer(0)->SetParticlePtCut(0.15);
    anaTaskCDF->GetClusterContainer(0)->SetClusECut(0.);
    anaTaskCDF->GetClusterContainer(0)->SetClusPtCut(0.);
    anaTaskCDF->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
    anaTaskCDF->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
    anaTaskCDF->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }

  if (bDoFullJets) {
    if (bDoSample) {
      AliJetContainer* jetCont02full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
      AliJetContainer* jetCont04full_sample = sampleTask->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");

      if (iBeamType != AliAnalysisTaskEmcal::kpp) {
        jetCont02full_sample->SetRhoName(sRhoFuName);
        jetCont02full_sample->SetPercAreaCut(0.6);
        jetCont04full_sample->SetRhoName(sRhoFuName);
        jetCont04full_sample->SetPercAreaCut(0.6);
        }
      }

    if (bDoCDF) {
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
    }

  if (bDoChargedJets) {
    if (bDoSample) {
      AliJetContainer* jetCont02chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
      AliJetContainer* jetCont04chg_sample = sampleTask->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");

      if (iBeamType != AliAnalysisTaskEmcal::kpp) {
        jetCont02chg_sample->SetRhoName(sRhoChName);
        jetCont02chg_sample->SetPercAreaCut(0.6);
        jetCont04chg_sample->SetRhoName(sRhoChName);
        jetCont04chg_sample->SetPercAreaCut(0.6);
        }
      }

    if (bDoCDF) {
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

  if (!pMgr->InitAnalysis()) { return NULL; }
  pMgr->PrintStatus();

//  pMgr->SetUseProgressBar(kTRUE, 250);

  TFile* pOutFile = new TFile("train.root","RECREATE");
  pOutFile->cd();
  pMgr->Write();
  pOutFile->Close();
  delete pOutFile;

  if ( (iStartAnalysis != 1) || (iStartAnalysis != 2) ) { iStartAnalysis = 1; }

  if (iStartAnalysis == 1) { // start local analysis
    TChain* pChain = 0;
    if (iDataType == AliAnalysisTaskEmcal::kESD)
      {
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateESDChain.C");
      pChain = CreateESDChain(sLocalFiles.Data(), iNumFiles, 0, kFALSE);
      }
    else
      {
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C");
      pChain = CreateAODChain(sLocalFiles.Data(), iNumFiles, 0, kFALSE);
      }

    // start analysis
    Printf("Starting Analysis...");
    pMgr->StartAnalysis("local", pChain, iNumEvents);
    }

  if (iStartAnalysis == 2) {  // start grid analysis
    // start analysis
    Printf("Starting GRID Analysis...");
    if (!std::strcmp(cGridMode, "test")) { pMgr->SetDebugLevel(0); }
    pMgr->StartAnalysis("grid", iNumEvents);
    }

cout << "END of runEMCalJetSampleTask.C" << endl;
return pMgr;
}

//######################################################################################################################################
AliAnalysisAlien* CreateAlienHandler(const char* gridMode ) {
  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if ( !plugin ) { cout << "!!! -->> alien handler could not be created <<-- !!!" << endl; return NULL;}

  TString tmpName = basedir();

  TString macroName(""); macroName = Form("%s.C", kJobTag.Data());
  TString execName("");  execName = Form("%s.sh", kJobTag.Data());
  TString jdlName("");   jdlName = Form("%s.jdl", kJobTag.Data());

  // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
  plugin->SetRunMode(gridMode);

  // Job tag
  plugin->SetJobTag ( tmpName.Data() );

  // AliEn directory containing the input packages
  plugin->SetGridWorkingDir( tmpName.Data() );

  // Declare alien output directory. Relative to working directory.
  plugin->SetGridOutputDir( kGridOutdir.Data() ); // In this case will be $HOME/work/output

  // Optionally modify the executable name (default analysis.sh)
  plugin->SetExecutable(execName.Data());

  // Optionally set a name for the generated analysis macro (default MyAnalysis.C)
  plugin->SetAnalysisMacro(macroName.Data());

  // Optionally modify the name of the generated JDL (default analysis.jdl)
  plugin->SetJDLName(jdlName.Data());

  // Use the output files connected to output containers from the analysis manager
  // rather than the files defined by SetOutputFiles
  plugin->SetDefaultOutputs(kTRUE);

  // Here you can set the (Ali)PHYSICS version you want to use
  plugin->SetAliPhysicsVersion ( kAliPhysicsVersion.Data() );

  //********************   PLUGIN OPTIONS   ************************

  // min (nr,4) replicas in grid storage
  plugin->SetNumberOfReplicas(2);

  // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
  plugin->SetNtestFiles ( iNumFiles );

  if ( kPluginUseProductionMode ) { plugin->SetProductionMode(); }

  // Use xrootd tweaks to reduce timeouts in file access
  plugin->SetFastReadOption ( kFALSE );

  // Maximum initial consecutive subjobs accepted to fail.
//   plugin->SetMaxInitFailed ( kMaxInitFailed );

  // Optionally resubmit threshold.
  // plugin->SetMasterResubmitThreshold(90);

  // Number of runs per masterjob
//   plugin->SetNrunsPerMaster(1);

  // exit from aliensh after submmiting job
  plugin->SetDropToShell ( kTRUE );

  // Overwrite existing files if any
  plugin->SetOverwriteMode();

  // write the output to subdirs named after run number
//   plugin->SetOutputToRunNo(1);

  // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
  plugin->SetSplitMaxInputFileNumber ( kGridFilesPerJob );

  // Optionally set time to live
  plugin->SetTTL(kTTL);

  // Optionally set input format (default xml-single)
  plugin->SetInputFormat("xml-single");

  // Optionally modify job price (default 1)
  plugin->SetPrice(1);

  // We split per SE or file
  plugin->SetSplitMode("se");

  // Enable merging via automatic JDL
  plugin->SetMergeViaJDL(kTRUE);

  // Maximum number of merging stages
//   plugin->SetMaxMergeStages(3);

  // Optionally set maximum number of files merged in a chunk (default 100, put 0 to ignore)
  plugin->SetMaxMergeFiles ( kGridMaxMergeFiles );

  // Maximum number of files to be merged in one chunk
//   plugin->SetOneStageMerging(kFALSE);

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
Bool_t LoadLibrary ( const TString& lib_name  )
    {
    TString lib (lib_name);
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

//######################################################################################################################################
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
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C");
}

//______________________________________________________________________________
TString GetInputDataPath ( const TString& file_list) {
  std::string line_str;
  TString line = line_str.c_str();

  if ( gSystem->AccessPathName ( file_list.Data() ) ) {
    std::cout << "File not found: " << file_list.Data() << std::endl; return line; }

  // Open the file
  ifstream in; in.open ( file_list.Data() );
  Int_t count = 0;
  
  while ( in.good() ) {
    in >> line_str;
    line = line_str.c_str();
    if ( line.IsNull() || line.BeginsWith ( "#" ) ) { continue; }
    if ( count == 1 ) { break; }  // look only over first file;
    count++;
    }
  in.close();
  line = line_str.c_str();
  return line;
}

//______________________________________________________________________________
TString GetPeriod ( const TString& file_path) {
  TString period = "";

  if (!file_path.IsNull()) {
    // split string in tokens (libs)
    TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
    TIter next_str(tokens_list);
    TObjString* token = NULL;
    while ((token=(TObjString*)next_str())) {
      TString token_str = token->GetString(); token_str.ToLower();
      if ( token_str.Contains("lhc") ) { period = token_str; break; }
      }
    delete tokens_list;
    }
  return period;
}

//______________________________________________________________________________
TString GetPass ( const TString& file_path) {
  TString pass = "";

  if (!file_path.IsNull()) {
    // split string in tokens (libs)
    TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
    TIter next_str(tokens_list);
    TObjString* token=0;
    while ((token=(TObjString*)next_str())) {
      TString token_str = token->GetString(); token_str.ToLower();
      if ( token_str.Contains("pass") ) { pass = token_str; break; }
      }
    delete tokens_list;
    }

  return pass;
}

//______________________________________________________________________________
Bool_t PeriodIsMC ( const TString& per_str ) {
  TString period = per_str;
  if (!period.IsNull()) {
    period.ToLower();
    if ( period.BeginsWith("lhc") && (period.Length() > 6) ) {return kTRUE;}
  }
  return kFALSE;
}

// kate: indent-mode none; indent-width 2; replace-tabs on;

