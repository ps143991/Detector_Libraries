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
 *  This class hold ZDD Treated  data                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "TZDDPhysics.h"

//   STL
#include <sstream>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <limits>
using namespace std;

//   NPL
#include "RootInput.h"
#include "RootOutput.h"
#include "NPDetectorFactory.h"
#include "NPOptionManager.h"

//   ROOT
#include "TChain.h"

ClassImp(TZDDPhysics)


///////////////////////////////////////////////////////////////////////////
TZDDPhysics::TZDDPhysics()
   : m_EventData(new TZDDData),
     m_PreTreatedData(new TZDDCalData),
     TSEvent(new TimeStamp),
     m_EventPhysics(this),
     m_Spectra(0),
     m_IC_E_RAW_Threshold(0), // adc channels
     m_PL_E_RAW_Threshold(0), // adc channels
     m_DC_E_RAW_Threshold(0), // adc channels
     m_EXO_E_RAW_Threshold(0), // adc channels
     m_E_Threshold(0),     // MeV
     m_NumberOfDetectors(0)
      {
}


///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::BuildSimplePhysicalEvent() {
  BuildPhysicalEvent();
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::BuildPhysicalEvent() {  //27a: event building in zdd
  //std::cout<<"build physical event from ZDD physics running"<<std::endl;
  ClaimReaderData();  //27b1: starts reading NPRaw_data
  // apply thresholds and calibration
  PreTreat(); //27c1:

  // match energy and time together
  Match_IC2();

  Match_PL();
  Match_DC();
  Match_EXO();
}


///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::ReadConfigurationTS(){
  TSEvent->ReadConfigurationFile();
}

///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::SetRefTS(std::string TSRef_Name, unsigned long long TSRef){
  RefTS = TSRef;
  RefTS_Name = TSRef_Name;
}

///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::Match_DC(){
    DC_Nbr.clear();
    DC_E.clear();
    DC_TS.clear();
    for(auto it = SortDC.begin(); it != SortDC.end(); ++it){
      for (auto& data: it->second){
          DC_Nbr.push_back(it->first);
          DC_E.push_back(data.first);
          DC_TS.push_back(data.second);
      }
 }
}
///////////////////////////////////////////////////////////////////////////

void TZDDPhysics::Match_IC2() {
    IC_Nbr.clear();
    IC_E.clear();
    IC_TS.clear();

    ICSum = 0;
    SUMofIC_PS = 0;
    for (auto it = SortIC2.begin(); it != SortIC2.end(); ++it) {
        for (auto& data : it->second) { // Iterate over all stored values per IC number
            ICSum += data.first;
            IC_Nbr.push_back(it->first);
            IC_E.push_back(data.first);
            IC_TS.push_back(data.second);
        }
    }
}

void TZDDPhysics::Match_PL(){
    PL_Nbr.clear();
    PL_E.clear();
    PL_TS.clear();
    for(auto it = SortPL.begin(); it != SortPL.end(); ++it){
      for (auto& data: it->second){
          PL_Nbr.push_back(it->first);
          PL_E.push_back(data.first);
          PL_TS.push_back(data.second);
      }
 }
}

void TZDDPhysics::Match_EXO(){
    EXO_Nbr.clear();
    EXO_E.clear();
    EXO_TS.clear();
    for(auto it = SortEXO.begin(); it != SortEXO.end(); ++it){
      for (auto& data: it->second){
          EXO_Nbr.push_back(it->first);
          EXO_E.push_back(data.first);
          EXO_TS.push_back(data.second);
      }
 }
}

///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::PreTreat() {

  // clear pre-treated object
  ClearPreTreatedData();

//ICs
  SortIC2.clear();

  for (int ps = 0; ps<6; ps++){ //0->5 5CHIOs ZDD + 1 CHIO E9
    for (int i=0; i<m_EventData->GetZDD_IC_Mult(ps); i++){  //[0-PS]th det. : 0,1,2,size(0th det)
      if (m_EventData->GetZDD_IC_E(ps, i) > m_IC_E_RAW_Threshold) {
        if(!RefTS_Name.empty()){
          std::string TSName = "IC_"+std::to_string(ps);
          TSEvent->AddTimeStamp(TSName,m_EventData->GetZDD_IC_TS(ps,i));
          TSEvent->AddTimeStamp(RefTS_Name, RefTS);

          if (TSEvent->MatchTS(TSName)){
                //Trying feeding directly to a map!!
                SortIC2[ps].push_back(std::make_pair(m_EventData->GetZDD_IC_E(ps,i), m_EventData->GetZDD_IC_TS(ps,i)));
          }
          TSEvent->ClearTimeStamps();
        }
        // else, all datas are filled
        else{
          //m_PreTreatedData->SetZDDIC(ps, m_EventData->GetZDD_IC_E(ps,i), m_EventData->GetZDD_IC_TS(ps,i));
          SortIC2[ps].push_back(std::make_pair(m_EventData->GetZDD_IC_E(ps,i), m_EventData->GetZDD_IC_TS(ps,i)));
        }
      }
    }
  }

