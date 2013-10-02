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

// JETAN DEV
#include "AliAnalysisTaskJetsReader.h"
#include "AliAnalysisTaskJetsFinder.h"




// #include "AddTaskJetsReader.C"
// #include "AddTaskJetsFinder.C"

#endif


//===================== ANALYSIS TRAIN =========================================

// =============================================================================
// ### General Steering variables
// =============================================================================
//== general setup variables


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

TString   kDataset  = "" ;

// FILES USED IN MACRO
TString     kCommonOutputFileName   = "CDFanalysis_out.root";

TString     kDeltaAODJetName        = "AliAOD.Jets.root";
TString     kDeltaAODJetNameInput   = "AliAOD.Jets.root";
TString     kCDFresults             = "CDF_histos.root";

Bool_t      kAODjets_analysis       = kFALSE; // enable for analysis of already written jets

// == grid plugin files rules
TString     kGridExtraFiles          = "";     // LIBRARIES files that will be added to the input list in the JDL...
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
Int_t       kSaveAOD            = 2;      // Bit switch 1 = Full AOD; 2 = Jet AOD;

//==============================================================================
Bool_t      kSkipTerminate      = kTRUE; // Do not call Terminate
Bool_t      kUseDebug           = kTRUE; // activate debugging
Int_t       kErrorIgnoreLevel   = -1; // takes the errror print level from .rootrc

unsigned int kUseSysInfo        = 0 ;           // activate debugging

TList       kJetListSpectrum;            // list of jets contains TObjString of possible jet finder names
Int_t       kHighPtFilterMask  = 32;     // change depending on the used AOD Filter

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

  kDataset = dataset;

  if ( (!kAnalysisMode.CompareTo("local")) || (!kPluginMode.CompareTo("test")) ) { kUsePAR = kFALSE; }

  // Load common libraries and set include path
  if ( LoadCommonLibraries() ) { Printf("ROOT libs loaded"); }

  // Load analysis specific libraries
  if ( !LoadAnalysisLibraries() ) { ::Error("CDFhistos", "Could not load analysis libraries"); return; }

  // Load InputData macro ++++++++++++
  gROOT->LoadMacro("InputData.C");
  //++++++++++++++++++++++++++++++++++

  //==========================================================================
  // SET UP AliEn handler -> main analysis engine : we will use many internal tools of AliAnalysisAlien
  //==========================================================================
  AliAnalysisAlien* plugin = CreateAlienHandler(kPluginMode.Data());

  // .txt file containing the list of files to be chained in test mode
  plugin->SetFileForTestMode("data.txt");

  if ( !LoadJetLibs() ) { ::Error("CDFhistos", "Could not load jet libraries"); return; }

  // Load AliAnalysisTaskCDFhistos
  gSystem->SetIncludePath("-I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT/include -I$ALICE_ROOT/JETAN -I$.");

//   gSystem->AddIncludePath("-I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT/include -I$ALICE_ROOT/JETAN -I$.");
//   gSystem->AddIncludePath("-I.");

//   gROOT->LoadMacro("AliAnalysisTaskCDFhistos.cxx++g");

  // Add my task
//   plugin->SetAnalysisSource("AliAnalysisTaskCDFhistos.cxx");
//   plugin->AddIncludePath(".");

  //******************
  // DATA INPUT
  //******************
  InputData ( plugin, kInputData );

  // Make the analysis manager and connect event handlers
  // AliAnalysisManager *mgr  = new AliAnalysisManager("CDFhistos", "CDF histos analysis");
  AliAnalysisManager* mgr  = plugin->CreateAnalysisManager ("CDFhistosMGR");

  if ( kCommonOutputFileName.Length() > 0 ) { mgr->SetCommonFileName( kCommonOutputFileName.Data() ); }

  mgr->SetSkipTerminate(kSkipTerminate);

  // Create input handler (input container created automatically)

  // AOD input handler + add to mgr
  AliAODInputHandler *aodH_in = new AliAODInputHandler();  mgr->SetInputEventHandler(aodH_in);
  aodH_in->SetCheckStatistics(kTRUE);

  if ( kAODjets_analysis && kDeltaAODJetNameInput.Length() )
    {
    Printf("Adding Friend %s",kDeltaAODJetNameInput.Data());
    aodH_in->AddFriend(kDeltaAODJetNameInput.Data());
    }

  // AOD output handler + add to mgr
  AliAODHandler* aodH_out   = new AliAODHandler(); mgr->SetOutputEventHandler(aodH_out);
  aodH_out->SetOutputFileName("AliAOD.root");

  AliAnalysisDataContainer* cout_aod = mgr->GetCommonOutputContainer();
  cout_aod->SetSpecialOutput();

