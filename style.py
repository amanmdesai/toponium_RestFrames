# Author: Aman Desai

import ROOT

def MyStyle():  # font  = 22,tsize = 17):
    MyStyle = ROOT.TStyle("MyStyle", "MyStyle")
    MyStyle.SetPaperSize(25,25)
    MyStyle.SetPadTopMargin(0.10)
    MyStyle.SetPadRightMargin(0.10)
    MyStyle.SetPadBottomMargin(0.10)
    MyStyle.SetPadLeftMargin(0.10)
    MyStyle.SetFrameBorderMode(0)
    MyStyle.SetFrameFillColor(0)
    MyStyle.SetPadColor(0)
    # MyStyle.SetTextFont(42)
    MyStyle.SetLabelSize(0.01, "X")
    MyStyle.SetTextSize(0.01)
    MyStyle.SetTitleXOffset(0)
    MyStyle.SetTitleYOffset(0)
    MyStyle.SetNdivisions(510, "x")
    MyStyle.SetNdivisions(505, "y")
    ROOT.gROOT.SetStyle("MyStyle")
    ROOT.gROOT.ForceStyle()