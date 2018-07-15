//
// nu-vs-antinu
// Copyright (C) 2018 by Pintaudi Giorgio <giorgio-pintaudi-kx@ynu.jp>
// Released under the GPLv3 license
//
// This file is part of nu-vs-antinu.
// nu-vs-antinu is a simple program that produces a graph to quickly estimate
// the sensibility of a given experiment to the neutrino mass hierarchy.
//

{

  gStyle->SetLineStyleString(5,"[]");
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleSize(0.04);
  gStyle->SetLabelSize(0.04);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  

  TFile * Bob = _file0;
  Bob->cd();

  // Raw Probability Plots	

  TH1F *_NuMuToNuE_Energy 		= (TH1F *) Bob->Get("lmu2eE");
  TH1F *_NuMuToNuE_Length	 	= (TH1F *) Bob->Get("lmu2eL");
  TH1F *_NuMuToNuMu_Energy 		= (TH1F *) Bob->Get("lmu2muE");
  TH1F *_NuMuToNuMu_Length 		= (TH1F *) Bob->Get("lmu2muL");
  TH1F *_NuMuToNuTau_Energy 		= (TH1F *) Bob->Get("lmu2tauE");
  TH1F *_NuMuToNuTau_Length 		= (TH1F *) Bob->Get("lmu2tauL");
  
  TCanvas *c4 = new TCanvas("c4","",5,5,800,800);
  // c4->SetLeftMargin(0.153226);
  // c4->SetRightMargin(0.153226);
  // c4->SetBottomMargin(0.153226);
  // c4->SetTopMargin(0.153226);
  // c4->SetRightMargin(0.153226);
  c4->SetFillColor(kWhite);
  c4->SetLogx(1);
	
  _NuMuToNuE_Energy->GetXaxis()->SetTitleSize(0.03);
  _NuMuToNuE_Energy->GetXaxis()->SetTitleOffset(1.3);
  _NuMuToNuE_Energy->GetYaxis()->SetTitleSize(0.03);
  // _NuMuToNuE_Energy->GetYaxis()->SetRangeUser(-1.00, 1.00);
  _NuMuToNuE_Energy->GetYaxis()->SetTitleOffset(1.3);
  // _NuMuToNuE_Energy->SetTitle("Prob 3 Flavor NuMu->NuE");
  _NuMuToNuE_Energy->GetXaxis()->SetTitle("Energy [GeV]");
  _NuMuToNuE_Energy->GetYaxis()->SetTitle("Oscillation Probability");
  _NuMuToNuE_Energy->Draw();

    TCanvas *c5 = new TCanvas("c5","",5,5,800,800);
  // c5->SetLeftMargin(0.153226);
  // c5->SetRightMargin(0.153226);
  // c5->SetBottomMargin(0.153226);
  // c5->SetTopMargin(0.153226);
  // c5->SetRightMargin(0.153226);
  c5->SetFillColor(kWhite);
  //c5->SetLogx(1);
	
  _NuMuToNuE_Length->GetXaxis()->SetTitleSize(0.03);
  _NuMuToNuE_Length->GetXaxis()->SetTitleOffset(1.3);
  _NuMuToNuE_Length->GetYaxis()->SetTitleSize(0.03);
  // _NuMuToNuE_Length->GetYaxis()->SetRangeUser(-1.00, 1.00);
  _NuMuToNuE_Length->GetYaxis()->SetTitleOffset(1.3);
  // _NuMuToNuE_Length->SetTitle("Prob 3 Flavor NuMu->NuE");
  _NuMuToNuE_Length->GetXaxis()->SetTitle("Length [Km]");
  _NuMuToNuE_Length->GetYaxis()->SetTitle("Oscillation Probability");
  _NuMuToNuE_Length->Draw();
  
 
  // Bob->Close();
   
  //------------------- End of Raw Probability Plots	

}

//  Copyright (C) 2018  Pintaudi Giorgio
//
//    This file is part of nu-vs-antinu.
//    nu-vs-antinu is a simple program that produces a graph to quickly estimate
//    the sensibility of a given experiment to the neutrino mass hierarchy.
//
//    nu-vs-antinu is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    nu-vs-antinu is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with nu-vs-antinu.  If not, see <https://www.gnu.org/licenses/>.
