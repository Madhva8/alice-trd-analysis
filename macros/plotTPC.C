void plotTPC() {
    TFile *f = TFile::Open("/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzo_apass5_250514_fewRuns/AO2D_merge_LHC23zzo.root");
    TTree *tree = (TTree*)f->Get("DF_2341027959650272/O2tpctofskimwde");

    TCanvas *c1 = new TCanvas("c1", "TPC dEdx vs Momentum", 800, 600);
    c1->SetLogz();

    TH2F *h = new TH2F("h", "TPC dE/dx vs Momentum;p (GeV/c);TPC Signal (a.u.)", 
                        200, 0, 5, 200, 0, 300);

    tree->Draw("fTPCSignal:abs(1.0/fSigned1Pt)>>h", 
               "abs(1.0/fSigned1Pt) < 5", "COLZ");

    c1->SaveAs("/home/madhvafakare/alice/my-analysis/tpc_dedx.png");
    cout << "Saved tpc_dedx.png" << endl;
}
