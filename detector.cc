#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
    //originally supposed to use G4PhysicsOrderedFreeVector but this was obsolted in 4.11
    //likely a good idea to learn where to read what was obsoleted and replaced, might be time to delve into documentation
    quEff = new G4PhysicsFreeVector();
    std::ifstream datafile;
    datafile.open("eff.dat");
    while (1)
    {
        G4double wlen, queff;
        datafile >> wlen >> queff;

        if (datafile.eof())
        {
            break;
        }
        //  G4cout << wlen << " " << queff << G4endl;
        quEff->InsertValues(wlen, queff / 100.);
    }
    datafile.close();
    //obsoleted member
    //quEff->SetSpline(false);
}

MySensitiveDetector::~MySensitiveDetector()
{
}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    G4Track *track = aStep->GetTrack();
    /*************************TOGGLING THIS OFF FOR FUNNY TESTING ********************/
    /*G4cout << "Hi, I'm a: " << track->GetParticleDefinition()->GetParticleName() << " and I have a kinetic energy of "
           << track->GetDynamicParticle()->GetKineticEnergy() / GeV << " [GeV]" << G4endl;*/


  /*  G4cout << "Hi, I'm a: " << track->GetParticleDefinition()->GetParticleName() << " and my primary has an energgy of "
           << track->GetVertexKineticEnergy() / GeV << " [GeV]" << G4endl;*/

    track->SetTrackStatus(fStopAndKill);
    /*>>>>>>>comented out for ONLY the timing test, has to be reenabled<<<<<<<<<<*/

    /*as soon as the photon enters the detector it DIES, track not propogated any further,
       if omitted they propgate through zie detector and we pick up a z component different from the start of our detector, that
       is to say we read its value inside the detector which is nonsensical*/
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
    /*information about the photon when it enters/leaves the detector, for charged particles this is harder since
       for every step there is a pre and post point? P.7 of series*/

    G4ThreeVector posPhoton = preStepPoint->GetPosition();
    G4ThreeVector momPhoton = preStepPoint->GetMomentum();
    //G4cout <<"DETECTOR HIT WITH VELOCITY: " <<preStepPoint->GetBeta()<<G4endl;
    //not really sure how to filter this out to muons only
    //Brehmstrahlung and ionization produces photons too which become problematic as well..


    G4double wlen = (1.239841939 * eV / momPhoton.mag()) * 1E+03;
    G4double time = preStepPoint->GetGlobalTime();
    /*global vs. local time, local time is from birth of particle glob is from start of run*/

    //G4cout << "Photon position: " << posPhoton << G4endl;
    //actual positions of the photons.... but we don't have that irl we have only the number of the detector

    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();

    G4int copyNo = touchable->GetCopyNumber();
    //G4cout <<"Copy number: " << copyNo << G4endl;
    //ok cool but we uuh want the position of the detector :)

    G4VPhysicalVolume *physVol = touchable->GetVolume();
    G4ThreeVector posDetector = physVol->GetTranslation();

    #ifndef G4MULTITHREADED
    G4cout << "Detector position: " << posDetector << G4endl;
    G4cout << "Photon wavelength" << wlen << G4endl;
    #endif
    //now we have the position of our detector

    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->FillNtupleIColumn(0, 0, evt);
    man->FillNtupleDColumn(0, 1, posPhoton[0]);
    man->FillNtupleDColumn(0, 2, posPhoton[1]);
    man->FillNtupleDColumn(0, 3, posPhoton[2]);
    man->FillNtupleDColumn(0, 4, wlen);
    man->FillNtupleDColumn(0, 5, time);
    man->AddNtupleRow(0);



    /*  if (G4UniformRand() < quEff->Value(wlen))
       {*/
    man->FillNtupleIColumn(1, 0, evt);
    man->FillNtupleDColumn(1, 1, posDetector[0]);
    man->FillNtupleDColumn(1, 2, posDetector[1]);
    man->FillNtupleDColumn(1, 3, posDetector[2]);
    man->AddNtupleRow(1);
    /*  } just turned off the efficiency for the time being*/

/*MUON DATA*/
    G4String parnam = track->GetParticleDefinition()->GetParticleName();
    if (parnam == "mu-" || parnam == "mu+")
    {
        man->FillNtupleDColumn(3, 0, track->GetDynamicParticle()->GetKineticEnergy() / GeV);
        man->FillNtupleDColumn(3, 1, posPhoton[0] / m);
        man->FillNtupleDColumn(3, 2, momPhoton.mag() / GeV);
        if (parnam == "mu-")
        {
            man->FillNtupleIColumn(3, 3, -1);
        }
        else
        {
            man->FillNtupleIColumn(3, 3, +1);
        }
        man->FillNtupleDColumn(3, 4, track->GetVertexKineticEnergy() / GeV);
        man->AddNtupleRow(3);
    }
    return true;
}
