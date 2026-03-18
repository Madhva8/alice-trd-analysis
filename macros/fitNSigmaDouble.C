void fitNSigmaDouble() {

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    TH1F* hSlice = new TH1F("hSlice",
        "TPC n#sigma distribution (1 < p < 2 GeV/c) - PbPb 2023 All Runs;"
        "n#sigma_{TPC};Counts",
        200, -10, 10);
    hSlice->SetDirectory(0);

    vector<TString> files = {
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzf_apass5_250514/AO2D_merge_LHC23zzf.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzg_apass5_250514/AO2D_merge_LHC23zzg.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzh_apass5_250514/AO2D_merge_LHC23zzh.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzi_apass5_250514/AO2D_merge_LHC23zzi.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzk_apass5_250514/AO2D_merge_LHC23zzk.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzl_apass5_250514/AO2D_merge_LHC23zzl.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzm_apass5_250514/AO2D_merge_LHC23zzm.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzn_apass5_250514/AO2D_merge_LHC23zzn.root",
        "/home/madhvafakare/alice/PbPb2023pass5/PbPb2023pass5/LHC23zzo_apass5_250514/AO2D_merge_LHC23zzo.root"
    };

    for (int iFile = 0; iFile < (int)files.size(); iFile++) {
        TFile* f = TFile::Open(files[iFile]);
        if (!f || f->IsZombie()) continue;
        cout << "File " << iFile+1 << "/" << files.size() << endl;

        TIter next(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)next())) {
            TString dirName = key->GetName();
            if (!dirName.BeginsWith("DF_")) continue;
            TTree* tree = (TTree*)f->Get(dirName + "/O2tpctofskimwde");
            if (!tree) continue;

            Float_t tpcMom, nSigTPC;
            tree->SetBranchAddress("fTPCInnerParam", &tpcMom);
            tree->SetBranchAddress("fNSigTPC", &nSigTPC);

            Long64_t nEntries = tree->GetEntries();
            for (Long64_t i = 0; i < nEntries; i++) {
                tree->GetEntry(i);
                if (tpcMom > 1.0 && tpcMom < 2.0)
                    hSlice->Fill(nSigTPC);
            }
        }
        f->Close();
    }

    cout << "Entries: " << hSlice->GetEntries() << endl;

    TCanvas* c1 = new TCanvas("c1", "nSigma Double Gaussian Fit", 900, 700);
    c1->SetGrid();
    c1->SetLogy();
    c1->SetLeftMargin(0.13);
    c1->SetBottomMargin(0.13);

    hSlice->SetLineColor(kBlue+1);
    hSlice->SetLineWidth(2);
    hSlice->SetFillColor(kBlue-9);
    hSlice->SetFillStyle(1001);
    hSlice->Draw();

    TF1* fitDouble = new TF1("fitDouble",
        "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[5])^2)",
        -5, 8);

    fitDouble->SetParameters(3.5e6, -0.5, 1.3, 1.0e4, 0.0, 1.0);
    fitDouble->SetParLimits(1, -2.0, 0.0);  // pion mean between -2 and 0
    fitDouble->SetParLimits(2, 0.5, 2.0);   // pion sigma reasonable
    fitDouble->SetParLimits(3, 100, 1e7);   // electron amplitude positive
    fitDouble->SetParLimits(4, 0.0, 5.0);   // electron mean must be > 0
    fitDouble->SetParLimits(5, 0.5, 2.0);   // electron sigma reasonable
    fitDouble->SetParName(0, "A_{#pi}");
    fitDouble->SetParName(1, "#mu_{#pi}");
    fitDouble->SetParName(2, "#sigma_{#pi}");
    fitDouble->SetParName(3, "A_{e}");
    fitDouble->SetParName(4, "#mu_{e}");
    fitDouble->SetParName(5, "#sigma_{e}");
    fitDouble->SetLineColor(kRed);
    fitDouble->SetLineWidth(2);
    hSlice->Fit(fitDouble, "R");

    TF1* fitPion = new TF1("fitPion",
        "[0]*exp(-0.5*((x-[1])/[2])^2)", -5, 8);
    fitPion->SetParameters(
        fitDouble->GetParameter(0),
        fitDouble->GetParameter(1),
        fitDouble->GetParameter(2));
    fitPion->SetLineColor(kGreen+2);
    fitPion->SetLineWidth(2);
    fitPion->SetLineStyle(2);
    fitPion->Draw("same");

    TF1* fitElec = new TF1("fitElec",
        "[0]*exp(-0.5*((x-[1])/[2])^2)", -5, 8);
    fitElec->SetParameters(
        fitDouble->GetParameter(3),
        fitDouble->GetParameter(4),
        fitDouble->GetParameter(5));
    fitElec->SetLineColor(kOrange+2);
    fitElec->SetLineWidth(2);
    fitElec->SetLineStyle(2);
    fitElec->Draw("same");

    TLegend* leg = new TLegend(0.55, 0.65, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->AddEntry(hSlice, "Data", "f");
    leg->AddEntry(fitDouble, "Double Gaussian fit", "l");
    leg->AddEntry(fitPion, "Pion component", "l");
    leg->AddEntry(fitElec, "Electron component", "l");
    leg->Draw();

    cout << "\n=== Fit Results ===" << endl;
    cout << "Pion  — Mean: " << fitDouble->GetParameter(1)
         << "  Sigma: " << fitDouble->GetParameter(2) << endl;
    cout << "Electron — Mean: " << fitDouble->GetParameter(4)
         << "  Sigma: " << fitDouble->GetParameter(5) << endl;
    cout << "Separation: "
         << TMath::Abs(fitDouble->GetParameter(4) - fitDouble->GetParameter(1))
         << " sigma" << endl;

    c1->Update();
    c1->SaveAs("plots/nsigma_double_fit.png");
    c1->SaveAs("plots/nsigma_double_fit.pdf");
    cout << "Saved!" << endl;
}