#if !( defined(__CINT__) || defined(__MAKECINT__) )
// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>

// ROOT includes
#include <Rtypes.h>
#include <Riostream.h>
#include <TRint.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TRegexp.h>
#include <TPRegexp.h>
#include <TInterpreter.h>

#include <TApplication.h>
#include <TList.h>
#include <TEnv.h>
#include <TKey.h>
#include <TBits.h>
#include <TError.h>
#include <TNamed.h>
#include <TTask.h>
#include <TDirectory.h>
#include <TDirectoryFile.h>
#include <TFile.h>
#include <TFileInfo.h>
#include <TFileCollection.h>
#include <TCollection.h>
#include <TChain.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TProof.h>

#include <TObject.h>
#include <TNamed.h>
#include <TTask.h>

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
#include "AliEmcalPhysicsSelection.h"
#include "AliCentralitySelectionTask.h"

#include "AliEmcalJetTask.h"

#include "AliEmcalClusterMaker.h"
#include "AliEmcalClusTrackMatcherTask.h"
#include "AliEMCALRecoUtils.h"

#include "AliMultSelectionTask.h"

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
#include "AliAnalysisTaskEmcalEmbeddingHelper.h"

// Tell ROOT where to find AliRoot headers
R__ADD_INCLUDE_PATH($ALICE_ROOT/include)

// Tell ROOT where to find AliPhysics headers
R__ADD_INCLUDE_PATH($ALICE_PHYSICS/include)

// add current directory
R__ADD_INCLUDE_PATH(.)
R__ADD_LIBRARY_PATH(.)

//R__LOAD_LIBRARY(LIBRARY)
#endif

#include "InputData.C"

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
// void                  load_config ( const char* file );
void                  LoadLibs    ( TString& listlibs, TString& listlibsextra );
bool                  LoadLibList ( const TString& list, TString& listlibs, TString& listlibsextra );

AliAnalysisAlien* CreateAlienHandler ( const char* gridMode, const char* tag, unsigned int nr_test_files = 1, unsigned int TTL = 43200 ,
                                       const char* outdir = "output", const char* subworkdir = "", const char* extradirlvl = "");

