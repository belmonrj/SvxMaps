void DoItByRunNumber();
void DoItByRunNumber(const int);


void MakeSimpleMaps()
{

  gStyle->SetOptStat(0);

  gSystem->Load("libfun4all.so");
  gSystem->Load("libsvx.so");

  DoItByRunNumber(415751); // Run14 3He+Au
  DoItByRunNumber(423844); // Run15 p+p
  DoItByRunNumber(408644); // Run14 Au+Au

}


void DoItByRunNumber()
{

  DoItByRunNumber(415751); // 3He+Au

}


void DoItByRunNumber(const int run)
{

  // --- make TCanvas for drawing plots

  TCanvas *c1 = new TCanvas("c1","",900,700);

  // --- get the pixel map object, the address object, and read from the DB

  SvxPixelHotDeadMapv2 pixelMap(0);
  svxAddress address;
  //address.Initialize(); // necessary?
  if(!pixelMap.readFromDatabase(run,true))
    {
      cout << "cannot read from database, stopping" << endl;
      return;
    }
  pixelMap.setTileMap();

  // --- declare variables for determining efficiency

  int B0_chip_count = 0;
  int B1_chip_count = 0;
  int B0_chip_total = 0;
  int B1_chip_total = 0;

  int B0_chip_count_diff[16][20];
  int B1_chip_count_diff[16][20];
  int B0_chip_total_diff[16][20];
  int B1_chip_total_diff[16][20];

  for(int i=0; i<16; i++)
    {
      for(int j=0; j<20; j++)
	{
	  // ---
	  B0_chip_count_diff[i][j] = 0;
	  B1_chip_count_diff[i][j] = 0;
	  // ---
	  B0_chip_total_diff[i][j] = 0;
	  B1_chip_total_diff[i][j] = 0;
	}
    }

  // --- declare variables for the loop

  int nLayer = 2;
  int nLadder = 20;
  int nSensor = 4;
  int nChip = 4;
  int nColumn = 32;
  int nRow = 256;

  // --- loop over layers, ladders, sensors, chips, and pixels

  int wtfmate = 0;

  for(int iLayer=0; iLayer<nLayer; iLayer++) { // Layer
    for(int iLadder=0; iLadder<nLadder; iLadder++) { // Ladder
      bool B0 = ( iLayer == 0 && iLadder < 10 );
      bool B1 = ( iLayer == 1 );
      if( !B0 && !B1 ) continue;
      for(int iSensor=0; iSensor<nSensor; iSensor++) { // Sensor
	int module = address.getPixelModuleID(iLayer,iLadder,iSensor); // get the module number based on layer, ladder, sensor
	for(int iChip=0; iChip<nChip; iChip++) { // Chip (ROC)
	  int ROC = iChip;
	  if( iSensor%2 ) ROC += 4;
	  int ChipIndex = iSensor*nChip + iChip;
	  // --- some diagnostics, testing the DB and comparing to the QA webpage
	  // if(iLayer == 0 && iLadder == 8 && ChipIndex == 3){
	  //   for(int iTile=0; iTile<16; iTile++)
	  //     {
	  // 	cout << module << " " << ROC << " " << iTile << " "
	  // 	     << pixelMap.getTileGoodFrac(module,ROC,iTile) << endl;
	  //     }
	  // }
	  for(int iColumn=0; iColumn<nColumn; iColumn++) { // column (32)
	    for(int iRow=0; iRow<nRow; iRow++) { // row (256*32 = 8192)
	      int chipStatus = pixelMap.getChipStatus(module,ROC);
	      int pixelStatus = pixelMap.getPixelStatus(module,ROC,iColumn,iRow);
	      // --- some diagnostics, testing the DB and comparing to the QA webpage
	      //if(iLayer == 0 && iLadder == 8 && ChipIndex == 3 && iRow == 74 && iColumn == 20)
	      // if(iLayer == 0 && iLadder == 8 && ChipIndex == 3 && (chipStatus !=0 || pixelStatus !=0 ) )
	      // 	{
	      // 	  cout << wtfmate << " " << iSensor << " "
	      // 	       << module << " " << ROC << " "
	      // 	       << chipStatus << " " << pixelStatus << " "
	      // 	       << iRow << " " << iColumn << " "
	      // 	       << endl;
	      // 	  wtfmate++;
	      // 	}
	      bool isgood = (chipStatus == 0 && pixelStatus == 0);
	      //bool isgood = pixelMap.isPixelOkForClustering(module,ROC,iColumn,iRow);
	      // --- overall...
	      if(B0) B0_chip_total++;
	      if(B1) B1_chip_total++;
	      if(B0 && isgood) B0_chip_count++;
	      if(B1 && isgood) B1_chip_count++;
	      // --- position...
	      if(B0) B0_chip_total_diff[ChipIndex][iLadder]++;
	      if(B1) B1_chip_total_diff[ChipIndex][iLadder]++;
	      if(B0 && isgood) B0_chip_count_diff[ChipIndex][iLadder]++;
	      if(B1 && isgood) B1_chip_count_diff[ChipIndex][iLadder]++;
	    } // Row
	  } // Column
	} // Chip (ROC)
      } // Sensor
    } // Ladder
  } // Layer

  cout << B0_chip_count_diff[3][8] << " " << B0_chip_total_diff[3][8] << endl;

  // --- now calculate the efficiencies from the counts

  float B0_chip_average_good = ((float)B0_chip_count/(float)B0_chip_total);
  float B1_chip_average_good = ((float)B1_chip_count/(float)B1_chip_total);
  cout << "Product method for B0: " << B0_chip_average_good << endl;
  cout << "Product method for B1: " << B1_chip_average_good << endl;

  TH2F *th2f_map_sensorXladder_B0 = new TH2F(Form("th2f_map_sensorXladder_B0_run%d",run),"",16,-0.5,15.5,10,-0.5,9.5);
  TH2F *th2f_map_sensorXladder_B1 = new TH2F(Form("th2f_map_sensorXladder_B1_run%d",run),"",16,-0.5,15.5,20,-0.5,19.5);
  float B0_chip_average_good_diff[16][20];
  float B1_chip_average_good_diff[16][20];
  float content_B0[16][20];
  float content_B1[16][20];
  for(int i=0; i<16; i++)
    {
      for(int j=0; j<20; j++)
	{
	  B0_chip_average_good_diff[i][j] = ((float)B0_chip_count_diff[i][j]/(float)B0_chip_total_diff[i][j]);
	  B1_chip_average_good_diff[i][j] = ((float)B1_chip_count_diff[i][j]/(float)B1_chip_total_diff[i][j]);
	  //if(TMath::IsNaN(B0_chip_average_good_diff[i][j])) B0_chip_average_good_diff[i][j] = 0; // 0/0 = nan...
	  //if(TMath::IsNaN(B1_chip_average_good_diff[i][j])) B1_chip_average_good_diff[i][j] = 0; // 0/0 = nan...
	  // --- now start filling maps...
	  if( j < 10 )
	    {
	      content_B0[i][j] = B0_chip_average_good_diff[i][j];
	      th2f_map_sensorXladder_B0->SetBinContent(i+1,j+1,content_B0[i][j]);
	    }
	  content_B1[i][j] = B1_chip_average_good_diff[i][j];
	  th2f_map_sensorXladder_B1->SetBinContent(i+1,j+1,content_B1[i][j]);
	}
    }

  // --- now make plots

  // --- B0
  th2f_map_sensorXladder_B0->GetZaxis()->SetRangeUser(-0.001,1.001);
  th2f_map_sensorXladder_B0->Draw("colz");
  // c1->Print(Form("Figures/%s.png",th2f_map_sensorXladder_B0->GetName()));
  for(int i=0; i<16; i++)
    {
      for(int j=0; j<10; j++)
	{
	  TLatex tex1;
	  tex1.SetTextSize(0.03);
	  tex1.DrawLatex(i-0.5,j-0.15,Form("%.0f%%",content_B0[i][j]*100));
	}
    }
  c1->Print(Form("Figures/%s_percentages.png",th2f_map_sensorXladder_B0->GetName()));
  // --- B1
  th2f_map_sensorXladder_B1->GetZaxis()->SetRangeUser(-0.001,1.001);
  th2f_map_sensorXladder_B1->Draw("colz");
  // c1->Print(Form("Figures/%s.png",th2f_map_sensorXladder_B1->GetName()));
  for(int i=0; i<16; i++)
    {
      for(int j=0; j<20; j++)
	{
	  TLatex tex1;
	  tex1.SetTextSize(0.03);
	  tex1.DrawLatex(i-0.5,j-0.25,Form("%.0f%%",content_B1[i][j]*100));
	}
    }
  c1->Print(Form("Figures/%s_percentages.png",th2f_map_sensorXladder_B1->GetName()));



  // --- COME BACK HERE

  // --- get the pixel map object, the address object, and read from the DB

  SvxDeadMap deadMap(0);

  RunToTime *rt = RunToTime::instance();
  PHTimeStamp *tStamp = rt->getBeginTime(run);
  if(!deadMap.readFromDatabase(tStamp))
    {
      cout << "could not read from database, stopping" << endl;
      return;
    }
  if(!deadMap.readReadoutsFromDatabase(tStamp))
    {
      cout << "could not read from database, stopping" << endl;
      return;
    }

  // --- declare variables for determining efficiency

  int B2_chan_count = 0;
  int B3_chan_count = 0;
  int B2_chan_total = 0;
  int B3_chan_total = 0;

  int B2_chan_count_diff[6][24];
  int B3_chan_count_diff[6][24];
  int B2_chan_total_diff[6][24];
  int B3_chan_total_diff[6][24];

  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  // ---
	  B2_chan_count_diff[i][j] = 0;
	  B3_chan_count_diff[i][j] = 0;
	  // ---
	  B2_chan_total_diff[i][j] = 0;
	  B3_chan_total_diff[i][j] = 0;
	}
    }

  // --- declare variables for the loop

  nLayer = 2;
  nLadder = 24;
  nSensor = 6;
  int nSection = 2;
  int nReadout = 2;
  int nChannel = 384;

  // --- loop over layers, ladders, sensors, chans, and pixels

  for(int iLayer=0; iLayer<nLayer; iLayer++) { // Layer
    for(int iLadder=0; iLadder<nLadder; iLadder++) { // Ladder
      for(int iSensor=0; iSensor<nSensor; iSensor++) { // Sensor
	bool B2 = ( iLayer == 0 && iLadder < 16 && iSensor < 5 );
	bool B3 = ( iLayer == 1 );
	if( !B2 && !B3 ) continue;
	for(int iSection=0; iSection<nSection; iSection++) { // Section
	  for(int iReadout=0; iReadout<nReadout; iReadout++) { // Readout
	    for(int iChannel=0; iChannel<nChannel; iChannel++) { // Channel
	      int readoutStatus = deadMap.readoutStatus(iLayer,iLadder,iSensor,iSection,iReadout);
	      int channelStatus = deadMap.channelStatus(iLayer,iLadder,iSensor,iSection,iReadout,iChannel);
	      bool isgood = (readoutStatus == 0 && channelStatus == 0);
	      // --- overall...
	      if(B2) B2_chan_total++;
	      if(B3) B3_chan_total++;
	      if(B2 && isgood) B2_chan_count++;
	      if(B3 && isgood) B3_chan_count++;
	      // --- position...
	      if(B2) B2_chan_total_diff[iSensor][iLadder]++;
	      if(B3) B3_chan_total_diff[iSensor][iLadder]++;
	      if(B2 && isgood) B2_chan_count_diff[iSensor][iLadder]++;
	      if(B3 && isgood) B3_chan_count_diff[iSensor][iLadder]++;
	    } // Channel
	  } // Readout
	} // Section
      } // Sensor
    } // Ladder
  } // Layer

  // --- now calculate the efficiencies from the counts

  float B2_chan_average_good = ((float)B2_chan_count/(float)B2_chan_total);
  float B3_chan_average_good = ((float)B3_chan_count/(float)B3_chan_total);
  cout << "Product method for B2: " << B2_chan_average_good << endl;
  cout << "Product method for B3: " << B3_chan_average_good << endl;

  TH2F *th2f_map_sensorXladder_B2 = new TH2F(Form("th2f_map_sensorXladder_B2_run%d",run),"",5,-0.5,4.5,16,-0.5,15.5);
  TH2F *th2f_map_sensorXladder_B3 = new TH2F(Form("th2f_map_sensorXladder_B3_run%d",run),"",6,-0.5,5.5,24,-0.5,23.5);
  float B2_chan_average_good_diff[6][24];
  float B3_chan_average_good_diff[6][24];
  float content_B2[6][24];
  float content_B3[6][24];
  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  B2_chan_average_good_diff[i][j] = ((float)B2_chan_count_diff[i][j]/(float)B2_chan_total_diff[i][j]);
	  B3_chan_average_good_diff[i][j] = ((float)B3_chan_count_diff[i][j]/(float)B3_chan_total_diff[i][j]);
	  // if(TMath::IsNaN(B2_chan_average_good_diff[i][j])) B2_chan_average_good_diff[i][j] = 1; // 0/0 = nan...
	  // if(TMath::IsNaN(B3_chan_average_good_diff[i][j])) B3_chan_average_good_diff[i][j] = 1; // 0/0 = nan...
	  // --- now start filling maps...
	  if( i < 5 && j < 16 )
	    {
	      content_B2[i][j] = B2_chan_average_good_diff[i][j];
	      th2f_map_sensorXladder_B2->SetBinContent(i+1,j+1,content_B2[i][j]);
	    }
	  content_B3[i][j] = B3_chan_average_good_diff[i][j];
	  th2f_map_sensorXladder_B3->SetBinContent(i+1,j+1,content_B3[i][j]);
	}
    }

  // --- now make plots

  // --- B2
  th2f_map_sensorXladder_B2->GetZaxis()->SetRangeUser(-0.001,1.001);
  th2f_map_sensorXladder_B2->Draw("colz");
  // c1->Print(Form("Figures/%s.png",th2f_map_sensorXladder_B2->GetName()));
  for(int i=0; i<5; i++)
    {
      for(int j=0; j<16; j++)
	{
	  TLatex tex1;
	  tex1.SetTextSize(0.03);
	  tex1.DrawLatex(i-0.5,j-0.15,Form("%.0f%%",content_B2[i][j]*100));
	}
    }
  c1->Print(Form("Figures/%s_percentages.png",th2f_map_sensorXladder_B2->GetName()));
  // --- B3
  th2f_map_sensorXladder_B3->GetZaxis()->SetRangeUser(-0.001,1.001);
  th2f_map_sensorXladder_B3->Draw("colz");
  // c1->Print(Form("Figures/%s.png",th2f_map_sensorXladder_B3->GetName()));
  for(int i=0; i<6; i++)
    {
      for(int j=0; j<24; j++)
	{
	  TLatex tex1;
	  tex1.SetTextSize(0.03);
	  tex1.DrawLatex(i-0.5,j-0.25,Form("%.0f%%",content_B3[i][j]*100));
	}
    }
  c1->Print(Form("Figures/%s_percentages.png",th2f_map_sensorXladder_B3->GetName()));

  delete c1;

}
