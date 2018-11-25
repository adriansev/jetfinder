#ifdef __CLING__
// Tell ROOT where to find AliRoot headers
R__ADD_INCLUDE_PATH($ALICE_ROOT/include)
// Tell ROOT where to find AliPhysics headers
R__ADD_INCLUDE_PATH($ALICE_PHYSICS/include)

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
#include "PWG/EMCAL/macros/CreateAODChain.C"
#include "PWG/EMCAL/macros/CreateESDChain.C"

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
void                  LoadLibs ();
void                  LoadLibList ( const TString& list );
Bool_t                LoadLibrary ( const TString& lib );

void                  LoadMacros();
AliAnalysisAlien*     CreateAlienHandler ( const char* gridMode, const char* tag, unsigned int number_of_files = 20, unsigned int TTL = 43200 ,
                                           const char* outdir = "output", const char subworkdir = "", const char* extradirlvl = "");

TString GetInputDataPath ( const TString& file_list);
TString GetPeriod        ( const TString& file_path);
// TString GetPass          ( const TString& file_path);
//############################################################


//##################################################
AliAnalysisManager* EmcalJetCDF (
    const char*   cDataType      = "AOD",       // set the analysis type, AOD or ESD
    const char*   cRunPeriod     = "LHC11d",    // set the run period
    const char*   cLocalFiles    = "data.txt",  // set the local list file
    const UInt_t  arg_sel_chg    = 3145763, // "mykEMC_noGA",  // physics selection
    const UInt_t  arg_sel_full   = 3145763, // "mykEMC_noGA",  // physics selection
    int           iStartAnalysis = 1,
    const char*   cGridMode      = "test",
    const char*   cTaskName      = "CDFJets",   // sets name of task manager
    unsigned int  iNumFiles      = 100,         // numger of files to process from list file
    unsigned int  iNumEvents     = 999999999,    // number of events to be analyzed
    bool          bDoChargedJets = true,        // enable charge jets
    bool          bDoFullJets    = false        // enable full jets
) {
unsigned int       kGridFilesPerJob         = iNumFiles;      // Maximum number of files per job (gives size of AOD)
unsigned int       kTTL                     = 64800 ;         // Time To Live; 18h = 64800; 12h = 43200

//---------------------------------------------------------------------------------------------
TRegexp false_regex ("[f,F][a,A][l,L][s,S][e,E]");
TRegexp true_regex ("[t,T][r,R][u,U][e,E]");
TRegexp enable_regex ("[e,E][n,N][a,A][b,B][l,L][e,E]");
TRegexp disable_regex ("[d,D][i,I][s,S][a,A][b,B][l,L][e,E]");

bool  bDoSample = false;
TString ENV_doSAMPLE = gSystem->Getenv("CDF_doSAMPLE");
if (!ENV_doSAMPLE.IsNull() && ( ENV_doSAMPLE.EqualTo("0") || ENV_doSAMPLE.Contains(false_regex) ) ) { bDoSample = kFALSE; }
if (!ENV_doSAMPLE.IsNull() && ( ENV_doSAMPLE.EqualTo("1") || ENV_doSAMPLE.Contains(true_regex)  ) ) { bDoSample = kTRUE; }

bool  bDoCDF    = true;
TString ENV_doCDF = gSystem->Getenv("CDF_doCDF");
if (!ENV_doCDF.IsNull() && ( ENV_doCDF.EqualTo("0") || ENV_doCDF.Contains(false_regex) ) ) { bDoCDF = kFALSE; }
if (!ENV_doCDF.IsNull() && ( ENV_doCDF.EqualTo("1") || ENV_doCDF.Contains(true_regex)  ) ) { bDoCDF = kTRUE; }

// ######   DEBUG    ######
Int_t           debug              =  0 ; // kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
UInt_t          mgr_debug          =  0 ; // AliAnalysisManager debug level
UInt_t          kUseSysInfo        =  0 ; // activate debugging

TString ENV_DEBUG = gSystem->Getenv("CDF_DEBUG");
if (!ENV_DEBUG.IsNull() && ENV_DEBUG.IsDigit() ) { debug = ENV_DEBUG.Atoi(); }

TString ENV_DEBUG_MGR = gSystem->Getenv("CDF_DEBUG_MGR");
if (!ENV_DEBUG_MGR.IsNull() && ENV_DEBUG_MGR.IsDigit() ) { mgr_debug = ENV_DEBUG_MGR.Atoi(); }

TString ENV_NSYSINFO = gSystem->Getenv("CDF_NSYSINFO");
if (!ENV_NSYSINFO.IsNull() && ENV_NSYSINFO.IsDigit() ) { kUseSysInfo = ENV_NSYSINFO.Atoi(); }

if ( debug == 0 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal   ); }
if ( debug == 1 ) { AliLog::SetGlobalLogLevel ( AliLog::kError   ); }
if ( debug == 2 ) { AliLog::SetGlobalLogLevel ( AliLog::kWarning ); }
if ( debug == 3 ) { AliLog::SetGlobalLogLevel ( AliLog::kInfo    ); }
if ( debug >= 4 ) { AliLog::SetGlobalLogLevel ( AliLog::kDebug   ); }

