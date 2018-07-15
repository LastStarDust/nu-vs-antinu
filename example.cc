/*
 * nu-vs-antinu
 * Copyright (C) 2018 by Pintaudi Giorgio <giorgio-pintaudi-kx@ynu.jp>
 * Released under the GPLv3 license
 *
 * This file is part of nu-vs-antinu.
 * nu-vs-antinu is a simple program that produces a graph to quickly estimate
 * the sensibility of a given experiment to the neutrino mass hierarchy.
 */

// C includes
#include <math.h>

// C++ includes
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

// ROOT includes
#include "TFile.h"
#include "TH1D.h"

// Prob3++ includes
#include "/home/neo/Code/Prob3/BargerPropagator.h"

/* Boost library includes

   "po" is a shorthand for the program-options library's namespace.
   The boost library is a collection of C++ libraries that are useful to
   write C++ programs. They are like a swiss-nife for C++ programming.
   Program Options is a library included in the boost collection that
   facilitate the input and the formatting of arguments to be passed 
   to a program through the command line. 
   Boost library is also available for MacOS. */
#include <boost/program_options.hpp>
namespace po = boost::program_options;

/* These parameters are fixed and cannot be chaned interactively by the user.
   To modify them the program must be recompiled. All other parameters can be 
   passed to the program as run-time arguments. 
   The names are rather self-explanatory */
#define DELTA_M21 7.55e-5
#define THETA_12 0.320
#define THETA_13 0.021

using namespace std;

