void check_branches() {
    TFile* f = TFile::Open("/home/madhvafakare/Downloads/AO2D.root");
    f->cd("DF_2336986335323680");
    
    TTree* tracks = (TTree*)gDirectory->Get("O2track_iu");
    
    cout << "\n=== Branch Details ===" << endl;
    
    TBranch* br = tracks->GetBranch("fSignedPt");
    if (br) {
        cout << "fSignedPt branch found!" << endl;
        cout << "  Type: " << br->GetClassName() << endl;
        cout << "  Title: " << br->GetTitle() << endl;
        
        // Try to get leaf info
        TLeaf* leaf = br->GetLeaf("fSignedPt");
        if (leaf) {
            cout << "  Leaf type: " << leaf->GetTypeName() << endl;
            cout << "  Length: " << leaf->GetLen() << endl;
        }
    }
    
    // Check all branch types
    cout << "\n=== All Branches with Types ===" << endl;
    TObjArray* branches = tracks->GetListOfBranches();
    for (int i = 0; i < branches->GetEntries(); i++) {
        TBranch* b = (TBranch*)branches->At(i);
        TLeaf* l = b->GetLeaf(b->GetName());
        if (l) {
            cout << b->GetName() << " : " << l->GetTypeName() << endl;
        }
    }
}

