#ifndef __CINT__
// Standard includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

// ROOT includes
#include "Riostream.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TList.h"
#include "TEnv.h"
#include "TKey.h"
#include "TBits.h"
#include "TError.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "TChain.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TMacro.h"
#include "TGrid.h"
#include "TGridResult.h"
#include "TGridCollection.h"
#include "TGridJDL.h"
#include "TGridJobStatusList.h"
#include "TGridJobStatus.h"
#include "TFileMerger.h"

// AliROOT includes
#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"
#include "AliAnalysisDataContainer.h"
#include "AliMultiInputEventHandler.h"
#include "AliLog.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliESDInputHandler.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskTagCreator.h"
#include "AliPhysicsSelectionTask.h"
#include "AliCentralitySelectionTask.h"
#include "AliEmcalCompatTask.h"
#include "AliEmcalSetupTask.h"
#include "AliAnalysisTaskEmcal.h"
#include "AliEMCALRecParam.h"
#include "AliEmcalJetTask.h"

#include "AliAnalysisTaskEmcal.h"
#include "AliAnalysisTaskEmcalJet.h"

#include "AddTaskEmcalPhysicsSelection.C"
#include "AddTaskEmcalCompat.C" //$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalCompat.C
#include "AddTaskEmcalSetup.C" //$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalSetup.C
#include "AddTaskEMCALTender.C" //$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEMCALTender.C
#include "AddTaskJetPreparation.C" //$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C
#include "AddTaskEmcalJet.C"  //$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C
#include "AddTaskEmcalPreparation.C" // $ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalPreparation.C
#include "AddTaskCentrality.C" //$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C

#include "AliAnalysisTaskEmcalJetCDF.h"
#include "AddTaskEmcalJetCDF.C"
#include "InputData.C"

// JETAN
#include "AddAODHandler.C"
#include "AddESDHandler.C"

#endif

#include <ctime>
#include "TGrid.h"

//______________________________________________________________________________
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//        AliEN plugin variables
//(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
//______________________________________________________________________________

Int_t       kTestFiles               = 10;    // Number of test files
Long64_t    nentries                 = 1234567890; // for local and proof mode, ignored in grid mode. Set to 1234567890 for all events.
Long64_t    firstentry               = 0; // for local and proof mode, ignored in grid mode

TString     kTrainName               = "sev_jets";           // *CHANGE ME* (no blancs or special characters)
TString     kWorkDir                 = "emcalcdf_histos1";   // AliEn work dir; relative to AliEn $HOME
TString     kJobTag                  = "sev_jet_analysis";   // *CHANGE ME*

TString     kPluginExecutableCommand = "aliroot -b -q";
Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

TString     kPluginAPIVersion        = "V1.1x";
TString     kPluginRootVersion       = "v5-34-08-6";
TString     kPluginAliRootVersion    = "vAN-20140712";

TString     kPackage1                = "boost::v1_53_0";
TString     kPackage2                = "cgal::v4.4";
TString     kPackage3                = "fastjet::v3.0.6_1.012";


Bool_t      kPluginFastReadOption    = kFALSE;         // use xrootd tweaks
Bool_t      kPluginOverwriteMode     = kTRUE;          // overwrite existing collections

Int_t       kGridFilesPerJob         = 70;    // Maximum number of files per job (gives size of AOD)
Int_t       kGridMaxMergeFiles       = 100;   // Number of files merged in a chunk grid run range
Int_t       kMaxInitFailed           = 10 ;    // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

Int_t       kTTL                     = 14400 ; // Time To Live
Int_t       kMergeTTL                = 1800 ;  // Time To Live merging

Int_t       kGridRunsPerMaster       = 1; // Number of runs per master job
Bool_t      kPluginMergeViaJDL       = kTRUE;          // merge via JDL

Int_t       kPluginOutputToRunNumber = 1;              // write the output to subdirs named after run number

