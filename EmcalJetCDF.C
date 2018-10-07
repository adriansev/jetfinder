// Defined defaults
unsigned int iNumFiles      = 1;       // number of files analyzed locally
unsigned int iNumEvents     = 999999999;     // number of events to be analyzed

bool  bDoChargedJets = true;
bool  bDoFullJets    = false;

bool  bDoSample = false;
bool  bDoCDF    = true;

unsigned int       kGridFilesPerJob         = 20;             // Maximum number of files per job (gives size of AOD)
unsigned int       kGridMaxMergeFiles       = 100;            // Number of files merged in a chunk grid run range
unsigned int       kMaxInitFailed           = 10 ;            // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.
unsigned int       kTTL                     = 64800 ;         // Time To Live

// Tell ROOT where to find AliRoot headers
R__ADD_INCLUDE_PATH($ALICE_ROOT/include)
// Tell ROOT where to find AliPhysics headers
R__ADD_INCLUDE_PATH($ALICE_PHYSICS/include)

// Main steering variables just below
#if !defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>

// ROOT includes
#include "Rtypes.h"
#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TRegexp.h"
#include "TInterpreter.h"

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

// AliROOT includes
#include "AliAnalysisTask.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisDataContainer.h"

#include "AliAnalysisTaskEmcal.h"
#include "AliAnalysisTaskEmcalJet.h"

#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"

#include "AliVCluster.h"
#include "AliVEvent.h"

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
#include "../OADB/macros/AddTaskPhysicsSelection.C"
#include "../OADB/macros/AddTaskCentrality.C"
#include "../PWGPP/PilotTrain/AddTaskCDBconnect.C"
#include "../PWG/EMCAL/macros/CreateAODChain.C"
#include "../PWG/EMCAL/macros/CreateESDChain.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C"


#include "OADB/macros/AddTaskPhysicsSelection.C"
#include "OADB/macros/AddTaskCentrality.C"
#include "PWGPP/PilotTrain/AddTaskCDBconnect.C"
#include "PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C"
#include "PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C"

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
class AliAnalysisTaskRho;
class AliAnalysisTaskEmcalJetSample;
class AliAnalysisGrid;
class AliAnalysisAlien;

//############################################################
// Forward declarations
void LoadLibs ();
void LoadLibList ( const TString& list );
Bool_t LoadLibrary ( const TString& lib );

void LoadMacros();
AliAnalysisAlien *CreateAlienHandler ( const char *gridMode );
TString GetInputDataPath ( const TString& file_list);
TString GetPeriod ( const TString& file_path);
TString GetPass   ( const TString& file_path);
const char* pwd()     { return gSystem->WorkingDirectory() ;}
const char* basedir() { return gSystem->BaseName( pwd() )  ;}
//############################################################

const AliEmcalPhysicsSelection::EOfflineEmcalTypes mykEMCAL      = AliEmcalPhysicsSelection::kEmcalOk;
const AliVEvent::EOfflineTriggerTypes  mykEMC             = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE | AliVEvent::kEMCEGA);
const AliVEvent::EOfflineTriggerTypes  mykEMC_noGA        = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE);

const AliVEvent::EOfflineTriggerTypes  mykMB              = AliVEvent::kAnyINT;
const AliVEvent::EOfflineTriggerTypes  mykMB_central      = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_semicentral  = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kSemiCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_mostcentral  = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral);

//##################################################
AliAnalysisManager* EmcalJetCDF (
    const char*   cDataType      = "AOD",         // set the analysis type, AOD or ESD
    const char*   cRunPeriod     = "LHC11d",     // set the run period
    const char*   cLocalFiles    = "data.txt",   // set the local list file
    AliVEvent::EOfflineTriggerTypes phys_sel_chg   = mykMB, // "mykEMC_noGA",  // physics selection
    AliVEvent::EOfflineTriggerTypes phys_sel_full  = mykMB, // "mykEMC_noGA",  // physics selection
    Int_t         iStartAnalysis = 1,
    const char*   cGridMode      = "test",
    const char*   cTaskName      = "CDFJets"     // sets name of analysis manager
) {
// ######   DEBUG    ######
Int_t           debug              =  4 ; // kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
UInt_t          mgr_debug          =  4 ; // AliAnalysisManager debug level
UInt_t          kUseSysInfo        =  0 ; // activate debugging

if ( debug == 0 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal   ); }
if ( debug == 1 ) { AliLog::SetGlobalLogLevel ( AliLog::kError   ); }
if ( debug == 2 ) { AliLog::SetGlobalLogLevel ( AliLog::kWarning ); }
if ( debug == 3 ) { AliLog::SetGlobalLogLevel ( AliLog::kInfo    ); }
if ( debug >= 4 ) { AliLog::SetGlobalLogLevel ( AliLog::kDebug   ); }

