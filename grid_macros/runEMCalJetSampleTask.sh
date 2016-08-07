#!/bin/bash

if [ -z "$1" ]; then
	DATATYPE="AOD"
else
	DATATYPE="${1}"
fi

if [ -z "$2" ]; then
	PERIOD="lhc11c"
else
	PERIOD="${2}"
fi

if [ -z "$3" ]; then
	FILELIST="data.txt"
else
	FILELIST="${3}"
fi

if [ -z "$4" ]; then
	NEVENTS="1234567890"
else
	NEVENTS="${4}"
fi

root -b -q -x runEMCalJetSampleTask.C\(\""${DATATYPE}"\",\""${PERIOD}"\",\""${FILELIST}"\","${NEVENTS}"\)

