using namespace std;


void topanalysis(){

  Int_t           numParticles;
  Float_t         eventweight;
  vector<int>     *pid = new vector<int>;
  vector<int>     *status= new vector<int>;
  vector<float>   *px = new vector<float>;
  vector<float>   *py= new vector<float>;
  vector<float>   *pz= new vector<float>;
  vector<float>   *e= new vector<float>;
  vector<float>   *mass= new vector<float>;
  vector<float>   *spin= new vector<float>;

  TFile *f1 = new TFile("toponium.root","read");
  TTree *tree = (TTree*)f1->Get("events");

  tree->SetBranchAddress("numParticles", &numParticles);
  tree->SetBranchAddress("eventweight", &eventweight);
  tree->SetBranchAddress("pid", &pid);
  tree->SetBranchAddress("status", &status);
  tree->SetBranchAddress("px", &px);
  tree->SetBranchAddress("py", &py);
  tree->SetBranchAddress("pz", &pz);
  tree->SetBranchAddress("energy", &e);
  tree->SetBranchAddress("mass", &mass);
  tree->SetBranchAddress("spin", &spin);

  double lumi = 300; //in fb^-1 LHC RUN3 multiplied so that L dN = d\sigma
  TLorentzVector lep1, lep2, b1, b2, MET;
  Int_t total_entries = tree->GetEntriesFast();
  for(Int_t entry=0; entry < total_entries; ++entry){
    tree->GetEntry(entry);

    //eventweight = cross_section*eventweight*lumi/totaleventweight;
    eventweight = eventweight*lumi;

    lep1.SetPxPyPzE(0,0,0,0);
    lep2.SetPxPyPzE(0,0,0,0);
    b1.SetPxPyPzE(0,0,0,0);
    b2.SetPxPyPzE(0,0,0,0);
    MET.SetPxPyPzE(0,0,0,0);

    Int_t lep_count = 0;
    Int_t lep_index1 =-1, lep_index2=-1;

    Int_t bjet_count = 0;
    Int_t b_index1=-1, b_index2=-1;

    for(Int_t i=0; i < pid->size(); ++i){


        if(status->at(i) != +1){continue;} //considering only final state particles 

        //get first two leptons, b-jets, and missing momentum
        // MET
        if(abs(pid->at(i)) == 12 || abs(pid->at(i)) == 14){
            MET.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
        }        
        
        //leptons
        if(abs(pid->at(i)) == 11 || abs(pid->at(i)) == 13){
            // only considering two leading leptons
            //cout << lep_count << endl;
            lep_count  += 1;

            if(lep_count==1){
                lep_index1 = i; 
            }; 

            if(lep_count==2){
                lep_index2 = i;
            };
        }

        //bjets
        if(abs(pid->at(i)) == 5){
            // only considering two leading bjets
            bjet_count+=1;

            if(bjet_count==1){
                b_index1= i;
            }; 
            if(bjet_count==2){
                b_index2= i;
            };
        }


    }//particle loop
    if(lep_count < 2){continue;}
    if(bjet_count < 2){continue;}
    lep1.SetPxPyPzE(px->at(lep_index1),py->at(lep_index1),pz->at(lep_index1),e->at(lep_index1));
    lep2.SetPxPyPzE(px->at(lep_index2),py->at(lep_index2),pz->at(lep_index2),e->at(lep_index2));

    b1.SetPxPyPzE(px->at(b_index1),py->at(b_index1),pz->at(b_index1),e->at(b_index1));
    b2.SetPxPyPzE(px->at(b_index2),py->at(b_index2),pz->at(b_index2),e->at(b_index2));

    }//event loop

}