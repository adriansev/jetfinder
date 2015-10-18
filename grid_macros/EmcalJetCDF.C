
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
#include "TObject.h"
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

// AliROOT includes
#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"
#include "AliAnalysisDataContainer.h"
#include "AliMultiInputEventHandler.h"
#include "AliLog.h"
#include "AliVEventHandler.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliESDInputHandler.h"
#include "AliInputEventHandler.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskTagCreator.h"
#include "AliPhysicsSelectionTask.h"
#include "AliCentralitySelectionTask.h"
#include "AliEmcalCompatTask.h"
#include "AliEmcalSetupTask.h"
#include "AliAnalysisTaskEmcal.h"
#include "AliEMCALRecParam.h"
#include "AliEmcalJetTask.h"
#include "AliAnalysisTaskRho.h"

#include "AliAnalysisDataContainer.h"

#include "AliAnalysisTaskEmcal.h"
#include "AliAnalysisTaskEmcalJet.h"
#include "AliAnalysisTaskEmcalJetSample.h"

#include "AddAODHandler.C"
#include "AddESDHandler.C"
#include "AddMCHandler.C"

#include "AddTaskEmcalPhysicsSelection.C"
#include "AddTaskEmcalCompat.C"
#include "AddTaskEmcalSetup.C"
#include "AddTaskEMCALTender.C"
#include "AddTaskJetPreparation.C"
#include "AddTaskEmcalJet.C"
#include "AddTaskEmcalPreparation.C"
#include "AddTaskCentrality.C"
#include "AddTaskRho.C"
#include "AddTaskEmcalJetSample.C"

#include "AliAnalysisTaskEmcalJetCDF.h"
#include "AliAnalysisTaskEmcalJetCDFUE.h"
#include "AddTaskEmcalJetCDF.C"
#include "AddTaskEmcalJetCDFUE.C"
#include "InputData.C"

#endif


//______________________________________________________________________________
// enum Jets used
enum AlgoType {kKT, kANTIKT}; // AddTaskEmcalJet.C
enum JetType  {kFULLJETS, kCHARGEDJETS, kNEUTRALJETS}; // AddTaskEmcalJet.C
enum JetAcceptanceType { kTPC, kEMCAL, kUser }; // AliJetContainer
enum runModeType {  kLocal, kGrid };  // AliAnalysisTaskLocalRho
enum fitModulationType { kNoFit, kV2, kV3, kCombined, kFourierSeries, kIntegratedFlow, kQC2, kQC4 }; // AliAnalysisTaskLocalRho

//______________________________________________________________________________
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//        AliEN plugin variables
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//______________________________________________________________________________

Int_t       kTestFiles               = 32;    // Number of test files
Long64_t    nentries                 = 1234567890; // for local and proof mode, ignored in grid mode. Set to 1234567890 for all events.
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

TString     kWorkDir                 = "emcalcdf";    // AliEn work dir; relative to AliEn $HOME
TString     kTrainName               = "sevjets";     // *CHANGE ME* (no blancs or special characters)
TString     kJobTag                  = "sevcdfjet";   // *CHANGE ME*

TString     kPluginExecutableCommand = "aliroot -b -q";
Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

TString     kAPIVersion              = "V1.1x";
TString     kRootVersion             = "v5-34-08-7";
TString     kAliRootVersion          = "v5-06-14";
TString     kAliPhysicsVersion       = "vAN-20150502";

TString     kPackage1                = "boost::v1_53_0";
TString     kPackage2                = "cgal::v4.4";
TString     kPackage3                = "fastjet::v3.0.6_1.012";
TString     kPackage4                = "GEANT3::v1-15a-1";
TString     kPackage5                = "jemalloc::v3.6.0";

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
    "asevcenc@skaf.saske.sk";

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
TString     kCommonOutputFileName    = "CDFanalysis.root";

// == grid plugin files rules
TString     kGridExtraFiles          = ""; // LIBRARIES files that will be added to the input list in the JDL
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