//___________________________________________________________________
  //<><><><><><><><><><>
  //    DEBUGGING
  //<><><><><><><><><><>

  if (kUseDebug) { mgr->SetDebugLevel(3); }

  if (!kPluginMode.CompareTo("test")) { kUseSysInfo = 1 ; }

  // Event frequency for collecting system information
  mgr->SetNSysInfo(kUseSysInfo);

  if ( kUseSysInfo > 0 )
    {
    mgr->RegisterExtraFile("syswatch.root");
    if(kGridMergeExclude.Length())kGridMergeExclude += " ";
    kGridMergeExclude += "syswatch.root";
    }
  else
    { AliLog::SetGlobalLogLevel(AliLog::kError); }

//___________________________________________________________________

//######################
//     Jet analysis
//######################

TString kNonStdBranchInput = "";

cout << "---> START TASK CONFIGURATION <---" << endl;

Int_t use_cdf = 0;

gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/AddTaskJetsReader.C");
gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/AddTaskJetsFinder.C");
gROOT->LoadMacro("AddTaskCDFhistos.C");


//   INIT JET EXCHANGE CONTAINERS
AliAnalysisDataContainer* cont_jet1 = AddJetExchangeContainer("jets_1");
// AliAnalysisDataContainer* cont_jet2 = AddJetExchangeContainer("jets_2");

// READER 1
AliAnalysisTaskJetsReader* taskjetsReader1 = AddTaskJetsReader(cont_jet1,"AOD",kHighPtFilterMask,0.15);
// if (!taskjetsReader) {::Fatal("AnalysisCDFJets", "no taskjetsReader"); return; }
// if(outFile.Length()>0)taskjetsReader->SetNonStdOutputFile(outFile.Data());

// READER 2
// AliAnalysisTaskJetsReader* taskjetsReader2 = AddTaskJetsReader(cont_jet2,"AOD", kHighPtFilterMask,1.);
// if (!taskjetsReader) {::Fatal("AnalysisCDFJets", "no taskjetsReader"); return; }
// if(outFile.Length()>0)taskjetsReader->SetNonStdOutputFile(outFile.Data());


// taskjetsFinder = AddTaskJetsFinder(cont_jet1,"UA1",0.4,0);     // UA1 no bkg subtraction (reader: jet_1)
// taskjetsFinder = AddTaskJetsFinder(cont_jet1,"FASTJET",0.4,1); // FASTJET B1             (reader: jet_1)
// taskjetsFinder = AddTaskJetsFinder(cont_jet2,"FASTJET",0.4,0); // FASTJET B0             (reader: jet_2)





// JET FINDER 1
AliAnalysisTaskJetsFinder* taskjetsFinder1 = AddTaskJetsFinder(cont_jet1,"FASTJET",0.2,0); // FASTJET

// if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }
// if(outFile.Length()>0)taskjetsFinder->SetNonStdOutputFile(outFile.Data());


// kNonStdBranchInput = taskjets->GetNonStdBranch(); cout << "JETFINDER :: nonstdbranch : " << kNonStdBranchInput.Data() << endl;
// if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }

cout << "---> END JET FINDER 1 CONFIGURATION <---" << endl;



// JET FINDER 2
// AliAnalysisTaskJetsFinder* taskjetsFinder2 = AddTaskJetsFinder(cont_jet1,"FASTJET",0.4,0); // FASTJET
// if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }
// if(outFile.Length()>0)taskjetsFinder->SetNonStdOutputFile(outFile.Data());

