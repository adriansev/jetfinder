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
#include "AliAnalysisTaskTagCreator.h"

#include "AliAnalysisTaskJetsReader.h"
#include "AliAnalysisTaskJetsFinder.h"

#endif


// #include "AlienHandler.C"
// #include "InputData.C"
// #include "AddTaskJetsReader.C"
// #include "AddTaskJetsFinder.C"


//LHC11a(Pass3_wSDD)    LHC period LHC11a (2.76TeV) - Pass3
// "pp276_lhc11a"

//pp, Pythia, 0.5T, 2760 GeV, Min.Bias, LHC11a anchor runs, ID #263
// "pp276_lhc11b10a"

// LHC10c(2)       LHC period LHC10c - Pass2 (7TeV)
// "pp7_lhc10c"

// pp, Pythia, 0.5T, 7 TeV, LHC10e anchor runs (10% statistics), ID #233
// "pp7_lhc10e"


TString   kInputData =
//                       "pp_lhc11a";
                       "pp_lhc10e";

TString     kWorkDir           = "CDF3";        // AliEn work dir; relative to AliEn $HOME

// steering vars for jet analysis
Int_t   UA1     = 1;
Int_t   CDF     = 0;
Int_t   SISCONE = 0;
Int_t   DA      = 0;
Int_t   FASTJET = 0;
Int_t   FASTKT  = 0;

Int_t   iCDFhistos1     = 1;
Int_t   iCDFhistos2     = 0;
Int_t   iCDFhistos3     = 0;

Int_t   debug = 100;


const Long64_t nentries = 999999999; // for local and proof mode, ignored in grid mode. Set to 1234567890 for all events.
const Long64_t firstentry = 0; // for local and proof mode, ignored in grid mode

// FILES USED IN MACRO
TString     kCommonOutputFileName   = "PWGJE_JetsOut.root";

TString     kDeltaAODJetName        = "AliAOD.Jets.root";
TString     kDeltaAODJetNameInput   = "AliAOD.Jets.root";
TString     kCDFresults             = "CDFoutput.root";

Bool_t      kAODjets_analysis       = kFALSE; // enable for analysis of already written jets

// == grid plugin files rules
TString     kGridExtraFiles          = "";     // files that will be added to the input list in the JDL...
TString     kGridMergeExclude        = "AliAOD.root AliAOD.Jets.root"; // Files that should not be merged
TString     kGridOutputStorages      = "disk=2"; // Make replicas on the storages


// Usage of par files ONLY in grid mode and ONLY if the code is not available
// in the deployed AliRoot versions. Par file search path: local dir, if not there $ALICE_ROOT.
// To refresh par files, remove the ones in the workdir, then do "make <target.par>" in
// AliRoot.

Bool_t      kUseCPAR            = kFALSE; // use par files for common libs
Bool_t      kUsePAR             = kTRUE; // use par files for extra libs
Bool_t      kUsePARJETAN        = kTRUE; // Use JETAN.par and PWG4Tasks.par

Bool_t      kFillAOD            = kTRUE;  // switch of AOD filling for on the fly analysis
Bool_t      kFilterAOD          = kFALSE;
Int_t       kSaveAOD            = 2;      // Bit switch 1 = Full AOD; 2 = Jet AOD;

//== general input and output variables

Int_t       iAODanalysis          = 1;   // Analysis on input AOD's
Int_t       iAODhandler           = 1;   // Analysis produces an AOD or dAOD's
Int_t       iCentralitySelection  = 1;   // Use the centrality
Int_t       iESDfilter            = 0;   // ESD to AOD filter (barrel + muon tracks)
Int_t       iPhysicsSelection     = 0;   // ESD to AOD filter (barrel + muon tracks)
UInt_t      iPhysicsSelectionFlag = 0;   // set by pyshics selection and passed to the task, kMB, kUserDefined etc
Bool_t      useTender             = kFALSE; // use tender wagon
Bool_t      kUseKinefilter        = kFALSE; // use Kinematics filter
Bool_t      kUseMuonfilter        = kFALSE; // use Kinematics filter



//== general process variables

// ### Other flags to steer the analysis
//==============================================================================
Bool_t      kSkipTerminate      = kTRUE; // Do not call Teminate
Bool_t      kUseDebug           = kFALSE; // activate debugging
Int_t       kErrorIgnoreLevel   = -1; // takes the errror print level from .rootrc

Int_t       kUseSysInfo         = 0;           // activate debugging

