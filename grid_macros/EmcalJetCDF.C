#ifndef __CINT__

// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>
#include <cstdio>

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

#include "AliAnalysisTaskRho.h"
#include "AliAnalysisTaskEMCALClusterizeFast.h"

#include "AliPhysicsSelectionTask.h"
#include "AliCentralitySelectionTask.h"

#include "AliEMCALTenderSupply.h"
#include "AliEmcalCompatTask.h"
#include "AliEmcalSetupTask.h"
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
#include "AliAnalysisTaskEmcalJetSpectraQA.h"

#include "AliAnalysisTaskEmcalJetCDF.h"
#include "AliAnalysisTaskEmcalJetSample.h"

// Macro locations
#include "../OADB/macros/AddTaskCentrality.C"

#include "../ANALYSIS/macros/train/AddAODHandler.C"
#include "../ANALYSIS/macros/train/AddESDHandler.C"
#include "../ANALYSIS/macros/train/AddMCHandler.C"

#include "../PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalCompat.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalSetup.C"
#include "../PWG/EMCAL/macros/AddTaskEMCALTender.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalPreparation.C"
#include "../PWG/EMCAL/macros/AddTaskClusterizerFast.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalClusterMaker.C"
#include "../PWG/EMCAL/macros/AddTaskEmcalClusTrackMatcher.C"
#include "../PWG/EMCAL/macros/AddTaskHadCorr.C"

#include "../OADB/macros/AddTaskPhysicsSelection.C"

#include "../PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSpectraQA.C"

#include "../PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDF.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDFUE.C"
#include "../PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C"

#include "InputData.C"

#endif

class AliAnalysisTaskEmcalJetCDF;
class AliESDInputHandler;
class AliAODInputHandler;
class AliVEvent;
class AliAnalysisManager;
class AliEmcalPhysicsSelectionTask;
class AliCentralitySelectionTask;
class AliEmcalSetupTask;
class AliJetContainer;

//______________________________________________________________________________
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@     ANALYSIS STEERING VARIABLES     @@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

const Int_t       kTestFiles               = 6000;          // Number of test files
const Long64_t    nentries                 = 1234567890; // for local and proof mode, ignored in grid mode. Set to 1234567890 for all events.

// SETUP THE ANALYSIS TYPE
const Bool_t   chgJets  = kTRUE;
const Bool_t   fullJets = kFALSE;
const Bool_t   mcJets   = kFALSE;

Bool_t   bDoTender  = kFALSE;
Bool_t   bDoHadCorr = kFALSE;

Bool_t   isMC       = kFALSE;   // trigger, if MC handler should be used
Bool_t   doBkg      = kFALSE;   // background estimation with AliAnalysisTaskRho

const Bool_t  useSample = kTRUE;
const Bool_t  useCDF = kTRUE;
const Bool_t  bar = kFALSE;

// ######   DEBUG    ######
const Int_t           debug              =  6 ; // kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
const UInt_t          mgr_debug          =  100 ; // AliAnalysisManager debug level
const UInt_t          kUseSysInfo        =  10 ; // activate debugging
const Int_t           kErrorIgnoreLevel  = -1 ; // takes the errror print level from .rootrc

//==============================================================================
const UInt_t  pSel                = AliVEvent::kAnyINT | AliVEvent::kCentral | AliVEvent::kSemiCentral;  // used event selection for every task except for the analysis tasks
const UInt_t  EMCALpSel           = AliEmcalPhysicsSelection::kEmcalOk;
// AliEmcalPhysicsSelection::kEmcalOk, AliEmcalPhysicsSelection::kEmcalH,
// AliVEvent::kINT7, AliVEvent::kMB, AliVEvent::kCentral, AliVEvent::kSemiCentral,
// AliVEvent::kEMCEGA, AliVEvent::kEMCEJE


Int_t          leadhadtype         = 0;             // AliJetContainer :: Int_t fLeadingHadronType;  0 = charged, 1 = neutral, 2 = both

const char*    cOCDBpath = "uselocal";   // change to "raw://" if running on the grid

Bool_t         bIsRun2             = kFALSE;
Bool_t         tracks_etaphi_cuts  = kFALSE;        // fiducial acceptance cuts on jet constituents (tracks)

//#################################################################################################

//    FORWARD DECLARATIONS
void                     AddIncludePaths ();
void                     LoadLibs ();
void                     PrepareAnalysisEnvironment();
void                     LoadMacros ();

void                     LoadLibList      ( const TString& list );
// void                     SetJFAccFid      ( const char* taskname, TString& cut ("TPC") );
// void                     SetCDFAccFid     ( const char* taskname, Int_t i = 0 , TString& cut ("TPC"));
void                     PrintInfoJF      ( const char* taskname );
void                     PrintInfoCDFtask ( const char* taskname, Int_t i = 0 );

AliAnalysisAlien*        CreateAlienHandler ( const char* plugin_mode = "test" );
std::vector<TString>     GetJetFinderList();

Bool_t                   LoadLibrary ( const TString& lib );
Bool_t                   IsTreeType  ( const TString& fileInput, const TString& treeName );
Bool_t                   PeriodIsMC  ( const TString& period );

TString                  FindTreeName     ( const TString& file_list ) const;
TString                  GetInputDataPath ( const TString& file_list );
TString                  GetPeriod        ( const TString& file_path );
TString                  GetPass          ( const TString& file_path );


//______________________________________________________________________________
// enum Jets used
enum eDataType { kAod, kEsd };
enum fitModulationType { kNoFit, kV2, kV3, kCombined, kFourierSeries, kIntegratedFlow, kQC2, kQC4 }; // AliAnalysisTaskLocalRho
enum VCluUserDefEnergy_t { kNonLinCorr = 0, kHadCorr = 1, kUserDefEnergy1 = 2, kUserDefEnergy2 = 3, kLastUserDefEnergy = 4 }; // AliVCluster.h

//______________________________________________________________________________
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//        AliEN plugin variables
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//______________________________________________________________________________
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

TString     kWorkDir                 = "emcalcdf";    // AliEn work dir; relative to AliEn $HOME
TString     kTrainName               = "sevjets";     // *CHANGE ME* (no blancs or special characters)
TString     kJobTag                  = "sevcdfjet";   // *CHANGE ME*

TString     kPluginExecutableCommand = "aliroot -b -q";
Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

TString     kAPIVersion              = "V1.1x";
TString     kRootVersion             = "v5-34-30-alice-24";
TString     kAliRootVersion          = "v5-08-12-1";
TString     kAliPhysicsVersion       = "vAN-20160619-1";

TString     kPackage1                = "boost::v1.59.0-7";
TString     kPackage2                = "cgal::v4.6.3-10";
TString     kPackage3                = "fastjet::v3.1.3_1.020-11";
TString     kPackage4                = "GEANT3::v2-1-1";
// TString     kPackage5                = "jemalloc::v3.6.0";

Bool_t      kPluginFastReadOption    = kFALSE;         // use xrootd tweaks
Bool_t      kPluginOverwriteMode     = kTRUE;          // overwrite existing collections

Bool_t      kSkipTerminate           = kTRUE;          // Do not call Terminate
Bool_t      kUsePAR                  = kFALSE;         // use par files for extra libs

Int_t       kGridFilesPerJob         = 35;             // Maximum number of files per job (gives size of AOD)
Int_t       kGridMaxMergeFiles       = 100;            // Number of files merged in a chunk grid run range
Int_t       kMaxInitFailed           = 10 ;            // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

Int_t       kTTL                     = 28800 ;         // Time To Live
Int_t       kMergeTTL                = 1800 ;          // Time To Live merging

