
//______________________________________________________________________________
// function to add specific data to be processed
void InputData ( AliAnalysisAlien* plugin, TString dataruns )
{

//______________________________________________________________________________
//  DEFINED INPUT DATA

  if (!plugin)             { printf ("InputData :: plugin invalid"); gApplication->Terminate(); }
  if ( dataruns.IsNull() ) { printf ("InputData :: no data input");  gApplication->Terminate(); }

  TString     kGridRunPattern = "%09d"; // important for leading zeroes!!

  Printf ("data = %s" ,dataruns.Data() );

// FILTER_Pb-Pb_073_LHC10h_Stage5: TOF tender, cascades on_FinalMerging
if ( !dataruns.CompareTo("pbpb073_lhc10h") )
  {
  // AliEn data production directory
  plugin->SetGridDataDir ("/alice/data/2010/LHC10h");

  plugin->SetRunPrefix   (kGridRunPattern.Data());

// Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...

// /alice/data/2010/LHC10h/000138275/ESDs/pass2/AOD073/0001/AliAOD.root

  plugin->SetDataPattern("ESDs/pass2/AOD073/*AliAOD.root");

  plugin->AddRunNumber(138275); //
  plugin->AddRunNumber(138795); //
  plugin->AddRunNumber(138534); //
  plugin->AddRunNumber(137748); //

  }

// 7 TeV FILTER_p-p037_LHC10e: No tender-> AODstd(+jets), vertexing, muons, dielectrons
else if ( !dataruns.CompareTo("pp_lhc10e") )
  {
  // AliEn data production directory
  plugin->SetGridDataDir ("/alice/data/2010/LHC10e");
  plugin->SetRunPrefix   (kGridRunPattern.Data());

// directory                                                 processed    filtered
// /alice/data/2010/LHC10e/000130795/ESDs/pass2/AOD037    // 10,689,247   9,847,754
// /alice/data/2010/LHC10e/000128257/ESDs/pass2/AOD037    //  7,950,373   7,532,282
// /alice/data/2010/LHC10e/000129654/ESDs/pass2/AOD037    //  7,607,730   6,881,354
// /alice/data/2010/LHC10e/000129042/ESDs/pass2/AOD037    //  6,889,397   6,042,817
// /alice/data/2010/LHC10e/000128609/ESDs/pass2/AOD037    //  5,406,257   5,125,970
// /alice/data/2010/LHC10e/000130517/ESDs/pass2/AOD037    //  6,530,240   5,213,938
// /alice/data/2010/LHC10e/000128452/ESDs/pass2/AOD037    //  5,406,325   4,982,075
// /alice/data/2010/LHC10e/000127931/ESDs/pass2/AOD037    //  5,073,689   4,621,616
// /alice/data/2010/LHC10e/000129961/ESDs/pass2/AOD037    //  5,527,363   4,535,549
// /alice/data/2010/LHC10e/000130844/ESDs/pass2/AOD037    //  5,190,861   4,532,341
// /alice/data/2010/LHC10e/000127729/ESDs/pass2/AOD037    //  4,368,154   4,019,002
// /alice/data/2010/LHC10e/000128504/ESDs/pass2/AOD037    //  4,356,053   4,042,845


// Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
  plugin->SetDataPattern("*ESDs/pass2/AOD037/*AliAOD.root");

  plugin->AddRunNumber(130795);
  plugin->AddRunNumber(128257);
  plugin->AddRunNumber(129654);
  plugin->AddRunNumber(129042);
  plugin->AddRunNumber(128609);
  plugin->AddRunNumber(130517);
  plugin->AddRunNumber(128452);
  plugin->AddRunNumber(127931);
  plugin->AddRunNumber(129961);
  plugin->AddRunNumber(130844);
  plugin->AddRunNumber(127729);
  plugin->AddRunNumber(128504);
  }


// FILTER_p-p_113_LHC11a: Standard AODs + deltas
else if ( !dataruns.CompareTo("pp_lhc11a") )
  {
  // AliEn data production directory
  plugin->SetGridDataDir ("/alice/data/2011/LHC11a/");
  plugin->SetRunPrefix   (kGridRunPattern.Data());

// directory                                                       processed    filtered
// /alice/data/2011/LHC11a/000146805/ESDs/pass4_with_SDD/AOD113   20,847,678   19,724,856
// /alice/data/2011/LHC11a/000146824/ESDs/pass4_with_SDD/AOD113   10,694,943   10,121,251
// /alice/data/2011/LHC11a/000146804/ESDs/pass4_with_SDD/AOD113    9,789,857    9,259,417
// /alice/data/2011/LHC11a/000146858/ESDs/pass4_with_SDD/AOD113    8,751,436    8,751,436
// /alice/data/2011/LHC11a/000146802/ESDs/pass4_with_SDD/AOD113    4,375,321    4,311,803
// /alice/data/2011/LHC11a/000146747/ESDs/pass4_with_SDD/AOD113    4,200,102    3,946,670
// /alice/data/2011/LHC11a/000146806/ESDs/pass4_with_SDD/AOD113    3,669,425    3,473,521

// Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
  plugin->SetDataPattern("*ESDs/pass4_with_SDD/AOD113/*AliAOD.root");

  plugin->AddRunNumber(146805);
  plugin->AddRunNumber(146824);
  plugin->AddRunNumber(146804);
  plugin->AddRunNumber(146858);
  plugin->AddRunNumber(146802);
  plugin->AddRunNumber(146747);
  plugin->AddRunNumber(146806);
  }

else
  { Printf("CDFhistos :: InputData :: NO DEFINED INPUT DATA RECOGNIZED !!! = %s", dataruns.Data() );gApplication->Terminate(); }


}
