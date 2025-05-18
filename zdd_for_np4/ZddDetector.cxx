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
 * This class is heavily based on the nptool v3 ZDD detector                 *
 *                                                                           *
 *****************************************************************************/
#include "ZddDetector.h"
#include "NPFunction.h"
#include "NPRootPlugin.h"
#include <iostream>
#include "ZddSpectra.h"

using namespace zdd;
using namespace std;
using namespace ROOT::Math;

////////////////////////////////////////////////////////////////////////////////
ZddDetector::ZddDetector() {
  std::cout << "////////////////////////////////////////////// test INIT ////////////////////////////////////////////////" << std::endl;
  m_RawData     = new zdd::ZddData();
  m_CalData     = new zdd::ZddData();
  m_PhysicsData = new zdd::ZddPhysics();
  std::cout << "Adresse phy " << m_PhysicsData << std::endl;
  m_Cal.InitCalibration();
}

ZddDetector::~ZddDetector() {}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::ReadConfiguration(nptool::InputParser parser) {
  auto                     blocks = parser.GetAllBlocksWithToken("zdd");
  std::vector<std::string> token  = {"Type", "Board", "NUMEXOChannel"};

  for (unsigned int i = 0; i < blocks.size(); i++) {
    if (blocks[i]->HasTokenList(token)) {

      std::string  Type  = blocks[i]->GetString("Type");
      unsigned int Board = blocks[i]->GetInt("Board");

      std::vector<int> NUMEXOChannel = blocks[i]->GetVectorInt("NUMEXOChannel");
      unsigned int     NUMEXOChannel_size = NUMEXOChannel.size();

      for (unsigned int j = 0; j < NUMEXOChannel_size; j++) {
        unsigned int index = Board * 1e3 + (unsigned int)NUMEXOChannel[j];
        SetNumexoMap(Type, Board, (unsigned int)NUMEXOChannel[j]);
        // m_BoardDataTypeMap[index] = "ZDD";
      }
    }
  }

  ReadAnalysisConfig();
}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::SetNumexoMap(std::string Type, unsigned int Board,
                               unsigned int NUMEXOChannel) {
  unsigned int index = Board * 1e3 + NUMEXOChannel;
  m_NumexoMap[index] = Type;
}