Bool_t      kUseMC              = kFALSE;      // use MC info
Bool_t      kIsMC               = kFALSE;      // is MC info, if false it overwrites Use(AOD)MC
Bool_t      kUseAODMC           = kFALSE;      // use MC infA
Bool_t      kUseESDTags         = kFALSE;      // use ESD tags for selection
Bool_t      kUseTR              = kTRUE;       // use track references
Bool_t      kUseAODTags         = kFALSE;      // use AOD tags
Bool_t      kIsPbPb             = kFALSE;      // Pb+Pb


// ### Analysis modules to be included. Some may not be yet fully implemented.
//==============================================================================
Int_t       iJETAN                                 = 2;      // Jet analysis (PWG4) // 1 write standard 2 write non-standard jets, 3 wrtie both
Int_t       iJETANLib                              = 1;
Int_t       iJETSUBTRACT                           = 0;      // Jet background subtration
TList       kJetListSpectrum;                                // list of jets contains TObjString of possible jet finder names

TString     kDefaultJetBranch                      = "";      // is currently set when filled (iJETAN or clusters) or from config macro
TString     kDefaultJetBackgroundBranch            = "";      // is currently set when filled (jet clsuters
TString     kDefaultJetBackgroundBranchCut1        = "";      // is currently set when filled (jet clsuters
TString     kDefaultJetBackgroundBranch_extra      = "";      // is currently set when filled (jet clsuters)
TString     kJetSubtractBranches                   = "";      // is currently set when filled (jet clsuters
TString     kJetSubtractBranchesCut1               = "";      // is currently set when filled (jet clsuters
TString     kJetSubtractBranches_extra             = "";      // is currently set when filled (jet clsuters
TString     kDefaultJetBranchMC                    = "";      // is currently set when filled (iJETAN or clusters) or from config macro
TString     kDefaultJetBackgroundBranchMC          = "";      // is currently set when filled (jet clsuters
TString     kDefaultJetBranchMC2                   = "";      // is currently set when filled (iJETAN or clusters) or from config macro
TString     kDefaultJetBackgroundBranchMC2         = "";      // is currently set when filled (jet clsuters
TString     kJetSubtractMask1                      = "B0";
TString     kJetSubtractMask2                      = "B%d";

Int_t       iJETANdevLib       = 1;
Int_t       iPWG4JetTasks      = 1;      // all jet tasks flag for lib laoding
Int_t       kHighPtFilterMask  = 32;     // change depending on the used AOD Filter

Int_t       iEMCUtilLibs       = 0;      // Flag to satisfy dependence on EMC utils

//==============================================================================

Bool_t jetandev = kFALSE;
Bool_t jetan    = kTRUE ;


// Temporaries.
TString anaPars = "";
TString anaLibs = "";
TString anaLibsExtra = "";
TString anaSources = "";

// Function signatures
class AliAnalysisGrid;
class AliAnalysisAlien;
class AliAnalysisManager;


