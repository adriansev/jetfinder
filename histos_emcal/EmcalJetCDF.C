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
#include "AliAnalysisTaskEmcalJetSample.h"

#include "AliAnalysisTaskEmcalJetCDF.h"
#include "AddTaskEmcalJetCDF.C"

// JETAN DEV
#include "AliAnalysisTaskJetsReader.h"
#include "AliAnalysisTaskJetsFinder.h"

#include "AddTaskEmcalPhysicsSelection.C"
#include "AddAODHandler.C"
#include "AddESDHandler.C"

#include "AddTaskCentrality.C"
#include "AddTaskEmcalCompat.C"
#include "AddTaskEmcalSetup.C"
#include "AddTaskEMCALTender.C"
#include "AddTaskJetPreparation.C"
#include "AddTaskEmcalJet.C"
#include "AddTaskEmcalJetSample.C"
#include "CreateAODChain.C"
#include "CreateESDChain.C"
#include "InputData.C"

#endif

#include <ctime>
#include "TGrid.h"

//______________________________________________________________________________
//  DEFINED INPUT DATA in AlienHandler.C

//LHC11a(Pass3_wSDD)    LHC period LHC11a (2.76TeV) - Pass3
// "pp276_lhc11a"

//pp, Pythia, 0.5T, 2760 GeV, Min.Bias, LHC11a anchor runs, ID #263
// "pp276_lhc11b10a"

// LHC10c(2)       LHC period LHC10c - Pass2 (7TeV)
// "pp7_lhc10c"

// pp, Pythia, 0.5T, 7 TeV, LHC10e anchor runs (10% statistics), ID #233
// "pp7_lhc10e"


TString   kInputData =
"pp_lhc11a";
//                       "pp_lhc10e";

// Debug
Int_t   debug = 100;

const Long64_t nentries = 500; // for local and proof mode, ignored in grid mode. Set to 1234567890 for all events.
const Long64_t firstentry = 0; // for local and proof mode, ignored in grid mode

//______________________________________________________________________________
// PROOF SETTINGS
//______________________________________________________________________________

TString kAAF        =
//                         "asevcenc@alice-caf.cern.ch" ;
"asevcenc@skaf.saske.sk";

Int_t   kProofReset = 0  ;
Int_t   kWorkers    = 20 ;
Int_t   kCores      = 8  ;


//==============================================================================
Bool_t      kSkipTerminate      = kTRUE; // Do not call Terminate
Bool_t      kUseDebug           = kTRUE; // activate debugging
Int_t       kErrorIgnoreLevel   = -1; // takes the errror print level from .rootrc
Bool_t      kUsePAR             = kFALSE; // use par files for extra libs

unsigned int kUseSysInfo        = 0 ;           // activate debugging

TString kDataset = "";

// FILES USED IN MACRO
TString     kCommonOutputFileName   = "CDFanalysis_out.root";

// == grid plugin files rules
TString     kGridExtraFiles          = "";     // LIBRARIES files that will be added to the input list in the JDL...
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages

TString ListPars = "";
TString ListLibs = "";
TString ListLibsExtra = "";
TString ListSources = "";

// Function signatures
class AliAnalysisGrid;
class AliAnalysisAlien;
class AliAnalysisManager;

// AliAnalysisAlien* CreateAlienHandler (const char* plugin_mode="test") ;