TString     kGridOutdir              = "out";          // AliEn output directory. If blank will become output_<kTrainName>
TString     kGridDataSet             = "";             // sub working directory not to confuse different run xmls
TString     kGridExtraAliendirLevel  = "";             // sub working directory not to confuse different run xmls

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

//==============================================================================
// Global objects to be access by utility functions
TString kInputDataLabel = "";

// Containers for IO file names
TString kDataset = "";
TString kDatafile = "";

// definition of variables to be used later - autodetected
TString     runPeriod           = ""; // run period (to be determined automatically form path to files; if path is not GRID like, set by hand)
TString     dataType            = ""; // analysis type, AOD, ESD or sESD (to be automatically determined below; if detection does not work, set by hand after detection)
TString     kPass               = ""; // pass string

TString     kPluginMode     = ""; // test, offline, submit, terminate, full
TString     kAnalysisMode   = ""; // local, grid, proof
TString     kInputStr       = ""; // input string

//==============================================================================

// FILES USED IN MACRO
TString     kCommonOutputFileName    = "AnalysisResults.root";

// == grid plugin files rules
TString     kGridExtraFiles          = ""; // LIBRARIES files that will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

// == Lists of libraries, pars and sources to be added to the alien plugin
TString     ListPars      = "";
TString     ListLibs      = "";
TString     ListLibsExtra = "";
TString     ListSources   = "";

//compile and load the custom local task in local macro
bool useLocal = false;
bool localMacros = false;

//__________________________________________________________________________________
// Objects (branch names) used in Jet framework
const TString  trk_str ("usedefault");
const TString clus_str ("usedefault");
const TString cell_str ("usedefault");
TString rhoName  ("");

const Double_t kHadCorrF            = 2.;

// Default values
AliJetContainer::EJetType_t               jet_type = AliJetContainer::kChargedJet;       // kFullJet, kChargedJet, kNeutralJet
AliJetContainer::JetAcceptanceType acceptance_type = AliJetContainer::kTPCfid;           // kTPC, kTPCfid, kEMCAL, kEMCALfid, kDCAL, kDCALfid, kUser
AliJetContainer::EJetAlgo_t                   algo = AliJetContainer::antikt_algorithm;
AliJetContainer::ERecoScheme_t        recombScheme = AliJetContainer::pt_scheme;

AliAnalysisManager* EmcalJetCDF (const char* analysis_mode = "local", const char* plugin_mode = "test", const char* input = "data.txt")
  {
  cout << "Beging of EmcalJetCDF" << endl;

  // set function arguments
  kPluginMode   = plugin_mode   ; kPluginMode.ToLower();    // test, offline, submit, terminate, full
  kAnalysisMode = analysis_mode ; kAnalysisMode.ToLower();  // local, grid, proof
  kInputStr     = input;

  // if analysis is done on localhost do not use PARs
  if ( kAnalysisMode.EqualTo ("local") || kPluginMode.EqualTo ("test")  ) { kUsePAR = kFALSE; }

  // if acceptance cut on input contituents then do not cut in acceptance of jets
  //if ( tracks_etaphi_cuts ) { acceptance_type = AliJetContainer::kTPCfid;}

  // PREPARE Includes, libs and paths
  cout << "Beging of loading Libs and Macros" << endl;
  LoadLibs(); // Load necessary libraries for the script and for the plugin
  LoadMacros(); // load all macros that might be nedeed

  //*******************************************
  //   Loading of libraries (script + plugin)
  //*******************************************
  AliAnalysisAlien* plugin = CreateAlienHandler ( kPluginMode.Data() );             // ###   SET UP AliEn handler ###
  AliAnalysisManager* mgr  = plugin->CreateAnalysisManager ( "CDFhistos_mgr" );     // ###   ANALYSIS MANAGER     ###

  // NOTE!!! after the custom task part to be picked up and loaded by alien plugin
  PrepareAnalysisEnvironment();

  //####################################################
  //###        Conditions preparation
  //####################################################

  // enable tender and hadronic correction in case of full jets
  if ( fullJets ) { bDoTender = kTRUE; bDoHadCorr = kTRUE; }

  AliAnalysisTaskEmcal::BeamType iBeamType = AliAnalysisTaskEmcal::kpp;

  if ( runPeriod.EqualTo("lhc10h") || runPeriod.EqualTo("lhc11h") || runPeriod.EqualTo("lhc15o") )
    { iBeamType = AliAnalysisTaskEmcal::kAA; }
  else
  if ( runPeriod.EqualTo("lhc12g") || runPeriod.EqualTo("lhc13b") || runPeriod.EqualTo("lhc13c") ||
       runPeriod.EqualTo("lhc13d") || runPeriod.EqualTo("lhc13e") || runPeriod.EqualTo("lhc13f") )
    { iBeamType = AliAnalysisTaskEmcal::kpA; }

  if ( runPeriod.Length() == 6 && runPeriod.BeginsWith("lhc15") ) { bIsRun2 = kTRUE; }
  Bool_t bSeparateEMCalDCal = bIsRun2;

  Double_t kGhostArea = 0.01;
  if (iBeamType != AliAnalysisTaskEmcal::kpp) { kGhostArea = 0.005; }

  AliTrackContainer::SetDefTrackCutsPeriod(runPeriod.Data());
  Printf("Default track cut period set to: %s", AliTrackContainer::GetDefTrackCutsPeriod().Data());

  const AliAnalysisTaskEmcalJetSpectraQA::EHistoType_t kHistoType = AliAnalysisTaskEmcalJetSpectraQA::kTHnSparse;

//####################################################
//#######        ANALYSIS TASKS LIST
//####################################################

// Physics selection task
  if ( dataType.EqualTo("esd") )
    { AliPhysicsSelectionTask *pPhysSelTask = AddTaskPhysicsSelection(); }

// Centrality task
  if ( dataType.EqualTo("esd") && (iBeamType != AliAnalysisTaskEmcal::kpp) )
    {
    AliCentralitySelectionTask* centralityTask = AddTaskCentrality ( kTRUE );
    centralityTask->SelectCollisionCandidates (AliVEvent::kAny);
    }

// Setup task
  if ( fullJets || dataType.EqualTo("esd") )
    {
    cout << "\n---->EmcalSetup ..." << endl;
    AliEmcalSetupTask* emcalsetupTask = AddTaskEmcalSetup();
    emcalsetupTask->SelectCollisionCandidates(AliVEvent::kAny);
    emcalsetupTask->SetOcdbPath(cOCDBpath);
    cout << "---->EmcalSetup DONE!\n" << endl;
    }

  if ( bDoTender )
    {
    cout << "\n---->Setup tender ..." << endl;

    // Tender
    // Only cell energy/time recalibration (and bad channel) is switched on
    Bool_t calibEnergy    = kTRUE;         // calibrate energy
    Bool_t calibTime      = kTRUE;         // calibrate timing
    Bool_t remBC          = kTRUE;         // remove bad channels
    Bool_t updateCellOnly = kTRUE;         // only change if you run your own clusterizer task

    // All these parameters are irrelevant for the tender
    Bool_t distBC         = kFALSE;        // switch for recalculation cluster position from bad channel
    Bool_t recalibClus    = kFALSE;        // recalibrate cluster energy
    Bool_t recalcClusPos  = kFALSE;        // recalculate cluster position
    Bool_t nonLinearCorr  = kFALSE;        // apply non-linearity
    Bool_t remExoticCell  = kFALSE;        // remove exotic cells
    Bool_t remExoticClus  = kFALSE;        // remove exotic clusters
    Bool_t fidRegion      = kFALSE;        // apply fiducial cuts

    Bool_t reclusterize   = kFALSE;        // kTRUE;   //reclusterize
    Float_t seedthresh    = 0.100;         // seed threshold - 100 MeV
    Float_t cellthresh    = 0.050;         // cell threshold - 50 MeV
    Bool_t trackMatch     = kFALSE;        // kTRUE;   // track matching
    Float_t timeMin       = 0;             // minimum time of physical signal in a cell/digit (s)
    Float_t timeMax       = 1e6;           // maximum time of physical signal in a cell/digit (s)
    Float_t timeCut       = 1e6;           // maximum time difference between the digits inside EMC cluster (s)
    Bool_t  remapMcAod    = kFALSE;        // switch on the remaping for the MC labels in AOD productions,
    const char* pass      = kPass.Data();  // string defining pass (use none if figured out from path)

    UInt_t clusterizer    = AliEMCALRecParam::kClusterizerv2;
    UInt_t nonLinFunct    = AliEMCALRecoUtils::kNoCorrection; // default : AliEMCALRecoUtils::kBeamTestCorrected;

    AliAnalysisTaskSE* pTenderTask = AddTaskEMCALTender(
    distBC, recalibClus, recalcClusPos, nonLinearCorr, remExoticCell, remExoticClus, fidRegion,
    calibEnergy, calibTime, remBC, nonLinFunct, reclusterize, seedthresh, cellthresh, clusterizer,
    trackMatch, updateCellOnly, timeMin, timeMax, timeCut, pass);

    pTenderTask->SelectCollisionCandidates(pSel);

    // Time cuts are switched off at cell level
    Float_t timeMin       = -50e-6;
    Float_t timeMax       =  50e-6;
    Float_t timeCut       =   1e6;

    AliAnalysisTaskEMCALClusterizeFast *pClusterizerTask = AddTaskClusterizerFast(
    "ClusterizerFast", "", "", clusterizer, 0.05, 0.1, timeMin, timeMax, timeCut,
    kFALSE, kFALSE, AliAnalysisTaskEMCALClusterizeFast::kFEEData);

    pClusterizerTask->SelectCollisionCandidates(pSel);

    remExoticClus  = kTRUE;
    nonLinFunct    = AliEMCALRecoUtils::kBeamTestCorrected;

    AliEmcalClusterMaker* pClusterMakerTask = AddTaskEmcalClusterMaker(nonLinFunct, remExoticClus, 0, "", 0., kTRUE);
    pClusterMakerTask->GetClusterContainer(0)->SetClusPtCut(0.);
    pClusterMakerTask->GetClusterContainer(0)->SetClusECut(0.);
    pClusterMakerTask->SelectCollisionCandidates(pSel);

    // Cluster-track matcher task
    AliEmcalClusTrackMatcherTask *pMatcherTask = AddTaskEmcalClusTrackMatcher("usedefault", "usedefault", 0.1, kFALSE, kTRUE, kTRUE, kTRUE);
    pMatcherTask->SelectCollisionCandidates(pSel);
    pMatcherTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
    pMatcherTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.15);
    pMatcherTask->GetClusterContainer(0)->SetClusECut(0.);
    pMatcherTask->GetClusterContainer(0)->SetClusPtCut(0.);

    if (dataType.EqualTo("esd")) { pMatcherTask->SetDoPropagation(kTRUE); }

    cout << "---->Setup tender DONE!\n" << endl;
    }

  if (bDoHadCorr) {
    cout << "\n---->Setup hadronic corrections ..." << endl;
    // Hadronic correction task
    AliHadCorrTask *pHadCorrTask = AddTaskHadCorr("usedefault", "usedefault", "", kHadCorrF, 0.15, 0.030, 0.015, 0, kTRUE, kTRUE);
    pHadCorrTask->SelectCollisionCandidates(pSel);
    pHadCorrTask->GetClusterContainer(0)->SetClusNonLinCorrEnergyCut(0.15);
    pHadCorrTask->GetClusterContainer(0)->SetClusECut(0);
    pHadCorrTask->GetClusterContainer(0)->SetClusPtCut(0.);
    cout << "---->Setup hadronic corrections DONE!\n" << endl;
    }

  // Background
  TString sRhoChName; TString sRhoFuName;
  if (iBeamType != AliAnalysisTaskEmcal::kpp)
    {
    sRhoChName = "Rho";
    sRhoFuName = "Rho_Scaled";

    AliEmcalJetTask *pKtChJetTask = AddTaskEmcalJet("usedefault", "", AliJetContainer::kt_algorithm, 0.4, AliJetContainer::kChargedJet, 0.15, 0, kGhostArea, AliJetContainer::pt_scheme, "Jet", 0., kFALSE, kFALSE);
    pKtChJetTask->SelectCollisionCandidates(pSel);

    AliAnalysisTaskRho* pRhoTask = AddTaskRhoNew("usedefault", "usedefault", sRhoChName, 0.4);
    pRhoTask->SetExcludeLeadJets(2);
    pRhoTask->SelectCollisionCandidates(pSel);

    if (fullJets)
      {
      TString sFuncPath = "alien:///alice/cern.ch/user/s/saiola/LHC11h_ScaleFactorFunctions.root";
      TString sFuncName = "LHC11h_HadCorr20_ClustersV2";
      pRhoTask->LoadRhoFunction(sFuncPath, sFuncName);
      }
    }


