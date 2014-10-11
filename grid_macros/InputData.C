#ifndef __CINT__
#include "Rtypes.h"
#include "TError.h"
#include "TString.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisAlien.h"
#endif

//______________________________________________________________________________
// function to add specific data to be processed
Bool_t InputData ( const TString& datarun )
    {
    // Get the pointer to the existing analysis manager via the static access method.
    //==============================================================================
    AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
    if ( !mgr ) { ::Error ( "InputData.C", "No analysis manager to connect to." ); return kFALSE; }

    // Check the analysis type using the event handlers connected to the analysis manager.
    //==============================================================================
    AliAnalysisAlien* plugin =  dynamic_cast <AliAnalysisAlien*> ( mgr->GetGridHandler() );
    if ( !plugin ) { ::Error ( "InputData.C", "InputData :: plugin invalid" ); return kFALSE; }

    //______________________________________________________________________________
    //  DEFINED INPUT DATA
    if ( datarun.IsNull() ) { printf ( "InputData :: no data input" ); return kFALSE; }

    TString     kGridRunPattern = "%09d"; // important for leading zeroes!!

    Printf ( "data = %s" , datarun.Data() );

// ##################################################################################

// FILTER_p-p_113_LHC11a: Standard AODs + deltas  pp 2.76 GeV
// https://aliceinfo.cern.ch/Notes/node/46
// https://aliceinfo.cern.ch/Notes/node/128
    if ( datarun.EqualTo ( "pp_lhc11a_aod_AN" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2011/LHC11a" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/ESDs/pass4_without_SDD/AOD113/*AliAOD.root" );

            TString list_lhc11a_AN_1 = "146746 146747 146748 146801 146802 146803 146804 146805 146806 146807 146817 146824 146856 146858 146859 146860";

            plugin->AddRunList (list_lhc11a_AN_1.Data());
            return kTRUE;
            }

    else if ( datarun.EqualTo ( "pp_lhc11a_aod_SDD_ALL" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2011/LHC11a" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*ESDs/pass4_with_SDD/AOD113/*AliAOD.root" );

            TString list_lhc11a_AN_1 = "146805 146824 146804 146858 146859 146802 146747 146806 146856 146689 146801 146860 146817 146857 146748 146688 146746 146686 146807 146803";

            plugin->AddRunList (list_lhc11a_AN_1.Data());
            return kTRUE;
            }

    else if ( datarun.EqualTo ( "pp_lhc11a_aod_NOSDD_ALL" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2011/LHC11a" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*ESDs/pass4_without_SDD/AOD113/*AliAOD.root" );

            TString list_lhc11a_AN_1 = "146805 146824 146804 146858 146859 146802 146747 146806 146856 146689 146801 146817 146860 146857 146748 146688 146746 146686 146807 146803";

            plugin->AddRunList (list_lhc11a_AN_1.Data());
            return kTRUE;
            }

// ##################################################################################

// AOD LHC10d; 7 TeV
// https://aliceinfo.cern.ch/Notes/node/128
// https://aliceinfo.cern.ch/Notes/node/205
    else if ( datarun.EqualTo ( "pp_lhc10d_aod_AN" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2010/LHC10d" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/ESDs/pass2/AOD147/*AliAOD.root" );

            TString list_lhc10d_AN_1 = "122374 125023 125085 125097 125100 125101 125134 125139 125140 125156 125186 125296 125630 125632 125633 125842";
            TString list_lhc10d_AN_2 = "125843 125844 125847 125848 125849 125850 125851 125855 126004 126007 126008 126073 126078 126081 126082 126088";
            TString list_lhc10d_AN_3 = "126090 126097 126158 126284 126285 126351 126352 126359 126403 126404 126405 126406 126407 126408 126409 126422";
            TString list_lhc10d_AN_4 = "126424 126425 126432 126437";

            plugin->AddRunList ( list_lhc10d_AN_1.Data() );
            plugin->AddRunList ( list_lhc10d_AN_2.Data() );
            plugin->AddRunList ( list_lhc10d_AN_3.Data() );
            plugin->AddRunList ( list_lhc10d_AN_4.Data() );
            return kTRUE;
            }

    else if ( datarun.EqualTo ( "pp_lhc10d_aod_ALL" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );
            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2010/LHC10d" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/ESDs/pass2/AOD147/*AliAOD.root" );

            TString list_lhc10d_ALL_1 = "126424 126284 126158 126090 126088 126432 125139 126404 125296 126082 126078 125085 126407 126351";
            TString list_lhc10d_ALL_2 = "126406 124383 126007 122374 126422 125097 124380 125134 124388 125628 124374 125632 126073 124606";
            TString list_lhc10d_ALL_3 = "125855 125842 124378 126408 125850 125140 126352 125101 126168 125295 125849 125630 126160 126177";
            TString list_lhc10d_ALL_4 = "126081 126437 125851 125186 126409 126425 126283 126097 126167 126359 124600 124360 124385 124358";
            TString list_lhc10d_ALL_5 = "126008 124381 124371 125633 122375 125848 125847 125131 126086 125133 126004 124605 124367 124608";
            TString list_lhc10d_ALL_6 = "124604 126403 125843 124607 124355 126162 126087 125100 124751 126405 126285 124187 124362 125083";
            TString list_lhc10d_ALL_7 = "126350 125156 125844 124603 124183 125023 124359 124186 125292";

            plugin->AddRunList ( list_lhc10d_ALL_1.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_2.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_3.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_4.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_5.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_6.Data() );
            plugin->AddRunList ( list_lhc10d_ALL_7.Data() );
            return kTRUE;
            }

