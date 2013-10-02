// $Id: AliAnalysisTaskEmcalJetCDF.cxx 62732 2013-05-31 16:10:24Z loizides $
//
// Jet sample analysis task.
//
// Author: S.Aiola

#include "AliAnalysisTaskEmcalJetCDF.h"

Double_t const  kPI        = TMath::Pi();
Double_t const  kTWOPI     = 2.*kPI;
Double_t const  kPI_3      = kPI/3.;
Double_t const  kPI_2      = kPI/2.;

ClassImp ( AliAnalysisTaskEmcalJetCDF )

//________________________________________________________________________
AliAnalysisTaskEmcalJetCDF::AliAnalysisTaskEmcalJetCDF() :
  AliAnalysisTaskEmcalJet ( "AliAnalysisTaskEmcalJetCDF", kTRUE ),
  fH1            (NULL),
  fH2            (NULL),
  fH3            (NULL),
  fH4            (NULL),
  fH5            (NULL),
  fH6            (NULL),
  fH7            (NULL),
  fH8            (NULL),
  fH9            (NULL),
  fH10           (NULL),
  fH20           (NULL),
  fH21           (NULL),
  fH21Toward     (NULL),
  fH21Transverse (NULL),
  fH21Away       (NULL),
  fH22           (NULL),
  fH22Toward     (NULL),
  fH22Transverse (NULL),
  fH22Away       (NULL),
  fH23           (NULL),
  fH23jet1       (NULL),
  fH23Toward     (NULL),
  fH23Transverse (NULL),
  fH23Away       (NULL),
  fH24           (NULL),
  fH25           (NULL),
  fH26           (NULL),
  fH27           (NULL),
  fH28           (NULL),
  fH29           (NULL)

  {
  // Default constructor.
  SetMakeGeneralHistograms ( kTRUE );
  }

//________________________________________________________________________
AliAnalysisTaskEmcalJetCDF::AliAnalysisTaskEmcalJetCDF ( const char* name ) :
  AliAnalysisTaskEmcalJet ( name, kTRUE ),
  fH1            (NULL),
  fH2            (NULL),
  fH3            (NULL),
  fH4            (NULL),
  fH5            (NULL),
  fH6            (NULL),
  fH7            (NULL),
  fH8            (NULL),
  fH9            (NULL),
  fH10           (NULL),
  fH20           (NULL),
  fH21           (NULL),
  fH21Toward     (NULL),
  fH21Transverse (NULL),
  fH21Away       (NULL),
  fH22           (NULL),
  fH22Toward     (NULL),
  fH22Transverse (NULL),
  fH22Away       (NULL),
  fH23           (NULL),
  fH23jet1       (NULL),
  fH23Toward     (NULL),
  fH23Transverse (NULL),
  fH23Away       (NULL),
  fH24           (NULL),
  fH25           (NULL),
  fH26           (NULL),
  fH27           (NULL),
  fH28           (NULL),
  fH29           (NULL)

  {
  // Standard constructor.
  SetMakeGeneralHistograms ( kTRUE );
  }

//________________________________________________________________________
AliAnalysisTaskEmcalJetCDF::~AliAnalysisTaskEmcalJetCDF()
  {
  // Destructor.
  }

//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalJetCDF::Run()
  {
  // Run analysis code here, if needed. It will be executed before FillHistograms().
  return kTRUE;  // If return kFALSE FillHistogram() will NOT be executed.
  }

