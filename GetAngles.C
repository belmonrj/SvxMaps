void GetAngles()
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



  // --- layer, ladder, and sensor
  for(int h=0; h<4; h++)
    {
      for(int i=0; i<24; i++)
	{
	  for(int j=0; j<6; j++)
	    {
	      bool B0 = (h == 0 && i < 10 && j < 4);
	      bool B1 = (h == 1 && i < 20 && j < 4);
	      bool B2 = (h == 2 && i < 16 && j < 5);
	      bool B3 = (h == 3 && i < 24 && j < 6);
	      if(B0) cout << "B0: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << geo->SensorPhiRad(h,i,j) << endl;
	      if(B1) cout << "B1: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << geo->SensorPhiRad(h,i,j) << endl;
	      if(B2) cout << "B2: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << geo->SensorPhiRad(h,i,j) << endl;
	      if(B3) cout << "B3: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << geo->SensorPhiRad(h,i,j) << endl;
	    }
	}
    }

}
