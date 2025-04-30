#ifndef __ZDDDATA__
#define __ZDDDATA__
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

// STL
#include <iostream>
#include <vector>
using namespace std;

// ROOT
#include "TObject.h"

class TZDDData : public TObject {
    //////////////////////////////////////////////////////////////
    // data members are hold into vectors in order
    // to allow multiplicity treatment
private:
    // IC
    vector<unsigned int> fZDD_IC_1_E;
    vector<unsigned int> fZDD_IC_2_E;
    vector<unsigned int> fZDD_IC_3_E;
    vector<unsigned int> fZDD_IC_4_E;
    vector<unsigned int> fZDD_IC_5_E;
    vector<unsigned int> fZDD_IC_9_E;

    vector<unsigned long long> fZDD_IC_1_TS;
    vector<unsigned long long> fZDD_IC_2_TS;
    vector<unsigned long long> fZDD_IC_3_TS;
    vector<unsigned long long> fZDD_IC_4_TS;
    vector<unsigned long long> fZDD_IC_5_TS;
    vector<unsigned long long> fZDD_IC_9_TS;

    vector<unsigned int> fZDD_IC_N;
    vector<unsigned int> fZDD_IC_E;
    vector<unsigned int> fZDD_IC_TS;


    // Plastic
    vector<unsigned int> fZDD_PL_1_E;
    vector<unsigned int> fZDD_PL_2_E;
    vector<unsigned int> fZDD_PL_3_E;
    vector<unsigned int> fZDD_PL_4_E;
    vector<unsigned int> fZDD_PL_5_E;
    vector<unsigned int> fZDD_PL_6_E;
    vector<unsigned int> fZDD_PL_7_E;
    vector<unsigned int> fZDD_PL_8_E;
    vector<unsigned int> fZDD_PL_9_E;
    vector<unsigned int> fZDD_PL_10_E;

    vector<unsigned long long> fZDD_PL_1_TS;
    vector<unsigned long long> fZDD_PL_2_TS;
    vector<unsigned long long> fZDD_PL_3_TS;
    vector<unsigned long long> fZDD_PL_4_TS;
    vector<unsigned long long> fZDD_PL_5_TS;
    vector<unsigned long long> fZDD_PL_6_TS;
    vector<unsigned long long> fZDD_PL_7_TS;
    vector<unsigned long long> fZDD_PL_8_TS;
    vector<unsigned long long> fZDD_PL_9_TS;
    vector<unsigned long long> fZDD_PL_10_TS;

    vector<unsigned int> fZDD_PL_E;
    vector<unsigned int> fZDD_PL_N;
    vector<unsigned long long> fZDD_PL_TS;

    // DC
    vector<unsigned int> fZDD_DC_1_E;
    vector<unsigned int> fZDD_DC_2_E;
    vector<unsigned int> fZDD_DC_3_E;
    vector<unsigned int> fZDD_DC_4_E;

    vector<unsigned long long> fZDD_DC_1_TS;
    vector<unsigned long long> fZDD_DC_2_TS;
    vector<unsigned long long> fZDD_DC_3_TS;
    vector<unsigned long long> fZDD_DC_4_TS;

    vector<unsigned int> fZDD_DC_E;
    vector<unsigned int> fZDD_DC_N;
    vector<unsigned long long> fZDD_DC_TS;

    // EXOZDD
    vector<unsigned int> fZDD_EXO_1_E;
    vector<unsigned int> fZDD_EXO_2_E;
    vector<unsigned int> fZDD_EXO_3_E;
    vector<unsigned int> fZDD_EXO_4_E;

    vector<unsigned long long> fZDD_EXO_1_TS;
    vector<unsigned long long> fZDD_EXO_2_TS;
    vector<unsigned long long> fZDD_EXO_3_TS;
    vector<unsigned long long> fZDD_EXO_4_TS;

    vector<unsigned int> fZDD_EXO_E;
    vector<unsigned int> fZDD_EXO_N;
    vector<unsigned long long> fZDD_EXO_TS;

