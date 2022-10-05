//hadd testcombine.root output0_t{0..7}.root
//just flat this one in not the root terminal combines them
//still need to look into combining as part of the code as that behaves strangely-bloated sizes
//able to read them in, but unsure of plotting tree entries yet, should be doable
//in terminal appears as
//Hits->Draw("fX:fY","","colz")
void myroothelper(){
	TFile *_file0 = TFile::Open("testcombine.root");
	 (TTree*)_file0->Get("Hits")->Draw("fX:fY","","colz");
}

