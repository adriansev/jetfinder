// AliAnalysisTaskCDFhistos *AddTaskCDFhistos ( Char_t* connect_file="", Char_t* connect_branch="", Char_t* file_out="CDFh", Char_t* branch_out="cdfh_br", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )
AliAnalysisTaskCDFhistos* AddTaskCDFhistos(Char_t* jb = "cdfhistos", Int_t jet1_min_tracks = 0, Double_t jet1_min_pt = 0. )
{
// Creates a CDF histograms task, configures it and adds it to the analysis manager.
// Get the pointer to the existing analysis manager via the static access method.
//==============================================================================
AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
if (!mgr) { ::Error("AddTaskCDFhistos", "No analysis manager to connect to."); return NULL; }

// Check the analysis type using the event handlers connected to the analysis manager.
//==============================================================================
if (!mgr->GetInputEventHandler()) { ::Error("AddTaskCDFhistos", "This task requires an input event handler"); return NULL; }

// input
// TString fOut  (file_out); // the file where CDF histos will be written
// TString jbOut (branch_out);  // the branch where the TList of histos will be written
// TString kConnectFile   (connect_file);  // deltaAOD file where jets are written
// TString kConnectBranch (connect_branch) ;  // branch of Jet Finder output

// TString file_folder_out ( Form("%s:hist_%s",fOut.Data(),branchOut_cdf.Data()) ); // file:folder


TString jbOut (jb); jbOut = jbOut(4,jbOut.Sizeof()); jbOut.ToLower();


TString branchOut_cdf = jbOut + Form("_j1t%d", (int)((jet1_min_tracks))) + Form("_j1pt%d",(int)((jet1_min_pt)));

TString taskName ( Form("CDFtask_%s",branchOut_cdf.Data() ) );
TString listName ( Form("CDFlist_%s",branchOut_cdf.Data() ) );

TString file_folder_out ( Form("%s:CDFhistos_%s",AliAnalysisManager::GetCommonFileName(),branchOut_cdf.Data()) );

// Create the task and configure it.
//===========================================================================
AliAnalysisTaskCDFhistos* cdfhistos = new AliAnalysisTaskCDFhistos( taskName );
cdfhistos->SetJetMinTraks (jet1_min_tracks);  // minimum nr of particles in jet1 (leading jet)
cdfhistos->SetJetMinPt    (jet1_min_pt);      // min pt of jet1 (leading jet)

cdfhistos->SetNonStdBranch(jb);

// Connect to inputs
// if ( kConnectFile.Length() > 0 )   { cdfhistos->SetNonStdFile  ( kConnectFile.Data()); }
// if ( kConnectBranch.Length() > 0 ) { cdfhistos->SetNonStdBranch( kConnectBranch.Data()); }

mgr->AddTask(cdfhistos);

AliAnalysisDataContainer* cout_cdfhistos = mgr->CreateContainer( listName.Data() , TList::Class(), AliAnalysisManager::kOutputContainer, file_folder_out.Data() );

// Create ONLY the output containers for the data produced by the task.
// Get and connect other common input/output containers via the manager as below
//==============================================================================
mgr->ConnectInput  (cdfhistos, 0, mgr->GetCommonInputContainer());
//    mgr->ConnectOutput (cdfhistos, 0, mgr->GetCommonOutputContainer());
mgr->ConnectOutput (cdfhistos, 1, cout_cdfhistos);


cout << "jb :" << jb << endl;
cout << "jbOut :" << jbOut << endl;
cout << "branchOut_cdf :" << branchOut_cdf << endl;
cout << "taskName :" << taskName << endl;
cout << "listName :" << listName << endl;
cout << "file_folder_out :" << file_folder_out << endl;

cout << "container info ---->" << endl;
cout_cdfhistos->PrintContainer() ;

return cdfhistos;
}



