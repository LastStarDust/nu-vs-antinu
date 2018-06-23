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
  TFile * Bob = _file0;
  Bob->cd();

  //----------------------- Raw Probability Plots -----------------------//	

  TGraph * LO_NH    	= (TGraph *) Bob->Get("LO_NH");
  TGraph * UO_NH    	= (TGraph *) Bob->Get("UO_NH");
  TGraph * LO_IH    	= (TGraph *) Bob->Get("LO_IH");
  TGraph * UO_IH    	= (TGraph *) Bob->Get("UO_IH");
  TGraph * NH_0    	= (TGraph *) Bob->Get("NH_0");
  TGraph * NH_1    	= (TGraph *) Bob->Get("NH_1");
  TGraph * NH_2    	= (TGraph *) Bob->Get("NH_2");
  TGraph * NH_3    	= (TGraph *) Bob->Get("NH_3");
  TGraph * IH_0    	= (TGraph *) Bob->Get("IH_0");
  TGraph * IH_1    	= (TGraph *) Bob->Get("IH_1");
  TGraph * IH_2    	= (TGraph *) Bob->Get("IH_2");
  TGraph * IH_3    	= (TGraph *) Bob->Get("IH_3");
  //TGraph empty_circle = new TGraph();
  //TGraph filled_circle = new TGraph();
  //TGraph empty_square = new TGraph();
  //TGraph filled_square = new TGraph();
  
  TCanvas *c4 = new TCanvas("c4","",5,5,800,800);

  gPad->SetTicks(1, 1);
  
  LO_NH->GetYaxis()->SetRangeUser(0., 0.07);
  auto axis = LO_NH->GetXaxis();
  axis->SetLimits(0., 0.07); // along X
 
  LO_NH->GetXaxis()->SetTitleSize(0.03);
  LO_NH->GetXaxis()->SetTitleOffset(1.3);
  LO_NH->GetYaxis()->SetTitleSize(0.03);
  LO_NH->GetYaxis()->SetTitleOffset(1.7);
  LO_NH->SetTitle("T2K FD");
  LO_NH->GetXaxis()->SetTitle("Neutrino Oscillation Probability");
  LO_NH->GetYaxis()->SetTitle("Anti-neutrino Oscillation Probability");

  LO_NH->SetLineColor(38);
  LO_NH->SetLineWidth(2);
  LO_NH->Draw("AC");

  UO_NH->SetLineColor(38);
  UO_NH->SetLineWidth(2);
  UO_NH->Draw("C");
   
  LO_IH->SetLineColor(2);
  LO_IH->SetLineWidth(2);
  LO_IH->Draw("C");
   
  UO_IH->SetLineColor(2);
  UO_IH->SetLineWidth(2);
  UO_IH->Draw("C");
  
  NH_0->SetMarkerSize(1.5);
  NH_0->SetMarkerColor(38);
  NH_0->SetMarkerStyle(24);
  NH_0->Draw("P");

  NH_1->SetMarkerSize(1.5);
  NH_1->SetMarkerColor(38);
  NH_1->SetMarkerStyle(20);
  NH_1->Draw("P");

  NH_2->SetMarkerSize(1.5);
  NH_2->SetMarkerColor(38);
  NH_2->SetMarkerStyle(25);
  NH_2->Draw("P");

  NH_3->SetMarkerSize(1.5);
  NH_3->SetMarkerColor(38);
  NH_3->SetMarkerStyle(21);
  NH_3->Draw("P");

  IH_0->SetMarkerSize(1.5);
  IH_0->SetMarkerColor(2);
  IH_0->SetMarkerStyle(24);
  IH_0->Draw("P");

  IH_1->SetMarkerSize(1.5);
  IH_1->SetMarkerColor(2);
  IH_1->SetMarkerStyle(20);
  IH_1->Draw("P");

  IH_2->SetMarkerSize(1.5);
  IH_2->SetMarkerColor(2);
  IH_2->SetMarkerStyle(25);
  IH_2->Draw("P");

  IH_3->SetMarkerSize(1.5);
  IH_3->SetMarkerColor(2);
  IH_3->SetMarkerStyle(21);
  IH_3->Draw("P");

  //  empty_circle.SetMarkerSize(1.5);
  //  empty_circle.

  gStyle->SetLegendBorderSize(0);
  gStyle->SetPalette(1);

  
  auto legend = new TLegend(0.1,0.1,0.5,0.3);
  legend->SetFillStyle(0);
  legend->SetNColumns(2);
   
  legend->AddEntry(NH_0,"#delta_{CP} = 0","p");
  legend->AddEntry(NH_1,"#delta_{CP} = #pi/2","p");
  legend->AddEntry(NH_2,"#delta_{CP} = #pi","p");
  legend->AddEntry(NH_3,"#delta_{CP} = 3#pi/2","p");
  legend->Draw();
  
  TLatex latex;
  latex.SetTextAlign(22);
  latex.SetTextSize(0.035);
  latex.DrawLatex(.056,.064,"sin^{2}2#theta_{13}=0.082");

  latex.DrawLatex(.056,.059,"UO : sin^{2}2#theta_{23}=0.59");

  latex.DrawLatex(.016,.036,"LO : sin^{2}2#theta_{23}=0.46");

  latex.SetTextColor(2);
  latex.DrawLatex(.018,.028,"IH : #Deltam^{2}_{32}=-2.55#times10^{-3} eV^{2}");

  latex.SetTextColor(38);
  latex.DrawLatex(.051,.019,"NH : #Deltam^{2}_{32}=2.50#times10^{-3} eV^{2}");
   
  Bob->Close();
   
  //------------------- End of Raw Probability Plots -----------------------//	

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

