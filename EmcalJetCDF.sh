#!/bin/bash

#mykEMC : 50192
#mykEMC_noGA : 17424
#mykMB : 3145763
#mykMB_central 3211299
#mykMB_semicentral 3276835
#mykMB_mostcentral 3342371

EXEC_ARGS="-l -b -q -x"

#EXEC="root.exe ${EXEC_ARGS}"
EXEC="aliroot ${EXEC_ARGS}"

export VER_ALIBUILD="ali-latest-1"
source $(alienv printenv AliPhysics/${VER_ALIBUILD})

${EXEC} EmcalJetCDF.C\(\"AOD\",\"lhc16r\"\,\"data.txt\",50192,50192,1,\"test\",\"CDFjets\",3,9999999,true,true\)