//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalJetCDF::FillHistograms()
{
// Fill histograms.

// protection against not finding jets
if ( !fJets )           { cout << "Jets pointer NULL" << endl; return; }
if ( fJets->IsEmpty() ) { cout << "Jets EMPTY" << endl; return; }

if ( fDebug > 0 )
  {
  cout << "Jets pointer : "  << fJets << endl;
  fJets->Print();
  }

  const Int_t fNJets = fJets->GetEntriesFast();
  const Int_t fNPart = fInputEvent->GetNumberOfTracks(); //fTracks->GetEntriesFast();

  printf ( "fNJets = %i ; fNPart = %i \n", fNJets, fNPart ); fflush(stdout);
  printf ( "CDFhistos::end of ConnectInputData() \n");       fflush(stdout);

  if ((fNJets == 0) || (fNPart == 0)) { return; }

  for ( Int_t ij = 0; ij < fNJets; ij++ )
    {
    AliEmcalJet* jet = static_cast<AliEmcalJet*> ( fJets->At ( ij ) );

    if ( !jet ) { AliError ( Form ( "Could not receive jet %d", ij ) ); continue; }
    if ( !AcceptJet ( jet ) ) {continue;}

    fH1->Fill ( jet->Pt() );  // Pt distribution of jets
    fH2->Fill ( jet->Eta() ); // Eta distribution of jets
    fH3->Fill ( jet->Phi() ); // Phi distribution of jets
    fH4->Fill ( jet->GetNumberOfTracks() );  // Multiplicity of jets // 1 unit of multiplicity /bin

    //   Float_t ptLeading = GetLeadingHadronPt ( jet );
    //   fHistJetsPtLeadHad[fCentBin]->Fill ( jet->Pt(), ptLeading );

    //   if ( fRho )
    //     {
    //     Float_t corrPt = jet->Pt() - fRhoVal * jet->Area();
    //     fHistJetsCorrPtArea[fCentBin]->Fill ( corrPt, jet->Area() );
    //     }
    }

  // Distribution of jets in events
  fH5->Fill ( fNJets );

  printf ("CDFhistos:: end of global jet histos \n"); fflush(stdout);

  static Int_t sortedJets[999] = { -1};
  Bool_t r = GetSortedArray ( sortedJets, fJets );

  Double_t jet1_pt = -1;
  Int_t    jet1_npart = -1;

  AliEmcalJet* jet1 = NULL;

  if ( r && sortedJets[0] >= 0 )
    {
    jet1 = static_cast<AliEmcalJet*> ( fJets->At ( sortedJets[0] ) );
//     if ( !AcceptJet ( jet1 ) ) {continue;}

    if ( jet1 )
      {
      jet1_pt = jet1->Pt();
      jet1_npart = jet1->GetNumberOfTracks();

      fH6->Fill( jet1_npart );           // Jet1 Charged Multiplicity Distribution ( ->Scale (events) )
      fH7->Fill( jet1_pt, jet1_npart );  // N_{chg}(jet1) vs P_{T}(charged jet1)
      }
    else
      { AliError ( "Could not retrieve leading jet!" ); }
    }

  // "Transverse" Pt Distribution and computing of pt sum of event

  Double_t event_pt = 0.;
  for (Int_t i = 0 ; i < fNPart ; i++ )
    {// parse particles in event
    AliVParticle* track = fInputEvent->GetTrack(i);
    if (track) { event_pt += track->Pt(); } // pt sum of event
    }

  if ( fDebug > 1 ) { printf ("Sum of all Pt in event : pt_sum_event = %g", event_pt) ; }

  //___________________________________________________________________________
  // Charge momentum distribution for leading jet
  // and
  // N_{chg} vs the Azimuthal Angle from Charged Jet1


  // parsing tracks of jet1 (leading jet)
  for( Int_t i = 0 ; i < jet1_npart ; i++ )
    {
    AliVParticle* track = jet1->TrackAt(i, fTracks);
    Double_t track_pt = track->Pt() ;

    fH8->Fill ( track_pt / jet1_pt ) ; //  Charge momentum distribution for leading jet

    fH23jet1->Fill ( track_pt ) ; // jet1 pt distribution
    //___________________________________________________________________________
    // Recomputing of radius of particles in leading jet
    fH20->Fill ( DeltaR(jet1, track) ); //  Distribution of R in leading jet
    }

  fH21->Fill ( jet1_pt, fNPart   ); // N_{chg} (in the event - including jet1) vs P_{T}(charged jet1)
  fH22->Fill ( jet1_pt, event_pt ); // PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)

//_______________________________________________________________________
// Computing of radius that contain 80% of Leading Jet ( PT and multiplicity )
//_______________________________________________________________________
Double_t jet1_core_part  = 0.8 * jet1_npart ;
Double_t jet1_core_pt    = 0.8 * jet1_pt ;
Int_t    counter_part = 0  ;
Double_t counter_pt   = 0. ;

//------------------------------------------------------------------>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// parsing tracks of jet1 (leading jet) in decreasing order of Pt
for( Int_t i = 0 ; i < jet1_npart ; i++ )
  {
  jet1->SortConstituents(); // Sort constituent by index (increasing).

  AliVParticle* track = jet1->TrackAt(i, fTracks);
  Double_t dpart = DeltaR (jet1, track);

  if ( counter_part <= jet1_core_part )
    {// fill histograms for 80% of particles
    fH24->Fill ( jet1_pt, dpart);         //  Jet1 Size vs P_{T}(charged jet1) - 80% of particles  (Fig 6)
    }

  if ( counter_pt   <= jet1_core_pt   )
    {// fill histograms for 80% of particles
    fH25->Fill ( jet1_pt, dpart);         //  Jet1 Size vs P_{T}(charged jet1) - 80% of Pt   (Fig 6)
    }
  }


// parsing all tracks in event
for (Int_t i = 0 ; i < fNPart ; i++ )
  {// parse particles in event
    AliAODTrack* track = dynamic_cast<AliAODTrack*> ( fAOD->GetTrack(i) );

    Double_t track_pt = track->Pt();
    Double_t dpart = jet1->DeltaR( track );
    Double_t dphi_part_jet1 = TVector2::Phi_mpi_pi ( track->Phi() - jet1_phi ) ; // restrict the delta phi to (-pi,pi) interval
    dphi_part_jet1 = TMath::Abs(dphi_part_jet1); // and then to (0,pi)

    counter_part++;
    counter_pt += track->Pt();

    if ( counter_part <= jet1_core_part )
    {// fill histograms for 80% of particles
      fH26->Fill ( dpart, counter_part );   //  N_{chg} vs the Distance R from Charged Jet1 - 80% of particles (Fig 8)
      fH28->Fill ( dpart, counter_pt   );   //  PT_{sum} vs the Distance R from Charged Jet1 - 80% of particles (Fig 9)
    }

    if ( counter_pt   <= jet1_core_pt   )
    {// fill histograms for 80% of particles
      fH27->Fill ( dpart, counter_part );   //  N_{chg} vs the Distance R from Charged Jet1 - 80% of Pt (Fig 8)
      fH29->Fill ( dpart, counter_pt   );   //  PT_{sum} vs the Distance R from Charged Jet1 - 80% of Pt  (Fig 9)
    }


    // dphi distribution (total and per toward,away,transverse regions)

    fH9 ->Fill ( TMath::RadToDeg() * dphi_part_jet1, fNPart )   ; //  N_{chg} vs the Azimuthal Angle from Charged Jet1 (Fig 15,17,19)
    fH10->Fill ( TMath::RadToDeg() * dphi_part_jet1, event_pt ) ; //  P_{T} sum vs the Azimuthal Angle from Charged Jet1  (Fig 16,18,20)

    if ( dphi_part_jet1 < kPI_3 )
    {
      fH21Toward->Fill ( jet1_pt, fNPart   ); // N_{chg} (in the event - including jet1) vs P_{T}(charged jet1)
      fH22Toward->Fill ( jet1_pt, event_pt ); // PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)
      fH23Toward->Fill ( track_pt );          // Pt Distribution of charged particles    (Fig 37-41)
    }
    else
      if ( ( dphi_part_jet1 >= kPI_3 ) && ( dphi_part_jet1 < (2.* kPI_3) ) )
      {
        fH21Transverse->Fill ( jet1_pt, fNPart   ); // N_{chg} (in the event - including jet1) vs P_{T}(charged jet1)
        fH22Transverse->Fill ( jet1_pt, event_pt ); // PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)
        fH23Transverse->Fill ( track_pt );          // Pt Distribution of charged particles  (Fig 37-41)
      }
      else
        if ( dphi_part_jet1 >= ( 2.* kPI_3 ) )
        {
          fH21Away->Fill ( jet1_pt, fNPart   ); // N_{chg} (in the event - including jet1) vs P_{T}(charged jet1)
          fH22Away->Fill ( jet1_pt, event_pt ); // PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)
          fH23Away->Fill ( track_pt );          // Pt Distribution of charged particles  (Fig 37-41)
        }
        fH23->Fill ( track_pt ); //  Pt Distribution of charged particles in event


  }



return kTRUE;
}

