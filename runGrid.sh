#!/bin/bash

CMD="exec root.exe -l -b -q -x"
MACRO="EmcalJetCDF.C"

if [[ ! -e "rootlogon.C" ]]; then
cat << "EOF" > rootlogon.C
{
gSystem->AddIncludePath("-I. -I$ALICE_PHYSICS/include -I$ALICE_ROOT/include -I$FASTJET/include -I/usr/include");
gSystem->AddDynamicPath(".:$ALICE_PHYSICS/lib:$ALICE_ROOT/lib:$ROOTSYS/lib:$FASTJET/lib");
gSystem->Load("libPWGPP");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}

EOF
fi

######################################################################
# Trigger values : (to be used as arguments)
#kMB : 1
#kINT7 : 2
#kAnyINT : 3145763
#kMB_central 3211299
#kMB_semicentral 3276835
#kMB_mostcentral 3342371
#kEMC : 50192
#kEMC_noGA : 17424

PERIOD="lhc18m"
FILE="pp_lhc18m_aod_01"
SELECT_CHG="2"
SELECT_FULL="2"
MANAGER_MODE="0"  # local = 0, proof = 1, grid = 2, mixing = 3
PLUGIN_MODE="0"   # test = 0, offline = 1, submit = 2, merge = 3, full = 4
TASK_NAME="CDFJets"
NR_FILES="10"
NR_EVENTS="99999999"
DO_JETS_CHG=1
DO_JETS_FULL=0

ROOT_VER=$(root-config --version)
[[ "${ROOT_VER}" =~ ^6 ]] && MACRO="${MACRO}+"

${CMD} "${MACRO}(\"${PERIOD}\",\"${FILE}\",${SELECT_CHG},${SELECT_FULL},${MANAGER_MODE},${PLUGIN_MODE},\"${TASK_NAME}\",${NR_FILES},${NR_EVENTS},${DO_JETS_CHG},${DO_JETS_FULL})" 2>&1 | tee runGrid_${MANAGER_MODE}_${PLUGIN_MODE}.log