// kNonStdBranchInput = taskjets->GetNonStdBranch(); cout << "JETFINDER :: nonstdbranch : " << kNonStdBranchInput.Data() << endl;
// if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }

cout << "---> END JET FINDER 2 CONFIGURATION <---" << endl;



// JET FINDER 3
// AliAnalysisTaskJetsFinder* taskjetsFinder3 = AddTaskJetsFinder(cont_jet1,"FASTJET",0.7,0); // FASTJET
// if ( kDeltaAODJetName.Length() > 0 ) { taskjets->SetNonStdOutputFile(kDeltaAODJetName.Data()); }
// if(outFile.Length()>0)taskjetsFinder->SetNonStdOutputFile(outFile.Data());

// kNonStdBranchInput = taskjets->GetNonStdBranch(); cout << "JETFINDER :: nonstdbranch : " << kNonStdBranchInput.Data() << endl;
// if ( kNonStdBranchInput.Length() ) { kJetListSpectrum.Add(new TObjString(kNonStdBranchInput.Data())); }

cout << "---> END JET FINDER 3 CONFIGURATION <---" << endl;


// if (!taskjetsFinder) {::Fatal("AnalysisCDFJets", "no taskjetsFinder"); return; }

cout << "---> END JET FINDERS CONFIGURATION <---" << endl;


if (use_cdf)
  {
  // AliAnalysisTaskCDFhistos *AddTaskCDFhistos( Char_t *connect_file="", Char_t *connect_branch="cdfh_br", Char_t *file_out="CDFh", Char_t *branch_out="cdfh_br", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )

//   AliAnalysisTaskCDFhistos *taskCDF = AddTaskCDFhistos ( "" , kNonStdBranchInput.Data(), kCDFresults.Data() ,kNonStdBranchInput.Data(), 0, 0.);
  AliAnalysisTaskCDFhistos* taskCDF = AddTaskCDFhistos ( kNonStdBranchInput.Data(), 0, 0.);

  if (!taskCDF) { ::Fatal("CDFhistos", "No CDFhistos pointer"); }

  taskCDF->SetDebugLevel(debug);
  taskCDF->SetNonStdFile(kDeltaAODJetName.Data());

  cout << "AddTaskCDF histos :: input file : "   << taskCDF->GetNonStdFile()   << endl;
  cout << "AddTaskCDF histos :: input branch : " << taskCDF->GetNonStdBranch() << endl;
  }


cout << "---> END TASK CONFIGURATION <---" << endl;


// REGISTERING FILE OUTPUTS
  if ( use_cdf && (kCDFresults.Length() > 0) )  { mgr->RegisterExtraFile(kCDFresults.Data()); }
  if (kDeltaAODJetName.Length() > 0 )           { mgr->RegisterExtraFile(kDeltaAODJetName.Data()); }
  if (kCommonOutputFileName.Length() > 0 )      { mgr->RegisterExtraFile(kCommonOutputFileName.Data()); }

// #########  Add grid handler to mgr     ################
  mgr->SetGridHandler(plugin);

//////////////////////////////////////////
////   LIBRARIES TO BE LOADED BY PLUGIN
/////////////////////////////////////////
  anaLibs += kGridExtraFiles;

  anaLibs      = anaLibs.Strip();
  anaLibsExtra = anaLibsExtra.Strip();

  Printf("### anaLibs : %s",anaLibs.Data());
  Printf("### anaLibsExtra : %s",anaLibsExtra.Data());

  if (anaLibs.Length())          {plugin->SetAdditionalLibs(anaLibs.Data());}
  if (anaLibsExtra.Length())     {plugin->SetAdditionalRootLibs(anaLibsExtra.Data());}

//______________________________________________
//              OUTPUT SETTINGS
//______________________________________________