//________________________________________________________________________
void AliAnalysisTaskEmcalJetCDF::UserCreateOutputObjects()
  {
  // Create user output.

  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();

  //  Histograms
  fOutput->SetOwner(kTRUE);

  // Create the list of histograms. Only the list is owned.

  fH1 = new TH1F ("histo1", "Pt distribution of jets", 400, 0, 400);  // 1GeV/bin
  fH1->SetStats(kTRUE);
  fH1->GetXaxis()->SetTitle("P_{T} of jets");
  fH1->GetYaxis()->SetTitle("Number of jets");
  fH1->GetXaxis()->SetTitleColor(1);
  fH1->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH1);

  fH2 = new TH1F ("histo2", "Eta distribution of jets", 200, -1., 1.); // 1 unit of rapidity / 100 bin
  fH2->SetStats(kTRUE);
  fH2->GetXaxis()->SetTitle("Eta of jets");
  fH2->GetYaxis()->SetTitle("Number of jets");
  fH2->GetXaxis()->SetTitleColor(1);
  fH2->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH2);

  fH3 = new TH1F ("histo3", "Phi distribution of jets", 126, 0.,6.3);
  fH3->SetStats(kTRUE);
  fH3->GetXaxis()->SetTitle("Phi of jets");
  fH3->GetYaxis()->SetTitle("Number of jets");
  fH3->GetXaxis()->SetTitleColor(1);
  fH3->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH3);

  fH4 = new TH1F ("histo4", "Multiplicity of jets", 100, 0,100);  // 1 unit of multiplicity /bin
  fH4->SetStats(kTRUE);
  fH4->GetXaxis()->SetTitle("Particles in jets");
  fH4->GetYaxis()->SetTitle("Number of jets");
  fH4->GetXaxis()->SetTitleColor(1);
  fH4->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH4);

  fH5 = new TH1F ("histo5", "Distribution of jets in events", 20, 0, 20);
  fH5->SetStats(kTRUE);
  fH5->GetXaxis()->SetTitle("Number of jets");
  fH5->GetYaxis()->SetTitle("Number of events");
  fH5->GetXaxis()->SetTitleColor(1);
  fH5->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH5);

  fH6 = new TH1F ("histo6", "Jet1 Charged Multiplicity Distribution", 100, 0,100);
  fH6->SetStats(kTRUE);
  fH6->GetXaxis()->SetTitle("N_{chg}");
  fH6->GetYaxis()->SetTitle("Number of jets");
  fH6->GetXaxis()->SetTitleColor(1);
  fH6->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH6);

  fH7 = new TProfile ("histo7","N_{chg}(jet1) vs P_{T}(charged jet1)", 400, 0. , 400. , 0., 400. ) ;
  fH7->SetStats(kTRUE);
  fH7->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH7->GetYaxis()->SetTitle("<N_{chg}(jet1)> in 1 GeV/c bin");
  fH7->GetXaxis()->SetTitleColor(1);
  fH7->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH7);

  fH8 = new TH1F ("histo8", "Charge momentum distribution for jet1", 120, 0 , 1.2);
  fH8->SetStats(kTRUE);
  fH8->GetXaxis()->SetTitle("z = p/P(charged jet1)");
  fH8->GetYaxis()->SetTitle("F(Z) = dN/dz");
  fH8->GetXaxis()->SetTitleColor(1);
  fH8->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH8);

  fH9 = new TProfile ("histo9", "N_{chg} vs the Azimuthal Angle from Charged Jet1", 50 , 0. , 180. , 0 , 20 );
  fH9->SetStats(kTRUE);
  fH9->GetXaxis()->SetTitle("#Delta#phi (degrees)");
  fH9->GetYaxis()->SetTitle("<N_{chg}> in 3.6 degree bin");
  fH9->GetXaxis()->SetTitleColor(1);
  fH9->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH9);

  fH10 = new TProfile ("histo10", "P_{T} sum vs the Azimuthal Angle from Charged Jet1", 50 , 0. , 180. , 0 , 100 );
  fH10->SetStats(kTRUE);
  fH10->GetXaxis()->SetTitle("|#Delta#phi| (degrees)");
  fH10->GetYaxis()->SetTitle("<P_{T} sum> in 3.6 degree bin");
  fH10->GetXaxis()->SetTitleColor(1);
  fH10->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH10);

  fH20 = new TH1F ("histo20", "Distribution of R in leading jet", 100, 0.,1.);
  fH20->SetStats(kTRUE);
  fH20->GetXaxis()->SetTitle("R");
  fH20->GetYaxis()->SetTitle("dN/dR");
  fH20->GetXaxis()->SetTitleColor(1);
  fH20->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH20);

  fH21 = new TProfile ("histo21","N_{chg}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0., 200. , 0., 30. ) ;
  fH21->SetStats(kTRUE);
  fH21->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH21->GetYaxis()->SetTitle("<N_{chg}(in the event - including jet1)> in 1 GeV/c bin");
  fH21->GetXaxis()->SetTitleColor(1);
  fH21->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH21);

  fH21Toward = new TProfile ("histo21_toward","N_{chg}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0., 200. , 0., 12. ) ;
  fH21Toward->SetStats(kTRUE);
  fH21Toward->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH21Toward->GetYaxis()->SetTitle("<N_{chg}(in the event - including jet1)> in 1 GeV/c bin");
  fH21Toward->GetXaxis()->SetTitleColor(1);
  fH21Toward->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH21Toward);

  fH21Transverse = new TProfile ("histo21_transverse","N_{chg}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0., 200. , 0., 12. ) ;
  fH21Transverse->SetStats(kTRUE);
  fH21Transverse->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH21Transverse->GetYaxis()->SetTitle("<N_{chg}(in the event - including jet1)> in 1 GeV/c bin");
  fH21Transverse->GetXaxis()->SetTitleColor(1);
  fH21Transverse->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH21Transverse);

  fH21Away = new TProfile ("histo21_away","N_{chg}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0., 200. , 0., 12. ) ;
  fH21Away->SetStats(kTRUE);
  fH21Away->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH21Away->GetYaxis()->SetTitle("<N_{chg}(in the event - including jet1)> in 1 GeV/c bin");
  fH21Away->GetXaxis()->SetTitleColor(1);
  fH21Away->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH21Away);


  //__________________________________________________________________
  fH22 = new TProfile ("histo22","PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0. , 200. , 0., 50. ) ;
  fH22->SetStats(kTRUE);
  fH22->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH22->GetYaxis()->SetTitle("<PT_{sum}(in the event - including jet1)> in 1 GeV/c bin");
  fH22->GetXaxis()->SetTitleColor(1);
  fH22->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH22);

  fH22Toward = new TProfile ("histo22_toward","PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0. , 200. , 0., 50. ) ;
  fH22Toward->SetStats(kTRUE);
  fH22Toward->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH22Toward->GetYaxis()->SetTitle("<PT_{sum}(in the event - including jet1)> in 1 GeV/c bin");
  fH22Toward->GetXaxis()->SetTitleColor(1);
  fH22Toward->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH22Toward);

  fH22Transverse = new TProfile ("histo22_transverse","PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0. , 200. , 0., 50. ) ;
  fH22Transverse->SetStats(kTRUE);
  fH22Transverse->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH22Transverse->GetYaxis()->SetTitle("<PT_{sum}(in the event - including jet1)> in 1 GeV/c bin");
  fH22Transverse->GetXaxis()->SetTitleColor(1);
  fH22Transverse->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH22Transverse);

  fH22Away = new TProfile ("histo22_away","PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1)", 200, 0. , 200. , 0., 50. ) ;
  fH22Away->SetStats(kTRUE);
  fH22Away->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH22Away->GetYaxis()->SetTitle("<PT_{sum}(in the event - including jet1)> in 1 GeV/c bin");
  fH22Away->GetXaxis()->SetTitleColor(1);
  fH22Away->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH22Away);


  //____________________________________________________________________________________
  fH23 = new TH1F ("histo23","Pt Distribution of charged particles", 800, 0., 800.);
  fH23->SetStats(kTRUE);
  fH23->GetXaxis()->SetTitle("P_{T} (charged) (GeV/c)");
  fH23->GetYaxis()->SetTitle("dN_{chg}/dP_{T} (1/GeV/c)");
  fH23->GetXaxis()->SetTitleColor(1);
  fH23->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH23);

  fH23jet1 = new TH1F ("histo23jet1","Pt Distribution of charged particles in jet1", 800, 0., 800.);
  fH23jet1->SetStats(kTRUE);
  fH23jet1->GetXaxis()->SetTitle("P_{T} (charged) (jet1) (GeV/c)");
  fH23jet1->GetYaxis()->SetTitle("dN_{chg}/dP_{T} (jet1) (1/GeV/c)");
  fH23jet1->GetXaxis()->SetTitleColor(1);
  fH23jet1->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH23jet1);

  fH23Toward = new TH1F ("histo23_toward","'Toward' Pt Distribution of charged particles", 800, 0., 800.);
  fH23Toward->SetStats(kTRUE);
  fH23Toward->GetXaxis()->SetTitle("P_{T} (charged) (GeV/c)");
  fH23Toward->GetYaxis()->SetTitle("dN_{chg}/dP_{T} (1/GeV/c)");
  fH23Toward->GetXaxis()->SetTitleColor(1);
  fH23Toward->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH23Toward);

  fH23Transverse = new TH1F ("histo23_transverse","'Transverse' Pt Distribution of charged particles", 800, 0., 800.);
  fH23Transverse->SetStats(kTRUE);
  fH23Transverse->GetXaxis()->SetTitle("P_{T} (charged) (GeV/c)");
  fH23Transverse->GetYaxis()->SetTitle("dN_{chg}/dP_{T} (1/GeV/c)");
  fH23Transverse->GetXaxis()->SetTitleColor(1);
  fH23Transverse->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH23Transverse);

  fH23Away = new TH1F ("histo23_away","'Away' Pt Distribution of charged particles", 800, 0., 800.);
  fH23Away->SetStats(kTRUE);
  fH23Away->GetXaxis()->SetTitle("P_{T} (charged) (GeV/c)");
  fH23Away->GetYaxis()->SetTitle("dN_{chg}/dP_{T} (1/GeV/c)");
  fH23Away->GetXaxis()->SetTitleColor(1);
  fH23Away->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH23Away);

  fH24 = new TProfile ("histo24","Jet1 Size vs P_{T}(charged jet1)", 200, 0., 200. , 0., 0.5) ;
  fH24->SetStats(kTRUE);
  fH24->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH24->GetYaxis()->SetTitle("<R(chgjet1)> in 1 GeV/c bin");
  fH24->GetXaxis()->SetTitleColor(1);
  fH24->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH24);

  fH25 = new TProfile ("histo25","Jet1 Size vs P_{T}(charged jet1)", 200, 0., 200. , 0., 0.5) ;
  fH25->SetStats(kTRUE);
  fH25->GetXaxis()->SetTitle("P_{T} (charged jet1) (GeV/c)");
  fH25->GetYaxis()->SetTitle("<R(chgjet1)> in 1 GeV/c bin");
  fH25->GetXaxis()->SetTitleColor(1);
  fH25->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH25);

  fH26 = new TProfile ("histo26", "N_{chg} vs the Distance R from Charged Jet1 - 80% of particles", 50, 0., 1., 0., 20.);
  fH26->SetStats(kTRUE);
  fH26->GetXaxis()->SetTitle("Distance R");
  fH26->GetYaxis()->SetTitle("<N_{chg}> in 0.02 bin");
  fH26->GetXaxis()->SetTitleColor(1);
  fH26->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH26);

  fH27 = new TProfile ("histo27", "N_{chg} vs the Distance R from Charged Jet1 - 80% of Pt", 50, 0., 1., 0., 20.);
  fH27->SetStats(kTRUE);
  fH27->GetXaxis()->SetTitle("Distance R");
  fH27->GetYaxis()->SetTitle("<N_{chg}> in 0.02 bin");
  fH27->GetXaxis()->SetTitleColor(1);
  fH27->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH27);

  fH28 = new TProfile ("histo28", "PT_{sum} vs the Distance R from Charged Jet1 - 80% of particles", 50, 0., 1., 0., 10.);
  fH28->SetStats(kTRUE);
  fH28->GetXaxis()->SetTitle("Distance R");
  fH28->GetYaxis()->SetTitle("<PT_{sum} (GeV/c)> in 0.02 bin");
  fH28->GetXaxis()->SetTitleColor(1);
  fH28->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH28);

  fH29 = new TProfile ("histo29", "PT_{sum} vs the Distance R from Charged Jet1 - 80% of Pt", 50, 0., 1., 0., 10.);
  fH29->SetStats(kTRUE);
  fH29->GetXaxis()->SetTitle("Distance R");
  fH29->GetYaxis()->SetTitle("<PT_{sum} (GeV/c)> in 0.02 bin");
  fH29->GetXaxis()->SetTitleColor(1);
  fH29->SetMarkerStyle(kFullCircle);
  fOutput->Add(fH29);


  // =========== Switch on Sumw2 for all histos ===========
  for (Int_t i=0; i < fOutput->GetEntries(); ++i)
    {
    TH1 *h1 = dynamic_cast<TH1*>(fOutput->At(i));
    if (h1) { h1->Sumw2(); continue; }

    TProfile *hprof1 = dynamic_cast<TProfile*>(fOutput->At(i));
    if (hprof1) { hprof1->Sumw2(); }
    }

  PostData ( 1, fOutput ); // Post data for ALL output slots > 0 here.
  }


//______________________________________________________________________________
Double_t AliAnalysisTaskEmcalJetCDF::Phi_mpi_pi (Double_t phi)
{ // (static function) returns phi angle in the interval [-PI,PI)
if ( TMath::IsNaN(phi) ) { gROOT->Error("AliAnalysisTaskEmcalJetCDF::Phi_mpi_pi","function called with NaN"); return phi; }

while (phi >= kPI) phi -= kTWOPI;
while (phi < -kPI) phi += kTWOPI;
return phi;
}

//________________________________________________________________________
Double_t AliAnalysisTaskEmcalJetCDF::DeltaR(const AliVParticle* part1, const AliVParticle* part2 ) const
{
// Helper function to calculate the distance between two jets or a jet and particle
  Double_t dPhi = part1->Phi() - part2->Phi();

  dPhi = Phi_mpi_pi (dPhi);

  Double_t dEta = part1->Eta() - part2->Eta();

  Double_t dR = TMath::Sqrt ( dPhi * dPhi + dEta * dEta );

  return dR;
}

//________________________________________________________________________
void AliAnalysisTaskEmcalJetCDF::Terminate ( Option_t* )
  {
  // Called once at the end of the analysis.
  }
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
