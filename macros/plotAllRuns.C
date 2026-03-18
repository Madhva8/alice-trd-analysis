void plotAllRuns() {

    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);

    TH2F* hDeDx = new TH2F("hDeDx",
        "TPC dE/dx vs Momentum - PbPb 2023 All Runs;"
        "p (GeV/c);TPC Signal (a.u.)",
        200, 0, 5, 200, 0, 300);
    hDeDx->SetDirectory(0);

    TH2F* hNSig = new TH2F("hNSig",
        "TPC n#sigma vs Momentum - PbPb 2023 All Runs;"
        "p (GeV/c);n#sigma_{TPC}",
        200, 0, 5, 200, -10, 10);
    hNSig->SetDirectory(0);

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

    long long totalTracks = 0;

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

            /* Read branches directly into variables */
            Float_t tpcSignal, tpcMom, nSigTPC;
            tree->SetBranchAddress("fTPCSignal", &tpcSignal);
            tree->SetBranchAddress("fTPCInnerParam", &tpcMom);
            tree->SetBranchAddress("fNSigTPC", &nSigTPC);

            Long64_t nEntries = tree->GetEntries();
            totalTracks += nEntries;
            cout << "  " << dirName << " — " << nEntries << " tracks" << endl;

            /* Loop and fill manually */
            for (Long64_t i = 0; i < nEntries; i++) {
                tree->GetEntry(i);
                if (tpcMom > 0 && tpcMom < 5) {
                    hDeDx->Fill(tpcMom, tpcSignal);
                    hNSig->Fill(tpcMom, nSigTPC);
                }
            }
        }
        f->Close();
    }

    cout << "\nTotal tracks: " << totalTracks << endl;
    cout << "hDeDx entries: " << hDeDx->GetEntries() << endl;

    TCanvas* c1 = new TCanvas("c1", "TPC PID All Runs - PbPb 2023", 1600, 700);
    c1->Divide(2, 1);

    c1->cd(1);
    gPad->SetLogz(); gPad->SetGrid();
    gPad->SetLeftMargin(0.13); gPad->SetBottomMargin(0.13);
    hDeDx->Draw("COLZ");

    c1->cd(2);
    gPad->SetLogz(); gPad->SetGrid();
    gPad->SetLeftMargin(0.13); gPad->SetBottomMargin(0.13);
    hNSig->Draw("COLZ");

    c1->Update();
    c1->SaveAs("plots/tpc_pid_all_runs.png");
    c1->SaveAs("plots/tpc_pid_all_runs.pdf");
    cout << "Saved!" << endl;
}