//______________________________________________________________________________
void AnalysisCDFjets ( const char* plugin_mode="test" , const char* analysis_mode="grid", const char* dataset="" )
{
  TString   kPluginMode   = plugin_mode   ; kPluginMode.ToLower();    // test, offline, submit, terminate, full
  TString   kAnalysisMode = analysis_mode ; kAnalysisMode.ToLower();  // local, grid, proof
  TString   kDataset      = dataset ;

  if ( (!kAnalysisMode.CompareTo("local")) || (!kPluginMode.CompareTo("test")) ) { kUsePAR = kFALSE; }

  // Load common libraries and set include path
  if ( LoadCommonLibraries() ) { Printf("ROOT libs loaded"); }

  // Load analysis specific libraries
  if ( !LoadAnalysisLibraries() ) { ::Error("CDFhistos", "Could not load analysis libraries"); return; }

  //==========================================================================
  // LOAD PERSONAL SCRIPTS
  //==========================================================================
  gROOT->LoadMacro("InputData.C");
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // Set up the logging mechanism of ROOT
//  RedirectHandle_t* redir_h = new RedirectHandle_t;
//  if (!gSystem->RedirectOutput ("full_output.txt","w",redir_h) ) {Printf ("Logging initialisation OK");}

//  gSystem->ShowOutput(redir_h);

  //==========================================================================
  // SET UP AliEn handler -> main analysis engine : we will use many internal tools of AliAnalysisAlien
  //==========================================================================
  AliAnalysisAlien* plugin = CreateAlienHandler(kPluginMode.Data());

  // .txt file containing the list of files to be chained in test mode
  plugin->SetFileForTestMode("data.txt");
//   plugin->SetFriendChainName("");  // AOD friend file name

  if ( !LoadJetLibs() ) { ::Error("CDFhistos", "Could not load jet libraries"); return; }
  if ( !LoadJetPars(plugin) ) { ::Error("CDFhistos", "Could not load jet PARs"); return; }

  //______________________________________________________________________________
  // DATA INPUT
  InputData ( plugin, kInputData );

  //______________________________________________________________________________
  // PROOF SETTINGS
  //______________________________________________________________________________

  TString kAAF        =
//                         "asevcenc@alice-caf.cern.ch" ;
                           "asevcenc@skaf.saske.sk";

  Int_t   kProofReset = 0  ;
  Int_t   kWorkers    = 20 ;
  Int_t   kCores      = 8  ;


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
//   plugin->SetRootVersionForProof ( kPluginRootVersion ) ;
  plugin->SetAliRootMode ( kAlirootMode ) ;

// plugin->SetProofParameter(const char *pname, const char *value);



//   cout << "Include Path : \n" << gSystem->GetIncludePath() <<"\n"<< endl;
//   cout << "List libs : \n" << gSystem->GetLibraries() <<"\n"<< endl;
//   gSystem->ListLibraries();

  // Make the analysis manager and connect event handlers
//   AliAnalysisManager *mgr  = new AliAnalysisManager("CDFhistos", "CDF histos analysis");
  AliAnalysisManager *mgr  = plugin->CreateAnalysisManager ("CDFhistosMGR");

  if ( kCommonOutputFileName.Length() > 0 ) { mgr->SetCommonFileName( kCommonOutputFileName.Data() ); }

  mgr->SetNSysInfo(0);
  if ( kUseSysInfo ) { mgr->SetNSysInfo(kUseSysInfo); }

  if (!kPluginMode.CompareTo("test")) { mgr->SetNSysInfo(1); }

  mgr->SetSkipTerminate(kSkipTerminate);

  // Create input handler (input container created automatically)

  // AOD input handler
  AliAODInputHandler *aodH_in = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodH_in);
  aodH_in->SetCheckStatistics(kTRUE);
  if ( kDeltaAODJetNameInput.Length() && kAODjets_analysis )
    {
    Printf("Adding Friend %s",kDeltaAODJetNameInput.Data());
    aodH_in->AddFriend(kDeltaAODJetNameInput.Data());
    }

  // AOD output handler
  AliAODHandler* aodH_out   = new AliAODHandler();
  aodH_out->SetOutputFileName("AliAOD.root");
  aodH_out->SetFillAODforRun ( kFillAOD );

  mgr->SetOutputEventHandler(aodH_out);
  AliAnalysisDataContainer * cout_aod = mgr->GetCommonOutputContainer();
  cout_aod->SetSpecialOutput();


  // Debugging if needed
  if (kUseDebug) { mgr->SetDebugLevel(3); }

  if ( kUseSysInfo > 0 )
    {
    mgr->RegisterExtraFile("syswatch.root");
    if(kGridMergeExclude.Length())kGridMergeExclude += " ";
    kGridMergeExclude += "syswatch.root";
    }
  else
    { AliLog::SetGlobalLogLevel(AliLog::kError); }


  //==========================================================================
  // Load the tasks configuration macros for all wagons. These files are supposed now to be
  // in the current workdir, but in AliEn they will be in the file catalog,
  // mapped from AliRoot and specified in the jdl input list.

  // AOD tags
  if ( kUseAODTags )
    {
    AliAnalysisTaskTagCreator* tagTask = new AliAnalysisTaskTagCreator("AOD Tag Creator");
    mgr->AddTask(tagTask);
    AliAnalysisDataContainer *coutTags = mgr->CreateContainer("cTag",  TTree::Class(), AliAnalysisManager::kOutputContainer, "AOD.tag.root");
    mgr->ConnectInput (tagTask, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(tagTask, 1, coutTags);
    }


  // Jet analysis
/*
if (jetandev)
{
gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/AddTaskJetsReader.C");
gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/AddTaskJetsFinder.C");
gROOT->LoadMacro("AddTaskCDFhistos.C");

//     gROOT->LoadMacro("AddTaskJetsReader.C");
//     gROOT->LoadMacro("AddTaskJetsFinder.C");

//   INIT JET CONTAINER AND FINDER
AliAnalysisTaskJetsReader* taskjetsReader = 0;
AliAnalysisTaskJetsFinder* taskjetsFinder = 0;
AliAnalysisDataContainer*  cont_jet1      = 0;

cont_jet1 = AddJetExchangeContainer("jets_1");
taskjetsReader = AddTaskJetsReader(cont_jet1,"AOD",kHighPtFilterMask,0.15);

if (!taskjetsReader) {::Fatal("AnalysisCDFJets", "no taskjetsReader"); return; }

taskjetsFinder = AddTaskJetsFinder(cont_jet1,"FASTJET",0.2,0); // FASTJET
if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }




// void        SetNonStdBranch(const char *c){fNonStdBranch = c;}
// void        SetNonStdOutputFile(const char *c){fNonStdFile = c;}

taskjetsFinder = AddTaskJetsFinder(cont_jet1,"FASTJET",0.4,0); // FASTJET
if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }



taskjetsFinder = AddTaskJetsFinder(cont_jet1,"FASTJET",0.7,0); // FASTJET
if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }



if (!taskjetsFinder) {::Fatal("AnalysisCDFJets", "no taskjetsFinder"); return; }

// taskjetsReader->GetJetReader()->GetReaderHeader()->SetIsHighMult(kIsPbPb)

}
*/

Int_t use_cdf = 0;

if (jetan)
{
gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/AddTaskJets.C");
gROOT->LoadMacro("AddTaskCDFhistos.C");

AliAnalysisTaskJets *taskjets = 0;

////////////////////
////   TASK 1   ////
////////////////////
// AliAnalysisTaskJets *AddTaskJets(Char_t *jr, Char_t *jf, Float_t radius,UInt_t filterMask,float ptTrackMin,int iBack)
taskjets = AddTaskJets("AOD","FASTJET", 0.2, kHighPtFilterMask, 0., 0. ); // no background subtraction

if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }

TString kNonStdBranchInput = taskjets->GetNonStdBranch();
if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }


