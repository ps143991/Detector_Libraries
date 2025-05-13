/*****************************************************************************
 * Original Author: V. Girard-Alcindor                                       *
 * contact address: girard-alcindor@ijclab.in2p3.fr                          *
 *                                                                           *
 * Creation Date  :    08/03/24                                              *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *****************************************************************************/
#ifndef TacSpectra_h
#define TacSpectra_h
// nebula-plus
#include "TacDetector.h"
// root
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
// std
#include <map>
#include <memory>
namespace tac {

  // forward declaration is necessary
  // class TacDetector;
  class TacSpectra {
   public:
    TacSpectra();
    ~TacSpectra(){};

   private:
    std::shared_ptr<tac::TacDetector> m_detector;
    tac::TacData* m_RawData;
    tac::TacPhysics* m_PhysicsData;
    std::map<std::string,TH2*> m_raw_hist;
    std::map<std::string,unsigned int> m_bin_label;
    std::map<std::string,std::vector<std::pair<unsigned int,std::string>>> m_raw_channels;
    std::map<std::string, std::pair<unsigned int, TH1*>> m_phy_hist;
    Double_t TAC_PL_1;
    Double_t TAC_PL_2;
    Double_t TAC_PL_3;
    Double_t TAC_PL_4;
    Double_t TAC_PL_5;
    Double_t norm_length;
    Double_t old_L_Bins[5]={19617,19836,20106,19857,19395}; //lower bins of shifted spectra of TACs to be changed by PS
    Double_t L_Bins[5] = {222,441,711,462,0};               //lower bins subtracted by the lowest bin of all low bins of 5 plastics (the 5th one)
    Double_t old_U_Bins[5] = {26541,26322,26052,26301,26763}; //upper bins of shifted spectra of TACs
    Double_t perbin_value[5] = {346.20000, 324.30000, 297.30000, 322.20000, 368.40000} ; //equal to old_U_Bins - old_L_Bins/20 (PLASTIC LENGTH)
    


   public:
    void FillRaw();
    void FillPhy();
    void Clear();
  };

} // namespace tac
#endif
