{
gSystem->AddIncludePath("-I. -I/home/physics-tools/alicebuild/sw/fedora30_x86-64/AliPhysics/ali-latest-r6-next-1/include -I/home/physics-tools/alicebuild/sw/fedora30_x86-64/AliRoot/ali-latest-r6-next-1/include -I/home/physics-tools/alicebuild/sw/fedora30_x86-64/fastjet/v3.2.1_1.024-alice3-1/include -I/usr/include");
gSystem->AddDynamicPath(".:/home/physics-tools/alicebuild/sw/fedora30_x86-64/AliPhysics/ali-latest-r6-next-1/lib:/home/physics-tools/alicebuild/sw/fedora30_x86-64/AliRoot/ali-latest-r6-next-1/lib:/home/physics-tools/alicebuild/sw/fedora30_x86-64/ROOT/v6-16-00-1/lib:/home/physics-tools/alicebuild/sw/fedora30_x86-64/fastjet/v3.2.1_1.024-alice3-1/lib");
gSystem->Load("libPWGPP");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}

