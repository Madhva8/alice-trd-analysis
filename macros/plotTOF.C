/*
 * File: plotTOF.C
 * Author: Madhva Fakare
 *
 * Plots TOF nSigma vs momentum from custom TPC skim data.
 *
 * Key observation:
 *   - Band at nSigma = +4 across all momenta = unmatched tracks (no TOF hit)
 *   - Real TOF separation only visible below ~0.5 GeV/c
 *   - Above 1 GeV/c TOF loses discrimination power — particles too fast
 *   - Conclusion: TPC + TRD are the primary PID tools for this analysis
 *
 * Run with:
 *   root -l -q macros/plotTOF.C
 */

void plotTOF() {

    TFile* f = TFile::Open(
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/"
        "LHC23zzo_apass5_250514_fewRuns/AO2D_merge_LHC23zzo.root");

    if (!f || f->IsZombie()) { cout << "Error opening file!" << endl; return; }

    TTree* t = (TTree*)f->Get("DF_2341027959650272/O2tpctofskimwde");
    if (!t) { cout << "Error finding tree!" << endl; return; }

    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);

    TCanvas* c = new TCanvas("c", "TOF nSigma vs Momentum - PbPb 2023", 800, 700);
    c->SetLogz();
    c->SetGrid();
    c->SetLeftMargin(0.13);
    c->SetBottomMargin(0.13);

    TH2F* h = new TH2F("h",
        "TOF n#sigma vs Momentum - PbPb 2023;"
        "p (GeV/c);"
        "n#sigma_{TOF}",
        200, 0, 5, 200, -10, 10);

    t->Draw("fNSigTOF:fTPCInnerParam>>h",
            "fTPCInnerParam > 0 && fTPCInnerParam < 5", "COLZ");

    c->SaveAs("plots/nsigma_tof_vs_momentum.png");
    c->SaveAs("plots/nsigma_tof_vs_momentum.pdf");

    cout << "Saved plots/nsigma_tof_vs_momentum.png" << endl;
    f->Close();
}
