#!/bin/bash

#VER="vAN-20161119-1"
#export VER

#eval $(alienv printenv VO_ALICE@AliPhysics::${VER})

##root -b -q -x runEMCalJetSampleTask.C\(\"AOD\",\"lhc11d\"\,\"data.txt\",mykMB,mykMB\)
root -b -q -x runEMCalJetSampleTask.C\(\"AOD\",\"lhc11d\"\,\"data.txt\",mykEMC_noGA,mykEMC_noGA\)

echo "end of $PBS_JOBNAME"


