void search_mc() {
    TFile* f = TFile::Open("/home/madhvafakare/Downloads/AO2D.root");
    
    if (!f) {
        cout << "Cannot open file!" << endl;
        return;
    }
    
    cout << "\n========== SEARCHING ALL DIRECTORIES ==========\n" << endl;
    
    // Loop through ALL directories
    TIter next(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TString dirname = key->GetName();
        
        if (dirname.Contains("DF_")) {
            cout << "\n>>> Checking directory: " << dirname << endl;
            f->cd(dirname);
            
            // Get all keys in this directory
            TList* dirkeys = gDirectory->GetListOfKeys();
            TIter dirnext(dirkeys);
            TKey* dirkey;
            
            // Look for MC-related trees
            while ((dirkey = (TKey*)dirnext())) {
                TString treename = dirkey->GetName();
                
                // Check if it's MC-related
                if (treename.Contains("mc", TString::kIgnoreCase) || 
                    treename.Contains("Mc") ||
                    treename.Contains("MC")) {
                    
                    TTree* tree = (TTree*)gDirectory->Get(treename);
                    if (tree) {
                        cout << "   FOUND MC TREE: " << treename 
                             << " with " << tree->GetEntries() << " entries" << endl;
                        
                        // Show first few branches
                        TObjArray* branches = tree->GetListOfBranches();
                        cout << "   First 5 branches: ";
                        for (int i = 0; i < TMath::Min(5, branches->GetEntries()); i++) {
                            cout << branches->At(i)->GetName();
                            if (i < 4 && i < branches->GetEntries()-1) cout << ", ";
                        }
                        cout << endl;
                    }
                }
            }
        }
    }
    
    cout << "\n========== SEARCH COMPLETE ==========\n" << endl;
}