// ################# Now: Add jet finders+analyzers
    Double_t       minTrPt         = 0.15;                    // default: 0.15  // min jet track momentum   (applied before clustering)
    Double_t       minClPt         = 0.30;                    // default: 0.30  // min jet cluster momentum (applied before clustering)
    const char*    tag             = "Jet";                   // default: "Jet"
    Double_t       minJetPt        = 1.;                      // default: 0.
    Bool_t         lockTask        = kFALSE;                  // default: kTRUE
    Bool_t         bFillGhosts     = kFALSE;

    //     AliEmcalJetTask* jetFinderTask;
    AliEmcalJetTask* jf = NULL;

    // List off all jetfinders in order to use analysis tasks for each of them
    std::vector<TString> jf_names;

    Double_t radius_list[] = { 0.2, 0.4, 0.6 }; // for each radius make a jetfinder

    std::size_t rnr = sizeof(radius_list)/sizeof(radius_list[0]);
    for (std::size_t j = 0; j < rnr; j++ )
        {
        TString jftaskname ("");
        if ( chgJets )
          {
          jet_type = AliJetContainer::kChargedJet; minClPt = 0. ;
          jf = AddTaskEmcalJet( trk_str.Data(), "", algo, radius_list[(unsigned int)j], jet_type, minTrPt, minClPt, kGhostArea, recombScheme, tag, minJetPt, lockTask, bFillGhosts);
          jf->SelectCollisionCandidates(pSel);

          jftaskname = jf->GetName();
          PrintInfoJF ( jftaskname.Data() );
          jf_names.push_back( jftaskname );
          }

        if ( fullJets )
          {
          jet_type = AliJetContainer::kFullJet; minClPt = 30. ;
          jf = AddTaskEmcalJet( trk_str.Data(), clus_str.Data(), algo, radius_list[(unsigned int)j], jet_type, minTrPt, minClPt, kGhostArea, recombScheme, tag, minJetPt, lockTask, bFillGhosts);
          jf->SelectCollisionCandidates(pSel);
          jf->GetClusterContainer(0)->SetDefaultClusterEnergy(AliVCluster::kHadCorr);

          jftaskname = jf->GetName();
          PrintInfoJF ( jftaskname.Data() );
          jf_names.push_back( jftaskname );
          }
        }
    jf = NULL; // make sure this pointer is not used anymore