void EmcalJetCDF ( const char* plugin_mode="test" , const char* analysis_mode="local", const char* dataset="" )
{
  TString   kPluginMode   = plugin_mode   ; kPluginMode.ToLower();    // test, offline, submit, terminate, full
  TString   kAnalysisMode = analysis_mode ; kAnalysisMode.ToLower();  // local, grid, proof

  kDataset = dataset;

  if ( (!kAnalysisMode.CompareTo("local")) || (!kPluginMode.CompareTo("test")) ) { kUsePAR = kFALSE; }

  const char*    dataType            = "AOD";                       // set the analysis type, AOD, ESD or sESD
  Bool_t         useGrid             = kFALSE;                      // local or grid
  const char*    gridMode            = "test";                      // set the grid run mode (can be "full", "test", "offline", "submit" or "terminate")
  const char*    uniqueName          = "EMCalJF_LEGOTrainTest";     // sets base string for the name of the task on the grid
  UInt_t         pSel                = AliVEvent::kAny;             // used event selection for every task except for the analysis tasks
  Bool_t         useTender           = kFALSE;                      // trigger, if tender task should be used
  Bool_t         isMC                = kFALSE;                      // trigger, if MC handler should be used

  enum AlgoType {kKT, kANTIKT};
  enum JetType  {kFULLJETS, kCHARGEDJETS, kNEUTRALJETS};

  // change this objects to strings
  TString usedData (dataType); cout << dataType << " analysis chosen" << endl;

  if ( !kAnalysisMode.CompareTo("grid") && (usedData == "sESD") ) { cout << "Skimmed ESD analysis not available on the grid!" << endl; return; }

  // Load necessary libraries
  LoadLibs();

  //compile and load the task in local macro
  if (gROOT->LoadMacro("AliAnalysisTaskEmcalJetCDF.cxx++g") != 0)  { Printf ("--->>> !!! compilation failed !!! <<<---") ; return; }

  // Load InputData macro ++++++++++++
  gROOT->LoadMacro("InputData.C");
  //++++++++++++++++++++++++++++++++++

  //==========================================================================
  // SET UP AliEn handler -> main analysis engine : we will use many internal tools of AliAnalysisAlien
  //==========================================================================
  AliAnalysisAlien* plugin = CreateAlienHandler(kPluginMode.Data());

  // Load aditional code (my task) to alien plugin ;
  // TProof.html#TProof:Load -> a basename(macro).h or .hh will be automatically loaded if in the same directory
  ListSources += " AliAnalysisTaskEmcalJetCDF.cxx"; // extra space at beginning in case is not first source; it will be stripped.

  //////////////////////////////////////////
  ////   LIBRARIES TO BE LOADED BY PLUGIN
  /////////////////////////////////////////
  ListLibs += kGridExtraFiles;

  ListLibs       = ListLibs.Strip();         Printf("### ListLibs : %s",ListLibs.Data());
  ListLibsExtra  = ListLibsExtra.Strip();    Printf("### ListLibsExtra : %s",ListLibsExtra.Data());
  ListSources    = ListSources.Strip();      Printf("### ListSources : %s",ListSources.Data());

  if (ListLibs.Length())       { plugin->SetAdditionalLibs(ListLibs.Data()); }
  if (ListLibsExtra.Length())  { plugin->SetAdditionalRootLibs(ListLibsExtra.Data()); }
  if (ListSources.Length())    { plugin->SetAnalysisSource(ListSources.Data()); }

  //******************
  //    DATA INPUT
  //******************
  InputData ( plugin, kInputData );

  // .txt file containing the list of files to be chained in test mode
  plugin->SetFileForTestMode("data.txt");

  //********************
  //    DATA OUTPUT
  //********************
  plugin->SetMergeExcludes  (kGridMergeExclude);

  plugin->SetDefaultOutputs(kTRUE);




  //#######################
  //   ANALYSIS MANAGER
  //#######################
  // Make the analysis manager and connect event handlers
  AliAnalysisManager* mgr  = plugin->CreateAnalysisManager ("CDFhistos_mgr");
  if ( kCommonOutputFileName.Length() > 0 ) { mgr->SetCommonFileName( kCommonOutputFileName.Data() ); }

  mgr->SetSkipTerminate(kSkipTerminate);

  //<><><><><><><><><><>
  //    DEBUGGING
  //<><><><><><><><><><>
  if ( kUseDebug && ((!kAnalysisMode.CompareTo("local")) || (!kPluginMode.CompareTo("test"))) ) { mgr->SetDebugLevel(10); }

  if (!kPluginMode.CompareTo("test")) { kUseSysInfo = 1 ; }

  // Event frequency for collecting system information
  mgr->SetNSysInfo(kUseSysInfo);

  if ( kUseSysInfo > 0 )
    {
    mgr->RegisterExtraFile("syswatch.root");
    if (kGridMergeExclude.Length()) { kGridMergeExclude += " "; }
    kGridMergeExclude += "syswatch.root";
    }
  else
    { AliLog::SetGlobalLogLevel(AliLog::kError); }

  // Check type of input and create handler for it
  TString localFiles("-1");
  if (usedData == "AOD")
    {
    localFiles = "files_aod.txt";
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
    AliAODInputHandler* aodH = AddAODHandler();
    }
  else
  if (usedData == "ESD")
    {
    localFiles = "files_esd.txt";
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
    AliESDInputHandler* esdH = AddESDHandler();
    }
  else
  if (usedData == "sESD")
    {
    localFiles = "files_sesd.txt";
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
    AliESDInputHandler* esdH = AddESDHandler();
    }
  else
    { cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n"; }

  if (!kAnalysisMode.CompareTo("grid")) { cout << "Using " << localFiles.Data() << " as input file list.\n"; }

  // Create MC handler, if MC is demanded
  if (isMC && (usedData != "AOD"))
    {
    AliMCEventHandler* mcH = new AliMCEventHandler();
    mcH->SetPreReadMode(AliMCEventHandler::kLmPreRead);
    mcH->SetReadTR(kTRUE);
    mgr->SetMCtruthEventHandler(mcH);
    }

  // ################# Now: Add some basic tasks

  // Physics selection task
  gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C");
  AliPhysicsSelectionTask* physSelTask = AddTaskEmcalPhysicsSelection(kTRUE, kTRUE, pSel, 5, 5, 10, kTRUE, -1, -1, -1, -1);
  if (!physSelTask) { cout << "no physSelTask"; return; }

  // Centrality task
  if (usedData == "ESD")
    {
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C");
    AliCentralitySelectionTask *centralityTask = AddTaskCentrality(kTRUE);
    }

  // Compatibility task, only needed for skimmed ESD
  if (usedData == "sESD")
    {
    gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalCompat.C");
    AliEmcalCompatTask *comptask = AddTaskEmcalCompat();
    }

  // Setup task
  gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalSetup.C");
  AliEmcalSetupTask *setupTask = AddTaskEmcalSetup();
  setupTask->SetGeoPath("$ALICE_ROOT/OADB/EMCAL");

  UInt_t tpc = AliAnalysisTaskEmcal::kTPC;

  // Tender Supplies
  if (useTender)
    {
    gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEMCALTender.C");
    AliAnalysisTaskSE *tender = AddTaskEMCALTender ( "", kFALSE, kTRUE, kTRUE, kTRUE, kTRUE, kTRUE, kFALSE, kTRUE, kTRUE, kTRUE, kFALSE, AliEMCALRecParam::kClusterizerNxN );
    if ( usedData != "AOD" && !kAnalysisMode.CompareTo("grid") )
      {
      AliTender *alitender = dynamic_cast<AliTender*>(tender);
      alitender->SetDefaultCDBStorage("local://$ALICE_ROOT/OCDB");
      }
    }

  // ################# Now: Call jet preparation macro (picotracks, hadronic corrected caloclusters, ...)

  // Jet preparation
  gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C");
  AddTaskJetPreparation(dataType);

  // ################# Now: Add jet finders+analyzers

  gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C");

  // ---> AddTaskEmcalJet arguments
  // const char *nTracks        = "Tracks",
  // const char *nClusters      = "CaloClusters",
  // const Int_t algo           = 1,
  // const Double_t radius      = 0.4,
  // const Int_t type           = 0,
  // const Double_t minTrPt     = 0.15,
  // const Double_t minClPt     = 0.30,
  // const Double_t ghostArea   = 0.01  ,
  // const char *tag            = "Jet"
  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet("PicoTracks", "CaloClustersCorr", kANTIKT, 0.4, kCHARGEDJETS, 0.150, 0.300);

//  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet( AliEmcalJetTask::kAKT, "Tracks", "CaloClusters", 0.15, 0.30, 0.005, 0.3, "Jet03");
//  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet( AliEmcalJetTask::kAKT, "Tracks", "CaloClusters", 0.15, 0.30, 0.005, 0.4, "Jet04");
//  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet( AliEmcalJetTask::kAKT, "Tracks", "CaloClusters", 0.15, 0.30, 0.005, 0.5, "Jet05");
//  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet( AliEmcalJetTask::kAKT, "Tracks", "CaloClusters", 0.15, 0.30, 0.005, 0.6, "Jet06");

  // Here you can put in your AddTaskMacro for your task
  gROOT->LoadMacro("AddTaskEmcalJetCDF.C");

  // ---> AddTaskEmcalJetCDF arguments
  // const char* ntracks            = "Tracks",
  // const char* nclusters          = "CaloClusters",
  // const char* njets              = "Jets",
  // const char* nrho               = "Rho",
  // Double_t    jetradius          = 0.2,
  // Double_t    jetptcut           = 1,
  // Double_t    jetareacut         = 0.557,
  // UInt_t      type               = AliAnalysisTaskEmcal::kEMCAL,
  // Int_t       leadhadtype        = 0,
  // const char *taskname           = "TaskEmcalJetCDF"

  cout << "jetFinder Task Name : " << jetFinderTask->GetName() << endl;

//   AliAnalysisTaskEmcalJetCDF* anaTask = AddTaskEmcalJetCDF("Tracks", "CaloClusters", "Jets" , "Rho", 0.4, 1, 0.557, AliAnalysisTaskEmcal::kEMCAL, 0, "TaskEmcalJetCDF" );
  AliAnalysisTaskEmcalJetCDF* anaTask = AddTaskEmcalJetCDF("PicoTracks", "CaloClustersCorr", jetFinderTask->GetName(), "");

  // Set the physics selection for all given tasks
  TObjArray *toptasks = mgr->GetTasks();
  for (Int_t i=0; i<toptasks->GetEntries(); ++i)
    {
    AliAnalysisTaskSE *task = dynamic_cast<AliAnalysisTaskSE*>(toptasks->At(i));
    if ( !task ) { continue; }
    if ( task->InheritsFrom("AliPhysicsSelectionTask") ) { continue; }
    ::Info("setPSel", "Set physics selection for %s (%s)", task->GetName(), task->ClassName());
    task->SelectCollisionCandidates(pSel);
    }


//==========================================
// ######       START ANALYSIS       #######
//==========================================

if ( mgr->InitAnalysis() )
  {
  cout << "##-->> Initialising Analysis :: Status :" << endl;
  mgr->PrintStatus();

  // grmpf, aliroot error handler overwrites root
  AliLog::SetGlobalLogLevel(AliLog::kError);

  gErrorIgnoreLevel = kErrorIgnoreLevel;
  if ( gErrorIgnoreLevel > 3000 ) { AliLog::SetGlobalLogLevel(AliLog::kFatal); }

  if ( ( kUseSysInfo > 0 ) && (!kPluginMode.CompareTo("test")) )
    { TFile * fM = TFile::Open("manager_local.root","RECREATE"); mgr->Write(); fM->Close(); }

  // task profiling
  if ( ( kUseSysInfo > 0 ) || (!kPluginMode.CompareTo("test")) )
    { for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask(i); } }

  if (!mgr->GetGridHandler()) { ::Error("AnalysisCDFjets::StartAnalysis", "Grid plugin not initialized"); return; }

  mgr->StartAnalysis(kAnalysisMode.Data(),nentries,firstentry);
  }
  // END of mgr->InitAnalysis()


}

