#!/bin/bash

USE_ALIROOT="" # set to yes to use aliroot instead of root.exe
USE_CVMFS=""      # set to yes to load cvmfs environment instead of alibuild

export VER_ALIBUILD="ali-latest-1"
export VER_CVMFS="vAN-20190310-1"

# Check if on centos
eval $(</etc/os-release)
if [[ "${ID}" == "centos" ]]; then
    IS_DEVTOOLSET=$(grep devtoolset-7 <<< "${PATH}")
    [[ -z "${IS_DEVTOOLSET}" ]] && { source /opt/rh/devtoolset-7/enable 2>/dev/null || { echo "devtoolset-7 could not be found"; exit 1;} }
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
        [[ -e "${ALICE_WORK_DIR}" ]] || { echo "ALICE_WORK_DIR could not be found; make sure it is defined in environment"; exit 1; }
        eval "$(alienv shell-helper)"
        eval "$(alienv -q load AliPhysics/${VER_ALIBUILD})"
    fi
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib64:/usr/lib64:/usr/local/lib:/usr/lib;
fi

IS_ALIROOT=$(which --skip-alias --skip-functions aliroot 2>/dev/null)
[[ -z "${IS_ALIROOT}" ]] && { echo "Still not found aliroot executable; Check the settings for ALIBUILD/CVMFS versions" ; exit 1; }

ROOT_VER=$(root-config --version)
[[ "${ROOT_VER}" =~ ^5 ]] && IsROOT5="1"
[[ "${ROOT_VER}" =~ ^6 ]] && IsROOT6="1"

######################################################################
# Trigger values : (to be used as arguments)
#mykMB : 1
#mykAnyINT : 3145763
#mykMB_central 3211299
#mykMB_semicentral 3276835
#mykMB_mostcentral 3342371
#mykEMC : 50192
#mykEMC_noGA : 17424

#export CDF_doMULT='false'   # disable unconditionaly the running of AliMultSelectionTask
export CDF_doSAMPLE='false' # toggle execution of TaskEmcalJetSample task
export CDF_doCDF='true'     # toggle execution of TaskEmcalJetCDF task

export CDF_DEBUG='0'        # set debug value kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
export CDF_DEBUG_MGR='0'    # set debug value for AliAnalysisManager
export CDF_NSYSINFO='0'     # set (end enable) profiling of task; any int > 0 will enable profiling at sampling rate defined
export PROGRESSBAR='false'  # toggle the progress bar of AliAnalysisManager; will disable debugging

# Analysis mode
# AnalysisType::local    = 0, // kLocalAnalysis
# AnalysisType::proof    = 1, // kProofAnalysis
# AnalysisType::grid     = 2, // kGridAnalysis
# AnalysisType::mixing   = 3  // kMixingAnalysis

# Plugin Mode
# PluginType::test     = 0,
# PluginType::offline  = 1,
# PluginType::submit   = 2,
# PluginType::merge    = 3,
# PluginType::full     = 4

#    const char*   cRunPeriod     = "LHC11d",
#    const char*   cLocalFiles    = "data.txt",
#    const UInt_t  arg_sel_chg    = 3145763,
#    const UInt_t  arg_sel_full   = 3145763,
#    AnalysisType  ManagerMode    = AnalysisType::local, // local = 0, proof = 1, grid = 2, mixing = 3
#    PluginType    PluginMode     = PluginType::test,    // test = 0, offline = 1, submit = 2, merge = 3, full = 4
#    const char*   cTaskName      = "CDFJets",
#    unsigned int  iNumFiles      = 100,
#    unsigned int  iNumEvents     = 999999999,
#    bool          bDoChargedJets = true,
#    bool          bDoFullJets    = false

MACRO="EmcalJetCDF.C"
[[ -n "${IsROOT6}" ]] && MACRO="${MACRO}+"

${EXEC} ${MACRO}\(\"lhc16r\"\,\"data.txt\",1,1,0,0,\"CDFjets\",1,10000,true,false\)

