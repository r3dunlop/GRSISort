#include <TMath.h>
#include "TCSM.h"


ClassImp(TCSM)

// various csm dimensions in mm
//const int TCSM::frontstripslist[16]     = {16,16,16,16,	24,24,24,24,	24,24,24,24,	16,16,16,16};
//const int TCSM::backstripslist[16]      = {24,24,24,24,	48,48,48,48,	48,48,48,48,	24,24,24,24};
//const double TCSM::frontpitchlist[16]   = {2.0,2.0,2.0,2.0,	3.0,3.0,3.0,3.0,	3.0,3.0,3.0,3.0,	2.0,2.0,2.0,2.0};
//const double TCSM::backpitchlist[16]    = {PI/48,PI/48,PI/48,PI/48,	1.0,1.0,1.0,1.0,	1.0,1.0,1.0,1.0,	PI/48,PI/48,PI/48,PI/48};    // QQQ back pitches are angles
//const double TCSM::stripFpitch          = TCSM::Ydim / TCSM::frontstripslist[5]; // 72.0/24 = 3.0 mm
//const double TCSM::ringpitch            = TCSM::Rdim / TCSM::frontstripslist[1]; // 32.0/16 = 2.0 mm
//const double TCSM::stripBpitch          = TCSM::Zdim / TCSM::backstripslist[5] ; // 48.0/48 = 1.0 mm
//const double TCSM::segmentpitch         = TCSM::Pdim / TCSM::backstripslist[1] ; // 81.6/24 = 3.4 degrees (angular pitch)


//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//
//==========================================================================//

int TCSM::fCfdBuildDiff = 5;

TCSM::TCSM() : data(0)
{
  Class()->IgnoreTObjectStreamer(true);
  //InitializeSRIMInputs();
}

TCSM::~TCSM()
{
  if(data) delete data;
}




void TCSM::FillData(TFragment *frag,TChannel *channel,MNEMONIC *mnemonic)
{
  if(!data)
    data = new TCSMData();

  if(mnemonic->collectedcharge.compare(0,1,"N")==0)    //Horizontal Strips. aka "front"
  {
    data->SetHorizontal(frag,channel,mnemonic);
    //cout<<":";
  }
  else if(mnemonic->collectedcharge.compare(0,1,"P")==0)      //Vertical Strips. aka "back"
  {
    data->SetVertical(frag,channel,mnemonic);
    //cout<<".";
  }
}

