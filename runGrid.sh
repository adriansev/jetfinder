#!/bin/bash

CMD="exec root.exe -l -b -q -x"
MACRO="EmcalJetCDF.C"

# cat << EOF > .rootrc
# EOF

cat << EOF > rootlogon.C
{
gSystem->AddIncludePath("-I. -I$ALICE_PHYSICS/include -I$ALICE_ROOT/include -I$FASTJET/include -I/usr/include");
gSystem->AddDynamicPath(".:$ALICE_PHYSICS/lib:$ALICE_ROOT/lib:$ROOTSYS/lib:$FASTJET/lib");

gSystem->Load("libPWGPP");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}

EOF

######################################################################
export CDF_doPhysSelect='true'
export CDF_doPileup='true'
export CDF_doMULT='false'   # disable unconditionaly the running of AliMultSelectionTask
export CDF_doBKRD='false'   # disable unconditionaly the running of RhoTask
export CDF_doCDF='true'     # toggle execution of TaskEmcalJetCDF task
export CDF_doSAMPLE='false' # toggle execution of TaskEmcalJetSample task

export CDF_DEBUG='0'        # set debug value kFatal = 0, kError, kWarning, kInfo, kDebug, kMaxType
export CDF_DEBUG_MGR='0'    # set debug value for AliAnalysisManager
export CDF_NSYSINFO='0'     # set (end enable) profiling of task; any int > 0 will enable profiling at sampling rate defined
export PROGRESSBAR='false'  # toggle the progress bar of AliAnalysisManager; will disable debugging

# Trigger values : (to be used as arguments)
#kMB : 1
#kINT7 : 2
#kAnyINT : 3145763
#kMB_central 3211299
#kMB_semicentral 3276835
#kMB_mostcentral 3342371
#kEMC : 50192
#kEMC_noGA : 17424

PERIOD="lhc16r"
FILE="data.txt"
SELECT_CHG="3145763"
SELECT_FULL="3145763"
MANAGER_MODE="0"  # local = 0, proof = 1, grid = 2, mixing = 3
PLUGIN_MODE="0"   # test = 0, offline = 1, submit = 2, merge = 3, full = 4
TASK_NAME="CDFJets"
NR_FILES="1"
NR_EVENTS="10000"
DO_JETS_CHG="true"
DO_JETS_FULL="false"

ROOT_VER=$(root-config --version)
[[ "${ROOT_VER}" =~ ^6 ]] && MACRO="${MACRO}+"

${CMD} "${MACRO}(\"${PERIOD}\",\"${FILE}\",${SELECT_CHG},${SELECT_FULL},${MANAGER_MODE},${PLUGIN_MODE},\"${TASK_NAME}\",${NR_FILES},${NR_EVENTS},\"${DO_JETS_CHG}\",\"${DO_JETS_FULL}\")"