// == Lists of libraries, pars and sources to be added to the alien plugin
TString     ListPars      = "";
TString     ListLibs      = "";
TString     ListLibsExtra = "";
TString     ListSources   = "";

//==============================================================================
//      DEBUG
Int_t           debug              =  0 ; // kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
UInt_t          mgr_debug          =  0 ; // AliAnalysisManager debug level
unsigned int    kUseSysInfo        =  0 ; // activate debugging
Int_t           kErrorIgnoreLevel  = -1 ; // takes the errror print level from .rootrc

//______________________________________________________________________________
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@     ANALYSIS STEERING VARIABLES     @@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Bool_t         isMC                = kFALSE;          // trigger, if MC handler should be used
Bool_t         useTender           = kTRUE;           // trigger, if tender task should be used
Bool_t         doBkg               = kFALSE;          // background estimation with AliAnalysisTaskRho

UInt_t         pSel                = AliVEvent::kMB;  // used event selection for every task except for the analysis tasks

Int_t          jettype             = 1;             // 0 --> AliEmcalJetTask::kFullJet; 1 --> AliEmcalJetTask::kChargedJet; 2 --> AliEmcalJetTask::kNeutralJet
TString        acceptance_type     = "EMCAL";       // TPC, EMCAL, USER
UInt_t         acceptance_type_i   = -1;            // AliJetContainer enum ... will be set in sync to string value below

Bool_t         tracks_etaphi_cuts  = kFALSE;        // fiducial acceptance cuts on jet constituents (tracks)

Int_t          leadhadtype         = 0;             // AliJetContainer :: Int_t fLeadingHadronType;  0 = charged, 1 = neutral, 2 = both

//__________________________________________________________________________________
// Objects (branch names) used in Jet framework
TString tracksName         = "PicoTracks";          // runEmcalJetAnalysis default = PicoTracks
TString clustersName       = "EmcCaloClusters";     // runEmcalJetAnalysis default = EmcCaloClusters
TString clustersCorrName   = "CaloClustersCorr";    // runEmcalJetAnalysis default = CaloClustersCorr
TString rhoName            = "";