void	TCSM::BuildHits(TGRSIDetectorData *ddata, Option_t *opt)
{
  TCSMData *cdata = (TCSMData *)ddata;

  if(cdata==0)
    cdata = (this->data);

  if(!cdata)
    return;

  //cdata->Print();
  //  after the data has been taken from the fragement tree, the data
  //  is stored/correlated breifly in by the tcsmdata class - these
  //  function takes the data out of tcsmdata, and puts it into the
  //  the tcsmhits class.  These tcsmhit objects are how we access
  //  the data stored in the tcsm branch in the analysis tree.
  //
  //  pcb.
  //
  std::string option = opt;
  std::vector<TCSMHit> D_Hits;
  std::vector<TCSMHit> E_Hits;
  //int fCfdBuildDiff = 5; // largest acceptable time difference between events (clock ticks)  (50 ns)
  /*
    if(GetMultiplicityHorizontal()>1 || GetMultiplicityVertical()>1)
    return;
  */
  std::vector<bool> HorUsed;
  std::vector<bool> VerUsed;
  HorUsed.assign(cdata->GetMultiplicityHorizontal(), false);
  VerUsed.assign(cdata->GetMultiplicityVertical(), false);
  double total_ver_energy = 0.0;
  double total_hor_energy = 0.0;
  int total_ver_hits = 0;
  int total_hor_hits = 0;

  std::vector<int> v1d;
  std::vector<int> v2d;
  std::vector<int> v1e;
  std::vector<int> v2e;
  std::vector<int> v3d;
  std::vector<int> v4d;

  std::vector<int> h1d;
  std::vector<int> h2d;
  std::vector<int> h1e;
  std::vector<int> h2e;
  std::vector<int> h3d;
  std::vector<int> h4d;

  v1d.clear();
  v2d.clear();
  v1e.clear();
  v2e.clear();
  v3d.clear();
  v4d.clear();
  
  h1d.clear();
  h2d.clear();
  h1e.clear();
  h2e.clear();
  h3d.clear();
  h4d.clear();

  for(int hiter=0;hiter<cdata->GetMultiplicityHorizontal();hiter++)
  {
    if(cdata->GetHorizontal_DetectorNbr(hiter)==3)
      h3d.push_back(hiter);
    else if(cdata->GetHorizontal_DetectorNbr(hiter)==4)
      h4d.push_back(hiter);
    else if(cdata->GetHorizontal_DetectorNbr(hiter)==1)
    {
      if(cdata->GetHorizontal_DetectorPos(hiter)=='D')
	h1d.push_back(hiter);
      else if(cdata->GetHorizontal_DetectorPos(hiter)=='E')
	h1e.push_back(hiter);
    }
    else if(cdata->GetHorizontal_DetectorNbr(hiter)==2)
    {
      if(cdata->GetHorizontal_DetectorPos(hiter)=='D')
	h2d.push_back(hiter);
      else if(cdata->GetHorizontal_DetectorPos(hiter)=='E')
	h2e.push_back(hiter);
    }
  }

  for(int viter=0;viter<cdata->GetMultiplicityVertical();viter++)
  {
    if(cdata->GetVertical_DetectorNbr(viter)==3)
      v3d.push_back(viter);
    else if(cdata->GetVertical_DetectorNbr(viter)==4)
      v4d.push_back(viter);
    else if(cdata->GetVertical_DetectorNbr(viter)==1)
    {
      if(cdata->GetVertical_DetectorPos(viter)=='D')
	v1d.push_back(viter);
      else if(cdata->GetVertical_DetectorPos(viter)=='E')
	v1e.push_back(viter);
    }
    else if(cdata->GetVertical_DetectorNbr(viter)==2)
    {
      if(cdata->GetVertical_DetectorPos(viter)=='D')
	v2d.push_back(viter);
      else if(cdata->GetVertical_DetectorPos(viter)=='E')
	v2e.push_back(viter);
    }
  }

  BuildVH(v1d,h1d,D_Hits,cdata);
  BuildVH(v1e,h1e,E_Hits,cdata);
  BuildVH(v2d,h2d,D_Hits,cdata);
  BuildVH(v2e,h2e,E_Hits,cdata);
  BuildVH(v3d,h3d,D_Hits,cdata);
  BuildVH(v4d,h4d,D_Hits,cdata);
  
  
  /*HERE IS THE OLD EVENT BUILDER
  for(int i=0; i<cdata->GetMultiplicityHorizontal(); i++)
  {
    total_hor_energy += cdata->GetHorizontal_Energy(i);
    total_hor_hits++;

    for(int j=0; j<cdata->GetMultiplicityVertical(); j++)
    {
      total_ver_energy += cdata->GetVertical_Energy(j);
      total_ver_hits++;

      if(cdata->GetHorizontal_DetectorNbr(i) == cdata->GetVertical_DetectorNbr(j))	  //check if same detector
      {
        if(cdata->GetHorizontal_DetectorPos(i) == cdata->GetVertical_DetectorPos(j))   //check the are from the same position!
        {
          //if(abs(GetHorizontal_TimeCFD(i)-GetVertical_TimeCFD(j)) > fCfdBuildDiff) {
          //	continue; // ensure there is front-back time correlation to protect against noise/false coinc.
          //}
          csmhit.Clear();

          if(cdata->GetHorizontal_DetectorPos(i) == 'D')    //i decided this is going to mean Delta.
          {
            csmhit.SetDetectorNumber(cdata->GetHorizontal_DetectorNbr(i));		//!
            csmhit.SetDHorizontalCharge(cdata->GetHorizontal_Charge(i)); 				//!
            csmhit.SetDVerticalCharge(cdata->GetVertical_Charge(j));    			//!
            csmhit.SetDHorizontalStrip(cdata->GetHorizontal_StripNbr(i)); 			//!
            csmhit.SetDVerticalStrip(cdata->GetVertical_StripNbr(j));   			//!
            csmhit.SetDHorizontalCFD(cdata->GetHorizontal_TimeCFD(i));					//!
            csmhit.SetDVerticalCFD(cdata->GetVertical_TimeCFD(j));					//!
            csmhit.SetDHorizontalTime(cdata->GetHorizontal_Time(i));	//!
            csmhit.SetDVerticalTime(cdata->GetVertical_Time(j));		//!
            csmhit.SetDHorizontalEnergy(cdata->GetHorizontal_Energy(i));				//!
            csmhit.SetDVerticalEnergy(cdata->GetVertical_Energy(j));				//!

            if(cdata->GetHorizontal_DetectorNbr(i)==2)
            {
              if(cdata->GetHorizontal_StripNbr(i)==9 ||
                  cdata->GetHorizontal_StripNbr(i)==10 ||
                  cdata->GetHorizontal_StripNbr(i)==11)
              {
                csmhit.SetDHorizontalCharge(cdata->GetVertical_Charge(j));
                csmhit.SetDHorizontalEnergy(cdata->GetVertical_Energy(j));
              }
            }

            if(cdata->GetHorizontal_DetectorNbr(i)==3)
            {
              if(cdata->GetHorizontal_StripNbr(i)==12 ||
                  cdata->GetHorizontal_StripNbr(i)==15)
              {
                csmhit.SetDHorizontalCharge(cdata->GetVertical_Charge(j));
                csmhit.SetDHorizontalEnergy(cdata->GetVertical_Energy(j));
              }
            }

            csmhit.SetDPosition(TCSM::GetPosition(cdata->GetHorizontal_DetectorNbr(i),
                                                  cdata->GetHorizontal_DetectorPos(i),
                                                  cdata->GetHorizontal_StripNbr(i),
                                                  cdata->GetVertical_StripNbr(j)));
            VerUsed.at(j) = true;
            HorUsed.at(i) = true;
            D_Hits.push_back(csmhit);
          }
          else if(cdata->GetHorizontal_DetectorPos(i) == 'E')
          {
            csmhit.SetDetectorNumber(cdata->GetHorizontal_DetectorNbr(i));		//!
            csmhit.SetEHorizontalCharge(cdata->GetHorizontal_Charge(i)); 				//!
            csmhit.SetEVerticalCharge(cdata->GetVertical_Charge(j));    			//!
            csmhit.SetEHorizontalStrip(cdata->GetHorizontal_StripNbr(i)); 			//!
            csmhit.SetEVerticalStrip(cdata->GetVertical_StripNbr(j));   			//!
            csmhit.SetEHorizontalCFD(cdata->GetHorizontal_TimeCFD(i));					//!
            csmhit.SetEVerticalCFD(cdata->GetVertical_TimeCFD(j));					//!
            csmhit.SetEHorizontalTime(cdata->GetHorizontal_Time(i)); //!
            csmhit.SetEVerticalTime(cdata->GetVertical_Time(j));     //!
            csmhit.SetEHorizontalEnergy(cdata->GetHorizontal_Energy(i));				//!
            csmhit.SetEVerticalEnergy(cdata->GetVertical_Energy(j));				//!
            csmhit.SetEPosition(TCSM::GetPosition(cdata->GetHorizontal_DetectorNbr(i),
                                                  cdata->GetHorizontal_DetectorPos(i),
                                                  cdata->GetHorizontal_StripNbr(i),
                                                  cdata->GetVertical_StripNbr(j)));
            VerUsed.at(j) = true;
            HorUsed.at(i) = true;
            E_Hits.push_back(csmhit);
          }
        }
      }
    }
  }
  */
  std::vector<int>HorStrpFree;
  std::vector<int> VerStrpFree;

  for(int i=0; i<cdata->GetMultiplicityHorizontal(); i++)
  {
    if(!HorUsed.at(i))
      HorStrpFree.push_back(i);
  }

  for(int j=0; j<cdata->GetMultiplicityVertical(); j++)
  {
    if(!VerUsed.at(j))
      VerStrpFree.push_back(j);
  }
  
  /*FREE HIT PRINT
  for(int iter=0; iter<VerStrpFree.size(); iter++)
  {
    int addr = VerStrpFree.at(iter);
    
    cdata->Print(addr,0);
  }

  for(int iter=0; iter<HorStrpFree.size(); iter++)
  {
    int addr = HorStrpFree.at(iter);
    
    cdata->Print(addr,1);
  }*/
  
/*  HERE BE EVENT RECOVERY
  for(int iter=0; iter<VerStrpFree.size(); iter++)
  {
    int addr = VerStrpFree.at(iter);

    if(cdata->GetVertical_DetectorNbr(addr)==1 && cdata->GetVertical_DetectorPos(addr)=='D')
    {
      csmhit.SetDetectorNumber(cdata->GetVertical_DetectorNbr(addr));		//!
      csmhit.SetDHorizontalCharge(cdata->GetVertical_Charge(addr)); 				//!
      csmhit.SetDVerticalCharge(cdata->GetVertical_Charge(addr));    			//!
      csmhit.SetDHorizontalStrip(6); 			//!
      csmhit.SetDVerticalStrip(cdata->GetVertical_StripNbr(addr));   			//!
      csmhit.SetDHorizontalCFD(cdata->GetVertical_TimeCFD(addr));					//!
      csmhit.SetDVerticalCFD(cdata->GetVertical_TimeCFD(addr));					//!
      csmhit.SetDHorizontalTime(cdata->GetVertical_Time(addr));	//!
      csmhit.SetDVerticalTime(cdata->GetVertical_Time(addr));		//!
      csmhit.SetDHorizontalEnergy(cdata->GetVertical_Energy(addr));				//!
      csmhit.SetDVerticalEnergy(cdata->GetVertical_Energy(addr));				//!
      //cout<<"Here2"<<endl;
      csmhit.SetDPosition(TCSM::GetPosition(cdata->GetVertical_DetectorNbr(addr),
                                            cdata->GetVertical_DetectorPos(addr),
                                            6,
                                            cdata->GetVertical_StripNbr(addr)));
      //cout<<"Here3"<<endl;
      D_Hits.push_back(csmhit);
      //cout<<"Here4"<<endl;
    }
  }

  for(int iter=0; iter<HorStrpFree.size(); iter++)
  {
    int addr = HorStrpFree.at(iter);

    //cout<<"addr: "<<addr<<endl;
    if(cdata->GetHorizontal_DetectorPos(addr)=='D')
    {
      if(cdata->GetHorizontal_DetectorNbr(addr)==2 && cdata->GetHorizontal_StripNbr(addr)!=9)
      {
        csmhit.SetDetectorNumber(cdata->GetHorizontal_DetectorNbr(addr));		//!
        csmhit.SetDHorizontalCharge(cdata->GetHorizontal_Charge(addr)); 				//!
        csmhit.SetDVerticalCharge(cdata->GetHorizontal_Charge(addr));    			//!
        csmhit.SetDHorizontalStrip(cdata->GetHorizontal_StripNbr(addr)); 			//!
        csmhit.SetDVerticalStrip(6);   			//!
        csmhit.SetDHorizontalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDVerticalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDHorizontalTime(cdata->GetHorizontal_Time(addr));	//!
        csmhit.SetDVerticalTime(cdata->GetHorizontal_Time(addr));		//!
        csmhit.SetDHorizontalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDVerticalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDPosition(TCSM::GetPosition(cdata->GetHorizontal_DetectorNbr(addr),
                                              cdata->GetHorizontal_DetectorPos(addr),
                                              cdata->GetHorizontal_StripNbr(addr),
                                              6));
        D_Hits.push_back(csmhit);
      }

      if(cdata->GetHorizontal_DetectorNbr(addr)==3)
      {
        csmhit.SetDetectorNumber(cdata->GetHorizontal_DetectorNbr(addr));		//!
        csmhit.SetDHorizontalCharge(cdata->GetHorizontal_Charge(addr)); 				//!
        csmhit.SetDVerticalCharge(cdata->GetHorizontal_Charge(addr));    			//!
        csmhit.SetDHorizontalStrip(cdata->GetHorizontal_StripNbr(addr)); 			//!
        csmhit.SetDVerticalStrip(11);   			//!
        csmhit.SetDHorizontalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDVerticalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDHorizontalTime(cdata->GetHorizontal_Time(addr));	//!
        csmhit.SetDVerticalTime(cdata->GetHorizontal_Time(addr));		//!
        csmhit.SetDHorizontalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDVerticalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDPosition(TCSM::GetPosition(cdata->GetHorizontal_DetectorNbr(addr),
                                              cdata->GetHorizontal_DetectorPos(addr),
                                              cdata->GetHorizontal_StripNbr(addr),
                                              11));
        D_Hits.push_back(csmhit);
      }
      else if(cdata->GetHorizontal_DetectorNbr(addr)==4)
      {
        csmhit.SetDetectorNumber(cdata->GetHorizontal_DetectorNbr(addr));		//!
        csmhit.SetDHorizontalCharge(cdata->GetHorizontal_Charge(addr)); 				//!
        csmhit.SetDVerticalCharge(cdata->GetHorizontal_Charge(addr));    			//!
        csmhit.SetDHorizontalStrip(cdata->GetHorizontal_StripNbr(addr)); 			//!
        csmhit.SetDVerticalStrip(15);   			//!
        csmhit.SetDHorizontalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDVerticalCFD(cdata->GetHorizontal_TimeCFD(addr));					//!
        csmhit.SetDHorizontalTime(cdata->GetHorizontal_Time(addr));	//!
        csmhit.SetDVerticalTime(cdata->GetHorizontal_Time(addr));		//!
        csmhit.SetDHorizontalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDVerticalEnergy(cdata->GetHorizontal_Energy(addr));				//!
        csmhit.SetDPosition(TCSM::GetPosition(cdata->GetHorizontal_DetectorNbr(addr),
                                              cdata->GetHorizontal_DetectorPos(addr),
                                              cdata->GetHorizontal_StripNbr(addr),
                                              15));
        D_Hits.push_back(csmhit);
      }
    }
  }*/

  //now we will try to match front and back detectors.
  std::vector<bool> usedpixel(E_Hits.size(), false);

  for(int i=0; i<D_Hits.size(); i++)
  {
    for(int j=0; j<E_Hits.size(); j++)
    {
      if(usedpixel.at(j))
      {
        continue;
      }

      if(D_Hits.at(i).GetDetectorNumber() == E_Hits.at(j).GetDetectorNumber())
      {
        if((D_Hits.at(i).GetDPosition() - E_Hits.at(j).GetEPosition()).Mag()>10.0)
        {
          continue;
        }

        usedpixel.at(j) = true;
        D_Hits.at(i).SetEHorizontalCharge(E_Hits.at(j).GetEHorizontalCharge());
        D_Hits.at(i).SetEVerticalCharge(E_Hits.at(j).GetEVerticalCharge());
        D_Hits.at(i).SetEHorizontalStrip(E_Hits.at(j).GetEHorizontalStrip());
        D_Hits.at(i).SetEVerticalStrip(E_Hits.at(j).GetEVerticalStrip());
        D_Hits.at(i).SetEHorizontalCFD(E_Hits.at(j).GetEHorizontalCFD());
        D_Hits.at(i).SetEVerticalCFD(E_Hits.at(j).GetEVerticalCFD());
        D_Hits.at(i).SetEHorizontalEnergy(E_Hits.at(j).GetEHorizontalEnergy());
        D_Hits.at(i).SetEVerticalEnergy(E_Hits.at(j).GetEVerticalEnergy());
        D_Hits.at(i).SetEHorizontalTime(E_Hits.at(j).GetEHorizontalTime());
        D_Hits.at(i).SetEVerticalTime(E_Hits.at(j).GetEVerticalTime());
        D_Hits.at(i).SetEPosition(E_Hits.at(j).GetEPosition());
      } // comparison of detector numbers
    } // loop over e hits

    csm_hits.push_back(D_Hits.at(i));
  }

  for(int k=0; k<usedpixel.size(); k++) //This loop puts in hits from E that weren't correlated
  {
    if(!usedpixel.at(k))
    {
      csm_hits.push_back(E_Hits.at(k));
    }
  }
}



