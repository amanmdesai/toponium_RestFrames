#include "RestFrames/RestFrames.hh"

#include <TLorentzVector.h>
#include <TH2.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TMath.h>
#include <TVector3.h>
#include <TStyle.h>
#include <TString.h>
#include <TCanvas.h>
#include <TColor.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <TBranch.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TColor.h>
#include <TStyle.h>
#include <TRandom.h>
#include <TF1.h>
#include <iostream>

using namespace std;
using namespace RestFrames;

void example_N_Wlnu(){
  std::string output_name = "example_N_Wlnu.root";

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
  TVector3 myMET;

  Int_t total_entries = tree->GetEntriesFast();


  double mH = 346; 
  double mT = 173; 
  double mW = 80.385;
    double mB = 4.18;
    double mL = 0.106;
    double mN = 0.;
  
    double wT = 1.49;
    double wW   = 2.085;
  
    
    // number of events to generate (per Higgs mass)
    int Ngen = 10000;
  
    /////////////////////////////////////////////////////////////////////////////////////////
    g_Log << LogInfo << "Initializing generator frames and tree..." << LogEnd;
    /////////////////////////////////////////////////////////////////////////////////////////
    ppLabGenFrame     LAB_Gen("LAB_Gen","LAB");
    DecayGenFrame     H_Gen("H_Gen","H^{ 0}");
    DecayGenFrame     Ta_Gen("Ta_Gen","t_{a}");
    DecayGenFrame     Tb_Gen("Tb_Gen","t_{b}");
    DecayGenFrame     Wa_Gen("Wa_Gen","W_{a}");
    DecayGenFrame     Wb_Gen("Wb_Gen","W_{b}");
    VisibleGenFrame   Ba_Gen("Ba_Gen","b_{a}");
    VisibleGenFrame   La_Gen("La_Gen","#it{l}_{a}");
    InvisibleGenFrame Na_Gen("Na_Gen","#nu_{a}");
    VisibleGenFrame   Bb_Gen("Bb_Gen","b_{b}");
    VisibleGenFrame   Lb_Gen("Lb_Gen","#it{l}_{b}");
    InvisibleGenFrame Nb_Gen("Nb_Gen","#nu_{b}");
  
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    LAB_Gen.SetChildFrame(H_Gen);
    H_Gen.AddChildFrame(Ta_Gen);
    H_Gen.AddChildFrame(Tb_Gen);
    Ta_Gen.AddChildFrame(Ba_Gen);
    Ta_Gen.AddChildFrame(Wa_Gen);
    Tb_Gen.AddChildFrame(Bb_Gen);
    Tb_Gen.AddChildFrame(Wb_Gen);
    Wa_Gen.AddChildFrame(La_Gen);
    Wa_Gen.AddChildFrame(Na_Gen);
    Wb_Gen.AddChildFrame(Lb_Gen);
    Wb_Gen.AddChildFrame(Nb_Gen);
  
    if(LAB_Gen.InitializeTree())
      g_Log << LogInfo << "...Successfully initialized generator tree" << LogEnd;
    else
      g_Log << LogError << "...Failed initializing generator tree" << LogEnd;							        
    
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
      
    // Higgs mass
    H_Gen.SetMass(mH);
    // set top masses
    Ta_Gen.SetMass(mT);            Tb_Gen.SetMass(mT-2);
    // set W masses
    Wa_Gen.SetMass(mW);            Wb_Gen.SetMass(mW);
    // set B masses
    Ba_Gen.SetMass(mB);            Bb_Gen.SetMass(mB);
    // set : masses
    La_Gen.SetMass(mL);            Lb_Gen.SetMass(mL);
    // set neutrino masses
    Na_Gen.SetMass(mN);            Nb_Gen.SetMass(mN);
  
    // set b-jet/lepton pT/eta cuts
    Ba_Gen.SetPtCut(20.);          Bb_Gen.SetPtCut(20.);
    Ba_Gen.SetEtaCut(2.5);         Bb_Gen.SetEtaCut(2.5);
    La_Gen.SetPtCut(15.);          Lb_Gen.SetPtCut(15.);
    La_Gen.SetEtaCut(2.5);         Lb_Gen.SetEtaCut(2.5);
  
    if(LAB_Gen.InitializeAnalysis())
      g_Log << LogInfo << "...Successfully initialized generator analysis" << LogEnd;
    else
      g_Log << LogError << "...Failed initializing generator analysis" << LogEnd;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////								    
    /////////////////////////////////////////////////////////////////////////////////////////
    g_Log << LogInfo << "Initializing reconstruction frames and tree..." << LogEnd;
    /////////////////////////////////////////////////////////////////////////////////////////
  
    LabRecoFrame       LAB("LAB","LAB");     
    DecayRecoFrame     H("H","H^{ 0}"); 
    DecayRecoFrame     Ta("Ta","t_{a}");   
    DecayRecoFrame     Tb("Tb","t_{b}");    
    DecayRecoFrame     Wa("Wa","W_{a}");     
    DecayRecoFrame     Wb("Wb","W_{b}");   
    VisibleRecoFrame   Ba("Ba","b_{a}");     
    VisibleRecoFrame   La("La","#it{l}_{a}");
    InvisibleRecoFrame Na("Na","#nu_{a}");    
    VisibleRecoFrame   Bb("Bb","b_{b}");     
    VisibleRecoFrame   Lb("Lb","#it{l}_{b}");
    InvisibleRecoFrame Nb("Nb","#nu_{b}");   
  
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    LAB.SetChildFrame(H);                  
    H.AddChildFrame(Ta);                 
    H.AddChildFrame(Tb);                   
    Ta.AddChildFrame(Ba);                  
    Ta.AddChildFrame(Wa);                   
    Tb.AddChildFrame(Bb);                  
    Tb.AddChildFrame(Wb);                
    Wa.AddChildFrame(La);                  
    Wa.AddChildFrame(Na);                  
    Wb.AddChildFrame(Lb);                  
    Wb.AddChildFrame(Nb);                  
  
    if(LAB.InitializeTree())
      g_Log << LogInfo << "...Successfully initialized reconstruction tree" << LogEnd;
    else
      g_Log << LogError << "...Failed initializing reconstruction tree" << LogEnd;
   
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    //////////////////////// define Groups for reconstruction tree ////////////////////////
    std::string group_name;
  
    // Invisible Group
    group_name = "#nu #nu Jigsaws";
    InvisibleGroup INV("INV", group_name);
    INV.AddFrame(Na);
    INV.AddFrame(Nb);
    CombinatoricGroup B("VIS","b-jet Jigsaws");
    B.AddFrame(Ba);
    B.AddFrame(Bb);
    B.SetNElementsForFrame(Ba, 1);
    B.SetNElementsForFrame(Bb, 1);
  
    //////////////////////// define Jigsaws for reconstruction tree ////////////////////////
    std::string jigsaw_name;
  
    // Minimize difference Mt jigsaws
    jigsaw_name = "M_{#nu #nu} ~ m_{#it{l} #it{l}}";
    SetMassInvJigsaw NuNuM("NuNuM", jigsaw_name);
    INV.AddJigsaw(NuNuM);
  
    jigsaw_name = "#eta_{#nu#nu} = #eta_{b #it{l} b #it{l}}";
    SetRapidityInvJigsaw NuNuR("NuNuR", jigsaw_name);
    INV.AddJigsaw(NuNuR);
    NuNuR.AddVisibleFrames(H.GetListVisibleFrames());
  
    jigsaw_name = "min ( M_{top a}- M_{top b} )^{2}";
    MinMassDiffInvJigsaw MinDeltaMt("MinDeltaMt", jigsaw_name, 2);
    INV.AddJigsaw(MinDeltaMt);
    MinDeltaMt.AddInvisibleFrame(Na, 0);
    MinDeltaMt.AddInvisibleFrame(Nb, 1);
    MinDeltaMt.AddVisibleFrames(La+Ba, 0);
    MinDeltaMt.AddVisibleFrames(Lb+Bb, 1);
    MinDeltaMt.AddMassFrame(La, 0);
    MinDeltaMt.AddMassFrame(Lb, 1);
  
    // b-jet combinatoric jigsaws for all trees
    jigsaw_name = "Minimize M(b #it{l} )_{a} , M(b #it{l} )_{b}";
  
    MinMassesCombJigsaw MinBL("MinBL", jigsaw_name);
    B.AddJigsaw(MinBL);
    MinBL.AddFrames(La+Ba,0);
    MinBL.AddFrames(Lb+Bb,1);
  
    if(LAB.InitializeAnalysis())
      g_Log << LogInfo << "...Successfully initialized analysis" << LogEnd;
    else
      g_Log << LogError << "...Failed initializing analysis" << LogEnd;	
  
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
  
    TreePlot* treePlot = new TreePlot("TreePlot","TreePlot");
   
    treePlot->SetTree(LAB_Gen);
    treePlot->Draw("GenTree", "Generator Tree", true);
    
    treePlot->SetTree(LAB);
    treePlot->Draw("RecoTree", "Reconstruction Tree");
  
    treePlot->SetTree(B);
    treePlot->Draw("VisTree", "b-jet Jigsaws", true);
  
    treePlot->SetTree(INV);
    treePlot->Draw("InvTree", "Inivisible Jigsaws", true);
  
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    std::string plot_title = "H^{ 0} #rightarrow t #bar{t} #rightarrow b W(#it{l} #nu) b W(#it{l} #nu)";
    HistPlot* histPlot = new HistPlot("HistPlot", plot_title);
  
    const HistPlotCategory& cat_Gen  = histPlot->GetNewCategory("Reco", "Generator");
    const HistPlotCategory& cat_Reco = histPlot->GetNewCategory("Reco", "Reconstruction");
    
    const HistPlotVar& MH     = histPlot->GetNewVar("MH", "M_{H^{ 0}} / m_{H^{ 0}}^{ true}", 0., 2.);
    const HistPlotVar& Eb_ta  = histPlot->GetNewVar("Eb_ta", "E_{b a}^{top a} / E_{b a}^{top a gen}", 0., 2.);
    const HistPlotVar& Eb_tb  = histPlot->GetNewVar("Eb_tb", "E_{b b}^{top b} / E_{b b}^{top b gen}", 0., 2.);
    const HistPlotVar& El_Wa  = histPlot->GetNewVar("El_Wa", "E_{#it{l} a}^{W a} / E_{#it{l} a}^{W a true}", 0., 2.);
    const HistPlotVar& El_Wb  = histPlot->GetNewVar("El_Wb", "E_{#it{l} b}^{W b} / E_{#it{l} b}^{W b true}", 0., 2.);
    const HistPlotVar& cosH   = histPlot->GetNewVar("cosH","cos #theta_{H^{ 0}}", -1., 1.);
    const HistPlotVar& costa  = histPlot->GetNewVar("costa","cos #theta_{top a}", -1., 1.);
    const HistPlotVar& costb  = histPlot->GetNewVar("costb","cos #theta_{top b}", -1., 1.);
    const HistPlotVar& cosWa  = histPlot->GetNewVar("cosWa","cos #theta_{W a}", -1., 1.);
    const HistPlotVar& cosWb  = histPlot->GetNewVar("cosWb","cos #theta_{W b}", -1., 1.);
    const HistPlotVar& DcosH  = histPlot->GetNewVar("Dcostt","#theta_{H^{ 0}} - #theta_{H^{ 0}}^{true}", 
                            -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& Dcosta = histPlot->GetNewVar("Dcosta","#theta_{top a} - #theta_{top a}^{true}", 
                            -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& Dcostb = histPlot->GetNewVar("Dcostb","#theta_{top b} - #theta_{top b}^{true}", 
                            -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& DcosWa = histPlot->GetNewVar("DcosWa","#theta_{W a} - #theta_{W a}^{true}", 
                            -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& DcosWb = histPlot->GetNewVar("DcosWb","#theta_{W b} - #theta_{W b}^{true}", 
                            -acos(-1.)/2., acos(-1.)/2.);
  
    histPlot->AddPlot(MH,    cat_Gen+cat_Reco);
    histPlot->AddPlot(Eb_ta, cat_Gen+cat_Reco);
    histPlot->AddPlot(El_Wa, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosH,  cat_Gen+cat_Reco);
    histPlot->AddPlot(Dcosta, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosWa, cat_Gen+cat_Reco);
  
    histPlot->AddPlot(MH, Eb_ta, cat_Gen+cat_Reco);
    histPlot->AddPlot(MH, El_Wa, cat_Gen+cat_Reco);
    histPlot->AddPlot(Eb_ta, Eb_tb, cat_Gen+cat_Reco);
    histPlot->AddPlot(El_Wa, El_Wb, cat_Gen+cat_Reco);
    histPlot->AddPlot(Eb_ta, El_Wa, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosH, MH,  cat_Gen+cat_Reco);
    histPlot->AddPlot(Dcosta, Eb_ta, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosWa, El_Wa, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosH, Dcosta, cat_Gen+cat_Reco);
    histPlot->AddPlot(Dcosta, Dcostb, cat_Gen+cat_Reco);
    histPlot->AddPlot(DcosWa, DcosWb, cat_Gen+cat_Reco);
    histPlot->AddPlot(Dcosta, DcosWa, cat_Gen+cat_Reco);
  
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
  
  
  
    for(Int_t entry=0; entry < total_entries; ++entry){
     
     
      H_Gen.SetMass(mH);
      LAB_Gen.InitializeAnalysis();

      tree->GetEntry(entry);
  
      //eventweight = cross_section*eventweight*lumi/totaleventweight;
      eventweight = eventweight*lumi;
  
      lep1.SetPxPyPzE(0,0,0,0);
      lep2.SetPxPyPzE(0,0,0,0);
      b1.SetPxPyPzE(0,0,0,0);
      b2.SetPxPyPzE(0,0,0,0);
      MET.SetPxPyPzE(0,0,0,0);
  
      Int_t lep_count = 0;
      Int_t lep_charge = 0;
      Int_t lep_index1 =-1, lep_index2=-1;
  
      Int_t bjet_count = 0;
      Int_t b_index1=-1, b_index2=-1;
  
  
    for(Int_t i=0; i < pid->size(); ++i){


        if(status->at(i) != +1){continue;} //considering only final state particles 

        //get first two leptons, b-jets, and missing momentum
        // MET
        if(abs(pid->at(i)) == 12 || abs(pid->at(i)) == 14){
          MET.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));

          myMET.SetPtEtaPhi(MET.Pt(),0.0,MET.Phi());
          myMET.SetZ(0.0);
      
        }        
        
        //leptons
        if(pid->at(i) == 11 || pid->at(i) == 13){
          lep_count+=1;
          lep1.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
        }

        if(pid->at(i) == -11 || pid->at(i) == -13){
          lep_count+=1;
          lep2.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
        }
        // only considering two leading leptons

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
    if(bjet_count < 2){continue;}
    if(lep_count < 2){continue;}

    
    b1.SetPxPyPzE(px->at(b_index1),py->at(b_index1),pz->at(b_index1),e->at(b_index1));
    b2.SetPxPyPzE(px->at(b_index2),py->at(b_index2),pz->at(b_index2),e->at(b_index2));
   
    if(lep1.Pt() < 10 || lep2.Pt() < 10){continue;}
    if(b1.Pt() < 10 || b2.Pt() < 10){continue;}
    if(MET.Pt() < 10){continue;}
    
    LAB_Gen.ClearEvent();                             // clear the gen tree
    LAB_Gen.AnalyzeEvent();                           // generate a new event

    LAB.ClearEvent();                                  // clear the event

    INV.SetLabFrameThreeVector(myMET);                   // Set MET in reco tree

    La.SetLabFrameFourVector(lep1);  // Set lepton 4-vectors
    Lb.SetLabFrameFourVector(lep2);

    B.AddLabFrameFourVector(b1);   // Set b-jet 4-vectors
    B.AddLabFrameFourVector(b2);
 
    LAB.AnalyzeEvent();                                // analyze the event

    //////////////////////////////////////
    // Observable Calculations
    //////////////////////////////////////

    double MHgen    = H_Gen.GetMass();
    double Eb_tagen = Ba_Gen.GetFourVector(Ta_Gen).E();
    double Eb_tbgen = Bb_Gen.GetFourVector(Tb_Gen).E();
    double El_Wagen = La_Gen.GetFourVector(Wa_Gen).E();
    double El_Wbgen = Lb_Gen.GetFourVector(Wb_Gen).E();
    double cosHgen  = H_Gen.GetCosDecayAngle();
    double costagen = Ta_Gen.GetCosDecayAngle();
    double costbgen = Tb_Gen.GetCosDecayAngle();
    double cosWagen = Wa_Gen.GetCosDecayAngle();
    double cosWbgen = Wb_Gen.GetCosDecayAngle();

    MH    = H.GetMass()/MHgen;

    cout << H.GetMass() << endl;
    
    Eb_ta = Ba.GetFourVector(Ta).E()/Eb_tagen;
    Eb_tb = Bb.GetFourVector(Tb).E()/Eb_tbgen;
    El_Wa = La.GetFourVector(Wa).E()/El_Wagen;
    El_Wb = Lb.GetFourVector(Wb).E()/El_Wbgen;
    cosH  = H.GetCosDecayAngle();
    costa = Ta.GetCosDecayAngle();
    costb = Tb.GetCosDecayAngle();
    cosWa = Wa.GetCosDecayAngle();
    cosWb = Wb.GetCosDecayAngle();
    DcosH  = asin(sqrt(1.-cosH*cosH)*cosHgen-sqrt(1.-cosHgen*cosHgen)*cosH);
    Dcosta = asin(sqrt(1.-costa*costa)*costagen-sqrt(1.-costagen*costagen)*costa);
    Dcostb = asin(sqrt(1.-costb*costb)*costbgen-sqrt(1.-costbgen*costbgen)*costb);
    DcosWa = asin(sqrt(1.-cosWa*cosWa)*cosWagen-sqrt(1.-cosWagen*cosWagen)*cosWa);
    DcosWb = asin(sqrt(1.-cosWb*cosWb)*cosWbgen-sqrt(1.-cosWbgen*cosWbgen)*cosWb);

    histPlot->Fill(cat_Gen);
    histPlot->Fill(cat_Reco);
  
  //LAB_Gen.PrintGeneratorEfficiency();
  

    }//event loop
    TFile fout(output_name.c_str(),"RECREATE");
    fout.Close();
    histPlot->WriteOutput(output_name);
    histPlot->WriteHist(output_name);
    treePlot->WriteOutput(output_name);
    g_Log << LogInfo << "Finished" << LogEnd;

}

# ifndef __CINT__ // main function for stand-alone compilation
int main(){
  example_N_Wlnu();
  return 0;
}
#endif