int EmcalJetCDF (const char* analysis_mode = "local", const char* plugin_mode = "test", const char* input = "data.txt")
  {
  gSystem->SetFPEMask(); // because is used in reference script
//  acceptance_type.ToUpper();

  // set function arguments
  kPluginMode   = plugin_mode   ; kPluginMode.ToLower();    // test, offline, submit, terminate, full
  kAnalysisMode = analysis_mode ; kAnalysisMode.ToLower();  // local, grid, proof
  kInputStr     = input;

  // if analysis is done on localhost do not use PARs
  if ( kAnalysisMode.EqualTo ("local") || kPluginMode.EqualTo ("test")  ) { kUsePAR = kFALSE; }

  // sanity checks
  if ( jettype != 1 )  { acceptance_type = "EMCAL"; }
  if ( jettype == 2 )  { leadhadtype == 1; }
  if ( jettype == 0 )  { leadhadtype == 2; }

  if ( acceptance_type.EqualTo("TPC"))   { acceptance_type_i = 0; }
  else
  if ( acceptance_type.EqualTo("EMCAL")) { acceptance_type_i = 1; }
  else
      { acceptance_type_i = 2; }

  // if acceptance cut on input contituents then do not cut in acceptance of jets
  if ( tracks_etaphi_cuts ) { acceptance_type = "TPC" ; acceptance_type_i = 0; }

//__________________________________________________________________________________
  //*******************************************
  //   Loading of libraries (script + plugin)
  //*******************************************
  AliAnalysisAlien* plugin = CreateAlienHandler ( kPluginMode.Data() );             // ###   SET UP AliEn handler ###
  AliAnalysisManager* mgr  = plugin->CreateAnalysisManager ( "CDFhistos_mgr" );     // ###   ANALYSIS MANAGER     ###

  AddIncludePaths(); // Add include paths for local task and plugin
  LoadLibs(); // Load necessary libraries for the script and for the plugin

//__________________________________________________________________________________
    // NOTE!!! after the custom task part to be picked up and loaded by alien plugin
    PrepareAnalysisEnvironment();

//####################################################
//#######        ANALYSIS TASKS LIST
//####################################################

//______________________________________________________________________________
// Signature PhysicsSelectionTask
    Bool_t     isLHC11a          = runPeriod.EqualTo("lhc11a");  // true then skip FastOnly events (only for LHC11a)
    Bool_t     withHistos        = kFALSE;  // true then write output
    UInt_t     triggers          = pSel;    // if not zero only process given trigges
    Double_t   minE              = 1;       // minimum clus energy (<0 -> do not compute)
    Double_t   minPt             = 1;       // minimum track pt (<0 -> do not compute)
    Double_t   vz                = 10;      // primary vertex z cut (-1 none)
    Bool_t     vzdiff            = kTRUE;   // true then select on PRI minus SPD z-vertex
    Double_t   cmin              = -1;      // minimum centrality required (V0M)
    Double_t   cmax              = -1;      // maximum centrality required (V0M)
    Double_t   minCellTrackScale = -1;      // minimum cells over tracks scale
    Double_t   maxCellTrackScale = -1;      // maximum cells over tracks scale

    // Physics selection task
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C" );
    AliPhysicsSelectionTask* physSelTask = AddTaskEmcalPhysicsSelection ( isLHC11a, withHistos, triggers, minE, minPt, vz, vzdiff, cmin, cmax, minCellTrackScale, maxCellTrackScale );
    if ( !physSelTask ) { cout << "----------  no physSelTask"; return; }

//______________________________________________________________________________
// Centrality task
    if ( dataType.EqualTo("esd") )
        {
        gROOT->LoadMacro ( "$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C" );
        AliCentralitySelectionTask* centralityTask = AddTaskCentrality ( kTRUE );
        }
//______________________________________________________________________________
// Compatibility task, only needed for skimmed ESD
    if ( dataType.EqualTo("sesd") )
        {
        gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalCompat.C" );
        AliEmcalCompatTask* comptask = AddTaskEmcalCompat();
        }
//______________________________________________________________________________
// Setup task - seems to be required ALWAYS!!
//    if ( acceptance_type.EqualTo("EMCAL") )
//     if ( kTRUE ) {     }
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalSetup.C" );
    const char* geop    = "$ALICE_PHYSICS/OADB/EMCAL"; // default = 0; path to geometry folder
    const char* oadp    = ""; // default = 0; path to OADB folder
    const char* ocdp    = ""; // default = 0; path to OCDB (if "uselocal", a copy placed in ALIROOT will be used
    const char* objs    = ""; // default = 0; objects for which alignment should be applied
    const Bool_t noOCDB = kFALSE; // default = false; if true then do not mess with OCDB

    AliEmcalSetupTask* emcalsetupTask = AddTaskEmcalSetup(geop, oadp, ocdp, objs, noOCDB);

//______________________________________________________________________________
// Tender Supplies
    if (useTender)
        {
        gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalPreparation.C");
        // adjust pass when running locally. On grid give empty string, will be picked up automatically from path to ESD/AOD file
        // const char *perstr  = "LHC11h"
        AliAnalysisTaskSE* clusm = AddTaskEmcalPreparation(runPeriod.Data());
        }

//__________________________________________________________________________________
// ################# Now: Call jet preparation macro (picotracks, hadronic corrected caloclusters, ...)

//______________________________________________________________________________
// Jet preparation
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C" );
    const char*    periodstr          = runPeriod.Data();          // default: "LHC11h";
    const char*    pTracksName        = tracksName.Data();         // default: "PicoTracks"
    const char*    usedMCParticles    = "" ;                       // default: "MCParticlesSelected"
    if (isMC)    { usedMCParticles    = "MCParticlesSelected"; }
    const char*    usedClusters       = clustersName.Data();       // default: "CaloClusters"
    const char*    outClusName        = clustersCorrName.Data();   // default: "CaloClustersCorr"
    const Double_t hadcorr            = 2.0;                       // default: 2.0
    const Double_t Eexcl              = 0.00;                      // default: 0.00
    const Double_t phiMatch           = 0.03;                      // default: 0.03
    const Double_t etaMatch           = 0.015;                     // default: 0.015
    const Double_t minPtEt            = 0.15;                      // default: 0.15
    const UInt_t   pSel_jetprep       = pSel;                      // default: AliVEvent::kAny
    const Bool_t   trackclus          = kTRUE;                     // default: kTRUE
    const Bool_t   doHistos           = kFALSE;                    // default: kFALSE
    const Bool_t   makePicoTracks     = kTRUE;                     // default: kTRUE
    const Bool_t   makeTrigger        = kTRUE;                     // default: kTRUE
    const Bool_t   isEmcalTrain       = kFALSE;                    // default: kFALSE
    const Double_t trackeff           = 1.0;                       // default: 1.0
    const Bool_t   doAODTrackProp     = kTRUE;                     // default: kTRUE
    const Bool_t   modifyMatchObjs    = kTRUE;                     // default: kTRUE

    AddTaskJetPreparation( periodstr, pTracksName, usedMCParticles, usedClusters, outClusName, hadcorr,
                           Eexcl, phiMatch, etaMatch, minPtEt, pSel_jetprep, trackclus, doHistos,
                           makePicoTracks, makeTrigger, isEmcalTrain, trackeff, doAODTrackProp, modifyMatchObjs );


// ################# Now: Add jet finders+analyzers
//______________________________________________________________________________
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C" );
    const char*    nTracks         = tracksName.Data();       // default: "Tracks";
    const char*    nClusters       = clustersCorrName.Data(); // default: "CaloClusters";
    Int_t          algo            = kANTIKT;    // default: 1 ; 0 --> AliEmcalJetTask::kKT ; != 0 --> AliEmcalJetTask::kAKT
    Double_t       radius          = 0.4;        // default: 0.4
    Int_t          type            = jettype;    // default: 0 --> AliEmcalJetTask::kFullJet; 1 --> AliEmcalJetTask::kChargedJet; 2 --> AliEmcalJetTask::kNeutralJet
    Double_t       minTrPt         = 0.15;       // default: 0.15  // min jet track momentum   (applied before clustering)
    Double_t       minClPt         = 0.30;       // default: 0.30  // min jet cluster momentum (applied before clustering)
    Double_t       ghostArea       = 0.005;      // default: 0.005 // ghost area
    Int_t          recombScheme    = 1;          // default: 1
    const char*    tag             = "Jet";      // default: "Jet"
    Double_t       minJetPt        = 1.;         // default: 0.
    Bool_t         selectPhysPrim  = kFALSE;     // default: kFALSE
    Bool_t         lockTask        = kFALSE;      // default: kTRUE
    Int_t          useExchangeCont = 0;
    Bool_t         bFillGhosts     = kFALSE;

    AliEmcalJetTask* jf = NULL;

    // List off all jetfinders in order to use analysis tasks for each of them
    std::vector<TString> jf_names;

    Double_t radius_list[] = { 0.4 }; // for each radius make a jetfinder

    Size_t rnr = sizeof(radius_list)/sizeof(radius_list[0]);
    for (Size_t j = 0; j < rnr; j++ )
        {
        jf = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius_list[j], jettype, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
        TString jftaskname = jf->GetName();

        PrintInfoJF ( jftaskname.Data() );
        jf_names.push_back( jftaskname );

        if ( tracks_etaphi_cuts ) { SetJFAccFid (jftaskname.Data(), acceptance_type); }
        }

//______________________________________________________________________________
    if (doBkg)
        {
        AliEmcalJetTask* jetFinderTaskKT = AddTaskEmcalJet (tracksName.Data(), clustersCorrName.Data(), kKT, radius, jettype, minTrPt, minClPt);
        TString     kTpcKtJetsName   = jetFinderTaskKT->GetName();
        TString     kTpcKtTracks     = jetFinderTaskKT->GetTracksName();
        TString     kTpcKtClusters   = jetFinderTaskKT->GetClusName();
        TString     kTpcKtJets       = jetFinderTaskKT->GetJetsName();
        Double_t    kTpcKtJetR       = jetFinderTaskKT->GetRadius();

        gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRho.C");
    //     const char*    nJets       = "Jets";          // DEFAULT
    //     const char*    nTracks     = "PicoTracks";    // DEFAULT
    //     const char*    nClusters   = "CaloClusters";  // DEFAULT
    //     const char*    nRho        = "Rho";           // DEFAULT
    //     Double_t       jetradius   = 0.2;             // DEFAULT
    //     const char*    cutType     = "TPC";           // DEFAULT
        Double_t       rhojetareacut  = 0.01;
        Double_t       emcareacut  = 0;
        TF1*           sfunc       = 0;
        const UInt_t   exclJets    = 2;
        const Bool_t   histo       = kFALSE;
        const char*    taskname    = "Rho";

        rhoName = "Rho";
        AliAnalysisTaskRho* rhotask = AddTaskRho ( kTpcKtJetsName.Data(), kTpcKtTracks.Data(), kTpcKtClusters.Data(), rhoName.Data(), kTpcKtJetR, acceptance_type.Data(),
                                                  rhojetareacut, emcareacut, sfunc, exclJets, kTRUE );
        //rhotask->SetScaleFunction(sfunc);
        //rhotask->SelectCollisionCandidates(kPhysSel);
        rhotask->SetHistoBins(100,0,250);
        }

//#####################################################################################
  //compile and load the custom local task in local macro
  bool useLocal = false;
  bool localMacros = false;

  // compile local task
  if (useLocal)
    {
    TString myTask = "AliAnalysisTaskEmcalJetCDF.cxx";

    // Load aditional code (my task) to alien plugin ;
    ListSources += " " + myTask; // extra space at beginning in case is not first source; it will be stripped.

    if ( kAnalysisMode.EqualTo("local") ) { myTask += "+g"  ;}
    if ( kAnalysisMode.EqualTo("grid") || kAnalysisMode.EqualTo("proof") ) { myTask += "++";}
    if ( gROOT->LoadMacro ( myTask.Data() ) != 0 )  { Printf ( "--->>> !!! compilation failed !!! <<<---" ) ; return; }

    }

//__________________________________________________________________________________
  // load AddTask macros
  if (localMacros)
    {
    gROOT->LoadMacro ( "AddTaskEmcalJetCDF.C" );
    gROOT->LoadMacro ( "AddTaskEmcalJetCDFUE.C" );
    gROOT->LoadMacro ( "AddTaskEmcalJetSample.C" );
    }
  else
    {
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDF.C" );
    gROOT->LoadMacro ( "$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJetCDFUE.C" );
    gROOT->LoadMacro ( "AddTaskEmcalJetSample.C" );
    }

//__________________________________________________________________________________
    //     AliEmcalJetTask* jetFinderTask;
    Double_t     jetareacut           = 0.001 ;
    const char*  tasknamecdf          = "CDF";
    const char*  tasknamecdfue        = "CDFUE";
    const char*  tasknamesample       = "Sample";
    const char*  nrho                 = "";

    AliAnalysisTaskEmcalJetCDF*    anaTaskCDF    = NULL;
    AliAnalysisTaskEmcalJetCDFUE*  anaTaskCDFUE  = NULL;
    AliAnalysisTaskEmcalJetSample* anaTaskSample = NULL;

    Double_t jetpt_cuts[] =
                            {1., 5., 10. ,15., 20., 25., 30., 35., 40.};
                            //{1., 5., 10. ,15., 20., 25., 30.};

    Size_t nrcuts = sizeof(jetpt_cuts)/sizeof(jetpt_cuts[0]); // number of cuts;

    for ( std::vector<TString>::iterator jf_it = jf_names.begin(); jf_it != jf_names.end(); ++jf_it)  // loop over jet finders
        {
        AliEmcalJetTask* jf_task = dynamic_cast<AliEmcalJetTask*>(mgr->GetTask((*jf_it)));
        if (!jf_task) { AliError("No jet finder with the name from jf_names list");}

        for (Size_t k = 0; k < (Size_t)nrcuts; k++ )  // loop over all jet pt cuts
            {
            Double_t jetptmin = jetpt_cuts[(unsigned int)k];
            Double_t jetptmax = ((unsigned int)k == ( nrcuts - 1)) ? 250. : jetpt_cuts[(unsigned int)k+1]; // if last cut, max is unlimited

            tasknamecdf = Form ("CDF%i",(unsigned int)k);
            anaTaskCDF  = AddTaskEmcalJetCDF (jf_task, jetptmin, jetptmax, jetareacut, acceptance_type.Data(), leadhadtype, nrho, tasknamecdf);
            anaTaskCDF->SetDebugLevel(debug);

            tasknamecdfue = Form ("CDFUE%i", (unsigned int)k);
            anaTaskCDFUE  = AddTaskEmcalJetCDFUE (jf_task, jetptmin, jetptmax, jetareacut, acceptance_type.Data(), leadhadtype, nrho, tasknamecdfue);
            anaTaskCDFUE->SetDebugLevel(debug);

            tasknamesample = Form ("Sample%i", (unsigned int)k);
            anaTaskSample = AddTaskEmcalJetSample ( jf_task, 1, jetptmin, jetareacut, acceptance_type.Data(), leadhadtype, nrho, tasknamesample) ;
            anaTaskSample->SetDebugLevel(debug);

            PrintInfoCDFtask(anaTaskCDF->GetName());
            }

        anaTaskCDF  = AddTaskEmcalJetCDF (jf_task, 0., 250., jetareacut, acceptance_type.Data(), leadhadtype, nrho, "CDFT");
        anaTaskCDF->SetDebugLevel(debug);

        anaTaskCDFUE  = AddTaskEmcalJetCDFUE (jf_task, 0., 250., jetareacut, acceptance_type.Data(), leadhadtype, nrho, "CDFUET");
        anaTaskCDFUE->SetDebugLevel(debug);
        }

    // enable class level debugging for these classes
    if ( debug > 3 ) { mgr->AddClassDebug("AliJetContainer", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliEmcalJetTask", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliAnalysisTaskEmcalJetCDF", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliAnalysisTaskEmcalJetCDFUE", 100); }
    if ( debug > 3 ) { mgr->AddClassDebug("AliAnalysisTaskEmcalJetSample", 100); }

//#################################################################
    // Set the physics selection for all given tasks
    TObjArray* toptasks = mgr->GetTasks();

    for ( Int_t i = 0; i < toptasks->GetEntries(); ++i )
        {
        AliAnalysisTaskSE* task = dynamic_cast<AliAnalysisTaskSE*> ( toptasks->At (i) );
        if ( !task ) { continue; }
        if ( task->InheritsFrom ( AliPhysicsSelectionTask::Class() ) ) { continue; }
        ::Info ( "setPSel", "Set physics selection for %s (%s)", task->GetName(), task->ClassName() );
        task->SelectCollisionCandidates ( pSel );
        }

//==========================================
// ######       START ANALYSIS       #######
//==========================================
    if ( mgr->InitAnalysis() )
        {
        cout << "##-->> Initialising Analysis :: Status :" << endl;
        mgr->PrintStatus();

        // grmpf, aliroot error handler overwrites root
        if ( debug == 0 )      { AliLog::SetGlobalLogLevel ( AliLog::kFatal ); }
        else if ( debug == 1 ) { AliLog::SetGlobalLogLevel ( AliLog::kError ); }
        else if ( debug == 2 ) { AliLog::SetGlobalLogLevel ( AliLog::kWarning ); }
        else if ( debug == 3 ) { AliLog::SetGlobalLogLevel ( AliLog::kInfo ); }
        else if ( debug >= 4 ) { AliLog::SetGlobalLogLevel ( AliLog::kDebug ); }

        gErrorIgnoreLevel = kErrorIgnoreLevel;
        if ( gErrorIgnoreLevel > 3000 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal ); }

        // task profiling
        if ( kUseSysInfo > 0 )
            {
            TFile * fM = TFile::Open ( "manager_local.root", "RECREATE" );
            mgr->Write();
            fM->Close();

            for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask (i); }
            }
        mgr->SetDebugLevel(mgr_debug);
        mgr->StartAnalysis ( kAnalysisMode.Data(), nentries );
        }
        // END of mgr->InitAnalysis()

    cout << "EmcalJetCDF END" << endl;
    return 0;
    }
