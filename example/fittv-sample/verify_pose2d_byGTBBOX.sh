#!/bin/bash

## O24 Pose2d
# Precondition: Move col7~col10 dataset to the target. (ex. ../images/aiR_Dataset)
#               Set $srcDir & $targetDir as you want.
#               make fittv_col*.json ( !!! check thresh_iou_update / gt_bboxes / 1 Euro Node)

srcDir=../images/aiR_Dataset/
targetDir=./pose2d_veri_results_byGTBBOX/

#### CHECK json file exist ####
if [ ! -s fittv_col7_gtbbox.json ] || [ ! -s fittv_col8_gtbbox.json ]  || [ ! -s fittv_col9_*_gtbbox.json ] || [ ! -s fittv_col10_1euro_gtbbox.json ]
then
  echo '!!!make fittv_colX_gtbbox.json'
  exit 1
fi


#### MAKE Target Directory ####
if [ ! -d $targetDir ]; then
  mkdir $targetDir
fi

for n in 7 8 9 10
do
  if [ ! -d $targetDir/col$n ]; then
    mkdir $targetDir/col$n
  fi
done

for id in 1 2 3 4 5 6 7 8 9
do
  if [ ! -d $targetDir/col9/$id ]; then
    mkdir $targetDir/col9/$id
  fi
done




#### RUN fittv-sample!! ####
./fittv-sample fittv_col7_gtbbox.json $srcDir/col7_bodyscan/ $targetDir/col7/
./fittv-sample fittv_col8_gtbbox.json $srcDir/col8_bodyscan_2/ $targetDir/col8/
for id in 1 2 3 4 5 6 7 8 9
do
echo fittv_col9_${id}_gtbbox.json
  ./fittv-sample fittv_col9_${id}_gtbbox.json $srcDir/col9_pose_model_val_data/images/$id/ $targetDir/col9/$id/
done
./fittv-sample fittv_col10_1euro_gtbbox.json $srcDir/col10_stop/images/ $targetDir/col10/



# Move targetDir to PC for conversion format and verifying.
