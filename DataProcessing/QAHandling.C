void QAHandling() {
    TCanvas *se = new TCanvas;
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
   TH1F *myHist = new TH1F("Data", "1 TeV muons on 3m of Fe", 1000, 0, 1000);
 
   // Open the file containing the tree.
   TFile *myFile = TFile::Open("QA100k.root");
 
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
  double mean = myHist->GetMean();
   
      cout << " FWHM OF " <<fwhm <<" and median of "<< x<< "with a scrimptiuous mean of"<<mean<<std::endl;
      
   myHist->Scale(1./myHist->GetEntries());
   	 myHist->GetXaxis()->SetTitle("Final momentum p [GeV/c]");
	myHist->GetYaxis()->SetTitle("dN/dp [1/(GeV/c)]");
   	 //this isn't working out, add entries manually
   	 
  /* 	     // Retrieve the stat box
    TPaveStats *ps = (TPaveStats*)myHist->GetPrimitive("stats");
    ps->SetName("mystats");
    TList *listOfLines = ps->GetListOfLines();
 
    // Remove the RMS line
    listOfLines->Remove(ps->GetLineWith("Mean"));
 
    // Add a new line in the stat box.
    // Note that "=" is a control character
    TLatex *myt = new TLatex(0,0,"Test = 10");
    myt ->SetTextFont(42);
    myt ->SetTextSize(0.04);
    myt ->SetTextColor(kRed);
    listOfLines->Add(myt);*/
   	 
   	 
	myHist->GetXaxis()->SetRangeUser(950,1000); // only draw axis range from 950 to 1000
	//myHist->GetXaxis()->SetRange(); //fixes the data tab
	
		myHist->SetStats(0);
   myHist->Draw("HIST");
	
	
	
	//meandrawing
	int bin = myHist->FindBin(mean);
	double y0 = myHist->GetBinContent(bin);
	
	TArrow *arr = new TArrow(964, 0.05, mean, y0);
	arr->SetLineWidth(2);
	arr->SetArrowSize(0.02);
	arr->Draw();
	
	TLatex *t = new TLatex(964, 0.05, "Mean of 977 GeV/c");
	t->Draw();
	
	//MedianDrawing
		int binmedi = myHist->FindBin(x);
	double y1 = myHist->GetBinContent(binmedi);
	
	TArrow *arr1 = new TArrow(985, 0.32, x, y1);
	arr1->SetLineWidth(3);
	arr1->SetArrowSize(0.02);
	arr1->Draw();
	
	TLatex *t1 = new TLatex(966, 0.32, "Median of 989 GeV/c");
	t1->Draw();
	
	
	TLatex *t2 = new TLatex(980, 0.2, "FWHM ~ 1 GeV/c");
	t2->Draw();
	
	
	
	
	


}
