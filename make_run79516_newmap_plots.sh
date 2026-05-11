#!/bin/bash
set -e

HISTS=root/KShort_hists_run79516_newmap.root
TAG=plots/KShort_run79516_newmap

root -b -q "fillKShortHists.C(\"root/outputKFParticle_KShort_run3pp_newmap_00079516_*.root\",\"${HISTS}\")"
root -b -q "plotKShortMass.C(\"${HISTS}\",\"${TAG}\")"
