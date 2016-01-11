// --- original author: Theo Koblesky
// --- current author: Ron Belmont

//#include <cmath>

void readDeadHotFromDB()
{

  gStyle->SetOptStat(0);

  cout << "Loading libraries..." << endl;
  gSystem->Load("libfun4all.so");
  gSystem->Load("libsvx.so");
  cout << "Libraries loaded..." << endl;


  //const char* runFile = "allrun14_woraw_runlist.txt";
  //const char* runFile = "run14_short.list";
  const char* runFile = "Run14_3HeAu_short.list";

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
    }
  cout << endl;
  cout << "Found " << runNumber.size() << " runs" << endl;

  int nruns = runNumber.size();

  // TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  // TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumber[0]-0.5,runNumber[nruns-1]-0.5);
  TH1F *B2_good = new TH1F("B2_good","B2_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);
  TH1F *B3_good = new TH1F("B3_good","B3_good",nruns,runNumberIndex[0]-0.5,runNumberIndex[nruns-1]-0.5);



  float ave_B2_read_count = 0;
  float ave_B3_read_count = 0;
  float ave_B2_chan_count = 0;
  float ave_B3_chan_count = 0;

  float ave_B2_read_total = 0;
  float ave_B3_read_total = 0;
  float ave_B2_chan_total = 0;
  float ave_B3_chan_total = 0;

  float ave_B2_read_count_diff[6][24];
  float ave_B3_read_count_diff[6][24];
  float ave_B2_chan_count_diff[6][24];
  float ave_B3_chan_count_diff[6][24];

  float ave_B2_read_total_diff[6][24];
  float ave_B3_read_total_diff[6][24];
  float ave_B2_chan_total_diff[6][24];
  float ave_B3_chan_total_diff[6][24];

  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  // ---
	  ave_B2_read_count_diff[i][j] = 0;
	  ave_B3_read_count_diff[i][j] = 0;
	  ave_B2_chan_count_diff[i][j] = 0;
	  ave_B3_chan_count_diff[i][j] = 0;
	  // ---
	  ave_B2_read_total_diff[i][j] = 0;
	  ave_B3_read_total_diff[i][j] = 0;
	  ave_B2_chan_total_diff[i][j] = 0;
	  ave_B3_chan_total_diff[i][j] = 0;
	}
    }


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

      int B2_read_count_diff[6][24];
      int B3_read_count_diff[6][24];
      int B2_chan_count_diff[6][24];
      int B3_chan_count_diff[6][24];

      int B2_read_total_diff[6][24];
      int B3_read_total_diff[6][24];
      int B2_chan_total_diff[6][24];
      int B3_chan_total_diff[6][24];

      for(int i=0; i<6; i++)
	{
	  for(int j=0; j<24; j++)
	    {
	      // ---
	      B2_read_count_diff[i][j] = 0;
	      B3_read_count_diff[i][j] = 0;
	      B2_chan_count_diff[i][j] = 0;
	      B3_chan_count_diff[i][j] = 0;
	      // ---
	      B2_read_total_diff[i][j] = 0;
	      B3_read_total_diff[i][j] = 0;
	      B2_chan_total_diff[i][j] = 0;
	      B3_chan_total_diff[i][j] = 0;
	    }
	}

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
	  for(int j=0; j<nSensors; j++) { // sensor---B3 has 6 sensors per ladder, but B2 has only 5 (see below)
	    for(int k=0; k<nSections; k++) { // section---I need find out what exactly this means...
	      for(int l=0; l<nReadouts; l++) { // readout---I'm a little confused here, because the web says 12 chips per sensor...
		if(h==0 && i < 16 && j < 5) {B2_read_total++;B2_read_total_diff[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		if(h==1) {B3_read_total++;B3_read_total_diff[j][i]++;}
		if(deadMap.readoutStatus(h,i,j,k,l)!=0) {
		  if(h==0 && i < 16 && j < 5) {B2_read_count++;B2_read_count_diff[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		  if(h==1) {B3_read_count++;B3_read_count_diff[j][i]++;}
		} // readout status not zero
		else {
		  for(int m=0; m<384; m++) { // channel---note that 384=3*128 ...
		    if(h==0 && i < 16 && j < 5) {B2_chan_total++;B2_chan_total_diff[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		    if(h==1) {B3_chan_total++;B3_chan_total_diff[j][i]++;}
		    if(deadMap.channelStatus(h,i,j,k,l,m)!=0 ) {
		      if(h==0 && i < 16 && j < 5) {B2_chan_count++;B2_chan_count_diff[j][i]++;} // requirement of 16 and 5 for B2 (see above)
		      if(h==1) {B3_chan_count++;B3_chan_count_diff[j][i]++;}
		    } // status not zero
		  } // channel
		} // else
	      } // readout
	    } // section
	  } // sensor
	} // ladder
      } // layer

      // ---
      ave_B2_read_count += B2_read_count;
      ave_B3_read_count += B3_read_count;
      ave_B2_chan_count += B2_chan_count;
      ave_B3_chan_count += B3_chan_count;
      // ---
      ave_B2_read_total += B2_read_total;
      ave_B3_read_total += B3_read_total;
      ave_B2_chan_total += B2_chan_total;
      ave_B3_chan_total += B3_chan_total;
      for(int i=0; i<6; i++)
	{
	  for(int j=0; j<24; j++)
	    {
	      // ---
	      ave_B2_read_count_diff[i][j] += B2_read_count_diff[i][j];
	      ave_B3_read_count_diff[i][j] += B3_read_count_diff[i][j];
	      ave_B2_chan_count_diff[i][j] += B2_chan_count_diff[i][j];
	      ave_B3_chan_count_diff[i][j] += B3_chan_count_diff[i][j];
	      // ---
	      ave_B2_read_total_diff[i][j] += B2_read_total_diff[i][j];
	      ave_B3_read_total_diff[i][j] += B3_read_total_diff[i][j];
	      ave_B2_chan_total_diff[i][j] += B2_chan_total_diff[i][j];
	      ave_B3_chan_total_diff[i][j] += B3_chan_total_diff[i][j];
	    }
	}



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

      // ---

      float B2_read_average_good_diff[6][24];
      float B2_chan_average_good_diff[6][24];
      float B3_read_average_good_diff[6][24];
      float B3_chan_average_good_diff[6][24];

      for(int i=0; i<6; i++)
	{
	  for(int j=0; j<24; j++)
	    {
	      B2_read_average_good_diff[i][j] = 1-((float)B2_read_count_diff[i][j]/(float)B2_read_total_diff[i][j]);
	      B2_chan_average_good_diff[i][j] = 1-((float)B2_chan_count_diff[i][j]/(float)B2_chan_total_diff[i][j]);

	      B3_read_average_good_diff[i][j] = 1-((float)B3_read_count_diff[i][j]/(float)B3_read_total_diff[i][j]);
	      B3_chan_average_good_diff[i][j] = 1-((float)B3_chan_count_diff[i][j]/(float)B3_chan_total_diff[i][j]);
	      if(B3_read_total_diff[i][j] == 0) B3_read_average_good_diff[i][j] = 1;
	      if(B3_chan_total_diff[i][j] == 0) B3_chan_average_good_diff[i][j] = 1;
	    }
	}



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

      // ------------------------
      // --- begin pixel stuff...
      // ------------------------

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

      // -------------------
      // --- end pixel stuff
      // -------------------

    } // loop over run numbers

  // ---
  ave_B2_read_count /= (float)nruns;
  ave_B3_read_count /= (float)nruns;
  ave_B2_chan_count /= (float)nruns;
  ave_B3_chan_count /= (float)nruns;
  // ---
  ave_B2_read_total /= (float)nruns;
  ave_B3_read_total /= (float)nruns;
  ave_B2_chan_total /= (float)nruns;
  ave_B3_chan_total /= (float)nruns;
  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  // ---
	  ave_B2_read_count_diff[i][j] /= (float)nruns;
	  ave_B3_read_count_diff[i][j] /= (float)nruns;
	  ave_B2_chan_count_diff[i][j] /= (float)nruns;
	  ave_B3_chan_count_diff[i][j] /= (float)nruns;
	  // ---
	  ave_B2_read_total_diff[i][j] /= (float)nruns;
	  ave_B3_read_total_diff[i][j] /= (float)nruns;
	  ave_B2_chan_total_diff[i][j] /= (float)nruns;
	  ave_B3_chan_total_diff[i][j] /= (float)nruns;
	  // ---
	  //cout << i << " " << j << " " << ave_B3_read_total_diff[i][j] << " " << ave_B3_chan_total_diff[i][j] << endl;
	  cout << i << " " << j << " " << ave_B3_read_count_diff[i][j] << "/" << ave_B3_read_total_diff[i][j] << " " << ave_B3_chan_count_diff[i][j] << "/" << ave_B3_chan_total_diff[i][j] << endl;
	}
    }

  float ave_B2_read_average_good = 1-((float)ave_B2_read_count/(float)ave_B2_read_total);
  float ave_B2_chan_average_good = 1-((float)ave_B2_chan_count/(float)ave_B2_chan_total);
  float ave_B3_read_average_good = 1-((float)ave_B3_read_count/(float)ave_B3_read_total);
  float ave_B3_chan_average_good = 1-((float)ave_B3_chan_count/(float)ave_B3_chan_total);

  float ave_B2_read_average_good_diff[6][24];
  float ave_B2_chan_average_good_diff[6][24];
  float ave_B3_read_average_good_diff[6][24];
  float ave_B3_chan_average_good_diff[6][24];

  TH2F *th2f_map_sensorXladder_B3 = new TH2F("th2f_map_sensorXladder_B3","",6,-0.5,5.5,24,-0.5,23.5);

  float content_B3[6][24];

  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  ave_B2_read_average_good_diff[i][j] = 1-((float)ave_B2_read_count_diff[i][j]/(float)ave_B2_read_total_diff[i][j]);
	  ave_B2_chan_average_good_diff[i][j] = 1-((float)ave_B2_chan_count_diff[i][j]/(float)ave_B2_chan_total_diff[i][j]);
	  ave_B3_read_average_good_diff[i][j] = 1-((float)ave_B3_read_count_diff[i][j]/(float)ave_B3_read_total_diff[i][j]);
	  ave_B3_chan_average_good_diff[i][j] = 1-((float)ave_B3_chan_count_diff[i][j]/(float)ave_B3_chan_total_diff[i][j]);
	  // if(isnan(ave_B3_read_average_good_diff[i][j])) ave_B3_read_average_good_diff[i][j] = 0; // 0/0 = nan...
	  // if(isnan(ave_B3_chan_average_good_diff[i][j])) ave_B3_chan_average_good_diff[i][j] = 0; // 0/0 = nan...
	  if(TMath::IsNaN(ave_B3_read_average_good_diff[i][j])) ave_B3_read_average_good_diff[i][j] = 1; // 0/0 = nan...
	  if(TMath::IsNaN(ave_B3_chan_average_good_diff[i][j])) ave_B3_chan_average_good_diff[i][j] = 1; // 0/0 = nan...
	  // --- now start filling maps...
	  content_B3[i][j] = ave_B3_read_average_good_diff[i][j]*ave_B3_chan_average_good_diff[i][j];
	  th2f_map_sensorXladder_B3->SetBinContent(i+1,j+1,content_B3[i][j]);
	  // ---
	  // if( i == 0 || i == 2 || i == 4 )
	  //   {
	  //     cout << i << " " << j << " "
	  // 	   << content_B3 << " "
	  // 	   << content_B3 << " "
	  // 	   << ave_B3_chan_average_good_diff[i][j] << " "
	  // 	   << ave_B3_chan_count_diff[i][j] << " "
	  // 	   << ave_B3_chan_total_diff[i][j] << " "
	  // 	   << endl;
	  //   }
	}
    }



  // ---
  cout << "Now looking at overall overages" << endl;
  cout << "Product method for ave_B2: " << ave_B2_read_average_good*ave_B2_chan_average_good << endl;
  cout << "Product method for ave_B3: " << ave_B3_read_average_good*ave_B3_chan_average_good << endl;


  cout << "readDeadhotFromDB: now writing histograms to file" << endl;

  TFile *outfile = TFile::Open("rundep.root","RECREATE");
  B2_good->Write();
  B3_good->Write();

  TCanvas *c1 = new TCanvas("c1","",800,600);
  th2f_map_sensorXladder_B3->GetZaxis()->SetRangeUser(-0.001,1.001);
  th2f_map_sensorXladder_B3->Draw("colz");
  c1->Print(Form("%s.png",th2f_map_sensorXladder_B3->GetName()));
  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  TLatex tex1;
	  tex1.DrawLatex(i-0.25,j-0.5,Form("%.1f%%",content_B3[i][j]*100));
	}
    }
  c1->Print(Form("%s_percentages.png",th2f_map_sensorXladder_B3->GetName()));



  cout << "readDeadhotFromDB: completed." << endl;


}
