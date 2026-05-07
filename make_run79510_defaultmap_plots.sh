#!/bin/bash
set -e

HISTS=root/KShort_hists_run79510_defaultmap.root
TAG=plots/KShort_run79510_defaultmap

root -b -q "fillKShortHists.C(\"root/outputKFParticle_KShort_run3pp_00079510_*.root\",\"${HISTS}\")"
root -b -q "plotKShortMass.C(\"${HISTS}\",\"${TAG}\")"
