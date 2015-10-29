/// \file AddTaskEmcalJetSample.C
/// \brief AddTaskEmcalJetSample
///
/// AddTaskEmcalJetSample
///
/// Add a AddTaskEmcalJetSample task - detailed signature
/// \param const char *ntracks
/// \param const char *nclusters
/// \param const char *njets
/// \param const char *nrho
/// \param Int_t nCentBins
/// \param Double_t jetradius
/// \param Double_t jetptcut
/// \param Double_t jetareacut
/// \param const char *type ; either TPC, EMCAL or USER
/// \param Int_t leadhadtype ; 0 = charged, 1 = neutral, 2 = both
/// \param const char *taskname
/// \return AliAnalysisTaskEmcalJetSample* task

//__________________________________________________________________________________
AliAnalysisTaskEmcalJetSample* AddTaskEmcalJetSample(
  const char *ntracks            = "Tracks",
  const char *nclusters          = "CaloClusters",
  const char *njets              = "Jets",
  const char *nrho               = "Rho",
  Int_t       nCentBins          = 1,
  Double_t    jetradius          = 0.2,
  Double_t    jetptcut           = 1,
  Double_t    jetareacut         = 0.6,
  const char *type               = "EMCAL",
  Int_t       leadhadtype        = 0,
  const char *taskname           = "EmcalJetSample"
)
{
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTaskEmcalJetSample", "No analysis manager to connect to.");
    return NULL;
  }

  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  if (!mgr->GetInputEventHandler())
  {
    ::Error("AddTaskEmcalJetSample", "This task requires an input event handler");
    return NULL;
  }

  //-------------------------------------------------------
  // Init the task and do settings
  //-------------------------------------------------------

  TString name     ( taskname );
  TString tracks   ( ntracks );
  TString clusters ( nclusters );
  TString jets     ( njets );
  TString rho      ( nrho );
  TString acctype  ( type );

  if ( jetptcut < 1. ) { jetptcut = 1.; }

  TString jetstr = "jpt";
  jetstr += ( ULong_t ) ( jetptcut * 1000 );

  if ( !jets.IsNull() )    { name += "_" + jets + "_" + jetstr ; }
  if ( !rho.IsNull() )     { name += "_" + rho; }
  if ( !acctype.IsNull() ) { name += "_" + acctype; }

  cout << "name: " << name.Data() << endl;

  AliAnalysisTaskEmcalJetSample* jetTask = new AliAnalysisTaskEmcalJetSample(name);
  jetTask->SetCentRange(0.,100.);
  jetTask->SetNCentBins(nCentBins);

  AliParticleContainer *trackCont  = jetTask->AddParticleContainer(ntracks);
  if(trackCont) { trackCont->SetClassName("AliVTrack"); }
  AliClusterContainer *clusterCont = jetTask->AddClusterContainer(nclusters);

  TString strType(type);
  AliJetContainer *jetCont = jetTask->AddJetContainer(njets,strType,jetradius);
  if(jetCont) {
    jetCont->SetRhoName(nrho);
    jetCont->ConnectParticleContainer(trackCont);
    jetCont->ConnectClusterContainer(clusterCont);
    jetCont->SetZLeadingCut(0.98,0.98);
    jetCont->SetPercAreaCut(jetareacut);
    jetCont->SetJetPtCut(jetptcut);
    jetCont->SetLeadingHadronType(leadhadtype);
  }

  //-------------------------------------------------------
  // Final settings, pass to manager and set the containers
  //-------------------------------------------------------

  mgr->AddTask(jetTask);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput1  = mgr->GetCommonInputContainer()  ;

  TString contname = name + "_histos";
  TString outfile = AliAnalysisManager::GetCommonFileName();

  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer( contname.Data(), TList::Class(), AliAnalysisManager::kOutputContainer, outfile.Data() );

  mgr->ConnectInput  (jetTask, 0,  cinput1 );
  mgr->ConnectOutput (jetTask, 1, coutput1 );

  return jetTask;
}

/// Add a AliAnalysisTaskEmcalJetSample task - info from AliEmcalJetTask* jetFinderTask
/// \param AliEmcalJetTask* jetFinderTask
/// \param Int_t nCentBins
/// \param Double_t jetptcut
/// \param Double_t jetareacut
/// \param const char *type ; either TPC, EMCAL or USER
/// \param Int_t leadhadtype ; 0 = charged, 1 = neutral, 2 = both
/// \param const char *nrho
/// \param const char *taskname
/// \return AddTaskEmcalJetSample* task
AliAnalysisTaskEmcalJetSample *AddTaskEmcalJetSample( AliEmcalJetTask* jetFinderTask,
  Int_t       nCentBins          = 1,
  Double_t    jetptcut           = 1.,
  Double_t    jetareacut         = 0.6,
  const char *type               = "EMCAL",
  Int_t       leadhadtype        = 0,
  const char *nrho               = "Rho",
  const char *taskname           = "EmcalJetSample"
)
  {
  if ( !jetFinderTask->InheritsFrom ( AliEmcalJetTask::Class() ) )
    { AliError("AddTaskEmcalJetSample :: task is not/ does not inherits from AliEmcalJetTask"); }

  const char* ntracks            = jetFinderTask->GetTracksName();
  const char* nclusters          = jetFinderTask->GetClusName();
  const char* njets              = jetFinderTask->GetJetsName();
  Double_t    jetradius          = jetFinderTask->GetRadius();

  AliAnalysisTaskEmcalJetSample* jetTask = AddTaskEmcalJetSample(ntracks , nclusters, njets, nrho, nCentBins, jetradius, jetptcut, jetareacut, type, leadhadtype, taskname);

  return jetTask;
  }

/// Add a AliAnalysisTaskEmcalJetCDF task - info from char* taskname
/// \param const char* taskname
/// \param Int_t nCentBins
/// \param Double_t jetptcut
/// \param Double_t jetareacut
/// \param const char *type ; either TPC, EMCAL or USER
/// \param Int_t leadhadtype ; 0 = charged, 1 = neutral, 2 = both
/// \param const char *nrho
/// \param const char *taskname
/// \return AddTaskEmcalJetSample* task
AliAnalysisTaskEmcalJetSample *AddTaskEmcalJetSample ( const char* taskname,
  Int_t       nCentBins          = 1,
  Double_t    jetptcut           = 1.,
  Double_t    jetareacut         = 0.6,
  const char *type               = "EMCAL",
  Int_t       leadhadtype        = 0,
  const char *nrho               = "Rho",
  const char *taskname           = "EmcalJetSample"
)
  {
  AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) { ::Error("AddTaskEmcalJetSample", "No analysis manager to connect to."); }

  AliEmcalJetTask* jf = dynamic_cast<AliEmcalJetTask*>(mgr->GetTask(taskname));
  if (!jf) { AliError("AddTaskEmcalJetSample :: task is not EmcalJetTask");}

  const char* ntracks            = jf->GetTracksName();
  const char* nclusters          = jf->GetClusName();
  const char* njets              = jf->GetJetsName();
  Double_t    jetradius          = jf->GetRadius();

  AliAnalysisTaskEmcalJetSample* jetTask = AddTaskEmcalJetSample(ntracks , nclusters, njets, nrho, nCentBins, jetradius, jetptcut, jetareacut, type, leadhadtype, taskname);

  return jetTask;
  }

// kate: indent-mode none; indent-width 2; replace-tabs on;

