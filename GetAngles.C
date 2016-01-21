const float pi = TMath::Pi();

void GetAngles(int verbosity = 3)
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


  float sensorwidth[4] = {1.28, 1.28, 3.072, 3.072};

  // --- layer, ladder, and sensor
  for(int h=0; h<4; h++) // layer
    {
      for(int i=0; i<24; i++) // ladder
	{
	  for(int j=0; j<6; j++) // sensor
	    {
	      bool B0 = (h == 0 && i < 10 && j < 4);
	      bool B1 = (h == 1 && i < 20 && j < 4);
	      bool B2 = (h == 2 && i < 16 && j < 5);
	      bool B3 = (h == 3 && i < 24 && j < 6);
	      float phi = geo->SensorPhiRad(h,i,j);
	      float radius = geo->SensorRadius(h,i,j);
	      float range = asin(sensorwidth[h]/radius/2.0);
	      //if(verbosity > 1 && j == 0 && (B0||B1||B2||B3)) cout << "layer " << h <<", ladder " << i << ", deltaphi " << deltaphi << endl;
	      if(verbosity > 3 || (verbosity > 2 && j == 0))
		{
		  if(B0) cout << "B0: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << phi << " +- " << range << endl;
		  if(B1) cout << "B1: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << phi << " +- " << range << endl;
		  if(B2) cout << "B2: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << phi << " +- " << range << endl;
		  if(B3) cout << "B3: layer " << h << ", ladder " << i << ", sensor " << j << ", phi angle = " << phi << " +- " << range << endl;
		} // if verbosity
	    } // sensor
	} // ladder
    } // layer



  float RADIUS[4] = {2.63, 5.13, 11.77, 16.69};
  float LENGTH[4] = {22.8, 22.8, 31.8,  38.2};
  int   NOBINS[4] = {16,16,5,6}; // 4,4,5,6 means sensors, 16,16,5,6 means chips for pixels and sensors for strips

  // --- layer, ladder, and sensor
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
      if(verbosity>3)
      cout << "layer " << h
	   << " eta range is from "   << etamin   << " to " << etamax
	   << " number of z bins is " << nb << " with bin width " << zwidth
	   << endl;
      // ---

      for(int i=0; i<24; i++)
	{
	  for(int j=0; j<16; j++)
	    {
	      bool B0 = (h == 0 && i < 10 && j < nb);
	      bool B1 = (h == 1 && i < 20 && j < nb);
	      bool B2 = (h == 2 && i < 16 && j < nb);
	      bool B3 = (h == 3 && i < 24 && j < nb);
	      // ---
	      float zlo = zmin + j*zwidth;
	      float zhi = zmin + (j+1)*zwidth;
	      float zce = zlo + zcenter;
	      // ---
	      float etalo = -log(tan(atan2(r,zlo)/2.0));
	      float etahi = -log(tan(atan2(r,zhi)/2.0));
	      float etace = -log(tan(atan2(r,zce)/2.0));
	      // ---
	      if(j<nb && i==0 && verbosity > 3)
		cout << " for z bin number " << j
		  //<< " center of bin is " << zce
		  //<< " low edge of bin is " << zlo
		  //<< " high edge of bin is " << zhi
		     << " center of bin is " << etace
		     << " low edge of bin is " << etalo
		     << " high edge of bin is " << etahi
		     << endl;

	    }
	}
    }



}
