#!/bin/bash
#https://cms-pdmv.cern.ch/mcm/requests?prepid=SUS-RunIISummer20UL18wmLHEGEN-00038&page=0&shown=262271

# Test script for Kamals data generation

# September 2023
#############################





job=$1

export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_19_patch3/src ] ; then
  echo release CMSSW_10_6_19_patch3 already exists
else
  scram p CMSSW CMSSW_10_6_19_patch3
fi
cd CMSSW_10_6_19_patch3/src
eval `scram runtime -sh`

# Download fragment from McM
#curl -s -k https://cms-pdmv.cern.ch/mcm/public/restapi/requests/get_fragment/SUS-RunIISummer20UL18wmLHEGEN-00038 --retry 3 --create-dirs -o Configuration/GenProduction/python/SUS-RunIISummer20UL18wmLHEGEN-00038-fragment.py
#[ -s Configuration/GenProduction/python/SUS-RunIISummer20UL18wmLHEGEN-00038-fragment.py ] || exit $?;


scram b
cd ../..

# Maximum validation duration: 28800s
# Margin for validation duration: 30%
# Validation duration with margin: 28800 * (1 - 0.30) = 20160s
# Time per event for each sequence: 3.3075s
# Threads for each sequence: 1
# Time per event for single thread for each sequence: 1 * 3.3075s = 3.3075s
# Which adds up to 3.3075s per event
# Single core events that fit in validation duration: 20160s / 3.3075s = 6095
# Produced events limit in McM is 10000
# According to 1.0000 efficiency, validation should run 10000 / 1.0000 = 10000 events to reach the limit of 10000
# Take the minimum of 6095 and 10000, but more than 0 -> 6095
# It is estimated that this validation will produce: 6095 * 1.0000 = 6095 events
EVENTS=100


# cmsDriver command
cmsDriver.py Configuration/GenProduction/python/fragment.py --python_filename Test${job}_1_cfg.py --eventcontent RAWSIM,LHE --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN,LHE --fileout file:Test${job}.root --conditions 106X_upgrade2018_realistic_v4 --beamspot Realistic25ns13TeVEarly2018Collision --customise_commands process.source.numberEventsInLuminosityBlock="cms.untracked.uint32(100)" --step LHE,GEN --geometry DB:Extended --era Run2_2018 --no_exec --mc -n $EVENTS || exit $? ;

# Run generated config
REPORT_NAME=Test${job}_report.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME Test${job}_1_cfg.py || exit $? ;




###########SUS-RunIISummer20UL18SIM-00056 commands




export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_17_patch1/src ] ; then
  echo release CMSSW_10_6_17_patch1 already exists
else
  scram p CMSSW CMSSW_10_6_17_patch1
fi
cd CMSSW_10_6_17_patch1/src
eval `scram runtime -sh`

scram b
cd ../..


# cmsDriver command
cmsDriver.py  --python_filename TestSIM${job}_1_cfg.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM --fileout file:TestSIM${job}.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --beamspot Realistic25ns13TeVEarly2018Collision --step SIM --geometry DB:Extended --filein file:Test${job}.root --era Run2_2018 --runUnscheduled --no_exec --mc -n -1 || exit $? ;

# Run generated config
REPORT_NAME=TestSIM${job}_report.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME TestSIM${job}_1_cfg.py || exit $? ;

## 32min37sec

########SUS-RunIISummer20UL18DIGIPremix-00056

#!/bin/bash


export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_17_patch1/src ] ; then
  echo release CMSSW_10_6_17_patch1 already exists
else
  scram p CMSSW CMSSW_10_6_17_patch1
fi
cd CMSSW_10_6_17_patch1/src
eval `scram runtime -sh`

scram b
cd ../..

rm Test${job}.root

# cmsDriver command
cmsDriver.py  --python_filename DIGIPremix${job}_1_cfg.py --eventcontent PREMIXRAW --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI --fileout file:DIGIPremix${job}.root --pileup_input "dbs:/Neutrino_E-10_gun/RunIISummer20ULPrePremix-UL18_106X_upgrade2018_realistic_v11_L1v1-v2/PREMIX" --conditions 106X_upgrade2018_realistic_v11_L1v1 --step DIGI,DATAMIX,L1,DIGI2RAW --procModifiers premix_stage2 --geometry DB:Extended --filein file:TestSIM${job}.root --datamix PreMix --era Run2_2018 --runUnscheduled --no_exec --mc -n -1 || exit $? ;

