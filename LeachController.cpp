/* *********************************************************************
 * This file contains functions that set the basic parameters,
 * clocks and bias lines of the CCD. Vdd toggling functions are also
 * implemented here.
 *
 * Created by Pitam Mitra on 2019-02-13.
 * *********************************************************************
 */

#include <string>
#include <iostream>
#include <fstream>

#include "CArcDevice.h"
#include "CArcPCIe.h"
#include "CArcPCI.h"
#include "CArcDeinterlace.h"
#include "CExpIFace.h"
#include "ArcDefs.h"


#include "LeachController.hpp"
#include "CCDControlDataTypes.hpp"



LeachController::LeachController(std::string INIFileLoc)
{

    /*New ArcDevice*/
    pArcDev = new arc::device::CArcPCIe;

    /*Open the Arc controller by loading a timing file*/
    arc::device::CArcPCIe::FindDevices();
    pArcDev->Open(0);

    this->INIFileLoc = INIFileLoc;

    /*Check if a controller is connected*/
    if ( !pArcDev->IsControllerConnected() )
        std::cout<<"Warning: A controller is not connected. Check if the unit is connected and powered on.\n";



}

LeachController::~LeachController()
{

    pArcDev->Close();

}

/* Function to apply all the basic CCD params.*/

void LeachController::ApplyAllCCDBasic(void ){

    if (this->CCDParams.super_sequencer){
        /*Set CCD type for the sequencer - DES or SK sequence*/
        this->SetCCDType();
        /*Now set the amplifier direction since this affects IDLE mode*/
        this->SelectAmplifierAndHClocks();
        /*AFTER amplifiers are selected, change the H-clock directions if necessary*/
        if (this->CCDParams.AmplifierDirection != this->CCDParams.HClkDirection)
            this->SetHDR();
        /*Set V-Clock direction*/
        this->SetVDR();
        /*Next, set the integral time*/
        this->ApplyNewIntegralTimeAndGain(this->CCDParams.IntegralTime, this->CCDParams.Gain);
        /*Next, we need to set the Pedestal and Signal wait times before an integration begins*/
        this->ApplyNewPedestalIntegralWait(this->CCDParams.PedestalIntgWait);
        this->ApplyNewSignalIntegralWait(this->CCDParams.SignalIntgWait);
        /*Binning*/
        this->ApplyPBIN(this->CCDParams.ParallelBin);
        this->ApplySBIN(this->CCDParams.SerialBin);
        /*Timing of DG, OG and Reset*/
        this->ApplyDGWidth(this->CCDParams.DGWidth);
        this->ApplyOGWidth(this->CCDParams.OGWidth);
        this->ApplySkippingRGWidth(this->CCDParams.SKRSTWidth);
        this->ApplySummingWellWidth(this->CCDParams.SWWidth);

    }

    if (this->CCDParams.CCDType == "SK"){
        this->SetSSR();
    }

}



/*
 * Function to set all the clock voltages of the CCD.
 * The lines are UW specific (or rather specific to the US second stage board)
 * so keeping this function here makes sense since people might edit
 * these values if they change the second stage board.
 */