//______________________________________________________________________________
void LoadLibs()
{
  // Load common libraries (better too many than too few)
  gSystem->Load("libTree");
  gSystem->Load("libVMC");
  gSystem->Load("libGeom");
  gSystem->Load("libGui");
  gSystem->Load("libXMLParser");
  gSystem->Load("libMinuit");
  gSystem->Load("libMinuit2");
  gSystem->Load("libProof");
  gSystem->Load("libPhysics");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libOADB");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libCDB");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libSTEER");
  gSystem->Load("libEVGEN");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libCORRFW");
  gSystem->Load("libTOFbase");
  //gSystem->Load("libTOFrec");
  gSystem->Load("libRAWDatabase.so");
  gSystem->Load("libRAWDatarec.so");
  gSystem->Load("libTPCbase.so");
  gSystem->Load("libTPCrec.so");
  gSystem->Load("libITSbase.so");
  gSystem->Load("libITSrec.so");
  gSystem->Load("libTRDbase.so");
  gSystem->Load("libTENDER.so");
  gSystem->Load("libSTAT.so");
  gSystem->Load("libTRDrec.so");
  gSystem->Load("libHMPIDbase.so");
  gSystem->Load("libPWGPP.so");
  gSystem->Load("libPWGHFbase");
  gSystem->Load("libPWGDQdielectron");
  gSystem->Load("libPWGHFhfe");
  gSystem->Load("libEMCALUtils");
  gSystem->Load("libPHOSUtils");
  gSystem->Load("libPWGCaloTrackCorrBase");
  gSystem->Load("libEMCALraw");
  gSystem->Load("libEMCALbase");
  gSystem->Load("libEMCALrec");
  gSystem->Load("libTRDbase");
  gSystem->Load("libVZERObase");
  gSystem->Load("libVZEROrec");
  gSystem->Load("libTENDER");
  gSystem->Load("libTENDERSupplies");
  gSystem->Load("libPWGTools");
  gSystem->Load("libPWGEMCAL");
  gSystem->Load("libPWGGAEMCALTasks");
  gSystem->Load("libPWGTools");
  gSystem->Load("libPWGCFCorrelationsBase");
  gSystem->Load("libPWGCFCorrelationsDPhi");

  gSystem->Load("libCGAL");
  gSystem->Load("libfastjet");
  gSystem->Load("libsiscone");
  gSystem->Load("libsiscone_spherical");
  gSystem->Load("libSISConePlugin");

  gSystem->Load("libJETAN");
  gSystem->Load("libFASTJETAN");
  gSystem->Load("libPWGJEEMCALJetTasks");


  ::Info("EmcalJetCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS");

  // include paths
  gSystem->AddIncludePath("-Wno-deprecated");
  gSystem->AddIncludePath("-I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/JETAN -I$ALICE_ROOT/EMCAL -I$.");

}

