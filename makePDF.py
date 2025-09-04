import ROOT
from style import MyStyle

ROOT.gStyle.SetOptStat(0)


def hist(method="Reco1"):
    MyStyle()
    # Open the ROOT file
    vars = {
        "mass" : "M_{t\\bar{t}} GeV",
        "heavy_top" : "M_{t_{H}} GeV",
        "light_top" : "M_{t_{L}} GeV",
        "hist_dilep_mass" : "M_{ll'} GeV",
        "hist_dilep_angle" : "\\Delta \\phi_{ll'}",

        "histNmttbar_1_1" : "M_{t\\bar{t}} GeV",
        "histNmttbar_1_2" : "M_{t\\bar{t}} GeV",
        "histNmttbar_1_3" : "M_{t\\bar{t}} GeV",

        "histNmttbar_2_1" : "M_{t\\bar{t}} GeV",
        "histNmttbar_2_2" : "M_{t\\bar{t}} GeV",
        "histNmttbar_2_3" : "M_{t\\bar{t}} GeV",

        "histNmttbar_3_1" : "M_{t\\bar{t}} GeV",
        "histNmttbar_3_2" : "M_{t\\bar{t}} GeV",
        "histNmttbar_3_3" : "M_{t\\bar{t}} GeV",
    }
    #vars = ["mass", "heavy_top", "light_top", "hist_dilep_mass", "hist_dilep_angle"]
    file1 = ROOT.TFile.Open("hist_toponium_reco.root")
    file2 = ROOT.TFile.Open("hist_ttbar_reco.root")

    if method == "Reco1":
        recomethod = "M_{top}^{ a} = M_{top}^{ b}"
    if method == "Reco2":
        recomethod = "M_{W}^{ a} = M_{W}^{ b}"
    if method == "Reco3":
        recomethod = "min #Sigma M_{top}^{ 2}"
    if method == "Reco4":
        recomethod = "min #Delta M_{top}"


    for var in vars:
        h1 = file1.Get(var)
        h2 = file2.Get(var)

        c = ROOT.TCanvas()
        text1 = ""
        text2 = ""

        if "histNmttbar" in var: 
            deltaphi_bins = [(-6, -2), (-2, 2), (2, 6)]
            nchel_bins    = [(-1, -0.4), (-0.4, 0.4), (0.4, 1)]
            parts = var.split("_")
            i = int(parts[1]) - 1  # Δφ bin index (0-based)
            j = int(parts[2]) - 1  # Nchel bin index (0-based)
            
            # Build text from bin ranges
            dp_low, dp_high = deltaphi_bins[i]
            nc_low, nc_high = nchel_bins[j]
            text1 = f"{dp_low} < \\Delta\\Phi (tt) < {dp_high}"
            text2 = f"{nc_low} < Nchel < {nc_high}"


        h1.SetLineColor(ROOT.kBlue)
        h2.SetLineColor(ROOT.kRed)

        h1.SetFillColor(ROOT.kBlue-1)
        h2.SetFillColor(ROOT.kRed-1)

        h1.SetFillStyle(3002)
        h2.SetFillStyle(3002)

        h1.SetTitle("Reco Method: " +recomethod)

        h1.GetXaxis().SetTitleFont(42)
        h1.GetXaxis().SetTitleSize(0.05)
        h1.GetXaxis().SetTitleOffset(1.0)
        h1.GetYaxis().SetTitleFont(42)
        h1.GetYaxis().SetTitleSize(0.05)
        h1.GetYaxis().SetTitleOffset(1.2)

        h1.GetXaxis().SetTitle(vars[var])
        h1.GetYaxis().SetTitle("Events")

        legend = ROOT.TLegend(0.72, 0.75, 0.9, 0.9)
        legend.AddEntry(h1, "Toponium","L")
        legend.AddEntry(h2, "t\\bar{t}","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        legend.SetTextFont(42)
        legend.SetTextSize(0.05)

        h1.SetMaximum(1.4*max(h1.GetMaximum(),h2.GetMaximum()))

        h1.Draw("hist")
        h2.Draw("histsame")

        legend.Draw()

        title_text1 = ROOT.TLatex()
        title_text1.SetNDC()
        title_text1.SetTextSize(0.05)
        title_text1.SetTextColor(1)
        title_text1.DrawLatex(0.25, 0.88, text1)
        title_text1.DrawLatex(0.25, 0.82, text2)

        c.SetLeftMargin(0.2)
        c.SetBottomMargin(0.2)
        c.Update()
        c.SaveAs("ttbar/"+var+"_lin.pdf")

        c.SetLogy()
        h1.SetMaximum(10*max(h1.GetMaximum(),h2.GetMaximum()))

        #c.SaveAs("ttbar/"+var+"_log.png")
        c.SaveAs("ttbar/"+var+"_log.pdf")


def hist_compare(method="Reco1"):

    file1 = ROOT.TFile.Open("toponium_reco.root")
    file2 = ROOT.TFile.Open("ttbar_reco.root")
    
    # Get the folder inside the file
    tree1 = file1.Get("HistPlot/hist")
    tree2 = file2.Get("HistPlot/hist")

    listobjects1d = []
    listobjects2d = []

    for key in tree1.GetListOfKeys():
        obj = key.ReadObj()
        name = obj.GetName()
        if '_v_' in name and name not in listobjects2d:
            listobjects2d.append(name)
        else:
            if name not in listobjects1d:
                listobjects1d.append(name)

    extracted1d = [name.split("_Reco")[0] for name in listobjects1d]

    extracted1d = list(dict.fromkeys(extracted1d))
    print(extracted1d)

    if method == "Reco1":
        recomethod = "M_{top}^{ a} = M_{top}^{ b}"
    if method == "Reco2":
        recomethod = "M_{W}^{ a} = M_{W}^{ b}"
    if method == "Reco3":
        recomethod = "min #Sigma M_{top}^{ 2}"
    if method == "Reco4":
        recomethod = "min #Delta M_{top}"

    for obj in extracted1d:
        print(obj)
        hist1 = tree1.Get(obj + "_" + method+"_HistPlot")
        hist2 = tree2.Get(obj + "_" + method+"_HistPlot")
        hist1.SetTitle("")
        hist2.SetTitle("")

        hist1.SetLineColor(ROOT.kBlue)
        hist2.SetLineColor(ROOT.kRed)

        hist1.SetFillColor(ROOT.kBlue-1)
        hist2.SetFillColor(ROOT.kRed-1)

        c = ROOT.TCanvas()#"","",800,600)
        c.SetLeftMargin(0.2)
        c.SetBottomMargin(0.2)
        text1 = ""
        text2 = ""

        if "hist_mttbar" in obj: 
            deltaphi_bins = [(-6, -2), (-2, 2), (2, 6)]
            nchel_bins    = [(-1, -0.4), (-0.4, 0.4), (0.4, 1)]
            parts = obj.split("_")
            i = int(parts[2]) - 1  # Δφ bin index (0-based)
            j = int(parts[3]) - 1  # Nchel bin index (0-based)
            
            # Build text from bin ranges
            dp_low, dp_high = deltaphi_bins[i]
            nc_low, nc_high = nchel_bins[j]
            text1 = f"{dp_low} < \\Delta\\Phi (tt) < {dp_high}"
            text2 = f"{nc_low} < Nchel < {nc_high}"
            hist1.SetMaximum(1.4*max(hist1.GetMaximum(), hist2.GetMaximum()))

        legend = ROOT.TLegend(0.65, 0.78, 0.9, 0.88)
        legend.AddEntry(hist1, "Toponium","L")
        legend.AddEntry(hist2, "t\\bar{t}","L")
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)

        if(hist1.GetYaxis().GetTitle()==""):
            hist1.GetYaxis().SetTitle("Events")
        if(hist1.GetXaxis().GetTitle()==""):
            hist1.GetXaxis().SetTitle(obj)
            
        hist1.GetXaxis().SetTitle(hist1.GetXaxis().GetTitle())
        hist1.GetYaxis().SetTitle(hist1.GetYaxis().GetTitle())

        hist1.SetMaximum(1.2*max(hist1.GetMaximum(), hist2.GetMaximum()))
                         
        hist1.Draw("hist")
        hist2.Draw("histsame")
        legend.Draw()

        title_text1 = ROOT.TLatex()
        title_text1.SetNDC()
        title_text1.SetTextSize(0.04)
        title_text1.SetTextColor(2)

        title_text1.DrawLatex(0.25, 0.78, text1)
        title_text1.DrawLatex(0.25, 0.74, text2)

        title_text = ROOT.TLatex()
        title_text.SetNDC()
        title_text.SetTextSize(0.04)
        title_text.DrawLatex(0.25, 0.84, "Reco Method: " +recomethod)
        c.Update()
        c.SaveAs("ttbar/"+obj+method+".pdf")



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
        text1 = ""
        text2 = ""

        if "hist_mttbar" in obj: 
            deltaphi_bins = [(-6, -2), (-2, 2), (2, 6)]
            nchel_bins    = [(-1, -0.4), (-0.4, 0.4), (0.4, 1)]
            parts = obj.split("_")
            i = int(parts[2]) - 1  # Δφ bin index (0-based)
            j = int(parts[3]) - 1  # Nchel bin index (0-based)
            
            # Build text from bin ranges
            dp_low, dp_high = deltaphi_bins[i]
            nc_low, nc_high = nchel_bins[j]
            text1 = f"{dp_low} < \\Delta\\Phi (tt) < {dp_high}"
            text2 = f"{nc_low} < Nchel < {nc_high}"

        histA.Draw("hist")
        histB.Draw("histsame")
        histC.Draw("histsame")
        histD.Draw("histsame")
        legend.Draw()

        title_text1 = ROOT.TLatex()
        title_text1.SetNDC()
        title_text1.SetTextSize(0.04)
        title_text1.SetTextColor(2)

        title_text1.DrawLatex(0.25, 0.80, text1)
        title_text1.DrawLatex(0.25, 0.76, text2)

        title_text = ROOT.TLatex()
        title_text.SetNDC()
        title_text.SetTextSize(0.05)
        title_text.DrawLatex(0.25, 0.84, sample)


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
        title_text.DrawLatex(0.02, 0.7, sample)

        c.SaveAs("ttbar/"+obj+"_"+sample+".pdf")


hist(method="Reco1")

#hist_compare("Reco1")
#hist_compare("Reco2")
#hist_compare("Reco4")
#Plot("ttbar")
#Plot("toponium")