    // DriftTime in DC
    vector<UShort_t> fZDD_Drift_DetectorNbr;
    vector<Double_t> fZDD_DriftTime;
    vector<Double_t> fZDD_X;

    //////////////////////////////////////////////////////////////
    // Constructor and destructor
public:
    TZDDData();
    ~TZDDData();

    //////////////////////////////////////////////////////////////
    // Inherited from TObject and overriden to avoid warnings
public:
    void Clear();
    void Clear(const Option_t*){};
    void Dump() const;

    //////////////////////////////////////////////////////////////
    // Getters and Setters
    // Prefer inline declaration to avoid unnecessary called of
    // frequently used methods
    // add //! to avoid ROOT creating dictionnary for the methods
public:
    //////////////////////    SETTERS    ////////////////////////
    // Energy
    void SetZDDIC(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp);

    void SetZDDPL(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp);

    void SetZDDDC(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp);

    void SetZDDEXO(const unsigned int& DetNumb, const unsigned int& Energy, const unsigned long long& TimeStamp);

    inline void SetDrift(const UShort_t& DetNbr, const Double_t& DriftTime, const Double_t& X) {
        fZDD_Drift_DetectorNbr.push_back(DetNbr);
        fZDD_DriftTime.push_back(DriftTime);
        fZDD_X.push_back(X);
    }; //!



    //////////////////////    GETTERS    ////////////////////////
    //ICs
    inline unsigned int	GetZDD_IC_Mult(int i) const {
        if (i==0){return fZDD_IC_1_E.size();}
        if (i==1){return fZDD_IC_2_E.size();}
        if (i==2){return fZDD_IC_3_E.size();}
        if (i==3){return fZDD_IC_4_E.size();}
        if (i==4){return fZDD_IC_5_E.size();}
        if (i==5){return fZDD_IC_9_E.size();}
        return 0;
    };
    inline unsigned int GetZDD_IC_E(int ps, int i) const {
        if (ps==0){return fZDD_IC_1_E[i];}
        if (ps==1){return fZDD_IC_2_E[i];}
        if (ps==2){return fZDD_IC_3_E[i];}
        if (ps==3){return fZDD_IC_4_E[i];}
        if (ps==4){return fZDD_IC_5_E[i];}
        if (ps==5){return fZDD_IC_9_E[i];}
        return 0;
    };
    inline unsigned long long GetZDD_IC_TS(int ps, int i) const {
        if (ps==0){return fZDD_IC_1_TS[i];}
        if (ps==1){return fZDD_IC_2_TS[i];}
        if (ps==2){return fZDD_IC_3_TS[i];}
        if (ps==3){return fZDD_IC_4_TS[i];}
        if (ps==4){return fZDD_IC_5_TS[i];}
        if (ps==5){return fZDD_IC_9_TS[i];}
        return 0;
    };
    inline unsigned int GetZDD_IC_N(const unsigned int& i) const { return fZDD_IC_N[i]; }

    //PLs
    inline unsigned int	GetZDD_PL_Mult(int i) const {
        if (i==0){return fZDD_PL_1_E.size();}
        if (i==1){return fZDD_PL_2_E.size();}
        if (i==2){return fZDD_PL_3_E.size();}
        if (i==3){return fZDD_PL_4_E.size();}
        if (i==4){return fZDD_PL_5_E.size();}
        if (i==5){return fZDD_PL_6_E.size();}
        if (i==6){return fZDD_PL_7_E.size();}
        if (i==7){return fZDD_PL_8_E.size();}
        if (i==8){return fZDD_PL_9_E.size();}
        if (i==9){return fZDD_PL_10_E.size();}
        return 0;
    };
    inline unsigned int GetZDD_PL_E(int ps, int i) const {
        if (ps==0){return fZDD_PL_1_E[i];}
        if (ps==1){return fZDD_PL_2_E[i];}
        if (ps==2){return fZDD_PL_3_E[i];}
        if (ps==3){return fZDD_PL_4_E[i];}
        if (ps==4){return fZDD_PL_5_E[i];}
        if (ps==5){return fZDD_PL_6_E[i];}
        if (ps==6){return fZDD_PL_7_E[i];}
        if (ps==7){return fZDD_PL_8_E[i];}
        if (ps==8){return fZDD_PL_9_E[i];}
        if (ps==9){return fZDD_PL_10_E[i];}
        return 0;
    };
    inline unsigned long long GetZDD_PL_TS(int ps, int i) const {
        if (ps==0){return fZDD_PL_1_TS[i];}
        if (ps==1){return fZDD_PL_2_TS[i];}
        if (ps==2){return fZDD_PL_3_TS[i];}
        if (ps==3){return fZDD_PL_4_TS[i];}
        if (ps==4){return fZDD_PL_5_TS[i];}
        if (ps==5){return fZDD_PL_6_TS[i];}
        if (ps==6){return fZDD_PL_7_TS[i];}
        if (ps==7){return fZDD_PL_8_TS[i];}
        if (ps==8){return fZDD_PL_9_TS[i];}
        if (ps==9){return fZDD_PL_10_TS[i];}
        return 0;
    };
    inline unsigned int GetZDD_PL_N(const unsigned int& i) const { return fZDD_PL_N[i]; }

