# KShort_run3pp

K_S0 → π⁺π⁻ reconstruction for sPHENIX Run 3 p+p data using KFParticle.

## Input

`DST_TRKR_TRACKS` files from the sPHENIX file catalog (Run 3 p+p,
production tag `ana538_2025p011_v001`).

## Running interactively

```bash
root -b -q 'Fun4All_KShortReco_run3pp.C(1000)'
```

Output lands in `KShort_run3pp/outputKFParticle_KShort_run3pp_RRRRRRRR_SSSSS_00000.root`.

## Condor submission

```bash
cd condor
./create_condor_list.sh [lustre_dst_dir] [output_dir] [max_jobs]
condor_submit condor.job
```

Output root files are collected in `$OUTDIR/root/`.

## Reconstruction settings

| Parameter | Value |
|-----------|-------|
| Decay | K_S0 → π⁺π⁻ |
| CDB tag | `newcdbtag` |
| Primary vertex constraint | None (K_S0 is displaced) |
| Min TPC hits | 20 |
| Max track χ²/nDOF | 100 |
| Max daughter DCA | 1.0 cm |
| Max vertex χ²/nDOF | 50 |
| Mass window | 300–700 MeV |

## Plotting

```bash
root -b -q 'plotKShortMass.C("root/KShort_run3pp_merged.root")'
```

Plots the π⁺π⁻ invariant mass with a reference line at the PDG K_S0 mass (497.6 MeV).
Output PDF is named after the input file.
