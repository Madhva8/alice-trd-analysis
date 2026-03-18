/*
 * File: fitBetheBloch.C
 * Author: Madhva Fakare
 *
 * Fits the empirical Bethe-Bloch formula to both the pion and
 * electron bands in the TPC dE/dx vs momentum plot.
 *
 * Method:
 *   1. Slice the 2D dE/dx histogram into momentum bins
 *   2. Extract MPV for pions (global max) and electrons (max above threshold)
 *   3. Fit Bethe-Bloch curves separately for each species
 *
 * Bethe-Bloch empirical formula (ALICE convention):
 *   f(bg) = P1/beta^P4 * (P2 - beta^P4 - ln(P3 + 1/(bg)^P5))
 *   where bg = betagamma = p/mass
 *
 * Particle masses (GeV/c^2):
 *   Pion:     0.13957
 *   Electron: 0.000511
 *
 * Run with:
 *   root -l -q macros/fitBetheBloch.C
 */

Double_t BetheBloch(Double_t* x, Double_t* par) {
    Double_t p    = x[0];
    Double_t mass = par[5];
    Double_t bg   = p / mass;
    Double_t beta = bg / TMath::Sqrt(1.0 + bg*bg);
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

    /* Extract MPV separately for pions and electrons */
    int nBins = hDeDx->GetNbinsX();
    vector<Double_t> pVec_pi, dedxVec_pi, errVec_pi;
    vector<Double_t> pVec_e,  dedxVec_e,  errVec_e;

    for (int i = 1; i <= nBins; i++) {
        TH1D* slice = hDeDx->ProjectionY("slice", i, i);
        if (slice->GetEntries() < 100) { delete slice; continue; }

        Double_t p = hDeDx->GetXaxis()->GetBinCenter(i);

        /* Pion MPV — global maximum of the slice */
        int maxBin_pi = slice->GetMaximumBin();
        Double_t mpv_pi = slice->GetBinCenter(maxBin_pi);
        pVec_pi.push_back(p);
        dedxVec_pi.push_back(mpv_pi);
        errVec_pi.push_back(slice->GetBinWidth(maxBin_pi));

        /* Electron MPV — maximum above a momentum-dependent threshold
         * Electrons at low momentum have much higher dE/dx than pions
         * The threshold separates the two bands:
         *   at p=0.1 GeV/c: threshold ~ 170 a.u.
         *   at p=1.0 GeV/c: threshold ~ 60 a.u. (bands converge)  */
        Double_t threshold = 120.0 * TMath::Exp(-p * 0.8) + 55.0;
        int threshBin = slice->FindBin(threshold);
        int maxBin_e  = threshBin;
        Double_t maxContent = 0;
        for (int b = threshBin; b <= slice->GetNbinsX(); b++) {
            if (slice->GetBinContent(b) > maxContent) {
                maxContent = slice->GetBinContent(b);
                maxBin_e   = b;
            }
        }

        /* Only store if a meaningful peak was found */
        if (maxContent > 10) {
            pVec_e.push_back(p);
            dedxVec_e.push_back(slice->GetBinCenter(maxBin_e));
            errVec_e.push_back(slice->GetBinWidth(maxBin_e));
        }

        delete slice;
    }

    cout << "Pion MPV points:    " << pVec_pi.size() << endl;
    cout << "Electron MPV points:" << pVec_e.size()  << endl;

    /* TGraphErrors for pions */
    TGraphErrors* gMPV_pi = new TGraphErrors(
        pVec_pi.size(), pVec_pi.data(), dedxVec_pi.data(),
        nullptr, errVec_pi.data());
    gMPV_pi->SetMarkerStyle(20);
    gMPV_pi->SetMarkerSize(0.8);
    gMPV_pi->SetMarkerColor(kRed);

    /* TGraphErrors for electrons */
    TGraphErrors* gMPV_e = new TGraphErrors(
        pVec_e.size(), pVec_e.data(), dedxVec_e.data(),
        nullptr, errVec_e.data());
    gMPV_e->SetMarkerStyle(20);
    gMPV_e->SetMarkerSize(0.8);
    gMPV_e->SetMarkerColor(kBlue+1);

    /* Pion Bethe-Bloch fit */
    TF1* fitBB_pi = new TF1("fitBB_pi", BetheBloch, 0.1, 5.0, 6);
    fitBB_pi->SetParameters(50.0, 2.5, 1e-3, 2.2, 2.0, 0.13957);
    fitBB_pi->FixParameter(5, 0.13957);
    fitBB_pi->SetLineColor(kRed);
    fitBB_pi->SetLineWidth(2);
    gMPV_pi->Fit(fitBB_pi, "R");

    /* Electron Bethe-Bloch fit — start from pion parameters */
    TF1* fitBB_e = new TF1("fitBB_e", BetheBloch, 0.1, 5.0, 6);
    fitBB_e->SetParameters(
        fitBB_pi->GetParameter(0),
        fitBB_pi->GetParameter(1),
        fitBB_pi->GetParameter(2),
        fitBB_pi->GetParameter(3),
        fitBB_pi->GetParameter(4),
        0.000511);
    fitBB_e->FixParameter(5, 0.000511);
    fitBB_e->SetLineColor(kBlue+1);
    fitBB_e->SetLineWidth(2);
    fitBB_e->SetLineStyle(2);
    gMPV_e->Fit(fitBB_e, "R");

    /* Draw */
    TCanvas* c1 = new TCanvas("c1", "Bethe-Bloch Fit - PbPb 2023", 900, 700);
    c1->SetLogx();
    c1->SetLogz();
    c1->SetGrid();
    c1->SetLeftMargin(0.13);
    c1->SetBottomMargin(0.13);

    hDeDx->Draw("COLZ");
    gMPV_pi->Draw("P same");
    gMPV_e->Draw("P same");
    fitBB_pi->Draw("same");
    fitBB_e->Draw("same");

    TLegend* leg = new TLegend(0.42, 0.60, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->AddEntry(hDeDx,    "TPC dE/dx data", "f");
    leg->AddEntry(gMPV_pi,  "MPV - pion band", "p");
    leg->AddEntry(gMPV_e,   "MPV - electron band", "p");
    leg->AddEntry(fitBB_pi, "Bethe-Bloch fit (#pi^{#pm}, m=0.140 GeV/c^{2})", "l");
    leg->AddEntry(fitBB_e,  "Bethe-Bloch fit (e^{#pm}, m=0.511 MeV/c^{2})", "l");
    leg->Draw();

    cout << "\n=== Pion Bethe-Bloch Fit ===" << endl;
    for (int i = 0; i < 5; i++)
        cout << "P" << i+1 << " = " << fitBB_pi->GetParameter(i) << endl;

    cout << "\n=== Electron Bethe-Bloch Fit ===" << endl;
    for (int i = 0; i < 5; i++)
        cout << "P" << i+1 << " = " << fitBB_e->GetParameter(i) << endl;

    c1->Update();
    c1->SaveAs("plots/bethebloch_fit.png");
    c1->SaveAs("plots/bethebloch_fit.pdf");
    cout << "Saved plots/bethebloch_fit.png" << endl;
}