if (use_cdf)
  {
  // AliAnalysisTaskCDFhistos *AddTaskCDFhistos( Char_t *connect_file="", Char_t *connect_branch="cdfh_br", Char_t *file_out="CDFh", Char_t *branch_out="cdfh_br", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )
  AliAnalysisTaskCDFhistos *taskCDF = AddTaskCDFhistos( "", kNonStdBranchInput.Data(), kCDFresults.Data() ,kNonStdBranchInput.Data(), 0, 0.);
  // if ( kDeltaAODJetName.Length() > 0 ) { taskCDF->SetNonStdFile(kDeltaAODJetName.Data()); }
  if (!taskCDF) { ::Fatal("CDFhistos", "No CDFhistos pointer"); }

  taskCDF->SetDebugLevel(debug);

  cout << "nonstdfile : "   << taskCDF->GetNonStdFile() << endl;
  cout << "nonstdbranch : " << taskCDF->GetNonStdBranch() << endl;

  }



////////////////////
////   TASK 2   ////
////////////////////
// AliAnalysisTaskJets *AddTaskJets(Char_t *jr, Char_t *jf, Float_t radius,UInt_t filterMask,float ptTrackMin,int iBack)
taskjets = AddTaskJets("AOD","FASTJET", 0.4, kHighPtFilterMask, 0., 0. ); // no background subtraction

if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }

TString kNonStdBranchInput = taskjets->GetNonStdBranch();
if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }


if (use_cdf)
  {
  // AliAnalysisTaskCDFhistos *AddTaskCDFhistos( Char_t *connect_file="", Char_t *connect_branch="cdfh_br", Char_t *file_out="CDFh", Char_t *branch_out="cdfh_br", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )
  AliAnalysisTaskCDFhistos *taskCDF = AddTaskCDFhistos( "", kNonStdBranchInput.Data(), kCDFresults.Data() ,kNonStdBranchInput.Data(), 0, 0.);
  // if ( kDeltaAODJetName.Length() > 0 ) { taskCDF->SetNonStdFile(kDeltaAODJetName.Data()); }
  if (!taskCDF) { ::Fatal("CDFhistos", "No CDFhistos pointer"); }

  taskCDF->SetDebugLevel(debug);

  cout << "nonstdfile : "   << taskCDF->GetNonStdFile() << endl;
  cout << "nonstdbranch : " << taskCDF->GetNonStdBranch() << endl;

  }


////////////////////
////   TASK 3   ////
////////////////////
// AliAnalysisTaskJets *AddTaskJets(Char_t *jr, Char_t *jf, Float_t radius,UInt_t filterMask,float ptTrackMin,int iBack)
taskjets = AddTaskJets("AOD","FASTJET", 0.7, kHighPtFilterMask, 0., 0. ); // no background subtraction

if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }

TString kNonStdBranchInput = taskjets->GetNonStdBranch();
if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }


