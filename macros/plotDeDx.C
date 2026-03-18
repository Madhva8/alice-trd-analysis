/*
 * File: plotDeDx.C
 * Author: Madhva Fakare
 *
 * Plots TPC dE/dx vs momentum and nSigma vs momentum
 * using the custom TPC skim data from Dr. Willems.
 *
 * Data: PbPb 2023 Run 3, apass5
 * Table: O2tpctofskimwde
 *
 * Key variables:
 *   fTPCSignal    — raw TPC dE/dx signal (your main PID observable)
 *   fTPCInnerParam — momentum at TPC inner wall
 *   fNSigTPC      — nSigma from TPC (how many sigma from expected for each species)
 *   fPidIndex     — particle species label (truth label for ML)
 *
 * Run with:
 *   root -l -q macros/plotDeDx.C
 */

void plotDeDx() {

    // ── Open data file ──────────────────────────────────────────────────────
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
    // O2tpctofskimwde is the custom table Dr. Willems prepared
    // It contains TPC + TOF PID variables for electron-pion separation
    TTree* tree = (TTree*)f->Get("DF_2341027959650272/O2tpctofskimwde");
    if (!tree) {
        cout << "Error: Cannot find tree O2tpctofskimwde!" << endl;
        return;
    }
    cout << "Tree entries: " << tree->GetEntries() << endl;

    // ── Style ───────────────────────────────────────────────────────────────
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);  // clean blue-yellow colour palette

    // ── Canvas: 2 plots side by side ────────────────────────────────────────
    TCanvas* c1 = new TCanvas("c1", "TPC PID — PbPb 2023", 1600, 700);
    c1->Divide(2, 1);

    // ── Plot 1: TPC dE/dx vs Momentum (Bethe-Bloch bands) ──────────────────
    // This is the classic PID plot — different particle species form
    // distinct bands due to the Bethe-Bloch energy loss formula.
    // Electrons sit above pions at low momentum.
    c1->cd(1);
    gPad->SetLogz();   // log scale on z (colour axis) — needed because
                       // pions dominate and would wash out everything else
    gPad->SetGrid();
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);

    TH2F* hDeDx = new TH2F("hDeDx",
        "TPC dE/dx vs Momentum — PbPb 2023;"
        "p (GeV/c);"
        "TPC Signal (a.u.)",
        200, 0, 5,    // x: momentum 0–5 GeV/c
        200, 0, 300); // y: dE/dx signal

    // fTPCInnerParam = momentum at TPC inner wall
    // fSigned1Pt     = signed 1/pT — take absolute value and invert for momentum
    // We use fTPCInnerParam directly as it's the correct momentum for dE/dx
    tree->Draw("fTPCSignal:fTPCInnerParam>>hDeDx",
               "fTPCInnerParam > 0 && fTPCInnerParam < 5", "COLZ");

    hDeDx->GetXaxis()->SetTitleSize(0.05);
    hDeDx->GetYaxis()->SetTitleSize(0.05);
    hDeDx->GetYaxis()->SetTitleOffset(1.2);

    // ── Plot 2: nSigma TPC vs Momentum ─────────────────────────────────────
    // nSigma = how many standard deviations a track is from the expected
    // dE/dx for a given particle species.
    // Electrons cluster around nSigma = 0, pions are displaced.
    // This is the direct input to your ML classifier.
    c1->cd(2);
    gPad->SetLogz();
    gPad->SetGrid();
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.13);

    TH2F* hNSig = new TH2F("hNSig",
        "TPC n#sigma vs Momentum — PbPb 2023;"
        "p (GeV/c);"
        "n#sigma_{TPC}",
        200, 0, 5,    // x: momentum 0–5 GeV/c
        200, -10, 10); // y: nSigma range

    tree->Draw("fNSigTPC:fTPCInnerParam>>hNSig",
               "fTPCInnerParam > 0 && fTPCInnerParam < 5", "COLZ");

    hNSig->GetXaxis()->SetTitleSize(0.05);
    hNSig->GetYaxis()->SetTitleSize(0.05);
    hNSig->GetYaxis()->SetTitleOffset(1.2);

    // ── Save ────────────────────────────────────────────────────────────────
    c1->Update();
    c1->SaveAs("plots/tpc_pid_summary.png");
    c1->SaveAs("plots/tpc_pid_summary.pdf");

    cout << "\nPlots saved to plots/tpc_pid_summary.png" << endl;
    cout << "Entries plotted: " << hDeDx->GetEntries() << endl;

    f->Close();
}