//______________________________________________________________________________
AliAnalysisAlien* CreateAlienHandler (const char* plugin_mode="test")
{
  Int_t       kTestFiles               =  1;    // Number of test files

  TString     kTrainName               = "sev_jets";           // *CHANGE ME* (no blancs or special characters)
  TString     kWorkDir                 = "emcalcdf_histos1";   // AliEn work dir; relative to AliEn $HOME
  TString     kJobTag                  = "sev_jet_analysis";   // *CHANGE ME*

  TString     kPluginExecutableCommand = "aliroot -b -q";
  Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

  TString     kPluginAPIVersion        = "V1.1x";
  TString     kPluginRootVersion       = "v5-34-08";
  TString     kPluginAliRootVersion    = "v5-05-22-AN";

  Bool_t      kPluginFastReadOption    = kFALSE;         // use xrootd tweaks
  Bool_t      kPluginOverwriteMode     = kTRUE;          // overwrite existing collections

  Int_t       kGridFilesPerJob         = 25;    // Maximum number of files per job (gives size of AOD)
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

  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if (!plugin) { printf("!!! -->> alien handler could not be created <<-- !!!"); return;}

  //********************   PLUGIN OPTIONS   ************************
  // exit from aliensh after submmiting job
  plugin->SetDropToShell(kTRUE);

  // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
  plugin->SetRunMode(plugin_mode);

  plugin->SetCheckCopy(kFALSE);

  // Set the number of test files; set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
  plugin->SetNtestFiles(kTestFiles);

  if (kPluginUseProductionMode) { plugin->SetProductionMode(); }

  plugin->SetExecutableCommand( kPluginExecutableCommand.Data() );
  plugin->SetJobTag(kJobTag);

  // Set versions of used packages
  plugin->SetAPIVersion(kPluginAPIVersion);
  plugin->SetROOTVersion(kPluginRootVersion);
  plugin->SetAliROOTVersion(kPluginAliRootVersion);

  // Add external packages
  plugin->AddExternalPackage("boost::v1_43_0");
  plugin->AddExternalPackage("cgal::v3.6");
  plugin->AddExternalPackage("fastjet::v2.4.2");

  // Use fastread option
  plugin->SetFastReadOption(kPluginFastReadOption);

  // UseOverwrite mode
  plugin->SetOverwriteMode(kPluginOverwriteMode);

  // Optionally set a name for the generated analysis macro (default MyAnalysis.C)
  plugin->SetAnalysisMacro(Form("%s.C", kTrainName.Data()));

  // Optionally modify the name of the generated JDL (default analysis.jdl)
  plugin->SetJDLName(Form("%s.jdl", kTrainName.Data()));

  // Optionally modify the executable name (default analysis.sh)
  plugin->SetExecutable(Form("%s.sh", kTrainName.Data()));

  // Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
  plugin->SetSplitMaxInputFileNumber(kGridFilesPerJob);

  // Optionally set maximum number of files merged in a chunk (default 100, put 0 to ignore)
  plugin->SetMaxMergeFiles(kGridMaxMergeFiles);

  // Maximum initial consecutive subjobs accepted to fail.
  plugin->SetMaxInitFailed(kMaxInitFailed);

  // Optionally resubmit threshold.
  // plugin->SetMasterResubmitThreshold(90);

  // Optionally set time to live
  plugin->SetTTL(kTTL);

  // Optionally set input format (default xml-single)
  plugin->SetInputFormat("xml-single");

  // Optionally modify job price (default 1)
  plugin->SetPrice(1);

  plugin->SetNrunsPerMaster (kGridRunsPerMaster);
  plugin->SetMergeViaJDL    (kPluginMergeViaJDL);

  // write the output to subdirs named after run number
  plugin->SetOutputToRunNo(kPluginOutputToRunNumber);

  if ( kGridDataSet.Length() > 0 ) { kWorkDir += Form("/%s%s",kGridDataSet.Data(),kGridExtraAliendirLevel.Data()); }
  plugin->SetGridWorkingDir(kWorkDir.Data());

  // Declare alien output directory. Relative to working directory.
  if ( !kGridOutdir.Length() ) { kGridOutdir = Form("out_%s",kTrainName.Data()); }
  plugin->SetGridOutputDir(kGridOutdir);

  plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output

  //-----------------------------------------------
  //           PROOF SETTINGS
  //-----------------------------------------------

  TString kAlirootMode="ALIROOT";      // STEERBase,ESD,AOD,ANALYSIS,ANALYSISalice (default aliroot mode)
  //  alirootMode="ALIROOT";   // $ALICE_ROOT/macros/loadlibs.C
  //  alirootMode="REC";       // $ALICE_ROOT/macros/loadlibsrec.C
  //  alirootMode="SIM";       // $ALICE_ROOT/macros/loadlibssim.C
  //  alirootMode="TRAIN";     // $ALICE_ROOT/macros/loadlibstrain.C (not working yet)
  //  alirootMode="CUSTOM";    // nothing is loaded, but aliroot variables are set (not working yet)

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
  Int_t result = -999 ; TString mod(module);

  if (!mod.Length()) { ::Error("AnalysisCDFjets::LoadLibrary", "Empty module name"); return kFALSE; }

  if (mod.EndsWith(".so")) { mod.Remove(mod.Index(".so")); }
  result = gSystem->Load(mod);

  if (result < 0) { ::Error("AnalysisCDFjets::LoadLibrary", "Could not load library %s", module); return kFALSE; }
  anaLibs      += Form("%s.so ",mod.Data());
  anaLibsExtra += Form("%s.so ",mod.Data());

  return kTRUE;
}


