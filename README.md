# ALICE TRD Analysis - Electron-Pion Separation

Master's thesis project on electron-pion separation using the Transition Radiation Detector (TRD) in ALICE.

## Overview

Using TRD signals (Q0, Q1, Q2) to separate electrons from pions with machine learning techniques.

## Current Progress

- ✅ TRD signal visualization
- ✅ Q0, Q1, Q2 analysis
- ✅ Working on MC truth matching
- ✅ ROOT basics and project setup
- ✅ Git version control established

## Recent Work (February 2026)

### Learning ROOT Fundamentals
- Created first ROOT histogram plotting TRD charge signals
- Established proper project structure with `macros/`, `plots/`, and `docs/` directories
- Practiced basic ROOT operations: TH1F histograms, TCanvas, styling, and file saving
- Set up development workflow with VS Code remote connection to lxplus

### Version Control Setup
- Initialized git repository for thesis project
- Created GitHub repository: `alice-trd-analysis`
- Learning git fundamentals: add, commit, push workflow
- Configured git with CERN email for proper attribution

### Next Steps
- Continue ROOT tutorials focusing on TTree operations
- Explore actual TRD data structures in O2Physics
- Begin understanding TMVA framework for BDT implementation

## Structure

- `macros/` - ROOT analysis scripts
- `plots/` - Generated figures
- `docs/` - Documentation

## Usage
```bash
alienv enter O2Physics/latest-master-o2
cd macros
# Explore TRD charge signals
root -l explore_trd.C

# Plot momentum distribution
root -l plot_momentum.C
```

## Author

Madhva Fakare - Physics Master's Student