//#####################################################################################
// Add to plugin custom include paths
AddIncludePaths(); // add system include paths

  // compile local task if selected
  if (useLocal)
    {
    TString myTask = "AliAnalysisTaskEmcalJetCDF.cxx";

    // Load aditional code (my task) to alien plugin ;
    ListSources += " " + myTask; // extra space at beginning in case is not first source; it will be stripped.

    if ( kAnalysisMode.EqualTo("local") ) { myTask += "+g"  ;}
    if ( kAnalysisMode.EqualTo("grid") || kAnalysisMode.EqualTo("proof") ) { myTask += "++";}
    if ( gROOT->LoadMacro ( myTask.Data() ) != 0 )  { Printf ( "--->>> !!! compilation failed !!! <<<---" ) ; return; }
    }

AddIncludePathsPlugin(); // Add include paths for plugin

//#######################################################################
//   Add analysis tasks
  Double_t     jetareacut           = 0.00 ;
  const char*  prefixcdf            = "CDF";

if ( useSample ) {
  //### Sample task
  AliAnalysisTaskEmcalJetSample* sampleTask = AddTaskEmcalJetSample(trk_str.Data(), clus_str.Data(), cell_str.Data());
  sampleTask->GetParticleContainer(0)->SetParticlePtCut(0.15);
  sampleTask->SetHistoBins(600, 0, 300);
  sampleTask->SelectCollisionCandidates(pSel);

  AliClusterContainer* cluscont_sample = sampleTask->GetClusterContainer(0);
  if ( fullJets && cluscont_sample )
    {
    cout << "sample Task :: found cluster container" << endl;
    cluscont_sample->SetClusECut(0.);
    cluscont_sample->SetClusPtCut(0.);
    cluscont_sample->SetClusNonLinCorrEnergyCut(0.);
    cluscont_sample->SetClusHadCorrEnergyCut(0.30);
    cluscont_sample->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }
}

if ( useCDF ) {
  //###   CDF task
  AliAnalysisTaskEmcalJetCDF* anaTaskCDF = dynamic_cast<AliAnalysisTaskEmcalJetCDF*>NS_AliAnalysisTaskEmcalJetCDF::AddTaskEmcalJetCDF ( trk_str.Data(), clus_str.Data(), cell_str.Data() ,"CDF" );
  if (!anaTaskCDF) { cout << "Could not add EmcalJetCDF; task = " << anaTaskCDF->GetName() << endl; gApplication->Terminate(); }
  anaTaskCDF->GetParticleContainer(0)->SetParticlePtCut(0.15);
  anaTaskCDF->SelectCollisionCandidates(pSel);
  anaTaskCDF->SetDebugLevel(debug);

  AliClusterContainer* cluscont_cdf = anaTaskCDF->GetClusterContainer(0);
  if ( fullJets && cluscont_cdf )
    {
    cout << "CDF Task :: found cluster container" << endl;
    cluscont_cdf->SetClusECut(0.);
    cluscont_cdf->SetClusPtCut(0.);
    cluscont_cdf->SetClusNonLinCorrEnergyCut(0.);
    cluscont_cdf->SetClusHadCorrEnergyCut(0.30);
    cluscont_cdf->SetDefaultClusterEnergy(AliVCluster::kHadCorr);
    }
}
cout << "End tasks initialisation\n" << endl;

//#######################################################################
//   Add jet containers to analysis tasks
  AliJetContainer*               jetCont       = NULL;
  Double_t jetpt_cuts[] =
                          {1., 5., 10. ,15., 20., 25., 30., 35., 40., 55., 60., 65., 70.};
                          //{1., 5., 10. ,15., 20., 25., 30.};

  Size_t nrcuts = sizeof(jetpt_cuts)/sizeof(jetpt_cuts[0]); // number of cuts;

  //###   Adding the jet containers
  AliJetContainer* jetCont = NULL;

  // loop over jet finders (their names are in std::vector<TString> jf_names )
  cout << "There are " << jf_names.size() << " jet finders in list" << endl;
  for ( std::vector<TString>::iterator jf_it = jf_names.begin(); jf_it != jf_names.end(); ++jf_it)  // loop over jet finders
    {
    AliEmcalJetTask* jf_task = dynamic_cast<AliEmcalJetTask*>(mgr->GetTask( (*jf_it).Data() ));
    if (!jf_task) { AliError("No jet finder with the name from jf_names list"); continue;}

    // casting does not work
    UInt_t jett_tmp = jf_task->GetJetType();        const AliJetContainer::EJetType_t    jettype_t = jett_tmp;
    Int_t reco_tmp  = jf_task->GetRecombScheme();   const AliJetContainer::ERecoScheme_t  recomb_t = reco_tmp;
    UInt_t algo_tmp = jf_task->GetJetAlgo();        const AliJetContainer::EJetAlgo_t    jetalgo_t = algo_tmp;

    if ( jettype_t == AliJetContainer::kChargedJet ) { acceptance_type = AliJetContainer::kTPCfid; leadhadtype = 0; }
    else
    if ( jettype_t == AliJetContainer::kFullJet )    { acceptance_type = AliJetContainer::kEMCALfid; leadhadtype = 2; }
    else
      { AliError("Unknown jet type"); continue;}

    const TString jf_name = jf_task->GetName();
    const Double_t r      = jf_task->GetRadius();

    PrintInfoJF (jf_name.Data());

    // #######   SAMPLE TASK   #########################################################################
    if ( useSample ) {
      jetCont = sampleTask->AddJetContainer(jettype_t, jetalgo_t, recomb_t, r, acceptance_type, "Jet");

      if ( (iBeamType != AliAnalysisTaskEmcal::kpp) ) {
        jetCont->SetPercAreaCut(0.6);
        if (jettype_t == AliJetContainer::kChargedJet) { jetCont->SetRhoName(sRhoChName); }
        if (jettype_t == AliJetContainer::kFullJet)    { jetCont->SetRhoName(sRhoFuName); }
        }
      }
    // #################################################################################################

    // #######   CDF TASK   #########################################################################
    Double_t jetptmin = 0. ; Double_t jetptmax = 0.;
    for (Size_t k = 0; k < (Size_t)nrcuts; k++ )  // loop over all jet pt bins - one jet container per pt bin
      {
      jetptmin = jetpt_cuts[(unsigned int)k];
      jetptmax = ((unsigned int)k == ( nrcuts - 1)) ? 500. : jetpt_cuts[(unsigned int)k+1]; // if last cut, max is unlimited

      if ( useCDF ) {
        jetCont = anaTaskCDF->AddJetContainer( jettype_t, jetalgo_t, recomb_t, r, acceptance_type, "Jet");
        if ( !jetCont ) { std::cout << "AddTaskEmcalJetCDF.C :: could not add jetCont" << std::endl; return NULL; }
        jetContSetPropreties (jetCont, jetptmin, jetptmax, jetareacut, leadhadtype ); // 1, 0.15, 1000 (NLeadingJets, track_ptmin, track_ptmax)

        if ( (iBeamType != AliAnalysisTaskEmcal::kpp) ) {
          jetCont->SetPercAreaCut(0.6);
          if (jettype_t == AliJetContainer::kChargedJet) { jetCont->SetRhoName(sRhoChName); }
          if (jettype_t == AliJetContainer::kFullJet)    { jetCont->SetRhoName(sRhoFuName); }
          }
        }
      }
    // #################################################################################################

    jetptmin = 1.; jetptmax = 500.; // jet container with all jets (full pt range)
    if ( useCDF ) {
      AliJetContainer* jetCont = anaTaskCDF->AddJetContainer( jettype_t, jetalgo_t, recomb_t, r, acceptance_type, "Jet");
      if ( !jetCont ) { std::cout << "AddTaskEmcalJetCDF.C :: could not add jetCont" << std::endl; return NULL; }
      jetContSetPropreties (jetCont, jetptmin, jetptmax, jetareacut, leadhadtype ); // 1, 0.15, 1000 (NLeadingJets, track_ptmin, track_ptmax)

      if ( (iBeamType != AliAnalysisTaskEmcal::kpp) ) {
        jetCont->SetPercAreaCut(0.6);
        if (jettype_t == AliJetContainer::kChargedJet) { jetCont->SetRhoName(sRhoChName); }
        if (jettype_t == AliJetContainer::kFullJet)    { jetCont->SetRhoName(sRhoFuName); }
        }
      }

    }

    // enable class level debugging for these classes
    if ( debug > 3 ) { mgr->AddClassDebug("AliJetContainer", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliEmcalJetTask", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliAnalysisTaskEmcalJetCDF", 100); }

