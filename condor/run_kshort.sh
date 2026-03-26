#!/usr/bin/bash
# Condor wrapper for Fun4All_KShortReco_run3pp.C
# Runs natively on Alma9 using the alma9.2-gcc-14.2.0 / ana.542 sPHENIX build.
#
# Arguments:
#   $1 : full path to input DST file
#   $2 : output directory

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${USER}

hostname

this_script=$BASH_SOURCE
this_script=`readlink -f $this_script`
this_dir=`dirname $this_script`
macro_dir=$(dirname $this_dir)

echo "Running: $this_script $*"
echo "Macro dir: $macro_dir"

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]; then
    cd $_CONDOR_SCRATCH_DIR
    rsync -av $macro_dir/Fun4All_KShortReco_run3pp.C .
    rsync -av $this_dir/run_kshort_inner.sh .
    chmod +x run_kshort_inner.sh
else
    echo "condor scratch NOT set"
    exit -1
fi

./run_kshort_inner.sh $*

echo "wrapper script done"
