#!/bin/bash

## O24 Pose3d
# Precondition: Move u007, u011, u016, u019 dataset to the target. (ex. ../images/pose3d_validation)
#               The dataset is so large that it may not be on the target all at once.
#               Set $srcDir & $targetDir $pipeline as you want.
#               make fittv_[YOUR PIPELINE]_fronthigh.json, fittv_[YOUR PIPELINE]_frontlow.json, fittv_[YOUR PIPELINE]_left.json, fittv_[YOUR PIPELINE]_right.json ( !!! check thresh_iou_update / gt_bboxes / 1 Euro Node)

srcDir=../images/pose3d_validation/
targetDir=./pose3d_validation/
pipeline=fittv_pipe_npu_all_v2_mid

if [ ! -s ${pipeline}_fronthigh.json ] || [ ! -s ${pipeline}_frontlow.json ]  || [ ! -s ${pipeline}_left.json ] || [ ! -s ${pipeline}_right.json ]
then
  echo '!!!make json files for each direction'
  exit 1
fi

if [ ! -d $targetDir ]; then
  mkdir -p $targetDir
fi

for person in "u007" "u011" "u016" "u019"
do
  personDir=$srcDir$person
  for catDir in $personDir/*
  do
    for poseDir in $catDir/*
    do
      for direction in "fronthigh" "frontlow" "left" "right"
      do
        directionDir=$poseDir/$direction
        jsonFile=${pipeline}_${direction}.json
        if [ -d $directionDir ]; then
          ./fittv-sample $jsonFile $directionDir/ $targetDir/

          namePostfix=${directionDir//$srcDir//}
          namePostfix=${namePostfix//\//_}
          rename outputs outputs_${pipeline}${namePostfix} ${targetDir}/outputs.json

        fi
      done
    done
  done
done
# Move targetDir to PC for conversion format and verifying.