// Progress bar
Bool_t bUseProgBar = kFALSE; // N.B. !! if true will set fDebug to 0
TString ENV_USEPROGBAR = gSystem->Getenv("PROGRESSBAR");
if (!ENV_USEPROGBAR.IsNull() && ( ENV_USEPROGBAR.EqualTo("1") || ENV_USEPROGBAR.Contains(true_regex) ) )  { bUseProgBar = kTRUE; }

//##################################################
//        AliEN plugin variables
//##################################################
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

const char* curdir = gSystem->BaseName(gSystem->pwd());
TString     kJobTag (curdir);

TString     kPluginExecutableCommand =
//                                      "aliroot -l -b -q -x";
                                        "root.exe -l -b -q -x";

TString     kAliPhysicsVersion       = "vAN-20181123-1";

// == grid plugin files rules
TString     kGridExtraFiles          = ""; // extra files that will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages
TString     kAlirootMode             = "ALIROOT";     // STEERBase,ESD,AOD,ANALYSIS,ANALYSISalice (default aliroot mode)

// FILES USED IN MACRO
TString     kCommonOutputFileName    = "AnalysisResults.root";

//--------------------
//   PROOF SETTINGS
//--------------------
TString kAAF        = "";
Int_t   kProofReset = 0;
Int_t   kWorkers    = 20;
Int_t   kCores      = 8  ;
//############################################################
const AliAnalysisTaskEmcal::EDataType_t kAod = AliAnalysisTaskEmcal::kAOD;
const AliAnalysisTaskEmcal::EDataType_t kEsd = AliAnalysisTaskEmcal::kESD;

const AliJetContainer::EJetType_t    fulljet = AliJetContainer::kFullJet;
const AliJetContainer::EJetType_t     chgjet = AliJetContainer::kChargedJet;
const AliJetContainer::EJetAlgo_t     antikt = AliJetContainer::antikt_algorithm;
const AliJetContainer::EJetAlgo_t         kt = AliJetContainer::kt_algorithm;
const AliJetContainer::ERecoScheme_t  recomb = AliJetContainer::pt_scheme;

// kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kDCALonly, kDCALonlyfid, kPHOS, kPHOSfid, kUser
const AliEmcalJet::JetAcceptanceType acc_chgjets  = AliEmcalJet::kTPCfid;
const AliEmcalJet::JetAcceptanceType acc_fulljets = AliEmcalJet::kEMCALfid;
//############################################################

// data source name
TString kDataSource (cLocalFiles);

// label of dataset from InputData.C
TString kGridDataSet ("");
if (iStartAnalysis == 2) { kGridDataSet = kDataSource;}

// EMCAL corrections task configuration file
TString EMCALcfg ("PWGJE_SEV_Config.yaml");
//("alien:///alice/cern.ch/user/m/mfasel/EMCALCorrectionConfig/ConfigDataPWGJEhighClusterThresholds.yaml");

