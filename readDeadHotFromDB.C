// --- original author: Theo Koblesky
// --- current author: Ron Belmont


void readDeadHotFromDB()
{

  cout << "Loading libraries..." << endl;
  gSystem->Load("libfun4all.so");
  gSystem->Load("libsvx.so");
  cout << "Libraries loaded..." << endl;


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
  int counter = 0;
  while ( fin >> tmpNumber )
    {
      runNumber.push_back(tmpNumber);
      cout << tmpNumber << " ";
      runNumberIndex.push_back(counter);
      counter++;
      if(counter>5) break; // temp
    }
  cout << endl;
  cout << "Found " << runNumber.size() << " runs" << endl;

  int nruns = runNumber.size();

  TFile *outfile = TFile::Open("rundep.root","RECREATE");

  // TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  // TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);
  TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);




  for ( int irun = 0; irun < nruns; irun++ )
    {
      //if(irun%4!=0) continue; // skip runs as desired...

      SvxDeadMap deadMap(0);

      int runno = runNumber[irun];
      RunToTime *rt = RunToTime::instance();
      PHTimeStamp* tStamp = rt->getBeginTime(runno);

      //now->setToSystemTime();
      bool success = deadMap.readFromDatabase (tStamp);

      bool success2 = deadMap.readReadoutsFromDatabase (tStamp);

      cout << "Read deadMap from database: "
	   << ( success ? "success" : "failure" )
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

      int B2_read_count[5][16] = 0;
      int B3_read_count[6][24] = 0;
      int B2_chan_count[5][16] = 0;
      int B3_chan_count[6][24] = 0;

      int B2_read_total[5][16] = 0;
      int B3_read_total[6][24] = 0;
      int B2_chan_total[5][16] = 0;
      int B3_chan_total[6][24] = 0;

      int nLayers = 2;
      int nLadders = 24; // 24 for B3, 16 for B2
      int nSensors = 6; // 6 for B3, 5 for B2
      // --- note that here we have 2*2*384 = 1536, on VTX webpage it's 12*128 = 1536
      // --- so the final number is equivalent, but how one arrives at it is different...
      // --- but I don't claim to understand the VTX software at such a level
      int nSections = 2;
      int nReadouts = 2;
      int nChannels = 384;

      for(int h=0; h<nLayers; h++) { // striplayer---layer 0 is B2, layer 1 is B3
	for(int i=0; i<nLadders; i++) { // ladder---B3 has 24 ladders, but B2 has only 16 (see below)
	  for(int j=0; j<nSensors; j++)  { // sensor---B3 has 6 sensors per ladder, but B2 has only 5 (see below)
	    for(int k=0; k<nSections; k++)  { // section---I need find out what exactly this means...
	      for(int l=0; l<nReadouts; l++)  { // readout---I'm a little confused here, because the web says 12 chips per sensor...
		if(h==0 && i < 16 && j < 5) {B2_read_total++; B2_read_total[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		if(h==1) {B3_read_total++; B3_read_total[j][i]++;}
		if(deadMap.readoutStatus(h,i,j,k,l)!=0) {
		  if(h==0 && i < 16 && j < 5) {B2_read_count++; B2_read_count[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		  if(h==1) {B3_read_count++; B3_read_count[j][i]++;}
		} // readout status not zero
		for(int m=0; m<384; m++) { // channel---note that 384=3*128 ...
		  if(deadMap.readoutStatus(h,i,j,k,l)==0) {
		    if(h==0 && i < 16 && j < 5) {B2_chan_total++; B2_chan_total[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		    if(h==1) {B3_chan_total++; B3_chan_total[j][i]++;}
		    if(deadMap.channelStatus(h,i,j,k,l,m)!=0 ) {
		      if(h==0 && i < 16 && j < 5) {B2_chan_count++; B2_chan_count[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		      if(h==1) {B3_chan_count++; B3_chan_count[j][i]++;}
		    } // status not zero
		  } // readout status zero
		} // channel
	      } // readout
	    } // section
	  } // sensor
	} // ladder
      } // layer

      // cout<<"Good percentage of B2: "<<100-(B2_chan_count+B2_read_count*3*128)*100.0/(16*5*2*2*384)<<endl;
      // cout<<"Good percentage of B3: "<<100-(B3_chan_count+B3_read_count*3*128)*100.0/(24*6*2*2*384)<<endl;
      cout<<"Good percentage of B2: "<<1-(B2_chan_count+B2_read_count*3*128)*1.0/(16*5*2*2*384)<<endl;
      cout<<"Good percentage of B3: "<<1-(B3_chan_count+B3_read_count*3*128)*1.0/(24*6*2*2*384)<<endl;

      // --- note that, for reasons unknown at this time, these numbers do not seem to be correct
      float B2_read_average_good = 1-((float)B2_read_count/(float)B2_read_total);
      float B2_chan_average_good = 1-((float)B2_chan_count/(float)B2_chan_total);
      float B3_read_average_good = 1-((float)B3_read_count/(float)B3_read_total);
      float B3_chan_average_good = 1-((float)B3_chan_count/(float)B3_chan_total);
      // cout << "Good percentage of B2 readout " << B2_read_average_good << endl;
      // cout << "Good percentage of B2 channel " << B2_chan_average_good << endl;
      // cout << "Good percentage of B3 readout " << B3_read_average_good << endl;
      // cout << "Good percentage of B3 channel " << B3_chan_average_good << endl;
      cout << "Product method for B2: " << B2_read_average_good*B2_chan_average_good << endl;
      cout << "Product method for B3: " << B3_read_average_good*B3_chan_average_good << endl;

      // --- histograms
      // B2_good->SetBinContent(irun+1,100-(B2_chan_count+B2_read_count*3*128)*100.0/(16*5*2*2*384));
      // B3_good->SetBinContent(irun+1,100-(B3_chan_count+B3_read_count*3*128)*100.0/(24*6*2*2*384));
      B2_good->SetBinContent(irun+1,1-(B2_chan_count+B2_read_count*3*128)*1.0/(16*5*2*2*384));
      B3_good->SetBinContent(irun+1,1-(B3_chan_count+B3_read_count*3*128)*1.0/(24*6*2*2*384));

      deadMap.writeToFile(Form("StripData/strip_channel_run%d.txt",runno));
      deadMap.writeReadoutsToFile(Form("StripData/strip_readout_run%d.txt",runno));

      continue; // need to sort out stuff with the pixels later...
      // --------- first i want to get some kind of map from the strips...

      //SvxPixelHotDeadMap pixelMap(0); // produced shitloads of error messages
      SvxPixelHotDeadMapv2 pixelMap(0);

      bool readref = false; // true or false??  not clear at the moment, but default in class is false
      bool successPixel = pixelMap.readFromDatabase(runno,readref);

    //   int nLayer = 1;
    //   int nLadder = 1;
    //   int nSouth = 2;
    //   int nROC = 1;
    //   int nColumn = 1;
    //   int nRow = 1;
    //   for(int iLayer=0; iLayer<nLayer; iLayer++) {
    // 	for(int iLadder=0; iLadder<nLadder; iLadder++) {
    // 	  for(int iSouth=0; iSouth<nSouth; iSouth++) {
    // 	    for(int iROC=0; i<nROC; iROC++) {
    // 	      for(int iColumn=0; i<nColumn; iColumn++) {
    // 		for(int iRow=0; i<nRow; iRow++) {
    // 		  // --- get the status from the map
    // 		  int status = pixelMap.getStatus(iLayer,iLadder,iSouth,iROC,iColumn,iRow);
    // 		  // --- do other stuff...
    // 		  // --- finished doing stuff
    // 		} // Row
    // 	      } // Column
    // 	    } // ROC
    // 	  } // South
    // 	} // Ladder
    //   } // Layer

    } // loop over run numbers

  cout << "readDeadhotFromDB: now writing histograms to file" << endl;

  outfile->Write();
  outfile->Close();
  delete outfile;

  cout << "readDeadhotFromDB: completed." << endl;

}
