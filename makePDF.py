import ROOT
from style import MyStyle

ROOT.gStyle.SetOptStat(0)


def hist():
    # Open the ROOT file
    vars = {
        "mass" : "M_{t\\bar{t}} GeV",
        "heavy_top" : "M_{t_{H}} GeV",
        "light_top" : "M_{t_{L}} GeV",
        "hist_dilep_mass" : "M_{ll'} GeV",
        "hist_dilep_angle" : "\\Delta \\phi_{ll'}",
    }
    #vars = ["mass", "heavy_top", "light_top", "hist_dilep_mass", "hist_dilep_angle"]
    file1 = ROOT.TFile.Open("results/hist_toponium_reco.root")
    file2 = ROOT.TFile.Open("results/hist_ttbar_reco.root")
    
    for var in vars:
        h1 = file1.Get(var)
        h2 = file2.Get(var)
        h1.SetLineColor(ROOT.kBlue)
        h2.SetLineColor(ROOT.kRed)
        print(vars[var])
        h1.GetXaxis().SetTitle(vars[var])
        h1.GetYaxis().SetTitle("Events")
        legend = ROOT.TLegend(0.8, 0.75, 0.9, 0.9)
        legend.AddEntry(h1, "\\eta_{t}","L")
        legend.AddEntry(h2, "t\\bar{t}","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        h1.SetMaximum(10*h1.GetMaximum())

        c = ROOT.TCanvas()
        h1.Draw("hist")
        h2.Draw("histsame")
        legend.Draw()
        c.SetLogy()
        c.SetLeftMargin(0.2)
        c.SetBottomMargin(0.2)

        c.SaveAs("ttbar/"+var+".pdf")


def Plot(sample):
    # Open the ROOT file
    file = ROOT.TFile.Open("results/"+sample+"_reco.root")

    # Get the folder inside the file
    tree = file.Get("HistPlot/hist")

    listobjects1d = []
    listobjects2d = []

    for key in tree.GetListOfKeys():
        obj = key.ReadObj()
        name = obj.GetName()
        if '_v_' in name and name not in listobjects2d:
            listobjects2d.append(name)
        else:
            if name not in listobjects1d:
                listobjects1d.append(name)

    extracted1d = [name.split("_Reco")[0] for name in listobjects1d]
    #extracted2d = [name.split("_Reco")[0] for name in listobjects2d]

    extracted1d = list(dict.fromkeys(extracted1d))
    extracted2d = list(dict.fromkeys(listobjects2d))
    print(extracted1d)



    for obj in extracted1d:
        print(obj)
        histA = tree.Get(obj + "_Reco1_HistPlot")
        histB = tree.Get(obj + "_Reco2_HistPlot")
        histC = tree.Get(obj + "_Reco3_HistPlot")
        histD = tree.Get(obj + "_Reco4_HistPlot")


        histA.SetLineColor(ROOT.kBlue)
        histB.SetLineColor(ROOT.kGreen)
        histC.SetLineColor(ROOT.kRed)
        histD.SetLineColor(ROOT.kYellow+2)

        histA.SetFillColor(ROOT.kBlue)
        histB.SetFillColor(ROOT.kGreen)
        histC.SetFillColor(ROOT.kRed)
        histD.SetFillColor(ROOT.kYellow+2)

        legend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
        legend.AddEntry(histA, "Reco1","L")
        legend.AddEntry(histB, "Reco2","L")
        legend.AddEntry(histC, "Reco3","L")
        legend.AddEntry(histD, "Reco4","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        if(histA.GetYaxis().GetTitle()==""):
            histA.GetYaxis().SetTitle("Events")
        if(histA.GetXaxis().GetTitle()==""):
            histA.GetXaxis().SetTitle(obj)
            
        histA.GetXaxis().SetTitle(histA.GetXaxis().GetTitle())
        histA.GetYaxis().SetTitle(histA.GetYaxis().GetTitle())

        histA.SetMaximum(1.2*max(histA.GetMaximum(), histB.GetMaximum(), histC.GetMaximum(), histD.GetMaximum()))

        c = ROOT.TCanvas()#"","",800,600)
        c.SetLeftMargin(0.2)
        c.SetBottomMargin(0.2)

        histA.Draw("hist")
        histB.Draw("histsame")
        histC.Draw("histsame")
        histD.Draw("histsame")
        legend.Draw()


        title_text = ROOT.TLatex()
        title_text.SetNDC()
        title_text.SetTextSize(0.05)
        title_text.DrawLatex(0.25, 0.82, sample)


        c.Update()
        c.SaveAs("ttbar/"+obj+"_"+sample+".pdf")


    for obj in extracted2d:
        print(obj)
        histA = tree.Get(obj)

        c = ROOT.TCanvas()#"","",800,600)
        c.SetLeftMargin(0.2)
        c.SetRightMargin(0.3)
        c.SetBottomMargin(0.2)
        histA.Draw("COLZ")


        title_text = ROOT.TLatex()
        title_text.SetNDC()
        title_text.SetTextSize(0.05)
        title_text.DrawLatex(0.25, 0.82, sample)

        c.SaveAs("ttbar/"+obj+"_"+sample+".pdf")


hist()
Plot("ttbar")
Plot("toponium")