//##################################################
//        AliEN plugin variables
//##################################################
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

TString     kWorkDir                 = "emcalcdf";    // AliEn work dir; relative to AliEn $HOME
TString     kTrainName               = "sevjets";     // *CHANGE ME* (no blancs or special characters)
TString     kJobTag                  = "cdfjet";   // *CHANGE ME*

TString     kPluginExecutableCommand =
                                      "aliroot -l -b -q";
//                                       "root.exe -l -b -q";

TString     kAliPhysicsVersion       = "vAN-20181002-1";

TString     kGridOutdir              = "output";          // AliEn output directory. If blank will become output_<kTrainName>
TString     kGridSubWorkDir          = "";             // sub working directory not to confuse different run xmls
TString     kGridExtraAliendirLevel  = "";             // sub working directory not to confuse different run xmls

// FILES USED IN MACRO
TString     kCommonOutputFileName    = "AnalysisResults.root";

// == grid plugin files rules
TString     kGridExtraFiles          = ""; // extra filesthat will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

TString     kAlirootMode             = "ALIROOT";     // STEERBase,ESD,AOD,ANALYSIS,ANALYSISalice (default aliroot mode)
//--------------------
//   PROOF SETTINGS
//--------------------
TString kAAF        = "";
Int_t   kProofReset = 0;
Int_t   kWorkers    = 20;
Int_t   kCores      = 8  ;
//------------------------------------------------------------------------------

const AliAnalysisTaskEmcal::EDataType_t kAod = AliAnalysisTaskEmcal::kAOD;
const AliAnalysisTaskEmcal::EDataType_t kEsd = AliAnalysisTaskEmcal::kESD;

const AliJetContainer::EJetType_t  fulljet = AliJetContainer::kFullJet;
const AliJetContainer::EJetType_t   chgjet = AliJetContainer::kChargedJet;

// kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kDCALonly, kDCALonlyfid, kPHOS, kPHOSfid, kUser
const AliEmcalJet::JetAcceptanceType acc_chgjets  = AliEmcalJet::kTPCfid;
const AliEmcalJet::JetAcceptanceType acc_fulljets = AliEmcalJet::kEMCALfid;

const AliJetContainer::EJetAlgo_t     antikt = AliJetContainer::antikt_algorithm;
const AliJetContainer::EJetAlgo_t         kt = AliJetContainer::kt_algorithm;

const AliJetContainer::ERecoScheme_t  recomb = AliJetContainer::pt_scheme;

// label of dataset from InputData.C
TString kGridDataSet ("pp_lhc16q_aod");

// data source name
TString kDataSource (cLocalFiles);

// EMCAL corrections task configuration file
TString EMCALcfg ("PWGJE_SEV_Config.yaml");
//("alien:///alice/cern.ch/user/m/mfasel/EMCALCorrectionConfig/ConfigDataPWGJEhighClusterThresholds.yaml");

if ( !kGridExtraFiles.IsNull() ) {kGridExtraFiles += " ";}
kGridExtraFiles += EMCALcfg;

//############################################################
// SETUP OF TRIGGERS
// Int_t kPhysSel = AliVEvent::kINT7;
// Int_t kPhysSelInt7 = AliVEvent::kAny;//kINT7; //used when running on triggered periods
// Int_t kPhysSelAll = AliVEvent::kINT7+AliVEvent::kEMCEJE+AliVEvent::kEMCEGA;
// Int_t kPhysSelAny = AliVEvent::kAny;

AliVEvent::EOfflineTriggerTypes kPhysSel   = mykMB; //AliVEvent::kAnyINT; // physics selection
AliVEvent::EOfflineTriggerTypes kSel_tasks = mykMB;

AliVEvent::EOfflineTriggerTypes kSel_chg   = phys_sel_chg;
AliVEvent::EOfflineTriggerTypes kSel_full  = phys_sel_full;