if ( !kGridExtraFiles.IsNull() ) {kGridExtraFiles += " ";}
kGridExtraFiles += EMCALcfg;

//############################################################
// SETUP OF TRIGGERS
const AliEmcalPhysicsSelection::EOfflineEmcalTypes mykEMCAL      = AliEmcalPhysicsSelection::kEmcalOk;
const AliVEvent::EOfflineTriggerTypes  mykEMC             = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE | AliVEvent::kEMCEGA);
const AliVEvent::EOfflineTriggerTypes  mykEMC_noGA        = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE);

const AliVEvent::EOfflineTriggerTypes  mykMB              = AliVEvent::kAnyINT;
const AliVEvent::EOfflineTriggerTypes  mykMB_central      = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_semicentral  = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kSemiCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_mostcentral  = (AliVEvent::EOfflineTriggerTypes) (AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral);

AliVEvent::EOfflineTriggerTypes kPhysSel   = mykMB; //AliVEvent::kAnyINT; // physics selection
AliVEvent::EOfflineTriggerTypes kSel_tasks = mykMB;

AliVEvent::EOfflineTriggerTypes kSel_chg   = arg_sel_chg;
AliVEvent::EOfflineTriggerTypes kSel_full  = arg_sel_full;

//############################################################
  // Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);
  pMgr->SetDebugLevel(mgr_debug);
  if ( kUseSysInfo > 0 ) { pMgr->SetNSysInfo ( kUseSysInfo ); }

  AliAnalysisAlien* plugin = NULL;

  if (iStartAnalysis == 2) {  // start grid analysis
    // ( const char* gridMode, const char* tag, unsigned int number_of_files, unsigned int TTL, const char* outdir, const char subworkdir, const char* extradirlvl);
    plugin = CreateAlienHandler(cGridMode, kJobTag.Data(), kGridFilesPerJob, kTTL);
    if ( !plugin ) { ::Error ( "runEMCalJetSampleTask.C - StartGridAnalysis", "plugin invalid" ); return NULL; }
    pMgr->SetGridHandler(plugin);

    // use this command to run the macro
    plugin->SetExecutableCommand(kPluginExecutableCommand.Data());

    // AliPhysics version.
    plugin->SetAliPhysicsVersion ( kAliPhysicsVersion.Data() ); // Here you can set the (Ali)PHYSICS version you want to use

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
  AliTaskCDBconnect *taskCDB = AliTaskCDBconnect::AddTaskCDBconnect();
  taskCDB->SetFallBackToRaw(kTRUE); // Needed for the moment as not all grid sites do support cvmfs OCDB for the moment
  if (!taskCDB) { std::cout << "--------->>>>  taskCDB :: could not connect!!!! CHECK CVMFS" << std::endl; return NULL;}

  // Physics selection task
  if (iDataType == AliAnalysisTaskEmcal::kESD) {
    // signature : (Bool_t mCAnalysisFlag = kFALSE, Bool_t applyPileupCuts = kFALSE, UInt_t deprecatedFlag2 = 0, Bool_t useSpecialOutput=kFALSE)
    AliPhysicsSelectionTask* pPhysSelTask = AddTaskPhysicsSelection();
    }

  // Centrality task
  if ( iBeamType != AliAnalysisTaskEmcal::kpp && bIsRun2 ) {
    //signature : (Bool_t fillHistos=kTRUE, Bool_t aod=kFALSE)
    AliCentralitySelectionTask *pCentralityTask = AddTaskCentrality(kFALSE, kIsAOD);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  // AliMultSelection
  if ( bIsRun2 ) {
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

  // Sample task - charge jets
  AliAnalysisTaskEmcalJetSample* sampleTaskchg = NULL;
  if (bDoSample  && bDoChargedJets) {
    sampleTaskchg = AliAnalysisTaskEmcalJetSample::AddTaskEmcalJetSample("usedefault", "", "", "SMPCHG");
    sampleTaskchg->GetParticleContainer(0)->SetParticlePtCut(0.15);
    sampleTaskchg->SetHistoBins(600, 0, 300);
    sampleTaskchg->SelectCollisionCandidates(kSel_chg);
    sampleTaskchg->SetDebugLevel(debug);
    if ( pMultSelTask ) { 
      sampleTaskchg->SetUseNewCentralityEstimation(bIsRun2);
      sampleTaskchg->SetNCentBins(5);
      }
    }

  // Sample task - full jets
  AliAnalysisTaskEmcalJetSample* sampleTaskfull = NULL;
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
    if ( pMultSelTask ) {
      sampleTaskfull->SetUseNewCentralityEstimation(bIsRun2);
      sampleTaskfull->SetNCentBins(5);
      }
    }

  //###   CDF task - charged jets
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFchg = NULL;
  if (bDoCDF && bDoChargedJets) {
    anaTaskCDFchg = CDF::AddTaskEmcalJetCDF ( "usedefault", "", "", "CDFchg" );
    anaTaskCDFchg->GetParticleContainer(0)->SetParticlePtCut(0.15);
    anaTaskCDFchg->SetHistoBins(600, 0, 300);
    anaTaskCDFchg->SelectCollisionCandidates(kSel_chg);
    anaTaskCDFchg->SetDebugLevel(debug);
    if ( pMultSelTask ) { 
      anaTaskCDFchg->SetUseNewCentralityEstimation(bIsRun2);
      anaTaskCDFchg->SetNCentBins(5);
      }
    }

  //###   CDF task - full jets
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFfull = NULL;
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
    anaTaskCDFfull->SetDebugLevel(debug);
    if ( pMultSelTask ) {
      anaTaskCDFfull->SetUseNewCentralityEstimation(bIsRun2);
      anaTaskCDFfull->SetNCentBins(5);
      }
    }

  // add jet containers to sample task for full jets
  if (bDoFullJets && bDoSample) {
    AliJetContainer* jetCont02full_sample = sampleTaskfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
    AliJetContainer* jetCont04full_sample = sampleTaskfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");

    if (iBeamType != AliAnalysisTaskEmcal::kpp) {
      jetCont02full_sample->SetRhoName(sRhoFuName);
      jetCont02full_sample->SetPercAreaCut(0.6);

      jetCont04full_sample->SetRhoName(sRhoFuName);
      jetCont04full_sample->SetPercAreaCut(0.6);
      }
    }

  // add jet containers to CDF task for full jets
  if (bDoFullJets && bDoCDF) {
    AliJetContainer* jetcont_full = NULL;
    for ( Float_t fi = 0 ; fi<=100 ; fi+=10) {
      // FULL JETS 0.2
      jetcont_full  = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);

      // FULL JETS 0.4
      jetcont_full  = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");
      CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);
      }

    AliJetContainer* jetcont_full   = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.2, acc_fulljets, "Jet");
    CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);

    AliJetContainer* jetcont_full   = anaTaskCDFfull->AddJetContainer(fulljet, antikt, recomb, 0.4, acc_fulljets, "Jet");
    CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);

    AliJetContainer* jetcont_full = NULL;
    }

  // add jet containers to sample task for charged jets
  if (bDoChargedJets && bDoSample) {
    AliJetContainer* jetCont02chg_sample = sampleTaskchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
    AliJetContainer* jetCont04chg_sample = sampleTaskchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");

    if (iBeamType != AliAnalysisTaskEmcal::kpp) {
      jetCont02chg_sample->SetRhoName(sRhoChName);
      jetCont02chg_sample->SetPercAreaCut(0.6);

      jetCont04chg_sample->SetRhoName(sRhoChName);
      jetCont04chg_sample->SetPercAreaCut(0.6);
      }
    }

  // add jet containers to CDF task for charged jets
  if (bDoChargedJets && bDoCDF) {
    AliJetContainer* jetcont_chg = NULL;
    for ( Float_t fi = 0 ; fi<=100 ; fi+=10) {
      // CHG JETS 0.2
      jetcont_chg  = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);

      // CHG JETS 0.4
      jetcont_chg  = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");
      CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);
      }

    AliJetContainer* jetcont_chg   = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.2, acc_chgjets, "Jet");
    CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);

    AliJetContainer* jetcont_chg   = anaTaskCDFchg->AddJetContainer(chgjet, antikt, recomb, 0.4, acc_chgjets, "Jet");
    CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);

    AliJetContainer* jetcont_chg = NULL;
    }

  TObjArray* tasks_list = pMgr->GetTasks(); TIter task_iter (tasks_list); AliAnalysisTaskSE* task = NULL;
  while (( task = dynamic_cast<AliAnalysisTaskSE*>(task_iter.Next()) )) {
    if (task->InheritsFrom("AliAnalysisTaskEmcal")) {
      Printf("Setting beam type %d for task %s", iBeamType, static_cast<AliAnalysisTaskEmcal*>(task)->GetName());
      static_cast<AliAnalysisTaskEmcal*>(task)->SetForceBeamType(iBeamType);
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
  pMgr->SetUseProgressBar(bUseProgBar, 100);

  TFile* pOutFile = new TFile("train.root","RECREATE");
  pOutFile->cd();
  pMgr->Write();
  pOutFile->Close();
  delete pOutFile;

  // task profiling
  if ( kUseSysInfo > 0 ) {
    for ( int i = 0; i < pMgr->GetTopTasks()->GetEntries(); i++ ) { pMgr->ProfileTask (i); }
    }

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
AliAnalysisAlien* CreateAlienHandler ( const char* gridMode, const char* tag, unsigned int number_of_files, unsigned int TTL,
                                       const char* outdir, const char subworkdir, const char* extradirlvl ) {

  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if ( !plugin ) { cout << "!!! -->> alien handler could not be created <<-- !!!" << endl; return NULL;}

  unsigned int       kGridMaxMergeFiles       = 100;            // Number of files merged in a chunk grid run range
  unsigned int       kMaxInitFailed           = 10 ;            // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

  TString     kGridOutdir              = outdir ;       // AliEn output directory. If blank will become output_<kTrainName>
  TString     kGridSubWorkDir          = subworkdir ;   // sub working directory not to confuse different run xmls
  TString     kGridExtraAliendirLevel  = extradirlvl ;  // sub working directory not to confuse different run xmls

  TString     kWorkDir                 = tag;  // AliEn work dir; relative to AliEn $HOME

  TString     kTrainName               = "cdfjets";     // *CHANGE ME* (no blancs or special characters)

  TString macroName("");  macroName = Form("%s.C", kTrainName.Data());
  TString execName("");   execName  = Form("%s.sh", kTrainName.Data());
  TString jdlName("");    jdlName   = Form("%s.jdl", kTrainName.Data());

  // Set run mode.  Can be "full", "test", "offline", "submit" or "merge"
  plugin->SetRunMode(gridMode);

  // Job tag
  plugin->SetJobTag(tag);

  // AliEn directory containing the input packages
  plugin->SetGridWorkingDir(tag);

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
    plugin->SetNtestFiles ( number_of_files );
    }
  else {
    // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
    plugin->SetNtestFiles ( number_of_files );
    }

  // Optionally set time to live
  plugin->SetTTL(TTL);

  // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
  plugin->SetSplitMaxInputFileNumber ( number_of_files );

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
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
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

// //##################################################
// TString GetPass ( const TString& file_path) {
//   TString pass = "";
// 
//   if (!file_path.IsNull()) {
//     // split string in tokens (libs)
//     TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
//     TIter next_str(tokens_list);
//     TObjString* token=0;
//     while ((token=(TObjString*)next_str())) {
//       TString token_str = token->GetString(); token_str.ToLower();
//       if ( token_str.Contains("pass") ) { pass = token_str; break; }
//       }
//     delete tokens_list;
//     }
// 
//   return pass;
// }

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