//>>>>>>  END of void EmcalJetCDF (.....)   <<<<<<<<<
//##########################################################################################################################

void AddIncludePaths ()
    {
    TString includes_str = "-Wno-deprecated -I$. -I$CGAL_DIR/include -I$FASTJET/include -I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include";

    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if ( !mgr ) { ::Error ( "EmcalJetCDF.C", "IncludePaths :: No analysis manager to connect to." ); return kFALSE; }

    AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
    if ( !plugin ) { ::Error ( "EmcalJetCDF.C", "IncludePaths :: plugin invalid" ); return kFALSE; }

    gSystem->AddIncludePath(includes_str.Data());
    plugin->AddIncludePath(includes_str.Data());
    }

//______________________________________________________________________________
void LoadLibs ()
    {
    gSystem->Load ("libSTEERBase.so");
    gSystem->Load ("libANALYSIS.so");
    gSystem->Load ("libANALYSISaliceBase.so");
    gSystem->Load ("libANALYSISalice.so");

    TString list_fj         = "CGAL fastjet siscone siscone_spherical fastjetplugins fastjettools fastjetcontribfragile";
    TString list_alicejets  = "PWGEMCAL EventMixing PWGJE FASTJETAN PWGJEEMCALJetTasks";

    LoadLibList (list_fj);
    LoadLibList (list_alicejets);

    ::Info ( "EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS" );

    }