//#################################################################
    // Set the physics selection for all given tasks
    TObjArray* toptasks = mgr->GetTasks();
    for ( Int_t i = 0; i <= toptasks->GetLast(); i++ )
        {
        AliAnalysisTaskSE* task = dynamic_cast<AliAnalysisTaskSE*> ( toptasks->At (i) );
        if ( !task ) { continue; }
        if ( task->InheritsFrom ( "AliAnalysisTaskEmcal" ) )
          {
          AliAnalysisTaskEmcal *pTaskEmcal = static_cast<AliAnalysisTaskEmcal*>(task);
          Printf("Setting beam type %d for task %s", iBeamType, pTaskEmcal->GetName());
          pTaskEmcal->SetForceBeamType(iBeamType);
          }
        }

//==========================================
// ######       START ANALYSIS       #######
//==========================================
  if (!mgr->InitAnalysis()) { return NULL; }

  cout << "##-->> Initialising Analysis :: Status :" << endl;
  mgr->PrintStatus();

  if (bar) { mgr->SetUseProgressBar(kTRUE, 100); }

  if ( debug == 0 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal ); }
  else
  if ( debug == 1 ) { AliLog::SetGlobalLogLevel ( AliLog::kError ); }
  else
  if ( debug == 2 ) { AliLog::SetGlobalLogLevel ( AliLog::kWarning ); }
  else
  if ( debug == 3 ) { AliLog::SetGlobalLogLevel ( AliLog::kInfo ); }
  else
  if ( debug >= 4 ) { AliLog::SetGlobalLogLevel ( AliLog::kDebug ); }

  gErrorIgnoreLevel = kErrorIgnoreLevel;
  if ( gErrorIgnoreLevel > 3000 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal ); }

  mgr->SetDebugLevel(mgr_debug);
  TFile* fM = TFile::Open ( "train.root", "RECREATE" );
  fM->cd(); mgr->Write(); fM->Close(); delete fM;
  // task profiling
  if ( kUseSysInfo > 0 )
      {
      for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask (i); }
      }

  if ( kAnalysisMode.EqualTo("local") )
    { mgr->StartAnalysis ( kAnalysisMode.Data(), nentries, firstentry ); }
  else
    { plugin->StartAnalysis ( nentries, firstentry ); }

  // END of mgr->InitAnalysis()

  cout << "EmcalJetCDF END" << endl;
  return mgr;
  }
//>>>>>>  END of void EmcalJetCDF (.....)   <<<<<<<<<
//##########################################################################################################################

//______________________________________________________________________________
void AddIncludePaths ()
    {
    TString includes_str = "-Wno-deprecated -I$. -I$CGAL_DIR/include -I$FASTJET/include -I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include";
    gSystem->AddIncludePath(includes_str.Data());
    }

//______________________________________________________________________________
void AddIncludePathsPlugin ()
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if ( !mgr ) { ::Error ( "EmcalJetCDF.C", "IncludePaths :: No analysis manager to connect to." ); }

    AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
    if ( !plugin ) { ::Error ( "EmcalJetCDF.C", "IncludePaths :: plugin invalid" ); }

    plugin->AddIncludePath(gSystem->GetIncludePath());
    }

//______________________________________________________________________________
void LoadLibs ()
  {
  TString list_fj         = "CGAL fastjet siscone siscone_spherical fastjetplugins fastjettools fastjetcontribfragile";
  TString list_alicejets  = "PWGJE PWGJEEMCALJetTasks";

  LoadLibList (list_fj);

//   gSystem->Load("libCore");
//   gSystem->Load("libPhysics");
//   gSystem->Load("libMinuit");
//   gSystem->Load("libVMC");
//   gSystem->Load("libNet");
//   gSystem->Load("libTree");
//
//   gSystem->Load ("libSTEERBase.so");
//   gSystem->Load ("libAOD");
//   gSystem->Load ("libANALYSIS.so");
//   gSystem->Load ("libANALYSISalice.so");

  LoadLibList (list_alicejets);

  ::Info ( "EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS" );
  gSystem->ListLibraries();

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

//________________________________________________________________________
void SetJFAccFid ( const char* taskname, TString cut = "TPC" )
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF", "No analysis manager to connect to."); }

    AliEmcalJetTask* jf = dynamic_cast<AliEmcalJetTask*>(mgr->GetTask(taskname));
    if (!jf) { AliError("SetJFAccFid() :: task is not EmcalJetTask");}

    cut.ToLower();
    Float radius = jf->GetRadius();

    Float_t fJetMinEta = -0.9, fJetMaxEta = 0.9 ;
    Float_t fJetMinPhi = -10., fJetMaxPhi = 10. ;

    if ( cut.EqualTo("emcal"))
        {
        fJetMinEta = -0.7   ; fJetMaxEta =  0.7 ;
        fJetMinPhi =  1.405 ; fJetMaxPhi =  3.135 ;

        fJetMinPhi += radius;
        fJetMaxPhi -= radius;
        }

    jf->SetPhiRange( fJetMinPhi, fJetMaxPhi );
    jf->SetEtaRange( fJetMinEta + radius, fJetMaxEta - radius );
    }


//________________________________________________________________________
void SetCDFAccFid( const char* taskname, Int_t i = 0 , TString cut = "TPC" )
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF", "No analysis manager to connect to."); }

    AliAnalysisTaskEmcalJetCDF* cdftask = dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(mgr->GetTask(taskname));
    if (!cdftask) { AliError("SetCDFAccFid() :: The task is not TaskEmcalJetCDF");}

    AliJetContainer* jetCont = cdftask->GetJetContainer(i);

    cut.ToLower();
    Float_t radius = jetCont->GetJetRadius();

    Float_t fJetMinEta = -0.9, fJetMaxEta = 0.9 ;
    Float_t fJetMinPhi = -10., fJetMaxPhi = 10. ;

    if ( cut.EqualTo("emcal"))
        {
        fJetMinEta = -0.7   ; fJetMaxEta =  0.7 ;
        fJetMinPhi =  1.405 ; fJetMaxPhi =  3.135 ;

        fJetMinPhi += radius;
        fJetMaxPhi -= radius;
        }

    jetCont->SetJetPhiLimits( fJetMinPhi, fJetMaxPhi );
    jetCont->SetJetEtaLimits( fJetMinEta + radius, fJetMaxEta - radius );
    }