//##################################################
AliAnalysisManager* EmcalJetCDF (
    const char*   cRunPeriod     = "LHC11d",    // set the run period
    const char*   cLocalFiles    = "data.txt",  // set the local list file
    const Int_t   arg_sel_chg    = 2,           // defaults kINT7  // physics selection
    const Int_t   arg_sel_full   = 2,           // defaults kINT7  // physics selection
    const Int_t   mgr_mode       = 0,           // local = 0, proof = 1, grid = 2
    const Int_t   alien_mode     = 0,           // test = 0, offline = 1, submit = 2, merge = 3, full = 4
    const char*   cTaskName      = "CDFJets",   // sets name of task manager
    unsigned int  iNumFiles      = 100,         // numger of files to process from list file
    unsigned int  iNumEvents     = 999999999,   // number of events to be analyzed
    bool          bDoChargedJets = true,        // enable charge jets
    bool          bDoFullJets    = false        // enable full jets
) {
//Load needed libs
TString     ListLibs      = ""; // string list of loaded libs
TString     ListLibsExtra = ""; // string list of loaded extra libs
LoadLibs( ListLibs, ListLibsExtra ); // load libs and save the lists to ListLibs and ListLibsExtra

AliAnalysisManager::EAliAnalysisExecMode  ManagerMode = static_cast<AliAnalysisManager::EAliAnalysisExecMode>(mgr_mode);
AliAnalysisGrid::EPluginRunMode           PluginMode  = static_cast<AliAnalysisGrid::EPluginRunMode>(alien_mode);

namespace CDF = PWGJE::EMCALJetTasks::AliAnalysisTaskEmcalJetCDF_NS; // shortcut to task namespace
unsigned int       kGridFilesPerJob         = iNumFiles;      // Maximum number of files per job (gives size of AOD)
unsigned int       kTTL                     = 64800 ;         // Time To Live; 18h = 64800; 12h = 43200

TString sAnalysisType ("local");
if ( ManagerMode == AliAnalysisManager::kProofAnalysis )  { sAnalysisType = "proof"; }
if ( ManagerMode == AliAnalysisManager::kGridAnalysis )   { sAnalysisType = "grid"; }
const char* cAnalysisType = sAnalysisType.Data();

TString sGridMode ("test");
if ( PluginMode == AliAnalysisGrid::kOffline ) { sGridMode = "offline"; }
if ( PluginMode == AliAnalysisGrid::kSubmit )  { sGridMode = "submit"; }
if ( PluginMode == AliAnalysisGrid::kMerge )   { sGridMode = "merge"; }
if ( PluginMode == AliAnalysisGrid::kFull )    { sGridMode = "full"; }

if (sAnalysisType.EqualTo("proof")) { sGridMode = "test"; } // always use test mode for plugin if we are using proof
const char* cGridMode = sGridMode.Data();

//######################################
//   LOAD CONFIGURATION FILE
CDF::load_config("cdf.steer");
//######################################

TRegexp false_regex ("[f,F][a,A][l,L][s,S][e,E]");
TRegexp true_regex ("[t,T][r,R][u,U][e,E]");
TRegexp enable_regex ("[e,E][n,N][a,A][b,B][l,L][e,E]");
TRegexp disable_regex ("[d,D][i,I][s,S][a,A][b,B][l,L][e,E]");

// Physics selection task
bool bDoPhysicsSelect = false;
TString ENV_doPhysSelect = gSystem->Getenv("CDF_doPhysSelect");
if (!ENV_doPhysSelect.IsNull() && ( ENV_doPhysSelect.EqualTo("1") || ENV_doPhysSelect.Contains(true_regex) ) ) { bDoPhysicsSelect = true; }

bool applyPileupCuts = false;
TString ENV_doPileup = gSystem->Getenv("CDF_doPileup");
if (!ENV_doPileup.IsNull() && ( ENV_doPileup.EqualTo("1") || ENV_doPileup.Contains(true_regex) ) ) { bDoPhysicsSelect = applyPileupCuts = true; }

bool bDoMultSelect = true;
TString ENV_doMult = gSystem->Getenv("CDF_doMULT");
if (!ENV_doMult.IsNull() && ( ENV_doMult.EqualTo("0") || ENV_doMult.Contains(false_regex) ) ) { bDoMultSelect = false; }

Int_t centbins = 1;
TString ENV_centbins = gSystem->Getenv("CDF_doMULT_bins");
if (!ENV_centbins.IsNull() && ENV_centbins.IsDigit() ) { centbins = ENV_centbins.Atoi(); if (centbins > 5) centbins = 5; }

bool bDoBackgroundSubtraction = true;
TString ENV_doBackground = gSystem->Getenv("CDF_doBKRD");
if (!ENV_doBackground.IsNull() && ( ENV_doBackground.EqualTo("0") || ENV_doBackground.Contains(false_regex) ) ) { bDoBackgroundSubtraction = false; }

// do Embedding
bool bDoEmbedding = false;

// Do jet matching
const bool useJetTagger = true;

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

TString     kPluginExecutableCommand ("root.exe -l -b -q -x");
TString     kAliPhysicsVersion       =
                                         "vAN-20190908-1";
                                         // "vAN-20190908_ROOT6-1";

TString ENV_ALIPHYSVER = gSystem->Getenv("ALIPHYSICS_VERSION");
TRegexp start_with_v ("^v.*"); // all cvmfs versions start with "v"; it is the common token for all cvmfs versions
if ( ENV_ALIPHYSVER.Contains(start_with_v) ) { kAliPhysicsVersion = ENV_ALIPHYSVER; }

// Report configuration of analysis
cout << "\n\n" << ">>>>>>>>   Analysis configuration   <<<<<<<<" << "\n";
cout << "ALIPHYSICS_VERSION - Running env: " << ENV_ALIPHYSVER.Data() << "\n";
cout << "ALIPHYSICS_VERSION - GRID: " << kAliPhysicsVersion.Data() << "\n";
cout << "PhysicsSelection task: " << bDoPhysicsSelect << "\n";
cout << "Apply Pileup Cuts: " << applyPileupCuts << "\n";
cout << "Do Multiplicity Selection: " << bDoMultSelect << "\n";
cout << "Multiplicity Bins: " << centbins << "\n";
cout << "Do background substraction: " << bDoBackgroundSubtraction << "\n";
cout << "DEBUG - Global: " << debug << "\n";
cout << "DEBUG - MGR: " << mgr_debug << "\n";
cout << "DEBUG - NSysInfo: " << kUseSysInfo << "\n";
cout << "Progress bar: " << bUseProgBar << "\n";
cout << "\n\n" << endl;

//##################################################
//        AliEN plugin variables
//##################################################
const char* curdir = gSystem->BaseName(gSystem->pwd());
TString     kJobTag (curdir);

// == grid plugin files rules
TString     kGridExtraFiles          = "cdf.steer rootlogon.C InputData.C"; // extra files that will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

// FILES USED IN MACRO
TString     kCommonOutputFileName    = "AnalysisResults.root";

//############################################################
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
if ( ManagerMode == AliAnalysisManager::kGridAnalysis ) { kGridDataSet = kDataSource;}

//############################################################
// SETUP OF TRIGGERS
const AliEmcalPhysicsSelection::EOfflineEmcalTypes mykEMCAL  = AliEmcalPhysicsSelection::kEmcalOk;
const AliVEvent::EOfflineTriggerTypes  mykEMC             = static_cast<AliVEvent::EOfflineTriggerTypes>(AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE | AliVEvent::kEMCEGA);
const AliVEvent::EOfflineTriggerTypes  mykEMC_noGA        = static_cast<AliVEvent::EOfflineTriggerTypes>(AliVEvent::kEMC1 | AliVEvent::kEMC7 | AliVEvent::kEMC8 | AliVEvent::kEMCEJE);

const AliVEvent::EOfflineTriggerTypes  mykMB              = AliVEvent::kMB;
const AliVEvent::EOfflineTriggerTypes  mykINT7            = AliVEvent::kINT7;
const AliVEvent::EOfflineTriggerTypes  mykAny             = AliVEvent::kAnyINT; // kAnyINT = kMB | kINT7 | kINT5 | kINT8 | kSPI7
const AliVEvent::EOfflineTriggerTypes  mykMB_central      = static_cast<AliVEvent::EOfflineTriggerTypes>(AliVEvent::kAnyINT | AliVEvent::kCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_semicentral  = static_cast<AliVEvent::EOfflineTriggerTypes>(AliVEvent::kAnyINT | AliVEvent::kSemiCentral);
const AliVEvent::EOfflineTriggerTypes  mykMB_mostcentral  = static_cast<AliVEvent::EOfflineTriggerTypes>(AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral);

AliVEvent::EOfflineTriggerTypes kPhysSel   = mykAny; //AliVEvent::kAnyINT; // physics selection
// AliVEvent::EOfflineTriggerTypes kSel_tasks = mykMB;

AliVEvent::EOfflineTriggerTypes kSel_chg   = static_cast<AliVEvent::EOfflineTriggerTypes>(arg_sel_chg);
AliVEvent::EOfflineTriggerTypes kSel_full  = static_cast<AliVEvent::EOfflineTriggerTypes>(arg_sel_full);

//############################################################
// Analysis manager
  AliAnalysisManager* pMgr = new AliAnalysisManager(cTaskName);
  pMgr->SetAnalysisType(ManagerMode);
  pMgr->SetDebugLevel(mgr_debug);
  if ( kUseSysInfo > 0 ) { pMgr->SetNSysInfo ( kUseSysInfo ); }

  // actual runName in the form of LHCXXX....
  TString sRunName;
  Bool_t kIsAOD = kTRUE;
  AliAnalysisTaskEmcal::EDataType_t iDataType = AliAnalysisTaskEmcal::kAOD; // assuming default is to process AOD

  // GRID MODE SETTINGS
  AliAnalysisAlien* plugin = NULL;
  if ( ManagerMode == AliAnalysisManager::kGridAnalysis ) {  // start grid analysis
    // ( const char* gridMode, const char* tag, unsigned int nr_test_files, unsigned int TTL, const char* outdir, const char subworkdir, const char* extradirlvl);
    plugin = CreateAlienHandler(cGridMode, kJobTag.Data(), kGridFilesPerJob, kTTL);
    if ( !plugin ) { ::Error ( "EmcalJetCDF.C - Start GRID Analysis", "plugin invalid" ); return NULL; }
    pMgr->SetGridHandler(plugin);

    // use this command to run the macro
    plugin->SetExecutableCommand(kPluginExecutableCommand.Data());

    // AliPhysics version.
    plugin->SetAliPhysicsVersion ( kAliPhysicsVersion.Data() ); // Here you can set the (Ali)PHYSICS version you want to use

    #if ( defined(__CINT__))
    gROOT->LoadMacro("InputData.C");
    #endif
    InputData(kGridDataSet);
    sRunName = CDF::GetPeriod( plugin->GetGridDataDir() );

    TString file = CDF::GetFileFromPath(plugin->GetDataPattern());
    if (file.Contains("AliESD")) { iDataType = AliAnalysisTaskEmcal::kESD; kIsAOD = kFALSE; }

    plugin->SetMergeExcludes(kGridMergeExclude.Data());
    }

  // LOCAL CHAIN
  TChain* pChain = NULL;
  if ( ManagerMode == AliAnalysisManager::kLocalAnalysis ) { // start local analysis
    if ( kDataSource.IsNull() ) { Printf("You need to provide the list of local files!"); return NULL; }
    pChain = CDF::CreateChain(kDataSource.Data(), "auto", "", iNumFiles);
    if (!pChain) { std::cout << ">>>>>>>>>>>>>>   CHAIN NOT CREATED   <<<<<<<<<<<<<<" << std::endl; return NULL; }
    Printf("Setting local analysis for %d files from list %s, max events = %d", iNumFiles, kDataSource.Data(), iNumEvents);

    // get the path of first file
    TString file = pChain->GetFile()->GetEndpointUrl()->GetUrl();
    sRunName = CDF::GetPeriod(file.Data()); // get the run name : first token beggining with lhc
    if ( CDF::GetFileFromPath(file).Contains("AliESD") ) { iDataType = AliAnalysisTaskEmcal::kESD; kIsAOD = kFALSE; }
    }

  // PROOF MODE SETTINGS
  if ( ManagerMode == AliAnalysisManager::kProofAnalysis ) { // start local proof analysis
    if ( kDataSource.IsNull() ) { Printf("You need to provide the list of local files!"); return NULL; }
    TChain* chain = CDF::CreateChain(kDataSource.Data(), "auto", "", 1);
    if (!chain) { std::cout << ">>>>>>>>>>>>>>   CHAIN NOT CREATED   <<<<<<<<<<<<<<" << std::endl; return NULL; }
    TString file = chain->GetFile()->GetEndpointUrl()->GetUrl();
    sRunName = CDF::GetPeriod(file.Data()); // get the run name : first token beggining with lhc
    if ( CDF::GetFileFromPath(file).Contains("AliESD") ) { iDataType = AliAnalysisTaskEmcal::kESD; kIsAOD = kFALSE; }
    chain->Delete();
    // ( const char* gridMode, const char* tag, unsigned int nr_test_files, unsigned int TTL, const char* outdir, const char subworkdir, const char* extradirlvl);
    plugin = CreateAlienHandler(cGridMode, kJobTag.Data(), kGridFilesPerJob, kTTL);
    if ( !plugin ) { ::Error ( "EmcalJetCDF.C - Start PROOF Analysis", "plugin invalid" ); return NULL; }
    pMgr->SetGridHandler(plugin);

    plugin->SetFileForTestMode(kDataSource.Data());
    plugin->SetProofConnectGrid(false);
    plugin->SetProofCluster("lite://");
    }

  cout << ">>>>>>>> ManagerMode: " << ManagerMode << " ; Analysis type: " << cAnalysisType << std::endl;
  cout << ">>>>>>>> PluginMode: "  << PluginMode  << " ; Type: "          << cGridMode << std::endl;

  TString sDataType ("AOD");
  if (!kIsAOD) { sDataType = "ESD"; }
  std::cout << ">>> Analysis data type : " << sDataType.Data() << std::endl;

  // name of de period (for MC is the name of "achored to" production)
  TString sRunPeriod (cRunPeriod);
  sRunPeriod.ToLower();

  bool isMC = CDF::PeriodIsMC(sRunName.Data());

  // EMCAL corrections task configuration file
  TString EMCALcfg ("CDF_CorrectionsConf.yaml");
  if (isMC) {EMCALcfg = "CDF_MC_CorrectionsConf.yaml";}

  if ( !kGridExtraFiles.IsNull() ) { kGridExtraFiles += " "; }
  kGridExtraFiles += EMCALcfg;

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;
  Bool_t bIsRun2 = kFALSE;

  if (sRunPeriod.Length() == 6 && (sRunPeriod.BeginsWith("lhc15") || sRunPeriod.BeginsWith("lhc16") || sRunPeriod.BeginsWith("lhc17") || sRunPeriod.BeginsWith("lhc18") ) ) { bIsRun2 = kTRUE; }

  if (sRunPeriod == "lhc10h" || sRunPeriod == "lhc11h" || sRunPeriod == "lhc15o" || sRunPeriod == "lhc17n" )                            { iBeamType = AliAnalysisTaskEmcal::kAA; }
  if (sRunPeriod == "lhc12g" || sRunPeriod == "lhc13b" || sRunPeriod == "lhc13c" || sRunPeriod == "lhc13d" || sRunPeriod == "lhc13e" ||
      sRunPeriod == "lhc13f" || sRunPeriod == "lhc16q" || sRunPeriod == "lhc16r" || sRunPeriod == "lhc16s" || sRunPeriod == "lhc16t" )  { iBeamType = AliAnalysisTaskEmcal::kpA; }

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(sRunPeriod);
  Printf(">>>   Default track cut period set to: %s\n", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  Bool_t bDoEmcalCorrections  = kFALSE;
  if (bDoFullJets) { bDoEmcalCorrections  = kTRUE; }

// ###   Containers and string definitions
  TString name_tracks   = "usedefault";
  TString name_clusters = "usedefault";
  TString name_cells    = "usedefault";
  // Set centrality estimator for centrality task
  TString cent_est_chg  = "V0M" ;
  TString cent_est_full = "V0M" ;

  TString mc_container ("");
  if (isMC) { mc_container = "mcparticles"; }

/*
  // General input object names
  TString tracksName             = AliEmcalContainerUtils::DetermineUseDefaultName(AliEmcalContainerUtils::kTrack);
  TString clustersName           = AliEmcalContainerUtils::DetermineUseDefaultName(AliEmcalContainerUtils::kCluster);
  TString emcalCellsName         = AliEmcalContainerUtils::DetermineUseDefaultName(AliEmcalContainerUtils::kCaloCells);
  // Combined (PbPb + embedded det level)
  TString emcalCellsCombined = emcalCellsName + "Combined";
  TString clustersCombined = clustersName + "Combined";
*/

//##########################
//   TASKS DEFINITIONS   ###
//##########################
  AliAODInputHandler* pAODHandler = NULL;
  AliESDInputHandler* pESDHandler = NULL;
  if (kIsAOD)
    { pAODHandler = AliAnalysisTaskEmcal::AddAODHandler(); }
  else
    { pESDHandler = AliAnalysisTaskEmcal::AddESDHandler(); }

  // CDBconnect task
  AliTaskCDBconnect* taskCDB = AliTaskCDBconnect::AddTaskCDBconnect();
  taskCDB->SetFallBackToRaw(kTRUE); // Needed for the moment as not all grid sites do support cvmfs OCDB for the moment
  if (!taskCDB) { std::cout << "--------->>>>  taskCDB :: could not connect!!!! CHECK CVMFS" << std::endl; return NULL;}

  // Physics selection task
  AliPhysicsSelectionTask* pPhysSelTask = NULL;
  if (bDoPhysicsSelect) {
    // signature : (Bool_t mCAnalysisFlag = kFALSE, Bool_t applyPileupCuts = kFALSE, UInt_t deprecatedFlag2 = 0, Bool_t useSpecialOutput=kFALSE)
    pPhysSelTask = AliPhysicsSelectionTask::AddTaskPhysicsSelection(isMC, applyPileupCuts);
    }

  // AliMultSelection
  AliMultSelectionTask* pMultSelTask = NULL;
  if (bDoMultSelect && bIsRun2) {
    // signature : ( Bool_t lCalibration = kFALSE, TString lExtraOptions = "", Int_t lNDebugEstimators = 1, const TString lMasterJobSessionFlag = "")
    pMultSelTask = AliMultSelectionTask::AddTaskMultSelection();
    pMultSelTask->SelectCollisionCandidates(AliVEvent::kAny);
    }

  // Centrality task
  // The Run 2 condition is too restrictive, but until the switch to MultSelection is complete, it is the best we can do
  AliCentralitySelectionTask* pCentralityTask = NULL;
  if ( !kIsAOD && (iBeamType != AliAnalysisTaskEmcal::kpp && bIsRun2) ) {
    //signature : (Bool_t fillHistos=kTRUE, Bool_t aod=kFALSE)
    pCentralityTask = AliCentralitySelectionTask::AddTaskCentrality(kFALSE, kIsAOD);
    pCentralityTask->SelectCollisionCandidates(AliVEvent::kAny);
    }


  // Embeded Configuration options
  // Embedding files list
  const TString embeddedFilesList = "aodFilesEmbed.txt";
  // If true, events that are not selected in the PbPb will not be used for embedding.
  // This ensures that good embedded events are not wasted on bad PbPb events.
  const bool internalEventSelection = true;
  // Embedding task
  if (bDoEmbedding) {
    // Debug options
    //AliLog::SetClassDebugLevel("AliAnalysisTaskEmcalEmbeddingHelper", AliLog::kDebug+0);

    // Setup embedding task
    AliAnalysisTaskEmcalEmbeddingHelper * embeddingHelper = AliAnalysisTaskEmcalEmbeddingHelper::AddTaskEmcalEmbeddingHelper();
    embeddingHelper->SelectCollisionCandidates(kPhysSel);
    // The pt hard bin should be set via the filenames in this file
    // If using a file pattern, it could be configured via embeddingHelper->SetPtHardBin(ptHardBin);
    embeddingHelper->SetFileListFilename(embeddedFilesList.Data());

    // Some example settings for LHC12a15e_fix (anchored to LHC11h)
    embeddingHelper->SetNPtHardBins(11);
    embeddingHelper->SetMCRejectOutliers();

    // Setup internal event selection and additional configuration options
    embeddingHelper->SetConfigurationPath("EmbeddingConfigurationExample.yaml");

    // Initialize the task to complete the setup.
    embeddingHelper->Initialize();
    }

  // EMCal corrections
  AliEmcalCorrectionTask* correctionTask = NULL;
  if (bDoEmcalCorrections) {
    // Configuration of the Correction Task is handled via a YAML file, which is setup below
    // signature : (TString suffix)
    correctionTask = AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask();
//  correctionTask = AliEmcalCorrectionTask::ConfigureEmcalCorrectionTaskOnLEGOTrain(suffix);
    correctionTask->SelectCollisionCandidates(kPhysSel);
    correctionTask->SetUseNewCentralityEstimation(bIsRun2);
    correctionTask->SetForceBeamType(static_cast<AliEmcalCorrectionTask::BeamType>(iBeamType));

    // Configure and initialize
    correctionTask->SetUserConfigurationFilename( EMCALcfg.Data() );
    //correctionTask->SetUserConfigurationFilename("alien:///alice/cern.ch/user/m/mfasel/EMCALCorrectionConfig/ConfigDataPWGJEhighClusterThresholds.yaml");
    correctionTask->Initialize(true);

    }

//     TObjArray correctionTasks;
//
//     // Create the Correction Tasks
//     // "data" corresponds to the PbPb level
//     // "embed" corresponds to the embedded detector level
//     // "combined" corresponds to the hybrid (PbPb + embedded detector) level
//     correctionTasks.Add(AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask("data"));
//     correctionTasks.Add(AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask("embed"));
//     // It is important that combined is last!
//     correctionTasks.Add(AliEmcalCorrectionTask::AddTaskEmcalCorrectionTask("combined"));
//
//     // Loop over all of the correction tasks to configure them
//     AliEmcalCorrectionTask * tempCorrectionTask = 0;
//     TIter next(&correctionTasks);
//     while (( tempCorrectionTask = static_cast<AliEmcalCorrectionTask *>(next()))) {
//       tempCorrectionTask->SelectCollisionCandidates(kPhysSel);
//       // Configure centrality
//       tempCorrectionTask->SetNCentBins(5);
//       tempCorrectionTask->SetUseNewCentralityEstimation(bIsRun2);
//       tempCorrectionTask->SetUserConfigurationFilename("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/EMCalCorrectionTaskEmbeddingExample.yaml");
//
//       tempCorrectionTask->Initialize(true);
//       }

  // Background
  TString sRhoChName;
  TString sRhoFuName;
  AliAnalysisTaskRho* pRhoTask = NULL;
  if ( bDoBackgroundSubtraction && iBeamType != AliAnalysisTaskEmcal::kpp ) {
    sRhoChName = "Rho";
    sRhoFuName = "Rho_Scaled";

    AliEmcalJetTask* pKtChJetTask = AliEmcalJetTask::AddTaskEmcalJet("usedefault", "", kt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 0., kFALSE, kFALSE);
    pKtChJetTask->SelectCollisionCandidates(kPhysSel);

    //signature :
    // const char* nTracks = "usedefault", const char* nClusters = "usedefault", const char* nRho = "Rho", Double_t jetradius = 0.2, UInt_t acceptance = AliEmcalJet::kTPCfid,
    // AliJetContainer::EJetType_t jetType = AliJetContainer::kChargedJet, const Bool_t histo = kFALSE, AliJetContainer::ERecoScheme_t rscheme = AliJetContainer::pt_scheme, const char* suffix = ""
    pRhoTask = AliAnalysisTaskRho::AddTaskRhoNew("usedefault", "usedefault", sRhoChName.Data(), 0.4);
    pRhoTask->SetExcludeLeadJets(2);
    pRhoTask->SelectCollisionCandidates(kPhysSel);
    pRhoTask->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);

    if (bDoFullJets) {
      TString sFuncPath = "alien:///alice/cern.ch/user/s/saiola/LHC11h_ScaleFactorFunctions.root";
      TString sFuncName = "LHC11h_HadCorr20_ClustersV2";
      pRhoTask->LoadRhoFunction(sFuncPath, sFuncName);
      }
    }

  TString sRhoChName_MC;
  TString sRhoFuName_MC;
  AliAnalysisTaskRho* pRhoTask_MC = NULL;
  if ( bDoBackgroundSubtraction && isMC && iBeamType != AliAnalysisTaskEmcal::kpp ) {
    sRhoChName = "Rho_MC";
    sRhoFuName = "Rho_Scaled_MC";

    AliEmcalJetTask* pKtChJetTask_MC = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", kt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "JetMC", 0., kFALSE, kFALSE);
    pKtChJetTask_MC->SelectCollisionCandidates(kPhysSel);

    //signature :
    // const char* nTracks = "usedefault", const char* nClusters = "usedefault", const char* nRho = "Rho", Double_t jetradius = 0.2, UInt_t acceptance = AliEmcalJet::kTPCfid,
    // AliJetContainer::EJetType_t jetType = AliJetContainer::kChargedJet, const Bool_t histo = kFALSE, AliJetContainer::ERecoScheme_t rscheme = AliJetContainer::pt_scheme, const char* suffix = ""
    pRhoTask_MC = AliAnalysisTaskRho::AddTaskRhoNew("mcparticles", "usedefault", sRhoChName_MC.Data(), 0.4);
    pRhoTask_MC->SetExcludeLeadJets(2);
    pRhoTask_MC->SelectCollisionCandidates(kPhysSel);
    pRhoTask_MC->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);

    if (bDoFullJets) {
      TString sFuncPath = "alien:///alice/cern.ch/user/s/saiola/LHC11h_ScaleFactorFunctions.root";
      TString sFuncName = "LHC11h_HadCorr20_ClustersV2";
      pRhoTask_MC->LoadRhoFunction(sFuncPath, sFuncName);
      }
    }

  // Find Charged jets
  AliEmcalJetTask* pChJet02Task = NULL;
  AliEmcalJetTask* pChJet04Task = NULL;
  AliEmcalJetTask* pChJet02Task_MC = NULL;
  AliEmcalJetTask* pChJet04Task_MC = NULL;
  if (bDoChargedJets) {
    pChJet02Task = AliEmcalJetTask::AddTaskEmcalJet(name_tracks.Data(), "", antikt, 0.2, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet02Task->SelectCollisionCandidates(kSel_chg);

    pChJet04Task = AliEmcalJetTask::AddTaskEmcalJet(name_tracks.Data(), "", antikt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pChJet04Task->SelectCollisionCandidates(kSel_chg);

    if (isMC) {
      pChJet02Task_MC = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", antikt, 0.2, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
      pChJet02Task_MC->SelectCollisionCandidates(kSel_chg);

      pChJet04Task_MC = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", antikt, 0.4, chgjet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
      pChJet04Task_MC->SelectCollisionCandidates(kSel_chg);

      if (bDoEmbedding) {
        pChJet02Task_MC->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);
        AliParticleContainer* partLevelTracks02Task_MC = pChJet02Task_MC->GetParticleContainer(0);
        // Called Embedded, but really just means get from an external event!
        partLevelTracks02Task_MC->SetIsEmbedding(kTRUE);

        pChJet04Task_MC->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);
        AliParticleContainer* partLevelTracks04Task_MC = pChJet04Task_MC->GetParticleContainer(0);
        // Called Embedded, but really just means get from an external event!
        partLevelTracks04Task_MC->SetIsEmbedding(kTRUE);
        }
      } // isMC
    } // bDoChargedJets

  // Find Full jets
  AliEmcalJetTask* pFuJet02Task = NULL;
  AliEmcalJetTask* pFuJet04Task = NULL;
  AliEmcalJetTask* pFuJet02Task_MC = NULL;
  AliEmcalJetTask* pFuJet04Task_MC = NULL;
  if (bDoFullJets) {
    pFuJet02Task = AliEmcalJetTask::AddTaskEmcalJet(name_tracks.Data(), name_clusters.Data(), antikt, 0.2, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet02Task->SelectCollisionCandidates(kSel_full);
    pFuJet02Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

    pFuJet04Task = AliEmcalJetTask::AddTaskEmcalJet(name_tracks.Data(), name_clusters.Data(), antikt, 0.4, fulljet, 0.15, 0.30, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
    pFuJet04Task->SelectCollisionCandidates(kSel_full);
    pFuJet04Task->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

    // runEmbeddingAnalysis.C - seems that for mc there is no setting of cluster containers
    if (isMC) {
      pFuJet02Task_MC = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", antikt, 0.2, fulljet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
      pFuJet02Task_MC->SelectCollisionCandidates(kSel_full);

      pFuJet04Task_MC = AliEmcalJetTask::AddTaskEmcalJet("mcparticles", "", antikt, 0.4, fulljet, 0.15, 0, kGhostArea, recomb, "Jet", 1., kFALSE, kFALSE);
      pFuJet04Task_MC->SelectCollisionCandidates(kSel_full);

      if (bDoEmbedding) {
        pFuJet02Task_MC->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);
        AliParticleContainer* partLevelTracks02Task_MC = pFuJet02Task_MC->GetParticleContainer(0);
        // Called Embedded, but really just means get from an external event!
        partLevelTracks02Task_MC->SetIsEmbedding(kTRUE);

        pFuJet04Task_MC->SetRecycleUnusedEmbeddedEventsMode(internalEventSelection);
        AliParticleContainer* partLevelTracks04Task_MC = pFuJet04Task_MC->GetParticleContainer(0);
        // Called Embedded, but really just means get from an external event!
        partLevelTracks04Task_MC->SetIsEmbedding(kTRUE);
        }
      } // isMC
    } // bDoFullJets

////////////////////////
//   ANALYSIS TASKS   //
////////////////////////

  //###   CDF task - charged jets
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFchg = NULL;
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFchg_MC = NULL;
  if (bDoChargedJets) {
    anaTaskCDFchg = CDF::AddTaskEmcalJetCDF ( name_tracks.Data(), "", "", "", "CDFchg" );
    anaTaskCDFchg->SetHistoBins(600, 0, 300);
    anaTaskCDFchg->SelectCollisionCandidates(kSel_chg);
    anaTaskCDFchg->SetDebugLevel(debug);
    anaTaskCDFchg->SetNCentBins(1);

    AliParticleContainer* anaTaskCDFchg_partCont = anaTaskCDFchg->GetParticleContainer(0);
    anaTaskCDFchg_partCont->SetParticlePtCut(0.15);

    if (bDoEmbedding) { anaTaskCDFchg_partCont->SetIsEmbedding(kTRUE); }

    if ( pMultSelTask ) {
      anaTaskCDFchg->SetUseNewCentralityEstimation(bIsRun2);
      anaTaskCDFchg->SetNCentBins(centbins);
      anaTaskCDFchg->SetCentralityEstimator(cent_est_chg.Data());
      }

    //#################################################
    if (isMC){
      anaTaskCDFchg_MC = CDF::AddTaskEmcalJetCDF ( "mcparticles", "", "", "", "CDFchgMC" );
      anaTaskCDFchg_MC->SetHistoBins(600, 0, 300);
      anaTaskCDFchg_MC->SelectCollisionCandidates(kSel_chg);
      anaTaskCDFchg_MC->SetDebugLevel(debug);
      anaTaskCDFchg_MC->SetNCentBins(1);

      AliParticleContainer* anaTaskCDFchg_partCont_MC = anaTaskCDFchg_MC->GetMCParticleContainer(0);
      anaTaskCDFchg_partCont->SetParticlePtCut(0.15);

      if (bDoEmbedding) { anaTaskCDFchg_partCont_MC->SetIsEmbedding(kTRUE); }

      if ( pMultSelTask ) {
        anaTaskCDFchg_MC->SetUseNewCentralityEstimation(bIsRun2);
        anaTaskCDFchg_MC->SetNCentBins(centbins);
        anaTaskCDFchg_MC->SetCentralityEstimator(cent_est_chg.Data());
        }
      }
    }

  //###   CDF task - full jets
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFfull = NULL;
  AliAnalysisTaskEmcalJetCDF* anaTaskCDFfull_MC = NULL;
  if (bDoFullJets) {
    anaTaskCDFfull = CDF::AddTaskEmcalJetCDF ( name_tracks.Data(), name_clusters.Data(), "", "", "CDFfull" );
    anaTaskCDFfull->SetHistoBins(600, 0, 300);
    anaTaskCDFfull->SelectCollisionCandidates(kSel_full);
    anaTaskCDFfull->SetDebugLevel(debug);

    AliParticleContainer* anaTaskCDFfull_partCont = anaTaskCDFfull->GetParticleContainer(0);
    anaTaskCDFfull_partCont->SetParticlePtCut(0.15);

    if (bDoEmbedding) { anaTaskCDFfull_partCont->SetIsEmbedding(kTRUE); }

    if ( pMultSelTask ) {
      anaTaskCDFfull->SetUseNewCentralityEstimation(bIsRun2);
      anaTaskCDFfull->SetNCentBins(centbins);
      anaTaskCDFfull->SetCentralityEstimator(cent_est_full.Data());
      }

    //#################################################
    if (isMC){
      anaTaskCDFfull_MC = CDF::AddTaskEmcalJetCDF ( "mcparticles", "", "", "", "CDFfullMC" );
      anaTaskCDFfull_MC->SetHistoBins(600, 0, 300);
      anaTaskCDFfull_MC->SelectCollisionCandidates(kSel_full);
      anaTaskCDFfull_MC->SetDebugLevel(debug);

      AliParticleContainer* anaTaskCDFfull_partCont_MC = anaTaskCDFfull_MC->GetMCParticleContainer(0);
      anaTaskCDFfull_partCont->SetParticlePtCut(0.15);

      if (bDoEmbedding) { anaTaskCDFfull_partCont_MC->SetIsEmbedding(kTRUE); }

      if ( pMultSelTask ) {
        anaTaskCDFfull_MC->SetUseNewCentralityEstimation(bIsRun2);
        anaTaskCDFfull_MC->SetNCentBins(centbins);
        anaTaskCDFfull_MC->SetCentralityEstimator(cent_est_full.Data());
        }
      }
    }


//########################
//   ANALYSIS TASKS - CONTAINERS SETUP
//########################
// add jet containers to CDF task for charged jets
  if (bDoChargedJets) {
    AliJetContainer* jetcont_chg = NULL;
    for ( Float_t fi = 0 ; fi<=100 ; fi+=10) {
      // CHG JETS 0.2
      jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg), pChJet02Task, acc_chgjets);
      CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);
      if (pRhoTask) {
        jetcont_chg->SetRhoName(sRhoChName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }

      // CHG JETS 0.4
      jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg), pChJet04Task, acc_chgjets);
      CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);
      if (pRhoTask) {
        jetcont_chg->SetRhoName(sRhoChName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }

      if (isMC) {
        // CHG JETS MC 0.2
        jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg_MC), pChJet02Task_MC, acc_chgjets);
        CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);
        if (pRhoTask_MC) {
          jetcont_chg->SetRhoName(sRhoChName_MC.Data());
          // jetcont_chg->SetPercAreaCut(0.6);
          }

        // CHG JETS MC 0.4
        jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg_MC), pChJet04Task_MC, acc_chgjets);
        CDF::jetContSetParams (jetcont_chg,    fi,   fi+10, 0, 0);
        if (pRhoTask_MC) {
          jetcont_chg->SetRhoName(sRhoChName_MC.Data());
          // jetcont_chg->SetPercAreaCut(0.6);
          }
        }
      }

    jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg), pChJet02Task, acc_chgjets);
    CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);
    if (pRhoTask) {
      jetcont_chg->SetRhoName(sRhoChName.Data());
      // jetcont_chg->SetPercAreaCut(0.6);
      }

    jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg), pChJet04Task, acc_chgjets);
    CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);
    if (pRhoTask_MC) {
      jetcont_chg->SetRhoName(sRhoChName.Data());
      // jetcont_chg->SetPercAreaCut(0.6);
      }


    if (isMC) {
      jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg_MC), pChJet02Task_MC, acc_chgjets);
      CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);
      if (pRhoTask_MC) {
        jetcont_chg->SetRhoName(sRhoChName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }

      jetcont_chg = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFchg_MC), pChJet04Task_MC, acc_chgjets);
      CDF::jetContSetParams (jetcont_chg,     1., 500., 0, 0);
      if (pRhoTask_MC) {
        jetcont_chg->SetRhoName(sRhoChName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }
      }

    jetcont_chg = NULL;
    }

  // add jet containers to CDF task for full jets
  if (bDoFullJets) {
    AliJetContainer* jetcont_full = NULL;
    for ( Float_t fi = 0 ; fi<=100 ; fi+=10) {
      // FULL JETS 0.2
      jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull), pFuJet02Task, acc_fulljets);
      CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);
      if (pRhoTask) {
        jetcont_full->SetRhoName(sRhoFuName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }

      // FULL JETS 0.4
      jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull), pFuJet04Task, acc_fulljets);
      CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);
      if (pRhoTask) {
        jetcont_full->SetRhoName(sRhoFuName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }


      if (isMC) {
        // CHG JETS MC 0.2
        jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull_MC), pFuJet02Task_MC, acc_fulljets);
        CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);
        if (pRhoTask_MC) {
          jetcont_full->SetRhoName(sRhoFuName.Data());
          // jetcont_chg->SetPercAreaCut(0.6);
          }

        // CHG JETS MC 0.4
        jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull_MC), pFuJet04Task_MC, acc_fulljets);
        CDF::jetContSetParams (jetcont_full,    fi,   fi+10, 0, 2);
        if (pRhoTask_MC) {
          jetcont_full->SetRhoName(sRhoFuName.Data());
          // jetcont_chg->SetPercAreaCut(0.6);
          }
        }
      }

    jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull), pFuJet02Task, acc_fulljets);
    CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);
    if (pRhoTask) {
      jetcont_full->SetRhoName(sRhoFuName.Data());
      // jetcont_chg->SetPercAreaCut(0.6);
      }

    jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull), pFuJet04Task, acc_fulljets);
    CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);
    if (pRhoTask) {
      jetcont_full->SetRhoName(sRhoFuName.Data());
      // jetcont_chg->SetPercAreaCut(0.6);
      }

    if (isMC) {
      jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull_MC), pFuJet02Task_MC, acc_fulljets);
      CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);
      if (pRhoTask_MC) {
        jetcont_full->SetRhoName(sRhoFuName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }

      jetcont_full = CDF::AddJetContainerJetTask(dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(anaTaskCDFfull_MC), pFuJet04Task_MC, acc_fulljets);
      CDF::jetContSetParams (jetcont_full,     1., 500., 0, 2);
      if (pRhoTask_MC) {
        jetcont_full->SetRhoName(sRhoFuName.Data());
        // jetcont_chg->SetPercAreaCut(0.6);
        }
      }
    jetcont_full = NULL;
    }

  TObjArray* tasks_list = pMgr->GetTasks(); TIter task_iter (tasks_list); AliAnalysisTaskSE* task = NULL;
  while (( task = dynamic_cast<AliAnalysisTaskSE*>(task_iter.Next()) )) {
    if (task->InheritsFrom("AliAnalysisTaskEmcal")) {
      Printf("Setting beam type %d for task %s", iBeamType, static_cast<AliAnalysisTaskEmcal*>(task)->GetName());
      static_cast<AliAnalysisTaskEmcal*>(task)->SetForceBeamType(iBeamType);
      }
    }


