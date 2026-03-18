# ALICE TRD Analysis — Electron-Pion Separation

Master's thesis project on electron-pion separation using the Transition Radiation Detector (TRD) in ALICE Run 3.

**Author:** Madhva Fakare — MSc Physics, University of Münster  
**Supervisor:** Dr. Guido Willems, Prof. Anton Andronic (AG Andronic)

---

## Physics Goal

Separate electrons from pions in PbPb collisions at 5.36 TeV using TRD signals and machine learning. The TRD exploits transition radiation — photons emitted when ultra-relativistic electrons cross material boundaries — to distinguish electrons from the far more abundant pion background.

---

## Dataset

**PbPb 2023 Run 3 data — apass5**  
Custom TPC skim prepared by Dr. Willems, stored at:
```
~/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzh_apass5_250514/AO2D_merge_LHC23zzh.root
```

Available runs: LHC23zzf, LHC23zzg, LHC23zzh, LHC23zzi, LHC23zzk, LHC23zzl, LHC23zzm, LHC23zzn, LHC23zzo

**Table:** `O2tpctofskimwde` — 78,721,000 tracks per merged file

| Branch | Description | Role in Analysis |
|--------|-------------|-----------------|
| `fTPCSignal` | Raw TPC dE/dx signal | Main PID observable |
| `fTPCInnerParam` | Momentum at TPC inner wall | X-axis of Bethe-Bloch plot |
| `fNSigTPC` | nσ from TPC | Electron/pion separation metric |
| `fNSigTOF` | nσ from TOF | Additional PID input |
| `fNSigITS` | nσ from ITS | Additional PID input |
| `fPidIndex` | Particle species label | Truth label for ML training |
| `fSigned1Pt` | Signed 1/pT | Momentum and charge |
| `fEta` / `fPhi` | Track angles | Kinematic variables |
| `fBetaGamma` | Relativistic βγ | Bethe-Bloch variable |
| `fNormNClustersTPC` | TPC cluster quality | Track quality cuts |
| `fRunNumber` | LHC run number | Run-by-run corrections |

---

## Project Structure
```
my-analysis/
├── tasks/
│   └── myFirstTask.cxx       # Minimal O2Physics task — learning example
├── macros/
│   ├── plotDeDx.C            # TPC dE/dx vs momentum + nSigma plots (main)
│   ├── plotTPC.C             # TPC dE/dx quick plot
│   ├── plotNSigma.C          # nSigma quick plot
│   ├── explore_trd.C         # TRD Q0/Q1/Q2 signal exploration
│   ├── plot_momentum.C       # Track momentum distributions
│   ├── check_branches.C      # Utility: inspect AO2D branch structure
│   └── search_mc.C           # Utility: search for MC truth tables
├── plots/                    # Generated figures
├── CMakeLists.txt            # O2Physics build definition
└── README.md
```

---

## How To Run

### Enter the O2Physics environment (always first)
```bash
cd ~/alice
alienv enter O2Physics/latest-master-o2
```

### Run ROOT macros
```bash
cd ~/alice/my-analysis

# Main TPC PID plots — dE/dx vs momentum and nSigma vs momentum
root -l -q macros/plotDeDx.C

# TRD charge signal exploration (Q0, Q1, Q2)
root -l -q macros/explore_trd.C

# Track momentum distributions
root -l -q macros/plot_momentum.C
```

### Inspect your data file
```bash
# See what directories exist
root -l -q -e 'TFile f("path/to/AO2D.root"); f.ls();' 2>/dev/null

# See what tables exist inside a dataframe
root -l -q -e '
  TFile f("path/to/AO2D.root");
  f.cd("DF_2336870310417472");
  gDirectory->ls();
' 2>/dev/null

# Print all branches of a table
root -l -q -e '
  TFile f("path/to/AO2D.root");
  f.cd("DF_2336870310417472");
  TTree* t = (TTree*)gDirectory->Get("O2tpctofskimwde");
  t->Print();
' 2>/dev/null
```

---

## Progress

- [x] TRD signal visualisation (Q0, Q1, Q2)
- [x] TPC dE/dx vs momentum plot
- [x] nSigma vs momentum plot
- [x] Dataset structure understood — 78M tracks, custom TPC skim
- [x] O2Physics environment set up via CVMFS alienv
- [x] Minimal O2Physics task written and understood
- [x] Git version control established
- [ ] Custom table integration in O2Physics task
- [ ] Track quality cuts
- [ ] ML model — BDT baseline (TMVA)
- [ ] ML model — Neural network
- [ ] Systematic uncertainties

---

## Next Steps

1. Get custom table header from Dr. Willems (`O2tpctofskimwde` definition)
2. Write O2Physics task that reads the skim directly
3. Apply track quality cuts using `fNormNClustersTPC`
4. Build BDT classifier using TMVA with features: `fTPCSignal`, `fNSigTPC`, `fBetaGamma`, `fEta`
5. Evaluate separation performance vs classical nSigma cut

---

## Environment

- **Framework:** O2Physics/latest-master-o2 via CVMFS alienv
- **Language:** C++17, ROOT
- **Machine:** AlmaLinux 9, HP EliteBook 845 G8
- **CERN:** lxplus access via SSH
