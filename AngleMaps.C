const float pi = TMath::Pi();



void AngleMaps(bool verbose = false)
{

  // gSystem->Load("libfun4all.so");
  // gSystem->Load("libsvx.so");
  gSystem->Load("libsvxgeo.so");

  // Geometry object
  SvxTGeo *geo = new SvxTGeo();
  // Read the par file
  geo->ReadParFile("parfiles/svxPISA.par");
  // Make an empty 100x100x100 cm^3 space in the experiment hall.
  geo->MakeTopVolume(200/2, 200/2, 200/2);
  // Place VTX sensors in the volume.
  geo->AddSensors();




  TFile *file = TFile::Open("efficiencies_runs.root");
  // --- B0
  TH2F *th2f_input_B0 = (TH2F *)file->Get("th2f_map_sensorXladder_B0_run415751");
  TH1F *th1f_ladder_B0 = (TH1F *)th2f_input_B0->ProjectionY();
  th1f_ladder_B0->Scale(1.0/th2f_input_B0->GetNbinsX());
  TH1F *th1f_phi_B0 = new TH1F("th1f_phi_B0","",14,-3.14159,3.38241); // the ladder widths are not evenly divisible by 2pi
  // --- B1
  TH2F *th2f_input_B1 = (TH2F *)file->Get("th2f_map_sensorXladder_B1_run415751");
  TH1F *th1f_ladder_B1 = (TH1F *)th2f_input_B1->ProjectionY();
  th1f_ladder_B1->Scale(1.0/th2f_input_B1->GetNbinsX());
  TH1F *th1f_phi_B1 = new TH1F("th1f_phi_B1","",27,-3.14159,3.26726); // the ladder widths are not evenly divisible by 2pi
  // --- B2
  TH2F *th2f_input_B2 = (TH2F *)file->Get("th2f_map_sensorXladder_B2_run415751");
  TH1F *th1f_ladder_B2 = (TH1F *)th2f_input_B2->ProjectionY();
  th1f_ladder_B2->Scale(1.0/th2f_input_B2->GetNbinsX());
  TH1F *th1f_phi_B2 = new TH1F("th1f_phi_B2","",21,-3.14159,3.14159); // the ladder widths are almost evenly divisible by 2pi
  // --- B3
  TH2F *th2f_input_B3 = (TH2F *)file->Get("th2f_map_sensorXladder_B3_run415751");
  TH1F *th1f_ladder_B3 = (TH1F *)th2f_input_B3->ProjectionY();
  th1f_ladder_B3->Scale(1.0/th2f_input_B3->GetNbinsX());
  TH1F *th1f_phi_B3 = new TH1F("th1f_phi_B3","",30,-3.14159,3.14159); // the ladder widths are almost evenly divisible by 2pi

  // --- layer, ladder
  for(int h=0; h<4; h++) // layer
    {
      for(int i=0; i<24; i++) // ladder
	{
	  bool B0 = (h == 0 && i < 10);
	  bool B1 = (h == 1 && i < 20);
	  bool B2 = (h == 2 && i < 16);
	  bool B3 = (h == 3 && i < 24);
	  // ---
	  float phi = geo->SensorPhiRad(h,i,0);
	  float eff = -9;
	  int index = -9;
	  // --- B0
	  if(B0)
	    {
	      eff = th1f_ladder_B0->GetBinContent(i+1);
	      index = th1f_phi_B0->GetXaxis()->FindBin(phi);
	      if(verbose) cout << i << " " << index << " " << phi << " " << eff << endl;
	      th1f_phi_B0->SetBinContent(index,eff);
	    } // if B0
	  // --- B1
	  if(B1)
	    {
	      eff = th1f_ladder_B1->GetBinContent(i+1);
	      index = th1f_phi_B1->GetXaxis()->FindBin(phi);
	      if(verbose) cout << i << " " << index << " " << phi << " " << eff << endl;
	      th1f_phi_B1->SetBinContent(index,eff);
	    } // if B1
	  // --- B2
	  if(B2)
	    {
	      eff = th1f_ladder_B2->GetBinContent(i+1);
	      index = th1f_phi_B2->GetXaxis()->FindBin(phi);
	      if(verbose) cout << i << " " << index << " " << phi << " " << eff << endl;
	      th1f_phi_B2->SetBinContent(index,eff);
	    } // if B2
	  // --- B3
	  if(B3)
	    {
	      eff = th1f_ladder_B3->GetBinContent(i+1);
	      index = th1f_phi_B3->GetXaxis()->FindBin(phi);
	      if(verbose) cout << i << " " << index << " " << phi << " " << eff << endl;
	      th1f_phi_B3->SetBinContent(index,eff);
	    } // if B3
	} // ladder
    } // layer



  th1f_phi_B0->Draw();
  c1->Print("Figures/phi_single_B0.png");

  th1f_phi_B1->Draw();
  c1->Print("Figures/phi_single_B1.png");

  th1f_phi_B2->Draw();
  c1->Print("Figures/phi_single_B2.png");

  th1f_phi_B3->Draw();
  c1->Print("Figures/phi_single_B3.png");



  TH1F *th1f_phi_combined = new TH1F("th1f_phi_combined","",200,-pi,pi);

  for(int i=0; i<200; i++)
    {
      float phi = th1f_phi_combined->GetBinCenter(i+1);
      // --- B0
      int binB0 = th1f_phi_B0->GetXaxis()->FindBin(phi);
      float effB0 = th1f_phi_B0->GetBinContent(binB0);
      // --- B1
      int binB1 = th1f_phi_B1->GetXaxis()->FindBin(phi);
      float effB1 = th1f_phi_B1->GetBinContent(binB1);
      // --- B2
      int binB2 = th1f_phi_B2->GetXaxis()->FindBin(phi);
      float effB2 = th1f_phi_B2->GetBinContent(binB2);
      // --- B3
      int binB3 = th1f_phi_B3->GetXaxis()->FindBin(phi);
      float effB3 = th1f_phi_B3->GetBinContent(binB3);
      // ---
      float effALL = effB0*effB1*effB2*effB3;
      th1f_phi_combined->SetBinContent(i+1,effALL);
    }

  th1f_phi_combined->Draw();
  c1->Print("Figures/phi_combined.png");

}
