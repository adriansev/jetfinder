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

// Get the pointer to the existing analysis manager via the static access method.
//==============================================================================
AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
if ( !mgr ) { ::Error ( "InputData.C", "No analysis manager to connect to." ); return kFALSE; }

// Check the analysis type using the event handlers connected to the analysis manager.
//==============================================================================
AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
if ( !plugin ) { ::Error ( "InputData.C", "InputData :: plugin invalid or not found" ); return kFALSE; }

//  DEFINED INPUT DATA
TString datarun (label);
if ( datarun.IsNull() ) { ::Error ( "InputData :: no data input" ); return kFALSE; }
datarun.ToLower();

TString     kGridRunPattern = "000"; // important for leading zeroes!!
TString     run_list;
Printf ( "InputData :: data label = %s" , datarun.Data() );

// ##################################################################################

// FILTER_p-p_113_LHC11a: Standard AODs + deltas  pp 2.76 GeV
// runlist for lego dataset LHC11a_pass4_AOD (pass4_wSDD) ; Jets_EMC_pp train
// MC anchored lhc11a : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC11a
if ( datarun.EqualTo ( "pp_lhc11a" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2011/LHC11a" );
    plugin->SetDataPattern ( "*ESDs/pass4_with_SDD/AOD113/*AliAOD.root" );

    run_list = "146860 146859 146858 146856 146824 146817 146807 146806 146805 146804 146803 146802 146801 146748 146747 146746";
    }

// ##################################################################################

// AOD LHC10d; 7 TeV
// MC anchored 10d :http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC10d
// runlist for lego dataset LHC10d_pass4_AOD ; Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc10d" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2010/LHC10d" );
    plugin->SetDataPattern ( "*pass4/AOD172/*AliAOD.root" );

    TString list_lhc10d_AN_1 = "126432 126425 126424 126422 126409 126408 126407 126406 126405 126404 126403 126359 126352 126351 126350 126285 126284 ";
    TString list_lhc10d_AN_2 = "126283 126168 126167 126160 126158 126097 126090 126088 126082 126081 126078 126073 126008 126007 126004 125855 125851 ";
    TString list_lhc10d_AN_3 = "125850 125849 125848 125847 125844 125843 125842 125633 125632 125630 125628 125296 125295 125186 125156 125140 125139 ";
    TString list_lhc10d_AN_4 = "125134 125133 125101 125100 125097 125085 125083 125023 124751 122375 122374";
    run_list = list_lhc10d_AN_1 + list_lhc10d_AN_2 + list_lhc10d_AN_3 + list_lhc10d_AN_4;
    }

// https://alice.its.cern.ch/jira/browse/ALIROOT-5603
else if ( datarun.EqualTo ( "pp_lhc10d_14j4d" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2014/LHC14j4d" );
    plugin->SetDataPattern ( "*/AOD173/*/*AliAOD.root" );

    TString list_lhc10d_AN_1 = "126432 126425 126424 126422 126409 126408 126407 126406 126405 126404 126403 126359 126352 126351 126350 126285 126284 ";
    TString list_lhc10d_AN_2 = "126283 126168 126167 126160 126158 126097 126090 126088 126082 126081 126078 126073 126008 126007 126004 125855 125851 ";
    TString list_lhc10d_AN_3 = "125850 125849 125848 125847 125844 125843 125842 125633 125632 125630 125628 125296 125295 125186 125156 125140 125139 ";
    TString list_lhc10d_AN_4 = "125134 125133 125101 125100 125097 125085 125083 125023 124751 122375 122374";
    run_list = list_lhc10d_AN_1 + list_lhc10d_AN_2 + list_lhc10d_AN_3 + list_lhc10d_AN_4;
    }

// ##################################################################################

// AOD LHC10e; 7 TeV
// MC anchored 10e : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC10e
// runlist for lego dataset LHC10e_pass4_AOD ; Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc10e" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2010/LHC10e" );
    plugin->SetDataPattern ( "*pass4/AOD172/*AliAOD.root" );

    TString list_lhc10e_AN_1 = "130850 130848 130847 130844 130842 130840 130834 130799 130798 130795 130793 130704 130696 130628 130623 130621 130620 130609 130608 130526 ";
    TString list_lhc10e_AN_2 = "130524 130520 130519 130517 130481 130480 130479 130375 130360 130358 130356 130354 130342 130178 130172 130168 130158 130157 130149 129983 ";
    TString list_lhc10e_AN_3 = "129966 129962 129961 129960 129744 129742 129738 129736 129735 129734 129729 129726 129725 129723 129666 129659 129653 129652 129651 129650 ";
    TString list_lhc10e_AN_4 = "129647 129641 129639 129599 129587 129586 129540 129536 129528 129527 129525 129524 129523 129521 129520 129519 129516 129515 129514 129513 ";
    TString list_lhc10e_AN_5 = "129512 129042 128913 128855 128853 128850 128843 128836 128835 128834 128833 128824 128823 128820 128819 128778 128777 128678 128677 128621 ";
    TString list_lhc10e_AN_6 = "128615 128611 128609 128605 128596 128594 128592 128582 128506 128505 128504 128503 128498 128495 128494 128486 128452 128366";
    run_list = list_lhc10e_AN_1 + list_lhc10e_AN_2 + list_lhc10e_AN_3 + list_lhc10e_AN_4 + list_lhc10e_AN_5 + list_lhc10e_AN_6;
    }

