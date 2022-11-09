void QAHandling() {
  /* //open datafile
   std::unique_ptr<TFile> QARfile(TFile::Open("QA10k.root") );
   //list TTrees in the chain
   QARfile->ls();
   
   //attempt to access the muonenergy TTree from QARfile
   std::unique_ptr<TTree> MomDist(QARfile->Get<TTree>("MuonsEnergy"));
   
   //MomDist->Draw("GroundLevelEnergy:MuonsPosition","","colz");
   //this is an attempt to draw
   //MomDist->ls();
   //THIS WORKS WAHOO, now we just need to, uuuh convert to a histogram? Do we really? probably not, do still want FWHM though
   MomDist->Draw("MuonsMomentum");*/


    //below adapted from https://root.cern/doc/master/classTTreeReader.html and several other bits and pieces from the forums
    //
   // Create a histogram for the values we read.
   TH1F *myHist = new TH1F("h1", "MomentumHistogramIthink", 1000, 0, 1000);
 
   // Open the file containing the tree.
   TFile *myFile = TFile::Open("QA5k.root");
 
   // Create a TTreeReader for the tree, for instance by passing the
   // TTree's name and the TDirectory / TFile it is in.
   TTreeReader myReader("MuonsEnergy", myFile);
 
   // The branch "px" contains floats; access them as myPx.
   TTreeReaderValue<Double_t> myPx(myReader, "MuonsMomentum");
   // The branch "py" contains floats, too; access those as myPy.
   //TTreeReaderValue<Float_t> myPy(myReader, "py");
 
   // Loop over all entries of the TTree or TChain.
   while (myReader.Next()) {
      // Just access the data as if myPx and myPy were iterators (note the '*'
      // in front of them):
      myHist->Fill(*myPx);
   }
   
   int bin1 = myHist->FindFirstBinAbove(myHist->GetMaximum()/2);
   int bin2 = myHist->FindLastBinAbove(myHist->GetMaximum()/2);
   double fwhm = myHist->GetBinCenter(bin2) - myHist->GetBinCenter(bin1);
   
  Double_t x, q;
  q = 0.5; // 0.5 for "median"
  myHist->ComputeIntegral(); // just a precaution
  myHist->GetQuantiles(1, &x, &q);
   
      cout << " FWHM OF " <<fwhm <<" and median of "<< x<<std::endl;
      
   myHist->Scale(1./myHist->GetEntries());
   	 
   	 //this isn't working out, add entries manually
	//myHist->GetXaxis()->SetRangeUser(950,1000); // only draw axis range from 950 to 1000
	//myHist->GetXaxis()->SetRange(); //fixes the data tab
	//myHist->ResetStats();
   myHist->Draw("HIST");
}