//______________________________________________________________________________
void LoadLibList ( const TString& list )
    {
    TObjArray* arr = list.Tokenize(" ");;
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
void SetJFAccFid( const char* taskname, TString cut = "TPC" )
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

    cout << "\nJet Finder Task Name : " << jf->GetName() << endl;

    cout << "Track Eta MIN : " << jf->GetEtaMin()    << " ; Track Eta MAX : " << jf->GetEtaMax() << endl;
    cout << "JET Eta MIN : "   << jf->GetJetEtaMin() << " ; JET Eta MAX : "   << jf->GetJetEtaMax() << endl;

    cout << "Track Phi MIN : " << jf->GetPhiMin()    << " ; Track Phi MAX : " << jf->GetPhiMax() << endl;
    cout << "JET Phi MIN : "   << jf->GetJetPhiMin() << " ; JET Phi MAX : "   << jf->GetJetPhiMax() << endl;

    cout << "\n"<< endl;
    }

//______________________________________________________________________________
void PrintInfoCDFtask ( const char* taskname, Int_t i = 0 )
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
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C" );
        AliAODInputHandler* aodH = AddAODHandler();
        aodH->SetCheckStatistics ( kTRUE );
        }
    else
    if ( dataType.EqualTo("esd") || dataType.EqualTo("sesd") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C" );
        AliESDInputHandler* esdH = AddESDHandler();
        }
    else
        { cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n"; return; }

    // Create MC handler, if MC is demanded
    if ( isMC && !dataType.EqualTo("aod") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddMCHandler.C" );
        AliMCEventHandler* mcH = AddMCHandler (kTRUE);
        }

    AliInputEventHandler* inputh = mgr->GetInputEventHandler();
    inputh->SetNeedField(kTRUE);

    }

//______________________________________________________________________________
AliAnalysisAlien* CreateAlienHandler ( const char* plugin_mode = "test" )
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


// kate: indent-mode none; indent-width 2; replace-tabs on;