if (use_cdf)
  {
  // AliAnalysisTaskCDFhistos *AddTaskCDFhistos( Char_t *connect_file="", Char_t *connect_branch="cdfh_br", Char_t *file_out="CDFh", Char_t *branch_out="cdfh_br", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )
  AliAnalysisTaskCDFhistos *taskCDF = AddTaskCDFhistos( "", kNonStdBranchInput.Data(), kCDFresults.Data() ,kNonStdBranchInput.Data(), 0, 0.);
  // if ( kDeltaAODJetName.Length() > 0 ) { taskCDF->SetNonStdFile(kDeltaAODJetName.Data()); }
  if (!taskCDF) { ::Fatal("CDFhistos", "No CDFhistos pointer"); }

  taskCDF->SetDebugLevel(debug);

  cout << "nonstdfile : "   << taskCDF->GetNonStdFile() << endl;
  cout << "nonstdbranch : " << taskCDF->GetNonStdBranch() << endl;

  }




}

  if (kCDFresults.Length() > 0 && use_cdf )   { mgr->RegisterExtraFile(kCDFresults.Data()); }
  if (kDeltaAODJetName.Length() > 0 )         { mgr->RegisterExtraFile(kDeltaAODJetName.Data()); }
  if (kCommonOutputFileName.Length() > 0 )    { mgr->RegisterExtraFile(kCommonOutputFileName.Data()); }

// #########  Add grid handler to mgr     ################
  mgr->SetGridHandler(plugin);

  ////////////////////////////
  //   INPUT/OUTPUT
  ///////////////////////////
  plugin->SetMergeExcludes  (kGridMergeExclude);

  // Optionally define the files to be archived.
//   plugin->SetOutputArchive("log_archive.zip:stdout,stderr@ALICE::NIHAM::File root_archive.zip:EventStat_temp.root,PWG4_JetTasksOutput.root@ALICE::NIHAM::File");

  // get all loaded libs
  TString libs_loaded = gSystem->GetLibraries("","D");
  // split string in tokens (libs)
  TObjArray* libs_list = libs_loaded.Tokenize(" "); libs_list->Compress();
  // this is the reference libs from where the ROOTSYS path will be extracted
  TString ref_lib = "libMathCore.so" ;
  // select the reference lib with full path from the list of libs
  TString lib = "";
  for ( Int_t i = 0 ; i < libs_list->Capacity() ; i++ )
  {
  TObjString * obj = libs_list->At(i);
  if (obj) { lib =  obj->GetString(); if ( lib.Contains(ref_lib.Data()) ) { break; } }
  }

  // extract the ROOTSYS/lib path and apend * for making it a TRegexp
  TString rootsys_lib = lib.ReplaceAll(ref_lib.Data(),""); rootsys_lib += "*" ;
  // extract only ROOT libs from the ALL loaded libs
  TString root_libs_D = gSystem->GetLibraries(rootsys_lib.Data(),"D");

  // clean up the list : remove path
  rootsys_lib.Chop(); root_libs_D.ReplaceAll(rootsys_lib.Data(),"") ; root_libs_D += " " ;
//   cout << "ROOT libs list ready for adding to anaLibsExtra : \n" << root_libs_D.Data() << endl;

  anaLibs += kGridExtraFiles; anaLibsExtra.Prepend(root_libs_D);

  anaLibs      = anaLibs.Strip();
  anaLibsExtra = anaLibsExtra.Strip();

  Printf("### anaLibs : %s",anaLibs.Data());
  Printf("### anaLibsExtra : %s",anaLibsExtra.Data());

  if (anaLibs.Length())          {plugin->SetAdditionalLibs(anaLibs.Data());}
  if (anaLibsExtra.Length())     {plugin->SetAdditionalRootLibs(anaLibsExtra.Data());}


//______________________________________________________________________________
//                      OUTPUT SETTINGS