//PL

  SortPL.clear();
  for(int ps = 0; ps<10; ps++){
    for (int i = 0; i<m_EventData->GetZDD_PL_Mult(ps); i++){
      if (m_EventData->GetZDD_PL_E(ps,i) > m_PL_E_RAW_Threshold){
        if(!RefTS_Name.empty()){
          std::string TSName = "PL_"+std::to_string(ps);
          TSEvent->AddTimeStamp(TSName,m_EventData->GetZDD_PL_TS(ps,i));
          TSEvent->AddTimeStamp(RefTS_Name, RefTS);

          if (TSEvent->MatchTS(TSName)){
            //m_PreTreatedData->SetZDDPL(ps,m_EventData->GetZDD_PL_E(ps,i), m_EventData->GetZDD_PL_TS(ps,i));
            SortPL[ps].push_back(std::make_pair(m_EventData->GetZDD_PL_E(ps,i), m_EventData->GetZDD_PL_TS(ps,i)));
          }
          TSEvent->ClearTimeStamps();
        }
        // else, all datas are filled
        else{
          //m_PreTreatedData->SetZDDPL(ps, m_EventData->GetZDD_PL_E(ps,i), m_EventData->GetZDD_PL_TS(ps,i));
          SortPL[ps].push_back(std::make_pair(m_EventData->GetZDD_PL_E(ps,i), m_EventData->GetZDD_PL_TS(ps,i)));
        }
      }
    }
  }
//DCs
  SortDC.clear();
  for(int ps = 0; ps<4; ps++){
    for (int i = 0; i<m_EventData->GetZDD_DC_Mult(ps); i++){
      if (m_EventData->GetZDD_DC_E(ps,i)>m_DC_E_RAW_Threshold){
        /*if(!RefTS_Name.empty()){
          std::string TSName = "DC_"+std::to_string(ps);
          TSEvent->AddTimeStamp(TSName,m_EventData->GetZDD_DC_TS(ps,i));
          TSEvent->AddTimeStamp(RefTS_Name, RefTS);

          if (TSEvent->MatchTS(TSName)){
            m_PreTreatedData->SetZDDDC(ps,m_EventData->GetZDD_DC_E(ps,i), m_EventData->GetZDD_DC_TS(ps,i));
          }
          TSEvent->ClearTimeStamps();
        }
        // else, all datas are filled
        else{*/
          //m_PreTreatedData->SetZDDDC(ps, m_EventData->GetZDD_DC_E(ps,i), m_EventData->GetZDD_DC_TS(ps,i));
            SortDC[ps].push_back(std::make_pair(m_EventData->GetZDD_DC_E(ps,i), m_EventData->GetZDD_DC_TS(ps,i)));
        //}
      }
    }
  }

//EXOZDD
  SortEXO.clear();
  for(int ps = 0; ps<4; ps++){
    for (int i = 0; i<m_EventData->GetZDD_EXO_Mult(ps); i++){
      if (m_EventData->GetZDD_EXO_E(ps,i)>m_EXO_E_RAW_Threshold){
        /*if(!RefTS_Name.empty()){
          std::string TSName = "EXO_"+std::to_string(ps);
          TSEvent->AddTimeStamp(TSName,m_EventData->GetZDD_EXO_TS(ps,i));
          TSEvent->AddTimeStamp(RefTS_Name, RefTS);

          if (TSEvent->MatchTS(TSName)){
            m_PreTreatedData->SetZDDEXO(ps,m_EventData->GetZDD_EXO_E(ps,i), m_EventData->GetZDD_EXO_TS(ps,i));
          }
          TSEvent->ClearTimeStamps();
        }
        // else, all datas are filled
        else{*/
          //m_PreTreatedData->SetZDDEXO(ps, m_EventData->GetZDD_EXO_E(ps,i), m_EventData->GetZDD_EXO_TS(ps,i));
          SortEXO[ps].push_back(std::make_pair(m_EventData->GetZDD_EXO_E(ps,i), m_EventData->GetZDD_EXO_TS(ps,i)));
        //}
      }
    }
  }

  ClearTSRef();
}


