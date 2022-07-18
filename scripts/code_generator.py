import os
import glob
from shutil import copyfile
import argparse, textwrap
import shutil

parser = argparse.ArgumentParser(description=textwrap.dedent( ''' Code Generator
    ex> python code_generator.py movenet movenet_single_pose_lightning_ptq.tflite movenet_single_pose_lightning_ptq_edgetpu.tflite '''), 
    formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('name', help='function name')
parser.add_argument('cpu_model', help='cpu model name')
#parser.add_argument('tpu_model', help='edgetpu model name')
#parser.add_argument('-i', help='install output', default=False, action='store_true')

args= parser.parse_args()

cpu_model_name = args.cpu_model
#tpu_model_name = args.tpu_model
fname = args.name 
Fname = fname.capitalize()
FNAME = fname.upper()

# replace file name
f1 = ['templates', 'mname', 'MName']
f2 = ['output', fname, Fname]

# replace sources contents
#t1 = ['MNAME', 'MName', 'mname', 'cpu_model_name', 'tpu_model_name']
#t2 = [FNAME, Fname, fname, cpu_model_name, tpu_model_name]
t1 = ['MNAME', 'MName', 'mname', 'cpu_model_name']
t2 = [FNAME, Fname, fname, cpu_model_name]
types = ('*.h', '*.cpp', '*.txt') 

src_path = './templates/**/'
srcs = []
for t in types:
    srcs.extend(glob.glob(src_path + t, recursive=True))

for src in srcs:
    dst = src
    for a, b in zip(f1, f2):
        dst = dst.replace(a, b, 1) 
    dst_folder = os.path.dirname(dst)
    if not os.path.exists(dst_folder):
        os.makedirs(dst_folder)
    if not os.path.exists(dst):
        print('copy : ' + src + ' -> ' + dst)
        copyfile(src, dst)

targets = []
dst_path = './output/**/'
for t in types:
    targets.extend(glob.glob(dst_path + t, recursive=True))

def replace_in_file(t1, t2, filename):
    fin = open(filename, "rt")
    data = fin.read()
    for i in range(0, len(t1)):
        data = data.replace(t1[i], t2[i])
    fin.close()
    fin = open(filename, "wt")
    fin.write(data)
    fin.close()

for target in targets:
    replace_in_file(t1, t2, target)

def recursive_overwrite(src, dest, ignore=None):
    if os.path.isdir(src):
        if not os.path.isdir(dest):
            os.makedirs(dest)
        files = os.listdir(src)
        if ignore is not None:
            ignored = ignore(src, files)
        else:
            ignored = set()
        for f in files:
            if f not in ignored:
                recursive_overwrite(os.path.join(src, f),
                                    os.path.join(dest, f),
                                    ignore)
    else:
        shutil.copyfile(src, dest)

