#if !defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

#include "Rtypes.h"
#include "TError.h"
#include "TString.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"

#endif

//______________________________________________________________________________
// function to add specific data to be processed
Bool_t InputData ( const char* label ) {
TString datarun (label);

//______________________________________________________________________________
//  DEFINED INPUT DATA
if ( datarun.IsNull() ) { printf ( "InputData :: no data input" ); return kFALSE; }

// Get the pointer to the existing analysis manager via the static access method.
//==============================================================================
AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
if ( !mgr ) { ::Error ( "InputData.C", "No analysis manager to connect to." ); return kFALSE; }

// Check the analysis type using the event handlers connected to the analysis manager.
//==============================================================================
AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
if ( !plugin ) { ::Error ( "InputData.C", "InputData :: plugin invalid" ); return kFALSE; }

TString     kGridRunPattern = "000"; // important for leading zeroes!!
Printf ( "data = %s" , datarun.Data() );

// ##################################################################################

// FILTER_p-p_113_LHC11a: Standard AODs + deltas  pp 2.76 GeV
// runlist for lego dataset LHC11a_pass4_AOD (pass4_wSDD) ; Jets_EMC_pp train
// MC anchored lhc11a : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC11a
if ( datarun.EqualTo ( "pp_lhc11a_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2011/LHC11a" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*ESDs/pass4_with_SDD/AOD113/*AliAOD.root" );

    plugin->AddRunList ("146860 146859 146858 146856 146824 146817 146807 146806 146805 146804 146803 146802 146801 146748 146747 146746");
    return kTRUE;
    }

// ##################################################################################

// AOD LHC10d; 7 TeV
// MC anchored 10d :http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC10d
// runlist for lego dataset LHC10d_pass4_AOD ; Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc10d_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2010/LHC10d" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass4/AOD172/*AliAOD.root" );

    TString list_lhc10d_AN_1 = "126432 126425 126424 126422 126409 126408 126407 126406 126405 126404 126403 126359 126352 126351 126350 126285 126284";
    TString list_lhc10d_AN_2 = "126283 126168 126167 126160 126158 126097 126090 126088 126082 126081 126078 126073 126008 126007 126004 125855 125851";
    TString list_lhc10d_AN_3 = "125850 125849 125848 125847 125844 125843 125842 125633 125632 125630 125628 125296 125295 125186 125156 125140 125139";
    TString list_lhc10d_AN_4 = "125134 125133 125101 125100 125097 125085 125083 125023 124751 122375 122374";

    plugin->AddRunList ( list_lhc10d_AN_1.Data() );
    plugin->AddRunList ( list_lhc10d_AN_2.Data() );
    plugin->AddRunList ( list_lhc10d_AN_3.Data() );
    plugin->AddRunList ( list_lhc10d_AN_4.Data() );
    return kTRUE;
    }

// ##################################################################################

// AOD LHC10e; 7 TeV
// MC anchored 10e : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC10e
// runlist for lego dataset LHC10e_pass4_AOD ; Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc10e_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2010/LHC10e" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass4/AOD172/*AliAOD.root" );

    TString list_lhc10e_AN_1 = "130850 130848 130847 130844 130842 130840 130834 130799 130798 130795 130793 130704 130696 130628 130623 130621 130620 130609 130608 130526";
    TString list_lhc10e_AN_2 = "130524 130520 130519 130517 130481 130480 130479 130375 130360 130358 130356 130354 130342 130178 130172 130168 130158 130157 130149 129983";
    TString list_lhc10e_AN_3 = "129966 129962 129961 129960 129744 129742 129738 129736 129735 129734 129729 129726 129725 129723 129666 129659 129653 129652 129651 129650";
    TString list_lhc10e_AN_4 = "129647 129641 129639 129599 129587 129586 129540 129536 129528 129527 129525 129524 129523 129521 129520 129519 129516 129515 129514 129513";
    TString list_lhc10e_AN_5 = "129512 129042 128913 128855 128853 128850 128843 128836 128835 128834 128833 128824 128823 128820 128819 128778 128777 128678 128677 128621";
    TString list_lhc10e_AN_6 = "128615 128611 128609 128605 128596 128594 128592 128582 128506 128505 128504 128503 128498 128495 128494 128486 128452 128366";

    plugin->AddRunList ( list_lhc10e_AN_1.Data() );
    plugin->AddRunList ( list_lhc10e_AN_2.Data() );
    plugin->AddRunList ( list_lhc10e_AN_3.Data() );
    plugin->AddRunList ( list_lhc10e_AN_4.Data() );
    plugin->AddRunList ( list_lhc10e_AN_5.Data() );
    plugin->AddRunList ( list_lhc10e_AN_6.Data() );
    return kTRUE;
    }

// ##################################################################################
// MC anchored lhc13g : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC13g
// AOD LHC13g; 2.76 TeV
// LHC13g_AOD155 Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc13g_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2013/LHC13g" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass1/AOD155/*AliAOD.root" );

    plugin->AddRunList ( "197471 197496 197497 197499 197500 197501 197529 197531 197553 197555 197583 197584 197608 197609 197610 197611 197618 197643 197669" );
    return kTRUE;
    }

// LHC12a15f pp, Jet-Jet Pythia6, 0.5T, 7000 GeV, LHC11d anchors, 11 Pt bins, ID #301
// jdl : https://alimonitor.cern.ch/jobs/jdl.jsp?pid=200443474
else if ( datarun.EqualTo ( "pp_lhc12a15f" ) ) {

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/sim/2012/LHC12a15f" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "/*/*/AliAOD.root" );

    TString list_lhc12a15f = "158285 159582";

    plugin->AddRunList ( list_lhc12a15f.Data() );
    return kTRUE;
    }

// LHC14b7 p-p, Pythia6 simulation, LHC11c and LHC11d anchors, 7TeV (RAW OCDB), ALIROOT-5345
else if ( datarun.EqualTo ( "pp_lhc14b7" ) ) {
    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/sim/2014/LHC14b7" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "/*/*/AliAOD.root" );

    TString list_lhc14b7 = "159599 159582 157818 157569 157203 154383 154211";

    plugin->AddRunList ( list_lhc14b7.Data() );
    return kTRUE;
    }

// AOD LHC16q; p-Pb 5.02 TeV
else if ( datarun.EqualTo ( "pp_lhc16q_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16q" );
// /alice/data/2016/LHC16q/000265521/pass1_CENT_wSDD/AOD190

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    // https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16q
    TString list_lhc16q_1 = "265525 265521 265501 265500 265499 265427 265426 265425 265424 265421 265420 265419 265388 265387 265384 265383 265378 265344 265343 265342 265339 265338 265336 265335 265334 265332 265309";
    TString list_lhc16q_2 = "265435 265422";

    plugin->AddRunList ( list_lhc16q_1.Data() );
//           plugin->AddRunList ( list_lhc16q_2.Data() ); // present in train dataset LHC16q_pass1_wSDD but not in the list AliDPGRunList16q/RunList_LHC16q_pass1_CentralBarrelTracking_calo_20171129_v2.txt
    return kTRUE;
    }

// AOD LHC16r; p-Pb 8.16 TeV
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16r
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16r/RunList_LHC16r_pass1_CentralBarrelTracking_calo_20170202_v0.txt
else if ( datarun.EqualTo ( "pp_lhc16r_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16r" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    TString list_lhc16r_1 = "266318 266317 266316 266208 266197 266196 266187 265744";

    plugin->AddRunList ( list_lhc16r_1.Data() );
    return kTRUE;
    }

// AOD LHC16s; Pb-p 8.16 TeV
// https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16s
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16s/RunList_LHC16s_pass1_CentralBarrelTracking_calo_20170202_v0.txt
else if ( datarun.EqualTo ( "pp_lhc16s_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16s" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    TString list_lhc16s_1 = "267110 267081 267077 267072 267070 266998 266997 266994 266993 266944 266886 266885 266883 266882 266437";

    plugin->AddRunList ( list_lhc16s_1.Data() );
    return kTRUE;
    }

// AOD LHC16t; p-Pb 5.02 TeV
else if ( datarun.EqualTo ( "pp_lhc16t_aod" ) ) {
    plugin->SetRunPrefix ( kGridRunPattern.Data() );

    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16t" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    // https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16t
    // https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16t/RunList_LHC16t_pass1_CentralBarrelTracking_calo_20170202_v0.txt
    TString list_lhc16t_1 = "267166 267165 267164 267163";

    plugin->AddRunList ( list_lhc16t_1.Data() );
    return kTRUE;
    }

// LHC18f3b_cent_2	p-Pb, 8.16 TeV - DPMJET production anchored to LHC16r, CENT, ALIROOT-7792
else if ( datarun.EqualTo ( "lhc18f3bcent" ) ) {
    // AliEn data production directory
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC18f3b_cent_2" );

    // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
    plugin->SetDataPattern ( "*/*AliAOD.root" );

    TString list_lhc18f3bcent_1 = "265594 265596 265607 265697 265698 265700 265701 265705 265709 265713 265714 265741 265742 265744 265746";
    TString list_lhc18f3bcent_2 = "265754 265756 265788 265789 265795 265797 266034 266074 266076 266081 266083 266084 266085 266086 266117";
    TString list_lhc18f3bcent_3 = "266187 266189 266190 266193 266196 266197 266208 266296 266299 266300 266304 266305 266316 266317 266318";

    TString list_lhc18f3bcent_all = list_lhc18f3bcent_1 + list_lhc18f3bcent_2 + list_lhc18f3bcent_3;

    plugin->AddRunList ( list_lhc18f3bcent_all.Data() );
    return kTRUE;
    }

// ##################################################################################
else
  { Printf ( "InputData :: NO DEFINED INPUT DATA RECOGNIZED !!! = %s", datarun.Data() ); return kFALSE; }

}
// kate: indent-mode none; indent-width 2; replace-tabs on;
