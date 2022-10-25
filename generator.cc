#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1); //only 1 primary vertex per event one particle
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle("chargedgeantino");
    /*formerly proton/mu-, geantino is a generic placeholder since macros set the particle anyway,
       as it turns out it gets weird if you combine moving and non moving applications, so we opt to set
       it here instead of the macro*/
    //additionally swapped to chargedgeantino for protons from atmosphere

    G4ThreeVector pos(0., 0., -1000. * m);
    G4ThreeVector mom(0., 0., 1.);

    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(mom);
    fParticleGun->SetParticleMomentum(1000 * GeV);
    fParticleGun->SetParticleDefinition(particle);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
    G4ParticleDefinition *particle = fParticleGun->GetParticleDefinition();
    if (particle == G4Geantino::Geantino())
    {
        G4int Z = 27;
        G4int A = 60;

        G4double charge = 0. * eplus;
        G4double energy = 0. * keV;

        G4ParticleDefinition *ion = G4IonTable::GetIonTable()->GetIon(Z, A, energy);
        fParticleGun->SetParticleDefinition(ion);
        fParticleGun->SetParticleCharge(charge);
    } //commented out for G4.11 macro compatibility

    //randomize energy
    G4double randenergy = G4UniformRand() * TeV + 500 * GeV;
    G4String ptype;
    G4double randX = (0.5 - G4UniformRand()) * 2000 * m;
    G4ThreeVector pos(randX, 0., -1000. * m);
    //charge ratio
    if (G4UniformRand() < 1.3 / 2.3)
    {
        fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("mu+"));
        ptype = "mu+";
    }
    else
    {
        fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("mu-"));
        ptype = "mu-";
    }
    fParticleGun->SetParticleEnergy(randenergy);
    fParticleGun->SetParticlePosition(pos);

    G4cout << "gnerated a " << ptype << " with energy " << randenergy / GeV << "[GeV]" << " at X = "
           << randX / m << "[m]" << G4endl;

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