//############################################################
  // Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);
  pMgr->SetDebugLevel(mgr_debug);
  if ( kUseSysInfo > 0 ) { pMgr->SetNSysInfo ( kUseSysInfo ); }

  AliAnalysisAlien* plugin = NULL;

  if (iStartAnalysis == 2) {  // start grid analysis
    plugin = CreateAlienHandler(cGridMode, kJobTag.Data());
    if ( !plugin ) { ::Error ( "runEMCalJetSampleTask.C - StartGridAnalysis", "plugin invalid" ); return NULL; }
    pMgr->SetGridHandler(plugin);

    plugin->SetAliPhysicsVersion ( kAliPhysicsVersion.Data() ); // Here you can set the (Ali)PHYSICS version you want to use
    if ( kPluginUseProductionMode ) { plugin->SetProductionMode(); }

    gROOT->LoadMacro("InputData.C");
    InputData(kGridDataSet);
    }

  TString sRunPeriod (cRunPeriod);
  sRunPeriod.ToLower();

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;
  Bool_t bIsRun2 = kFALSE;

  if (sRunPeriod.Length() == 6 && (sRunPeriod.BeginsWith("lhc15") || sRunPeriod.BeginsWith("lhc16") || sRunPeriod.BeginsWith("lhc17") || sRunPeriod.BeginsWith("lhc18") ) ) { bIsRun2 = kTRUE; }

  if (sRunPeriod == "lhc10h" || sRunPeriod == "lhc11h" || sRunPeriod == "lhc15o" || sRunPeriod == "lhc17n" ) { iBeamType = AliAnalysisTaskEmcal::kAA; }

  if (sRunPeriod == "lhc12g" || sRunPeriod == "lhc13b" || sRunPeriod == "lhc13c" || sRunPeriod == "lhc13d" || sRunPeriod == "lhc13e" || sRunPeriod == "lhc13f" ||
      sRunPeriod == "lhc16q" || sRunPeriod == "lhc16r" || sRunPeriod == "lhc16s" || sRunPeriod == "lhc16t" )
    { iBeamType = AliAnalysisTaskEmcal::kpA; }

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(sRunPeriod);
  Printf("Default track cut period set to: %s", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  //Load needed libs
  TString     ListLibs      = ""; // string list of loaded libs
  TString     ListLibsExtra = ""; // string list of loaded extra libs

  LoadLibs( ListLibs, ListLibsExtra );
  namespace CDF = NS_AliAnalysisTaskEmcalJetCDF;

  Bool_t   bDoEmcalCorrections  = kFALSE;
  if (bDoFullJets) { bDoEmcalCorrections  = kTRUE; }

  Bool_t kIsAOD = kTRUE;
  AliAnalysisTaskEmcal::EDataType_t iDataType = AliAnalysisTaskEmcal::kAOD; Bool_t kIsAOD = kTRUE;
  if (!std::strcmp(cDataType, "ESD")) { iDataType = AliAnalysisTaskEmcal::kESD; kIsAOD = kFALSE; }

  Printf("%s analysis chosen.", cDataType);

  if (iStartAnalysis == 1) {
    if ( kDataSource.IsNull() ) { Printf("You need to provide the list of local files!"); return 0; }
    Printf("Setting local analysis for %d files from list %s, max events = %d", iNumFiles, kDataSource.Data(), iNumEvents);
    }

  #ifndef __CLING__
  LoadMacros();
  #endif

  AliAODInputHandler* pAODHandler = NULL;
  AliESDInputHandler* pESDHandler = NULL;

  if ( iDataType == AliAnalysisTaskEmcal::kAOD ) {
    pAODHandler = AliAnalysisTaskEmcal::AddAODHandler();
    }
  else {
    pESDHandler = AliAnalysisTaskEmcal::AddESDHandler();
    }

  // CDBconnect task
  AliTaskCDBconnect *taskCDB = AddTaskCDBconnect(); //AliTaskCDBconnect::AddTaskCDBconnect();//
  taskCDB->SetFallBackToRaw(kTRUE); // Needed for the moment as not all grid sites do support cvmfs OCDB for the moment
  if (!taskCDB) { std::cout << "--------->>>>  taskCDB :: could not connect!!!! CHECK CVMFS" << std::endl; return NULL;}

  // Physics selection task
  if (iDataType == AliAnalysisTaskEmcal::kESD) {
    // signature : (Bool_t mCAnalysisFlag = kFALSE, Bool_t applyPileupCuts = kFALSE, UInt_t deprecatedFlag2 = 0, Bool_t useSpecialOutput=kFALSE)
    AliPhysicsSelectionTask* pPhysSelTask = AddTaskPhysicsSelection();
    }

  // Centrality task
  if ( iBeamType != AliAnalysisTaskEmcal::kpp && bIsRun2 == kFALSE ) {
    //signature : (Bool_t fillHistos=kTRUE, Bool_t aod=kFALSE)
    AliCentralitySelectionTask *pCentralityTask = AddTaskCentrality(kTRUE, kIsAOD);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  // AliMultSelection
  if (bIsRun2 == kTRUE ) {
    // signature : ( Bool_t lCalibration = kFALSE, TString lExtraOptions = "", Int_t lNDebugEstimators = 1, const TString lMasterJobSessionFlag = "")
    AliMultSelectionTask* pMultSelTask = AliMultSelectionTask::AddTaskMultSelection();
    pMultSelTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  if (bDoEmcalCorrections) {
    // Configuration of the Correction Task is handled via a YAML file, which is setup below
    // signature : (TString suffix)
    AliEmcalCorrectionTask * correctionTask = AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask();
//     AliEmcalCorrectionTask * correctionTask = AliEmcalCorrectionTask::ConfigureEmcalCorrectionTaskOnLEGOTrain(suffix);
    correctionTask->SelectCollisionCandidates(kPhysSel);
    correctionTask->SetUseNewCentralityEstimation(bIsRun2);
    correctionTask->SetForceBeamType(static_cast<AliEmcalCorrectionTask::BeamType>(iBeamType));

    // Configure and initialize
    correctionTask->SetUserConfigurationFilename( EMCALcfg.Data() );
    //correctionTask->SetUserConfigurationFilename("alien:///alice/cern.ch/user/m/mfasel/EMCALCorrectionConfig/ConfigDataPWGJEhighClusterThresholds.yaml");
    correctionTask->Initialize(true);
    }

  // Background
  TString sRhoChName;
  TString sRhoFuName;
  if ( iBeamType != AliAnalysisTaskEmcal::kpp ) {
    sRhoChName = "Rho";
    sRhoFuName = "Rho_Scaled";

    AliEmcalJetTask* pKtChJetTask = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "", kt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 0., kFALSE, kFALSE);
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
    AliEmcalJetTask *pChJet02Task = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "", antikt, 0.2, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet02Task->SelectCollisionCandidates(kSel_chg);

    AliEmcalJetTask *pChJet04Task = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "", antikt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet04Task->SelectCollisionCandidates(kSel_chg);
    }

  // Full jet analysis
  if (bDoFullJets) {
    AliEmcalJetTask *pFuJet02Task = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.2, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet02Task->SelectCollisionCandidates(kSel_full);
    pFuJet02Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

    AliEmcalJetTask *pFuJet04Task = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "usedefault", antikt, 0.4, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet04Task->SelectCollisionCandidates(kSel_full);
    pFuJet04Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }

  // Sample task
  AliAnalysisTaskEmcalJetSample* sampleTaskchg = NULL;
  AliAnalysisTaskEmcalJetSample* sampleTaskfull = NULL;

  if (bDoSample  && bDoChargedJets) {
    sampleTaskchg = AliAnalysisTaskEmcalJetSample::AddTaskEmcalJetSample("usedefault", "", "", "SMPCHG");
    sampleTaskchg->GetParticleContainer(0)->SetParticlePtCut(0.15);
    sampleTaskchg->SetHistoBins(600, 0, 300);
    sampleTaskchg->SelectCollisionCandidates(kSel_chg);
    sampleTaskchg->SetDebugLevel(debug);
    }

  if (bDoSample  && bDoFullJets) {
    sampleTaskfull = AliAnalysisTaskEmcalJetSample::AddTaskEmcalJetSample("usedefault", "usedefault", "usedefault", "SMPFULL");
    sampleTaskfull->GetClusterContainer(0)->SetClusECut(0.);
    sampleTaskfull->GetClusterContainer(0)->SetClusPtCut(0.);
    sampleTaskfull->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
    sampleTaskfull->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
    sampleTaskfull->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    sampleTaskfull->GetParticleContainer(0)->SetParticlePtCut(0.15);
    sampleTaskfull->SetHistoBins(600, 0, 300);
    sampleTaskfull->SelectCollisionCandidates(kSel_full);
    sampleTaskfull->SetDebugLevel(debug);
    }

  //###   CDF task
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFchg = NULL;
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFfull = NULL;

  if (bDoCDF && bDoChargedJets) {
    anaTaskCDFchg = CDF::AddTaskEmcalJetCDF ( "usedefault", "", "", "CDFchg" );
    anaTaskCDFchg->GetParticleContainer(0)->SetParticlePtCut(0.15);
    anaTaskCDFchg->SetHistoBins(600, 0, 300);
    anaTaskCDFchg->SelectCollisionCandidates(kSel_chg);
    anaTaskCDFchg->SetDebugLevel(debug);
    }

  if (bDoCDF && bDoFullJets) {
    anaTaskCDFfull = CDF::AddTaskEmcalJetCDF ( "usedefault", "usedefault", "usedefault", "CDFfull" );
    anaTaskCDFfull->GetClusterContainer(0)->SetClusECut(0.);
    anaTaskCDFfull->GetClusterContainer(0)->SetClusPtCut(0.);
    anaTaskCDFfull->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.);
    anaTaskCDFfull->GetClusterContainer(0)->SetClusHadCorrEnergyCut(0.30);
    anaTaskCDFfull->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    anaTaskCDFfull->GetParticleContainer(0)->SetParticlePtCut(0.15);
    anaTaskCDFfull->SetHistoBins(600, 0, 300);
    anaTaskCDFfull->SelectCollisionCandidates(kSel_full);
    anaTaskCDFchg->SetDebugLevel(debug);
    }


  if (bDoFullJets) {
    if (bDoSample) {
      AliJetContainer* jetCont02full_sample = sampleTaskfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
      AliJetContainer* jetCont04full_sample = sampleTaskfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");

      if (iBeamType != AliAnalysisTaskEmcal::kpp) {
        jetCont02full_sample->SetRhoName(sRhoFuName);
        jetCont02full_sample->SetPercAreaCut(0.6);
        jetCont04full_sample->SetRhoName(sRhoFuName);
        jetCont04full_sample->SetPercAreaCut(0.6);
        }
      }

    if (bDoCDF) {
      AliJetContainer* jetcont_full = NULL;
      for ( Float_t fi = 0 ; fi<=95 ; fi+=5) {
        // FULL JETS 0.2
        jetcont_full  = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
        CDF::jetContSetParams (jetcont_full,    fi,   fi+5, 0, 2);

        // FULL JETS 0.4
        jetcont_full  = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");
        CDF::jetContSetParams (jetcont_full,    fi,   fi+5, 0, 2);
        }

      AliJetContainer* jetCont02full_cdf_bin21 = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetCont02full_cdf_bin21, 100., 500., 0, 2);

      AliJetContainer* jetCont02full_cdf_all   = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetCont02full_cdf_all,     1., 500., 0, 2);

      AliJetContainer* jetCont04full_cdf_bin21 = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetCont04full_cdf_bin21, 100., 500., 0, 2);

      AliJetContainer* jetCont04full_cdf_all   = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetCont04full_cdf_all,     1., 500., 0, 2);

      AliJetContainer* jetcont_full = NULL;
      }
    }

  if (bDoChargedJets) {
    if (bDoSample) {
      AliJetContainer* jetCont02chg_sample = sampleTaskchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
      AliJetContainer* jetCont04chg_sample = sampleTaskchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");

      if (iBeamType != AliAnalysisTaskEmcal::kpp) {
        jetCont02chg_sample->SetRhoName(sRhoChName);
        jetCont02chg_sample->SetPercAreaCut(0.6);
        jetCont04chg_sample->SetRhoName(sRhoChName);
        jetCont04chg_sample->SetPercAreaCut(0.6);
        }
      }

    if (bDoCDF) {
      AliJetContainer* jetcont_chg = NULL;
      for ( Float_t fi = 0 ; fi<=95 ; fi+=5) {
        // CHG JETS 0.2
        jetcont_chg  = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
        CDF::jetContSetParams (jetcont_chg,    fi,   fi+5, 0, 0);

        // CHG JETS 0.4
        jetcont_chg  = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");
        CDF::jetContSetParams (jetcont_chg,    fi,   fi+5, 0, 0);
        }

      AliJetContainer* jetCont02chg_cdf_bin21 = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetCont02chg_cdf_bin21, 100., 500., 0, 0);

      AliJetContainer* jetCont02chg_cdf_all   = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetCont02chg_cdf_all,     1., 500., 0, 0);

      AliJetContainer* jetCont04chg_cdf_bin21 = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetCont04chg_cdf_bin21, 100., 500., 0, 0);

      AliJetContainer* jetCont04chg_cdf_all   = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetCont04chg_cdf_all,     1., 500., 0, 0);

      AliJetContainer* jetcont_chg = NULL;
      }
    }

  TObjArray *pTopTasks = pMgr->GetTasks();
  for (Int_t i = 0; i < pTopTasks->GetEntries(); ++i) {
    AliAnalysisTaskSE *pTask = dynamic_cast<AliAnalysisTaskSE*>(pTopTasks->At(i));
    if (!pTask) { continue; }
    if (pTask->InheritsFrom("AliAnalysisTaskEmcal")) {
      AliAnalysisTaskEmcal *pTaskEmcal = static_cast<AliAnalysisTaskEmcal*>(pTask);
      Printf("Setting beam type %d for task %s", iBeamType, pTaskEmcal->GetName());
      pTaskEmcal->SetForceBeamType(iBeamType);
      }
    }

