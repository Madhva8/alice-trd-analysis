/*
 * File: fitNSigma.C
 * Author: Madhva Fakare
 *
 * Fits a Gaussian to the TPC nSigma distribution at 1-2 GeV/c
 * using all PbPb 2023 runs combined.
 *
 * Physics goal:
 *   - Find the pion peak position and width
 *   - Look for electron signal
 *   - Quantify separation — this is your ML baseline
 *
 * Run with:
 *   root -l -q macros/fitNSigma.C
 */

void fitNSigma() {

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);  /* show fit parameters on plot */

    /* Combined histogram from all runs */
    TH1F* hSlice = new TH1F("hSlice",
        "TPC n#sigma distribution (1 < p < 2 GeV/c) - PbPb 2023 All Runs;"
        "n#sigma_{TPC};Counts",
        200, -10, 10);
    hSlice->SetDirectory(0);

    /* Load all runs */
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
                /* Only tracks in 1-2 GeV/c momentum slice */
                if (tpcMom > 1.0 && tpcMom < 2.0) {
                    hSlice->Fill(nSigTPC);
                }
            }
        }
        f->Close();
    }

    cout << "Total entries in slice: " << hSlice->GetEntries() << endl;

    /* Canvas */
    TCanvas* c1 = new TCanvas("c1", "nSigma Fit - PbPb 2023", 900, 700);
    c1->SetGrid();
    c1->SetLeftMargin(0.13);
    c1->SetBottomMargin(0.13);
    c1->SetLogy();  /* log scale to see electron peak if present */

    hSlice->SetLineColor(kBlue+1);
    hSlice->SetLineWidth(2);
    hSlice->SetFillColor(kBlue-9);
    hSlice->SetFillStyle(1001);
    hSlice->Draw();

    /* Gaussian fit on pion peak region (-3 to +2) */
    /* TF1 = ROOT's function object */
    /* "gaus" = built-in Gaussian: A * exp(-0.5 * ((x-mean)/sigma)^2) */
    TF1* fitPion = new TF1("fitPion", "gaus", -3, 2);
    fitPion->SetLineColor(kRed);
    fitPion->SetLineWidth(2);

    /* Fit — "R" means fit only in the range specified above */
    hSlice->Fit(fitPion, "R");

    cout << "\n=== Pion peak fit ===" << endl;
    cout << "Mean:  " << fitPion->GetParameter(1) << " +/- " << fitPion->GetParError(1) << endl;
    cout << "Sigma: " << fitPion->GetParameter(2) << " +/- " << fitPion->GetParError(2) << endl;

    c1->Update();
    c1->SaveAs("plots/nsigma_fit.png");
    c1->SaveAs("plots/nsigma_fit.pdf");
    cout << "\nSaved plots/nsigma_fit.png" << endl;
}