int main(int argc, char * argv[] )
{
  int i, j;
  
  /// Oscillation Parameters
  bool kSquared = true;   // Using sin^2(x) variables and not sin^2(2*x)

  int    mode = 1; // 1 for neutrino or -1 for anti-neutrino

  double theta23 = 0.46; // This is actually sin^2(42.7°) Lower Octant
  //  double theta23 = 0.59; // This is actually sin^2(50.2°) Upper Octant
  double theta13 = THETA_13; /* This is actually sin^2(8.32°),
				while sin^2(2*8.32°) = 0.082 */
  double theta12 = THETA_12; // This is actually sin^2(34.5°)
  
  double DM21    = DELTA_M21; // in eV
  double DM32 = 2.50e-3; // in eV

  double delta_degrees = 0; // T2K best fit
  double delta; // CP violation phase expressed in radiants

  /***** Density *****/
  // Continental crust desity. The unit of measure is grams over cubic centimeters
  double Density = 2.70; // Average density of continental crust is 2.7 g/cm^3
      
  try {

    /* Description of all the options that can be passed to our program.
       In fact, these options are just a subset of all the oscillation parameters.
       Given the scope of the program only the following parameters can be set
       by the user. The remaining parameters are fixed once-for-all by #define
       at compilation time. */
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("deltacp", po::value<double>(), "CP symmetry violation phase in degrees")
      ("deltam32",  po::value<double>(), "DeltaM^2_32 in meV (10^-3 eV)")
      ("theta23",   po::value<double>(), "Sin^2(theta23)")     
      ("beammode",  po::value<int>(), "\"1\": for neutrino mode or \"-1\""
       " for anti-neutrino mode")
      ;

    /* The following three lines of code, create the object "vm" that will contain
       the command line arguments.
       These are read through the method "parse_command_line" */
    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // The --help option produces the description of all the allowed options 
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 0;
    }

    /****************** Summary of all the oscillation parameters ******************/

    if (vm.count("deltacp")) {
      std::cout << "  The CP symmetry violation phase was set to " 
		<< vm["deltacp"].as<double>() << " degrees.\n";
      delta_degrees = vm["deltacp"].as<double>();
    } else {
      std::cout << "  CP symmetry violation phase is assumed to be " <<
	delta_degrees << " degrees.\n";
    }

    if (vm.count("deltam32")) {
      std::cout << "  The mass splitting M^2_3 - M^2_2 (DeltaM^2_32) was set to " 
		<< vm["deltam32"].as<double>() << " x 10^-3 eV.\n";
      DM32 = vm["deltam32"].as<double>() * 1e-3;
    } else {
      std::cout << "  The mass splitting M^2_3 - M^2_2 (DeltaM^2_32)"
	" is assumed to be " << DM32 << " eV.\n";
    }

    if (vm.count("theta23")) {
      std::cout << "  The Sin^2(theta23) oscillation angle was set to " 
		<< vm["theta23"].as<double>() << " .\n";
      theta23 = vm["theta23"].as<double>();
    } else {
      std::cout << "   The Sin^2(theta23) oscillation angle"
	" is assumed to be " << theta23 << " .\n";
    }

    if (vm.count("beammode")) {
      if (vm["beammode"].as<int>() == 1) {
	std::cout << "  Beam mode was set to neutrino mode"
	  " (Negative horn current)\n";
	mode = 1;
      }
      else {
	std::cout << "  Beam mode was set to anti-neutrino mode"
	  " (Positive horn current)\n";
	mode = -1;
      }
    } else {
      if (mode == 1)
	std::cout << "  Beam mode is assumed to be neutrino mode"
	  " (Negative horn current).\n";
      else
	std::cout << "  Beam mode is assumed to be anti-neutrino mode"
	  " (Positive horn current).\n";
    }
  }
  // If any exception is found the program is terminated with an error.
  catch(exception& e) {
    cerr << "  Error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << "  Exception of unknown type!\n";
    return 1;
  }

  // The CP violation phase delta expressed in radiants.
  delta = delta_degrees * ( M_PI /180.0 );

  std::cout << "  Using          " << std::endl
            << "      DM32       " <<  DM32        << std::endl
            << "      DM21       " <<  DM21        << std::endl
            << "      theta23    " <<  theta23     << std::endl
            << "      theta13    " <<  theta13     << std::endl
            << "      theta12    " <<  theta12     << std::endl
            << "      mode       " <<  mode        << std::endl;

  /****************** End of summary ******************/
 
  /****************** Histograms ******************/

  stringstream ssE, ssL;
  TH1D * histos[2][2];
  double Entry;
  
  //// Binning     
  int NBinsEnergy = 100000;
  int NBinsPath   = 100000; 
  
  /***** Path Length *****/
  // The unit of measure is Km
  double PathLengthEdge[NBinsPath+1];
  double BasePath = 810.0; // The T2K path length is approximately 295 Km and NOvA is 810 Km
  double path_start = 0;
  double path_end = 2000.0;
  double path_step = (path_end - path_start)/double(NBinsPath);
  // Linear Scale

  // Populate the Path histogram horizontal-axis
  PathLengthEdge[0]= path_start;
  for ( i=1; i <= NBinsPath; i++ ){
    Entry = path_start + double(i)*path_step;
    PathLengthEdge[i] = Entry;
  }

  /***** Energy Range *****/
  // The unit of measure is GeV
  double EnergyBins[NBinsEnergy+1];
  double BaseEnergy = 2; // T2K mean neutrino flux energy is 600 MeV  
  double e_start = 1.0e-3; // 1MeV 
  double e_end  =  10.0  ; // 10 GeV
  double e_step = log10(e_end/e_start)/double(NBinsEnergy);
  // Log Scale

  // Populate the Energy histogram horizontal-axis
  Entry = e_start;
  for(i=0; i <= NBinsEnergy; i++ )
    {
      Entry = e_start*pow( 10.0 , double(i)*e_step );
      EnergyBins[i] = Entry;
    }

  /***** Create the ROOT histograms for the neutrino oscillations *****/
  
  ///////////////////////////
  /// mu to E 
  ssE.str("");
  ssE <<  "P(#nu_{#mu} #rightarrow #nu_{e})" << " L = " << BasePath << " Km"; 
  ssL.str("");
  ssL <<  "P(#nu_{#mu} #rightarrow #nu_{e})" << " E = " << BaseEnergy << " GeV"; 
  TH1D * lmu2eE =
    new TH1D("lmu2eE", ssE.str().c_str() , NBinsEnergy, EnergyBins );
  TH1D * lmu2eL =
    new TH1D("lmu2eL", ssL.str().c_str() , NBinsPath, PathLengthEdge );

  ///////////////////////////
  /// mu to mu 
  ssE.str("");
  ssE <<  "P(#nu_{#mu} #rightarrow #nu_{#mu})" << " L = " << BasePath << " Km"; 
  ssL.str("");
  ssL <<  "P(#nu_{#mu} #rightarrow #nu_{#mu})" << " E = " << BaseEnergy << " GeV";
  TH1D * lmu2muE =
    new TH1D("lmu2muE", ssE.str().c_str() , NBinsEnergy, EnergyBins );
  TH1D * lmu2muL =
    new TH1D("lmu2muL", ssL.str().c_str() , NBinsPath, PathLengthEdge );

  ///////////////////////////
   /// mu to tau 
  ssE.str("");
  ssE <<  "P(#nu_{#mu} #rightarrow #nu_{#tau})" << " L = " << BasePath << " Km"; 
  ssL.str("");
  ssL <<  "P(#nu_{#mu} #rightarrow #nu_{#tau})" << " E = " << BaseEnergy << " GeV";
  TH1D * lmu2tauE =
    new TH1D("lmu2tauE", ssE.str().c_str() , NBinsEnergy, EnergyBins );
  TH1D * lmu2tauL =
    new TH1D("lmu2tauL", ssL.str().c_str() , NBinsPath, PathLengthEdge );
  
  histos[0][0] = lmu2eE; 
  histos[0][1] = lmu2eL; 

  histos[1][0] = lmu2muE; 
  histos[1][1] = lmu2muL; 

  histos[2][0] = lmu2tauE; 
  histos[2][1] = lmu2tauL; 

  /****************** End of histograms ******************/

  double total_prob;
  double path, energy;
  BargerPropagator   * bNu;
  bNu = new BargerPropagator( );
  bNu->UseMassEigenstates( false );

  /* The following loop spans all the energy range for Baseline given by
     Base_Path. The energy is "scanned" logarithmically. */
  
  for ( i = 0 ; i <= NBinsEnergy ; i++ )
    {
      total_prob = 0.0;
      energy = e_start*pow(10.0, double(i)*e_step);
      bNu->SetMNS( theta12, theta13, theta23, DM21, DM32, delta,
  		   energy, kSquared, mode ); 
      bNu->propagateLinear( 1*mode, BasePath, Density );

      for( j = 1; j <= 3; j++)
  	total_prob += bNu->GetProb(2, j); // Normalize the Probabilities //

      if ( total_prob >1.00001 || total_prob<0.99998 )
  	{
  	  std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
  		    << " - Energy: "<< energy << " " << std::endl;
  	  abort(); }

      for( j = 1 ; j <= 3 ; j++ ) {
      	histos[j-1][0]->Fill( energy, bNu->GetProb(2, j) );
      	// std::cout << "     Energy = " << energy << " GeV - Prob (1, "
      	// 	  << j << ") = " << bNu->GetProb(2, j) << std::endl;
      }
    } // End Energy Loop //

  /* The following loop spans the baseline for a energy given by
     BaseEnergy. The range is "scanned" linearly. */
  
  for ( i = 0 ; i < NBinsPath ; i++ ) 
    {
      path = path_start + double(i)*path_step;
      bNu->SetMNS( theta12, theta13, theta23, DM21, DM32, delta,
		   BaseEnergy, kSquared, mode ); 
      bNu->propagateLinear( 1*mode, path, Density );

      for( j = 1 ; j <= 3 ; j++ ) {
        histos[j-1][1]->Fill( path , bNu->GetProb(2, j) );
	// std::cout << "     Path = " << path << " Km - Prob (1, "
	//  	  << j << ") = " << bNu->GetProb(2, j) << std::endl;
      }
    } // End Path Loop //

  /////
  // Write the output
  TFile *tmp = new TFile("example.root", "recreate");
  tmp->cd();

  for( j = 0 ; j < 3 ; j++ ){
     histos[j][0]->Write();     
     histos[j][1]->Write();     
  }

  tmp->Close();

  cout << endl<<"Done!" << endl;
  
  return 0;
}

/*  Copyright (C) 2018  Pintaudi Giorgio

    This file is part of nu-vs-antinu.
    nu-vs-antinu is a simple program that produces a graph to quickly estimate
    the sensibility of a given experiment to the neutrino mass hierarchy.

    nu-vs-antinu is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    nu-vs-antinu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with nu-vs-antinu.  If not, see <https://www.gnu.org/licenses/>.
*/
