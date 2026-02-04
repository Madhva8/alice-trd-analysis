# ALICE TRD Analysis - Electron-Pion Separation

Master's thesis project on electron-pion separation using the Transition Radiation Detector (TRD) in the ALICE experiment at CERN.

## Project Overview

Using machine learning (Boosted Decision Trees) to separate electrons from pions based on TRD signals in Run 3 data.

## Current Progress

- ✅ Set up O2Physics environment via CVMFS
- ✅ Analyzed TRD data structure (Q0, Q1, Q2 signals)
- ✅ Created visualization macros for TRD signals
- 🔄 Working on MC truth matching for particle labeling

## Repository Structure
```
├── macros/          # ROOT analysis macros
├── plots/           # Generated plots and figures
├── data/            # Sample data files (not tracked)
├── docs/            # Documentation and notes
└── README.md        # This file
```

## Setup Instructions
```bash
# On lxplus or local AlmaLinux with CVMFS
alienv enter O2Physics/latest-master-o2
cd macros
root -l explore_trd.C
```

## Requirements

- O2Physics environment (via CVMFS or local build)
- ROOT 6.x
- AO2D.root data file

## Author

Madhva Fakare - Master's Student in Physics  
Thesis: TRD-based electron-pion separation using ML

## Acknowledgments

- ALICE Collaboration
- Dr. Guido Willems
- CERN

## License

This project is part of academic research. Code is provided for educational purposes.