// ##################################################################################
// MC anchored lhc13g : http://alimonitor.cern.ch/job_details.jsp?jt_description=LHC13g
// AOD LHC13g; 2.76 TeV
// LHC13g_AOD155 Jets_EMC_pp train
else if ( datarun.EqualTo ( "pp_lhc13g" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2013/LHC13g" );
    plugin->SetDataPattern ( "*pass1/AOD155/*AliAOD.root" );

    run_list = "197471 197496 197497 197499 197500 197501 197529 197531 197553 197555 197583 197584 197608 197609 197610 197611 197618 197643 197669";
    }

// LHC12a15f pp, Jet-Jet Pythia6, 0.5T, 7000 GeV, LHC11d anchors, 11 Pt bins, ID #301
// jdl : https://alimonitor.cern.ch/jobs/jdl.jsp?pid=200443474
else if ( datarun.EqualTo ( "pp_lhc12a15f" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2012/LHC12a15f" );
    plugin->SetDataPattern ( "/*/*/AliAOD.root" );

    run_list = "158285 159582";
    }

// LHC14b7 p-p, Pythia6 simulation, LHC11c and LHC11d anchors, 7TeV (RAW OCDB), ALIROOT-5345
else if ( datarun.EqualTo ( "pp_lhc14b7" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2014/LHC14b7" );
    plugin->SetDataPattern ( "/*/*/AliAOD.root" );

    run_list = "159599 159582 157818 157569 157203 154383 154211";
    }

// AOD LHC16q; p-Pb 5.02 TeV
else if ( datarun.EqualTo ( "pp_lhc16q" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16q" );
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    // https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16q
    TString list_lhc16q_1 = "265525 265521 265501 265500 265499 265427 265426 265425 265424 265421 265420 265419 265388 265387 265384 265383 265378 265344 265343 265342 265339 265338 265336 265335 265334 265332 265309 ";
    TString list_lhc16q_2 = "265435 265422";
    run_list = list_lhc16q_1 + list_lhc16q_2;
    }

// AOD LHC16r; p-Pb 8.16 TeV
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16r
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16r/RunList_LHC16r_pass1_CentralBarrelTracking_calo_20170202_v0.txt
else if ( datarun.EqualTo ( "pp_lhc16r" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16r" );
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    run_list = "266318 266317 266316 266208 266197 266196 266187 265744";
    }

// AOD LHC16s; Pb-p 8.16 TeV
// https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16s
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16s/RunList_LHC16s_pass1_CentralBarrelTracking_calo_20170202_v0.txt
else if ( datarun.EqualTo ( "pp_lhc16s" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16s" );
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    run_list = "267110 267081 267077 267072 267070 266998 266997 266994 266993 266944 266886 266885 266883 266882 266437";
    }

// AOD LHC16t; p-Pb 5.02 TeV
else if ( datarun.EqualTo ( "pp_lhc16t" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2016/LHC16t" );
    plugin->SetDataPattern ( "*pass1_CENT_wSDD/AOD190/*AliAOD.root" );

    // https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList16t
    // https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList16t/RunList_LHC16t_pass1_CentralBarrelTracking_calo_20170202_v0.txt
    run_list = "267166 267165 267164 267163";
    }

// LHC18f3b_cent_2 p-Pb, 8.16 TeV - DPMJET production anchored to LHC16r, CENT, ALIROOT-7792
else if ( datarun.EqualTo ( "lhc18f3bcent" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC18f3b_cent_2" );
    plugin->SetDataPattern ( "*/*AliAOD.root" );

    TString list_lhc18f3bcent_1 = "265594 265596 265607 265697 265698 265700 265701 265705 265709 265713 265714 265741 265742 265744 265746 ";
    TString list_lhc18f3bcent_2 = "265754 265756 265788 265789 265795 265797 266034 266074 266076 266081 266083 266084 266085 266086 266117 ";
    TString list_lhc18f3bcent_3 = "266187 266189 266190 266193 266196 266197 266208 266296 266299 266300 266304 266305 266316 266317 266318 ";
    run_list = list_lhc18f3bcent_1 + list_lhc18f3bcent_2 + list_lhc18f3bcent_3;
    }

// LHC18m pp 13 TeV
// https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList18m
// https://alimonitor.cern.ch/configuration/index.jsp?partition=LHC18m
// https://twiki.cern.ch/twiki/pub/ALICE/AliDPGRunList18m/RunList_LHC18m_pass2_CentralBarrelTracking.txt
// https://alimonitor.cern.ch/prod/jobs.jsp?t=23891
else if ( datarun.EqualTo ( "pp_lhc18m_emc_01" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2018/LHC18m" );
    plugin->SetDataPattern ( "pass2/AOD/*/AliAOD.root" );

    run_list = "290459 291375 291481 292430 291263 292398 291484 292696 290645 291286 290979 291420 290943 291397";
    }

else if ( datarun.EqualTo ( "pp_lhc18m_emc_02" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2018/LHC18m" );
    plugin->SetDataPattern ( "pass2/AOD/*/AliAOD.root" );

    TString list01 = "292811 290660 291003 291284 292693 290975 290323 292523 292804 291116 292554 290590 291482 290425 ";
    TString list02 = "292456 291209 292495 292497 290469 292429 291360 292405 292839 292831 291004 292739 292500 292457 291041 291453 291456 ";
    run_list = list01 + list02;
    }

else if ( datarun.EqualTo ( "pp_lhc18m_emc_03" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2018/LHC18m" );
    plugin->SetDataPattern ( "pass2/AOD/*/AliAOD.root" );

    TString list01 = "290500 291188 290426 290588 291069 290399 292750 291037 292695 290327 290948 291377 290974 292406 ";
    TString list02 = "291282 292832 292560 291285 291065 291100 290980 290401 290458 290544 290456 290615 291111 291485 ";
    TString list03 = "290375 292698 290613 290627 292748 290632 290538 291110 292836 291446 291361 291417 291424 290941 ";
    TString list04 = "290944 290614 292586 291402 290412 290411 292810 291416 292553 292432 290612 291257 290427 291265 ";
    TString list05 = "290550 290499 292809 291419 292584 290423 292526 290976 291143 292557 291005 291266 291283 292747 ";
    TString list06 = "290374 291399 292803 290540 292428 291240 290350 292737 291457 292521 292701 290539 291101 292524 ";
    TString list07 = "291447 290553 292434 290665 292704 291035 292563 291002 292397 290549 292496 291093 292559 291362 ";
    run_list = list01 + list02 + list03 + list04 + list05 + list06 + list07;
    }

else if ( datarun.EqualTo ( "pp_lhc18m_barr_01" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2018/LHC18m" );
    plugin->SetDataPattern ( "pass2/AOD/*/AliAOD.root" );

    TString list01 = "290404 292040 291066 291948 290848 290687 292240 292273 292115 292062 292160 ";
    TString list02 = "291953 290501 290254 290860 292107 291661 291756 291697 291665 292298 290886 ";
    run_list = list01 + list02;
    }

else if ( datarun.EqualTo ( "pp_lhc18m_barr_02" ) ) {
    plugin->SetGridDataDir ( "/alice/data/2018/LHC18m" );
    plugin->SetDataPattern ( "pass2/AOD/*/AliAOD.root" );

    TString list01 = "290843 290376 290689 292218 291760 290294 292269 292140 290253 291762 292192 ";
    TString list02 = "291006 292162 292164 290790 290297 290895 291795 292114 290223 290293 290862 ";
    TString list03 = "292241 291796 290888 292167 291690 292081 292067 292161 291702 291768 290841 ";
    TString list04 = "291766 292108 291977 291946 291803 290894 292265 292744 291942 292242 290222 ";
    TString list05 = "292077 292109 291614 290935 292754 292166 291945 291451 291363 292834 291692 ";
    TString list06 = "291400 292752 292270 292758 290787 291626 291694 290766 291615 290467 292061 ";
    TString list07 = "291698 292163 291729 292106 292274 291657 290932 290853 291622 291618 291944 ";
    TString list08 = "292080 291982 292012 292168 292060 290887 290846 291755 291590 292075 291976 291706 291769 291943 291624 ";
    run_list = list01 + list02 + list03 + list04 + list05 + list06 + list07 + list08;
    }

// LHC18m pass2 anchored
// https://alice.its.cern.ch/jira/browse/ALIROOT-8628
//  pp, 13 TeV - General-purpose Monte Carlo production anchored to LHC18m pass2, store BKG, ALIROOT-8628
else if ( datarun.EqualTo ( "pp_lhc21a5a_emc_01" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC21a5a" );
    plugin->SetDataPattern ( "AOD/*/AliAOD.root" );

    run_list = "290459 291375 291481 292430 291263 292398 291484 292696 290645 291286 290979 291420 290943 291397";
    }

else if ( datarun.EqualTo ( "pp_lhc21a5a_emc_02" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC21a5a" );
    plugin->SetDataPattern ( "AOD/*/AliAOD.root" );

    TString list01 = "292811 290660 291003 291284 292693 290975 290323 292523 292804 291116 292554 290590 291482 290425 ";
    TString list02 = "292456 291209 292495 292497 290469 292429 291360 292405 292839 292831 291004 292739 292500 292457 291041 291453 291456 ";
    run_list = list01 + list02;
    }

else if ( datarun.EqualTo ( "pp_lhc21a5a_emc_03" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC21a5a" );
    plugin->SetDataPattern ( "AOD/*/AliAOD.root" );

    TString list01 = "290500 291188 290426 290588 291069 290399 292750 291037 292695 290327 290948 291377 290974 292406 ";
    TString list02 = "291282 292832 292560 291285 291065 291100 290980 290401 290458 290544 290456 290615 291111 291485 ";
    TString list03 = "290375 292698 290613 290627 292748 290632 290538 291110 292836 291446 291361 291417 291424 290941 ";
    TString list04 = "290944 290614 292586 291402 290412 290411 292810 291416 292553 292432 290612 291257 290427 291265 ";
    TString list05 = "290550 290499 292809 291419 292584 290423 292526 290976 291143 292557 291005 291266 291283 292747 ";
    TString list06 = "290374 291399 292803 290540 292428 291240 290350 292737 291457 292521 292701 290539 291101 292524 ";
    TString list07 = "291447 290553 292434 290665 292704 291035 292563 291002 292397 290549 292496 291093 292559 291362 ";
    run_list = list01 + list02 + list03 + list04 + list05 + list06 + list07;
    }

else if ( datarun.EqualTo ( "pp_lhc21a5a_barr_01" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC21a5a" );
    plugin->SetDataPattern ( "AOD/*/AliAOD.root" );

    TString list01 = "290404 292040 291066 291948 290848 290687 292240 292273 292115 292062 292160 ";
    TString list02 = "291953 290501 290254 290860 292107 291661 291756 291697 291665 292298 290886 ";
    run_list = list01 + list02;
    }

else if ( datarun.EqualTo ( "pp_lhc21a5a_barr_02" ) ) {
    plugin->SetGridDataDir ( "/alice/sim/2018/LHC21a5a" );
    plugin->SetDataPattern ( "AOD/*/AliAOD.root" );

    TString list01 = "290843 290376 290689 292218 291760 290294 292269 292140 290253 291762 292192 ";
    TString list02 = "291006 292162 292164 290790 290297 290895 291795 292114 290223 290293 290862 ";
    TString list03 = "292241 291796 290888 292167 291690 292081 292067 292161 291702 291768 290841 ";
    TString list04 = "291766 292108 291977 291946 291803 290894 292265 292744 291942 292242 290222 ";
    TString list05 = "292077 292109 291614 290935 292754 292166 291945 291451 291363 292834 291692 ";
    TString list06 = "291400 292752 292270 292758 290787 291626 291694 290766 291615 290467 292061 ";
    TString list07 = "291698 292163 291729 292106 292274 291657 290932 290853 291622 291618 291944 ";
    TString list08 = "292080 291982 292012 292168 292060 290887 290846 291755 291590 292075 291976 291706 291769 291943 291624 ";
    run_list = list01 + list02 + list03 + list04 + list05 + list06 + list07 + list08;
    }

// ##################################################################################
else {
    Printf ( "InputData :: NO DEFINED INPUT DATA RECOGNIZED !!! = %s", datarun.Data() );
    return kFALSE;
    }

TString grid_data_dir = plugin->GetGridDataDir()
if grid_data_dir.Contains("/alice/data/") { plugin->SetRunPrefix ( kGridRunPattern.Data() ); }

plugin->AddRunList(run_list.Data());
return kTRUE;
}
// kate: indent-mode none; indent-width 2; replace-tabs on;