//#############################################################################
//#############################################################################
  if ( !pMgr->InitAnalysis() ) { std::cout << ">>>>>>>>>>>>>> AliAnalysisManager Initialising FAILED!!! " << std::endl; return NULL; }
  std::cout << "##-->> Initialising Analysis :: Status :" << std::endl;
  pMgr->PrintStatus();
  if (bUseProgBar) { pMgr->SetUseProgressBar(bUseProgBar, 100); }

  // task profiling
  if ( kUseSysInfo > 0 ) { for ( int i = 0; i < pMgr->GetTopTasks()->GetEntries(); i++ ) { pMgr->ProfileTask (i); } }

//#############################################################################
  if ( ManagerMode == AliAnalysisManager::kLocalAnalysis ) { // start local analysis
    // enable class level debugging for these classes
    if ( debug > 2 ) {
      //pMgr->AddClassDebug("AliJetContainer", 100);
      //pMgr->AddClassDebug("AliEmcalJetTask", 100);
      pMgr->AddClassDebug("AliAnalysisTaskEmcalJetCDF", 100);
      }

    if (!pChain) { std::cout << ">>>>>>>>>>>>>>   StartAnalysis LOCAL :: CHAIN NOT FOUND   <<<<<<<<<<<<<<" << std::endl; return NULL; }
    Printf("Starting LOCAL Analysis...");
    pMgr->StartAnalysis( cAnalysisType, pChain, iNumEvents );
    }