void LeachController::ApplyAllCCDClocks(void )
{
    /*Set Clocks*/

    /*These sets of clocks are common and did not change between Second stage generations.*/

    this->SetDACValueClock(0, this->ClockParams.one_vclock_lo, this->ClockParams.one_vclock_hi); //Channel 0: V1
    this->SetDACValueClock(1, this->ClockParams.one_vclock_lo, this->ClockParams.one_vclock_hi); //Channel 1: V2
    this->SetDACValueClock(2, this->ClockParams.one_vclock_lo, this->ClockParams.one_vclock_hi); //Channel 2: V3

    this->SetDACValueClock(12, this->ClockParams.l_hclock_lo, this->ClockParams.l_hclock_hi); //Channel 12: H1L
    this->SetDACValueClock(13, this->ClockParams.l_hclock_lo, this->ClockParams.l_hclock_hi); //Channel 13: H2L
    this->SetDACValueClock(14, this->ClockParams.l_hclock_lo, this->ClockParams.l_hclock_hi); //Channel 14: H3L
    this->SetDACValueClock(15, this->ClockParams.u_hclock_lo, this->ClockParams.u_hclock_hi); //Channel 15: H1U
    this->SetDACValueClock(16, this->ClockParams.u_hclock_lo, this->ClockParams.u_hclock_hi); //Channel 16: H2U
    this->SetDACValueClock(17, this->ClockParams.u_hclock_lo, this->ClockParams.u_hclock_hi); //Channel 17: H3U
    
    this->SetMultiplexers(this->ClockParams.mux1, this->ClockParams.mux2);

    /*These parameters do change between boards */
    if (this->CCDParams.SecondStageVersion == "UW1") {


        this->SetDACValueClock(6, this->ClockParams.tg_lo, this->ClockParams.tg_hi); //Channel 6: TG

        this->SetDACValueClock(18, this->ClockParams.sw_lo, this->ClockParams.sw_hi); //Channel 18: SWL
        this->SetDACValueClock(23, this->ClockParams.sw_lo, this->ClockParams.sw_hi); //Channel 23: SWU

        //Reset gate needs to be checked against the current timing file and be flipped if necessary
        if (this->CCDParams.InvRG) {
            this->SetDACValueClock(20, this->ClockParams.rg_hi, this->ClockParams.rg_lo); //Channel 20: RGL
            this->SetDACValueClock(21, this->ClockParams.rg_hi, this->ClockParams.rg_lo); //Channel 21: RGU
        } else {
            this->SetDACValueClock(20, this->ClockParams.rg_lo, this->ClockParams.rg_hi); //Channel 20: RGL
            this->SetDACValueClock(21, this->ClockParams.rg_lo, this->ClockParams.rg_hi); //Channel 21: RGU
        }

        this->SetDACValueClock(7, this->ClockParams.og_lo, this->ClockParams.og_hi); //Channel 7: OG
        this->SetDACValueClock(9, this->ClockParams.og_lo, this->ClockParams.og_hi); //Channel 9: OG

        this->SetDACValueClock(8, this->ClockParams.dg_lo, this->ClockParams.dg_hi); //Channel 8: DG
        this->SetDACValueClock(10, this->ClockParams.dg_lo, this->ClockParams.dg_hi); //Channel 10: DG



    } else if (this->CCDParams.SecondStageVersion=="UW2") {


        this->SetDACValueClock(3, this->ClockParams.two_vclock_lo, this->ClockParams.two_vclock_hi); //Channel 3: 2V1
        this->SetDACValueClock(4, this->ClockParams.two_vclock_lo, this->ClockParams.two_vclock_hi); //Channel 4: 2V2
        this->SetDACValueClock(5, this->ClockParams.two_vclock_lo, this->ClockParams.two_vclock_hi); //Channel 5: 2V3

        /*You should only be able open the TG corresponding to the direction of charge movement*/
        if (this->CCDParams.VClkDirection == "1") {
            this->SetDACValueClock(6, this->ClockParams.tg_lo, this->ClockParams.tg_hi); //Channel 6: TG1
            this->SetDACValueClock(8, this->ClockParams.tg_hi, this->ClockParams.tg_hi); //Channel 8: TG2
        } else if (this->CCDParams.VClkDirection == "2"){
            this->SetDACValueClock(6, this->ClockParams.tg_hi, this->ClockParams.tg_hi); //Channel 6: TG1
            this->SetDACValueClock(8, this->ClockParams.tg_lo, this->ClockParams.tg_hi); //Channel 8: TG2
        } else {
            this->SetDACValueClock(6, this->ClockParams.tg_lo, this->ClockParams.tg_hi); //Channel 6: TG1
            this->SetDACValueClock(8, this->ClockParams.tg_lo, this->ClockParams.tg_hi); //Channel 8: TG2

            if (this->CCDParams.VClkDirection != "12")
                std::cout<<"V-Clock direction is ambiguous, so both TG are set to enabled. "
                           "However, you should still stop and verify the V-clock directions.";
        }

        
        this->SetDACValueClock(7, this->ClockParams.og_lo, this->ClockParams.og_hi); //Channel 7: OG1
        this->SetDACValueClock(9, this->ClockParams.og_lo, this->ClockParams.og_hi); //Channel 9: OG2

        this->SetDACValueClock(18, this->ClockParams.sw_lo, this->ClockParams.sw_hi); //Channel 18: SWL
        this->SetDACValueClock(23, this->ClockParams.sw_lo, this->ClockParams.sw_hi); //Channel 23: SWU


        //Reset gate needs to be checked against the current timing file and be flipped if necessary
        if (this->CCDParams.InvRG) {
            this->SetDACValueClock(20, this->ClockParams.rg_hi, this->ClockParams.rg_lo); //Channel 20: RG1
            this->SetDACValueClock(22, this->ClockParams.rg_hi, this->ClockParams.rg_lo); //Channel 21: RG2
        } else {
            this->SetDACValueClock(20, this->ClockParams.rg_lo, this->ClockParams.rg_hi); //Channel 20: RG1
            this->SetDACValueClock(22, this->ClockParams.rg_lo, this->ClockParams.rg_hi); //Channel 21: RG2
        }


        this->SetDACValueClock(19, this->ClockParams.dg_lo, this->ClockParams.dg_hi); //Channel 18: DG1
        this->SetDACValueClock(21, this->ClockParams.dg_lo, this->ClockParams.dg_hi); //Channel 23: DG2

    } else {

        std::cout<<"The second stage is neither UW1 or UW2, which means pre-set clock voltages could not be applied. "
                 <<"Stop and check.\n";
    }

}