void TCSM::Clear(Option_t *option)
{
  //cout << "clearing " << endl;
  if(data) data->Clear();

  csm_hits.clear();
  //cout <<" size: " << csm_hits.size() << endl;
  return;
}

void TCSM::Print(Option_t *option)
{
  printf("not yet written...\n");
  return;
}


TVector3 TCSM::GetPosition(int detector,char pos, int horizontalstrip, int verticalstrip, double X, double Y, double Z)
{
  //horizontal strips collect N charge!
  //vertical strips collect P charge!
  TVector3 Pos;
  double SideX = 68;
  double SideZ = -4.8834;
  double dEX = 54.9721;
  double dEZ = 42.948977;
  double EX = 58.062412;
  double EZ = 48.09198;
  double detTheta = 31. * (TMath::Pi()/180.);
  double x = 0.0,y = 0.0,z = 0.0;

  if(detector==3&&pos=='D')
  {
    //Right Side
    verticalstrip=15-verticalstrip;
    x = SideX;
    z = SideZ + (50./32.)*(2*verticalstrip+1);
  }
  else if(detector==4&&pos=='D')
  {
    //Left Side
    x = -SideX;
    z = SideZ + (50./32.)*(2*verticalstrip+1);
  }
  else if(detector==1&&pos=='D')
  {
    //Right dE
    verticalstrip=15-verticalstrip;
    x = dEX - (50./32.)*cos(detTheta)*(2*verticalstrip+1);
    z = dEZ + (50./32.)*sin(detTheta)*(2*verticalstrip+1);
  }
  else if(detector==2&&pos=='D')
  {
    //Left dE
    x = -dEX + (50./32.)*cos(detTheta)*(2*verticalstrip+1);
    z = dEZ + (50./32.)*sin(detTheta)*(2*verticalstrip+1);
  }
  else if(detector==1&&pos=='E')
  {
    //Right E
    x = EX - (50./32.)*cos(detTheta)*(2*verticalstrip+1);
    z = EZ + (50./32.)*sin(detTheta)*(2*verticalstrip+1);
  }
  else if(detector==2&&pos=='E')
  {
    //Left E
    verticalstrip=15-verticalstrip;
    x = -EX + (50./32.)*cos(detTheta)*(2*verticalstrip+1);
    z = EZ + (50./32.)*sin(detTheta)*(2*verticalstrip+1);
  }
  else
  {
    printf("***Error, unrecognized detector and position combo!***\n");
  }

  y = (50./32.)*(2*horizontalstrip+1) - (50/16.)*8;
  Pos.SetX(x + X);
  Pos.SetY(y + Y);
  Pos.SetZ(z+ Z);
  return(Pos);
}

