import ROOT
from style import MyStyle

ROOT.gStyle.SetOptStat(0)

# Open the ROOT file
sample = "ttbar"
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
extracted2d = [name.split("_Reco")[0] for name in listobjects2d]


extracted1d = list(dict.fromkeys(extracted1d))
extracted2d = list(dict.fromkeys(extracted2d))
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

    print(histA.GetYaxis().GetTitle())

    histA.GetXaxis().SetTitle(histA.GetXaxis().GetTitle())
    histA.GetYaxis().SetTitle(histA.GetYaxis().GetTitle())

    histA.SetMaximum(1.8*histA.GetMaximum())

    c = ROOT.TCanvas()#"","",800,600)
    c.SetLeftMargin(0.2)
    c.SetBottomMargin(0.2)

    histA.Draw("hist")
    histB.Draw("histsame")
    histC.Draw("histsame")
    histD.Draw("histsame")
    legend.Draw()

    c.Update()
    c.SaveAs("ttbar/"+obj+"_"+sample+".pdf")
"""

for key in folder.GetListOfKeys():
    obj = key.ReadObj()
    print(obj.GetName())
    #obj.SaveAs()
    hist = obj.Clone()
    #hist.SetDirectory(0)
    c = ROOT.TCanvas("canvas","",800,600)
    c.cd()
    hist.Draw()
    c.Update()
    c.SaveAs("ttbar/" + hist.GetName()+".pdf","pdf")
"""
