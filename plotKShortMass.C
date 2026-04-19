void plotKShortMass(const std::string &infile = "root/outputKFParticle_KShort_run3pp_*.root")
{
  TChain *tree = new TChain("DecayTree");
  int nadded = tree->Add(infile.c_str());
  if (nadded == 0)
  {
    std::cerr << "No files matched: " << infile << std::endl;
    return;
  }
  std::cout << "Chained " << nadded << " files, " << tree->GetEntries() << " entries" << std::endl;

  TCanvas *c = new TCanvas("c_kshort_mass", "K_{S}^{0} Mass", 800, 600);
  c->SetLeftMargin(0.12);

  TH1F *h = new TH1F("h_kshort_mass", ";m(#pi^{+}#pi^{-}) [GeV/c^{2}];Candidates / 5 MeV", 80, 0.300, 0.700);
  h->SetLineColor(kBlue + 1);
  h->SetLineWidth(2);

  tree->Draw("K_S0_mass>>h_kshort_mass", "", "E");
  c->Update();

  // Mark the PDG K_S0 mass (497.6 MeV) — span the full visible y range
  TLine *pdg = new TLine(0.4976, gPad->GetUymin(), 0.4976, gPad->GetUymax());
  pdg->SetLineColor(kRed);
  pdg->SetLineStyle(2);
  pdg->SetLineWidth(2);
  pdg->Draw();

  TLegend *leg = new TLegend(0.65, 0.75, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->AddEntry(h, "Data", "l");
  leg->AddEntry(pdg, "PDG K_{S}^{0}", "l");
  leg->Draw();

  c->SaveAs("KShort_run3pp_mass.pdf");
  std::cout << "Entries: " << tree->GetEntries() << std::endl;
}