plugin->SetMergeExcludes  (kGridMergeExclude);

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
        if (!mgr->GetOutputEventHandler()) {continue;}
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

    // Use the output files connected to output containers from the analysis manager
    // rather than the files defined by SetOutputFiles (true/false)
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


  // Optionally define the files to be archived.
  //   plugin->SetOutputArchive("log_archive.zip:stdout,stderr@ALICE::NIHAM::File root_archive.zip:EventStat_temp.root,PWG4_JetTasksOutput.root@ALICE::NIHAM::File");

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
      ::Fatal("AnalysisCDFjets", "No other outputs than std !");
      }
    }

  plugin->SetDefaultOutputs(kFALSE);
  plugin->SetOutputArchive ( outputArchive );   cout << "PLUGIN :: Output archive : " << outputArchive.Data() << endl;


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

  if (!mgr->GetGridHandler()) { ::Error("AnalysisCDFjets::StartAnalysis", "Grid plugin not initialized"); return; }

  mgr->StartAnalysis(kAnalysisMode.Data(),nentries,firstentry);
//  mgr->StartAnalysis(kAnalysisMode.Data());

  }
  // END of mgr->InitAnalysis()

}

//______________________________________________________________________________
Bool_t LoadCommonLibraries()
  {
//   gROOT->ProcessLine("./include $CGAL_DIR/include $FASTJET/include $ALICE_ROOT/include");

//   gSystem->SetIncludePath("-I$ROOTSYS/include -I$CGAL_DIR/include -I$FASTJET/include -I$ALICE_ROOT/include");
//   gROOT->ProcessLine(".include $ALICE_ROOT/include");

//   ::Info("AnalysisCDF::LoadAnalysisLibs", "Include path for Aclic compilation:\n%s", gSystem->GetIncludePath());

  Int_t result = -999;

  // ROOT libraries
  result = gSystem->Load("libTree");    if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libGeom");    if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libVMC");     if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libPhysics"); if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libMinuit");  if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libMinuit2"); if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libGui");     if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }
  result = gSystem->Load("libProof");   if (result < 0) { Printf("ROOT lib load error"); return kFALSE; }

  ::Info("AnalysisCDF::LoadROOTLibs", "Load ROOT libraries:    SUCCESS");
  return kTRUE;
  }


//______________________________________________________________________________
Bool_t LoadAnalysisLibraries()
{
// Load common analysis libraries.
  Bool_t success = kTRUE;

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
  success &= LoadLibrary("libPWGJE.so", kTRUE);

//   cout << "Libs FASTJET :" << gSystem->GetLibraries("","D") << endl;

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


if (result < 0) { ::Error("AnalysisCDFjets::LoadSources", "Could not load source %s", source); return kFALSE; }

return kTRUE;
}

//______________________________________________________________________________
AliAnalysisAlien* CreateAlienHandler (const char* plugin_mode="test")
{
  TString     kPluginExecutableCommand = "aliroot -b -q";
  Int_t       kTestFiles               =  1;    // Number of test files

  TString     kTrainName         = "CDFanalysis";   // *CHANGE ME* (no blancs or special characters)
  TString     kWorkDir           = "CDF2";        // AliEn work dir; relative to AliEn $HOME
  TString     kJobTag            = "sev_jet_analysis"; // *CHANGE ME*

  Bool_t      kPluginUseProductionMode = kFALSE;         // use the plugin in production mode

  TString     kPluginAPIVersion        = "V1.1x";
  TString     kPluginRootVersion       = "v5-34-05";
  TString     kPluginAliRootVersion    = "v5-04-59-AN";

  Bool_t      kPluginFastReadOption    = kFALSE;         // use xrootd tweaks
  Bool_t      kPluginOverwriteMode     = kTRUE;          // overwrite existing collections

  Int_t       kGridFilesPerJob         = 25;    // Maximum number of files per job (gives size of AOD)
  Int_t       kGridMaxMergeFiles       = 100;   // Number of files merged in a chunk grid run range
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
  // exit from aliensh after submmiting job
  plugin->SetDropToShell(kTRUE);

  // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
  plugin->SetRunMode(plugin_mode);

  plugin->SetCheckCopy(kFALSE);

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
  //   plugin->SetRootVersionForProof ( kPluginRootVersion ) ;
  plugin->SetAliRootMode ( kAlirootMode ) ;

  // plugin->SetProofParameter(const char *pname, const char *value);

return plugin;
}
