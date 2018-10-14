#!/bin/bash

USE_ALIROOT="" # set to yes to use aliroot instead of root.exe
USE_CVMFS=""      # set to yes to load cvmfs environment instead of alibuild

export VER_ALIBUILD="ali-latest-1"
export VER_CVMFS="vAN-20181014-1"

# Check if on centos
eval $(</etc/os-release)
if [[ "${ID}" == "centos" ]]; then
    IS_DEVTOOLSET=$(grep devtoolset-7 <<< "${PATH}")
    [[ -z "${IS_DEVTOOLSET}" ]] && { source /opt/rh/devtoolset-7/enable 2>/dev/null || echo "devtoolset-7 could not be found"; }
fi

EXEC_ARGS="-l -b -q -x"
# Establish the running executable and base arguments
CMD="root.exe"
[[ "${USE_ALIROOT}" == "yes" ]] && CMD="aliroot"
EXEC="${CMD} ${EXEC_ARGS}"

IS_ALIROOT=$(which --skip-alias --skip-functions aliroot 2>/dev/null)
if [[ -z "${IS_ALIROOT}" ]]; then
    if [[ "${USE_CVMFS}" == "yes" ]]; then
        eval $(/cvmfs/alice.cern.ch/bin/alienv printenv VO_ALICE@AliPhysics::${VER_CVMFS});
    else
        eval $(~/.local/bin/alienv -q load AliPhysics/${VER_ALIBUILD});
    fi
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib64:/usr/lib64:/usr/local/lib:/usr/lib;
fi

IS_ALIROOT=$(which --skip-alias --skip-functions aliroot 2>/dev/null)
[[ -z "${IS_ALIROOT}" ]] && { echo "Still not found aliroot executable; Check the settings for ALIBUILD/CVMFS versions" ; exit 1; }

######################################################################
#mykEMC : 50192
#mykEMC_noGA : 17424
#mykMB : 3145763
#mykMB_central 3211299
#mykMB_semicentral 3276835
#mykMB_mostcentral 3342371

export CDF_doSAMPLE='false' # toggle execution of TaskEmcalJetSample task
export CDF_doCDF='true'     # toggle execution of TaskEmcalJetCDF task

export CDF_DEBUG='0'        # set debug value kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
export CDF_DEBUG_MGR='0'    # set debug value for AliAnalysisManager
export CDF_NSYSINFO='0'     # set (end enable) profiling of task; any int > 0 will enable profiling at sampling rate defined
export PROGRESSBAR='false'  # toggle the progress bar of AliAnalysisManager; will disable debugging

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