//#############################################################################
  if ( ManagerMode == AliAnalysisManager::kProofAnalysis ) {
//     gProofDebugMask = TProofDebug::kAll;
//     gProofDebugLevel = 5;

    if ( !pMgr->IsProofMode() ) {cout << "WE ARE NOT IN PROOF MODE" << endl; return NULL;}
    Printf("Starting PROOF Analysis...");
    plugin->StartAnalysis(iNumEvents);
    if (gProof) {gProof->Print();} 
    }

//#############################################################################
  if ( ManagerMode == AliAnalysisManager::kGridAnalysis ) {  // start grid analysis
  Printf("Starting GRID Analysis...");

  ListLibs += kGridExtraFiles;
  if ( ListLibs.Length() )       { plugin->SetAdditionalLibs     ( ListLibs.Data() ); }
  if ( ListLibsExtra.Length() )  { plugin->SetAdditionalRootLibs ( ListLibsExtra.Data() ); }

  if ( PluginMode == AliAnalysisGrid::kTest ) {
    if (plugin) { plugin->StartAnalysis(iNumEvents); }
    }
  else {
    pMgr->SetDebugLevel(0);
    CDF::SaveManager("train.root");
    if (plugin) { plugin->StartAnalysis(); }
    }
  } 

cout << "END of EmcalJetCDF.C" << std::endl;
return pMgr;
}

