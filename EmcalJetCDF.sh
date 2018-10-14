#!/bin/bash

USE_ALIROOT="" # set to yes to use aliroot instead of root.exe
USE_CVMFS=""   # set to yes to load cvmfs environment instead of alibuild

export VER_ALIBUILD="ali-latest-1"
export VER_CVMFS="vAN-20181014-1"

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
export PROGRESSBAR='false'       # toggle the progress bar of AliAnalysisManager; will disable debugging

EXEC_ARGS="-l -b -q -x"

# Establish the running executable and base arguments
CMD="root.exe"
[[ "${USE_ALIROOT}" == "yes" ]] && CMD="aliroot"
EXEC="${CMD} ${EXEC_ARGS}"

if [[ -z "${ALICE_PHYSICS}" ]]; then # if there is no environment loaded
    if [[ "${USE_CVMFS}" == "yes" ]]; then
        eval $(alienv printenv VO_ALICE@AliPhysics::${VER_CVMFS})
    else
        source $(alienv printenv AliPhysics/${VER_ALIBUILD})
    fi
fi

#    const char*   cDataType      = "AOD",
#    const char*   cRunPeriod     = "LHC11d",
#    const char*   cLocalFiles    = "data.txt",
#    const UInt_t  arg_sel_chg    = 3145763,
#    const UInt_t  arg_sel_full   = 3145763,
#    int           iStartAnalysis = 1,
#    const char*   cGridMode      = "test",
#    const char*   cTaskName      = "CDFJets",
#    unsigned int  iNumFiles      = 100,
#    unsigned int  iNumEvents     = 999999999,
#    bool          bDoChargedJets = true,
#    bool          bDoFullJets    = false

${EXEC} EmcalJetCDF.C\(\"AOD\",\"lhc16r\"\,\"data.txt\",50192,50192,1,\"test\",\"CDFjets\",1,1000,true,false\)

