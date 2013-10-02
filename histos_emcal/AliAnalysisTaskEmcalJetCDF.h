#ifndef ALIANALYSISTASKEMCALJETSAMPLE_H
#define ALIANALYSISTASKEMCALJETSAMPLE_H

// $Id: AliAnalysisTaskEmcalJetSample.h 60393 2013-01-20 10:05:07Z loizides $

#include <cstdio>

#include <Riostream.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TList.h>
#include <TMath.h>
#include <TProfile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>


#include "AliLog.h"
#include "AliStack.h"
#include "AliRhoParameter.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisTaskEmcalJet.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisHelperJetTasks.h"

#include "AliJetHistos.h"
#include "AliESD.h"
#include "AliESDEvent.h"

#include "AliVEvent.h"
#include "AliMCEvent.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"

#include "AliVCluster.h"
#include "AliVTrack.h"
#include "AliEmcalJet.h"

#include "AliInputEventHandler.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliAODJet.h"
#include "AliAODJetEventBackground.h"
#include "AliAODExtension.h"
#include "AliMCEventHandler.h"
#include "AliAODInputHandler.h"
#include "AliAODJetEventBackground.h"
#include "AliAODMCParticle.h"

class TObjArray;
class TRefArray;
class TClonesArray;
class TList;
class TH1;
class TH1D;
class TH1F;
class TH2D;
class TH2F;
class TH2;
class TProfile;

// class AliESDEvent;
// class AliAODEvent;
// class AliAODExtension;
// class AliAODJet;
// class AliGenPythiaEventHeader;
// class AliAODJetEventBackground;

class AliAnalysisTaskEmcalJetCDF : public AliAnalysisTaskEmcalJet
  {
  public:

    AliAnalysisTaskEmcalJetCDF();
    AliAnalysisTaskEmcalJetCDF ( const char* name );
    virtual ~AliAnalysisTaskEmcalJetCDF();

    void                        UserCreateOutputObjects();
    void                        Terminate ( Option_t* option );

    Double_t                    Phi_mpi_pi (Double_t phi);
    Double_t                    DeltaR (const AliVParticle* part1, const AliVParticle* part2 ) const;

  protected:
    Bool_t                      FillHistograms()   ;
    Bool_t                      Run()              ;

    // Histograms    ( are owned by fListOfHistos TList )
    TH1F*       fH1; //!  Pt distribution of jets
    TH1F*       fH2; //!  Eta distribution of jets
    TH1F*       fH3; //!  Phi distribution of jets
    TH1F*       fH4; //!  Multiplicity of jets // 1 unit of multiplicity /bin
    TH1F*       fH5; //!  Distribution of jets in events
    TH1F*       fH6; //!  Jet1 Charged Multiplicity Distribution (Fig 5)- MUST BE SCALED WITH ENTRIES ( ->Scale(fH6->GetEntries()) )
    TProfile*   fH7; //!  N_{chg}(jet1) vs P_{T}(charged jet1) (Fig 4)
    TH1F*       fH8; //!  Charge momentum distribution for leading jet (fragmentation function)(Fig 10-12)
    TProfile*   fH9; //!  N_{chg} vs the Azimuthal Angle from Charged Jet1  (Fig 15,17,19)
    TProfile*  fH10; //!  P_{T} sum vs the Azimuthal Angle from Charged Jet1 (Fig 16,18,20)
    TH1F*      fH20; //!  Distribution of R in leading jet

    TProfile*  fH21;           //!  N_{chg}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 13)
    TProfile*  fH21Toward;     //!  N_{chg}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 21,23,24)
    TProfile*  fH21Transverse; //!  N_{chg}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 21,28,30,32,33,34)
    TProfile*  fH21Away;       //!  N_{chg}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 21,25,27)

    TProfile*  fH22;           //!  PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 22)
    TProfile*  fH22Toward;     //!  PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 22)
    TProfile*  fH22Transverse; //!  PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 22,29,31)
    TProfile*  fH22Away;       //!  PT_{sum}(in the event - including jet1) vs P_{T}(charged jet1) (Fig 22,26)

    TH1F*      fH23;           //!  TOTAL Pt Distribution of charged particles
    TH1F*      fH23jet1;       //!  jet1 Pt Distribution of charged particles
    TH1F*      fH23Toward;     //!  'Toward' Pt Distribution of charged particles
    TH1F*      fH23Transverse; //!  'Transverse' Pt Distribution of charged particles  (Fig 37,38,39,40,41)
    TH1F*      fH23Away;       //!  'Away' Pt Distribution of charged particles

    TProfile*  fH24;           //!  Jet1 Size vs P_{T}(charged jet1) - 80% of particles  (Fig 6)
    TProfile*  fH25;           //!  Jet1 Size vs P_{T}(charged jet1) - 80% of Pt  (Fig 6)
    TProfile*  fH26;           //!  N_{chg} vs the Distance R from Charged Jet1 - 80% of particles   (Fig 8)
    TProfile*  fH27;           //!  N_{chg} vs the Distance R from Charged Jet1 - 80% of Pt  (Fig 8)
    TProfile*  fH28;           //!  PT_{sum} vs the Distance R from Charged Jet1 - 80% of particles   (Fig 9)
    TProfile*  fH29;           //!  PT_{sum} vs the Distance R from Charged Jet1 - 80% of Pt  (Fig 9)

  private:
    AliAnalysisTaskEmcalJetCDF ( const AliAnalysisTaskEmcalJetCDF& );         // not implemented
    AliAnalysisTaskEmcalJetCDF& operator= ( const AliAnalysisTaskEmcalJetCDF& ); // not implemented

    ClassDef ( AliAnalysisTaskEmcalJetCDF, 1 ) // jet sample analysis task
  };
#endif
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