//######################################################################################################################################
//######################################################################################################################################
AliAnalysisAlien* CreateAlienHandler ( const char* gridMode, const char* tag, unsigned int nr_test_files, unsigned int TTL,
                                       const char* outdir, const char* subworkdir, const char* extradirlvl ) {

  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if ( !plugin ) { std::cout << "!!! -->> alien handler could not be created <<-- !!!" << std::endl; return NULL;}

  plugin->AddIncludePath("-I.");
  plugin->AddIncludePath("-I$ALICE_PHYSICS/include");
  plugin->AddIncludePath("-I$ALICE_ROOT/include");
  plugin->AddIncludePath("-I$FASTJET/include");
  plugin->AddIncludePath("-I/usr/include");

  unsigned int       kGridMaxMergeFiles       = 100;            // Number of files merged in a chunk grid run range
  unsigned int       kMaxInitFailed           = 30 ;            // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

  TString     kGridOutdir              = outdir ;       // AliEn output directory. If blank will become output_<kTrainName>
  TString     kGridSubWorkDir          = subworkdir ;   // sub working directory not to confuse different run xmls
  TString     kGridExtraAliendirLevel  = extradirlvl ;  // sub working directory not to confuse different run xmls
  TString     kWorkDir                 = tag;           // AliEn work dir; relative to AliEn $HOME
  TString     kTrainName               = "cdfjets";     // *CHANGE ME* (no blancs or special characters)

  TString macroName("");  macroName = Form("%s.C", kTrainName.Data());
  TString execName("");   execName  = Form("%s.sh", kTrainName.Data());
  TString jdlName("");    jdlName   = Form("%s.jdl", kTrainName.Data());

  // Set run mode.  Can be "full", "test", "offline", "submit" or "merge"
  plugin->SetRunMode(gridMode);
  if (std::strcmp(gridMode, "test")) { plugin->SetCheckCopy(kFALSE); }

  // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
  plugin->SetNtestFiles ( nr_test_files );

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

  // Optionally set time to live
  plugin->SetTTL(TTL);

  // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
  plugin->SetSplitMaxInputFileNumber ( nr_test_files );

  // Use xrootd tweaks to reduce timeouts in file access
  plugin->SetFastReadOption ( kFALSE );

  // Maximum initial consecutive subjobs accepted to fail.
  // plugin->SetMaxInitFailed ( kMaxInitFailed );

  // Failed jobs will be resubmitted until this DONE ratio.
  plugin->SetMasterResubmitThreshold(98);

  // Number of runs per masterjob
  plugin->SetNrunsPerMaster(1); // default value is 1

  // exit from aliensh after submmiting job
  plugin->SetDropToShell ( kFALSE );

  // Overwrite existing files if any
  plugin->SetOverwriteMode();

  // write the output to subdirs named after run number
  plugin->SetOutputToRunNo(1);

  // Optionally set input format (default xml-single)
  plugin->SetInputFormat("xml-single");

  // Optionally modify job price (default 1)
  plugin->SetPrice(1);

  // We split per SE or file
  plugin->SetSplitMode("se"); // default value is "se"

  // MERGING - Enable merging via automatic JDL
  plugin->SetMergeViaJDL(kTRUE);

  // Maximum number of files to be merged in one chunk
  plugin->SetOneStageMerging(kFALSE);

  // Maximum number of merging stages
  plugin->SetMaxMergeStages(2);

  // Optionally set maximum number of files merged in a chunk (default 100, put 0 to ignore)
  // plugin->SetMaxMergeFiles ( kGridMaxMergeFiles );

  return plugin;
}

