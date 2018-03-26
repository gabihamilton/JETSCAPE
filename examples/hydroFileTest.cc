// -----------------------------------------
// JetScape (modular/task) based framework
// Intial Design: Joern Putschke (2017)
//                (Wayne State University)
// -----------------------------------------
// License and Doxygen-like Documentation to be added ...

// ------------------------------------------------------------
// JetScape Framework hydro from file Test Program
// (use either shared library (need to add paths; see setup.csh)
// (or create static library and link in)
// -------------------------------------------------------------

#include <iostream>
#include <time.h>
#include <chrono>
#include <thread>

// JetScape Framework includes ...
#include "JetScape.h"
#include "JetEnergyLoss.h"
#include "JetEnergyLossManager.h"
#include "JetScapeWriterAscii.h"
//#include "JetScapeWriterAsciiGZ.h"
//#include "JetScapeWriterHepMC.h"

// User modules derived from jetscape framework clasess
// to be used to run Jetscape ...
#include "AdSCFT.h"
#include "Matter.h"
#include "Martini.h"
#include "brick_jetscape.h"
#include "Gubser_hydro_jetscape.h"
#include "hydro_file_jetscape.h"
#include "PGun.h"

#ifdef USE_HDF5
#include "InitialFromFile.h"
#endif
// using namespace std;

using namespace Jetscape;

// Forward declaration
void Show();

// -------------------------------------

int main(int argc, char** argv)
{
  clock_t t; t = clock();
  time_t start, end; time(&start);
  
  cout<<endl;
    
  // DEBUG=true by default and REMARK=false
  // can be also set also via XML file (at least partially)
  JetScapeLogger::Instance()->SetDebug(false);
  JetScapeLogger::Instance()->SetRemark(false);
  //SetVerboseLevel (9 a lot of additional debug output ...)
  //If you want to suppress it: use SetVerboseLevle(0) or max  SetVerboseLevle(9) or 10
  JetScapeLogger::Instance()->SetVerboseLevel(0);
   
  Show();

  auto jetscape = make_shared<JetScape>("./jetscape_init.xml", 10);
  jetscape->set_reuse_hydro (true);
  jetscape->set_n_reuse_hydro (5);

  auto jlossmanager = make_shared<JetEnergyLossManager> ();
  auto jloss = make_shared<JetEnergyLoss> ();
  auto hydro = make_shared<HydroFile> ();
  //auto hydro = make_shared<GubserHydro> ();
  
  auto matter = make_shared<Matter> ();
  auto martini = make_shared<Martini> ();
  auto adscft = make_shared<AdSCFT> ();
  //DBEUG: Remark:
  //does not matter unfortunately since not called recursively, done by JetEnergyLoss class ...
  //matter->SetActive(false);
  //martini->SetActive(false);
  // This works ... (check with above logic ...)
  //jloss->SetActive(false);
  //

  auto pGun= make_shared<PGun> ();

  // only pure Ascii writer implemented and working with graph output ...
  auto writer= make_shared<JetScapeWriterAscii> ("test_out.dat");
  //auto writer= make_shared<JetScapeWriterAsciiGZ> ("test_out.dat.gz");  
  //auto writer= make_shared<JetScapeWriterHepMC> ("test_out.dat");
  //writer->SetActive(false);

  //Remark: For now modules have to be added
  //in proper "workflow" order (can be defined via xml and sorted if necessary)
  
#ifdef USE_HDF5
  auto initial = make_shared<InitialFromFile>();
  jetscape->Add(initial);
#endif

  jetscape->Add(pGun);

   //Some modifications will be needed for reusing hydro events, so far
  //simple test hydros always executed "on the fly" ...
  jetscape->Add(hydro);

  // Matter with silly "toy shower (no physics)
  // and Martini dummy ...
  // Switching Q2 (or whatever variable used
  // hardcoded at 5 to be changed to xml)
  //jloss->Add(matter);
  //jloss->Add(martini);
  jloss->Add(adscft);
  
  jlossmanager->Add(jloss);
  
  jetscape->Add(jlossmanager);
  
  jetscape->Add(writer);

  // Intialize all modules tasks
  jetscape->Init();

  // Run JetScape with all task/modules as specified ...
  jetscape->Exec();

  // "dummy" so far ...
  // Most thinkgs done in write and clear ...
  jetscape->Finish();
  
  INFO_NICE<<"Finished!";
  cout<<endl;

  // wait for 5s
  //std::this_thread::sleep_for(std::chrono::milliseconds(500000));

  t = clock() - t;
  time(&end);
  printf ("CPU time: %f seconds.\n",((float)t)/CLOCKS_PER_SEC);
  printf ("Real time: %f seconds.\n",difftime(end,start));
  //printf ("Real time: %f seconds.\n",(start-end));
  return 0;
}

// -------------------------------------

void Show()
{
  INFO_NICE<<"-----------------------------------------------";
  INFO_NICE<<"| Hydro from file Test JetScape Framework ... |";
  INFO_NICE<<"-----------------------------------------------";
  INFO_NICE;
}