// ##################################################################################

// AOD LHC10e; 7 TeV
// https://aliceinfo.cern.ch/Notes/node/128
// https://aliceinfo.cern.ch/Notes/node/205
    else if ( datarun.EqualTo ( "pp_lhc10e_aod_AN" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2010/LHC10e" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/ESDs/pass2/AOD147/*AliAOD.root" );

            TString list_lhc10e_AN_1 = "127712 127714 127718 127719 128495 128498 128503 128504 128505 128507 128605 128615 128621 128677";
            TString list_lhc10e_AN_2 = "128678 128777 128778 128820 128823 128824 128833 128835 128836 128843 128850 128853 128855 128913";
            TString list_lhc10e_AN_3 = "129512 129513 129514 129599 129639 129641 129652 129653 129654 129659 129667 129960 129961 130157";
            TString list_lhc10e_AN_4 = "130158 130179 130519 130601 130608 130696 130704 130793 130798 130799 130834 130840";

            plugin->AddRunList ( list_lhc10e_AN_1.Data() );
            plugin->AddRunList ( list_lhc10e_AN_2.Data() );
            plugin->AddRunList ( list_lhc10e_AN_3.Data() );
            plugin->AddRunList ( list_lhc10e_AN_4.Data() );
            return kTRUE;
            }

    else if ( datarun.EqualTo ( "pp_lhc10e_aod_ALL" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );
            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2010/LHC10e" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/ESDs/pass2/AOD147/*AliAOD.root" );

            TString list_lhc10e_ALL_1 = "130795 128503 129654 130517 128189 129961 128609 129983 130358 128452 128504 130369 130519 127942";
            TString list_lhc10e_ALL_2 = "130623 130704 130803 130178 130480 130158 130157 128180 130149 127718 129540 128824 130793 128486";
            TString list_lhc10e_ALL_3 = "129667 130628 130356 128186 127937 128182 128836 130696 130799 127936 129736 129735 130343 128777";
            TString list_lhc10e_ALL_4 = "127822 128855 128185 128833 128260 129641 130526 129962 130834 128853 128366 129520 129523 129738";
            TString list_lhc10e_ALL_5 = "129729 129653 128615 127933 127935 130148 130802 128494 128590 128191 130620 129960 130172 128778";
            TString list_lhc10e_ALL_6 = "128820 129666 128843 127712 129652 129528 128911 129742 129587 129744 129726 128913 130360 129647";
            TString list_lhc10e_ALL_7 = "129514 127714 129586 128175 129959 128611 127941 129650 130840 128605 128507 128192 128582 129513";
            TString list_lhc10e_ALL_8 = "128495 128483 128823 129512 128678 130798 129599 129527 130804 129659 129041 130342 130179 128834";
            TString list_lhc10e_ALL_9 = "127932 129723 127813 129725 130831 130354 130348 130609 129639 128677 128835 128581 129748 129734";
            TString list_lhc10e_ALL_10 = "128813 130168 128910 129529 129508 129651 128849 129526 128589 128262 130640 129745 130353 129760";
            TString list_lhc10e_ALL_11 = "130365 129747 128359 129750 129649 128256 128776 130627 127820 129598 129763 129655 128190 128817";
            TString list_lhc10e_ALL_12 = "129597 128357 129510 129522 128183 129731 129541 129648 129665 130833 127723 127940 130156 128610";
            TString list_lhc10e_ALL_13 = "128814 128050 128053 128818 130170 127715";

            plugin->AddRunList ( list_lhc10e_ALL_1.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_2.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_3.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_4.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_5.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_6.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_7.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_8.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_9.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_10.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_11.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_12.Data() );
            plugin->AddRunList ( list_lhc10e_ALL_13.Data() );
            return kTRUE;
            }


// ##################################################################################

// AOD LHC13g; 2.76 TeV
    else if ( datarun.EqualTo ( "pp_lhc13g_aod" ) )
            {
            plugin->SetRunPrefix ( kGridRunPattern.Data() );

            // AliEn data production directory
            plugin->SetGridDataDir ( "/alice/data/2013/LHC13g" );

            // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
            plugin->SetDataPattern ( "*/pass1/AOD155/*AliAOD.root" );

            TString list_lhc13g_1 = "197669 197643 197584 197501 197583 197613 197553 197471 197606 197500 197616 197611 197692";
            TString list_lhc13g_2 = "197497 197499 197555 197470 197608 197531 197618 197610 197609 197496 197529 197527";

            plugin->AddRunList ( list_lhc13g_1.Data() );
            plugin->AddRunList ( list_lhc13g_2.Data() );
            return kTRUE;
            }


// ##################################################################################
    else
        { Printf ( "InputData :: NO DEFINED INPUT DATA RECOGNIZED !!! = %s", datarun.Data() ); return kFALSE; }

    }
// kate: indent-mode none; indent-width 4; replace-tabs on;