    //DCs
        inline unsigned int	GetZDD_DC_Mult(int i) const {
        if (i==0){return fZDD_DC_1_E.size();}
        if (i==1){return fZDD_DC_2_E.size();}
        if (i==2){return fZDD_DC_3_E.size();}
        if (i==3){return fZDD_DC_4_E.size();}
        return 0;
    };
    inline unsigned int GetZDD_DC_E(int ps, int i) const {
        if (ps==0){return fZDD_DC_1_E[i];}
        if (ps==1){return fZDD_DC_2_E[i];}
        if (ps==2){return fZDD_DC_3_E[i];}
        if (ps==3){return fZDD_DC_4_E[i];}
        return 0;
    };
    inline unsigned long long GetZDD_DC_TS(int ps, int i) const {
        if (ps==0){return fZDD_DC_1_TS[i];}
        if (ps==1){return fZDD_DC_2_TS[i];}
        if (ps==2){return fZDD_DC_3_TS[i];}
        if (ps==3){return fZDD_DC_4_TS[i];}
        return 0;
    };
    inline unsigned int GetZDD_DC_N(const unsigned int& i) const { return fZDD_DC_N[i]; }

    //EXOs
        inline unsigned int	GetZDD_EXO_Mult(int i) const {
        if (i==0){return fZDD_EXO_1_E.size();}
        if (i==1){return fZDD_EXO_2_E.size();}
        if (i==2){return fZDD_EXO_3_E.size();}
        if (i==3){return fZDD_EXO_4_E.size();}
        return 0;
    };
    inline unsigned int GetZDD_EXO_E(int ps, int i) const {
        if (ps==0){return fZDD_EXO_1_E[i];}
        if (ps==1){return fZDD_EXO_2_E[i];}
        if (ps==2){return fZDD_EXO_3_E[i];}
        if (ps==3){return fZDD_EXO_4_E[i];}
        return 0;
    };
    inline unsigned long long GetZDD_EXO_TS(int ps, int i) const {
        if (ps==0){return fZDD_EXO_1_TS[i];}
        if (ps==1){return fZDD_EXO_2_TS[i];}
        if (ps==2){return fZDD_EXO_3_TS[i];}
        if (ps==3){return fZDD_EXO_4_TS[i];}
        return 0;
    };
    inline unsigned int GetZDD_EXO_N(const unsigned int& i) const { return fZDD_EXO_N[i]; }


    // inline UShort_t GetMultDrift() const { return fZDD_Drift_DetectorNbr.size(); }
    // inline UShort_t GetDrift_DetectorNbr(const unsigned int& i) const { return fZDD_Drift_DetectorNbr[i];}
    // inline Double_t Get_DriftTime(const unsigned int& i) const { return fZDD_DriftTime[i]; }

    //////////////////////////////////////////////////////////////
    // Required for ROOT dictionnary
    ClassDef(TZDDData, 2) // ZDDData structure
};

#endif