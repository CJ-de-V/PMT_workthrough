#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4Tubs.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Trd.hh"

#include "detector.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
MyDetectorConstruction();
~MyDetectorConstruction();

G4LogicalVolume *GetScoringVolume() const
{
    return fScoringVolume;
}

virtual G4VPhysicalVolume *Construct();

private:
virtual void ConstructSDandField();

G4int nCols, nRows;
G4double xWorld = 1 * m, yWorld = 1 * m, zWorld = 1 * m;
G4bool isCherenkov, isScintillator, isTOF, isAtmosphere, isMountain, isIronSlab;

G4Box *solidWorld, *solidRadiator, *solidDetector, *solidScintillator, *solidAtmosphere, *solidMBase, *solidIronSlab;
G4Trd *solidMCap;
G4LogicalVolume *logicWorld, *logicRadiator, *logicDetector, *fScoringVolume, *logicScintillator, *logicAtmosphere[10], *logicMBase, *logicMCap, *logicIronSlab;
G4VPhysicalVolume *physWorld, *physRadiator, *physDetector, *physScintillator, *physAtmosphere[10], *physMBase, *physMCap, *physIronSlab;
G4OpticalSurface *mirrorSurface;

G4GenericMessenger *fMessenger;

G4Material *SiO2, *H2O, *Aerogel, *worldMat, *NaI, *Air[10], *Granite, *Sandstone, *Fe;
G4Element *C, *Na, *I, *O, *N;

void DefineMaterials();
void DefineWorldVolume();
void ConstructCherenkov();
void ConstructScintillator();
void ConstructTOF();
void ConstructAtmosphere();
void ConstructMountain();
void ConstructIronSlab();
};

#endif
