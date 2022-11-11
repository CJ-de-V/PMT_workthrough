#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void MainProcess() {
/*
    TChain ch ("MuonsEnergy");
    int max n=0;
    ch.Add
*/
    
    //ch.Draw("MuonsMomentum");*/
    

    //sniff out the many files
     std::string ext(".root");
     std::string path("/home/cornelius/Documents/mygeant4projects/physicsmatterstut/DataProcessing/MainData/");
    int numFiles = 0; //we can pin this down
    for (auto &p : fs::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ext)
         numFiles++;
    }

   //Open for reading using a TChain
   TChain *chain = new TChain("MuonsEnergy");
   for (int fileNum=0;fileNum < numFiles;fileNum++) {
   //needs a const string, easier to just do it in this braindead way
      chain->AddFile(Form("/home/cornelius/Documents/mygeant4projects/physicsmatterstut/DataProcessing/MainData/output%d.root", fileNum));
   }
   //chain->Draw("MuonsMomentum");
   //chain->Draw("MuonsPrimEnergy:MuonsPosition","","lego");
   //use lego instead of colz for a delectable 3d plot
   
   TH2F *someTH2=new TH2F("someTH2", "Muons counts as a function of position and primary energy", 15, -725 ,25, 19, 75, 1025);
   
   	someTH2->GetXaxis()->SetTitle("Position");
	someTH2->GetYaxis()->SetTitle("Primary energy");
   
   	TCanvas *c1 = new TCanvas();
	someTH2->SetStats(0); //do not want the statistics box 
	c1->SetTickx();
	c1->SetTicky();
	c1->SetGridx();
	c1->SetGridy();
   
   //so apparently the >> is some kind of "piping" thing by the looks of it? look into multiple canvasses
    gStyle->SetPalette(1);
    chain->Draw("MuonsPrimEnergy : MuonsPosition >> someTH2", "", "colzTEXT");
    
    TH2F *groundTH2=new TH2F("groundTH2", "Groundlevelenergies vs position", 15, -725 ,25, 19, 75, 1025);
    
    groundTH2->SetStats(0);
    //chain->Draw("GroundLevelEnergy : MuonsPosition >> groundTH2", "", "colzTEXT");
    
       TH2F *engcompTH2=new TH2F("engcompTH2", "Primary energy vs groundlevel energy", 20, 0 ,1000, 20, 0, 1000);
    groundTH2->SetStats(0);
    //chain->Draw("MuonsPrimEnergy:GroundLevelEnergy : MuonsPosition", "", "TEXT");
    
    
}