//##################################################
void LoadLibs (TString& listlibs, TString& listlibsextra) {
  gSystem->AddIncludePath("-I. -I$ALICE_PHYSICS/include -I$ALICE_ROOT/include -I$FASTJET/include -I/usr/include");
  gSystem->AddDynamicPath("$ALICE_PHYSICS/lib:$ALICE_ROOT/lib:$ROOTSYS/lib:$FASTJET/lib:/usr/lib64");

  TString list_fj         = "CGAL fastjet siscone siscone_spherical fastjetplugins fastjettools fastjetcontribfragile";
  TString list_alicejets  = "PWGPP PWGJEEMCALJetTasks";
  TString libs = list_fj + " " + list_alicejets;

  LoadLibList (libs, listlibs, listlibsextra);
  cout << "EmcalJetCDF::LoadLibs done!" << endl;
  }

//##################################################
bool LoadLibList ( const TString& list, TString& listlibs, TString& listlibsextra ) {
  TObjArray* arr = list.Tokenize(" ");
  TObjString *objstr = NULL;

  cout << "Loading library list (-2 = version mismatch, -1 = not found or error, 0 = success, 1 = already loaded) :" << endl;
  TIter next(arr);
  while ( (objstr=(TObjString*)next()) ) {
      TString module = objstr->GetString();
      if (module.IsNull()) { continue; }
      module.Prepend("lib");
      if(module.EndsWith(".so")) { module.Remove( module.Index(".so")); }
      std::cout << "loading module :" << module.Data() << " ... " ;
      int result = gSystem->Load(module.Data(), "", kTRUE);
      std::cout << result << std::endl; ;
      if (result < 0) { std::cout << "EmcalJetCDF::LoadLibList - Could not load library >>>"  << module.Data() << "<<< ; Error = " << result << std::endl; return false; }

      TString lib_in_list = module + ".so "; // blank after .so
      listlibs      += lib_in_list;
      listlibsextra += lib_in_list;
      }
  delete arr;
  return true;
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

