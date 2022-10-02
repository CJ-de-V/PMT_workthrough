#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator(){
	fParticleGun = new G4ParticleGun(1); //only 1 primary vertex per event one particle
	G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName="proton";	//switcing between muons and protons
	G4ParticleDefinition *particle = particleTable->FindParticle (particleName); 
	
	G4ThreeVector pos(0.,0.,0);
	G4ThreeVector mom(0.,0.,1.); 
	
	fParticleGun->SetParticlePosition(pos);
	fParticleGun->SetParticleMomentumDirection(mom);
	fParticleGun->SetParticleMomentum(100.*GeV);
	fParticleGun->SetParticleDefinition(particle);
	
}

MyPrimaryGenerator::~MyPrimaryGenerator(){
	delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
	fParticleGun->GeneratePrimaryVertex(anEvent);
}
