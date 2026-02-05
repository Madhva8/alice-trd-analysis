void plot_momentum() {
    TFile* f = TFile::Open("/home/madhvafakare/Downloads/AO2D.root");
    f->cd("DF_2336986335323680");
    
    TTree* tracks = (TTree*)gDirectory->Get("O2track_iu");
    
    cout << "Track entries: " << tracks->GetEntries() << endl;
    
    gStyle->SetOptStat(1111);
    
    TCanvas* c1 = new TCanvas("c1", "Track Momentum", 1400, 1000);
    c1->Divide(2, 2);
    
    // Plot 1: Signed 1/pT
    c1->cd(1);
    gPad->SetLogy();
    gPad->SetGrid();
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    
    TH1F* h1 = new TH1F("h1", "Track Signed 1/pT;Signed 1/pT [GeV/c]^{-1};Counts", 
                        100, -5, 5);
    h1->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    h1->SetFillColor(kBlue);
    h1->SetFillStyle(3004);
    
    tracks->Draw("fSigned1Pt>>h1", "abs(fSigned1Pt) < 100");  // Cut extreme values
    h1->GetXaxis()->SetNdivisions(506);
    
    // Plot 2: pT (calculated as 1/|fSigned1Pt|)
    c1->cd(2);
    gPad->SetLogy();
    gPad->SetGrid();
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    
    TH1F* h2 = new TH1F("h2", "Track pT;pT [GeV/c];Counts", 
                        100, 0, 10);
    h2->SetLineColor(kRed);
    h2->SetLineWidth(2);
    h2->SetFillColor(kRed);
    h2->SetFillStyle(3004);
    
    tracks->Draw("1.0/abs(fSigned1Pt)>>h2", "abs(fSigned1Pt) > 0.01 && abs(fSigned1Pt) < 100");
    h2->GetXaxis()->SetNdivisions(506);
    
    // Plot 3: Track angle
    c1->cd(3);
    gPad->SetLogy();
    gPad->SetGrid();
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    
    TH1F* h3 = new TH1F("h3", "Track Dip Angle;tan(#lambda);Counts", 
                        100, -2, 2);
    h3->SetLineColor(kGreen+2);
    h3->SetLineWidth(2);
    h3->SetFillColor(kGreen+2);
    h3->SetFillStyle(3004);
    
    tracks->Draw("fTgl>>h3");
    h3->GetXaxis()->SetNdivisions(506);
    
    // Plot 4: Track type
    c1->cd(4);
    gPad->SetGrid();
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    
    TH1F* h4 = new TH1F("h4", "Track Type;Track Type;Counts", 
                        10, -0.5, 9.5);
    h4->SetLineColor(kMagenta+2);
    h4->SetLineWidth(2);
    h4->SetFillColor(kMagenta+2);
    h4->SetFillStyle(3004);
    
    tracks->Draw("fTrackType>>h4");
    
    c1->Update();
    c1->SaveAs("../plots/momentum_dist.png");
    c1->SaveAs("../plots/momentum_dist.pdf");
    
    cout << "\n=== Statistics ===" << endl;
    cout << "1/pT - Mean: " << h1->GetMean() << ", RMS: " << h1->GetRMS() << endl;
    cout << "pT - Mean: " << h2->GetMean() << ", RMS: " << h2->GetRMS() << endl;
    cout << "Dip angle - Mean: " << h3->GetMean() << ", RMS: " << h3->GetRMS() << endl;
    
    cout << "\nPlots saved!" << endl;
}