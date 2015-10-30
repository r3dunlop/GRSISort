#ifndef TPULSE_ANALYZER_H
#define TPULSE_ANALYZER_H

#include "TFragment.h"
#include <vector>
#include <TNamed.h>
#include <Rtypes.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//For ROOT
// #include "TH1.h"
// #include "TF1.h"
// #include "TStyle.h"
// #include "TCanvas.h"
// #include "TApplication.h"
// #include "TROOT.h"


// Mostly a direct port of SFU code
// I have stripped out some surplus and encapsulated it, but I havent changed much
// I'm sure there is more that can be stripped and reformated but it is working currently.

class TPulseAnalyzer {
  private:
	typedef struct WaveFormPar{
		//parameters for baseline
		int    baseline_range;
		double baseline; //baseline
		double baselineStDev; //baseline variance
		double baselinefin; //baseline
		double baselineStDevfin; //baseline variance
		int    bflag; //flag for baseline determiniation
		//paremeters for exclusion zone
		double max; //max value of the filtered waveform 
		double tmax; //x position of the max value of the filtered waveform
		double baselineMin; //max crossing point for exclusion zone
		double baselineMax; //max crossing point for exclusion zone
		int    temax; //x position, exclusion zone upper limit
		int    temin; //x position, exclusion zone lower limit
		double afit,bfit; //parameters for the line which fits risetime above temax
		int    mflag; //flag for tmax found
		int    teflag; //flag for exclusion zone determined
		double t0; //required for compilation of map.c - check that it works
		double t0_local;
		//new stuff necessary for compiliation of Kris' waveform analyzer changes
		double b0;
		double b1;
		long double s0;
		long double s1;
		long double s2;
		double t90;
		double t50;
		double t30;
		double t10;
		double t10t90;
		int t10_flag;
		int t30_flag;
		int t50_flag;
		int t90_flag;
		int t10t90_flag;
		int    thigh;
		double sig2noise;
	}WaveFormPar;

	typedef struct LinePar{
		double slope;
		double intercept;
		double chisq;
		double ndf;
	}LinePar;
	
	typedef struct ParPar{
		double constant;
		double linear;
		double quadratic;
		double chisq;
		double ndf;
	}ParPar;
	
	
  public:
    TPulseAnalyzer();
    TPulseAnalyzer(TFragment &frag,double=0);
    virtual ~TPulseAnalyzer();
    
    void SetData(TFragment &frag,double=0);
    void Clear(Option_t *opt = "");
    bool IsSet() { return set; }
    
    double    fit_newT0();
    double    get_sig2noise();
    void      print_WavePar();
//     void      display_newT0_fit(TApplication*);

  private:
	  
       bool   set;
       WaveFormPar* wpar;
       int N;
       TFragment* frag;

	//pulse fitting parameters
	int FILTER; //integration region for noise reduction (in samples)
	int T0RANGE; //tick range over which baseline is calulated
	double LARGECHISQ;       
	
	//linear equation dataholders
	int  lineq_dim;
	long double lineq_matrix[20][20];
	long double lineq_vector[20];
	long double lineq_solution[20];
	long double copy_matrix[20][20];  
	
	
       //internal methods       
	int solve_lin_eq();
	long double  determinant(int);

	int      fit_parabola(int,int,ParPar*);
	int      fit_smooth_parabola(int,int,double,ParPar*);
	int      fit_line(int,int,LinePar*);
	double   get_linear_T0();
	double   get_parabolic_T0();
	double   get_smooth_T0();

	void      get_baseline();
	void      get_baseline_fin();
	void      get_tmax();

	double   get_tfrac(double,double,double);
	void     get_t10();
	void     get_t30();
	void     get_t50();
	void     get_t90();       

	//bad chi squares for debugging
	const static int BADCHISQ_SMOOTH_T0=   -1024-2; //smooth_t0 gives bad result
	const static int BADCHISQ_PAR_T0    =  -1024-3; //parabolic_t0 gives bad result
	const static int BADCHISQ_LIN_T0     = -1024-4; //linear_t0 gives bad result
	const static int BADCHISQ_MAT         =-1024-5; //matrix for fit is not invertable
	//new definitions for Kris' changes to the waveform analyzer
	const static int PIN_BASELINE_RANGE=16; //minimum ticks before max for a valid signal
	const static int BAD_BASELINE_RANGE =-1024-11;
	const static int MAX_SAMPLES= 4096;	

    ClassDef(TPulseAnalyzer,1);
};

















#endif