TString     kGridOutdir              = "out"; // AliEn output directory. If blank will become output_<kTrainName>
TString     kGridDataSet             = ""; // sub working directory not to confuse different run xmls
TString     kGridExtraAliendirLevel  = ""; // sub working directory not to confuse different run xmls

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

Int_t   kProofReset = 0  ;
Int_t   kWorkers    = 20 ;
Int_t   kCores      = 8  ;

//______________________________________________________________________________
//  DEFINED INPUT DATA in AlienHandler.C

TString   kInputData =
    "pp_lhc11a";
//                       "pp_lhc10e";

// Debug
Int_t   debug = 100;

//==============================================================================
Bool_t      kSkipTerminate      = kTRUE; // Do not call Terminate
Bool_t      kUseDebug           = kTRUE; // activate debugging
Int_t       kErrorIgnoreLevel   = -1; // takes the errror print level from .rootrc
Bool_t      kUsePAR             = kFALSE; // use par files for extra libs

unsigned int kUseSysInfo        = 0 ;           // activate debugging

//______________________________________________________________________________
// Containers for IO file names
TString kDataset = "";
TString kDatafile = "";

// FILES USED IN MACRO
TString     kCommonOutputFileName   = "CDFanalysis.root";

// == grid plugin files rules
TString     kGridExtraFiles          = "";     // LIBRARIES files that will be added to the input list in the JDL...
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

TString ListPars = "";
TString ListLibs = "";
TString ListLibsExtra = "";
TString ListSources = "";

//______________________________________________________________________________
// enum Jets used
enum AlgoType {kKT, kANTIKT};
enum JetType  {kFULLJETS, kCHARGEDJETS, kNEUTRALJETS};

//______________________________________________________________________________
// Analysis steering variables
TString        runPeriod           = "LHC10e";            // set the run period (used on grid)
TString        dataType            = "AOD";               // set the analysis type, AOD, ESD or sESD
UInt_t         pSel                = AliVEvent::kMB;      // used event selection for every task except for the analysis tasks
Bool_t         useTender           = kTRUE;               // trigger, if tender task should be used
Bool_t         isMC                = kFALSE;              // trigger, if MC handler should be used

// Function signatures
class AliAnalysisGrid;
class AliAnalysisAlien;
class AliAnalysisManager;