/*
 * Function to apply the bias voltages.
 * Again, this is specific to the UW second stage board.
 */
void LeachController::ApplyAllBiasVoltages(void )
{

    /*Set Biases*/
    //Vdd
    this->SetDACValueBias(0,BiasVoltToADC(this->BiasParams.vdd,0));
    this->SetDACValueBias(1,BiasVoltToADC(this->BiasParams.vdd,1));
    this->SetDACValueBias(2,0);
    this->SetDACValueBias(3,0);


    //VR(1-4)
    if (this->CCDParams.CCDType=="DES")
    {
        this->SetDACValueBias(4,BiasVoltToADC(this->BiasParams.vref,4));
        this->SetDACValueBias(5,BiasVoltToADC(this->BiasParams.vref,5));
    }
    else
    {
        this->SetDACValueBias(4,BiasVoltToADC(this->BiasParams.vrefsk,4));
        this->SetDACValueBias(5,BiasVoltToADC(this->BiasParams.vrefsk,5));
    }
    //DrainL and DrainU
    if(this->CCDParams.CCDType=="SK")
    {
        this->SetDACValueBias(6,BiasVoltToADC(this->BiasParams.drain,6));
        this->SetDACValueBias(7,BiasVoltToADC(this->BiasParams.drain,7));
    }

    //OG(1-4)
    if (this->CCDParams.CCDType=="DES")
    {
        this->SetDACValueBias(8,BiasVoltToADC(this->BiasParams.opg,8));
        this->SetDACValueBias(9,BiasVoltToADC(this->BiasParams.opg,9));
    }

    this->SetDACValueBias(10,0);
    //Controls Relay for battery box
    this->SetDACValueBias(11,BiasVoltToADC(this->BiasParams.battrelay,11));

    //VSUB
    //this->SetDACValueBias(pArcDev, 12,0);
    //this->SetDACValueBias(pArcDev, 13,0);

    //Video Offsets, channels 2 and 3 on the video board. 3 is R and 2 is L
    this->SetDACValueVideoOffset(2, this->BiasParams.video_offsets_U);
    this->SetDACValueVideoOffset(3, this->BiasParams.video_offsets_L);

}


/*
 * The Vdd toggling function
 */

void LeachController::ToggleVDD(bool VDDState){

    if (VDDState == 1){
        this->SetDACValueBias(0,BiasVoltToADC(this->BiasParams.vdd,0));
        this->SetDACValueBias(1,BiasVoltToADC(this->BiasParams.vdd,1));
        _expose_isVDDOn = true;
    } else if (VDDState == 0){
        this->SetDACValueBias(0,0);
        this->SetDACValueBias(1,0);
        _expose_isVDDOn = false;
    } else {
        printf("Could not toggle the VDD since condition was not understood.\n");
    }



}
