#!/bin/bash

# enable GRID mode
export CDF_GRID=0

# Set run mode.  Can be "full", "test", "offline", "submit" or "merge"
export CDF_GRIDMODE="test"

# for grid mode, use the dataset label found in InputData.C
export CDF_dataset="pp_lhc12a15f"

# number of files to process
export CDF_NUMFILES=1

# numeber of events to process
export CDF_NUMEV=100

# enable charged jets finder
export CDF_CHGJETS=1

# enable full jets finder
export CDF_FULLJETS=1

# enable sample task
export CDF_doSAMPLE=1

# enable CDF task
export CDF_doCDF=1

# configuration file (full path) of Correction Framework
export CDF_EMCALCFG="PWGJE_SEV_Config.yaml"

# trigger selection for charged jets task
CDF_TRGSEL_CHG="kEMC_noGA"

# trigger selection for full jets task
CDF_TRGSEL_FULL="kEMC_noGA"

root -b -q -x runEMCalJetSampleTask.C
RV=$?

echo "END runEMCalJetSampleTask.sh"
exit $RV