//______________________________________________________________________________
Bool_t LoadPars ( AliAnalysisAlien* plugin, const char *module, Bool_t rec = kFALSE )
{
  // Load a module library in a given mode. Reports success.
  Int_t result = -999; Bool_t success = kTRUE;

  TString mod(module);
  if (!mod.Length()) { ::Error("AnalysisCDFjets::LoadLibrary", "Empty module name"); return kFALSE; }

  if ( !plugin->SetupPar(module) ) { return kFALSE; }

  mod += ".par"; plugin->EnablePackage(mod.Data()); anaPars += mod.Data();

  //   mod.ReplaceAll(".par",".so"); mod.Prepend("lib"); result = gSystem->Load(mod);
  //   if ( result < 0 ) { cout << "Load PARs : Load library failed : " << mod.Data() << endl; return kFALSE ; }

  return success;
  }

//______________________________________________________________________________
Bool_t LoadSource(const char *source, Bool_t rec=kFALSE)
{
  // Load a module library in a given mode. Reports success.
  Int_t result = -1;
  TString ssource(source);
  TString basename = gSystem->BaseName(ssource.Data());

  if (!ssource.Length()) {
    ::Error("AnalysisPWG4CDF::LoadSource", "Empty task name");
    return kFALSE;
  }
  // we have a source code so compile it
  if (ssource.EndsWith(".cxx")) {
    // need to copy it here other wise the path is also used on grid...
    ssource.Remove(ssource.Index(".cxx"));
    basename.Remove(basename.Index(".cxx"));
    Printf("LoadSources:: Copying...  path %s{cxx,h}",ssource.Data());
    gSystem->Exec(Form("cp %s.cxx . ",ssource.Data()));
    gSystem->Exec(Form("cp %s.h . ",ssource.Data()));
    // Path to code only needed for local compilation, in grid and proof mode the task headers are uploaded
    //     path.Remove(path.Index(gSystem->BaseName(path.Data())));
    // Printf("LoadSources:: Including path %s",path.Data());
    //  if(path.Length()>0)gROOT->ProcessLine(Form(".include %s",path.Data()));
    Printf("LoadSources:: Loading...  path %s",basename.Data());

    result = gROOT->LoadMacro(Form("%s.cxx++g",basename.Data()));
    if (rec){
      // what we want to compile
      anaSources += Form("%s.cxx ",basename.Data());
      // what we need as input...
      anaLibs += Form("%s.cxx %s.h ",basename.Data(),basename.Data());
    }
  }


  if (result < 0) { ::Error("AnalysisCDFjets::LoadSources", "Could not load source %s", source); return kFALSE; }

  return kTRUE;
}