//______________________________________________________________________________
void PrintInfoJF ( const char* taskname )
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF", "No analysis manager to connect to."); }

    AliEmcalJetTask* jf = dynamic_cast<AliEmcalJetTask*>(mgr->GetTask(taskname));
    if (!jf) { AliError("PrintInfoJF() :: task is not EmcalJetTask");}

    AliJetContainer::EJetType_t       jettype_t = jf->GetJetType();
    AliJetContainer::ERecoScheme_t     recomb_t = jf->GetRecombScheme();
    AliJetContainer::EJetAlgo_t       jetalgo_t = jf->GetJetAlgo();
    Double_t r = jf->GetRadius();

    cout << "Jet Finder Task Name : " << jf->GetName() << endl;
    cout << "Jet type : " << jettype_t << endl;
    cout << "Jet recomb scheme : " << recomb_t << endl;
    cout << "Jet algo : " << jetalgo_t << endl;
    cout << "Jet Radius : " << r << endl;
    cout << "JET Eta MIN : "   << jf->GetJetEtaMin() << " ; JET Eta MAX : "   << jf->GetJetEtaMax() << endl;
    cout << "JET Phi MIN : "   << jf->GetJetPhiMin() << " ; JET Phi MAX : "   << jf->GetJetPhiMax() << endl;

    cout << "\n"<< endl;
    }

//______________________________________________________________________________
void PrintInfoCDFtask ( const char* taskname, Int_t i )
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF", "No analysis manager to connect to."); }

    AliAnalysisTaskEmcalJetCDF* cdftask = dynamic_cast<AliAnalysisTaskEmcalJetCDF*>(mgr->GetTask(taskname));
    cout << "\nJet Finder Task Name : " << cdftask->GetName() << endl;

    AliJetContainer* jetcont = cdftask->GetJetContainer(i);
    jetcont->PrintCuts();
    }

//______________________________________________________________________________
std::vector<TString> GetJetFinderList()
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF", "No analysis manager to connect to."); }

    TObjArray* tasks_list = mgr->GetTasks(); // get list of of tasks
    std::vector<TString> jf_list; // vector of strings (names of jet finder tasks)

    for ( Int_t t_idx = 0; t_idx < tasks_list->GetEntries(); ++t_idx )
        {
        AliAnalysisTaskSE* task = dynamic_cast<AliAnalysisTaskSE*> ( tasks_list->At ( t_idx ) );
        if ( !task ) { continue; }
        TString task_name = task->GetName();
        if ( task->IsA()->InheritsFrom(AliEmcalJetTask::Class()) ) { jf_list.push_back(task_name); }
        }

    return jf_list;
    }

//______________________________________________________________________________
void PrepareAnalysisEnvironment()
    {
    // get analysis manager
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) { ::Error("EmcalJetCDF::PrepareAnalysisEnvironment()", "No analysis manager to connect to."); }

    // get alien plugin
    AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
    if ( !plugin ) { ::Error ( "EmcalJetCDF::PrepareAnalysisEnvironment()", "no alien plugin" ); return kFALSE; }

    if ( kCommonOutputFileName.Length() > 0 ) { mgr->SetCommonFileName ( kCommonOutputFileName.Data() ); }

    mgr->SetSkipTerminate ( kSkipTerminate );

//__________________________________________________________________________________
    //////////////////////////////////////////
    ////   LIBRARIES TO BE LOADED BY PLUGIN
    /////////////////////////////////////////
    ListLibs += kGridExtraFiles;

    ListLibs       = ListLibs.Strip();      if ( debug > 3 ) {  Printf ( "### ListLibs : %s", ListLibs.Data() ); }
    ListLibsExtra  = ListLibsExtra.Strip(); if ( debug > 3 ) {  Printf ( "### ListLibsExtra : %s", ListLibsExtra.Data() ); }
    ListSources    = ListSources.Strip();   if ( debug > 3 ) {  Printf ( "### ListSources : %s", ListSources.Data() ); }

    if ( ListLibs.Length() )       { plugin->SetAdditionalLibs     ( ListLibs.Data() ); }
    if ( ListLibsExtra.Length() )  { plugin->SetAdditionalRootLibs ( ListLibsExtra.Data() ); }
    if ( ListSources.Length() )    { plugin->SetAnalysisSource     ( ListSources.Data() ); }

//______________________________________________________________________________
    //******************
    //    DATA INPUT
    //******************
    // Prepare input decisions
    kInputDataLabel = "";

    std::ifstream ifs (kInputStr.Data()); // try to open as file

    if ( ifs.is_open() ) // if input is file
        {
        kDatafile = kInputStr;

        // if input data is file then analysis mode must be local or proof (if forgotten to grid, will revert to local)
        if ( kAnalysisMode.EqualTo("grid") ) { kAnalysisMode = "local" ;}

        // .txt file containing the list of files to be chained in test mode; even if analysis is PROOF (plugin is test)
        plugin->SetFileForTestMode ( kDatafile.Data() );   //fFileForTestMode
        }
    else if ( kInputStr.IsAscii() ) // either grid (tags from InputData.C) either dataset
        {
        if ( kAnalysisMode.EqualTo("grid"))
            {
            kInputDataLabel = kInputStr ; // input field must corespond with defined tags in InputData.C macro
            gROOT->LoadMacro ( "InputData.C" );    // Load InputData macro
            if ( !InputData(kInputDataLabel) ) { cout << "Analysis mode is GRID but no InputData.C label was recognized!! exiting..." << endl; gApplication->Terminate(); }
            }
        else
            {
            kDataset = kInputStr; // all other conditions means kInputStr is a proof dataset name
            if ( !kDataset.BeginsWith("/") ) { cout << "Dataset is not beggining with \"/\". Check the arguments passed to script. exiting..." << endl; gApplication->Terminate();  }
            }
        }
    //++++++++++++++++++++++++++++++++++


    //   *****************************
    //         DATA TYPE SETTINGS
    //   *****************************
    // extract (if possible) the data type from input path
    TString dataPattern   = plugin->GetDataPattern();
    TString file4TestMode = plugin->GetFileForTestMode();

    if ( kPluginMode.EqualTo("test") && ( kAnalysisMode.EqualTo("local") || kAnalysisMode.EqualTo("proof") ) )
        { dataType = FindTreeName(file4TestMode); }
    else
        {
        if (dataPattern.Contains("AliAOD.root")) { dataType = "aod"; }
        if (dataPattern.Contains("AliESD.root")) { dataType = "esd"; }
        }

    if (!dataType.IsNull())
        { cout << "----->>>>>>> Data type : " << dataType.Data() << endl; }
    else
        {
        dataType = "aod";
        cout << "ERROR ----->>>>>>>   Data type IS NULL !!! ; dataType fallback to AOD type" << endl;
        Printf ("ERROR ############   Set dataType by hand in %s at line %d\n",(char*)__FILE__,__LINE__+2);
        }
    // dataType = "esd"; // Here it should be set by hand in case automatic procedure did not work

    //   *****************************
    //         PERIOD SETTINGS
    //   *****************************
    // extract (if possible) the Period from input path
    if ( kPluginMode.EqualTo("test") && ( kAnalysisMode.EqualTo("local") || kAnalysisMode.EqualTo("proof") ) )
        {
        TString data_path = GetInputDataPath (file4TestMode);
        runPeriod = GetPeriod (data_path);
        }
    else
        {
        TString griddatadir = plugin->GetGridDataDir();
        runPeriod = GetPeriod ( griddatadir );
        }

    if (!runPeriod.IsNull())
        { cout << "----->>>>>>> Period : " << runPeriod.Data() << endl; }
    else
        {
        cout << "ERROR ----->>>>>>>   PERIOD IS NULL !!!" << endl;
        Printf ("ERROR ############   Set runPeriod by hand in %s at line %d\n",(char*)__FILE__,__LINE__+2);
        }
    // runPeriod = "lhc10e"; // Here it should be set by hand in case automatic procedure did not work

    cout << "----->>>>>>> Period is MonteCarlo? : " << PeriodIsMC (runPeriod) << endl;

    //   *****************************
    //         PASS SETTINGS
    //   *****************************
    // extract (if possible) the Pass from input path
    if ( kPluginMode.EqualTo("test") && ( kAnalysisMode.EqualTo("local") || kAnalysisMode.EqualTo("proof") ) )
        {
        TString data_path = GetInputDataPath (file4TestMode);
        kPass = GetPass (data_path);
        }
    else
        { kPass = GetPass (dataPattern); }

    cout << "----->>>>>>> Pass is : " << kPass.Data() << endl;

    //<><><><><><><><><><>
    //    DEBUGGING
    //<><><><><><><><><><>
    mgr->SetDebugLevel(debug);

    // Event frequency for collecting system information
    mgr->SetNSysInfo ( kUseSysInfo );

    if ( kUseSysInfo > 0 )
        {
        mgr->RegisterExtraFile ( "syswatch.root" );

        if ( kGridMergeExclude.Length() ) { kGridMergeExclude += " "; }
        kGridMergeExclude += "syswatch.root";
        }

    //********************
    //    DATA OUTPUT
    //********************
    plugin->SetMergeExcludes  ( kGridMergeExclude );
    plugin->SetDefaultOutputs ( kTRUE );


