{
gSystem->AddIncludePath("-I. -I$ALICE_PHYSICS/include -I$ALICE_ROOT/include -I$FASTJET/include -I/usr/include");
gSystem->AddDynamicPath(".:$ALICE_PHYSICS/lib:$ALICE_ROOT/lib:$ROOTSYS/lib:$FASTJET/lib");
gSystem->Load("libPWGPP");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}