////////////////////////////////////////////////////////////////////////////////
std::string ZddDetector::GetTypeFromNumexoMap(unsigned int Board,
                                              unsigned int Channel) {
  unsigned int index = Board * 1e3 + Channel;
  std::string  Type  = m_NumexoMap[index];
  return Type;
}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::BuildRawEvent(const std::string& daq_name,
                                const std::string& st_type_key,
                                void*              commonframe) {
#ifdef MFM_FOUND
  int type_key = ((MFMCommonFrame*)commonframe)->GetFrameType();
  if (type_key == MFM_REA_GENE_FRAME_TYPE) {
    TreatFrame((MFMCommonFrame*)commonframe);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::TreatFrame(void* commonframe) {
#ifdef MFM_FOUND

  std::shared_ptr<MFMReaGenericFrame> ReaFrame
      = std::make_shared<MFMReaGenericFrame>();
  ReaFrame->SetAttributs(((MFMCommonFrame*)commonframe)->GetPointHeader());

  int                Board   = -1;
  int                Channel = -1;
  unsigned short     E       = 0;
  unsigned long long TS      = 0;
  int                Time    = -1;

  Board   = ReaFrame->GetBoardId();
  Channel = ReaFrame->GetTGCristalId();
  E       = ReaFrame->GetEnergy();
  TS      = ReaFrame->GetTimeStamp();
  Time    = ReaFrame->GetTime();

  std::string Type = GetTypeFromNumexoMap(Board, Channel);

  //int i = 0;

  if (Type == "PL") {
    m_RawData->SetZDDPL(Channel, E, TS);
    //i++;
   //std::cout << " PL " << i << std::endl;
  } else if (Type == "IC") {
    m_RawData->SetZDDIC(Channel, E, TS);
    //i++;
    //std::cout << " IC " << i << std::endl;
  } else if (Type == "DC") {
    m_RawData->SetZDDDC(Channel, E, TS);
    //i++;
    //std::cout << " DC " << i <<std::endl;
  } else if (Type == "EXO") {
    m_RawData->SetZDDEXO(Channel, E, TS, Time);
    //std::cout << " EX ";
  }


#endif
}

void ZddDetector::BuildPhysicalEvent() {

  //   if (NPOptionManager::getInstance()->IsReader() == true) {
  //     m_RawData = &(**r_ReaderEventData);
  //   }

  // apply thresholds and calibration
  PreTreat();

  // match energy and time together
  Match_IC1();
  // if (m_PhysicsData->IC_Nbr.size() > 0)
  Match_PL();

  Treat_Exo();
  Match_DC();
  SortIC.clear();
  SortPL.clear();
  SortDC.clear();
}

///////////////////////////////////////////////////////////////////////////
void ZddDetector::Treat_Exo() {

  unsigned int size = m_CalData->GetZDD_EXOMult();
  for(unsigned int i = 0 ; i < size ; i++){
    m_PhysicsData->Exo_Nbr.push_back(m_CalData->GetZDD_EXON(i));
    m_PhysicsData->Exo_E.push_back(m_CalData->GetZDD_EXOE(i));
    m_PhysicsData->Exo_TS.push_back(m_CalData->GetZDD_EXOTS(i));
  }
}

///////////////////////////////////////////////////////////////////////////
void ZddDetector::Match_DC(){
    m_PhysicsData->DC_Nbr.clear();
    m_PhysicsData->DC_E.clear();
    m_PhysicsData->DC_TS.clear();
    for(auto it = SortDC.begin(); it != SortDC.end(); ++it){
      for (auto& data: it->second){
          m_PhysicsData->DC_Nbr.push_back(it->first);
          m_PhysicsData->DC_E.push_back(data.first);
          m_PhysicsData->DC_TS.push_back(data.second);
      }
    }
}
///////////////////////////////////////////////////////////////////////////
void ZddDetector::Match_IC() {
  //////////////////////////////// Currently Simply matching if mult = 5, could
  /// be improved to treat mult > 5

  if (m_CalData->GetZDD_ICMult() == 5) {
    // CHecking that each IC is only encountered once and then sorting them in
    // the right order with the map
    for (unsigned int i = 0; i < m_CalData->GetZDD_ICMult(); i++) {
      if (SortIC.find(m_CalData->GetZDD_ICN(i)) != SortIC.end()) {
        SortIC.clear();
        break;
      }
      SortIC[m_CalData->GetZDD_ICN(i)]
        = std::make_pair(m_CalData->GetZDD_ICE(i), m_CalData->GetZDD_ICTS(i));
    }
    // Adding the IC info to the std::vectors
    m_PhysicsData->ICSum = 0;
    for (auto it = SortIC.begin(); it != SortIC.end(); ++it) {
      m_PhysicsData->ICSum += (it->second).first;
      m_PhysicsData->IC_Nbr.push_back(it->first);
      m_PhysicsData->IC_E.push_back((it->second).first);
      m_PhysicsData->IC_TS.push_back((it->second).second);
    }
  }
}

void ZddDetector::Match_IC1() {
  //////////////////////////////// Currently Simply matching if mult = 5, could
  /// be improved to treat mult > 5
  // CHecking that each IC is only encountered once and then sorting them in the
  // right order with the map
  for (unsigned int i = 0; i < m_CalData->GetZDD_ICMult(); i++) {
    if (SortIC.find(m_CalData->GetZDD_ICN(i)) == SortIC.end()) {
      SortIC[m_CalData->GetZDD_ICN(i)]
        = std::make_pair(m_CalData->GetZDD_ICE(i), m_CalData->GetZDD_ICTS(i));
    }
  }
  // Adding the IC info to the std::vectors
  m_PhysicsData->ICSum = 0;
  for (auto it = SortIC.begin(); it != SortIC.end(); ++it) {
    m_PhysicsData->ICSum += (it->second).first;
    m_PhysicsData->IC_Nbr.push_back(it->first);
    m_PhysicsData->IC_E.push_back((it->second).first);
    m_PhysicsData->IC_TS.push_back((it->second).second);
    // std::cout << (it->second).first << std::endl;
  }
}

void ZddDetector::Match_PL() {
  for (unsigned int i = 0; i < m_CalData->GetZDD_PLMult(); i++) {
    SortPL[m_CalData->GetZDD_PLN(i)%5].push_back(std::make_pair(m_CalData->GetZDD_PLE(i), m_CalData->GetZDD_PLTS(i)));
  }
  for (auto it = SortPL.begin(); it != SortPL.end(); ++it) {
    if(it->second.size()==2){
    m_PhysicsData->PL_Nbr.push_back(it->first);
    m_PhysicsData->PL_E.push_back(sqrt((it->second)[0].first*(it->second)[1].first));
    m_PhysicsData->PL_TS.push_back(((it->second)[0].second +(it->second)[1].second)/2);
    }
  }
}

void ZddDetector::PreTreat() {
  ClearPreTreatedData();
  m_IC_Mult  = m_RawData->GetZDD_ICMult();
  m_PL_Mult  = m_RawData->GetZDD_PLMult();
  m_DC_Mult  = m_RawData->GetZDD_DCMult();
  m_EXO_Mult = m_RawData->GetZDD_EXOMult();

  //if(m_RawData->GetZDD_DCMult()>0){
  //std::cout << "before = " << m_RawData->GetZDD_DCMult() << " " << m_CalData->GetZDD_DCMult() << std::endl;
  //}

  for (unsigned int i = 0; i < m_IC_Mult; ++i) {
    if (m_RawData->GetZDD_ICE(i) > m_IC_E_RAW_Threshold) {
      // std::cout << m_RawData->GetZDD_ICN(i) << " " << m_RawData->GetZDD_ICE(i)
      //           << " " << Map_IC[m_RawData->GetZDD_ICN(i)] << std::endl;
      // std::cout << m_IC_E_RAW_Threshold << std::endl;
      m_CalData->SetZDDIC(Map_IC[m_RawData->GetZDD_ICN(i)],
          m_RawData->GetZDD_ICE(i), m_RawData->GetZDD_ICTS(i));
    }
  }

  for (unsigned int i = 0; i < m_PL_Mult; ++i) {
    if (m_RawData->GetZDD_PLE(i) > m_PL_E_RAW_Threshold) {
      m_CalData->SetZDDPL(m_RawData->GetZDD_PLN(i), m_RawData->GetZDD_PLE(i),
          m_RawData->GetZDD_PLTS(i));
    }
  }

//std:: cout << " PS TESTING:::: AM I HERE??" << std::endl;

  for (unsigned int i = 0; i < m_DC_Mult; ++i) {
    if (m_RawData->GetZDD_DCE(i) > m_DC_E_RAW_Threshold) {
      m_CalData->SetZDDDC(m_RawData->GetZDD_DCN(i), m_RawData->GetZDD_DCE(i),
          m_RawData->GetZDD_DCTS(i));
    }
  }
  SortDC.clear();
  for(int ps = 0; ps<4; ps++){
    for (int i = 0; i<m_RawData->GetZDD_DC_Mult(ps); i++){
      if (m_RawData->GetZDD_DC_E(ps,i)>m_DC_E_RAW_Threshold){
            SortDC[ps].push_back(std::make_pair(m_RawData->GetZDD_DC_E(ps,i), m_RawData->GetZDD_DC_TS(ps,i)));
      }
    }
  }

  for (unsigned int i = 0; i < m_EXO_Mult; ++i) {
    // if (m_RawData->GetZDD_EXOE(i) > m_EXO_E_RAW_Threshold) {
    // std::cout << fZDDEXO_E(i) << std::endl;
    m_CalData->SetZDDEXO(m_RawData->GetZDD_EXON(i), fZDDEXO_E(i),
        m_RawData->GetZDD_EXOTS(i), m_RawData->GetZDD_EXOTime(i));
    // }
  }
  
  //if(m_RawData->GetZDD_DCMult()>0){
  //std::cout << "after = " << m_RawData->GetZDD_DCMult() << " " << m_CalData->GetZDD_DCMult() << std::endl;
  //}
  
}

///////////////////////////////////////////////////////////////////////////
void ZddDetector::PreTreatEnergy(std::string Detector) {
  /*unsigned int mysize = m_RawData->GetMultEnergy(Detector);
    for (UShort_t i = 0; i < mysize ; ++i) {
    if (m_RawData->Get_Energy(Detector, i) > m_E_RAW_Threshold) {
    Double_t Energy =
    Cal->ApplyCalibration("ZDD/ENERGY"+nptool::itoa(m_RawData->GetE_DetectorNbr(Detector,
    i)),m_RawData->Get_Energy(Detector, i)); if (Energy > m_E_Threshold) {
    if(Detector == "IC")
    m_CalData->Set_IC_Energy(m_RawData->GetE_DetectorNbr(Detector, i),
    Energy);

    else if(Detector == "Plastic")
    m_CalData->Set_Plastic_Energy(m_RawData->GetE_DetectorNbr(Detector,
    i), Energy);
    }
    }
    }
    */
}

///////////////////////////////////////////////////////////////////////////
void ZddDetector::PreTreatTime(std::string Detector) {
  /*  unsigned int mysize = m_RawData->GetMultTime(Detector);
      for (UShort_t i = 0; i < mysize; ++i) {
      Double_t Time=
      Cal->ApplyCalibration("ZDD/TIME"+nptool::itoa(m_RawData->GetT_DetectorNbr(Detector,
      i)),m_RawData->Get_Time(Detector,i)); if(Detector == "IC")
      m_CalData->Set_IC_Time(m_RawData->GetT_DetectorNbr(Detector, i), Time);
      else if(Detector == "Plastic")
      m_CalData->Set_Plastic_Time(m_RawData->GetT_DetectorNbr(Detector, i),
      Time); else if(Detector == "DC")
      m_CalData->Set_DC_Time(m_RawData->GetT_DetectorNbr(Detector, i), Time);
      }
      */
}

///////////////////////////////////////////////////////////////////////////
void ZddDetector::ReadAnalysisConfig() {
  bool ReadingStatus = false;

  // path to file
  string FileName = "./configs/ConfigZDD.dat";

  // open analysis config file
  ifstream AnalysisConfigFile;
  AnalysisConfigFile.open(FileName.c_str());

  if (!AnalysisConfigFile.is_open()) {
    cout << " No ConfigZDD.dat found: Default parameter loaded for Analayis "
      << FileName << endl;
    return;
  }
  cout << " Loading user parameter for Analysis from ConfigZDD.dat " << endl;

  //// Save it in a TAsciiFile
  // TAsciiFile* asciiConfig =
  // RootOutput::getInstance()->GetAsciiFileAnalysisConfig();
  // asciiConfig->AppendLine("%%% ConfigZDD.dat %%%");
  // asciiConfig->Append(FileName.c_str());
  // asciiConfig->AppendLine("");
  //  read analysis config file
  string LineBuffer, DataBuffer, whatToDo;
  while (!AnalysisConfigFile.eof()) {
    // Pick-up next line
    getline(AnalysisConfigFile, LineBuffer);

    // search for "header"
    string name = "ConfigZDD";
    if (LineBuffer.compare(0, name.length(), name) == 0)
      ReadingStatus = true;

    // loop on tokens and data
    while (ReadingStatus) {
      whatToDo = "";
      AnalysisConfigFile >> whatToDo;

      // Search for comment symbol (%)
      if (whatToDo.compare(0, 1, "%") == 0) {
        AnalysisConfigFile.ignore(numeric_limits<streamsize>::max(), '\n');
      }

      else if (whatToDo == "IC_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_IC_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_IC_E_RAW_Threshold << endl;
      } else if (whatToDo == "PL_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_PL_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_PL_E_RAW_Threshold << endl;
      } else if (whatToDo == "DC_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_DC_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_DC_E_RAW_Threshold << endl;
      } else if (whatToDo == "EXO_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_EXO_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_EXO_E_RAW_Threshold << endl;
      } else if (whatToDo == "MAP_IC") {
        AnalysisConfigFile >> DataBuffer;
        Map_IC[atoi(DataBuffer.substr(0, 1).c_str())]
          = atoi(DataBuffer.substr(1, 1).c_str());
        cout << whatToDo << " " << atoi(DataBuffer.substr(0, 1).c_str()) << " "
          << atoi(DataBuffer.substr(1, 1).c_str()) << endl;
      } else if (whatToDo == "E_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_E_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_E_Threshold << endl;
      }
      else {
        ReadingStatus = false;
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
//	tranform an integer to a string
double ZddDetector::fZDDEXO_E(const unsigned int& i) {
  static string name;
  name = "ZDDEXO_E";
  name += nptool::itoa(m_RawData->GetZDD_EXON(i));
  return m_Cal.ApplyCalibration(name, m_RawData->GetZDD_EXOE(i), 1);
}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::InitializeDataInputRaw(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("zdd", "zdd::ZddData", &m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::InitializeDataInputPhysics(
    std::shared_ptr<nptool::VDataInput> input) {
  // input->Attach("zdd", "zdd::ZddData", &m_RawData);
  input->Attach("zdd", "zdd::ZddPhysics", &m_PhysicsData);
}
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::InitializeDataOutputRaw(
    std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("zdd", "zdd::ZddData", &m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::InitializeDataOutputPhysics(
    std::shared_ptr<nptool::VDataOutput> output) {
  // output->Attach("zdd", "zdd::ZddData", &m_RawData);
  output->Attach("zdd", "zdd::ZddPhysics", &m_PhysicsData);
}

////////////////////////////////////////////////////////////////////////////////
void ZddDetector::InitSpectra() {
  m_Spectra = std::make_shared<zdd::ZddSpectra>();
};
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::FillSpectra() {
  auto app = nptool::Application::GetApplication();
  
  if(app->HasFlag("--input-raw"))
    m_Spectra->FillRaw();
  if(app->HasFlag("--input-phy"))
    m_Spectra->FillPhy();
};
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::WriteSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::CheckSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void ZddDetector::ClearSpectra() { m_Spectra->Clear();};


////////////////////////////////////////////////////////////////////////////////
extern "C" {
  shared_ptr<nptool::VDetector> ConstructDetector() {
    return make_shared<zdd::ZddDetector>();
  };
}