// enable class level debugging for these classes
if ( debug > 2 ) {
//   pMgr->AddClassDebug("AliJetContainer", 100);
//   pMgr->AddClassDebug("AliEmcalJetTask", 100);
  if (bDoCDF)    { pMgr->AddClassDebug("AliAnalysisTaskEmcalJetCDF", 100); }
  if (bDoSample) { pMgr->AddClassDebug("AliAnalysisTaskEmcalJetSample", 100); }
  }

  if (!pMgr->InitAnalysis()) { return NULL; }
  cout << "##-->> Initialising Analysis :: Status :" << endl;
  pMgr->PrintStatus();

//   pMgr->SetUseProgressBar(kTRUE, 100);

  TFile* pOutFile = new TFile("train.root","RECREATE");
  pOutFile->cd();
  pMgr->Write();
  pOutFile->Close();
  delete pOutFile;

  // task profiling
  if ( kUseSysInfo > 0 ) { for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask (i); } }


  if (iStartAnalysis == 1) { // start local analysis
    TChain* pChain = NULL;

    if (iDataType == AliAnalysisTaskEmcal::kAOD) {
      #ifdef __CLING__
      std::stringstream aodChain;
      aodChain << ".x " << gSystem->Getenv("ALICE_PHYSICS") <<  "/PWG/EMCAL/macros/CreateAODChain.C(";
      aodChain << "\"" << kDataSource.Data() << "\", ";
      aodChain << iNumEvents << ", ";
      aodChain << 0 << ", ";
      aodChain << std::boolalpha << kFALSE << ");";
      pChain = reinterpret_cast<TChain *>(gROOT->ProcessLine(aodChain.str().c_str()));
      #else
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C");
      pChain = CreateAODChain(kDataSource.Data(), iNumFiles, 0, kFALSE);
      #endif
    }
    else {
      #ifdef __CLING__
      std::stringstream esdChain;
      esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS") <<  "/PWG/EMCAL/macros/CreateESDChain.C(";
      esdChain << "\"" << kDataSource.Data() << "\", ";
      esdChain << iNumEvents << ", ";
      esdChain << 0 << ", ";
      esdChain << std::boolalpha << kFALSE << ");";
      pChain = reinterpret_cast<TChain *>(gROOT->ProcessLine(esdChain.str().c_str()));
      #else
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateESDChain.C");
      pChain = CreateESDChain(kDataSource.Data(), iNumFiles, 0, kFALSE);
      #endif
    }

    // start analysis
    Printf("Starting Analysis...");
    pMgr->StartAnalysis("local", pChain, iNumEvents);
    }

  if (iStartAnalysis == 2) {  // start grid analysis
    // start analysis
    Printf("Starting GRID Analysis...");

    ListLibs += kGridExtraFiles;
    if ( ListLibs.Length() )       { plugin->SetAdditionalLibs     ( ListLibs.Data() ); }
    if ( ListLibsExtra.Length() )  { plugin->SetAdditionalRootLibs ( ListLibsExtra.Data() ); }

    if (!std::strcmp(cGridMode, "test")) { pMgr->SetDebugLevel(0); }
//     pMgr->StartAnalysis("grid", iNumEvents);
    plugin->StartAnalysis(iNumEvents);
    }

cout << "END of EmcalJetCDF.C" << endl;
return pMgr;
}

