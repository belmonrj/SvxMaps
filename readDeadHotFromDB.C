// --- original author: Theo Koblesky
// --- current author: Ron Belmont


void readDeadHotFromDB()
{

  cout << "Loading libraries...\n";
  gSystem->Load("libfun4all.so");
  gSystem->Load("libsvx.so");
  cout << "Libraries loaded...\n";


  //const char* runFile = "allrun14_woraw_runlist.txt";
  const char* runFile = "run14_short.list";

  vector<int> runNumber;
  vector<int> runNumberIndex;


  ifstream fin(runFile);
  if (!fin)
    {
      cout << "!!ERROR!! Unable to open " << runFile << endl;
      return;
    }
  int tmpNumber;
  fin >> tmpNumber;
  int counter = 0;
  while (!fin.eof())
    {
      if (true/*tmpNumber >= repairRunLow[0] && tmpNumber <= repairRunHigh[0]*/)
        {
          runNumber.push_back(tmpNumber);
          cout << tmpNumber << " ";
        }
      fin >> tmpNumber;
      runNumberIndex.push_back(counter);
      counter++;
    }
  cout << endl;
  cout << "Found " << runNumber.size() << " runs" << endl;

  unsigned int nruns = runNumber.size();

  // TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  // TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);
  TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);



  for(int irun = 0; irun < nruns; irun++)
    {

      //if(irun%4!=0) continue; // skip runs as desired...

      SvxDeadMap deadMap(0);

      int runno = runNumber[irun];
      RunToTime *rt = RunToTime::instance();
      PHTimeStamp* tStamp = rt->getBeginTime(runno);

      //now->setToSystemTime();
      bool success = deadMap.readFromDatabase (tStamp);

      bool success2 = deadMap.readReadoutsFromDatabase (tStamp);

      cout << "Read deadMap from database: " << (success ? "success" : "failure")
		   <<" For Run "<<runno<< endl;

      //if (success) deadMap.print();


      int B2_read_count = 0;
      int B3_read_count = 0;
      int B2_chan_count = 0;
      int B3_chan_count = 0;

      int B2_read_total = 0;
      int B3_read_total = 0;
      int B2_chan_total = 0;
      int B3_chan_total = 0;

      for(int h=0; h<2; h++) { // striplayer
	for(int i=0; i<24; i++) { // ladder
	  for(int j=0; j<6; j++)  { // sensor
	    for(int k=0; k<2; k++)  { // section
	      for(int l=0; l<2; l++)  { // readout
		if(h==0 && i < 16 && j < 5) B2_read_total++;
		if(h==1) B3_read_total++;
		if(deadMap.readoutStatus(h,i,j,k,l)!=0) {
		  if(h==0 && i < 16 && j < 5) B2_read_count++;
		  if(h==1) B3_read_count++;
		} // status not zero
		else {
		  for(int m=0; m<384; m++) { // channel
		    if(h==0 && i < 16 && j < 5) B2_chan_total++;
		    if(h==1) B3_chan_total++;
		    if(deadMap.channelStatus(h,i,j,k,l,m)!=0 ) {
		      if(h==0 && i < 16 && j < 5)B2_chan_count++;
		      if(h==1)B3_chan_count++;
		    } // status not zero
		  } // channel
		} // else
	      } // readout
	    } // section
	  } // sensor
	} // ladder
      } // layer

      cout<<"Good percentage of B2: "<<100-(B2_chan_count+B2_read_count*3*128)*100.0/(16*5*2*2*384)<<endl;
      cout<<"Good percentage of B3: "<<100-(B3_chan_count+B3_read_count*3*128)*100.0/(24*6*2*2*384)<<endl;

      cout << "Good percentage of B2 readout " << 1-((float)B2_read_count/(float)B2_read_total) << endl;
      cout << "Good percentage of B2 chanout " << 1-((float)B2_chan_count/(float)B2_chan_total) << endl;

      B2_good->SetBinContent(irun+1,100-(B2_chan_count+B2_read_count*3*128)*100.0/(16*5*2*2*384));
      B3_good->SetBinContent(irun+1,100-(B3_chan_count+B3_read_count*3*128)*100.0/(24*6*2*2*384));

      break;

    } // loop over run numbers
  //cout<<"Good percentage of B2: "<<100-(B2_read_count)*100.0/(16*5*2*2)<<endl;
  //cout<<"Good percentage of B3: "<<100-(B3_read_count)*100.0/(24*6*2*2)<<endl;

  deadMap.writeToFile(Form("StripData/strip_channel_run%d.txt",runno));
  deadMap.writeReadoutsToFile(Form("StripData/strip_readout_run%d.txt",runno));

  cout << "readDeadhotFromDB: completed." << endl;

  TFile *outfile = TFile::Open("rundep.root","RECREATE");
  B2_good->Write();
  B3_good->Write();

}