// Put default output files to archive
  TString listhists = "";
  TString listaods  = "";

  TIter next ( mgr->GetOutputs() );
  AliAnalysisDataContainer *output;

  while ( (output = (AliAnalysisDataContainer*)next()) )
    {
    const char *filename = output->GetFileName();
      if (!(strcmp(filename, "default")))
        {
        if (!mgr->GetOutputEventHandler()) continue;
        filename = mgr->GetOutputEventHandler()->GetOutputFileName();
        if (listaods.Length()) { listaods += " ";}
        listaods += filename;
        }
      else
        {
        if(!listhists.Contains(filename))
          {
          if (listhists.Length()) {listhists += " ";}
          listhists += filename;
          }
        }
    }

  if (mgr->GetExtraFiles().Length())
    {
    if (listaods.Length()) {listaods += " ";}
    listaods += mgr->GetExtraFiles();
    }

  // if we do not fill the aod we do not need to store it
  //   kGridMergeExclude = listaods;

  if ( kSaveAOD >= 0 )
    {
    TString outputFiles =  "";
    outputFiles += mgr->GetExtraFiles();
    if (listhists.Length()) { outputFiles += " "; }
    outputFiles += listhists;

    plugin->SetDefaultOutputs(kFALSE);
    Printf("%s:%d Starting with the files %s",(char*)__FILE__,__LINE__,outputFiles.Data());

    // remove : no harm done when we try to remove something that is not there :)
    if ( !(kSaveAOD&(1<<0)) )
      {
      outputFiles.ReplaceAll("AliAOD.root ","");
      listaods.ReplaceAll("AliAOD.root ","");
      }

    if ( !(kSaveAOD&(1<<1)) )
      {
      if ( kDeltaAODJetName.Length() ) { outputFiles.ReplaceAll(kDeltaAODJetName.Data(),""); }
      if ( kDeltaAODJetName.Length() ) { listaods.ReplaceAll(kDeltaAODJetName.Data(),""); }
      }

     Printf("%s:%d Saving the files %s",(char*)__FILE__,__LINE__,outputFiles.Data());
     plugin->SetOutputFiles(outputFiles.Data());
   }

  TString outputArchive;
  outputArchive = Form("log_archive.zip:std*@%s","disk=1");

  listaods.ReplaceAll(" ", ",");
  listhists.ReplaceAll(" ", ",");

  if ( listhists.Length() )
    {
    outputArchive += " ";
    outputArchive += "root_archive.zip:";
    outputArchive += listhists;

    if ( listaods.Length() ) { outputArchive += ","; outputArchive += listaods; }

    outputArchive += Form("@%s", kGridOutputStorages.Data());
    }
  else
    {
    if (listaods.Length())
      {
      // we have only aod'ish output
      outputArchive += " ";
      outputArchive += "root_archive.zip:";
      outputArchive += listaods;
      outputArchive += Form("@%s", kGridOutputStorages.Data());
      }
    else
      { // no other outputs than std..
      ::Fatal("AnalysisTrainPWG4Jets", "No other outputs than std !");
      }
    }

  plugin->SetDefaultOutputs(kFALSE);
  plugin->SetOutputArchive ( outputArchive ); cout << "PLUGIN :: Output archive : " << outputArchive.Data() << endl;




  //==========================================
  // ######       START ANALYSIS       #######
  //==========================================

if ( mgr->InitAnalysis() )
  {
  cout << "##-->> Initialising Analysis :: Status :" << endl;
  mgr->PrintStatus();

  if ( ( kUseSysInfo > 0 ) && (!kPluginMode.CompareTo("test")) )
    { TFile * fM = TFile::Open("manager_local.root","RECREATE"); mgr->Write(); fM->Close(); }

  // grmpf, aliroot error handler overwrites root
  AliLog::SetGlobalLogLevel(AliLog::kError);

  gErrorIgnoreLevel = kErrorIgnoreLevel;
  if ( gErrorIgnoreLevel > 3000 ) { AliLog::SetGlobalLogLevel(AliLog::kFatal); }

  // task profiling
  if ( ( kUseSysInfo > 0 ) || (!kPluginMode.CompareTo("test")) )
    { for ( int i = 0; i < mgr->GetTopTasks()->GetEntries(); i++ ) { mgr->ProfileTask(i); } }

  if (!mgr->GetGridHandler()) { ::Error("AnalysisPWG4CDF::StartAnalysis", "Grid plugin not initialized"); return; }

//   mgr->StartAnalysis(kAnalysisMode.Data(),nentries,firstentry);
  mgr->StartAnalysis(kAnalysisMode.Data());

  }
  // END of mgr->InitAnalysis()

}

//______________________________________________________________________________
Bool_t LoadCommonLibraries()
  {
//   gROOT->ProcessLine("./include $CGAL_DIR/include $FASTJET/include $ALICE_ROOT/include");

  gSystem->SetIncludePath("-I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT/include");
  gROOT->ProcessLine(".include $ALICE_ROOT/include");

  ::Info("AnalysisCDF::LoadAnalysisLibs", "Include path for Aclic compilation:\n%s", gSystem->GetIncludePath());

  Int_t result = -999;

  // ROOT libraries
  result = gSystem->Load("libTree");    if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libGeom");    if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libVMC");     if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libPhysics"); if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libMinuit");  if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libMinuit2"); if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libGui");     if (result < 0) { Printf("ROOT lib load error"); exit; }
  result = gSystem->Load("libProof");   if (result < 0) { Printf("ROOT lib load error"); exit; }

  ::Info("AnalysisCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS");
  return kTRUE;
  }