# Run generated config
REPORT_NAME=DIGIPremix${job}.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME DIGIPremix${job}_1_cfg.py || exit $? ;

###5min23sec


######## SUS-RunIISummer20UL18HLT-00056
#2min36sec

export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_2_16_UL/src ] ; then
  echo release CMSSW_10_2_16_UL already exists
else
  scram p CMSSW CMSSW_10_2_16_UL
fi
cd CMSSW_10_2_16_UL/src
eval `scram runtime -sh`

scram b
cd ../..

rm TestSIM${job}.root

# cmsDriver command
cmsDriver.py  --python_filename TestHLT${job}_1_cfg.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-RAW --fileout file:TestHLT${job}.root --conditions 102X_upgrade2018_realistic_v15 --customise_commands 'process.source.bypassVersionCheck = cms.untracked.bool(True)' --step HLT:2018v32 --geometry DB:Extended --filein file:DIGIPremix${job}.root --era Run2_2018 --no_exec --mc -n -1 || exit $? ;



# Run generated config
REPORT_NAME=TestHLT${job}.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME TestHLT${job}_1_cfg.py || exit $? ;





######SUS-RunIISummer20UL18RECO-00056

#8min58sec

export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_17_patch1/src ] ; then
  echo release CMSSW_10_6_17_patch1 already exists
else
  scram p CMSSW CMSSW_10_6_17_patch1
fi
cd CMSSW_10_6_17_patch1/src
eval `scram runtime -sh`

scram b
cd ../..

rm DIGIPremix${job}.root


#EVENTS=1740


# cmsDriver command
cmsDriver.py  --python_filename TestRECO${job}_1_cfg.py --eventcontent AODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier AODSIM --fileout file:TestRECO${job}.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --step RAW2DIGI,L1Reco,RECO,RECOSIM,EI --geometry DB:Extended --filein file:TestHLT${job}.root --era Run2_2018 --runUnscheduled --no_exec --mc -n -1 || exit $? ;

# Run generated config
REPORT_NAME=TestRECO${job}.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME TestRECO${job}_1_cfg.py || exit $? ;



######SUS-RunIISummer20UL18MiniAODv2-00057
#1min54sec


export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_20/src ] ; then
  echo release CMSSW_10_6_20 already exists
else
  scram p CMSSW CMSSW_10_6_20
fi
cd CMSSW_10_6_20/src
eval `scram runtime -sh`

scram b
cd ../..


rm TestHLT${job}.root

# cmsDriver command
cmsDriver.py  --python_filename TestMiniAOD${job}_1_cfg.py --eventcontent MINIAODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier MINIAODSIM --fileout file:TestMiniAOD${job}.root --conditions 106X_upgrade2018_realistic_v16_L1v1 --step PAT --procModifiers run2_miniAOD_UL --geometry DB:Extended --filein file:TestRECO${job}.root --era Run2_2018 --runUnscheduled --no_exec --mc -n -1 || exit $? ;

# Run generated config
REPORT_NAME=TestMiniAOD${job}_report.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME TestMiniAOD${job}_1_cfg.py || exit $? ;



######SUS-RunIISummer20UL18NanoAODv9-00057


export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_26/src ] ; then
  echo release CMSSW_10_6_26 already exists
else
  scram p CMSSW CMSSW_10_6_26
fi
cd CMSSW_10_6_26/src
eval `scram runtime -sh`

scram b
cd ../..

rm TestRECO${job}.root

# cmsDriver command
cmsDriver.py  --python_filename TestNanoAOD${job}_1_cfg.py --eventcontent NANOAODSIM  --customise Configuration/DataProcessing/Utils.addMonitoring --datatier NANOAODSIM --fileout file:TestNanoAOD${job}.root --conditions 106X_upgrade2018_realistic_v16_L1v1 --step NANO --filein file:TestMiniAOD${job}.root --era Run2_2018,run2_nanoAOD_106Xv2 --no_exec --mc -n -1 || exit $? ;

# Run generated config
REPORT_NAME=TestNanoAOD${job}_report.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME TestNanoAOD${job}_1_cfg.py || exit $? ;

rm TestMiniAOD${job}.root
