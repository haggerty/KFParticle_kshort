#!/usr/bin/bash
# Generates condor.list for Fun4All_KShortReco_run3pp condor submission.
#
# Usage:
#   ./create_condor_list.sh [lustre_dst_dir] [output_dir] [max_jobs]
#
# Defaults:
#   lustre_dst_dir : /sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_TRACKS
#   output_dir     : /sphenix/user/$USER/KShort_run3pp
#   max_jobs       : 0 (no limit)

DST_BASE=${1:-/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_TRACKS}
OUTDIR=${2:-/sphenix/user/$(whoami)/analysis/2026-03-23/KShort_run3pp}
MAX_JOBS=${3:-0}

this_script=$(readlink -f $0)
this_dir=$(dirname $this_script)

LOGDIR=${OUTDIR}/condorlogs
mkdir -p $OUTDIR || { echo "ERROR: could not create $OUTDIR"; exit 1; }
mkdir -p $LOGDIR || { echo "ERROR: could not create $LOGDIR"; exit 1; }

echo "Scanning: $DST_BASE"
echo "Output dir: $OUTDIR ($(ls -d $OUTDIR))"
echo "Log dir: $LOGDIR ($(ls -d $LOGDIR))"
[[ $MAX_JOBS -gt 0 ]] && echo "Job limit: $MAX_JOBS" || echo "Job limit: none"

rm -f condor.list

nfiles=0
for dstfile in $(find $DST_BASE -name "DST_TRKR_TRACKS_*.root" | sort); do
    [[ $MAX_JOBS -gt 0 && $nfiles -ge $MAX_JOBS ]] && break

    lfn=$(basename $dstfile)
    outfile=${LOGDIR}/condor-${lfn%.root}.out
    errfile=${LOGDIR}/condor-${lfn%.root}.err
    logfile=/tmp/$(whoami)-condor-${lfn%.root}.log

    echo "$dstfile $OUTDIR $outfile $errfile $logfile $this_dir" >> condor.list
    nfiles=$((nfiles + 1))
done

echo "Created condor.list with $nfiles jobs"
echo "Submit with: condor_submit condor.job"