//______________________________________________________________________________
Bool_t LoadAnalysisLibraries()
{
// Load common analysis libraries.
  Bool_t success = kTRUE;

if (!gSystem->Getenv("ALICE_ROOT"))
  { ::Error("AnalysisPWG4CDF", "Analysis train requires that ALICE_ROOT is set to pick up Configurations"); return kFALSE; }

  success &= LoadLibrary("libSTEERBase.so",kTRUE );
  success &= LoadLibrary("libESD.so",kTRUE);
  success &= LoadLibrary("libAOD.so",kTRUE);
  success &= LoadLibrary("libOADB.so",kTRUE);
  success &= LoadLibrary("libANALYSIS.so",kTRUE);
  success &= LoadLibrary("libANALYSISalice.so",kTRUE);
  success &= LoadLibrary("libCORRFW.so",kTRUE);
// success &= LoadLibrary("libEventMixing.so");

  success &= LoadLibrary("libCDB.so",kTRUE);
  success &= LoadLibrary("libRAWDatabase.so",kTRUE);
  success &= LoadLibrary("libSTEER.so",kTRUE );

  success &= LoadLibrary("libPWGEMCAL.so",kTRUE);
  success &= LoadLibrary("libPWGGAEMCALTasks.so",kTRUE);

  success &= LoadLibrary("libEMCALUtils.so",kTRUE);
  success &= LoadLibrary("libPHOSUtils.so",kTRUE);
  gROOT->ProcessLine(".include $ALICE_ROOT/include");


if (success)
  { ::Info("AnalysisCDF::LoadAnalysisLibs", "Load analysis libraries:    SUCCESS"); }
else
  { ::Info("AnalysisCDF::LoadCommodLibraries", "Load analysis libraries:    FAILED"); }

return success;
}

//______________________________________________________________________________
Bool_t LoadJetLibs()
{
// Load common analysis libraries.
  Bool_t success = kTRUE; Int_t result = 999;

  // LOAD FASTJET
  success &= LoadLibrary("libCGAL.so", kTRUE);
  success &= LoadLibrary("libfastjet.so", kTRUE);
  success &= LoadLibrary("libsiscone.so", kTRUE);
  success &= LoadLibrary("libSISConePlugin.so", kTRUE);

  success &= LoadLibrary("libJETAN.so", kTRUE);
  success &= LoadLibrary("libFASTJETAN.so", kTRUE);

  // LOAD FASTJET
  success &= LoadLibrary("libJETANdev.so", kTRUE);
  success &= LoadLibrary("libFASTJETANdev.so", kTRUE);
//   success &= LoadLibrary("libPWGJEEMCALJetTasks.so");


  success &= LoadLibrary("libPWGTools.so",kTRUE);
//   success &= LoadLibrary("libPWGJE.so", kTRUE);

  cout << "Libs FASTJET :" << gSystem->GetLibraries("","D") << endl;

if (success)
  { ::Info("AnalysisCDF::LoadJetsLibs", "Load Jets libraries:    SUCCESS"); }
else
  { ::Info("AnalysisCDF::LoadJetsLibs", "Load Jets libraries:    FAILED"); }

return success;
}


//______________________________________________________________________________
Bool_t LoadJetPars( AliAnalysisAlien* plugin )
{
// Load common analysis libraries.
  Bool_t success = kTRUE;

//   success &= LoadPars( plugin, "JETANdev", kTRUE);
//   success &= LoadPars( plugin, "FASTJETANdev", kTRUE);
  success &= LoadPars( plugin, "PWGJE", kTRUE);

  // LOAD FASTJET
//   success &= LoadPars( plugin, "libFASTJETANdev", kTRUE);
//   success &= LoadPars( plugin, "libPWGJEEMCALJetTasks", kTRUE);

if (success)
  { ::Info("AnalysisCDF::LoadJetsLibs", "Load Jets libraries:    SUCCESS"); }
else
  { ::Info("AnalysisCDF::LoadJetsLibs", "Load Jets libraries:    FAILED"); }

return success;
}



//______________________________________________________________________________
Bool_t LoadLibrary( const char *module, Bool_t rec = kFALSE )
  {
  Int_t result = -999 ; TString mod(module);

  if (!mod.Length()) { ::Error("AnalysisPWG4CDF::LoadLibrary", "Empty module name"); return kFALSE; }

  if (mod.EndsWith(".so")) { mod.Remove(mod.Index(".so")); }
  result = gSystem->Load(mod);

  if (result < 0) { ::Error("AnalysisPWG4CDF::LoadLibrary", "Could not load library %s", module); return kFALSE; }
  anaLibs      += Form("%s.so ",mod.Data());
  anaLibsExtra += Form("%s.so ",mod.Data());

  return kTRUE;
  }


//______________________________________________________________________________
Bool_t LoadPars( AliAnalysisAlien* plugin, const char *module, Bool_t rec = kFALSE )
{
  // Load a module library in a given mode. Reports success.
  Int_t result = -999; Bool_t success = kTRUE;

  TString mod(module);
  if (!mod.Length()) { ::Error("AnalysisPWG4CDF::LoadLibrary", "Empty module name"); return kFALSE; }

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
     // Path to code
     // only needed for local compilation, in grid and proof mode
     // the task headers are uploaded
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


if (result < 0) { ::Error("AnalysisPWG4CDF::LoadSources", "Could not load source %s", source); return kFALSE; }

return kTRUE;
}

