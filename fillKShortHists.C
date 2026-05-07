// Fill K_S0 mass histogram from a TChain of KFParticle output trees and save
// to a small root file for fast downstream plotting.
//
// Usage (run on SDCC where files are local):
//   root -b -q 'fillKShortHists.C("root/outputKFParticle_KShort_run3pp_*.root","KShort_hists.root")'

void fillKShortHists(const std::string &infile  = "root/outputKFParticle_KShort_run3pp_*.root",
                     const std::string &outfile = "root/KShort_hists.root")
{
  TChain *tree = new TChain("DecayTree");
  int nadded = tree->Add(infile.c_str());
  if (nadded == 0)
  {
    std::cerr << "No files matched: " << infile << std::endl;
    return;
  }
  std::cout << "Chained " << nadded << " files, " << tree->GetEntries() << " entries" << std::endl;

  const int    nbins = 80;
  const double mlo   = 0.300, mhi = 0.700;

  TH1F *h = new TH1F("h_kshort_mass",
                     Form(";m(#pi^{+}#pi^{-}) [GeV/c^{2}];Candidates / %.0f MeV",
                          (mhi - mlo) / nbins * 1000.),
                     nbins, mlo, mhi);
  h->Sumw2();
  tree->Draw("K_S0_mass>>h_kshort_mass", "", "goff");

  TFile *fout = TFile::Open(outfile.c_str(), "RECREATE");
  h->Write();
  fout->Close();

  std::cout << "Histogram saved to " << outfile << std::endl;
}
