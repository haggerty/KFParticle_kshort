/*
 * Fun4All macro to run KFParticle K_S0 -> pi^+ pi^- reconstruction
 * from a pre-reconstructed track DST (DST_TRKR_TRACKS) for Run 3 p+p data.
 *
 * K_S0 is a displaced decay, so no primary vertex constraint is applied.
 *
 * Usage:
 *   root -b -q 'Fun4All_KShortReco_run3pp.C(1000)'
 */

#include <GlobalVariables.C>

#include <G4_ActsGeom.C>
#include <G4_Magnet.C>
#include <QA.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

#include <globalvertex/GlobalVertexReco.h>

#include <cdbobjects/CDBTTree.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundefined-internal"
#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
R__LOAD_LIBRARY(libtrackingqa.so)

void Fun4All_KShortReco_run3pp(
    const int nEvents = 0,
    const std::string &inputDST = "DST_TRKR_TRACKS_run3pp_ana538_2025p011_v001-00079709-08158.root",
    const std::string &inputDir = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_TRACKS/run_00079700_00079800/",
    const int nSkip = 0,
    const std::string &fieldmap = "FIELDMAP_TRACKING")
{
  std::string inputFile = inputDir + inputDST;

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputDST);
  int runnumber = runseg.first;
  int segment = runseg.second;

  std::stringstream nice_runnumber;
  nice_runnumber << std::setw(8) << std::setfill('0') << runnumber;
  std::stringstream nice_segment;
  nice_segment << std::setw(5) << std::setfill('0') << segment;
  std::stringstream nice_skip;
  nice_skip << std::setw(5) << std::setfill('0') << nSkip;

  // --- Output paths ---
  std::string output_dir = "./";
  std::string reco_name = "KShort_run3pp";
  std::string processing_folder = "inReconstruction/";
  std::string trailer = "_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + ".root";

  std::string output_file_name = "outputKFParticle_" + reco_name + trailer;
  std::string output_reco_dir = output_dir + reco_name + "/" + processing_folder;
  std::string output_reco_file = output_reco_dir + output_file_name;

  system(("mkdir -p " + output_reco_dir).c_str());

  // --- Setup ---
  TRACKING::pp_mode = true;
  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

  auto *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  TpcReadoutInit(runnumber);
  std::cout << " run: " << runnumber
            << " samples: " << TRACKING::reco_tpc_maxtime_sample
            << " pre: " << TRACKING::reco_tpc_time_presample
            << " vdrift: " << G4TPC::tpc_drift_velocity_reco
            << std::endl;

  auto *se = Fun4AllServer::instance();
  se->Verbosity(1);

  // --- Geometry ---
  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  TrackingInit();

  // --- Input DST ---
  auto *tracks = new Fun4AllDstInputManager("TrackInputManager");
  tracks->fileopen(inputFile);
  se->registerInputManager(tracks);

  // --- Global vertex ---
  GlobalVertexReco *gblvertex = new GlobalVertexReco();
  gblvertex->Verbosity(0);
  se->registerSubsystem(gblvertex);

  // --- KFParticle: K_S0 -> pi^+ pi^- (no PV constraint) ---
  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(reco_name);
  kfparticle->Verbosity(0);

  kfparticle->setDecayDescriptor("K_S0 -> pi^+ pi^-");

  // Node / input configuration
  kfparticle->magFieldFile(fieldmap);
  kfparticle->usePID(false);
  kfparticle->allowZeroMassTracks();
  kfparticle->dontUseGlobalVertex(true);
  kfparticle->requireTrackVertexBunchCrossingMatch(true);
  kfparticle->getAllPVInfo(false);
  kfparticle->use2Dmatching(false);
  kfparticle->getTriggerInfo(false);
  kfparticle->getDetectorInfo(false);
  kfparticle->saveDST(false);
  kfparticle->setContainerName(reco_name);

  // PV-to-SV cuts: no PV constraint (K_S0 is displaced)
  kfparticle->constrainToPrimaryVertex(false);
  kfparticle->setMotherIPchi2(FLT_MAX);
  kfparticle->setFlightDistancechi2(-1.);
  kfparticle->setMinDIRA(-1.1);
  kfparticle->setDecayLengthRange(0., FLT_MAX);
  kfparticle->setDecayTimeRange(-1. * FLT_MAX, FLT_MAX);

  // Track quality cuts
  kfparticle->setMinMVTXhits(0);
  kfparticle->setMinTPChits(20);
  kfparticle->setMinimumTrackPT(0.0);
  kfparticle->setMinimumTrackIPchi2(-1.);
  kfparticle->setMinimumTrackIP(-1.);
  kfparticle->setMaximumTrackchi2nDOF(100.);

  // Decay vertex cuts
  kfparticle->setMaximumVertexchi2nDOF(50.);
  kfparticle->setMaximumDaughterDCA(1.0);  // 1 cm

  // Mother / mass cuts
  kfparticle->setMotherPT(0.);
  kfparticle->setMinimumMass(0.300);  // K_S0 PDG mass: 497.6 MeV
  kfparticle->setMaximumMass(0.700);
  kfparticle->setMaximumMotherVertexVolume(0.1);

  kfparticle->setOutputName(output_reco_file);

  se->registerSubsystem(kfparticle);

  // --- Run ---
  se->skip(nSkip);
  se->run(nEvents);
  se->End();
  se->PrintTimer();

  // Move output out of inReconstruction/ when done
  std::ifstream file(output_reco_file);
  if (file.good())
  {
    std::string moveOutput = "mv " + output_reco_file + " " + output_dir + reco_name + "/";
    system(moveOutput.c_str());
  }

  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}
