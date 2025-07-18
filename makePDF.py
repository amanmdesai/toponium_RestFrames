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
    file1 = ROOT.TFile.Open("hist_toponium_reco.root")
    file2 = ROOT.TFile.Open("hist_ttbar_reco.root")
    
    for var in vars:
        h1 = file1.Get(var)
        h2 = file2.Get(var)
        h1.SetLineColor(ROOT.kBlue)
        h2.SetLineColor(ROOT.kRed)
        h1.SetLineWidth(2)
        h2.SetLineWidth(2)
        h1.SetTitle("")

        h1.GetXaxis().SetTitleFont(42)
        h1.GetXaxis().SetTitleSize(0.05)
        h1.GetXaxis().SetTitleOffset(1.0)
        h1.GetYaxis().SetTitleFont(42)
        h1.GetYaxis().SetTitleSize(0.05)
        h1.GetYaxis().SetTitleOffset(1.2)

        print(vars[var])
        h1.GetXaxis().SetTitle(vars[var])
        h1.GetYaxis().SetTitle("Events")
        legend = ROOT.TLegend(0.72, 0.75, 0.9, 0.9)
        legend.AddEntry(h1, "toponium","L")
        legend.AddEntry(h2, "t\\bar{t}","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        legend.SetTextFont(42)
        legend.SetTextSize(0.05)

        c = ROOT.TCanvas()
        h1.Draw("hist")
        h2.Draw("histsame")
        h1.SetMaximum(1.2*max(h1.GetMaximum(),h2.GetMaximum()))
        legend.Draw()
        c.SetLeftMargin(0.2)
        c.SetBottomMargin(0.2)

        c.SaveAs("plots/"+var+"_lin.png")
        c.SaveAs("plots/"+var+"_lin.pdf")


def Plot(sample):
    # Open the ROOT file
    file = ROOT.TFile.Open(sample+"_reco.root")

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

        legend = ROOT.TLegend(0.65, 0.7, 0.9, 0.9)
        legend.AddEntry(histA, "M_{top}^{ a} = M_{top}^{ b} Reco","L")
        legend.AddEntry(histB, "M_{W}^{ a} = M_{W}^{ b} Reco","L")
        legend.AddEntry(histC, "min #Sigma M_{top}^{ 2} Reco","L")
        legend.AddEntry(histD, "min #Delta M_{top} Reco","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        if(histA.GetYaxis().GetTitle()==""):
            histA.GetYaxis().SetTitle("Events")
        if(histA.GetXaxis().GetTitle()==""):
            histA.GetXaxis().SetTitle(obj)
            
        histA.GetXaxis().SetTitle(histA.GetXaxis().GetTitle())
        histA.GetYaxis().SetTitle(histA.GetYaxis().GetTitle())

        histA.SetMaximum(1.4*max(histA.GetMaximum(), histB.GetMaximum(), histC.GetMaximum(), histD.GetMaximum()))

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
