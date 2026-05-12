#!/bin/bash
set -e

HISTS=root/KShort_hists_run79516_defaultmap.root
TAG=plots/KShort_run79516_defaultmap

root -b -q "fillKShortHists.C(\"root/newcdbtag/outputKFParticle_KShort_run3pp_00079516_*.root\",\"${HISTS}\")"
root -b -q "plotKShortMass.C(\"${HISTS}\",\"${TAG}\")"
