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
#include "TGraph.h"

// Prob3++ includes
#include "BargerPropagator.h"

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

#define N_DELTA_STEPS 1000

using namespace std;

// A redefition of the operator << to include the case of a string
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(os, " ")); 
  return os;
}

int main(int argc, char * argv[] )
{
  int i, j;
  
  /// Oscillation Parameters
  bool kSquared = true;   // Using sin^2(x) variables and not sin^2(2*x)

  double theta12 = 0.320; // This is actually sin^2(34.5°)
  double theta13 = 0.021; /* This is actually sin^2(8.32°),
			     while sin^2(2*8.32°) = 0.082 */
  double theta23_LO = 0.46; // This is actually sin^2(42.7°) Lower Octant
  double theta23_UO = 0.59; // This is actually sin^2(50.2°) Upper Octant
  
  double DM21 = 7.55e-5; // in eV
  double DM32_NH = 2.50e-3; // in eV^2 - Normal Hierarchy
  double DM32_IH = -2.55e-3; // in eV^2 - Inverted Hierarchy
  
  double delta; // CP violation phase expressed in radiants

  // Continental crust desity. The unit of measure is grams over cubic centimeters
  double density = 2.70; // Average density of continental crust is 2.7 g/cm^3

  double energy = 0.600; // Mean neutrino or antineutrino beam energy in GeV

  double distance = 295; // Distance to SK far detector in Km

  string output; // ROOT output file name
  
  try {

    /* Description of all the options that can be passed to our program.
       In fact, these options are just a subset of all the oscillation parameters.
       Given the scope of the program only the following parameters can be set
       by the user. The remaining parameters are fixed once-for-all by #define
       at compilation time. */
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("DM21",      po::value<double>(), "DeltaM^2_21 in 10^-5 eV^2")
      ("DM32NH",    po::value<double>(), "DeltaM^2_32 in 10^-3 eV^2 -"
       " Normal Hierarchy (positive sign)")
      ("DM32IH",    po::value<double>(), "DeltaM^2_32 in 10^-3 eV^2 -"
       " Inverted Hierarchy (negative sign)")
      ("theta12",   po::value<double>(), "Sin^2(theta12)")
      ("theta13",   po::value<double>(), "Sin^2(theta13)")
      ("theta23LO", po::value<double>(), "Sin^2(theta23) - Lower Octant")
      ("theta23UO", po::value<double>(), "Sin^2(theta23) - Upper Octant")
      ("density",   po::value<double>(), "Continental Crust Density in g/cm^3")
      ("energy",    po::value<double>(), "Mean Beam Energy in GeV")
      ("distance",  po::value<double>(), "Distance to Far Detector in Km")
      ("output,o",  po::value<string>(&output)->default_value("output.root"),
       "Output ROOT file name")
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

    std::cout << std::endl;
    if (vm.count("DM21")) {
      std::cout << "  The mass splitting M^2_2 - M^2_1 (DeltaM^2_21) was set to " 
		<< vm["DM21"].as<double>() << " x 10^-5 eV^2.\n";
      DM21 = vm["DM21"].as<double>() * 1e-5;
    } else {
      std::cout << "  The mass splitting M^2_2 - M^2_1 (DeltaM^2_21)"
	" is assumed to be " << DM21 << " eV^2.\n";
    }

    if (vm.count("DM32NH")) {
      std::cout << "  The mass splitting DM32 for the normal hierarchy was set to " 
		<< vm["DM32NH"].as<double>() << " x 10^-3 eV^2.\n";
      DM32_NH = vm["DM32NH"].as<double>() * 1e-3;
    } else {
      std::cout << "  The mass splitting DM32 for the normal hierarchy"
	" is assumed to be " << DM32_NH << " eV^2.\n";
    }

    if (vm.count("DM32IH")) {
      std::cout << "  The mass splitting DM32 for the inverted hierarchy was set to " 
		<< vm["DM32IH"].as<double>() << " x 10^-3 eV^2.\n";
      DM32_IH = vm["DM32IH"].as<double>() * 1e-3;
    } else {
      std::cout << "  The mass splitting DM32 for the inverted hierarchy"
	" is assumed to be " << DM32_IH << " eV^2.\n";
    } 

    if (vm.count("theta12")) {
      std::cout << "  The Sin^2(theta12) oscillation angle was set to " 
		<< vm["theta12"].as<double>() << " .\n";
      theta12 = vm["theta12"].as<double>();
    } else {
      std::cout << "  The Sin^2(theta12) oscillation angle"
	" is assumed to be " << theta12 << " .\n";
    }
    
    if (vm.count("theta13")) {
      std::cout << "  The Sin^2(theta13) oscillation angle was set to " 
		<< vm["theta13"].as<double>() << " .\n";
      theta13 = vm["theta13"].as<double>();
    } else {
      std::cout << "  The Sin^2(theta13) oscillation angle"
	" is assumed to be " << theta13 << " .\n";
    }

    if (vm.count("theta23LO")) {
      std::cout << "  The Sin^2(theta23) oscillation angle for the lower"
	" octant was set to " << vm["theta23LO"].as<double>() << " .\n";
      theta23_LO = vm["theta23LO"].as<double>();
    } else {
      std::cout << "  The Sin^2(theta23) oscillation angle for the lower"
	" octant is assumed to be " << theta23_LO << " .\n";
    }
    
    if (vm.count("theta23UO")) {
      std::cout << "  The Sin^2(theta23) oscillation angle for the upper"
	" octant was set to " << vm["theta23UO"].as<double>() << " .\n";
      theta23_UO = vm["theta23UO"].as<double>();
    } else {
      std::cout << "  The Sin^2(theta23) oscillation angle for the upper"
	" octant is assumed to be " << theta23_UO << " .\n";
    }    

    if (vm.count("density")) {
      std::cout << "  The average density of continental crust was set to " 
		<< vm["density"].as<double>() << " g/cm^3.\n";
      density = vm["density"].as<double>();
    } else {
      std::cout << "  The average density of continental crust"
	" is assumed to be " << density << " g/cm^3.\n";
    }

    if (vm.count("energy")) {
      std::cout << "  The mean energy of the neutrino beam was set to " 
		<< vm["energy"].as<double>() << " GeV.\n";
      energy = vm["energy"].as<double>();
    } else {
      std::cout << "  The mean energy of the neutrino beam"
	" is assumed to be " << energy << " GeV.\n";
    }
    
    if (vm.count("distance")) {
      std::cout << "  The distance to the far detector was set to " 
		<< vm["distance"].as<double>() << " Km.\n";
      distance = vm["distance"].as<double>();
    } else {
      std::cout << "  The distance to the far detector"
	" is assumed to be " << distance << " Km.\n";
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

  std::cout << std::endl;
  std::cout << "  Summary:       " <<                              std::endl
	    << "      DM21       " <<  DM21       << " eV^2" <<   std::endl
            << "      DM32_NH    " <<  DM32_NH    << " eV^2" <<   std::endl
	    << "      DM32_IH    " <<  DM32_IH    << " eV^2" <<   std::endl
            << "      theta12    " <<  theta12    <<               std::endl
	    << "      theta13    " <<  theta13    <<               std::endl
            << "      theta23_LO " <<  theta23_LO <<               std::endl
	    << "      theta23_UO " <<  theta23_UO <<               std::endl
	    << "      density    " <<  density    << " g/cm^3" <<  std::endl
    	    << "      energy     " <<  energy     << " GeV"   <<  std::endl
    	    << "      distance   " <<  distance   << " Km"     <<  std::endl;

  /****************** End of summary ******************/
 
  /***** Calculate the neutrino oscillation points *****/

  double total_prob;
  double delta_step = 2 * M_PI / (double) N_DELTA_STEPS;
  double mu2e_LO_NH[2][N_DELTA_STEPS+1];
  double mu2e_UO_NH[2][N_DELTA_STEPS+1];
  double mu2e_LO_IH[2][N_DELTA_STEPS+1];
  double mu2e_UO_IH[2][N_DELTA_STEPS+1];
  BargerPropagator * bNu;
  bNu = new BargerPropagator( );
  bNu->UseMassEigenstates( false );
  
  /************ NORMAL HIERARCHY - LOWER OCTANT ************/

  for(i = 0; i <= N_DELTA_STEPS; i++) {
    delta = - M_PI + i*delta_step;
    
    // neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, delta,
		 energy, kSquared, 1 ); 
    bNu->propagateLinear( 1, distance, density );

    total_prob = 0;
    for( j = 1; j <= 3; j++) total_prob += bNu->GetProb(2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }

    mu2e_LO_NH[0][i] = bNu->GetProb(2, 1); // Fill the array

    // anti-neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, delta,
		 energy, kSquared, -1 ); 
    bNu->propagateLinear( -1, distance, density );

    total_prob = 0;
    for( j = -1; j >= -3; j--) total_prob += bNu->GetProb(-2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }
      
    mu2e_LO_NH[1][i] = bNu->GetProb(-2, -1);

    // std::cout << "  Delta = " << delta << " - mu2e = " << mu2e_LO_NH[0][i] 
    // 	      << " - mu2e_bar = " << mu2e_LO_NH[1][i] << std::endl;
  }

  /************ NORMAL HIERARCHY - UPPER OCTANT ************/
  delta = 0;
  for(i = 0; i <= N_DELTA_STEPS; i++) {
    delta = - M_PI + i*delta_step;
	
    // neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, delta,
		 energy, kSquared, 1 ); 
    bNu->propagateLinear( 1, distance, density );

    total_prob = 0;
    for( j = 1; j <= 3; j++) total_prob += bNu->GetProb(2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }

    mu2e_UO_NH[0][i] = bNu->GetProb(2, 1); // Fill the array

    // anti-neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, delta,
		 energy, kSquared, -1 ); 
    bNu->propagateLinear( -1, distance, density );

    total_prob = 0;
    for( j = -1; j >= -3; j--) total_prob += bNu->GetProb(-2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }
      
    mu2e_UO_NH[1][i] = bNu->GetProb(-2, -1);
  }

  /************ INVERTED HIERARCHY - LOWER OCTANT ************/
  delta = 0;
  for(i = 0; i <= N_DELTA_STEPS; i++) {
    delta = - M_PI + i*delta_step;
    
    // neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, delta,
		 energy, kSquared, 1 ); 
    bNu->propagateLinear( 1, distance, density );

    total_prob = 0;
    for( j = 1; j <= 3; j++) total_prob += bNu->GetProb(2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }

    mu2e_LO_IH[0][i] = bNu->GetProb(2, 1); // Fill the array

    // anti-neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, delta,
		 energy, kSquared, -1 ); 
    bNu->propagateLinear( -1, distance, density );

    total_prob = 0;
    for( j = -1; j >= -3; j--) total_prob += bNu->GetProb(-2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }
      
    mu2e_LO_IH[1][i] = bNu->GetProb(-2, -1);
  }

  /************ INVERTED HIERARCHY - UPPER OCTANT ************/
  delta = 0;
  for(i = 0; i <= N_DELTA_STEPS; i++) {
    delta = - M_PI + i*delta_step;
    
    // neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, delta,
		 energy, kSquared, 1 ); 
    bNu->propagateLinear( 1, distance, density );

    total_prob = 0;
    for( j = 1; j <= 3; j++) total_prob += bNu->GetProb(2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }

    mu2e_UO_IH[0][i] = bNu->GetProb(2, 1); // Fill the array

    // anti-neutrino beam
    bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, delta,
		 energy, kSquared, -1 ); 
    bNu->propagateLinear( -1, distance, density );

    total_prob = 0;
    for( j = -1; j >= -3; j--) total_prob += bNu->GetProb(-2, j);
    if ( total_prob >1.00001 || total_prob<0.99998 )
      {
	std::cerr << "  ERROR (i = " << i << ") - Prob: " << total_prob
		  << " - Energy: "<< energy << std::endl;
	abort(); }
      
    mu2e_UO_IH[1][i] = bNu->GetProb(-2, -1);
  }

  /***** Create the ROOT graph for the neutrino oscillations *****/

  TGraph *gr_LO_NH = new TGraph(N_DELTA_STEPS+1, mu2e_LO_NH[0], mu2e_LO_NH[1]);
  TGraph *gr_UO_NH = new TGraph(N_DELTA_STEPS+1, mu2e_UO_NH[0], mu2e_UO_NH[1]);
  TGraph *gr_LO_IH = new TGraph(N_DELTA_STEPS+1, mu2e_LO_IH[0], mu2e_LO_IH[1]);
  TGraph *gr_UO_IH = new TGraph(N_DELTA_STEPS+1, mu2e_UO_IH[0], mu2e_UO_IH[1]);

  double x[2], y[2];

  /* The following code just creates 8 graphs each containing only 2 points.
     These points correspond to the values of delta: 0, 1/2 pi, pi, 3/2 pi.
     Each graph is in common with the lower and upper octants.
     Actually the Prob3++ defines delta as going from -pi to pi so, to recover
     the standard dominion of definition [0,2pi] we will need to add a fixed pi
     phase when drawing the graph.
*/

  // delta = 0 - Normal hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, 0,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, 0,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, 0,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, 0,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_NH_0 = new TGraph(2, x, y);

  // delta = 1/2 pi - Normal hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, .5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, .5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, .5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, .5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_NH_1 = new TGraph(2, x, y);

  // delta = pi - Normal hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_NH_2 = new TGraph(2, x, y);

  // delta = 3/2 pi - Normal hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, 1.5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_NH, 1.5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, 1.5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_NH, 1.5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_NH_3 = new TGraph(2, x, y);

  // delta = 0 - Inverted hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, 0,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, 0,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, 0,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, 0,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_IH_0 = new TGraph(2, x, y);

  // delta = 1/2 pi - Inverted hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, .5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, .5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, .5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, .5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_IH_1 = new TGraph(2, x, y);

  // delta = pi - Inverted hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_IH_2 = new TGraph(2, x, y);

  // delta = 3/2 pi - Inverted hierarchy
  
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, 1.5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[0] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_LO, DM21, DM32_IH, 1.5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[0] = bNu->GetProb(-2, -1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, 1.5 * M_PI,
	       energy, kSquared, 1 ); 
  bNu->propagateLinear( 1, distance, density );
  x[1] = bNu->GetProb(2, 1);
  bNu->SetMNS( theta12, theta13, theta23_UO, DM21, DM32_IH, 1.5 * M_PI,
	       energy, kSquared, -1 ); 
  bNu->propagateLinear( -1, distance, density );
  y[1] = bNu->GetProb(-2, -1);
  TGraph *gr_IH_3 = new TGraph(2, x, y);
  
  // Write the output
  TFile *tmp = new TFile(output.c_str(), "recreate");
  tmp->cd();

  gr_LO_NH->Write("LO_NH");
  gr_UO_NH->Write("UO_NH");
  gr_LO_IH->Write("LO_IH");
  gr_UO_IH->Write("UO_IH");
  gr_NH_0->Write("NH_0");
  gr_NH_1->Write("NH_1");
  gr_NH_2->Write("NH_2");
  gr_NH_3->Write("NH_3");
  gr_IH_0->Write("IH_0");
  gr_IH_1->Write("IH_1");
  gr_IH_2->Write("IH_2");
  gr_IH_3->Write("IH_3");
  
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