///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::ReadAnalysisConfig() {
  bool ReadingStatus = false;

  // path to file
  string FileName = "./configs/ConfigZDD.dat";

  // open analysis config file
  ifstream AnalysisConfigFile;
  AnalysisConfigFile.open(FileName.c_str());

  if (!AnalysisConfigFile.is_open()) {
    cout << " No ConfigZDD.dat found: Default parameter loaded for Analayis " << FileName << endl;
    return;
  }
  cout << " Loading user parameter for Analysis from ConfigZDD.dat " << endl;

  // Save it in a TAsciiFile
  TAsciiFile* asciiConfig = RootOutput::getInstance()->GetAsciiFileAnalysisConfig();
  asciiConfig->AppendLine("%%% ConfigZDD.dat %%%");
  asciiConfig->Append(FileName.c_str());
  asciiConfig->AppendLine("");
  // read analysis config file
  string LineBuffer,DataBuffer,whatToDo;
  while (!AnalysisConfigFile.eof()) {
    // Pick-up next line
    getline(AnalysisConfigFile, LineBuffer);

    // search for "header"
    string name = "ConfigZDD";
    if (LineBuffer.compare(0, name.length(), name) == 0)
      ReadingStatus = true;

    // loop on tokens and data
    while (ReadingStatus ) {
      whatToDo="";
      AnalysisConfigFile >> whatToDo;

      // Search for comment symbol (%)
      if (whatToDo.compare(0, 1, "%") == 0) {
        AnalysisConfigFile.ignore(numeric_limits<streamsize>::max(), '\n' );
      }

      else if (whatToDo=="IC_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_IC_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_IC_E_RAW_Threshold << endl;
      }
      else if (whatToDo=="PL_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_PL_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_PL_E_RAW_Threshold << endl;
      }
      else if (whatToDo=="DC_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_DC_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_DC_E_RAW_Threshold << endl;
      }
      else if (whatToDo=="EXO_E_RAW_THRESHOLD") {
        AnalysisConfigFile >> DataBuffer;
        m_EXO_E_RAW_Threshold = atof(DataBuffer.c_str());
        cout << whatToDo << " " << m_EXO_E_RAW_Threshold << endl;
      }
      else if (whatToDo=="MAP_IC") {
        AnalysisConfigFile >> DataBuffer;
        Map_IC[atoi(DataBuffer.substr(0,1).c_str())] = atoi(DataBuffer.substr(1,1).c_str());
        cout << whatToDo << " " << atoi(DataBuffer.substr(0,1).c_str()) << " " << atoi(DataBuffer.substr(1,1).c_str()) << endl;
      }
      else if (whatToDo=="E_THRESHOLD") {
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



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::Clear() {
  ICSum = 0;
  SortIC2.clear();
  SortPL.clear();
  IC_Nbr.clear();
  IC_E.clear();
  IC_TS.clear();
  PL_Nbr.clear();
  PL_E.clear();
  PL_TS.clear();
  DC_DetectorNumber.clear();
  DC_DriftTime.clear();

}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::ReadConfiguration(NPL::InputParser parser){

  vector<NPL::InputBlock*> blocks = parser.GetAllBlocksWithToken("ZDD");
  if(NPOptionManager::getInstance()->GetVerboseLevel())
    cout << "//// " << blocks.size() << " detectors found " << endl;

  vector<string> TokenZDD  = {"R", "Theta"};



  for(unsigned int i = 0 ; i < blocks.size() ; i++){
    if (blocks[i]->HasTokenList(TokenZDD)) {
        if (NPOptionManager::getInstance()->GetVerboseLevel())
            cout << endl << "////  ZDD " << i + 1 << endl;
        double R     = blocks[i]->GetDouble("R", "mm");
        double Theta = blocks[i]->GetDouble("Theta", "deg");

        Add_ZDD(R, Theta);
    }
    else{
      cout << "ERROR: check your input file formatting " << endl;
      exit(1);
    }
  }
  ReadAnalysisConfig();
}

///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::ClaimReaderData() {
    //std::cout<<"build physical event from TAC physics running::::: claim reader data????"<<std::endl;

  if (NPOptionManager::getInstance()->IsReader() == true) {
        //std::cout<<"build physical event from TAC physics running::::: claim reader data????  YESSS"<<std::endl;

    m_EventData = &(**r_ReaderEventData); //27b: comes from TZDDPhysicsReader on line 495 of this code m_EventData is the event from raw datat
  }
}


///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::InitSpectra() {
  m_Spectra = new TZDDSpectra(m_NumberOfDetectors);
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::FillSpectra() {
  // m_Spectra -> FillRawSpectra(m_EventData);
  // m_Spectra -> FillPreTreatedSpectra(m_PreTreatedData);
  // m_Spectra -> FillPhysicsSpectra(m_EventPhysics);
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::CheckSpectra() {
  m_Spectra->CheckSpectra();
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::ClearSpectra() {
  // To be done
}



///////////////////////////////////////////////////////////////////////////
map< string , TH1*> TZDDPhysics::GetSpectra() {
  if(m_Spectra)
    return m_Spectra->GetMapHisto();
  else{
    map< string , TH1*> empty;
    return empty;
  }
}

///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::WriteSpectra() {
  m_Spectra->WriteSpectra();
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::AddParameterToCalibrationManager() {
  CalibrationManager* Cal = CalibrationManager::getInstance();
  for (int i = 0; i < m_NumberOfDetectors; ++i) {
    Cal->AddParameter("ZDD", "D"+ NPL::itoa(i+1)+"_ENERGY","ZDD_D"+ NPL::itoa(i+1)+"_ENERGY");
    Cal->AddParameter("ZDD", "D"+ NPL::itoa(i+1)+"_TIME","ZDD_D"+ NPL::itoa(i+1)+"_TIME");
  }
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::InitializeRootInputRaw() {
  TChain* inputChain = RootInput::getInstance()->GetChain();
  // Option to use the nptreereader anaysis
  if (NPOptionManager::getInstance()->IsReader() == true) {
    TTreeReader* inputTreeReader = RootInput::getInstance()->GetTreeReader();
    inputTreeReader->SetTree(inputChain);
  }
  // Option to use the standard npanalysis
  else{
  inputChain->SetBranchStatus("ZDD",  true );
  inputChain->SetBranchAddress("ZDD", &m_EventData );
  }
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::InitializeRootInputPhysics() {
  TChain* inputChain = RootInput::getInstance()->GetChain();
  // Option to use the nptreereader anaysis
  if (NPOptionManager::getInstance()->IsReader() == true) {
    TTreeReader* inputTreeReader = RootInput::getInstance()->GetTreeReader();
    inputTreeReader->SetTree(inputChain);
  }
  // Option to use the standard npanalysis
  else{
  inputChain->SetBranchStatus("ZDD",  true );
  inputChain->SetBranchAddress("ZDD", &m_EventPhysics);
  }
}



///////////////////////////////////////////////////////////////////////////
void TZDDPhysics::InitializeRootOutput() {
  TTree* outputTree = RootOutput::getInstance()->GetTree();
  outputTree->Branch("ZDD", "TZDDPhysics", &m_EventPhysics);
}

void TZDDPhysics::SetTreeReader(TTreeReader* TreeReader) {
   TZDDPhysicsReader::r_SetTreeReader(TreeReader);  //27c: reads data from TZDDData means from NPRaw_ files
 }


////////////////////////////////////////////////////////////////////////////////
//            Construct Method to be pass to the DetectorFactory              //
////////////////////////////////////////////////////////////////////////////////
NPL::VDetector* TZDDPhysics::Construct() {
  return (NPL::VDetector*) new TZDDPhysics();
}

NPL::VTreeReader* TZDDPhysics::ConstructReader() { return (NPL::VTreeReader*)new TZDDPhysicsReader(); }



////////////////////////////////////////////////////////////////////////////////
//            Registering the construct method to the factory                 //
////////////////////////////////////////////////////////////////////////////////
extern "C"{
class proxy_ZDD{
  public:
    proxy_ZDD(){
      NPL::DetectorFactory::getInstance()->AddToken("ZDD","ZDD");
      NPL::DetectorFactory::getInstance()->AddDetector("ZDD",TZDDPhysics::Construct);
      NPL::DetectorFactory::getInstance()->AddDetectorReader("ZDD", TZDDPhysics::ConstructReader);
    }
};

proxy_ZDD p_ZDD;
}