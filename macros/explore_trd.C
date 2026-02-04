void explore_trd() {
    // Open the AO2D file
    TFile* f = TFile::Open("/home/madhvafakare/Downloads/AO2D.root");
    
    if (!f || f->IsZombie()) {
        cout << "Error: Cannot open file!" << endl;
        return;
    }
    
    cout << "File opened successfully!" << endl;
    
    // Navigate to the data directory
    f->cd("DF_2336986335323680");
    
    // Get TRD tree
    TTree* trd = (TTree*)gDirectory->Get("O2trdextra");
    cout << "TRD entries: " << trd->GetEntries() << endl;
    
    // Get tracks tree
    TTree* tracks = (TTree*)gDirectory->Get("O2track_iu");
    cout << "Track entries: " << tracks->GetEntries() << endl;
    
    // Search for MC data in all directories
    cout << "\n=== Searching for MC data ===" << endl;
    f->cd();
    TIter next(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TString name = key->GetName();
        if (name.Contains("DF_")) {
            f->cd(name);
            // Check if this directory has MC particles
            TTree* mctest = (TTree*)gDirectory->Get("O2mcparticle_001");
            if (mctest) {
                cout << "Found O2mcparticle_001 in " << name << " with " 
                     << mctest->GetEntries() << " entries!" << endl;
            }
            // Check for MC labels
            TTree* mclabel = (TTree*)gDirectory->Get("O2mctracklabel_iu");
            if (mclabel) {
                cout << "Found O2mctracklabel_iu in " << name << " with " 
                     << mclabel->GetEntries() << " entries!" << endl;
            }
        }
    }
    
    // Go back to TRD directory
    f->cd("DF_2336986335323680");
    
    // Set better style
    gStyle->SetOptStat(1111);
    gStyle->SetPalette(kRainBow);
    
    // Create canvas with 2x2 grid
    TCanvas* c1 = new TCanvas("c1", "TRD Signals Analysis", 1600, 1200);
    c1->Divide(2, 2);  // 2 columns, 2 rows
    
    // ========== Plot 1: Q0 Signal ==========
    c1->cd(1);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    gPad->SetLogy();
    gPad->SetGrid();
    
    TH1F* h1 = new TH1F("h1", "TRD Q0 Signal;Q0 [a.u.];Counts", 100, 0, 50000);
    h1->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    h1->SetFillColor(kBlue);
    h1->SetFillStyle(3004);
    
    trd->Draw("fTRDQ0sCorrected[0]>>h1", "fTRDQ0sCorrected[0] > 0");
    h1->GetXaxis()->SetNdivisions(506);
    h1->GetXaxis()->SetLabelSize(0.045);
    h1->GetXaxis()->SetTitleSize(0.05);
    
    // ========== Plot 2: Q1 Signal ==========
    c1->cd(2);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    gPad->SetLogy();
    gPad->SetGrid();
    
    TH1F* h2 = new TH1F("h2", "TRD Q1 Signal;Q1 [a.u.];Counts", 100, 0, 30000);
    h2->SetLineColor(kGreen+2);
    h2->SetLineWidth(2);
    h2->SetFillColor(kGreen+2);
    h2->SetFillStyle(3004);
    
    trd->Draw("fTRDQ1sCorrected[0]>>h2", "fTRDQ1sCorrected[0] > 0");
    h2->GetXaxis()->SetNdivisions(506);
    h2->GetXaxis()->SetLabelSize(0.045);
    h2->GetXaxis()->SetTitleSize(0.05);
    
    // ========== Plot 3: Q2 Signal ==========
    c1->cd(3);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    gPad->SetLogy();
    gPad->SetGrid();
    
    TH1F* h3 = new TH1F("h3", "TRD Q2 Signal;Q2 [a.u.];Counts", 100, 0, 50000);
    h3->SetLineColor(kMagenta+2);
    h3->SetLineWidth(2);
    h3->SetFillColor(kMagenta+2);
    h3->SetFillStyle(3004);
    
    trd->Draw("fTRDQ2sCorrected[0]>>h3", "fTRDQ2sCorrected[0] > 0");
    h3->GetXaxis()->SetNdivisions(506);
    h3->GetXaxis()->SetLabelSize(0.045);
    h3->GetXaxis()->SetTitleSize(0.05);
    
    // ========== Plot 4: Total Signal (Q0+Q1+Q2) ==========
    c1->cd(4);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    gPad->SetLogy();
    gPad->SetGrid();
    
    TH1F* h4 = new TH1F("h4", "Total TRD Signal;Q_{total} [a.u.];Counts", 100, 0, 150000);
    h4->SetLineColor(kRed);
    h4->SetLineWidth(2);
    h4->SetFillColor(kRed);
    h4->SetFillStyle(3004);
    
    trd->Draw("fTRDQ0sCorrected[0] + fTRDQ1sCorrected[0] + fTRDQ2sCorrected[0]>>h4", 
              "fTRDQ0sCorrected[0] > 0");
    h4->GetXaxis()->SetNdivisions(506);
    h4->GetXaxis()->SetLabelSize(0.045);
    h4->GetXaxis()->SetTitleSize(0.05);
    
    // Update canvas
    c1->Update();
    
    // Save the canvas
    c1->SaveAs("trd_all_signals.png");
    c1->SaveAs("trd_all_signals.pdf");
    cout << "\nPlots saved as trd_all_signals.png and trd_all_signals.pdf" << endl;
    
    // Print summary statistics
    cout << "\n=== TRD Signal Statistics ===" << endl;
    cout << "Q0 - Mean: " << h1->GetMean() << ", RMS: " << h1->GetRMS() << endl;
    cout << "Q1 - Mean: " << h2->GetMean() << ", RMS: " << h2->GetRMS() << endl;
    cout << "Q2 - Mean: " << h3->GetMean() << ", RMS: " << h3->GetRMS() << endl;
    cout << "Q_total - Mean: " << h4->GetMean() << ", RMS: " << h4->GetRMS() << endl;
}