//______________________________________________________________________________
// handlers definition
    if ( dataType.EqualTo("aod") )
        {
        AliAODInputHandler* aodH = AddAODHandler();
        aodH->SetCheckStatistics ( kTRUE );
        }
    else
    if ( dataType.EqualTo("esd") || dataType.EqualTo("sesd") )
        {
//         AliESDInputHandler* esdH = AddESDHandler();
        Printf("For the moment, this macro is only available for AOD analysis!");
        gApplication->Terminate();
       }
    else
        { cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n"; gApplication->Terminate(); }

    // Create MC handler, if MC is demanded
    if ( isMC && !dataType.EqualTo("aod") )
        {
        AliMCEventHandler* mcH = AddMCHandler (kTRUE);
        }

    AliInputEventHandler* inputh = mgr->GetInputEventHandler();
    inputh->SetNeedField(kTRUE);

    }

//______________________________________________________________________________
AliAnalysisAlien* CreateAlienHandler ( const char* plugin_mode )
    {
    AliAnalysisAlien* plugin = new AliAnalysisAlien();

    if ( !plugin ) { cout << "!!! -->> alien handler could not be created <<-- !!!" << endl; return;}

    //********************   PLUGIN OPTIONS   ************************
    // exit from aliensh after submmiting job
    plugin->SetDropToShell ( kTRUE );

    // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
    plugin->SetRunMode ( plugin_mode );

    plugin->SetCheckCopy ( kFALSE );

    // min (nr,4) replicas in grid storage
    plugin->SetNumberOfReplicas(2);

    // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
    plugin->SetNtestFiles ( kTestFiles );

    if ( kPluginUseProductionMode ) { plugin->SetProductionMode(); }

    plugin->SetExecutableCommand ( kPluginExecutableCommand.Data() );
    plugin->SetJobTag ( kJobTag );

    // Set versions of used packages
    plugin->SetAPIVersion        ( kAPIVersion.Data() );
    plugin->SetROOTVersion       ( kRootVersion.Data() );
    plugin->SetAliROOTVersion    ( kAliRootVersion.Data() );
    plugin->SetAliPhysicsVersion ( kAliPhysicsVersion.Data() );

    // Add external packages
    plugin->AddExternalPackage ( kPackage1.Data() );
    plugin->AddExternalPackage ( kPackage2.Data() );
    plugin->AddExternalPackage ( kPackage3.Data() );
    plugin->AddExternalPackage ( kPackage4.Data() );

    // Use fastread option
    plugin->SetFastReadOption ( kPluginFastReadOption );

    // Overwrite existing files if any
    plugin->SetOverwriteMode ( kPluginOverwriteMode );

    // Optionally set a name for the generated analysis macro (default MyAnalysis.C)
    plugin->SetAnalysisMacro ( Form ( "%s.C", kTrainName.Data() ) );

    // Optionally modify the name of the generated JDL (default analysis.jdl)
    plugin->SetJDLName ( Form ( "%s.jdl", kTrainName.Data() ) );

    // Optionally modify the executable name (default analysis.sh)
    plugin->SetExecutable ( Form ( "%s.sh", kTrainName.Data() ) );

    // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
    plugin->SetSplitMaxInputFileNumber ( kGridFilesPerJob );

    // Optionally set maximum number of files merged in a chunk (default 100, put 0 to ignore)
    plugin->SetMaxMergeFiles ( kGridMaxMergeFiles );

    // Maximum initial consecutive subjobs accepted to fail.
    plugin->SetMaxInitFailed ( kMaxInitFailed );

    // Optionally resubmit threshold.
    // plugin->SetMasterResubmitThreshold(90);

    // Optionally set time to live
    plugin->SetTTL ( kTTL );

    // Optionally set input format (default xml-single)
    plugin->SetInputFormat("xml-single");

    // Optionally modify job price (default 1)
    plugin->SetPrice(1);

    // Number of runs per masterjob
    plugin->SetNrunsPerMaster(1);

    // We split per SE or file
    plugin->SetSplitMode("se");

    // Enable merging via automatic JDL
    plugin->SetMergeViaJDL(kTRUE);

    // Maximum number of files to be merged in one chunk
    plugin->SetOneStageMerging(kFALSE);

    // Maximum number of merging stages
    plugin->SetMaxMergeStages(3);

    // write the output to subdirs named after run number
    plugin->SetOutputToRunNo(1);

    // AliEn directory containing the input packages
    if ( kGridDataSet.Length() > 0 ) { kWorkDir += Form ( "/%s%s", kGridDataSet.Data(), kGridExtraAliendirLevel.Data() ); }
    plugin->SetGridWorkingDir ( kWorkDir.Data() );

    // Declare alien output directory. Relative to working directory.
    if ( !kGridOutdir.Length() ) { kGridOutdir = Form ( "out_%s", kTrainName.Data() ); }
    plugin->SetGridOutputDir ( kGridOutdir );


    //-----------------------------------------------
    //           PROOF SETTINGS
    //-----------------------------------------------
    plugin->SetProofCluster  ( kAAF )        ;
    plugin->SetProofDataSet  ( kDataset )    ;
    plugin->SetProofReset    ( kProofReset ) ;
    plugin->SetNproofWorkers ( kWorkers )    ;
    // plugin->SetNproofWorkersPerSlave( kCores)  ;
    // plugin->SetRootVersionForProof ( kPluginRootVersion ) ;
    plugin->SetAliRootMode ( kAlirootMode ) ;

    // plugin->SetProofParameter(const char *pname, const char *value);

    return plugin;
    }


//______________________________________________________________________________
Bool_t LoadPars ( const char* module )
    {
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if ( !mgr ) { ::Error ( "LoadPars", "No analysis manager to connect to." ); return kFALSE; }

    AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
    if ( !plugin ) { ::Error ( "LoadPars", "InputData :: plugin invalid" ); return kFALSE; }

    // Load a module library in a given mode. Reports success.
    Int_t result = -999; Bool_t success = kTRUE;

    TString mod ( module );

    if ( !mod.Length() ) { ::Error ( "EmcalJetCDF::LoadPars", "Empty module name" ); return kFALSE; }

    if ( !plugin->SetupPar ( module ) ) { return kFALSE; }

    mod += ".par"; plugin->EnablePackage ( mod.Data() ); // anaPars += mod.Data();

    mod.ReplaceAll(".par",".so"); mod.Prepend("lib");
    result = gSystem->Load(mod);

    if ( result < 0 ) { cout << "Load PARs : Load library failed : " << mod.Data() << endl; return kFALSE ; }

    return success;
    }

