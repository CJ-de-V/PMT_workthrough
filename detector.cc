#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory * ROhist)
{
	G4Track *track = aStep->GetTrack();

	track->SetTrackStatus(fStopAndKill);/*as soon as the photon enters zie photon it DIES, track not propogated any further,
	if omitted they propgate through zie detector and we pick up a z component different from the start of our detector, that
	is to say we read its value inside the detector which is nonsensical*/
	G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
	G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
	/*information about the photon when it enters/leaves the detector, for charged particles this is harder since
	for every step there is a pre and post point? P.7 of series*/

	G4ThreeVector posPhoton = preStepPoint->GetPosition();

	//G4cout << "Photon position: " << posPhoton << G4endl;
	//actual positions of the photons.... but we don't have that irl we have only the number of the detector

	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();

	G4int copyNo = touchable->GetCopyNumber();
	//G4cout <<"Copy number: " << copyNo << G4endl;
	//ok cool but we uuh want the position of the detector :)

	G4VPhysicalVolume *physVol = touchable->GetVolume();
	G4ThreeVector posDetector = physVol->GetTranslation();
	
	#ifndef G4MULTITHREADED
		G4cout << "Detector position: " <<posDetector<<G4endl;
	#endif
	//now we have the position of our detector

	G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

	G4AnalysisManager *man = G4AnalysisManager::Instance();
	man->FillNtupleIColumn(0,evt);
	man->FillNtupleDColumn(1,posDetector[0]);
	man->FillNtupleDColumn(2,posDetector[1]);
	man->FillNtupleDColumn(3,posDetector[2]);
	man->AddNtupleRow(0);


return true;
}