//######################################################################################################################################
AliAnalysisAlien* CreateAlienHandler(const char* gridMode, const char* tag) {
  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if ( !plugin ) { cout << "!!! -->> alien handler could not be created <<-- !!!" << endl; return NULL;}

  TString tmpName = basedir();

  TString macroName(""); macroName = Form("%s.C", tag);
  TString execName("");  execName = Form("%s.sh", tag);
  TString jdlName("");   jdlName = Form("%s.jdl", tag);

  // Set run mode.  Can be "full", "test", "offline", "submit" or "merge"
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

  //********************   PLUGIN OPTIONS   ************************
  // min (nr,4) replicas in grid storage
  plugin->SetNumberOfReplicas(2);

  if (std::strcmp(gridMode, "test")) {
    plugin->SetCheckCopy(kFALSE);
    plugin->SetNtestFiles ( iNumFiles );
    plugin->SetTTL(kTTL);
    }
  else {
    // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
    plugin->SetNtestFiles ( kGridFilesPerJob );

    // Optionally set time to live
    plugin->SetTTL(172800); // 48h ttl for test mode
    }

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

  // Optionally set input format (default xml-single)
  plugin->SetInputFormat("xml-single");

  // Optionally modify job price (default 1)
  plugin->SetPrice(1);

  // We split per SE or file
  plugin->SetSplitMode("se");

  // MERGING
  // Enable merging via automatic JDL
  plugin->SetMergeViaJDL(kTRUE);

  // Maximum number of files to be merged in one chunk
  plugin->SetOneStageMerging(kFALSE);

  // Maximum number of merging stages
  plugin->SetMaxMergeStages(2);

  // Optionally set maximum number of files merged in a chunk (default 100, put 0 to ignore)
  //plugin->SetMaxMergeFiles ( kGridMaxMergeFiles );

  return plugin;
}

