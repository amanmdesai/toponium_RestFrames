#include "RestFrames.hh"

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
ROOT::EnableImplicitMT(10);

void run(std::string sample){

  //std::string sample = "ttbar";
  //std::string sample = "toponium";

  std::string output_name = sample+"_reco.root";
  TFile *f1 = new TFile((sample+".root").c_str(),"read");

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
  TLorentzVector top, antitop, lep1, lep2, b1, b2, MET;
  TVector3 myMET;

  Int_t total_entries = tree->GetEntriesFast();


  double mH = 348; 
  double mT = 173; 
  double mW = 80.385;
  double mB = 4.18;
  double mL = 0.106;
  double mN = 0.;

  double wT = 1.49;
  double wW   = 2.085;
  
    
    // number of events to generate (per Higgs mass)
    int Ngen = 500000;
  
    /////////////////////////////////////////////////////////////////////////////////////////
    g_Log << LogInfo << "Initializing generator frames and tree..." << LogEnd;
    /////////////////////////////////////////////////////////////////////////////////////////
    ppLabGenFrame     LAB_Gen("LAB_Gen","LAB");
    DecayGenFrame     TT_Gen("TT_Gen","t #bar{t}");
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
  
    LAB_Gen.SetChildFrame(TT_Gen);
    TT_Gen.AddChildFrame(Ta_Gen);
    TT_Gen.AddChildFrame(Tb_Gen);
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
    TT_Gen.SetMass(mH);
    // set top masses
    Ta_Gen.SetMass(mT);            Tb_Gen.SetMass(mT);
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
  
    LabRecoFrame       LAB_R1("LAB_R1","LAB");      LabRecoFrame       LAB_R2("LAB_R2","LAB");
    DecayRecoFrame     TT_R1("TT_R1","t #bar{t}");  DecayRecoFrame     TT_R2("TT_R2","t #bar{t}");
    DecayRecoFrame     Ta_R1("Ta_R1","t_{a}");      DecayRecoFrame     Ta_R2("Ta_R2","t_{a}");
    DecayRecoFrame     Tb_R1("Tb_R1","t_{b}");      DecayRecoFrame     Tb_R2("Tb_R2","t_{b}");
    DecayRecoFrame     Wa_R1("Wa_R1","W_{a}");      DecayRecoFrame     Wa_R2("Wa_R2","W_{a}");
    DecayRecoFrame     Wb_R1("Wb_R1","W_{b}");      DecayRecoFrame     Wb_R2("Wb_R2","W_{b}");
    VisibleRecoFrame   Ba_R1("Ba_R1","b_{a}");      VisibleRecoFrame   Ba_R2("Ba_R2","b_{a}");
    VisibleRecoFrame   La_R1("La_R1","#it{l}_{a}"); VisibleRecoFrame   La_R2("La_R2","#it{l}_{a}");
    InvisibleRecoFrame Na_R1("Na_R1","#nu_{a}");    InvisibleRecoFrame Na_R2("Na_R2","#nu_{a}");
    VisibleRecoFrame   Bb_R1("Bb_R1","b_{b}");      VisibleRecoFrame   Bb_R2("Bb_R2","b_{b}");
    VisibleRecoFrame   Lb_R1("Lb_R1","#it{l}_{b}"); VisibleRecoFrame   Lb_R2("Lb_R2","#it{l}_{b}");
    InvisibleRecoFrame Nb_R1("Nb_R1","#nu_{b}");    InvisibleRecoFrame Nb_R2("Nb_R2","#nu_{b}");
  
    LabRecoFrame       LAB_R3("LAB_R3","LAB");      LabRecoFrame       LAB_R4("LAB_R4","LAB");
    DecayRecoFrame     TT_R3("TT_R3","t #bar{t}");  DecayRecoFrame     TT_R4("TT_R4","t #bar{t}");
    DecayRecoFrame     Ta_R3("Ta_R3","t_{a}");      DecayRecoFrame     Ta_R4("Ta_R4","t_{a}");
    DecayRecoFrame     Tb_R3("Tb_R3","t_{b}");      DecayRecoFrame     Tb_R4("Tb_R4","t_{b}");
    DecayRecoFrame     Wa_R3("Wa_R3","W_{a}");      DecayRecoFrame     Wa_R4("Wa_R4","W_{a}");
    DecayRecoFrame     Wb_R3("Wb_R3","W_{b}");      DecayRecoFrame     Wb_R4("Wb_R4","W_{b}");
    VisibleRecoFrame   Ba_R3("Ba_R3","b_{a}");      VisibleRecoFrame   Ba_R4("Ba_R4","b_{a}");
    VisibleRecoFrame   La_R3("La_R3","#it{l}_{a}"); VisibleRecoFrame   La_R4("La_R4","#it{l}_{a}");
    InvisibleRecoFrame Na_R3("Na_R3","#nu_{a}");    InvisibleRecoFrame Na_R4("Na_R4","#nu_{a}");
    VisibleRecoFrame   Bb_R3("Bb_R3","b_{b}");      VisibleRecoFrame   Bb_R4("Bb_R4","b_{b}");
    VisibleRecoFrame   Lb_R3("Lb_R3","#it{l}_{b}"); VisibleRecoFrame   Lb_R4("Lb_R4","#it{l}_{b}");
    InvisibleRecoFrame Nb_R3("Nb_R3","#nu_{b}");    InvisibleRecoFrame Nb_R4("Nb_R4","#nu_{b}");
    
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    LAB_R1.SetChildFrame(TT_R1);                    LAB_R2.SetChildFrame(TT_R2);
    TT_R1.AddChildFrame(Ta_R1);                     TT_R2.AddChildFrame(Ta_R2);
    TT_R1.AddChildFrame(Tb_R1);                     TT_R2.AddChildFrame(Tb_R2);
    Ta_R1.AddChildFrame(Ba_R1);                     Ta_R2.AddChildFrame(Ba_R2);
    Ta_R1.AddChildFrame(Wa_R1);                     Ta_R2.AddChildFrame(Wa_R2);
    Tb_R1.AddChildFrame(Bb_R1);                     Tb_R2.AddChildFrame(Bb_R2);
    Tb_R1.AddChildFrame(Wb_R1);                     Tb_R2.AddChildFrame(Wb_R2);
    Wa_R1.AddChildFrame(La_R1);                     Wa_R2.AddChildFrame(La_R2);
    Wa_R1.AddChildFrame(Na_R1);                     Wa_R2.AddChildFrame(Na_R2);
    Wb_R1.AddChildFrame(Lb_R1);                     Wb_R2.AddChildFrame(Lb_R2);
    Wb_R1.AddChildFrame(Nb_R1);                     Wb_R2.AddChildFrame(Nb_R2);
  
    LAB_R3.SetChildFrame(TT_R3);                    LAB_R4.SetChildFrame(TT_R4);
    TT_R3.AddChildFrame(Ta_R3);                     TT_R4.AddChildFrame(Ta_R4);
    TT_R3.AddChildFrame(Tb_R3);                     TT_R4.AddChildFrame(Tb_R4);
    Ta_R3.AddChildFrame(Ba_R3);                     Ta_R4.AddChildFrame(Ba_R4);
    Ta_R3.AddChildFrame(Wa_R3);                     Ta_R4.AddChildFrame(Wa_R4);
    Tb_R3.AddChildFrame(Bb_R3);                     Tb_R4.AddChildFrame(Bb_R4);
    Tb_R3.AddChildFrame(Wb_R3);                     Tb_R4.AddChildFrame(Wb_R4);
    Wa_R3.AddChildFrame(La_R3);                     Wa_R4.AddChildFrame(La_R4);
    Wa_R3.AddChildFrame(Na_R3);                     Wa_R4.AddChildFrame(Na_R4);
    Wb_R3.AddChildFrame(Lb_R3);                     Wb_R4.AddChildFrame(Lb_R4);
    Wb_R3.AddChildFrame(Nb_R3);                     Wb_R4.AddChildFrame(Nb_R4);
  
    if(LAB_R1.InitializeTree() && LAB_R2.InitializeTree() && 
       LAB_R3.InitializeTree() && LAB_R4.InitializeTree())
      g_Log << LogInfo << "...Successfully initialized reconstruction trees" << LogEnd;
    else
      g_Log << LogError << "...Failed initializing reconstruction trees" << LogEnd;
  
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    //////////////////////// define Groups for reconstruction tree ////////////////////////
    std::string group_name;

    // Invisible Group
    group_name = "#splitline{#nu #nu Jigsaws for}{min M_{top} , M_{top}^{ a} = M_{top}^{ b}}";
    InvisibleGroup INV_R1("INV_R1", group_name);
    INV_R1.AddFrame(Na_R1);
    INV_R1.AddFrame(Nb_R1);
    // Combinatoric Group for b's
    CombinatoricGroup B_R1("VIS_R1","b-jet Jigsaws");
    B_R1.AddFrame(Ba_R1);
    B_R1.AddFrame(Bb_R1);
    // b-jet frames must have at least one element
    B_R1.SetNElementsForFrame(Ba_R1, 1);
    B_R1.SetNElementsForFrame(Bb_R1, 1);
  
    group_name = "#splitline{#nu #nu Jigsaws for}{min M_{W}, M_{W}^{ a} = M_{W}^{ b}}";
    InvisibleGroup INV_R2("INV_R2", group_name);
    INV_R2.AddFrame(Na_R2);
    INV_R2.AddFrame(Nb_R2);
    CombinatoricGroup B_R2("VIS_R2","b-jet Jigsaws");
    B_R2.AddFrame(Ba_R2);
    B_R2.AddFrame(Bb_R2);
    B_R2.SetNElementsForFrame(Ba_R2, 1);
    B_R2.SetNElementsForFrame(Bb_R2, 1);
  
    group_name = "#splitline{#nu #nu Jigsaws for}{min M_{top a}^{2}+ M_{top b}^{2}}";
    InvisibleGroup INV_R3("INV_R3", group_name);
    INV_R3.AddFrame(Na_R3);
    INV_R3.AddFrame(Nb_R3);
    CombinatoricGroup B_R3("VIS_R3","b-jet Jigsaws");
    B_R3.AddFrame(Ba_R3);
    B_R3.AddFrame(Bb_R3);
    B_R3.SetNElementsForFrame(Ba_R3, 1);
    B_R3.SetNElementsForFrame(Bb_R3, 1);
  
    group_name = "#splitline{#nu #nu Jigsaws for}{min (M_{top a}- M_{top b})^{2}}";
    InvisibleGroup INV_R4("INV_R4", group_name);
    INV_R4.AddFrame(Na_R4);
    INV_R4.AddFrame(Nb_R4);
    CombinatoricGroup B_R4("VIS_R4","b-jet Jigsaws");
    B_R4.AddFrame(Ba_R4);
    B_R4.AddFrame(Bb_R4);
    B_R4.SetNElementsForFrame(Ba_R4, 1);
    B_R4.SetNElementsForFrame(Bb_R4, 1);

      //////////////////////// define Jigsaws for reconstruction tree ////////////////////////
  std::string jigsaw_name;

  // Minimize equal top masses neutrino jigsaws
  jigsaw_name = "M_{#nu#nu} = f(m_{b#it{l}b#it{l}} , m_{b#it{l}}^{ a} , m_{b#it{l}}^{ b})";
  SetMassInvJigsaw NuNuM_R1("NuNuM_R1", jigsaw_name);
  INV_R1.AddJigsaw(NuNuM_R1);

  jigsaw_name = "#eta_{#nu#nu} = #eta_{b #it{l} b #it{l}}";
  SetRapidityInvJigsaw NuNuR_R1("NuNuR_R1", jigsaw_name);
  INV_R1.AddJigsaw(NuNuR_R1);
  NuNuR_R1.AddVisibleFrames(La_R1+Ba_R1+Lb_R1+Bb_R1);

  jigsaw_name = "min M_{top}, M_{top}^{ a} = M_{top}^{ b}";
  ContraBoostInvJigsaw MinMt_R1("MinMt_R1", jigsaw_name);
  INV_R1.AddJigsaw(MinMt_R1);
  MinMt_R1.AddVisibleFrames(La_R1+Ba_R1, 0);
  MinMt_R1.AddVisibleFrames(Lb_R1+Bb_R1, 1);
  MinMt_R1.AddInvisibleFrame(Na_R1, 0);
  MinMt_R1.AddInvisibleFrame(Nb_R1, 1);

  // Minimize equal W masses neutrino jigsaws
  jigsaw_name = "M_{#nu#nu} = f(m_{#it{l}#it{l}} , m_{#it{l}}^{ a} , m_{#it{l}}^{ b})";
  SetMassInvJigsaw NuNuM_R2("NuNuM_R2", jigsaw_name);
  INV_R2.AddJigsaw(NuNuM_R2);

  jigsaw_name = "#eta_{#nu#nu} = #eta_{b #it{l} b #it{l}}";
  SetRapidityInvJigsaw NuNuR_R2("NuNuR_R2", jigsaw_name);
  INV_R2.AddJigsaw(NuNuR_R2);
  NuNuR_R2.AddVisibleFrames(La_R2+Ba_R2+Lb_R2+Bb_R2);

  jigsaw_name = "min M_{W}, M_{W}^{ a} = M_{W}^{ b}";
  ContraBoostInvJigsaw MinMW_R2("MinMW_R2", jigsaw_name);
  INV_R2.AddJigsaw(MinMW_R2);
  MinMW_R2.AddVisibleFrame(La_R2, 0);
  MinMW_R2.AddVisibleFrame(Lb_R2, 1);
  MinMW_R2.AddInvisibleFrame(Na_R2, 0);
  MinMW_R2.AddInvisibleFrame(Nb_R2, 1);

  // Minimize sum Mt^2 jigsaws
  jigsaw_name = "M_{#nu#nu} = f(m_{#it{l}#it{l}} , m_{#it{l}}^{ a} , m_{#it{l}}^{ b})";
  SetMassInvJigsaw NuNuM_R3("NuNuM_R3", jigsaw_name);
  INV_R3.AddJigsaw(NuNuM_R3);

  jigsaw_name = "#eta_{#nu#nu} = #eta_{b #it{l} b #it{l}}";
  SetRapidityInvJigsaw NuNuR_R3("NuNuR_R3", jigsaw_name);
  INV_R3.AddJigsaw(NuNuR_R3);
  NuNuR_R3.AddVisibleFrames(LAB_R3.GetListVisibleFrames());

  jigsaw_name = "min #Sigma M_{top}^{2}";
  MinMassesSqInvJigsaw MinMt_R3("MinMt_R3", jigsaw_name, 2);
  INV_R3.AddJigsaw(MinMt_R3);
  MinMt_R3.AddInvisibleFrame(Na_R3, 0);
  MinMt_R3.AddInvisibleFrame(Nb_R3, 1);
  MinMt_R3.AddVisibleFrames(La_R3+Ba_R3, 0);
  MinMt_R3.AddVisibleFrames(Lb_R3+Bb_R3, 1);
  MinMt_R3.AddMassFrame(La_R3, 0);
  MinMt_R3.AddMassFrame(Lb_R3, 1);

  // Minimize difference Mt jigsaws
  jigsaw_name = "M_{#nu#nu} = f(m_{#it{l}#it{l}} , m_{#it{l}}^{ a} , m_{#it{l}}^{ b})";
  SetMassInvJigsaw NuNuM_R4("NuNuM_R4", jigsaw_name);
  INV_R4.AddJigsaw(NuNuM_R4);

  jigsaw_name = "#eta_{#nu#nu} = #eta_{b #it{l} b #it{l}}";
  SetRapidityInvJigsaw NuNuR_R4("NuNuR_R4", jigsaw_name);
  INV_R4.AddJigsaw(NuNuR_R4);
  NuNuR_R4.AddVisibleFrames(LAB_R4.GetListVisibleFrames());

  jigsaw_name = "min ( M_{top a}- M_{top b} )^{2}";
  MinMassDiffInvJigsaw MinDeltaMt_R4("MinDeltaMt_R4", jigsaw_name, 2);
  INV_R4.AddJigsaw(MinDeltaMt_R4);
  MinDeltaMt_R4.AddInvisibleFrame(Na_R4, 0);
  MinDeltaMt_R4.AddInvisibleFrame(Nb_R4, 1);
  MinDeltaMt_R4.AddVisibleFrames(La_R4+Ba_R4, 0);
  MinDeltaMt_R4.AddVisibleFrames(Lb_R4+Bb_R4, 1);
  MinDeltaMt_R4.AddMassFrame(La_R4, 0);
  MinDeltaMt_R4.AddMassFrame(Lb_R4, 1);

  // b-jet combinatoric jigsaws for all trees
  jigsaw_name = "Minimize M(b #it{l} )_{a} , M(b #it{l} )_{b}";

  MinMassesCombJigsaw MinBL_R1("MinBL_R1", jigsaw_name);
  B_R1.AddJigsaw(MinBL_R1);
  MinBL_R1.AddFrames(La_R1+Ba_R1,0);
  MinBL_R1.AddFrames(Lb_R1+Bb_R1,1);

  MinMassesCombJigsaw MinBL_R2("MinBL_R2", jigsaw_name);
  B_R2.AddJigsaw(MinBL_R2);
  MinBL_R2.AddFrames(La_R2+Ba_R2,0);
  MinBL_R2.AddFrames(Lb_R2+Bb_R2,1);

  MinMassesCombJigsaw MinBL_R3("MinBL_R3", jigsaw_name);
  B_R3.AddJigsaw(MinBL_R3);
  MinBL_R3.AddFrames(La_R3+Ba_R3,0);
  MinBL_R3.AddFrames(Lb_R3+Bb_R3,1);

  MinMassesCombJigsaw MinBL_R4("MinBL_R4", jigsaw_name);
  B_R4.AddJigsaw(MinBL_R4);
  MinBL_R4.AddFrames(La_R4+Ba_R4,0);
  MinBL_R4.AddFrames(Lb_R4+Bb_R4,1);

  if(LAB_R1.InitializeAnalysis() && LAB_R2.InitializeAnalysis() && 
     LAB_R3.InitializeAnalysis() && LAB_R4.InitializeAnalysis())
    g_Log << LogInfo << "...Successfully initialized analysis" << LogEnd;
  else
    g_Log << LogError << "...Failed initializing analysis" << LogEnd;	
  
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
  
    TreePlot* treePlot = new TreePlot("TreePlot","TreePlot");
 
    treePlot->SetTree(LAB_Gen);
    treePlot->Draw("GenTree", "Generator Tree", true);
    
    treePlot->SetTree(LAB_R1);
    treePlot->Draw("RecoTree", "Reconstruction Tree");
  
    treePlot->SetTree(B_R1);
    treePlot->Draw("VisTree", "b-jet Jigsaws", true);
  
    treePlot->SetTree(INV_R1);
    treePlot->Draw("InvTree_R1", "Inivisibl Jigsaws", true);
  
    treePlot->SetTree(INV_R2);
    treePlot->Draw("InvTree_R2", "Inivisibl Jigsaws", true);
  
    treePlot->SetTree(INV_R3);
    treePlot->Draw("InvTree_R3", "Inivisibl Jigsaws", true);
  
    treePlot->SetTree(INV_R4);
    treePlot->Draw("InvTree_R4", "Inivisibl Jigsaws", true);
  
    //-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//
  
    HistPlot* histPlot = new HistPlot("HistPlot", "t #bar{t} #rightarrow b W(#it{l} #nu) b W(#it{l} #nu)");
  
    const HistPlotCategory& cat_Gen = histPlot->GetNewCategory("Gen",  "Generator");
    const HistPlotCategory& cat_R1  = histPlot->GetNewCategory("Reco1", "M_{top}^{ a} = M_{top}^{ b} Reco");
    const HistPlotCategory& cat_R2  = histPlot->GetNewCategory("Reco2", "M_{W}^{ a} = M_{W}^{ b} Reco");
    const HistPlotCategory& cat_R3  = histPlot->GetNewCategory("Reco3", "min #Sigma M_{top}^{ 2} Reco");
    const HistPlotCategory& cat_R4  = histPlot->GetNewCategory("Reco4", "min #Delta M_{top} Reco");
  
    const HistPlotVar& Mta    = histPlot->GetNewVar("Mta", "M_{ta}", 0., 300.);
    const HistPlotVar& Mtb    = histPlot->GetNewVar("Mtb", "M_{tb}", 0., 300.);
    const HistPlotVar& MWa    = histPlot->GetNewVar("MWa", "M_{Wa}", 0., 300.);
    const HistPlotVar& MWb    = histPlot->GetNewVar("MWb", "M_{Wb}", 0., 300.);

    const HistPlotVar& MtH   = histPlot->GetNewVar("MtH", "M_{tH}", 0., 300.);
    const HistPlotVar& MtL    = histPlot->GetNewVar("MtL", "M_{tL}", 0., 300.);

    const HistPlotVar& MDt    = histPlot->GetNewVar("MDt", "M_{tH}-M_{tL}|", -5., 5.);
    const HistPlotVar& phitt    = histPlot->GetNewVar("phitt", "#Phi(t,t)", -6., 6.);
    const HistPlotVar& phill = histPlot->GetNewVar("phill","#phi_{#it{l}a} - #phi_{#it{l}b}", -5,5);

    const HistPlotVar& chel    = histPlot->GetNewVar("chel", "c_{hel}", -1., 1.);
    const HistPlotVar& chan    = histPlot->GetNewVar("chan", "c_{han}", -1., 1.);

    const HistPlotVar& nchel    = histPlot->GetNewVar("nchel", "nc_{hel}", -1., 1.);

    const HistPlotVar& hist_mttbar_1_1 = histPlot->GetNewVar("hist_mttbar_1_1", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_1_2 = histPlot->GetNewVar("hist_mttbar_1_2", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_1_3 = histPlot->GetNewVar("hist_mttbar_1_3", "M_{t #bar{t}}", 200., 600.);

    const HistPlotVar& hist_mttbar_2_1 = histPlot->GetNewVar("hist_mttbar_2_1", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_2_2 = histPlot->GetNewVar("hist_mttbar_2_2", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_2_3 = histPlot->GetNewVar("hist_mttbar_2_3", "M_{t #bar{t}}", 200., 600.);

    const HistPlotVar& hist_mttbar_3_1 = histPlot->GetNewVar("hist_mttbar_3_1", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_3_2 = histPlot->GetNewVar("hist_mttbar_3_2", "M_{t #bar{t}}", 200., 600.);
    const HistPlotVar& hist_mttbar_3_3 = histPlot->GetNewVar("hist_mttbar_3_3", "M_{t #bar{t}}", 200., 600.);

    const HistPlotVar& Meta    = histPlot->GetNewVar("Meta", "M_{t #bar{t}} / M_{toponium}", 0., 4.);
    const HistPlotVar& Mtt    = histPlot->GetNewVar("Mtt", "M_{t #bar{t}}", 0., 800.);
    const HistPlotVar& Mll   = histPlot->GetNewVar("Mll", "M_{#it{l}_{a}#it{l}_{b}}", 0., 300.);
    const HistPlotVar& Mllbb   = histPlot->GetNewVar("Mllbb", "M_{#it{l}_{a}#it{l}_{b}bb}", 0., 800.);
    const HistPlotVar& Eb_ta  = histPlot->GetNewVar("Eb_ta", "E_{b a}^{top a}", 0., 100.);
    const HistPlotVar& Eb_tb  = histPlot->GetNewVar("Eb_tb", "E_{b b}^{top b}", 0., 100.);
    const HistPlotVar& El_Wa  = histPlot->GetNewVar("El_Wa", "E_{#it{l} a}^{W a}", 0., 100.);
    const HistPlotVar& El_Wb  = histPlot->GetNewVar("El_Wb", "E_{#it{l} b}^{W b}", 0., 100.);
    const HistPlotVar& costt  = histPlot->GetNewVar("costt","cos #theta_{t #bar{t}}", -1., 1.);
    const HistPlotVar& costa  = histPlot->GetNewVar("costa","cos #theta_{top a}", -1., 1.);
    const HistPlotVar& costb  = histPlot->GetNewVar("costb","cos #theta_{top b}", -1., 1.);
    const HistPlotVar& cosWa  = histPlot->GetNewVar("cosWa","cos #theta_{W a}", -1., 1.);
    const HistPlotVar& cosWb  = histPlot->GetNewVar("cosWb","cos #theta_{W b}", -1., 1.);
    const HistPlotVar& Dcostt = histPlot->GetNewVar("Dcostt","#theta_{t #bar{t}}", 
                -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& Dcosta = histPlot->GetNewVar("Dcosta","#theta_{top a}", 
                -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& Dcostb = histPlot->GetNewVar("Dcostb","#theta_{top b}", 
                -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& DcosWa = histPlot->GetNewVar("DcosWa","#theta_{W a}", 
                -acos(-1.)/2., acos(-1.)/2.);
    const HistPlotVar& DcosWb = histPlot->GetNewVar("DcosWb","#theta_{W b}", 
                -acos(-1.)/2., acos(-1.)/2.);

    histPlot->AddPlot(Mta, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Mtb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(MtH, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(MtL, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(MWa, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(MWb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(phill, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Meta, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(MDt, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Mtt,   cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Eb_ta, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Eb_tb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(El_Wa, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(El_Wb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Mll, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Mllbb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(cosWa, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(cosWb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(costa, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(costb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Dcostt, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Dcosta, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(Dcostb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(DcosWa, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(DcosWb, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(chel, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(chan, cat_R1+cat_R2+cat_R3+cat_R4);
  
    histPlot->AddPlot(nchel, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(phitt, cat_R1+cat_R2+cat_R3+cat_R4);

    histPlot->AddPlot(hist_mttbar_1_1, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_1_2, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_1_3, cat_R1+cat_R2+cat_R3+cat_R4);

    histPlot->AddPlot(hist_mttbar_2_1, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_2_2, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_2_3, cat_R1+cat_R2+cat_R3+cat_R4);

    histPlot->AddPlot(hist_mttbar_3_1, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_3_2, cat_R1+cat_R2+cat_R3+cat_R4);
    histPlot->AddPlot(hist_mttbar_3_3, cat_R1+cat_R2+cat_R3+cat_R4);

    histPlot->AddPlot(phill, phitt, cat_R1);
    histPlot->AddPlot(phill, nchel, cat_R1);
    histPlot->AddPlot(phitt, nchel, cat_R1);
    histPlot->AddPlot(Eb_ta, nchel, cat_R1);
    histPlot->AddPlot(cosWa, nchel, cat_R1);
    histPlot->AddPlot(cosWb, nchel, cat_R1);
    histPlot->AddPlot(costa, nchel, cat_R1);
    histPlot->AddPlot(costb, nchel, cat_R1);
    histPlot->AddPlot(Mll, nchel, cat_R1);
    histPlot->AddPlot(Mllbb, nchel, cat_R1);
    histPlot->AddPlot(Mtt, nchel, cat_R1);

    histPlot->AddPlot(Eb_ta, phitt, cat_R1);
    histPlot->AddPlot(cosWa, phitt, cat_R1);
    histPlot->AddPlot(cosWb, phitt, cat_R1);
    histPlot->AddPlot(costa, phitt, cat_R1);
    histPlot->AddPlot(costb, phitt, cat_R1);
    histPlot->AddPlot(Mll, phitt, cat_R1);
    histPlot->AddPlot(Mllbb, phitt, cat_R1);
    histPlot->AddPlot(Mtt, phitt, cat_R1);




    histPlot->AddPlot(Eb_ta, chel, cat_R1);
    histPlot->AddPlot(Eb_ta, chan, cat_R1);
    histPlot->AddPlot(cosWa, chel, cat_R1);
    histPlot->AddPlot(cosWa, chan, cat_R1);
    histPlot->AddPlot(cosWb, chel, cat_R1);
    histPlot->AddPlot(cosWb, chan, cat_R1);
    histPlot->AddPlot(costa, chel, cat_R1);
    histPlot->AddPlot(costa, chan, cat_R1);
    histPlot->AddPlot(costb, chel, cat_R1);
    histPlot->AddPlot(costb, chan, cat_R1);
    histPlot->AddPlot(Mll, chel, cat_R1);
    histPlot->AddPlot(Mll, chan, cat_R1);
    histPlot->AddPlot(Mllbb, chel, cat_R1);
    histPlot->AddPlot(Mllbb, chan, cat_R1);
    histPlot->AddPlot(Meta, Eb_ta, cat_R1);
    histPlot->AddPlot(Mtt, Eb_ta, cat_R1);
    histPlot->AddPlot(Mtt, Mll, cat_R1);
    histPlot->AddPlot(Mtt, Mllbb, cat_R1);
    histPlot->AddPlot(Mtt, chel, cat_R1);
    histPlot->AddPlot(Mtt, chan, cat_R1);
    histPlot->AddPlot(Mtt, El_Wa, cat_R1);
    histPlot->AddPlot(Mtt, Eb_tb, cat_R1);
    histPlot->AddPlot(Mtt, El_Wb, cat_R1);
    histPlot->AddPlot(Eb_ta, Eb_tb, cat_R1);
    histPlot->AddPlot(El_Wa, El_Wb, cat_R1);
    histPlot->AddPlot(Eb_ta, El_Wa, cat_R1);
    histPlot->AddPlot(Eb_ta, El_Wb, cat_R1);
    histPlot->AddPlot(Dcostt, Mtt,  cat_R1);
    histPlot->AddPlot(Dcosta, Eb_ta, cat_R1);
    histPlot->AddPlot(Dcostb, Eb_tb, cat_R1);
    histPlot->AddPlot(DcosWa, El_Wa, cat_R1);
    histPlot->AddPlot(DcosWb, El_Wb, cat_R1);
    histPlot->AddPlot(Dcostt, Dcosta, cat_R1);
    histPlot->AddPlot(Dcosta, Dcostb, cat_R1);
    histPlot->AddPlot(DcosWa, DcosWb, cat_R1);
    histPlot->AddPlot(Dcosta, DcosWa, cat_R1);
    histPlot->AddPlot(Dcostb, DcosWb, cat_R1);
    histPlot->AddPlot(Mta, Mtb, cat_R1);
    histPlot->AddPlot(MtH, MtL, cat_R1);
    histPlot->AddPlot(Mta, MWa, cat_R1);
    histPlot->AddPlot(Mtb, MWb, cat_R1);

    histPlot->AddPlot(phill, phitt, cat_R2);
    histPlot->AddPlot(phill, nchel, cat_R2);
    histPlot->AddPlot(phitt, nchel, cat_R2);
    histPlot->AddPlot(Eb_ta, nchel, cat_R2);
    histPlot->AddPlot(cosWa, nchel, cat_R2);
    histPlot->AddPlot(cosWb, nchel, cat_R2);
    histPlot->AddPlot(costa, nchel, cat_R2);
    histPlot->AddPlot(costb, nchel, cat_R2);
    histPlot->AddPlot(Mll, nchel, cat_R2);
    histPlot->AddPlot(Mllbb, nchel, cat_R2);
    histPlot->AddPlot(Mtt, nchel, cat_R2);



    histPlot->AddPlot(Eb_ta, phitt, cat_R2);
    histPlot->AddPlot(cosWa, phitt, cat_R2);
    histPlot->AddPlot(cosWb, phitt, cat_R2);
    histPlot->AddPlot(costa, phitt, cat_R2);
    histPlot->AddPlot(costb, phitt, cat_R2);
    histPlot->AddPlot(Mll, phitt, cat_R2);
    histPlot->AddPlot(Mllbb, phitt, cat_R2);
    histPlot->AddPlot(Mtt, phitt, cat_R2);




    histPlot->AddPlot(Eb_ta, chel, cat_R2);
    histPlot->AddPlot(Eb_ta, chan, cat_R2);
    histPlot->AddPlot(cosWa, chel, cat_R2);
    histPlot->AddPlot(cosWa, chan, cat_R2);
    histPlot->AddPlot(cosWb, chel, cat_R2);
    histPlot->AddPlot(cosWb, chan, cat_R2);
    // histPlot->AddPlot(cosWb-cosWa, chel, cat_R2);
    // histPlot->AddPlot(cosWb-cosWa, chan, cat_R2);
    histPlot->AddPlot(costa, chel, cat_R2);
    histPlot->AddPlot(costa, chan, cat_R2);
    histPlot->AddPlot(costb, chel, cat_R2);
    histPlot->AddPlot(costb, chan, cat_R2);
    histPlot->AddPlot(Mll, chel, cat_R2);
    histPlot->AddPlot(Mll, chan, cat_R2);
    histPlot->AddPlot(Mllbb, chel, cat_R2);
    histPlot->AddPlot(Mllbb, chan, cat_R2);

    histPlot->AddPlot(Meta, Eb_ta, cat_R2);
    histPlot->AddPlot(Mtt, Eb_ta, cat_R2);
    histPlot->AddPlot(Mtt, Mll, cat_R2);
    histPlot->AddPlot(Mtt, Mllbb, cat_R2);
    histPlot->AddPlot(Mtt, chel, cat_R2);
    histPlot->AddPlot(Mtt, chan, cat_R2);
    histPlot->AddPlot(Mtt, El_Wa, cat_R2);
    histPlot->AddPlot(Mtt, Eb_tb, cat_R2);
    histPlot->AddPlot(Mtt, El_Wb, cat_R2);
    histPlot->AddPlot(Eb_ta, Eb_tb, cat_R2);
    histPlot->AddPlot(El_Wa, El_Wb, cat_R2);
    histPlot->AddPlot(Eb_ta, El_Wa, cat_R2);
    histPlot->AddPlot(Eb_ta, El_Wb, cat_R2);
    histPlot->AddPlot(Dcostt, Mtt,  cat_R2);
    histPlot->AddPlot(Dcosta, Eb_ta, cat_R2);
    histPlot->AddPlot(Dcostb, Eb_tb, cat_R2);
    histPlot->AddPlot(DcosWa, El_Wa, cat_R2);
    histPlot->AddPlot(DcosWb, El_Wb, cat_R2);
    histPlot->AddPlot(Dcostt, Dcosta, cat_R2);
    histPlot->AddPlot(Dcosta, Dcostb, cat_R2);
    histPlot->AddPlot(DcosWa, DcosWb, cat_R2);
    histPlot->AddPlot(Dcosta, DcosWa, cat_R2);
    histPlot->AddPlot(Dcostb, DcosWb, cat_R2);
    histPlot->AddPlot(Mta, Mtb, cat_R2);
    histPlot->AddPlot(MtH, MtL, cat_R2);
    histPlot->AddPlot(Mta, MWa, cat_R2);
    histPlot->AddPlot(Mtb, MWb, cat_R2);

    histPlot->AddPlot(phill, phitt, cat_R3);
    histPlot->AddPlot(phill, nchel, cat_R3);
    histPlot->AddPlot(phitt, nchel, cat_R3);
    histPlot->AddPlot(Eb_ta, nchel, cat_R3);
    histPlot->AddPlot(cosWa, nchel, cat_R3);
    histPlot->AddPlot(cosWb, nchel, cat_R3);
    histPlot->AddPlot(costa, nchel, cat_R3);
    histPlot->AddPlot(costb, nchel, cat_R3);
    histPlot->AddPlot(Mll, nchel, cat_R3);
    histPlot->AddPlot(Mllbb, nchel, cat_R3);
    histPlot->AddPlot(Mtt, nchel, cat_R3);

    histPlot->AddPlot(Eb_ta, phitt, cat_R3);
    histPlot->AddPlot(cosWa, phitt, cat_R3);
    histPlot->AddPlot(cosWb, phitt, cat_R3);
    histPlot->AddPlot(costa, phitt, cat_R3);
    histPlot->AddPlot(costb, phitt, cat_R3);
    histPlot->AddPlot(Mll, phitt, cat_R3);
    histPlot->AddPlot(Mllbb, phitt, cat_R3);
    histPlot->AddPlot(Mtt, phitt, cat_R3);

    histPlot->AddPlot(Eb_ta, chel, cat_R3);
    histPlot->AddPlot(Eb_ta, chan, cat_R3);
    histPlot->AddPlot(cosWa, chel, cat_R3);
    histPlot->AddPlot(cosWa, chan, cat_R3);
    histPlot->AddPlot(cosWb, chel, cat_R3);
    histPlot->AddPlot(cosWb, chan, cat_R3);
    // histPlot->AddPlot(cosWb-cosWa, chel, cat_R3);
    // histPlot->AddPlot(cosWb-cosWa, chan, cat_R3);
    histPlot->AddPlot(costa, chel, cat_R3);
    histPlot->AddPlot(costa, chan, cat_R3);
    histPlot->AddPlot(costb, chel, cat_R3);
    histPlot->AddPlot(costb, chan, cat_R3);
    histPlot->AddPlot(Mll, chel, cat_R3);
    histPlot->AddPlot(Mll, chan, cat_R3);
    histPlot->AddPlot(Mllbb, chel, cat_R3);
    histPlot->AddPlot(Mllbb, chan, cat_R3);


    histPlot->AddPlot(Meta, Eb_ta, cat_R3);
    histPlot->AddPlot(Mtt, Eb_ta, cat_R3);
    histPlot->AddPlot(Mtt, Mll, cat_R3);
    histPlot->AddPlot(Mtt, Mllbb, cat_R3);
    histPlot->AddPlot(Mtt, chel, cat_R3);
    histPlot->AddPlot(Mtt, chan, cat_R3);
    histPlot->AddPlot(Mtt, El_Wa, cat_R3);
    histPlot->AddPlot(Mtt, Eb_tb, cat_R3);
    histPlot->AddPlot(Mtt, El_Wb, cat_R3);
    histPlot->AddPlot(Eb_ta, Eb_tb, cat_R3);
    histPlot->AddPlot(El_Wa, El_Wb, cat_R3);
    histPlot->AddPlot(Eb_ta, El_Wa, cat_R3);
    histPlot->AddPlot(Eb_ta, El_Wb, cat_R3);
    histPlot->AddPlot(Dcostt, Mtt,  cat_R3);
    histPlot->AddPlot(Dcosta, Eb_ta, cat_R3);
    histPlot->AddPlot(Dcostb, Eb_tb, cat_R3);
    histPlot->AddPlot(DcosWa, El_Wa, cat_R3);
    histPlot->AddPlot(DcosWb, El_Wb, cat_R3);
    histPlot->AddPlot(Dcostt, Dcosta, cat_R3);
    histPlot->AddPlot(Dcosta, Dcostb, cat_R3);
    histPlot->AddPlot(DcosWa, DcosWb, cat_R3);
    histPlot->AddPlot(Dcosta, DcosWa, cat_R3);
    histPlot->AddPlot(Dcostb, DcosWb, cat_R3);
    histPlot->AddPlot(Mta, Mtb, cat_R3);
    histPlot->AddPlot(MtH, MtL, cat_R3);
    histPlot->AddPlot(Mta, MWa, cat_R3);
    histPlot->AddPlot(Mtb, MWb, cat_R3);


    histPlot->AddPlot(phill, phitt, cat_R4);
    histPlot->AddPlot(phill, nchel, cat_R4);
    histPlot->AddPlot(phitt, nchel, cat_R4);
    histPlot->AddPlot(Eb_ta, nchel, cat_R4);
    histPlot->AddPlot(cosWa, nchel, cat_R4);
    histPlot->AddPlot(cosWb, nchel, cat_R4);
    histPlot->AddPlot(costa, nchel, cat_R4);
    histPlot->AddPlot(costb, nchel, cat_R4);
    histPlot->AddPlot(Mll, nchel, cat_R4);
    histPlot->AddPlot(Mllbb, nchel, cat_R4);
    histPlot->AddPlot(Mtt, nchel, cat_R4);

    histPlot->AddPlot(Eb_ta, phitt, cat_R4);
    histPlot->AddPlot(cosWa, phitt, cat_R4);
    histPlot->AddPlot(cosWb, phitt, cat_R4);
    histPlot->AddPlot(costa, phitt, cat_R4);
    histPlot->AddPlot(costb, phitt, cat_R4);
    histPlot->AddPlot(Mll, phitt, cat_R4);
    histPlot->AddPlot(Mllbb, phitt, cat_R4);
    histPlot->AddPlot(Mtt, phitt, cat_R4);

    histPlot->AddPlot(Eb_ta, chel, cat_R4);
    histPlot->AddPlot(Eb_ta, chan, cat_R4);
    histPlot->AddPlot(cosWa, chel, cat_R4);
    histPlot->AddPlot(cosWa, chan, cat_R4);
    histPlot->AddPlot(cosWb, chel, cat_R4);
    histPlot->AddPlot(cosWb, chan, cat_R4);
    histPlot->AddPlot(costa, chel, cat_R4);
    histPlot->AddPlot(costa, chan, cat_R4);
    histPlot->AddPlot(costb, chel, cat_R4);
    histPlot->AddPlot(costb, chan, cat_R4);
    histPlot->AddPlot(Mll, chel, cat_R4);
    histPlot->AddPlot(Mll, chan, cat_R4);
    histPlot->AddPlot(Mllbb, chel, cat_R4);
    histPlot->AddPlot(Mllbb, chan, cat_R4);


    histPlot->AddPlot(Meta, Eb_ta, cat_R4);
    histPlot->AddPlot(Mtt, Eb_ta, cat_R4);
    histPlot->AddPlot(Mtt, Mll, cat_R4);
    histPlot->AddPlot(Mtt, Mllbb, cat_R4);
    histPlot->AddPlot(Mtt, chel, cat_R4);
    histPlot->AddPlot(Mtt, chan, cat_R4);
    histPlot->AddPlot(Mtt, El_Wa, cat_R4);
    histPlot->AddPlot(Mtt, Eb_tb, cat_R4);
    histPlot->AddPlot(Mtt, El_Wb, cat_R4);
    histPlot->AddPlot(Eb_ta, Eb_tb, cat_R4);
    histPlot->AddPlot(El_Wa, El_Wb, cat_R4);
    histPlot->AddPlot(Eb_ta, El_Wa, cat_R4);
    histPlot->AddPlot(Eb_ta, El_Wb, cat_R4);
    histPlot->AddPlot(Dcostt, Mtt,  cat_R4);
    histPlot->AddPlot(Dcosta, Eb_ta, cat_R4);
    histPlot->AddPlot(Dcostb, Eb_tb, cat_R4);
    histPlot->AddPlot(DcosWa, El_Wa, cat_R4);
    histPlot->AddPlot(DcosWb, El_Wb, cat_R4);
    histPlot->AddPlot(Dcostt, Dcosta, cat_R4);
    histPlot->AddPlot(Dcosta, Dcostb, cat_R4);
    histPlot->AddPlot(DcosWa, DcosWb, cat_R4);
    histPlot->AddPlot(Dcosta, DcosWa, cat_R4);
    histPlot->AddPlot(Dcostb, DcosWb, cat_R4);
    histPlot->AddPlot(Mta, Mtb, cat_R4);
    histPlot->AddPlot(MtH, MtL, cat_R4);
    histPlot->AddPlot(Mta, MWa, cat_R4);
    histPlot->AddPlot(Mtb, MWb, cat_R4);

    TH1 *histNmttbar_1_1 = new TH1D("histNmttbar_1_1", "histNmttbar_1_1", 200, 100, 1000);
    TH1 *histNmttbar_1_2 = new TH1D("histNmttbar_1_2", "histNmttbar_1_2", 200, 100, 1000);
    TH1 *histNmttbar_1_3 = new TH1D("histNmttbar_1_3", "histNmttbar_1_3", 200, 100, 1000);

    TH1 *histNmttbar_2_1 = new TH1D("histNmttbar_2_1", "histNmttbar_2_1", 200, 100, 1000);
    TH1 *histNmttbar_2_2 = new TH1D("histNmttbar_2_2", "histNmttbar_2_2", 200, 100, 1000);
    TH1 *histNmttbar_2_3 = new TH1D("histNmttbar_2_3", "histNmttbar_2_3", 200, 100, 1000);

    TH1 *histNmttbar_3_1 = new TH1D("histNmttbar_3_1", "histNmttbar_3_1", 200, 100, 1000);
    TH1 *histNmttbar_3_2 = new TH1D("histNmttbar_3_2", "histNmttbar_3_2", 200, 100, 1000);
    TH1 *histNmttbar_3_3 = new TH1D("histNmttbar_3_3", "histNmttbar_3_3", 200, 100, 1000);
    // some hists

    TH1 *hist_toponium_mass = new TH1D("mass","mass",200,200,1000);
    TH1 *hist_top_mass = new TH1D("heavy_top","heavy_top",50,100,250);
    TH1 *hist_antitop_mass = new TH1D("light_top","light_top",50,100,250);
    TH1 *hist_dilep_mass = new TH1D("hist_dilep_mass","hist_dilep_mass",50,0,250);
    TH1 *hist_dilep_angle = new TH1D("hist_dilep_angle","hist_dilep_angle",50,-5,5);

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    TVector3 la_ta, lb_tb, tb_vec;
    Double_t weight, cross_section; 

    for(Int_t entry=0; entry < total_entries; ++entry){
    //for(Int_t entry=0; entry < total_entries; ++entry){
      
      tree->GetEntry(entry);

      if(sample == "toponium"){cross_section = 0.2767;}
      if(sample == "ttbar"){cross_section = 36.9;}
      weight = cross_section*lumi*1000/total_entries;

      //if(sample == "toponium"){weight = 1./133;}
      //if(sample == "ttbar"){weight = 1.;}

      top.SetPxPyPzE(0,0,0,0);
      antitop.SetPxPyPzE(0,0,0,0);
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
      Float_t mTopo = 0.0;
  
    for(Int_t i=0; i < pid->size(); ++i){

      if(pid->at(i)==32 & status->at(i) == 3){
        mTopo = mass->at(i);
      }

        //truth tops
        if(pid->at(i)==6){
          top.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
        }

        if(pid->at(i)==-6){
          antitop.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
        }
        // heavy and less heavy top differentiated later on


        if(status->at(i) == 1){ //considering only final state particles 

        //get first two leptons, b-jets, and missing momentum
        // MET
        if(abs(pid->at(i)) == 12 || abs(pid->at(i)) == 14){
          MET.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));

          myMET.SetPtEtaPhi(MET.Pt(),0.0,MET.Phi());
          myMET.SetZ(0.0);
      
        }        
        
        //leptons
        if(pid->at(i) == 11 || pid->at(i) == 13){
          if(lep1.E() ==0){
            lep_count+=1;
            lep1.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
          }
        }

        if(pid->at(i) == -11 || pid->at(i) == -13){
          if(lep2.E() ==0){
            lep_count+=1;
            lep2.SetPxPyPzE(px->at(i),py->at(i),pz->at(i),e->at(i));
          }
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
      }

    }//particle loop

    if(bjet_count < 2){continue;}
    if(lep_count < 2){continue;}

    b1.SetPxPyPzE(px->at(b_index1),py->at(b_index1),pz->at(b_index1),e->at(b_index1));
    b2.SetPxPyPzE(px->at(b_index2),py->at(b_index2),pz->at(b_index2),e->at(b_index2));
    
    // if(lep1.Pt() < 5 || lep2.Pt() < 5){continue;}
    // if(abs(lep1.Eta()) > 2.5 || abs(lep1.Eta()) > 2.5){continue;}

    // if(b1.Pt() < 5 || b2.Pt() < 5){continue;}
    // if(abs(b1.Eta()) > 2.5 || abs(b2.Eta()) > 2.5){continue;}
    
     if(lep1.Pt() < 15 || lep2.Pt() < 15){continue;}
    if(abs(lep1.Eta()) > 2.5 || abs(lep1.Eta()) > 2.5){continue;}

    if(b1.Pt() < 20 || b2.Pt() < 20){continue;}
    if(abs(b1.Eta()) > 2.5 || abs(b2.Eta()) > 2.5){continue;}
       
    
    if(top.M() > 0 && antitop.M() > 0){
      hist_toponium_mass->Fill((top+antitop).M());
      if(top.M() > antitop.M())
      {
        hist_top_mass->Fill(top.M());
        hist_antitop_mass->Fill(antitop.M());
      }

      else{
      hist_top_mass->Fill(antitop.M());
      hist_antitop_mass->Fill(top.M());
      }
    }
    
    hist_dilep_angle->Fill(lep1.Phi() -lep2.Phi());
    hist_dilep_mass->Fill((lep1+lep2).M());
    
    //cout << mTopo << endl;
    //if(mTopo <= 0.){mTopo = mH;};
    
    mTopo = mH;
    
    LAB_Gen.InitializeAnalysis();

    LAB_Gen.ClearEvent();                             // clear the gen tree
    LAB_Gen.AnalyzeEvent();                           // generate a new event

    LAB_R1.ClearEvent();   
    LAB_R2.ClearEvent(); 
    LAB_R3.ClearEvent(); 
    LAB_R4.ClearEvent(); 
             
    INV_R1.SetLabFrameThreeVector(myMET); 
    INV_R2.SetLabFrameThreeVector(myMET); 
    INV_R3.SetLabFrameThreeVector(myMET);
    INV_R4.SetLabFrameThreeVector(myMET);

    La_R1.SetLabFrameFourVector(lep1);
    Lb_R1.SetLabFrameFourVector(lep2);
    La_R2.SetLabFrameFourVector(lep1);
    Lb_R2.SetLabFrameFourVector(lep2);
    La_R3.SetLabFrameFourVector(lep1);
    Lb_R3.SetLabFrameFourVector(lep2);
    La_R4.SetLabFrameFourVector(lep1);
    Lb_R4.SetLabFrameFourVector(lep2);

    std::vector<RFKey> B_R1_ID; // ID for tracking jets in tree
    B_R1_ID.push_back(B_R1.AddLabFrameFourVector(b1));
    B_R1_ID.push_back(B_R1.AddLabFrameFourVector(b2));
    B_R2.AddLabFrameFourVector(b1);
    B_R2.AddLabFrameFourVector(b2);
    B_R3.AddLabFrameFourVector(b1);
    B_R3.AddLabFrameFourVector(b2);
    B_R4.AddLabFrameFourVector(b1);
    B_R4.AddLabFrameFourVector(b2);
   
    LAB_R1.AnalyzeEvent(); // analyze the event
    LAB_R2.AnalyzeEvent();     
    LAB_R3.AnalyzeEvent(); 
    LAB_R4.AnalyzeEvent(); 

    //////////////////////////////////////
    // Observable Calculations
    //////////////////////////////////////

    double Mttgen   = TT_Gen.GetMass();

    double Eb_tagen = Ba_Gen.GetFourVector(Ta_Gen).E();
    double Eb_tbgen = Bb_Gen.GetFourVector(Tb_Gen).E();
    double El_Wagen = La_Gen.GetFourVector(Wa_Gen).E();
    double El_Wbgen = Lb_Gen.GetFourVector(Wb_Gen).E();
    double costtgen = TT_Gen.GetCosDecayAngle();
    double costagen = Ta_Gen.GetCosDecayAngle();
    double costbgen = Tb_Gen.GetCosDecayAngle();
    double cosWagen = Wa_Gen.GetCosDecayAngle();
    double cosWbgen = Wb_Gen.GetCosDecayAngle();


    Meta = TT_R1.GetMass()/mTopo;
    Mtt = TT_R1.GetMass();
    Mta = Ta_R1.GetMass();
    Mtb = Tb_R1.GetMass();
    MWa = Wa_R1.GetMass();
    MWb = Wb_R1.GetMass();
    Mll = (La_R1+Lb_R1).GetMass();
    Mllbb = (La_R1+Lb_R1+Ba_R1+Bb_R1).GetMass();
    phitt = Ta_R1.GetFourVector().Phi() - Tb_R1.GetFourVector().Phi(); 

    if(Mta > Mtb){
      MtH = Mta;
      MtL = Mtb;
      MDt = Mta - Mtb;
    }
    else{
      MtH = Mtb;
      MtL = Mta;
      MDt = Mtb - Mta;
    }

    TVector3 boostToTTbarCM, boostToTopRest,  boostToAntitopRest ; 
    TLorentzVector lep_boosteda,lep_boostedb, topInTTbar, antitopInTTbar;

    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);
    topInTTbar.SetPxPyPzE(0, 0, 0, 0);
    antitopInTTbar.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM.SetXYZ(0, 0, 0);
    boostToTopRest.SetXYZ(0, 0, 0);
    boostToAntitopRest.SetXYZ(0, 0, 0);

    // Initialize TLorentzVector to zero
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM = -(Ta_R1.GetFourVector() + Tb_R1.GetFourVector()).BoostVector();
    // Step 2: Boost top and antitop 4-vectors into ttbar CM frame
    topInTTbar = Ta_R1.GetFourVector();
    topInTTbar.Boost(boostToTTbarCM);

    antitopInTTbar = Tb_R1.GetFourVector();
    antitopInTTbar.Boost(boostToTTbarCM);

    // Step 3: Calculate boost vectors to go from ttbar CM frame to each top rest frame
    // (negative because BoostVector() gives boost from rest to lab frame)
    boostToTopRest = -topInTTbar.BoostVector();
    boostToAntitopRest = -antitopInTTbar.BoostVector();

    // Step 4: Boost leptons into the ttbar CM frame first
    lep_boosteda = La_R1.GetFourVector();
    lep_boosteda.Boost(boostToTTbarCM);

    lep_boostedb = Lb_R1.GetFourVector();
    lep_boostedb.Boost(boostToTTbarCM);

    // Step 5: Boost leptons from ttbar CM frame into their respective parent top rest frames
    // Assuming lep1 comes from antitop and lep2 from top (or adjust accordingly)
    lep_boosteda.Boost(boostToTopRest);
    lep_boostedb.Boost(boostToAntitopRest);

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());
    
    chel = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

    chan = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

    TVector3 boostVec, boostVeca, boostVecb;
  
    boostVec.SetXYZ(0, 0, 0);
    boostVeca.SetXYZ(0, 0, 0);
    boostVecb.SetXYZ(0, 0, 0);

    boostVec = -(Ta_R1.GetFourVector() + Tb_R1.GetFourVector()).BoostVector();
    lep_boosteda = La_R1.GetFourVector();
    lep_boostedb = Lb_R1.GetFourVector();
    boostVeca = Ta_R1.GetFourVector().BoostVector();
    boostVecb = Tb_R1.GetFourVector().BoostVector();
    lep_boosteda.Boost(boostVec);
    lep_boostedb.Boost(boostVec);
    lep_boosteda.Boost(boostVeca);
    lep_boostedb.Boost(boostVecb);

    TVector3 leptopa, leptopb;

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());

    nchel = leptopa.Dot(leptopb)/(leptopa.Mag()*leptopb.Mag());

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

// Δφ bin 1: [-6, -2]
// nchel bin 1: [-1, -0.4]
if (phitt > -6. && phitt < -2. && nchel > -1. && nchel < -0.4) {
    histNmttbar_1_1->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -6. && phitt < -2. && nchel > -0.4 && nchel < 0.4) {
    histNmttbar_1_2->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 3: [0.4, 1]
if (phitt > -6. && phitt < -2. && nchel > 0.4 && nchel < 1.) {
    histNmttbar_1_3->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}

// Δφ bin 2: [-2, 2]
// nchel bin 1: [-1, -0.4]
if (phitt > -2. && phitt < 2. && nchel > -1. && nchel < -0.4) {
    histNmttbar_2_1->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -2. && phitt < 2. && nchel > -0.4 && nchel < 0.4) {
    histNmttbar_2_2->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 3: [0.4, 1]
if (phitt > -2. && phitt < 2. && nchel > 0.4 && nchel < 1.) {
    histNmttbar_2_3->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}

// Δφ bin 3: [2, 6]
// nchel bin 1: [-1, -0.4]
if (phitt > 2. && phitt < 6. && nchel > -1. && nchel < -0.4) {
    histNmttbar_3_1->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > 2. && phitt < 6. && nchel > -0.4 && nchel < 0.4) {
    histNmttbar_3_2->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}
// nchel bin 3: [0.4, 1]
if (phitt > 2. && phitt < 6. && nchel > 0.4 && nchel < 1.) {
    histNmttbar_3_3->Fill((Ta_R1.GetFourVector()+Tb_R1.GetFourVector()).M(), weight);
}



// Δφ bin 1: [-6, -2]
// nchel bin 1: [-1, -0.4]
if (phitt > -6. && phitt < -2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_1_1 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -6. && phitt < -2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_1_2 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -6. && phitt < -2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_1_3 = (Ta_R1+Tb_R1).GetMass();
}

// Δφ bin 2: [-2, 2]
// nchel bin 1: [-1, -0.4]
if (phitt > -2. && phitt < 2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_2_1 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -2. && phitt < 2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_2_2 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -2. && phitt < 2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_2_3 = (Ta_R1+Tb_R1).GetMass();
}

// Δφ bin 3: [2, 6]
// nchel bin 1: [-1, -0.4]
if (phitt > 2. && phitt < 6. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_3_1 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > 2. && phitt < 6. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_3_2 = (Ta_R1+Tb_R1).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > 2. && phitt < 6. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_3_3 = (Ta_R1+Tb_R1).GetMass();
}



    Eb_ta = Ba_R1.GetFourVector(Ta_R1).E();
    Eb_tb = Bb_R1.GetFourVector(Tb_R1).E();
    El_Wa = La_R1.GetFourVector(Wa_R1).E();
    El_Wb = Lb_R1.GetFourVector(Wb_R1).E();
    costt = TT_R1.GetCosDecayAngle();
    costa = Ta_R1.GetCosDecayAngle();
    costb = Tb_R1.GetCosDecayAngle();
    cosWa = Wa_R1.GetCosDecayAngle();
    cosWb = Wb_R1.GetCosDecayAngle();
    Dcostt = asin(sqrt(1.-costt*costt)*costtgen-sqrt(1.-costtgen*costtgen)*costt);
    Dcosta = asin(sqrt(1.-costa*costa)*costagen-sqrt(1.-costagen*costagen)*costa);
    Dcostb = asin(sqrt(1.-costb*costb)*costbgen-sqrt(1.-costbgen*costbgen)*costb);
    DcosWa = asin(sqrt(1.-cosWa*cosWa)*cosWagen-sqrt(1.-cosWagen*cosWagen)*cosWa);
    DcosWb = asin(sqrt(1.-cosWb*cosWb)*cosWbgen-sqrt(1.-cosWbgen*cosWbgen)*cosWb);
    
    phill = La_R1.GetFourVector().Phi() - Lb_R1.GetFourVector().Phi();

    histPlot->Fill(cat_R1);

    Meta = TT_R2.GetMass()/mTopo;
    Mtt = TT_R2.GetMass();
    Mta = Ta_R2.GetMass();
    Mtb = Tb_R2.GetMass();
    MWa = Wa_R2.GetMass();
    MWb = Wb_R2.GetMass();
    Mll = (La_R2+Lb_R2).GetMass();
    Mllbb = (La_R2+Lb_R2+Ba_R2+Bb_R2).GetMass();
    phitt = Ta_R2.GetFourVector().Phi() - Tb_R2.GetFourVector().Phi(); 

    if(Mta > Mtb){
      MtH = Mta;
      MtL = Mtb;
      MDt = Mta - Mtb;
    }
    else{
      MtH = Mtb;
      MtL = Mta;
      MDt = Mtb - Mta;
    }

    

    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);
    topInTTbar.SetPxPyPzE(0, 0, 0, 0);
    antitopInTTbar.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM.SetXYZ(0, 0, 0);
    boostToTopRest.SetXYZ(0, 0, 0);
    boostToAntitopRest.SetXYZ(0, 0, 0);

    // Initialize TLorentzVector to zero
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM = -(Ta_R2.GetFourVector() + Tb_R2.GetFourVector()).BoostVector();
    // Step 2: Boost top and antitop 4-vectors into ttbar CM frame
    topInTTbar = Ta_R2.GetFourVector();
    topInTTbar.Boost(boostToTTbarCM);

    antitopInTTbar = Tb_R2.GetFourVector();
    antitopInTTbar.Boost(boostToTTbarCM);

    // Step 3: Calculate boost vectors to go from ttbar CM frame to each top rest frame
    // (negative because BoostVector() gives boost from rest to lab frame)
    boostToTopRest = -topInTTbar.BoostVector();
    boostToAntitopRest = -antitopInTTbar.BoostVector();

    // Step 4: Boost leptons into the ttbar CM frame first
    lep_boosteda = La_R2.GetFourVector();
    lep_boosteda.Boost(boostToTTbarCM);

    lep_boostedb = Lb_R2.GetFourVector();
    lep_boostedb.Boost(boostToTTbarCM);

    // Step 5: Boost leptons from ttbar CM frame into their respective parent top rest frames
    // Assuming lep1 comes from antitop and lep2 from top (or adjust accordingly)
    lep_boosteda.Boost(boostToTopRest);
    lep_boostedb.Boost(boostToAntitopRest);

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());
    
    chel = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

    chan = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

  
    boostVec.SetXYZ(0, 0, 0);
    boostVeca.SetXYZ(0, 0, 0);
    boostVecb.SetXYZ(0, 0, 0);

    boostVec = -(Ta_R2.GetFourVector() + Tb_R2.GetFourVector()).BoostVector();
    lep_boosteda = La_R2.GetFourVector();
    lep_boostedb = Lb_R2.GetFourVector();
    boostVeca = Ta_R2.GetFourVector().BoostVector();
    boostVecb = Tb_R2.GetFourVector().BoostVector();
    lep_boosteda.Boost(boostVec);
    lep_boostedb.Boost(boostVec);
    lep_boosteda.Boost(boostVeca);
    lep_boostedb.Boost(boostVecb);

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());

    nchel = leptopa.Dot(leptopb)/(leptopa.Mag()*leptopb.Mag());

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

// Δφ bin 1: [-6, -2]
// nchel bin 1: [-1, -0.4]
if (phitt > -6. && phitt < -2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_1_1 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -6. && phitt < -2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_1_2 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -6. && phitt < -2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_1_3 = (Ta_R2+Tb_R2).GetMass();
}

// Δφ bin 2: [-2, 2]
// nchel bin 1: [-1, -0.4]
if (phitt > -2. && phitt < 2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_2_1 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -2. && phitt < 2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_2_2 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -2. && phitt < 2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_2_3 = (Ta_R2+Tb_R2).GetMass();
}

// Δφ bin 3: [2, 6]
// nchel bin 1: [-1, -0.4]
if (phitt > 2. && phitt < 6. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_3_1 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > 2. && phitt < 6. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_3_2 = (Ta_R2+Tb_R2).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > 2. && phitt < 6. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_3_3 = (Ta_R2+Tb_R2).GetMass();;
}


    Eb_ta = Ba_R2.GetFourVector(Ta_R2).E();
    Eb_tb = Bb_R2.GetFourVector(Tb_R2).E();
    El_Wa = La_R2.GetFourVector(Wa_R2).E();
    El_Wb = Lb_R2.GetFourVector(Wb_R2).E();
    costt = TT_R2.GetCosDecayAngle();
    costa = Ta_R2.GetCosDecayAngle();
    costb = Tb_R2.GetCosDecayAngle();
    cosWa = Wa_R2.GetCosDecayAngle();
    cosWb = Wb_R2.GetCosDecayAngle();
    Dcostt = asin(sqrt(1.-costt*costt)*costtgen-sqrt(1.-costtgen*costtgen)*costt);
    Dcosta = asin(sqrt(1.-costa*costa)*costagen-sqrt(1.-costagen*costagen)*costa);
    Dcostb = asin(sqrt(1.-costb*costb)*costbgen-sqrt(1.-costbgen*costbgen)*costb);
    DcosWa = asin(sqrt(1.-cosWa*cosWa)*cosWagen-sqrt(1.-cosWagen*cosWagen)*cosWa);
    DcosWb = asin(sqrt(1.-cosWb*cosWb)*cosWbgen-sqrt(1.-cosWbgen*cosWbgen)*cosWb);

    phill = La_R2.GetFourVector().Phi() - Lb_R2.GetFourVector().Phi();

    histPlot->Fill(cat_R2);

    Meta = TT_R3.GetMass()/mTopo;
    Mtt = TT_R3.GetMass();
    Mta = Ta_R3.GetMass();
    Mtb = Tb_R3.GetMass();
    MWa = Wa_R3.GetMass();
    MWb = Wb_R3.GetMass();
    Mll = (La_R3+Lb_R3).GetMass();
    Mllbb = (La_R3+Lb_R3+Ba_R3+Bb_R3).GetMass();
    phitt = Ta_R3.GetFourVector().Phi() - Tb_R3.GetFourVector().Phi(); 

    if(Mta > Mtb){
      MtH = Mta;
      MtL = Mtb;
      MDt = Mta - Mtb;
    }
    else{
      MtH = Mtb;
      MtL = Mta;
      MDt = Mtb - Mta;
    }
    
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);
    topInTTbar.SetPxPyPzE(0, 0, 0, 0);
    antitopInTTbar.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM.SetXYZ(0, 0, 0);
    boostToTopRest.SetXYZ(0, 0, 0);
    boostToAntitopRest.SetXYZ(0, 0, 0);

    // Initialize TLorentzVector to zero
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM = -(Ta_R3.GetFourVector() + Tb_R3.GetFourVector()).BoostVector();
    // Step 2: Boost top and antitop 4-vectors into ttbar CM frame
    topInTTbar = Ta_R3.GetFourVector();
    topInTTbar.Boost(boostToTTbarCM);

    antitopInTTbar = Tb_R3.GetFourVector();
    antitopInTTbar.Boost(boostToTTbarCM);

    // Step 3: Calculate boost vectors to go from ttbar CM frame to each top rest frame
    // (negative because BoostVector() gives boost from rest to lab frame)
    boostToTopRest = -topInTTbar.BoostVector();
    boostToAntitopRest = -antitopInTTbar.BoostVector();

    // Step 4: Boost leptons into the ttbar CM frame first
    lep_boosteda = La_R3.GetFourVector();
    lep_boosteda.Boost(boostToTTbarCM);

    lep_boostedb = Lb_R3.GetFourVector();
    lep_boostedb.Boost(boostToTTbarCM);

    // Step 5: Boost leptons from ttbar CM frame into their respective parent top rest frames
    // Assuming lep1 comes from antitop and lep2 from top (or adjust accordingly)
    lep_boosteda.Boost(boostToTopRest);
    lep_boostedb.Boost(boostToAntitopRest);

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());
    
    chel = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

    chan = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

  
    boostVec.SetXYZ(0, 0, 0);
    boostVeca.SetXYZ(0, 0, 0);
    boostVecb.SetXYZ(0, 0, 0);

    boostVec = -(Ta_R3.GetFourVector() + Tb_R3.GetFourVector()).BoostVector();
    lep_boosteda = La_R3.GetFourVector();
    lep_boostedb = Lb_R3.GetFourVector();
    boostVeca = Ta_R3.GetFourVector().BoostVector();
    boostVecb = Tb_R3.GetFourVector().BoostVector();
    lep_boosteda.Boost(boostVec);
    lep_boostedb.Boost(boostVec);
    lep_boosteda.Boost(boostVeca);
    lep_boostedb.Boost(boostVecb);

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());

    nchel = leptopa.Dot(leptopb)/(leptopa.Mag()*leptopb.Mag());

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

// Δφ bin 1: [-6, -2]
// nchel bin 1: [-1, -0.4]
if (phitt > -6. && phitt < -2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_1_1 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -6. && phitt < -2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_1_2 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -6. && phitt < -2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_1_3 = (Ta_R3+Tb_R3).GetMass();
}

// Δφ bin 2: [-2, 2]
// nchel bin 1: [-1, -0.4]
if (phitt > -2. && phitt < 2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_2_1 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -2. && phitt < 2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_2_2 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -2. && phitt < 2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_2_3 = (Ta_R3+Tb_R3).GetMass();
}

// Δφ bin 3: [2, 6]
// nchel bin 1: [-1, -0.4]
if (phitt > 2. && phitt < 6. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_3_1 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > 2. && phitt < 6. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_3_2 = (Ta_R3+Tb_R3).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > 2. && phitt < 6. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_3_3 = (Ta_R3+Tb_R3).GetMass();
}

    Eb_ta = Ba_R3.GetFourVector(Ta_R3).E();
    Eb_tb = Bb_R3.GetFourVector(Tb_R3).E();
    El_Wa = La_R3.GetFourVector(Wa_R3).E();
    El_Wb = Lb_R3.GetFourVector(Wb_R3).E();
    costt = TT_R3.GetCosDecayAngle();
    costa = Ta_R3.GetCosDecayAngle();
    costb = Tb_R3.GetCosDecayAngle();
    cosWa = Wa_R3.GetCosDecayAngle();
    cosWb = Wb_R3.GetCosDecayAngle();
    Dcostt = asin(sqrt(1.-costt*costt)*costtgen-sqrt(1.-costtgen*costtgen)*costt);
    Dcosta = asin(sqrt(1.-costa*costa)*costagen-sqrt(1.-costagen*costagen)*costa);
    Dcostb = asin(sqrt(1.-costb*costb)*costbgen-sqrt(1.-costbgen*costbgen)*costb);
    DcosWa = asin(sqrt(1.-cosWa*cosWa)*cosWagen-sqrt(1.-cosWagen*cosWagen)*cosWa);
    DcosWb = asin(sqrt(1.-cosWb*cosWb)*cosWbgen-sqrt(1.-cosWbgen*cosWbgen)*cosWb);

    phill = La_R3.GetFourVector().Phi() - Lb_R3.GetFourVector().Phi();

    histPlot->Fill(cat_R3);

    Meta = TT_R4.GetMass()/mTopo;
    Mtt = TT_R4.GetMass();
    Mta = Ta_R4.GetMass();
    Mtb = Tb_R4.GetMass();
    MWa = Wa_R4.GetMass();
    MWb = Wb_R4.GetMass();
    Mll = (La_R4+Lb_R4).GetMass();
    Mllbb = (La_R4+Lb_R4+Ba_R4+Bb_R4).GetMass();
    phitt = Ta_R4.GetFourVector().Phi() - Tb_R4.GetFourVector().Phi(); 


    if(Mta > Mtb){
      MtH = Mta;
      MtL = Mtb;
      MDt = Mta - Mtb;
    }
    else{
      MtH = Mtb;
      MtL = Mta;
      MDt = Mtb - Mta;
    }
    
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);
    topInTTbar.SetPxPyPzE(0, 0, 0, 0);
    antitopInTTbar.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM.SetXYZ(0, 0, 0);
    boostToTopRest.SetXYZ(0, 0, 0);
    boostToAntitopRest.SetXYZ(0, 0, 0);

    // Initialize TLorentzVector to zero
    lep_boosteda.SetPxPyPzE(0, 0, 0, 0);
    lep_boostedb.SetPxPyPzE(0, 0, 0, 0);

    boostToTTbarCM = -(Ta_R4.GetFourVector() + Tb_R4.GetFourVector()).BoostVector();
    // Step 2: Boost top and antitop 4-vectors into ttbar CM frame
    topInTTbar = Ta_R4.GetFourVector();
    topInTTbar.Boost(boostToTTbarCM);

    antitopInTTbar = Tb_R4.GetFourVector();
    antitopInTTbar.Boost(boostToTTbarCM);

    // Step 3: Calculate boost vectors to go from ttbar CM frame to each top rest frame
    // (negative because BoostVector() gives boost from rest to lab frame)
    boostToTopRest = -topInTTbar.BoostVector();
    boostToAntitopRest = -antitopInTTbar.BoostVector();

    // Step 4: Boost leptons into the ttbar CM frame first
    lep_boosteda = La_R4.GetFourVector();
    lep_boosteda.Boost(boostToTTbarCM);

    lep_boostedb = Lb_R4.GetFourVector();
    lep_boostedb.Boost(boostToTTbarCM);

    // Step 5: Boost leptons from ttbar CM frame into their respective parent top rest frames
    // Assuming lep1 comes from antitop and lep2 from top (or adjust accordingly)
    lep_boosteda.Boost(boostToTopRest);
    lep_boostedb.Boost(boostToAntitopRest);

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());
    
    chel = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

    la_ta.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    lb_tb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

    chan = la_ta.Dot(lb_tb)/(la_ta.Mag()*lb_tb.Mag());

  
    boostVec.SetXYZ(0, 0, 0);
    boostVeca.SetXYZ(0, 0, 0);
    boostVecb.SetXYZ(0, 0, 0);

    boostVec = -(Ta_R4.GetFourVector() + Tb_R4.GetFourVector()).BoostVector();
    lep_boosteda = La_R4.GetFourVector();
    lep_boostedb = Lb_R4.GetFourVector();
    boostVeca = Ta_R4.GetFourVector().BoostVector();
    boostVecb = Tb_R4.GetFourVector().BoostVector();
    lep_boosteda.Boost(boostVec);
    lep_boostedb.Boost(boostVec);
    lep_boosteda.Boost(boostVeca);
    lep_boostedb.Boost(boostVecb);

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),lep_boostedb.Pz());

    nchel = leptopa.Dot(leptopb)/(leptopa.Mag()*leptopb.Mag());

    leptopa.SetXYZ(lep_boosteda.Px(),lep_boosteda.Py(),lep_boosteda.Pz());
    leptopb.SetXYZ(lep_boostedb.Px(),lep_boostedb.Py(),-lep_boostedb.Pz());

// Δφ bin 1: [-6, -2]
// nchel bin 1: [-1, -0.4]
if (phitt > -6. && phitt < -2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_1_1 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -6. && phitt < -2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_1_2 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -6. && phitt < -2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_1_3 = (Ta_R4+Tb_R4).GetMass();
}

// Δφ bin 2: [-2, 2]
// nchel bin 1: [-1, -0.4]
if (phitt > -2. && phitt < 2. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_2_1 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > -2. && phitt < 2. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_2_2 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > -2. && phitt < 2. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_2_3 = (Ta_R4+Tb_R4).GetMass();
}

// Δφ bin 3: [2, 6]
// nchel bin 1: [-1, -0.4]
if (phitt > 2. && phitt < 6. && nchel > -1. && nchel < -0.4) {
    hist_mttbar_3_1 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 2: [-0.4, 0.4]
if (phitt > 2. && phitt < 6. && nchel > -0.4 && nchel < 0.4) {
    hist_mttbar_3_2 = (Ta_R4+Tb_R4).GetMass();
}
// nchel bin 3: [0.4, 1]
if (phitt > 2. && phitt < 6. && nchel > 0.4 && nchel < 1.) {
    hist_mttbar_3_3 = (Ta_R4+Tb_R4).GetMass();
}

    Eb_ta = Ba_R4.GetFourVector(Ta_R4).E();
    Eb_tb = Bb_R4.GetFourVector(Tb_R4).E();
    El_Wa = La_R4.GetFourVector(Wa_R4).E();
    El_Wb = Lb_R4.GetFourVector(Wb_R4).E();
    costt = TT_R4.GetCosDecayAngle();
    costa = Ta_R4.GetCosDecayAngle();
    costb = Tb_R4.GetCosDecayAngle();
    cosWa = Wa_R4.GetCosDecayAngle();
    cosWb = Wb_R4.GetCosDecayAngle();
    Dcostt = asin(sqrt(1.-costt*costt)*costtgen-sqrt(1.-costtgen*costtgen)*costt);
    Dcosta = asin(sqrt(1.-costa*costa)*costagen-sqrt(1.-costagen*costagen)*costa);
    Dcostb = asin(sqrt(1.-costb*costb)*costbgen-sqrt(1.-costbgen*costbgen)*costb);
    DcosWa = asin(sqrt(1.-cosWa*cosWa)*cosWagen-sqrt(1.-cosWagen*cosWagen)*cosWa);
    DcosWb = asin(sqrt(1.-cosWb*cosWb)*cosWbgen-sqrt(1.-cosWbgen*cosWbgen)*cosWb);

    phill = La_R4.GetFourVector().Phi() - Lb_R4.GetFourVector().Phi();

    histPlot->Fill(cat_R4);
  
    //cout << TT_R1.GetMass()/mTopo << " " << TT_R2.GetMass()/mTopo << " " << TT_R3.GetMass()/mTopo << " " << TT_R4.GetMass()/mTopo << " " << endl; 
    }//event loop

    histPlot->Draw();
    LAB_Gen.PrintGeneratorEfficiency();
  
    TFile fout(output_name.c_str(),"RECREATE");
    fout.Close();
    histPlot->WriteOutput(output_name);
    histPlot->WriteHist(output_name);
    treePlot->WriteOutput(output_name);
    g_Log << LogInfo << "Finished" << LogEnd;

    TFile f2(("hist_"+output_name).c_str(),"RECREATE");
    hist_toponium_mass->Write();
    hist_top_mass->Write();
    hist_antitop_mass->Write();
    hist_dilep_mass->Write();
    hist_dilep_angle->Write();

//     if(sample=="NOtoponium"){
//       cout << " Toponium" << endl;

// histNmttbar_1_1->Scale(1/133.);
// histNmttbar_1_2->Scale(1/133.);
// histNmttbar_1_3->Scale(1/133.);

// histNmttbar_2_1->Scale(1/133.);
// histNmttbar_2_2->Scale(1/133.);
// histNmttbar_2_3->Scale(1/133.);

// histNmttbar_3_1->Scale(1/133.);
// histNmttbar_3_2->Scale(1/133.);
// histNmttbar_3_3->Scale(1/133.);
//     }

histNmttbar_1_1->Write();
histNmttbar_1_2->Write();
histNmttbar_1_3->Write();

histNmttbar_2_1->Write();
histNmttbar_2_2->Write();
histNmttbar_2_3->Write();

histNmttbar_3_1->Write();
histNmttbar_3_2->Write();
histNmttbar_3_3->Write();


    f2.Close();

    // TCanvas *c1= new TCanvas("","",800,600);
    // hist_toponium_mass->Draw("hist");
    // c1->SaveAs((sample+"_ditop_mass.pdf").c_str(),"PDF");

    // TCanvas *c2= new TCanvas("","",800,600);
    // hist_top_mass->Draw("hist");
    // c2->SaveAs((sample+"_topmass.pdf").c_str(),"PDF");

    // TCanvas *c3= new TCanvas("","",800,600);
    // hist_antitop_mass->Draw("hist");
    // c3->SaveAs((sample+"_antitop_mass.pdf").c_str(),"PDF");

  }

# ifndef __CINT__ // main function for stand-alone compilation
int toponium(){
  run("toponium");
  run("ttbar");
  return 0;
}
#endif