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

## DecayTree branch reference

### Event / bookkeeping

| Branch | Type | Description |
|--------|------|-------------|
| `runNumber` | `Int_t` | Run number |
| `eventNumber` | `Int_t` | Event number |
| `event_bco` | `Long64_t` | Event bunch crossing offset |
| `BCO` | `Long64_t` | Bunch crossing number |
| `last_event_bco` | `Long64_t` | Last event BCO |
| `nPrimaryVerticesOfBC` | `Int_t` | Number of primary vertices in bunch crossing |
| `nTracksOfBC` | `Int_t` | Number of tracks in bunch crossing |
| `nTracksOfVertex` | `Int_t` | Number of tracks at decay vertex (always 2) |

### K_S0 candidate (mother)

| Branch | Type | Description |
|--------|------|-------------|
| `K_S0_mass` | `Float_t` | Invariant mass (GeV/c²) |
| `K_S0_massErr` | `Float_t` | Mass uncertainty |
| `K_S0_x/y/z` | `Float_t` | Decay vertex position (cm) |
| `K_S0_px/py/pz` | `Float_t` | Momentum components (GeV/c) |
| `K_S0_pE` | `Float_t` | Energy (GeV) |
| `K_S0_p` | `Float_t` | Total momentum (GeV/c) |
| `K_S0_pErr` | `Float_t` | Momentum uncertainty |
| `K_S0_pT` | `Float_t` | Transverse momentum (GeV/c) |
| `K_S0_pTErr` | `Float_t` | pT uncertainty |
| `K_S0_pseudorapidity` | `Float_t` | Pseudorapidity η |
| `K_S0_rapidity` | `Float_t` | Rapidity y |
| `K_S0_theta` | `Float_t` | Polar angle (rad) |
| `K_S0_phi` | `Float_t` | Azimuthal angle (rad) |
| `K_S0_charge` | `Char_t` | Charge (always 0) |
| `K_S0_chi2` | `Float_t` | Vertex fit χ² |
| `K_S0_nDoF` | `UInt_t` | Vertex fit degrees of freedom |
| `K_S0_PDG_ID` | `Int_t` | PDG ID (310) |
| `K_S0_vertex_volume` | `Float_t` | Vertex volume |
| `K_S0_Covariance[21]` | `Float_t` | KFParticle covariance matrix |

### Daughter tracks (track_1 = π⁺, track_2 = π⁻; same branches for each)

| Branch | Type | Description |
|--------|------|-------------|
| `track_N_mass` | `Float_t` | Track mass hypothesis (GeV/c²) |
| `track_N_x/y/z` | `Float_t` | Track position at vertex (cm) |
| `track_N_px/py/pz` | `Float_t` | Momentum components (GeV/c) |
| `track_N_pE` | `Float_t` | Energy (GeV) |
| `track_N_p` | `Float_t` | Total momentum (GeV/c) |
| `track_N_pT` | `Float_t` | Transverse momentum (GeV/c) |
| `track_N_jT` | `Float_t` | Transverse momentum relative to mother |
| `track_N_pseudorapidity` | `Float_t` | Pseudorapidity η |
| `track_N_rapidity` | `Float_t` | Rapidity y |
| `track_N_theta` | `Float_t` | Polar angle (rad) |
| `track_N_phi` | `Float_t` | Azimuthal angle (rad) |
| `track_N_charge` | `Char_t` | Track charge (±1) |
| `track_N_chi2` | `Float_t` | Track fit χ² |
| `track_N_nDoF` | `UInt_t` | Track fit degrees of freedom |
| `track_N_track_ID` | `Int_t` | Track ID in the DST |
| `track_N_PDG_ID` | `Int_t` | PDG ID (211 = π) |
| `track_N_bunch_crossing` | `Int_t` | Bunch crossing of track |
| `track_N_Covariance[21]` | `Float_t` | KFParticle covariance matrix |

### Pair variables

| Branch | Type | Description |
|--------|------|-------------|
| `track_1_track_2_DCA` | `Float_t` | 3D distance of closest approach between daughters (cm) |
| `track_1_track_2_DCA_xy` | `Float_t` | Transverse DCA between daughters (cm) |
| `secondary_vertex_mass_pionPID` | `Float_t` | Invariant mass with explicit pion mass hypothesis |

## Plotting

```bash
root -b -q 'plotKShortMass.C("root/KShort_run3pp_merged.root")'
```

Plots the π⁺π⁻ invariant mass with a reference line at the PDG K_S0 mass (497.6 MeV).
Output PDF is named after the input file.
