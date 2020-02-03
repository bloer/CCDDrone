//
// Created by Pitam Mitra on 2019-02-13.
//

#ifndef CCDCONTROL_DTYPES
#define CCDCONTROL_DTYPES
#include <chrono>


struct CCDVariables{
    /*Variables that will need to be set before exposure*/
    std::string sTimFile;
    std::string CCDType;
    std::string SecondStageVersion;


    float fExpTime;
    int nSkipperR;
    int dRows;
    int dCols;
    bool InvRG;

    std::string AmplifierDirection;
    std::string HClkDirection;
    std::string VClkDirection;

    bool super_sequencer;
    double IntegralTime;
    double PedestalIntgWait;
    double SignalIntgWait;
    double DGWidth;
    double OGWidth;
    double SKRSTWidth;
    double SWWidth;

    int ParallelBin;
    int SerialBin;

    int Gain;
    int ItgSpeed;

};

struct ClockVariables{
    /*Clock settings*/
    double one_vclock_hi;
    double one_vclock_lo;
    double two_vclock_hi;
    double two_vclock_lo;

    double tg_hi;
    double tg_lo;

    double u_hclock_hi;
    double u_hclock_lo;
    double l_hclock_hi;
    double l_hclock_lo;

    double dg_hi;
    double dg_lo;
    double rg_hi;
    double rg_lo;

    double sw_hi;
    double sw_lo;
    double og_hi;
    double og_lo;

	int mux1;
	int mux2;
};


struct BiasVariables{

    double vdd;
    double vrefsk;
    double vref;
    double drain;
    double opg;
    double battrelay;
    int video_offsets_U;
    int video_offsets_L;

};


struct TimeVariables{

    std::chrono::system_clock::time_point ProgramStart;
    std::chrono::system_clock::time_point ExpStart;
    std::chrono::system_clock::time_point Readoutstart;
    std::chrono::system_clock::time_point ReadoutEnd;

    double MeasuredReadout;
    double MeasuredExp;

    bool isExp = false;
    bool isReadout = false;
    bool rClockCounter = 0;

};

#endif //CCDCONTROL_DTYPES