//______________________________________________________________________________
Int_t SetupPar(char* pararchivename)
{
   if (!pararchivename || !strlen(pararchivename)) return -1;
   char processline[1024];
   if (gSystem->AccessPathName(Form("%s.par", pararchivename))) {
      if (!gSystem->AccessPathName(Form("%s/%s.par", gSystem->Getenv("ALICE_ROOT"),pararchivename))) {
         ::Info("AnalysisPWG4CDF::SetupPar", "Getting %s.par from $ALICE_ROOT", pararchivename);
         TFile::Cp(gSystem->ExpandPathName(Form("$ALICE_ROOT/%s.par", pararchivename)), Form("%s.par",pararchivename));
      } else {
         ::Error("AnalysisPWG4CDF::SetupPar", "Cannot find %s.par", pararchivename);
         return -1;
      }
   }

   gSystem->Exec(Form("tar xvzf %s.par", pararchivename));

   TString ocwd = gSystem->WorkingDirectory();
   if (!gSystem->ChangeDirectory(pararchivename)) return -1;

   // check for BUILD.sh and execute
   if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
      printf("*******************************\n");
      printf("*** Building PAR archive    ***\n");
      printf("*******************************\n");
      if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
         Error("runProcess","Cannot Build the PAR Archive! - Abort!");
         return -1;
      }
   }

        // check for SETUP.C and execute
        if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
            printf("*******************************\n");
            printf("*** Setup PAR archive       ***\n");
            printf("*******************************\n");
            gROOT->Macro("PROOF-INF/SETUP.C");
        }
        if (!gSystem->ChangeDirectory(ocwd.Data())){
          Error("Change back directory",Form("Cannot change to %s",ocwd.Data()));
          return -1;
        }
   return 0;
}

//______________________________________________________________________________
AliAnalysisAlien* CreateAlienHandler (const char* plugin_mode="test")
{
  TString     kPluginExecutableCommand = "aliroot -b -q";
  Int_t       kTestFiles               =  1;    // Number of test files

  TString     kTrainName         = "CDFanalysis";   // *CHANGE ME* (no blancs or special characters)
  TString     kJobTag            = "sev_jet_analysis"; // *CHANGE ME*

  Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

  TString     kPluginAPIVersion        = "V1.1x";
  TString     kPluginRootVersion       = "v5-34-02-1";
  TString     kPluginAliRootVersion    = "v5-04-09-AN";

  Bool_t      kPluginFastReadOption    = kFALSE;         // use xrootd tweaks
  Bool_t      kPluginOverwriteMode     = kTRUE;          // overwrite existing collections

  Int_t       kGridFilesPerJob         = 25;    // Maximum number of files per job (gives size of AOD)
  Int_t       kGridMaxMergeFiles       = 50;    // Number of files merged in a chunk grid run range
  Int_t       kMaxInitFailed           = 5 ;    // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.

  Int_t       kTTL                     = 14400 ; // Time To Live
  Int_t       kMergeTTL                = 1800 ;  // Time To Live merging

  Int_t       kGridRunsPerMaster       = 1; // Number of runs per master job
  Bool_t      kPluginMergeViaJDL       = kTRUE;          // merge via JDL

  Int_t       kPluginOutputToRunNumber = 1;              // write the output to subdirs named after run number

  TString     kGridOutdir              = "out"; // AliEn output directory. If blank will become output_<kTrainName>
  TString     kGridDataSet             = ""; // sub working directory not to confuse different run xmls
  TString     kGridExtraAliendirLevel  = ""; // sub working directory not to confuse different run xmls

  AliAnalysisAlien* plugin = new AliAnalysisAlien();
  if (!plugin) { printf("alien handler invalid");   return;}

  //********************   PLUGIN OPTIONS   ************************
  // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
  plugin->SetRunMode(plugin_mode);

  plugin->SetCheckCopy(kFALSE);

  plugin->SetDropToShell(kFALSE);

  // Set the number of test files;
  // set to kGridFilesPerJob as to evaluate the memory consumption and ttl on grid
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
  // Optionally set number of failed jobs that will trigger killing waiting sub-jobs.
  plugin->SetMaxInitFailed(kMaxInitFailed);
  // Optionally resubmit threshold.
  // plugin->SetMasterResubmitThreshold(90);
  // Optionally set time to live
  plugin->SetTTL(kTTL);
  // Optionally set time to live
  //    plugin->SetMergeTTL(kMergeTTL);
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

return plugin;
}