//##################################################
void LoadLibs (TString& listlibs, TString& listlibsextra) {
  TString list_fj         = "CGAL fastjet siscone siscone_spherical fastjetplugins fastjettools fastjetcontribfragile";
  TString list_alicejets  = "PWGJEEMCALJetTasks";

  LoadLibList (list_fj, listlibs, listlibsextra);
  LoadLibList (list_alicejets, listlibs, listlibsextra);

  ::Info ( "EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS" );
  }

//##################################################
void LoadLibList ( const TString& list, TString& listlibs, TString& listlibsextra ) {
    TObjArray* arr = list.Tokenize(" ");
    TObjString *objstr = NULL;

    TIter next(arr);
    while ( (objstr=(TObjString*)next()) ) {
        TString module = objstr->GetString();
        module.Prepend("lib");
        if ( !LoadLibrary (module, listlibs, listlibsextra) ) { gApplication->Terminate(); }
        }
    delete arr;
    }

//##################################################
Bool_t LoadLibrary ( const TString& lib_name, TString& listlibs, TString& listlibsextra ) {
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

    listlibs      += lib_in_list;
    listlibsextra += lib_in_list;

    return kTRUE;
    }

//######################################################################################################################################
void LoadMacros() {
  // Aliroot macros
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGPP/PilotTrain/AddTaskCDBconnect.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C");
}

//##################################################
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

//##################################################
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

//##################################################
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

//##################################################
Bool_t PeriodIsMC ( const TString& per_str ) {
  TString period = per_str;
  if (!period.IsNull()) {
    period.ToLower();
    if ( period.BeginsWith("lhc") && (period.Length() > 6) ) {return kTRUE;}
  }
  return kFALSE;
}

//##################################################
Bool_t copyLocal2Alien(const char* where, const char* loc, const char* rem) {
  TString sl(Form("file:%s", loc));
  TString sr(Form("alien://%s", rem));
  Bool_t ret = TFile::Cp(sl, sr);
  if (!ret) { Warning(where, "Failed to copy %s to %s", sl.Data(), sr.Data()); }
  return ret;
}

// kate: indent-mode none; indent-width 2; replace-tabs on;

