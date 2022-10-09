#include "run.hh"

MyRunAction::MyRunAction()
{
//G4AnalysisManager::Instance()->SetVerboseLevel(1);
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->CreateNtuple("Photons", "Photons");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fX");
    man->CreateNtupleDColumn("fY");
    man->CreateNtupleDColumn("fZ");
    man->CreateNtupleDColumn("fWlen");
    man->CreateNtupleDColumn("fT");    
    man->FinishNtuple(0);

    man->CreateNtuple("Hits", "Hits");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fX");
    man->CreateNtupleDColumn("fY");
    man->CreateNtupleDColumn("fZ");
    man->FinishNtuple(1);

		man->CreateNtuple("Scoring","Scoring");
		man->CreateNtupleDColumn("fEdep");
		man->FinishNtuple(2);

}//action here for start of entire thing, spans multiple runs

MyRunAction::~MyRunAction()
{
}

void MyRunAction::BeginOfRunAction(const G4Run *run)
{
    //action here for start of each run, every particle generated

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();
    std::stringstream strRunID;
    strRunID << runID;

    G4String filename = "output" + strRunID.str() + ".root";
    man->OpenFile(filename);

    //G4cout << "Using " << man->GetType() << G4endl;
}

void MyRunAction::EndOfRunAction(const G4Run *)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->Write();
    man->CloseFile();
}
