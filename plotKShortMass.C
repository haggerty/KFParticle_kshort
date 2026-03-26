void plotKShortMass(const std::string &infile = "root/KShort_run3pp_merged.root")
{
  TFile *f = TFile::Open(infile.c_str(), "READ");
  if (!f || f->IsZombie())
  {
    std::cerr << "Could not open " << infile << std::endl;
    return;
  }

  TTree *tree = (TTree *) f->Get("DecayTree");
  if (!tree)
  {
    std::cerr << "DecayTree not found in " << infile << std::endl;
    return;
  }

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

  std::string pdfname = infile.substr(0, infile.rfind('.')) + "_mass.pdf";
  c->SaveAs(pdfname.c_str());
  std::cout << "Entries: " << tree->GetEntries() << std::endl;
}
