// Plot K_S0 mass from a pre-filled histogram file produced by fillKShortHists.C.
// Fast: no TChain — just fit and draw.
//
// Usage:
//   root -b -q 'plotKShortMass.C("KShort_hists.root","KShort_run3pp_10k")'

void plotKShortMass(const std::string &histfile = "KShort_hists.root",
                    const std::string &tag      = "KShort_run3pp_10k")
{
  TFile *fin = TFile::Open(histfile.c_str(), "READ");
  if (!fin || fin->IsZombie())
  {
    std::cerr << "Cannot open " << histfile << std::endl;
    return;
  }
  TH1F *h = (TH1F *) fin->Get("h_kshort_mass");
  if (!h)
  {
    std::cerr << "h_kshort_mass not found in " << histfile << std::endl;
    return;
  }
  h->SetDirectory(nullptr);
  fin->Close();

  const double fit_lo   = 0.42,  fit_hi = 0.58;
  const double pdg_mass = 0.4976;
  const double bin_width = h->GetBinWidth(1);

  gStyle->SetOptStat(0);
  gStyle->SetTextFont(42);

  // -------------------------------------------------------------------------
  // Fit: Gaussian signal + 2nd-order polynomial background
  // -------------------------------------------------------------------------
  TF1 *ftot = new TF1("ftot",
    "[0]*TMath::Gaus(x,[1],[2],true) + [3] + [4]*(x-0.5) + [5]*(x-0.5)*(x-0.5)",
    fit_lo, fit_hi);

  ftot->SetParNames("SigAmp", "Mean", "Sigma", "p0", "p1", "p2");
  ftot->SetParameter(0, 1e4);
  ftot->SetParameter(1, pdg_mass);
  ftot->SetParameter(2, 0.006);
  ftot->SetParameter(3, 1e6);
  ftot->SetParameter(4, 1e6);
  ftot->SetParameter(5, -5e5);
  ftot->SetParLimits(0, 0, 1e8);
  ftot->SetParLimits(1, 0.480, 0.515);
  ftot->SetParLimits(2, 0.002, 0.015);

  h->Fit(ftot, "R0Q");

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

  double mean    = ftot->GetParameter(1);
  double mean_e  = ftot->GetParError(1);
  double sigma   = ftot->GetParameter(2);
  double sigma_e = ftot->GetParError(2);
  double sigamp  = ftot->GetParameter(0);
  double sigamp_e= ftot->GetParError(0);
  double yield   = sigamp   / bin_width;
  double yield_e = sigamp_e / bin_width;
  double chi2ndf = ftot->GetChisquare() / ftot->GetNDF();

  std::cout << "Mean   = " << mean  * 1000. << " +/- " << mean_e  * 1000. << " MeV" << std::endl;
  std::cout << "Sigma  = " << sigma * 1000. << " +/- " << sigma_e * 1000. << " MeV" << std::endl;
  std::cout << "Yield  = " << yield << " +/- " << yield_e << std::endl;
  std::cout << "chi2/ndf = " << chi2ndf << std::endl;

  // -------------------------------------------------------------------------
  // Canvas 1: data + fit
  // -------------------------------------------------------------------------
  TCanvas *c1 = new TCanvas("c1_kshort_fit", "K_{S}^{0} Mass Fit", 800, 680);

  // Plot pad leaves a caption strip at the bottom
  TPad *pad1 = new TPad("pad1", "", 0.0, 0.10, 1.0, 1.0);
  pad1->SetLeftMargin(0.13);
  pad1->SetRightMargin(0.05);
  pad1->SetTopMargin(0.05);
  pad1->SetBottomMargin(0.13);
  pad1->Draw();
  pad1->cd();

  h->SetLineColor(kBlue + 1);
  h->SetLineWidth(2);
  // Extra headroom at top so the legend doesn't sit on data
  h->SetMaximum(h->GetMaximum() + (h->GetMaximum() - h->GetMinimum()) * 0.45);
  h->Draw("E");
  ftot->Draw("same");
  fbkg->Draw("same");

  TLine *pdg = new TLine(pdg_mass, pad1->GetUymin(), pdg_mass, pad1->GetUymax());
  pdg->SetLineColor(kGreen + 2);
  pdg->SetLineStyle(3);
  pdg->SetLineWidth(2);
  pdg->Draw();

  // Legend in upper-left headroom (data there is ~750k, well below headroom)
  TLegend *leg1 = new TLegend(0.14, 0.76, 0.50, 0.94);
  leg1->SetBorderSize(0);
  leg1->SetTextFont(42);
  leg1->SetTextSize(0.038);
  leg1->AddEntry(h,    "Data",          "lpe");
  leg1->AddEntry(ftot, "Signal + bkg",  "l");
  leg1->AddEntry(fbkg, "Background",    "l");
  leg1->AddEntry(pdg,  Form("PDG %.1f MeV", pdg_mass * 1000.), "l");
  leg1->Draw();

  // Caption strip: two TLatex lines drawn in canvas coordinates
  c1->cd();
  TLatex lat;
  lat.SetNDC();
  lat.SetTextFont(42);
  lat.SetTextSize(0.032);
  lat.SetTextAlign(11);
  lat.DrawLatex(0.13, 0.068,
    Form("#mu = %.2f #pm %.2f MeV      #sigma = %.2f #pm %.2f MeV",
         mean * 1000., mean_e * 1000., sigma * 1000., sigma_e * 1000.));
  lat.DrawLatex(0.13, 0.022,
    Form("Yield = %.0f #pm %.0f      #chi^{2}/ndf = %.1f",
         yield, yield_e, chi2ndf));

  c1->SaveAs((tag + "_fit.pdf").c_str());

  // -------------------------------------------------------------------------
  // Canvas 2: background-subtracted
  // -------------------------------------------------------------------------
  TH1F *hsub = (TH1F *) h->Clone("hsub");
  hsub->SetTitle(Form(";m(#pi^{+}#pi^{-}) [GeV/c^{2}];Candidates / %.0f MeV (bkg sub.)",
                      bin_width * 1000.));
  hsub->SetMaximum(-1111);  // reset inherited display scale before subtraction

  for (int i = 1; i <= hsub->GetNbinsX(); i++)
  {
    double x   = hsub->GetBinCenter(i);
    double val = h->GetBinContent(i);
    double err = h->GetBinError(i);
    if (x >= fit_lo && x <= fit_hi)
    {
      hsub->SetBinContent(i, val - fbkg->Eval(x));
      hsub->SetBinError(i, err);
    }
    else
    {
      hsub->SetBinContent(i, 0);
      hsub->SetBinError(i, 0);
    }
  }

  TF1 *fsig = new TF1("fsig", "[0]*TMath::Gaus(x,[1],[2],true)", fit_lo, fit_hi);
  fsig->SetParameter(0, sigamp);
  fsig->SetParameter(1, mean);
  fsig->SetParameter(2, sigma);
  fsig->SetLineColor(kRed);
  fsig->SetLineWidth(2);

  TCanvas *c2 = new TCanvas("c2_kshort_sub", "K_{S}^{0} Background Subtracted", 800, 680);

  TPad *pad2 = new TPad("pad2", "", 0.0, 0.10, 1.0, 1.0);
  pad2->SetLeftMargin(0.13);
  pad2->SetRightMargin(0.05);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.13);
  pad2->Draw();
  pad2->cd();

  hsub->SetLineColor(kBlue + 1);
  hsub->SetLineWidth(2);
  hsub->SetMaximum(hsub->GetMaximum() * 1.35);
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

  TLegend *leg2 = new TLegend(0.14, 0.79, 0.50, 0.94);
  leg2->SetBorderSize(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.038);
  leg2->AddEntry(hsub, "Data #minus bkg", "lpe");
  leg2->AddEntry(fsig, "Gaussian fit",    "l");
  leg2->AddEntry(pdg2, Form("PDG %.1f MeV", pdg_mass * 1000.), "l");
  leg2->Draw();

  c2->cd();
  TLatex lat2;
  lat2.SetNDC();
  lat2.SetTextFont(42);
  lat2.SetTextSize(0.032);
  lat2.SetTextAlign(11);
  lat2.DrawLatex(0.13, 0.068,
    Form("#mu = %.2f #pm %.2f MeV      #sigma = %.2f #pm %.2f MeV",
         mean * 1000., mean_e * 1000., sigma * 1000., sigma_e * 1000.));
  lat2.DrawLatex(0.13, 0.022,
    Form("Yield = %.0f #pm %.0f      #chi^{2}/ndf = %.1f",
         yield, yield_e, chi2ndf));

  c2->SaveAs((tag + "_subtracted.pdf").c_str());
}
