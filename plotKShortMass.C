void plotKShortMass(const std::string &infile = "root/outputKFParticle_KShort_run3pp_*.root",
                    const std::string &tag    = "KShort_run3pp_10k")
{
  TChain *tree = new TChain("DecayTree");
  int nadded = tree->Add(infile.c_str());
  if (nadded == 0)
  {
    std::cerr << "No files matched: " << infile << std::endl;
    return;
  }
  std::cout << "Chained " << nadded << " files, " << tree->GetEntries() << " entries" << std::endl;

  const int    nbins    = 80;
  const double mlo      = 0.300, mhi = 0.700;
  const double fit_lo   = 0.42,  fit_hi = 0.58;
  const double pdg_mass = 0.4976;
  const double bin_width = (mhi - mlo) / nbins;  // 5 MeV

  TH1F *h = new TH1F("h_kshort_mass",
                     Form(";m(#pi^{+}#pi^{-}) [GeV/c^{2}];Candidates / %.0f MeV",
                          bin_width * 1000.),
                     nbins, mlo, mhi);
  h->SetLineColor(kBlue + 1);
  h->SetLineWidth(2);
  h->Sumw2();
  tree->Draw("K_S0_mass>>h_kshort_mass", "", "goff");

  // -------------------------------------------------------------------------
  // Fit: Gaussian signal + 2nd-order polynomial background
  //   f(x) = A * Gaus(x; mu, sigma) + p0 + p1*(x-0.5) + p2*(x-0.5)^2
  // Centering polynomial at 0.5 GeV reduces parameter correlations.
  // -------------------------------------------------------------------------
  TF1 *ftot = new TF1("ftot",
    "[0]*TMath::Gaus(x,[1],[2],true) + [3] + [4]*(x-0.5) + [5]*(x-0.5)*(x-0.5)",
    fit_lo, fit_hi);

  ftot->SetParNames("SigAmp", "Mean", "Sigma", "p0", "p1", "p2");
  // SigAmp: yield*bin_width (norm Gaus integrates to 1).
  // Background ~1e6 counts/bin at 0.5 GeV, rising left-to-right => p1 > 0.
  ftot->SetParameter(0, 1e4);
  ftot->SetParameter(1, pdg_mass);
  ftot->SetParameter(2, 0.006);
  ftot->SetParameter(3, 1e6);
  ftot->SetParameter(4, 1e6);
  ftot->SetParameter(5, -5e5);
  ftot->SetParLimits(0, 0, 1e8);              // signal must be positive
  ftot->SetParLimits(1, 0.480, 0.515);        // mean constrained near PDG
  ftot->SetParLimits(2, 0.002, 0.015);        // sigma in [2, 15] MeV

  h->Fit(ftot, "R0Q");  // chi-squared fit, range, no draw yet

  // Background component only
  TF1 *fbkg = new TF1("fbkg",
    "[0] + [1]*(x-0.5) + [2]*(x-0.5)*(x-0.5)",
    fit_lo, fit_hi);
  fbkg->SetParameter(0, ftot->GetParameter(3));
  fbkg->SetParameter(1, ftot->GetParameter(4));
  fbkg->SetParameter(2, ftot->GetParameter(5));
  fbkg->SetLineColor(kRed);
  fbkg->SetLineStyle(2);
  fbkg->SetLineWidth(2);

  ftot->SetLineColor(kRed);
  ftot->SetLineWidth(2);

  // Signal yield = integral of Gaussian component (bins in fit range)
  double sigma    = ftot->GetParameter(2);
  double sigamp   = ftot->GetParameter(0);
  double sigamp_e = ftot->GetParError(0);
  // Gaussian integral: A/sqrt(2pi)/sigma * integral = A (normalized form)
  // bin yield = A * bin_width / sqrt(2pi) / sigma ... but TMath::Gaus(norm=true)
  // integrates to 1 over full range, so yield = A / bin_width * bin_width ...
  // With norm=true: Gaus(x,mu,sigma,true) = (1/sqrt(2pi)/sigma)*exp(...)
  // => integral over all x = 1, so signal counts = SigAmp / bin_width
  double yield   = sigamp / bin_width;
  double yield_e = sigamp_e / bin_width;

  double mean    = ftot->GetParameter(1);
  double mean_e  = ftot->GetParError(1);
  double sigma_e = ftot->GetParError(2);
  double chi2ndf = ftot->GetChisquare() / ftot->GetNDF();

  // -------------------------------------------------------------------------
  // Canvas 1: data + fit
  // -------------------------------------------------------------------------
  TCanvas *c1 = new TCanvas("c1_kshort_fit", "K_{S}^{0} Mass Fit", 800, 600);
  c1->SetLeftMargin(0.12);

  h->Draw("E");
  ftot->Draw("same");
  fbkg->Draw("same");

  TLine *pdg = new TLine(pdg_mass, gPad->GetUymin(), pdg_mass, gPad->GetUymax());
  pdg->SetLineColor(kGreen + 2);
  pdg->SetLineStyle(3);
  pdg->SetLineWidth(2);
  pdg->Draw();

  TLegend *leg1 = new TLegend(0.55, 0.65, 0.88, 0.88);
  leg1->SetBorderSize(0);
  leg1->AddEntry(h,    "Data",                      "lpe");
  leg1->AddEntry(ftot, "Gauss + poly bkg",          "l");
  leg1->AddEntry(fbkg, "Poly background",           "l");
  leg1->AddEntry(pdg,  Form("PDG  %.1f MeV", pdg_mass * 1000.), "l");
  leg1->Draw();

  // Stat box
  TPaveText *pt = new TPaveText(0.13, 0.60, 0.50, 0.88, "NDC");
  pt->SetFillStyle(0);
  pt->SetBorderSize(0);
  pt->SetTextAlign(12);
  pt->AddText(Form("#mu = %.2f #pm %.2f MeV", mean * 1000., mean_e * 1000.));
  pt->AddText(Form("#sigma = %.2f #pm %.2f MeV", sigma * 1000., sigma_e * 1000.));
  pt->AddText(Form("Yield = %.0f #pm %.0f", yield, yield_e));
  pt->AddText(Form("#chi^{2}/ndf = %.1f", chi2ndf));
  pt->Draw();

  c1->SaveAs((tag + "_fit.pdf").c_str());

  // -------------------------------------------------------------------------
  // Canvas 2: background-subtracted
  // -------------------------------------------------------------------------
  TH1F *hsub = (TH1F *) h->Clone("hsub");
  hsub->SetTitle(Form(";m(#pi^{+}#pi^{-}) [GeV/c^{2}];Candidates / %.0f MeV (bkg subtracted)",
                      bin_width * 1000.));

  for (int i = 1; i <= hsub->GetNbinsX(); i++)
  {
    double x    = hsub->GetBinCenter(i);
    double bkg  = fbkg->Eval(x);
    double val  = hsub->GetBinContent(i);
    double err  = hsub->GetBinError(i);
    if (x >= fit_lo && x <= fit_hi)
    {
      hsub->SetBinContent(i, val - bkg);
      hsub->SetBinError(i, err);
    }
    else
    {
      hsub->SetBinContent(i, 0);
      hsub->SetBinError(i, 0);
    }
  }

  TF1 *fsig = new TF1("fsig",
    "[0]*TMath::Gaus(x,[1],[2],true)",
    fit_lo, fit_hi);
  fsig->SetParameter(0, sigamp);
  fsig->SetParameter(1, mean);
  fsig->SetParameter(2, sigma);
  fsig->SetLineColor(kRed);
  fsig->SetLineWidth(2);

  TCanvas *c2 = new TCanvas("c2_kshort_sub", "K_{S}^{0} Background Subtracted", 800, 600);
  c2->SetLeftMargin(0.12);

  hsub->SetLineColor(kBlue + 1);
  hsub->SetLineWidth(2);
  hsub->Draw("E");
  fsig->Draw("same");

  TLine *zero = new TLine(fit_lo, 0, fit_hi, 0);
  zero->SetLineColor(kGray + 1);
  zero->SetLineStyle(2);
  zero->Draw();

  TLine *pdg2 = new TLine(pdg_mass, hsub->GetMinimum(), pdg_mass, hsub->GetMaximum());
  pdg2->SetLineColor(kGreen + 2);
  pdg2->SetLineStyle(3);
  pdg2->SetLineWidth(2);
  pdg2->Draw();

  TLegend *leg2 = new TLegend(0.55, 0.70, 0.88, 0.88);
  leg2->SetBorderSize(0);
  leg2->AddEntry(hsub, "Data #minus background", "lpe");
  leg2->AddEntry(fsig, "Gaussian fit",           "l");
  leg2->AddEntry(pdg2, Form("PDG  %.1f MeV", pdg_mass * 1000.), "l");
  leg2->Draw();

  c2->SaveAs((tag + "_subtracted.pdf").c_str());

  std::cout << "Mean   = " << mean  * 1000. << " +/- " << mean_e  * 1000. << " MeV" << std::endl;
  std::cout << "Sigma  = " << sigma * 1000. << " +/- " << sigma_e * 1000. << " MeV" << std::endl;
  std::cout << "Yield  = " << yield << " +/- " << yield_e << std::endl;
  std::cout << "chi2/ndf = " << chi2ndf << std::endl;
}
