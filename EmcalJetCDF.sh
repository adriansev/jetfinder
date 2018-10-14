#!/bin/bash
#mykEMC : 50192
#mykEMC_noGA : 17424
#mykMB : 3145763
#mykMB_central 3211299
#mykMB_semicentral 3276835
#mykMB_mostcentral 3342371

export CDF_doSAMPLE='false' # toggle execution of TaskEmcalJetSample task
export CDF_doCDF='true'     # toggle execution of TaskEmcalJetCDF task

export CDF_DEBUG='0'        # set debug value kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
export CDF_DEBUG_MGR='0'     # set debug value for AliAnalysisManager
export CDF_NSYSINFO='0'      # set (end enable) profiling of task; any int > 0 will enable profiling at sampling rate defined
export PROGRESSBAR='true'       # toggle the progress bar of AliAnalysisManager; will disable debugging

EXEC_ARGS="-l -b -q -x"

#EXEC="root.exe ${EXEC_ARGS}"
EXEC="aliroot ${EXEC_ARGS}"

export VER_ALIBUILD="ali-latest-1"
[[ -z "${ALICE_PHYSICS}" ]] && source $(alienv printenv AliPhysics/${VER_ALIBUILD})

${EXEC} EmcalJetCDF.C\(\"AOD\",\"lhc16r\"\,\"data.txt\",50192,50192,1,\"test\",\"CDFjets\",1,1000,true,true\)