//______________________________________________________________________________
Bool_t LoadSource ( const char* source, Bool_t rec = kFALSE  )
    {
    // Load a module library in a given mode. Reports success.
    Int_t result = -1;
    TString ssource ( source );
    TString basename = gSystem->BaseName ( ssource.Data() );

    if ( !ssource.Length() ) { ::Error ( "EmcalJetCDF::LoadSource", "Empty task name" ); return kFALSE; }

    // we have a source code so compile it
    if ( ssource.EndsWith ( ".cxx" ) )
        {
        // need to copy it here otherwise the path is also used on grid...
        ssource.Remove ( ssource.Index ( ".cxx" ) );
        basename.Remove ( basename.Index ( ".cxx" ) );
        Printf ( "LoadSources:: Copying...  path %s{cxx,h}", ssource.Data() );
        gSystem->Exec ( Form ( "cp %s.cxx . ", ssource.Data() ) );
        gSystem->Exec ( Form ( "cp %s.h . ", ssource.Data() ) );

        // Path to code only needed for local compilation, in grid and proof mode the task headers are uploaded
        //     path.Remove(path.Index(gSystem->BaseName(path.Data())));
        // Printf("LoadSources:: Including path %s",path.Data());
        //  if(path.Length()>0)gROOT->ProcessLine(Form(".include %s",path.Data()));
        Printf ( "LoadSources:: Loading...  path %s", basename.Data() );

        result = gROOT->LoadMacro ( Form ( "%s.cxx++g", basename.Data() ) );

        if ( rec )
            {
            // what we want to compile
            ListSources += Form ( "%s.cxx ", basename.Data() );
            // what we need as input...
            ListLibs += Form ( "%s.cxx %s.h ", basename.Data(), basename.Data() );
            }
        }


    if ( result < 0 ) { ::Error ( "EmcalJetsCDF::LoadSources", "Could not load source %s", source ); return kFALSE; }

    return kTRUE;
    }


//______________________________________________________________________________
TString FindTreeName ( const TString& file_list ) const
    {
    TString fDataType = "";  // result
    if ( gSystem->AccessPathName ( file_list.Data() ) )
            { cout << "File not found: " << file_list.Data() << endl; return fDataType; }

    // Open the file
    ifstream in; in.open ( file_list.Data() );
    Int_t count = 0;
    TString line;

    while ( in.good() )
        {
        in >> line;
        if ( line.IsNull() || line.BeginsWith ( "#" ) ) continue;
        if ( count++ == 1 ) break; // look only over first file;

        TString file ( line );

        // STRICT ORDERING!!! If multiple tree types found last valid will be preffered
        if (IsTreeType(file,"TreeK"))      { fDataType = "kine";    }
        if (IsTreeType(file,"TE"))         { fDataType = "galice";  }
        if (IsTreeType(file,"HLTESDTree")) { fDataType = "hltesd";  }
        if (IsTreeType(file,"esdTree"))    { fDataType = "esd";     }
        if (IsTreeType(file,"aodTree"))    { fDataType = "aod";     }

        return fDataType;
        }
    in.close();
    return fDataType ;
    }

//______________________________________________________________________________
Bool_t IsTreeType ( const TString& fileInput, const TString& treeName)
    {
    TFile* f = TFile::Open (fileInput.Data());

    if ( f->IsZombie() )
        { cout << " :: Skipping un-openable file: << " << fileInput.Data()  << endl; return kFALSE; }
    else
        {
        TKey* key = f->FindKeyAny (treeName.Data());
        if (key) { f->Close(); return kTRUE;  }
        else     { f->Close(); return kFALSE; }
        }
    }

//______________________________________________________________________________
TString GetInputDataPath ( const TString& file_list)
    {
    std::string line_str;
    TString line = line_str.c_str();

    if ( gSystem->AccessPathName ( file_list.Data() ) )
        { cout << "File not found: " << file_list.Data() << endl; return line; }

    // Open the file
    ifstream in; in.open ( file_list.Data() );
    Int_t count = 0;

    while ( in.good() )
        {
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
TString GetPeriod ( const TString& file_path)
    {
    TString period = "";

    if (!file_path.IsNull())
        {
        // split string in tokens (libs)
        TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
        TIter next_str(tokens_list);
        TObjString* token = NULL;
        Int_t j=0;
        while ((token=(TObjString*)next_str()))
            {
            TString token_str = token->GetString(); token_str.ToLower();
            if ( token_str.Contains("lhc") ) { period = token_str; }
            }
        delete tokens_list;
        }

    return period;
    }

//______________________________________________________________________________
TString GetPass ( const TString& file_path)
    {
    TString pass = "";

    if (!file_path.IsNull())
        {
        // split string in tokens (libs)
        TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
        TIter next_str(tokens_list);
        TObjString* token=0;
        Int_t j=0;
        while ((token=(TObjString*)next_str()))
            {
            TString token_str = token->GetString(); token_str.ToLower();
            if ( token_str.Contains("pass") ) { pass = token_str; }
            }
        delete tokens_list;
        }

    return pass;
    }

//______________________________________________________________________________
Bool_t PeriodIsMC ( const TString& period )
    {
    if (!period.IsNull())
        {
        period.ToLower();
        if ( period.BeginsWith("lhc") && (period.Length() > 6) ) {return kTRUE;}
        }
    return kFALSE;
    }

//______________________________________________________________________________
void LoadMacros ()
  {
  //__________________________________________________________________________________
  // load AddTask macros
  gROOT->LoadMacro ( "InputData.C" );    // Load InputData macro
  if (localMacros)
    {
    gROOT->LoadMacro ( "AddTaskEmcalJetCDF.C" );
    gROOT->LoadMacro ( "AddTaskEmcalJetCDFUE.C" );
    }
  else
    {
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDF.C" );
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDFUE.C" );
    }

  gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C" );
  gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C" );
  gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddMCHandler.C" );

  gROOT->LoadMacro ( "$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");

  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalSetup.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEMCALTender.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskClusterizerFast.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusterMaker.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalClusTrackMatcher.C");
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskHadCorr.C");
//   gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C");

  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C" );
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetSample.C" );
  gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRhoNew.C" );
  }

TString CreateCDFTaskName ( TString prefix = "CDF", AliJetContainer::EJetType_t jtype, AliJetContainer::JetAcceptanceType acc, AliJetContainer::EJetAlgo_t jalgo, AliJetContainer::ERecoScheme_t reco,
                                Double_t jetptcutmin, Double_t jetptcutmax, Double_t r )
{
  TString jet ("NA");
  if ( jtype == AliJetContainer::kFullJet )    { jet = "fullJet" ;}
  if ( jtype == AliJetContainer::kChargedJet ) { jet = "chgJet" ;}
  if ( jtype == AliJetContainer::kNeutralJet ) { jet = "neutralJet" ;}

  TString acctype ("acc");   acctype += TString::Itoa(acc,10);
  TString algotype ("algo"); algotype += TString::Itoa(jalgo,10);
  TString recotype ("reco"); recotype += TString::Itoa(reco,10);

  if ( jetptcutmin < 1. ) { jetptcutmin = 1.; cout << "CreateCDFTaskName :: jetptcutmin < 1." << endl; }
  TString jetstrmin ("jptmin");
  jetstrmin += ( ULong_t ) ( jetptcutmin * 1000 );

  if ( jetptcutmax <= jetptcutmin ) { cout << "CreateCDFTaskName :: jetptcutmax <= jetptcutmin" << endl; }
  TString jetstrmax ("jptmax");
  jetstrmax += ( ULong_t ) ( jetptcutmax * 1000 );

  TString rstr ("R0"); r *= 10; rstr += TString::Itoa(r,10);

  TString cdftaskname = prefix + "_" + jet + "_" + acctype + "_" + algotype + "_" + recotype + "_" + jetstrmin + "_" + jetstrmax + "_" + rstr;
  return cdftaskname;
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
