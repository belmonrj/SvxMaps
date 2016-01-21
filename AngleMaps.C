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
  TH1F *th1f_phi_B0 = new TH1F("th1f_phi_B0","",400,-pi/2,3*pi/2);
  // --- B1
  TH2F *th2f_input_B1 = (TH2F *)file->Get("th2f_map_sensorXladder_B1_run415751");
  TH1F *th1f_ladder_B1 = (TH1F *)th2f_input_B1->ProjectionY();
  th1f_ladder_B1->Scale(1.0/th2f_input_B1->GetNbinsX());
  TH1F *th1f_phi_B1 = new TH1F("th1f_phi_B1","",400,-pi/2,3*pi/2);
  // --- B2
  TH2F *th2f_input_B2 = (TH2F *)file->Get("th2f_map_sensorXladder_B2_run415751");
  TH1F *th1f_ladder_B2 = (TH1F *)th2f_input_B2->ProjectionY();
  th1f_ladder_B2->Scale(1.0/th2f_input_B2->GetNbinsX());
  TH1F *th1f_phi_B2 = new TH1F("th1f_phi_B2","",400,-pi/2,3*pi/2);
  // --- B3
  TH2F *th2f_input_B3 = (TH2F *)file->Get("th2f_map_sensorXladder_B3_run415751");
  TH1F *th1f_ladder_B3 = (TH1F *)th2f_input_B3->ProjectionY();
  th1f_ladder_B3->Scale(1.0/th2f_input_B3->GetNbinsX());
  TH1F *th1f_phi_B3 = new TH1F("th1f_phi_B3","",400,-pi/2,3*pi/2);

  float phiwidth[8] = {
    0.245814, // layer 0, sublayer 0
    0.125082, // layer 1, sublayer 1
    0.148811, // layer 2, sublayer 2 (inner)
    0.130931, // layer 2, sublayer 3 (middle)
    0.119838, // layer 2, sublayer 4 (outer)
    0.099434, // layer 3, sublayer 5 (inner)
    0.092178, // layer 3, sublayer 6 (middle)
    0.085910  // layer 3, sublayer 7 (outer)
  };

  // --- layer, ladder
  for(int h=0; h<4; h++) // layer
    {
      for(int i=0; i<24; i++) // ladder
	{
	  bool B0 = (h == 0 && i < 10);
	  bool B1 = (h == 1 && i < 20);
	  bool B2 = (h == 2 && i < 16);
	  bool B3 = (h == 3 && i < 24);
	  if(!B0 && !B1 && !B2 && !B3) continue;
	  // ---
	  int sublayer = h;
	  if(h==2 && (i==1 || i==4 || i==7 || i==8 || i==11 || i==14)) sublayer = 2; // inner
	  if(h==2 && (i==0 || i==3 || i==6 || i==9 || i==12 || i==15)) sublayer = 3; // middle
	  if(h==2 && (i==2 || i==5 || i==10 || i==13))                 sublayer = 4; // outer
	  if(h==3 && (i==0 || i==3 || i==6 || i==9  || i==14 || i==17 || i==20 || i==23)) sublayer = 5; // inner
	  if(h==3 && (i==1 || i==4 || i==7 || i==10 || i==13 || i==16 || i==19 || i==22)) sublayer = 6; // middle
	  if(h==3 && (i==2 || i==5 || i==8 || i==11 || i==12 || i==15 || i==18 || i==21)) sublayer = 7; // outer
	  float hilo = phiwidth[sublayer];
	  // ---
	  float phi = geo->SensorPhiRad(h,i,0);
	  if(phi < -pi/2) phi += 2*pi;
	  float eff = -9;
	  int indexhi = -9;
	  int indexlo = -9;
	  // --- B0
	  if(B0)
	    {
	      eff = th1f_ladder_B0->GetBinContent(i+1);
	      indexlo = th1f_phi_B0->GetXaxis()->FindBin(phi-hilo);
	      indexhi = th1f_phi_B0->GetXaxis()->FindBin(phi+hilo);
	      for(int j=indexlo; j<indexhi; j++) th1f_phi_B0->SetBinContent(j,eff);
	    } // if B0
	  // --- B1
	  if(B1)
	    {
	      eff = th1f_ladder_B1->GetBinContent(i+1);
	      indexlo = th1f_phi_B1->GetXaxis()->FindBin(phi-hilo);
	      indexhi = th1f_phi_B1->GetXaxis()->FindBin(phi+hilo);
	      for(int j=indexlo; j<indexhi; j++) th1f_phi_B1->SetBinContent(j,eff);
	    } // if B1
	  // --- B2
	  if(B2)
	    {
	      eff = th1f_ladder_B2->GetBinContent(i+1);
	      indexlo = th1f_phi_B2->GetXaxis()->FindBin(phi-hilo);
	      indexhi = th1f_phi_B2->GetXaxis()->FindBin(phi+hilo);
	      for(int j=indexlo; j<indexhi; j++) th1f_phi_B2->SetBinContent(j,eff);
	    } // if B2
	  // --- B3
	  if(B3)
	    {
	      eff = th1f_ladder_B3->GetBinContent(i+1);
	      indexlo = th1f_phi_B3->GetXaxis()->FindBin(phi-hilo);
	      indexhi = th1f_phi_B3->GetXaxis()->FindBin(phi+hilo);
	      for(int j=indexlo; j<indexhi; j++) th1f_phi_B3->SetBinContent(j,eff);
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



  TH1F *th1f_phi_ALL = (TH1F *)th1f_phi_B0->Clone("th1f_phi_ALL");
  th1f_phi_ALL->Multiply(th1f_phi_B1);
  th1f_phi_ALL->Multiply(th1f_phi_B2);
  th1f_phi_ALL->Multiply(th1f_phi_B2);
  th1f_phi_ALL->SetLineColor(kBlack);
  th1f_phi_ALL->Draw();

  c1->Print("Figures/phi_combined.png");



  th1f_phi_B0->SetLineColor(kBlue);
  th1f_phi_B1->SetLineColor(kRed);
  th1f_phi_B2->SetLineColor(kGreen+2);
  th1f_phi_B3->SetLineColor(kMagenta);
  th1f_phi_ALL->SetLineColor(kBlack);
  th1f_phi_B0->Draw();
  th1f_phi_B1->Draw("same");
  th1f_phi_B2->Draw("same");
  th1f_phi_B3->Draw("same");
  th1f_phi_ALL->Draw("same");

  c1->Print("Figures/phi_ALL.png");



  // -----------
  // --- now eta
  // -----------


  // --- B0
  TH1F *th1f_sensor_B0 = (TH1F *)th2f_input_B0->ProjectionX();
  th1f_sensor_B0->Scale(1.0/th2f_input_B0->GetNbinsY());
  TH1F *th1f_eta_B0 = new TH1F("th1f_eta_B0","",500,-2.5,2.5);
  // --- B1
  TH1F *th1f_sensor_B1 = (TH1F *)th2f_input_B1->ProjectionX();
  th1f_sensor_B1->Scale(1.0/th2f_input_B1->GetNbinsY());
  TH1F *th1f_eta_B1 = new TH1F("th1f_eta_B1","",500,-2.5,2.5);
  // --- B2
  TH1F *th1f_sensor_B2 = (TH1F *)th2f_input_B2->ProjectionX();
  th1f_sensor_B2->Scale(1.0/th2f_input_B2->GetNbinsY());
  TH1F *th1f_eta_B2 = new TH1F("th1f_eta_B2","",500,-2.5,2.5);
  // --- B3
  TH1F *th1f_sensor_B3 = (TH1F *)th2f_input_B3->ProjectionX();
  th1f_sensor_B3->Scale(1.0/th2f_input_B3->GetNbinsY());
  TH1F *th1f_eta_B3 = new TH1F("th1f_eta_B3","",500,-2.5,2.5);



  float RADIUS[4] = {2.63, 5.13, 11.77, 16.69};
  float LENGTH[4] = {22.8, 22.8, 31.8,  38.2};
  int   NOBINS[4] = {16,16,5,6}; // 4,4,5,6 means sensors, 16,16,5,6 means chips for pixels and sensors for strips

  // --- layer, sensor
  for(int h=0; h<4; h++)
    {
      int nb = NOBINS[h];
      float r = RADIUS[h];
      float z = LENGTH[h];
      float zmax = z/2.0;
      float zmin = -zmax;
      float thetamax = atan2(r,zmax);
      float thetamin = atan2(r,zmin);
      float etamax = -log(tan(thetamax/2.0));
      float etamin = -log(tan(thetamin/2.0));
      float relthetamax = thetamax - pi/2.0;
      float relthetamin = thetamin - pi/2.0;
      // ---
      float div = (float)nb;
      float zwidth = z/div;
      float zcenter = zwidth/2.0;
      // ---
      for(int i=0; i<16; i++)
	{
	  bool B0 = (h == 0 && i < nb);
	  bool B1 = (h == 1 && i < nb);
	  bool B2 = (h == 2 && i < nb);
	  bool B3 = (h == 3 && i < nb);
	  // ---
	  float zlo = zmin + i*zwidth;
	  float zhi = zmin + (i+1)*zwidth;
	  float zce = zlo + zcenter;
	  // ---
	  float etalo = -log(tan(atan2(r,zlo)/2.0));
	  float etahi = -log(tan(atan2(r,zhi)/2.0));
	  float etace = -log(tan(atan2(r,zce)/2.0));
	  // ---
	  float eff = -9;
	  int indexhi = -9;
	  int indexlo = -9;
	  // --- B0
	  if(B0)
	    {
	      eff = th1f_sensor_B0->GetBinContent(i+1);
	      indexlo = th1f_eta_B0->GetXaxis()->FindBin(etalo);
	      indexhi = th1f_eta_B0->GetXaxis()->FindBin(etahi);
	      for(int j=indexlo; j<indexhi; j++) th1f_eta_B0->SetBinContent(j,eff);
	    } // if B0
	  // --- B1
	  if(B1)
	    {
	      eff = th1f_sensor_B1->GetBinContent(i+1);
	      indexlo = th1f_eta_B1->GetXaxis()->FindBin(etalo);
	      indexhi = th1f_eta_B1->GetXaxis()->FindBin(etahi);
	      for(int j=indexlo; j<indexhi; j++) th1f_eta_B1->SetBinContent(j,eff);
	    } // if B1
	  // --- B2
	  if(B2)
	    {
	      eff = th1f_sensor_B2->GetBinContent(i+1);
	      indexlo = th1f_eta_B2->GetXaxis()->FindBin(etalo);
	      indexhi = th1f_eta_B2->GetXaxis()->FindBin(etahi);
	      for(int j=indexlo; j<indexhi; j++) th1f_eta_B2->SetBinContent(j,eff);
	    } // if B2
	  // --- B3
	  if(B3)
	    {
	      eff = th1f_sensor_B3->GetBinContent(i+1);
	      indexlo = th1f_eta_B3->GetXaxis()->FindBin(etalo);
	      indexhi = th1f_eta_B3->GetXaxis()->FindBin(etahi);
	      for(int j=indexlo; j<indexhi; j++) th1f_eta_B3->SetBinContent(j,eff);
	    } // if B3
	} // sensor
    } // layer



  th1f_eta_B0->Draw();
  c1->Print("Figures/eta_single_B0.png");

  th1f_eta_B1->Draw();
  c1->Print("Figures/eta_single_B1.png");

  th1f_eta_B2->Draw();
  c1->Print("Figures/eta_single_B2.png");

  th1f_eta_B3->Draw();
  c1->Print("Figures/eta_single_B3.png");



  TH1F *th1f_eta_ALL = (TH1F *)th1f_eta_B0->Clone("th1f_eta_ALL");
  th1f_eta_ALL->Multiply(th1f_eta_B1);
  th1f_eta_ALL->Multiply(th1f_eta_B2);
  th1f_eta_ALL->Multiply(th1f_eta_B2);
  th1f_eta_ALL->SetLineColor(kBlack);
  th1f_eta_ALL->Draw();

  c1->Print("Figures/eta_combined.png");



  th1f_eta_B0->SetLineColor(kBlue);
  th1f_eta_B1->SetLineColor(kRed);
  th1f_eta_B2->SetLineColor(kGreen+2);
  th1f_eta_B3->SetLineColor(kMagenta);
  th1f_eta_ALL->SetLineColor(kBlack);
  th1f_eta_B0->Draw();
  th1f_eta_B1->Draw("same");
  th1f_eta_B2->Draw("same");
  th1f_eta_B3->Draw("same");
  th1f_eta_ALL->Draw("same");

  c1->Print("Figures/eta_ALL.png");



  TFile *file = new TFile("histograms.root","recreate");
  file->cd();
  th1f_phi_B0->Write();
  th1f_phi_B1->Write();
  th1f_phi_B2->Write();
  th1f_phi_B3->Write();
  th1f_phi_ALL->Write();
  th1f_eta_B0->Write();
  th1f_eta_B1->Write();
  th1f_eta_B2->Write();
  th1f_eta_B3->Write();
  th1f_eta_ALL->Write();
  file->Write();
  file->Close();

}
