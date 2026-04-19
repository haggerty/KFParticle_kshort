#!/usr/bin/bash
# Generates condor.list for Fun4All_KShortReco_run3pp condor submission.
#
# Usage:
#   ./create_condor_list.sh [max_jobs]
#
# The DST and output paths are hardcoded to their standard locations.
# max_jobs defaults to 0 (no limit).

MAX_JOBS=${1:-0}
DST_BASE=/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_TRACKS
OUTDIR=/sphenix/user/$(whoami)/analysis/2026-03-23/KShort_run3pp

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

# Build sorted file list using ls per subdirectory (find is too slow on lustre;
# glob expansion on 500k-file dirs hits ARG_MAX, so list the dir and grep)
filelist=$(mktemp)
for rundir in $DST_BASE/run_*/; do
    ls -1 "${rundir}" 2>/dev/null \
        | grep '^DST_TRKR_TRACKS_.*\.root$' \
        | sed "s|^|${rundir}|"
done | sort > "$filelist"

# Apply job limit
if [[ $MAX_JOBS -gt 0 ]]; then
    head -n $MAX_JOBS "$filelist" > "${filelist}.trim"
    mv "${filelist}.trim" "$filelist"
fi

while read dstfile; do
    lfn=$(basename $dstfile)
    outfile=${LOGDIR}/condor-${lfn%.root}.out
    errfile=${LOGDIR}/condor-${lfn%.root}.err
    logfile=/tmp/$(whoami)-condor-${lfn%.root}.log
    echo "$dstfile $OUTDIR $outfile $errfile $logfile $this_dir" >> condor.list
done < "$filelist"

nfiles=$(wc -l < condor.list 2>/dev/null || echo 0)
rm -f "$filelist"

echo "Created condor.list with $nfiles jobs"
echo "Submit with: condor_submit condor.job"
