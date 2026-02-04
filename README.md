# TRD Analysis Progress

## Date: 2026-02-04

### What I've Done:
- Installed O2Physics via CVMFS (no compilation needed!)
- Opened AO2D.root file from tutor
- Found TRD data in O2trdextra table
- Created plots of Q0, Q1, Q2, and total TRD signals
- Learned ROOT basics: TFile, TTree, TCanvas, TH1F

### Current Status:
- Need to find MC truth data for electron/pion labeling
- Waiting for tutor guidance on MC linking

### Files:
- explore_trd.C - Main analysis macro
- trd_all_signals.pdf - Current plots
- search_mc.C - MC data search script

### Next Goals:
1. Link TRD to MC truth
2. Plot electrons vs pions separately
3. Start building BDT features
