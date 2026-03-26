#!/usr/bin/bash
# Inner script — sources the Alma9 sPHENIX environment and runs the Fun4All macro.
#
# Arguments:
#   $1 : full path to input DST file
#   $2 : output directory

source /cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana.542

infile=$1
outdir=$2
inbase=$(basename $infile)
indir=$(dirname $infile)/

echo "OFFLINE_MAIN: $OFFLINE_MAIN"
echo "Input file : $infile"
echo "Output dir : $outdir"

root.exe -q -b "Fun4All_KShortReco_run3pp.C(0,\"${inbase}\",\"${indir}\")"

mkdir -p $outdir/root
find . -name "outputKFParticle_*.root" -exec mv {} $outdir/root/ \;

echo "Script done"
