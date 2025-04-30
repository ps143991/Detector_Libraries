/*****************************************************************************
 * Copyright (C) 2009-2022   this file is part of the NPTool Project       *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Hugo Jacob  contact address: hjacob@ijclab.in2p3.fr                        *
 *                                                                           *
 * Creation Date  : October 2022                                           *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold ZDD Raw data                                    *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#include "TZDDData.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

ClassImp(TZDDData)


//////////////////////////////////////////////////////////////////////
TZDDData::TZDDData() {
}



//////////////////////////////////////////////////////////////////////
TZDDData::~TZDDData() {
}



//////////////////////////////////////////////////////////////////////
void TZDDData::Clear() {
    //ICs clear

    fZDD_IC_1_E.clear();
    fZDD_IC_2_E.clear();
    fZDD_IC_3_E.clear();
    fZDD_IC_4_E.clear();
    fZDD_IC_5_E.clear();
    fZDD_IC_9_E.clear();
    fZDD_IC_1_TS.clear();
    fZDD_IC_2_TS.clear();
    fZDD_IC_3_TS.clear();
    fZDD_IC_4_TS.clear();
    fZDD_IC_5_TS.clear();
    fZDD_IC_9_TS.clear();

    fZDD_IC_E.clear();
    fZDD_IC_N.clear();
    fZDD_IC_TS.clear();

    //PLs Clear
    fZDD_PL_1_E.clear();
    fZDD_PL_2_E.clear();
    fZDD_PL_3_E.clear();
    fZDD_PL_4_E.clear();
    fZDD_PL_5_E.clear();
    fZDD_PL_6_E.clear();
    fZDD_PL_7_E.clear();
    fZDD_PL_8_E.clear();
    fZDD_PL_9_E.clear();
    fZDD_PL_10_E.clear();

    fZDD_PL_1_TS.clear();
    fZDD_PL_2_TS.clear();
    fZDD_PL_3_TS.clear();
    fZDD_PL_4_TS.clear();
    fZDD_PL_5_TS.clear();
    fZDD_PL_6_TS.clear();
    fZDD_PL_7_TS.clear();
    fZDD_PL_8_TS.clear();
    fZDD_PL_9_TS.clear();
    fZDD_PL_10_TS.clear();

    fZDD_PL_E.clear();
    fZDD_PL_N.clear();
    fZDD_PL_TS.clear();



    //DCs clear
    fZDD_DC_1_E.clear();
    fZDD_DC_2_E.clear();
    fZDD_DC_3_E.clear();
    fZDD_DC_4_E.clear();

    fZDD_DC_1_TS.clear();
    fZDD_DC_2_TS.clear();
    fZDD_DC_3_TS.clear();
    fZDD_DC_4_TS.clear();

    fZDD_DC_E.clear();
    fZDD_DC_N.clear();
    fZDD_DC_TS.clear();

    // fZDD_Drift_DetectorNbr.clear();
    // fZDD_DriftTime.clear();
    // fZDD_X.clear();

    //Exos clear
    fZDD_EXO_1_E.clear();
    fZDD_EXO_2_E.clear();
    fZDD_EXO_3_E.clear();
    fZDD_EXO_4_E.clear();

    fZDD_EXO_1_TS.clear();
    fZDD_EXO_2_TS.clear();
    fZDD_EXO_3_TS.clear();
    fZDD_EXO_4_TS.clear();

    fZDD_EXO_E.clear();
    fZDD_EXO_N.clear();
    fZDD_EXO_TS.clear();
}

void TZDDData::SetZDDIC(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp){
    if(DetNumb==0){fZDD_IC_1_E.push_back(Energy); fZDD_IC_1_TS.push_back(TimeStamp);}
    if(DetNumb==1){fZDD_IC_2_E.push_back(Energy); fZDD_IC_2_TS.push_back(TimeStamp);}
    if(DetNumb==2){fZDD_IC_3_E.push_back(Energy); fZDD_IC_3_TS.push_back(TimeStamp);}
    if(DetNumb==3){fZDD_IC_4_E.push_back(Energy); fZDD_IC_4_TS.push_back(TimeStamp);}
    if(DetNumb==8){fZDD_IC_5_E.push_back(Energy); fZDD_IC_5_TS.push_back(TimeStamp);}
    if(DetNumb==9){fZDD_IC_9_E.push_back(Energy); fZDD_IC_9_TS.push_back(TimeStamp);}
    fZDD_IC_E.push_back(Energy);
    fZDD_IC_N.push_back(DetNumb);
    fZDD_IC_TS.push_back(TimeStamp);
}

void TZDDData::SetZDDPL(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp){
    if(DetNumb==0){fZDD_PL_1_E.push_back(Energy); fZDD_PL_1_TS.push_back(TimeStamp);}
    if(DetNumb==1){fZDD_PL_2_E.push_back(Energy); fZDD_PL_2_TS.push_back(TimeStamp);}
    if(DetNumb==2){fZDD_PL_3_E.push_back(Energy); fZDD_PL_3_TS.push_back(TimeStamp);}
    if(DetNumb==3){fZDD_PL_4_E.push_back(Energy); fZDD_PL_4_TS.push_back(TimeStamp);}
    if(DetNumb==4){fZDD_PL_5_E.push_back(Energy); fZDD_PL_5_TS.push_back(TimeStamp);}
    if(DetNumb==5){fZDD_PL_6_E.push_back(Energy); fZDD_PL_6_TS.push_back(TimeStamp);}
    if(DetNumb==6){fZDD_PL_7_E.push_back(Energy); fZDD_PL_7_TS.push_back(TimeStamp);}
    if(DetNumb==7){fZDD_PL_8_E.push_back(Energy); fZDD_PL_8_TS.push_back(TimeStamp);}
    if(DetNumb==8){fZDD_PL_9_E.push_back(Energy); fZDD_PL_9_TS.push_back(TimeStamp);}
    if(DetNumb==9){fZDD_PL_10_E.push_back(Energy); fZDD_PL_10_TS.push_back(TimeStamp);}
    fZDD_PL_E.push_back(Energy);
    fZDD_PL_N.push_back(DetNumb);
    fZDD_PL_TS.push_back(TimeStamp);
}

void TZDDData::SetZDDDC(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp) {
    if(DetNumb==0){fZDD_DC_1_E.push_back(Energy); fZDD_DC_1_TS.push_back(TimeStamp);}
    if(DetNumb==1){fZDD_DC_2_E.push_back(Energy); fZDD_DC_2_TS.push_back(TimeStamp);}
    if(DetNumb==2){fZDD_DC_3_E.push_back(Energy); fZDD_DC_3_TS.push_back(TimeStamp);}
    if(DetNumb==3){fZDD_DC_4_E.push_back(Energy); fZDD_DC_4_TS.push_back(TimeStamp);}
    fZDD_DC_E.push_back(Energy);
    fZDD_DC_N.push_back(DetNumb);
    fZDD_DC_TS.push_back(TimeStamp);
}

void TZDDData::SetZDDEXO(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp) {
    if(DetNumb==0){fZDD_EXO_1_E.push_back(Energy); fZDD_EXO_1_TS.push_back(TimeStamp);}
    if(DetNumb==1){fZDD_EXO_2_E.push_back(Energy); fZDD_EXO_2_TS.push_back(TimeStamp);}
    if(DetNumb==2){fZDD_EXO_3_E.push_back(Energy); fZDD_EXO_3_TS.push_back(TimeStamp);}
    if(DetNumb==3){fZDD_EXO_4_E.push_back(Energy); fZDD_EXO_4_TS.push_back(TimeStamp);}
    fZDD_EXO_E.push_back(Energy);
    fZDD_EXO_N.push_back(DetNumb);
    fZDD_EXO_TS.push_back(TimeStamp);
    }

//////////////////////////////////////////////////////////////////////
void TZDDData::Dump() const {
  // This method is very useful for debuging and worth the dev.
  cout << "XXXXXXXXXXXXXXXXXXXXXXXX New Event [TZDDData::Dump()] XXXXXXXXXXXXXXXXX" << endl;

  cout << "ZDD_ICE Mult: " << fZDD_IC_E.size() << endl;
  for (size_t i = 0 ; i < fZDD_IC_E.size(); i++){
    cout << "DetNbr: " << fZDD_IC_N[i] << " Energy: " << fZDD_IC_E[i] << " TimeStamp: " << fZDD_IC_TS[i] << endl;
  }
  // cout << "ZDD_PLE Mult: " << fZDD_PL_E.size() << endl;
  // for (size_t i = 0 ; i < fZDD_PL_E.size(); i++){
  //   cout << "DetNbr: " << fZDD_PL_N[i] << " Energy: " << fZDD_PL_E[i] << " TimeStamp: " << fZDD_PL_TS[i] << endl;
  // }
  // cout << "ZDD_DCE Mult: " << fZDD_DC_E.size() << endl;
  // for (size_t i = 0 ; i < fZDD_DC_E.size(); i++){
  //   cout << "DetNbr: " << fZDD_DC_N[i] << " Energy: " << fZDD_DC_E[i] << " TimeStamp: " << fZDD_DC_TS[i] << endl;
  // }
  // cout << "ZDD_EXOE Mult: " << fZDD_EXO_E.size() << endl;
  // for (size_t i = 0 ; i < fZDD_EXO_E.size(); i++){
  //   cout << "DetNbr: " << fZDD_EXO_N[i] << " Energy: " << fZDD_EXO_E[i] << " TimeStamp: " << fZDD_EXO_TS[i] << endl;
  // }
}