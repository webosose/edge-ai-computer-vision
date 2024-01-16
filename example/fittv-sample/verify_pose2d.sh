#!/bin/bash

## O24 Pose2d
# Precondition: Move col7~col10 dataset to the target. (ex. ../images/aiR_Dataset)
#               Set $srcDir & $targetDir as you want.
#               make fittv_col*.json ( !!! check thresh_iou_update / gt_bboxes / 1 Euro Node)

srcDir=../images/aiR_Dataset/
targetDir=../images/results/

if [ ! -s fittv_col7.json ] || [ ! -s fittv_col8.json ]  || [ ! -s fittv_col9.json ] || [ ! -s fittv_col10_1euro.json ]
then
  echo '!!!make fittv_colX.json'
  exit 1
fi

./fittv-sample fittv_col7.json $srcDir/col7_bodyscan/
./fittv-sample fittv_col8.json $srcDir/col8_bodyscan_2/
./fittv-sample fittv_col9.json $srcDir/col9_pose_model_val_data/images/
./fittv-sample fittv_col10_1euro.json $srcDir/col10_stop/images/



if [ ! -d $targetDir ]; then
  mkdir $targetDir
  mkdir $targetDir/col7/ && mkdir $targetDir/col8/ && mkdir $targetDir/col9/ && mkdir $targetDir/col10/

  for id in 1 2 3 4 5 6 7 8 9
  do
    mkdir $targetDir/col9/$id
  done
fi

mv $srcDir/col7_bodyscan/*_res.jpg $targetDir/col7/
mv $srcDir/col7_bodyscan/outputs*.json $targetDir/col7/

mv $srcDir/col8_bodyscan_2/*_res.jpg $targetDir/col8/
mv $srcDir/col8_bodyscan_2/outputs*.json $targetDir/col8/

for id in 1 2 3 4 5 6 7 8 9
do
  mv $srcDir/col9_pose_model_val_data/images/$id/*_res.jpg $targetDir/col9/$id/
  mv $srcDir/col9_pose_model_val_data/images/$id/outputs*.json $targetDir/col9/$id/
done

mv $srcDir/col10_stop/images/output*.json $targetDir/col10/


# Move targetDir to PC for conversion format and verifying.
