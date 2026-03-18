/*
 * File: fitBetheBloch.C
 * Author: Madhva Fakare
 *
 * Fits the empirical Bethe-Bloch formula to the pion band
 * in the TPC dE/dx vs momentum plot.
 *
 * Method:
 *   1. Slice the 2D dE/dx histogram into momentum bins
 *   2. Find the most probable value (MPV) in each slice
 *   3. Fit the Bethe-Bloch curve through the MPV points
 *
 * Bethe-Bloch empirical formula (ALICE convention):
 *   f(bg) = P1/beta^P4 * (P2 - beta^P4 - ln(P3 + 1/(bg)^P5))
 *   where bg = betagamma = p/mass
 *
 * Run with:
 *   root -l -q macros/fitBetheBloch.C
 */

/* Bethe-Bloch function for ROOT TF1
 * x[0] = momentum p (GeV/c)
 * par[0..4] = P1..P5
 * par[5] = particle mass (GeV/c^2) */
Double_t BetheBloch(Double_t* x, Double_t* par) {
    Double_t p    = x[0];
    Double_t mass = par[5];
    Double_t bg   = p / mass;               // betagamma
    Double_t beta = bg / TMath::Sqrt(1.0 + bg*bg);  // beta = v/c

    if (beta <= 0 || beta >= 1) return 0;

    Double_t P1 = par[0];
    Double_t P2 = par[1];
    Double_t P3 = par[2];
    Double_t P4 = par[3];
    Double_t P5 = par[4];

    return (P1 / TMath::Power(beta, P4)) *
           (P2 - TMath::Power(beta, P4) -
            TMath::Log(P3 + TMath::Power(1.0/bg, P5)));
}

void fitBetheBloch() {

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPalette(kBird);

    /* Load all runs into 2D histogram */
    TH2F* hDeDx = new TH2F("hDeDx",
        "TPC dE/dx vs Momentum - PbPb 2023 All Runs;"
        "p (GeV/c);TPC Signal (a.u.)",
        100, 0.1, 5, 100, 0, 300);
    hDeDx->SetDirectory(0);

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

            Float_t tpcSignal, tpcMom;
            tree->SetBranchAddress("fTPCSignal", &tpcSignal);
            tree->SetBranchAddress("fTPCInnerParam", &tpcMom);

            Long64_t nEntries = tree->GetEntries();
            for (Long64_t i = 0; i < nEntries; i++) {
                tree->GetEntry(i);
                if (tpcMom > 0.1 && tpcMom < 5.0)
                    hDeDx->Fill(tpcMom, tpcSignal);
            }
        }
        f->Close();
    }

    cout << "Total entries: " << hDeDx->GetEntries() << endl;

    /* Step 1: Extract MPV per momentum bin */
    /* MPV = most probable value = peak of dE/dx in each momentum slice */
    int nBins = hDeDx->GetNbinsX();
    vector<Double_t> pVec, dedxVec, errVec;

    for (int i = 1; i <= nBins; i++) {
        /* Get 1D projection of this momentum slice */
        TH1D* slice = hDeDx->ProjectionY("slice", i, i);

        if (slice->GetEntries() < 100) { delete slice; continue; }

        /* Find the bin with maximum content = MPV */
        int maxBin = slice->GetMaximumBin();
        Double_t mpv = slice->GetBinCenter(maxBin);
        Double_t p   = hDeDx->GetXaxis()->GetBinCenter(i);

        /* Error estimate from bin width */
        Double_t err = slice->GetBinWidth(maxBin);

        pVec.push_back(p);
        dedxVec.push_back(mpv);
        errVec.push_back(err);

        delete slice;
    }

    cout << "MPV points extracted: " << pVec.size() << endl;

    /* Create TGraphErrors from MPV points */
    TGraphErrors* gMPV = new TGraphErrors(
        pVec.size(),
        pVec.data(),
        dedxVec.data(),
        nullptr,
        errVec.data());

    gMPV->SetMarkerStyle(20);
    gMPV->SetMarkerSize(0.8);
    gMPV->SetMarkerColor(kBlack);
    gMPV->SetLineColor(kBlack);

    /* Step 2: Fit Bethe-Bloch through MPV points */
    /* Pion mass = 0.13957 GeV/c^2 */
    TF1* fitBB = new TF1("fitBB", BetheBloch, 0.1, 5.0, 6);

    /* Initial parameters — typical ALICE values for pions */
    fitBB->SetParameters(50.0, 2.5, 1e-3, 2.2, 2.0, 0.13957);
    fitBB->FixParameter(5, 0.13957);  /* fix pion mass */
    fitBB->SetParName(0, "P1");
    fitBB->SetParName(1, "P2");
    fitBB->SetParName(2, "P3");
    fitBB->SetParName(3, "P4");
    fitBB->SetParName(4, "P5");
    fitBB->SetParName(5, "mass");
    fitBB->SetLineColor(kRed);
    fitBB->SetLineWidth(2);

    gMPV->Fit(fitBB, "R");

    /* Draw everything */
    TCanvas* c1 = new TCanvas("c1", "Bethe-Bloch Fit - PbPb 2023", 900, 700);
    c1->SetLogx();
    c1->SetLogz();
    c1->SetGrid();
    c1->SetLeftMargin(0.13);
    c1->SetBottomMargin(0.13);

    hDeDx->Draw("COLZ");
    gMPV->Draw("P same");
    fitBB->Draw("same");

    /* Legend */
    TLegend* leg = new TLegend(0.45, 0.65, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->AddEntry(hDeDx, "TPC dE/dx data", "f");
    leg->AddEntry(gMPV, "Most Probable Value per bin", "p");
    leg->AddEntry(fitBB, "Bethe-Bloch fit (#pi^{-} mass)", "l");
    leg->Draw();

    cout << "\n=== Bethe-Bloch Fit Parameters ===" << endl;
    for (int i = 0; i < 5; i++)
        cout << "P" << i+1 << " = " << fitBB->GetParameter(i) << endl;

    c1->Update();
    c1->SaveAs("plots/bethebloch_fit.png");
    c1->SaveAs("plots/bethebloch_fit.pdf");
    cout << "Saved plots/bethebloch_fit.png" << endl;
}