void TCSM::BuildVH(vector<int> &vvec,vector<int> &hvec,vector<TCSMHit> &hitvec,TCSMData *cdataVH)
{
  if(vvec.size()==0 && hvec.size()==0)
    return;

  TCSMHit csmhitVH;
  
  if(vvec.size()==1&&hvec.size()==1)
  {
    csmhitVH.SetDetectorNumber(cdataVH->GetHorizontal_DetectorNbr(hvec.at(0)));		//!
    csmhitVH.SetDHorizontalCharge(cdataVH->GetHorizontal_Charge(hvec.at(0))); 				//!
    csmhitVH.SetDVerticalCharge(cdataVH->GetVertical_Charge(vvec.at(0)));    			//!
    csmhitVH.SetDHorizontalStrip(cdataVH->GetHorizontal_StripNbr(hvec.at(0))); 			//!
    csmhitVH.SetDVerticalStrip(cdataVH->GetVertical_StripNbr(vvec.at(0)));   			//!
    csmhitVH.SetDHorizontalCFD(cdataVH->GetHorizontal_TimeCFD(hvec.at(0)));					//!
    csmhitVH.SetDVerticalCFD(cdataVH->GetVertical_TimeCFD(vvec.at(0)));					//!
    csmhitVH.SetDHorizontalTime(cdataVH->GetHorizontal_Time(hvec.at(0)));	//!
    csmhitVH.SetDVerticalTime(cdataVH->GetVertical_Time(vvec.at(0)));		//!
    csmhitVH.SetDHorizontalEnergy(cdataVH->GetHorizontal_Energy(hvec.at(0)));				//!
    csmhitVH.SetDVerticalEnergy(cdataVH->GetVertical_Energy(vvec.at(0)));				//!
    csmhitVH.SetDPosition(TCSM::GetPosition(cdataVH->GetHorizontal_DetectorNbr(hvec.at(0)),
					  cdataVH->GetHorizontal_DetectorPos(hvec.at(0)),
					  cdataVH->GetHorizontal_StripNbr(hvec.at(0)),
					  cdataVH->GetVertical_StripNbr(vvec.at(0))));
    //csmhitVH.Print();
    hitvec.push_back(csmhitVH);
    vvec.pop_back();
    hvec.pop_back();
  }
  if(!vvec.empty() || !hvec.empty())
  {
    cdataVH->Print();
    for(int iter=0;iter<vvec.size();iter++)
    {
      cout<<vvec.at(iter)<<endl;
    }
  }
  
}