void EmcalJetCDF ( const char* plugin_mode = "test" , const char* analysis_mode = "local", const char* input = "data.txt" )
    {
    LoadLibs( kTRUE ); // load FJ3 libs  // Load necessary libraries for the script

    TString   kPluginMode   = plugin_mode   ; kPluginMode.ToLower();    // test, offline, submit, terminate, full
    TString   kAnalysisMode = analysis_mode ; kAnalysisMode.ToLower();  // local, grid, proof

    gSystem->SetFPEMask(); // because is used in reference script

    //==========================================================================
    // SET UP AliEn handler -> main analysis engine : we will use many internal tools of AliAnalysisAlien
    //==========================================================================
    AliAnalysisAlien* plugin = CreateAlienHandler ( kPluginMode.Data() );

    //______________________________________________________________________________
    // Prepare input decisions
    TString input_data = input;

    if ( input_data.EndsWith ( ".txt" ) )
        { kDatafile = input_data; }
    else
        { kDataset = input_data; }

    if ( ( !kAnalysisMode.CompareTo ( "local" ) ) || ( !kPluginMode.CompareTo ( "test" ) ) ) { kUsePAR = kFALSE; }


    // Analysis input type
    dataType.ToLower();
    cout << "Data type : " << dataType.Data() << " analysis chosen" << endl;

    if ( !kAnalysisMode.CompareTo ("grid") && !dataType.CompareTo("sesd") ) { cout << "Skimmed ESD analysis not available on the grid!" << endl; return; }

    //******************
    //    DATA INPUT
    //******************
    // Load InputData macro ++++++++++++
    gROOT->LoadMacro ( "InputData.C" );
    //++++++++++++++++++++++++++++++++++
    InputData ( plugin, kInputData );   // fDataPattern

    // .txt file containing the list of files to be chained in test mode
    plugin->SetFileForTestMode ( kDatafile.Data() );   //fFileForTestMode

    // to set up also runPeriod from string;
    // irrelevant at this moment what exactly is runPeriod unless MC!!!!


    //*******************************************
    //   Loading of libraries (script + plugin)
    //*******************************************
    //compile and load the task in local macro
    TString myTaskCompileOpt = "AliAnalysisTaskEmcalJetCDF.cxx";
    if ( !kAnalysisMode.CompareTo("local") ) { myTaskCompileOpt += "+g"  ;}
    if ( !kAnalysisMode.CompareTo("grid") || !kAnalysisMode.CompareTo("proof") ) { myTaskCompileOpt += "++";}

    if ( gROOT->LoadMacro ( myTaskCompileOpt.Data() ) != 0 )  { Printf ( "--->>> !!! compilation failed !!! <<<---" ) ; return; }

    // Load aditional code (my task) to alien plugin ;
    // TProof.html#TProof:Load -> a basename(macro).h or .hh will be automatically loaded if in the same directory
    ListSources += " AliAnalysisTaskEmcalJetCDF.cxx"; // extra space at beginning in case is not first source; it will be stripped.

    //////////////////////////////////////////
    ////   LIBRARIES TO BE LOADED BY PLUGIN
    /////////////////////////////////////////
    ListLibs += kGridExtraFiles;

    ListLibs       = ListLibs.Strip();         Printf ( "### ListLibs : %s", ListLibs.Data() );
    ListLibsExtra  = ListLibsExtra.Strip();    Printf ( "### ListLibsExtra : %s", ListLibsExtra.Data() );
    ListSources    = ListSources.Strip();      Printf ( "### ListSources : %s", ListSources.Data() );

    if ( ListLibs.Length() )       { plugin->SetAdditionalLibs     ( ListLibs.Data() ); }
    if ( ListLibsExtra.Length() )  { plugin->SetAdditionalRootLibs ( ListLibsExtra.Data() ); }
    if ( ListSources.Length() )    { plugin->SetAnalysisSource     ( ListSources.Data() ); }

//______________________________________________________________________________
    //#######################
    //   ANALYSIS MANAGER
    //#######################
    // Make the analysis manager and connect event handlers
    AliAnalysisManager* mgr  = plugin->CreateAnalysisManager ( "CDFhistos_mgr" );

    if ( kCommonOutputFileName.Length() > 0 ) { mgr->SetCommonFileName ( kCommonOutputFileName.Data() ); }

    mgr->SetSkipTerminate ( kSkipTerminate );

    //<><><><><><><><><><>
    //    DEBUGGING
    //<><><><><><><><><><>
    if ( kUseDebug && ( ( !kAnalysisMode.CompareTo ( "local" ) ) || ( !kPluginMode.CompareTo ( "test" ) ) ) ) { mgr->SetDebugLevel ( 10 ); }

    // Event frequency for collecting system information
    mgr->SetNSysInfo ( kUseSysInfo );

    if ( kUseSysInfo > 0 )
        {
        mgr->RegisterExtraFile ( "syswatch.root" );

        if ( kGridMergeExclude.Length() ) { kGridMergeExclude += " "; }
        kGridMergeExclude += "syswatch.root";
        }
    else
        { AliLog::SetGlobalLogLevel ( AliLog::kError ); }


    //********************
    //    DATA OUTPUT
    //********************
    plugin->SetMergeExcludes  ( kGridMergeExclude );
    plugin->SetDefaultOutputs ( kTRUE );

    //********************************************

    if ( !dataType.CompareTo("aod") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C" );
        AliAODInputHandler* aodH = AddAODHandler();
        aodH->SetCheckStatistics ( kTRUE );
        }
    else
    if ( !dataType.CompareTo("esd") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C" );
        AliESDInputHandler* esdH = AddESDHandler();
        }
    else
    if ( !dataType.CompareTo("sesd") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C" );
        AliESDInputHandler* esdH = AddESDHandler();
        }
    else
        { cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n"; return; }

    // Create MC handler, if MC is demanded
    if ( isMC && dataType.CompareTo("aod") ) // CompareTo returns 0 for matching - aod is excluded
        {
        AliMCEventHandler* mcH = new AliMCEventHandler();
        mcH->SetPreReadMode ( AliMCEventHandler::kLmPreRead );
        mcH->SetReadTR ( kTRUE );
        mgr->SetMCtruthEventHandler ( mcH );
        }

    // ################# Now: Add some basic tasks
//______________________________________________________________________________
// Signature PhysicsSelectionTask
    Bool_t     isLHC11a          = kFALSE;  // true then skip FastOnly events (only for LHC11a)
    Bool_t     withHistos        = kTRUE;   // true then write output
    UInt_t     triggers          = pSel;    // if not zero only process given trigges
    Double_t   minE              = 5;       // minimum clus energy (<0 -> do not compute)
    Double_t   minPt             = 5;       // minimum track pt (<0 -> do not compute)
    Double_t   vz                = 10;      // primary vertex z cut (-1 none)
    Bool_t     vzdiff            = kTRUE;   // true then select on PRI minus SPD z-vertex
    Double_t   cmin              = -1;      // minimum centrality required (V0M)
    Double_t   cmax              = -1;      // maximum centrality required (V0M)
    Double_t   minCellTrackScale = -1;      // minimum cells over tracks scale
    Double_t   maxCellTrackScale = -1;      // maximum cells over tracks scale

    // Physics selection task
    gROOT->LoadMacro ( "$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C" );
    AliPhysicsSelectionTask* physSelTask = AddTaskEmcalPhysicsSelection ( isLHC11a, withHistos, triggers, minE, minPt, vz, vzdiff, cmin, cmax, minCellTrackScale, maxCellTrackScale );
    if ( !physSelTask ) { cout << "----------  no physSelTask"; return; }

//______________________________________________________________________________
    // Centrality task
    if ( !dataType.CompareTo("esd") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C" );
        AliCentralitySelectionTask* centralityTask = AddTaskCentrality ( kTRUE );
        }
//______________________________________________________________________________
    // Compatibility task, only needed for skimmed ESD
    if ( !dataType.CompareTo("sesd") )
        {
        gROOT->LoadMacro ( "$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalCompat.C" );
        AliEmcalCompatTask* comptask = AddTaskEmcalCompat();
        }
//______________________________________________________________________________
    // Setup task
    gROOT->LoadMacro ( "$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalSetup.C" );
    AliEmcalSetupTask* setupTask = AddTaskEmcalSetup();
    setupTask->SetGeoPath ( "$ALICE_ROOT/OADB/EMCAL" );

//______________________________________________________________________________
    // Tender Supplies
    if (useTender)
        {
        gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalPreparation.C");
        //adjust pass when running locally. On grid give empty string, will be picked up automatically from path to ESD/AOD file
        // const char *perstr  = "LHC11h"
        // const char *pass    = 0 /*should not be needed*/
        AliAnalysisTaskSE* clusm = AddTaskEmcalPreparation(runPeriod,"pass1");
        }

    // ################# Now: Call jet preparation macro (picotracks, hadronic corrected caloclusters, ...)

    //______________________________________________________________________________
    // Objects (branch names) used in Jet framework
    TString tracksName         = "PicoTracks";
    TString clustersName       = "EmcCaloClusters";
    TString clustersCorrName   = "CaloClustersCorr";
    TString rhoName            = "";

    //______________________________________________________________________________
    // Jet preparation
    gROOT->LoadMacro ( "$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C" );
    //     const char*    periodstr          = "LHC11h",
    //     const char*    pTracksName        = "PicoTracks",
    //     const char*    usedMCParticles    = "MCParticlesSelected",
    //     const char*    usedClusters       = "CaloClusters",
    //     const char*    outClusName        = "CaloClustersCorr",
    //     const Double_t hadcorr            = 2.0,
    //     const Double_t Eexcl              = 0.00,
    //     const Double_t phiMatch           = 0.03,
    //     const Double_t etaMatch           = 0.015,
    //     const Double_t minPtEt            = 0.15,
    //     const UInt_t   pSel               = AliVEvent::kAny,
    //     const Bool_t   trackclus          = kTRUE,
    //     const Bool_t   doHistos           = kFALSE,
    //     const Bool_t   makePicoTracks     = kTRUE,
    //     const Bool_t   makeTrigger        = kTRUE,
    //     const Bool_t   isEmcalTrain       = kFALSE,
    //     const Double_t trackeff           = 1.0,
    //     const Bool_t   doAODTrackProp     = kFALSE,
    //     const Bool_t   modifyMatchObjs    = kTRUE
    TString particlesMCName   = "";
    if ( isMC ) {particlesMCName  = "MCParticlesSelected";}
    AddTaskJetPreparation(runPeriod.Data(), tracksName.Data(), particlesMCName.Data(), clustersName.Data(), clustersCorrName.Data());

    // ################# Now: Add jet finders+analyzers

    gROOT->LoadMacro ( "$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C" );
//------------   DEFAULTS   -----------------------------------------------------
    Int_t          algo            = kANTIKT; // 0 --> AliEmcalJetTask::kKT ; != 0 --> AliEmcalJetTask::kAKT
    Double_t       radius          = 0.2;
    Int_t          type            = kCHARGEDJETS; // 0 --> AliEmcalJetTask::kFullJet; 1 --> AliEmcalJetTask::kChargedJet; 2 --> AliEmcalJetTask::kNeutralJet
    Double_t       minTrPt         = 0.15;
    Double_t       minClPt         = 0.30;
    Double_t       ghostArea       = 0.01;
    Int_t          recombScheme    = 1;
    const char*    tag             = "Jet";
    Double_t       minJetPt        = 1.;
    Bool_t         selectPhysPrim  = kFALSE;
    Bool_t         lockTask        = kTRUE;
//------------------------------------------------------------------------------

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 1.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_1_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_1_04_chg Task Name : " << jetFinderTask_015_1_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 1.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_1_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_1_06_chg Task Name : " << jetFinderTask_015_1_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 10.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_10_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_10_04_chg Task Name : " << jetFinderTask_015_10_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 10.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_10_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_10_06_chg Task Name : " << jetFinderTask_015_10_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 20.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_20_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_20_04_chg Task Name : " << jetFinderTask_015_20_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 20.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_20_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_20_06_chg Task Name : " << jetFinderTask_015_20_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 30.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_30_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_30_04_chg Task Name : " << jetFinderTask_015_30_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 30.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_30_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_30_06_chg Task Name : " << jetFinderTask_015_30_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 40.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_40_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_40_04_chg Task Name : " << jetFinderTask_015_40_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 40.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_40_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_40_06_chg Task Name : " << jetFinderTask_015_40_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 50.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_50_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_50_04_chg Task Name : " << jetFinderTask_015_50_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 50.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_50_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_50_06_chg Task Name : " << jetFinderTask_015_50_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 60.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_60_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_60_04_chg Task Name : " << jetFinderTask_015_60_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 60.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_60_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_60_06_chg Task Name : " << jetFinderTask_015_60_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 70.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_70_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_70_04_chg Task Name : " << jetFinderTask_015_70_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 70.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_70_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_70_06_chg Task Name : " << jetFinderTask_015_70_06_chg->GetName() << endl;

    //______________________________________________________________________________
    minTrPt = 0.15;  minJetPt = 80.;  radius = 0.4;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_80_04_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_80_04_chg Task Name : " << jetFinderTask_015_80_04_chg->GetName() << endl;

    minTrPt = 0.15;  minJetPt = 80.;  radius = 0.6;  type = kCHARGEDJETS;
    AliEmcalJetTask* jetFinderTask_015_80_06_chg = AddTaskEmcalJet( tracksName.Data(), clustersCorrName.Data(), algo, radius, type, minTrPt, minClPt, ghostArea, recombScheme, tag, minJetPt, selectPhysPrim, lockTask);
    cout << "jetFinder_015_80_06_chg Task Name : " << jetFinderTask_015_80_06_chg->GetName() << endl;

//#####################################################################################

    gROOT->LoadMacro ( "AddTaskEmcalJetCDF.C" );
    //     AliEmcalJetTask* jetFinderTask,
    //     Double_t     jetareacut   = 0.557,
    //     const char*  type         = "EMCAL",
    //     Int_t        leadhadtype  = 0,         // AliJetContainer :: Int_t fLeadingHadronType;  0 = charged, 1 = neutral, 2 = both
    //     const char*  taskname     = "TaskEmcalJetCDF"

    AliAnalysisTaskEmcalJetCDF* anaTask1  = AddTaskEmcalJetCDF (jetFinderTask_015_1_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask2  = AddTaskEmcalJetCDF (jetFinderTask_015_1_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask3  = AddTaskEmcalJetCDF (jetFinderTask_015_10_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask4  = AddTaskEmcalJetCDF (jetFinderTask_015_10_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask5  = AddTaskEmcalJetCDF (jetFinderTask_015_20_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask6  = AddTaskEmcalJetCDF (jetFinderTask_015_20_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask7  = AddTaskEmcalJetCDF (jetFinderTask_015_30_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask8  = AddTaskEmcalJetCDF (jetFinderTask_015_30_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask9  = AddTaskEmcalJetCDF (jetFinderTask_015_40_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask10 = AddTaskEmcalJetCDF (jetFinderTask_015_40_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask11 = AddTaskEmcalJetCDF (jetFinderTask_015_50_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask12 = AddTaskEmcalJetCDF (jetFinderTask_015_50_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask13 = AddTaskEmcalJetCDF (jetFinderTask_015_60_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask14 = AddTaskEmcalJetCDF (jetFinderTask_015_60_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask15 = AddTaskEmcalJetCDF (jetFinderTask_015_70_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask16 = AddTaskEmcalJetCDF (jetFinderTask_015_70_06_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask17 = AddTaskEmcalJetCDF (jetFinderTask_015_80_04_chg);
    AliAnalysisTaskEmcalJetCDF* anaTask18 = AddTaskEmcalJetCDF (jetFinderTask_015_80_06_chg);


//#################################################################
    // Set the physics selection for all given tasks
    TObjArray *toptasks = mgr->GetTasks();

    for ( Int_t i = 0; i < toptasks->GetEntries(); ++i )
        {
        AliAnalysisTaskSE* task = dynamic_cast<AliAnalysisTaskSE*> ( toptasks->At ( i ) );
        if ( !task ) { continue; }
        if ( task->InheritsFrom ( "AliPhysicsSelectionTask" ) ) { continue; }
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
        AliLog::SetGlobalLogLevel ( AliLog::kError );

        gErrorIgnoreLevel = kErrorIgnoreLevel;

        if ( gErrorIgnoreLevel > 3000 ) { AliLog::SetGlobalLogLevel ( AliLog::kFatal ); }

        if ( ( kUseSysInfo > 0 ) && ( !kPluginMode.CompareTo ( "test" ) ) )
            { TFile * fM = TFile::Open ( "manager_local.root", "RECREATE" ); mgr->Write(); fM->Close(); }

        // task profiling
        if ( ( kUseSysInfo > 0 ) || ( !kPluginMode.CompareTo ( "test" ) ) )
            { for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask ( i ); } }

        if ( !mgr->GetGridHandler() ) { ::Error ( "AnalysisCDFjets::StartAnalysis", "Grid plugin not initialized" ); return; }

        mgr->StartAnalysis ( kAnalysisMode.Data(), nentries, firstentry );
        }

    // END of mgr->InitAnalysis()


    }

//______________________________________________________________________________
void LoadLibs( Bool_t isFJ3 = kFALSE)
    {
    // FASTJET LIBS
    gSystem->Load("libCGAL");
    gSystem->Load("libfastjet");
    gSystem->Load("libsiscone");
    gSystem->Load("libsiscone_spherical");
    gSystem->Load("libfastjetplugins");

    if (isFJ3)
        {
        gSystem->Load("libfastjettools");
        gSystem->Load("libfastjetcontribfragile");
        }

    // Load common libraries (better too many than too few)
    gSystem->Load ( "libTree" );
    gSystem->Load ( "libVMC" );
    gSystem->Load ( "libGeom" );
    gSystem->Load ( "libGui" );
    gSystem->Load ( "libXMLParser" );
    gSystem->Load ( "libMinuit" );
    gSystem->Load ( "libMinuit2" );
    gSystem->Load ( "libProof" );
    gSystem->Load ( "libPhysics" );
    gSystem->Load ( "libSTEERBase" );
    gSystem->Load ( "libESD" );
    gSystem->Load ( "libAOD" );
    gSystem->Load ( "libOADB" );
    gSystem->Load ( "libANALYSIS" );
    gSystem->Load ( "libCDB" );
    gSystem->Load ( "libRAWDatabase" );
    gSystem->Load ( "libSTEER" );
    gSystem->Load ( "libEVGEN" );
    gSystem->Load ( "libANALYSISalice" );
    gSystem->Load ( "libCORRFW" );
    gSystem->Load ( "libTOFbase" );
    //gSystem->Load("libTOFrec");
    gSystem->Load ( "libRAWDatabase.so" );
    gSystem->Load ( "libRAWDatarec.so" );
    gSystem->Load ( "libTPCbase.so" );
    gSystem->Load ( "libTPCrec.so" );
    gSystem->Load ( "libITSbase.so" );
    gSystem->Load ( "libITSrec.so" );
    gSystem->Load ( "libTRDbase.so" );
    gSystem->Load ( "libTENDER.so" );
    gSystem->Load ( "libSTAT.so" );
    gSystem->Load ( "libTRDrec.so" );
    gSystem->Load ( "libHMPIDbase.so" );
    gSystem->Load ( "libPWGPP.so" );
    gSystem->Load ( "libPWGHFbase" );
    gSystem->Load ( "libPWGDQdielectron" );
    gSystem->Load ( "libPWGHFhfe" );
    gSystem->Load ( "libEMCALUtils" );
    gSystem->Load ( "libPHOSUtils" );
    gSystem->Load ( "libPWGCaloTrackCorrBase" );
    gSystem->Load ( "libEMCALraw" );
    gSystem->Load ( "libEMCALbase" );
    gSystem->Load ( "libEMCALrec" );
    gSystem->Load ( "libTRDbase" );
    gSystem->Load ( "libVZERObase" );
    gSystem->Load ( "libVZEROrec" );
    gSystem->Load ( "libTENDER" );
    gSystem->Load ( "libTENDERSupplies" );
    gSystem->Load ( "libPWGTools" );
    gSystem->Load ( "libPWGEMCAL" );
    gSystem->Load ( "libESDfilter.so" );
    gSystem->Load ( "libPWGGAEMCALTasks" );
    gSystem->Load ( "libPWGTools" );
    gSystem->Load ( "libPWGCFCorrelationsBase" );
    gSystem->Load ( "libPWGCFCorrelationsDPhi" );

    gSystem->Load ( "libJETAN" );
    gSystem->Load ( "libFASTJETAN" );
    gSystem->Load ( "libPWGJEEMCALJetTasks" );


    ::Info ( "EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS" );

    // include paths
    gSystem->AddIncludePath ( "-Wno-deprecated" );
    gSystem->AddIncludePath ( "-I$. -I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT/include -I$ALICE_ROOT/JETAN -I$ALICE_ROOT/EMCAL" );
//     gSystem->AddIncludePath("-I$ALICE_ROOT/PWGDQ/dielectron -I$ALICE_ROOT/PWGHF/hfe");
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

    // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
    plugin->SetNtestFiles ( kTestFiles );

    if ( kPluginUseProductionMode ) { plugin->SetProductionMode(); }

    plugin->SetExecutableCommand ( kPluginExecutableCommand.Data() );
    plugin->SetJobTag ( kJobTag );

    // Set versions of used packages
    plugin->SetAPIVersion     ( kPluginAPIVersion );
    plugin->SetROOTVersion    ( kPluginRootVersion );
    plugin->SetAliROOTVersion ( kPluginAliRootVersion );

    // Add external packages
    plugin->AddExternalPackage ( kPackage1.Data() );
    plugin->AddExternalPackage ( kPackage2.Data() );
    plugin->AddExternalPackage ( kPackage3.Data() );

    // Use fastread option
    plugin->SetFastReadOption ( kPluginFastReadOption );

    // UseOverwrite mode
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
    plugin->SetInputFormat ( "xml-single" );

    // Optionally modify job price (default 1)
    plugin->SetPrice ( 1 );

    plugin->SetNrunsPerMaster ( kGridRunsPerMaster );
    plugin->SetMergeViaJDL ( kPluginMergeViaJDL );

    // write the output to subdirs named after run number
    plugin->SetOutputToRunNo ( kPluginOutputToRunNumber );

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
Bool_t LoadLibrary ( const char *module, Bool_t rec = kFALSE )
    {
    Int_t result = -999 ; TString mod ( module );

    if ( !mod.Length() ) { ::Error ( "AnalysisCDFjets::LoadLibrary", "Empty module name" ); return kFALSE; }

    if ( mod.EndsWith ( ".so" ) ) { mod.Remove ( mod.Index ( ".so" ) ); }

    result = gSystem->Load ( mod );

    if ( result < 0 ) { ::Error ( "AnalysisCDFjets::LoadLibrary", "Could not load library %s", module ); return kFALSE; }

    ListLibs      += Form ( "%s.so ", mod.Data() );
    ListLibsExtra += Form ( "%s.so ", mod.Data() );

    return kTRUE;
    }


//______________________________________________________________________________
Bool_t LoadPars ( AliAnalysisAlien* plugin, const char *module, Bool_t rec = kFALSE )
    {
    // Load a module library in a given mode. Reports success.
    Int_t result = -999; Bool_t success = kTRUE;

    TString mod ( module );

    if ( !mod.Length() ) { ::Error ( "AnalysisCDFjets::LoadLibrary", "Empty module name" ); return kFALSE; }

    if ( !plugin->SetupPar ( module ) ) { return kFALSE; }

    mod += ".par"; plugin->EnablePackage ( mod.Data() ); anaPars += mod.Data();

    //   mod.ReplaceAll(".par",".so"); mod.Prepend("lib"); result = gSystem->Load(mod);
    //   if ( result < 0 ) { cout << "Load PARs : Load library failed : " << mod.Data() << endl; return kFALSE ; }

    return success;
    }

//______________________________________________________________________________
Bool_t LoadSource ( const char *source, Bool_t rec = kFALSE )
    {
    // Load a module library in a given mode. Reports success.
    Int_t result = -1;
    TString ssource ( source );
    TString basename = gSystem->BaseName ( ssource.Data() );

    if ( !ssource.Length() )
        {
        ::Error ( "AnalysisPWG4CDF::LoadSource", "Empty task name" );
        return kFALSE;
        }

    // we have a source code so compile it
    if ( ssource.EndsWith ( ".cxx" ) )
        {
        // need to copy it here other wise the path is also used on grid...
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


    if ( result < 0 ) { ::Error ( "AnalysisCDFjets::LoadSources", "Could not load source %s", source ); return kFALSE; }

    return kTRUE;
    }


// kate: indent-mode cstyle; indent-width 4; replace-tabs on;
