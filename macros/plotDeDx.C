/*
 * File: plotDeDx.C
 * Author: Madhva Fakare
 *
 * Plots TPC dE/dx vs momentum, nSigma vs momentum,
 * and nSigma distribution in a momentum slice.
 * Uses custom TPC skim data from Dr. Willems.
 *
 * Data: PbPb 2023 Run 3, apass5
 * Table: O2tpctofskimwde
 *
 * Key variables:
 *   fTPCSignal     — raw TPC dE/dx signal (your main PID observable)
 *   fTPCInnerParam — momentum at TPC inner wall
 *   fNSigTPC       — nSigma from TPC (how many sigma from expected for each species)
 *   fPidIndex      — particle species label (truth label for ML)
 *
 * Run with:
 *   root -l -q macros/plotDeDx.C
 */

void plotDeDx() {

    // ── Open data file ──────────────────────────────────────────────────────
    // This is the skimmed PbPb 2023 data from Dr. Willems
    // It is NOT the full ALICE AO2D — it is a custom TPC skim
    // containing only the variables needed for electron-pion separation
    const char* filepath =
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/"
        "LHC23zzo_apass5_250514_fewRuns/AO2D_merge_LHC23zzo.root";

    TFile* f = TFile::Open(filepath);
    if (!f || f->IsZombie()) {
        cout << "Error: Cannot open file!" << endl;
        return;
    }
    cout << "File opened successfully." << endl;

    // ── Get the TPC skim tree ───────────────────────────────────────────────
    // TTree is ROOT's table format — like a spreadsheet where each row
    // is one track and each column is one variable (branch)
    // DF_2341027959650272 is a dataframe directory — one chunk of collisions
    TTree* tree = (TTree*)f->Get("DF_2341027959650272/O2tpctofskimwde");
    if (!tree) {
        cout << "Error: Cannot find tree O2tpctofskimwde!" << endl;
        return;
    }
    cout << "Tree entries: " << tree->GetEntries() << endl;

    // ── Style ───────────────────────────────────────────────────────────────
    gStyle->SetOptStat(0);       // hide the statistics box on plots
    gStyle->SetPalette(kBird);   // clean blue-yellow colour palette

    // ── Canvas 1: Two 2D plots side by side ─────────────────────────────────
    // TCanvas is ROOT's drawing surface — like a figure in matplotlib
    // Divide(2,1) splits it into 2 columns, 1 row
    TCanvas* c1 = new TCanvas("c1", "TPC PID - PbPb 2023", 1600, 700);
    c1->Divide(2, 1);

    // ── Plot 1: TPC dE/dx vs Momentum (Bethe-Bloch bands) ──────────────────
    // This is the classic PID plot — different particle species form
    // distinct curved bands described by the Bethe-Bloch formula.
    // At low momentum pions, kaons, protons separate clearly.
    // Electrons sit above pions at low momentum due to higher dE/dx.
    // This is what your ML model will learn to separate.
    c1->cd(1);  // cd(1) = go to pad 1 (like subplot(1,2,1) in matplotlib)
    gPad->SetLogz();        // log scale on z (colour axis)
                            // needed because pions dominate by orders of magnitude
                            // and would wash out the electron signal
    gPad->SetGrid();
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);

    // TH2F = 2D histogram with float precision
    // Arguments: name, title, x-bins, x-min, x-max, y-bins, y-min, y-max
    TH2F* hDeDx = new TH2F("hDeDx",
        "TPC dE/dx vs Momentum - PbPb 2023;"  // title; x-axis label; y-axis label
        "p (GeV/c);"
        "TPC Signal (a.u.)",
        200, 0, 5,     // x: momentum 0-5 GeV/c in 200 bins
        200, 0, 300);  // y: dE/dx signal 0-300 in 200 bins

    // tree->Draw("y:x>>histogram", "selection cut", "draw option")
    // fTPCInnerParam is used as momentum — it's measured at the TPC inner wall
    // which is the correct reference point for the dE/dx measurement
    tree->Draw("fTPCSignal:fTPCInnerParam>>hDeDx",
               "fTPCInnerParam > 0 && fTPCInnerParam < 5", "COLZ");

    hDeDx->GetXaxis()->SetTitleSize(0.05);
    hDeDx->GetYaxis()->SetTitleSize(0.05);
    hDeDx->GetYaxis()->SetTitleOffset(1.2);

    // ── Plot 2: nSigma TPC vs Momentum ─────────────────────────────────────
    // nSigma = (measured dE/dx - expected dE/dx) / resolution
    // It tells you how many standard deviations a track is away from
    // what you would expect for a given particle species.
    // A track with nSigma = 0 is perfectly consistent with that species.
    // This is the direct input feature for your ML classifier.
    c1->cd(2);  // go to pad 2
    gPad->SetLogz();
    gPad->SetGrid();
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);

    TH2F* hNSig = new TH2F("hNSig",
        "TPC n#sigma vs Momentum - PbPb 2023;"  // #sigma renders as Greek sigma
        "p (GeV/c);"
        "n#sigma_{TPC}",                         // _{TPC} renders as subscript
        200, 0, 5,      // x: momentum 0-5 GeV/c
        200, -10, 10);  // y: nSigma range -10 to +10

    tree->Draw("fNSigTPC:fTPCInnerParam>>hNSig",
               "fTPCInnerParam > 0 && fTPCInnerParam < 5", "COLZ");

    hNSig->GetXaxis()->SetTitleSize(0.05);
    hNSig->GetYaxis()->SetTitleSize(0.05);
    hNSig->GetYaxis()->SetTitleOffset(1.2);

    // ── Plot 3: nSigma distribution in momentum slice 1-2 GeV/c ────────────
    // Instead of the full 2D plot, we slice at 1-2 GeV/c and look at
    // just the 1D nSigma distribution. This directly shows how separated
    // electrons and pions are — ideally you would see two distinct peaks.
    // This momentum range is chosen because TRD separation is most
    // powerful here — electrons emit transition radiation, pions don't.
    TCanvas* c2 = new TCanvas("c2", "nSigma slice - PbPb 2023", 800, 700);
    c2->SetGrid();
    c2->SetLeftMargin(0.13);
    c2->SetBottomMargin(0.13);

    // TH1F = 1D histogram with float precision
    TH1F* hSlice = new TH1F("hSlice",
        "TPC n#sigma distribution (1 < p < 2 GeV/c) - PbPb 2023;"
        "n#sigma_{TPC};"
        "Counts",
        200, -10, 10);  // 200 bins from -10 to +10 sigma

    hSlice->SetLineColor(kBlue+1);
    hSlice->SetLineWidth(2);
    hSlice->SetFillColor(kBlue-9);
    hSlice->SetFillStyle(1001);

    // Apply momentum slice cut: only tracks with 1 < p < 2 GeV/c
    tree->Draw("fNSigTPC>>hSlice",
               "fTPCInnerParam > 1.0 && fTPCInnerParam < 2.0", "");

    hSlice->GetXaxis()->SetTitleSize(0.05);
    hSlice->GetYaxis()->SetTitleSize(0.05);
    hSlice->GetYaxis()->SetTitleOffset(1.3);

    c2->Update();
    c2->SaveAs("plots/nsigma_slice_1_2GeV.png");
    c2->SaveAs("plots/nsigma_slice_1_2GeV.pdf");

    // ── Save main canvas ────────────────────────────────────────────────────
    c1->Update();
    c1->SaveAs("plots/tpc_pid_summary.png");
    c1->SaveAs("plots/tpc_pid_summary.pdf");

    cout << "\nAll plots saved to plots/" << endl;
    cout << "dE/dx entries: " << hDeDx->GetEntries() << endl;
    cout << "nSigma slice entries: " << hSlice->GetEntries() << endl;

    f->Close();
}