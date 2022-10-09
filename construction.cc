#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
    fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");
    fMessenger->DeclareProperty("nCols", nCols, "Number of columns");
    fMessenger->DeclareProperty("nRows", nCols, "Number of Rows");
    fMessenger->DeclareProperty("cherenkov", isCherenkov, "Toggle Cherenkov Setup");
    fMessenger->DeclareProperty("scintillator", isScintillator, "Toggle Scintillator Setup");
    fMessenger->DeclareProperty("TOF", isTOF, "Toggle time of flight setup");
    fMessenger->DeclareProperty("atmosphere", isAtmosphere, "Toggle atmosphere setup");
    nCols = 10;
    nRows = 10;

    DefineMaterials();

    xWorld = 20. * m;
    yWorld = 20. * m;
    zWorld = 20. * m;

    isCherenkov = false;
    isScintillator = false;
    isTOF = false;
    isAtmosphere = true;
}

MyDetectorConstruction::~MyDetectorConstruction()
{
}

void MyDetectorConstruction::DefineMaterials()
{
    G4NistManager *nist = G4NistManager::Instance();

    G4double energy[2] = { 1.239841939 * eV / 0.9, 1.239841939 * eV / 0.2 };//energies of blue and violet light


    //base material creation
    SiO2 = new G4Material("SiO2", 2.201 * g / cm3, 2);
    SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
    SiO2->AddElement(nist->FindOrBuildElement("O"), 2);

    H2O = new G4Material("H2O", 1.000 * g / cm3, 2);
    H2O->AddElement(nist->FindOrBuildElement("H"), 2);
    H2O->AddElement(nist->FindOrBuildElement("O"), 1);

    C = nist->FindOrBuildElement("C");

    NaI = new G4Material("NaI", 3.67 * g / cm3, 2);
    NaI->AddElement(nist->FindOrBuildElement("Na"), 1);
    NaI->AddElement(nist->FindOrBuildElement("I"), 1);
    G4MaterialPropertiesTable *mptNaI = new G4MaterialPropertiesTable();
    G4double rindexNaI[2] = { 1.78, 1.78 };                         //refractive index for NaI
    mptNaI->AddProperty("RINDEX", energy, rindexNaI, 2);
    G4double fraction[2] = { 1.0, 1.0 };//for now we assume all the wavelengths have the same intensity
    mptNaI->AddProperty("SCINTILLATIONCOMPONENT1", energy, fraction, 2);
    mptNaI->AddConstProperty("SCINTILLATIONYIELD", 38. / keV);
    mptNaI->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250 * ns);
    mptNaI->AddConstProperty("RESOLUTIONSCALE", 1.0);
    mptNaI->AddConstProperty("SCINTILLATIONYIELD1", 1.);//all these values should be looked up for proper examples
    NaI->SetMaterialPropertiesTable(mptNaI);

    mirrorSurface = new G4OpticalSurface("mirrorSurface");
    mirrorSurface->SetType(dielectric_metal);
    mirrorSurface->SetFinish(ground);
    mirrorSurface->SetModel(unified);
    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();
    G4double reflectivity[2] = { 1., 1. };//all photons are reflected, trapped in mirror coating
    mptMirror->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
    mirrorSurface->SetMaterialPropertiesTable(mptMirror);

    Aerogel = new G4Material("Aerogel", 0.200 * g / cm3, 3);
    Aerogel->AddMaterial(SiO2, 62.5 * perCent);
    Aerogel->AddMaterial(H2O, 37.4 * perCent);
    Aerogel->AddElement(C, 0.1 * perCent);
    G4double rindexAerogel[2] = { 1.1, 1.1 };                         //refractive index for aerogel
    G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
    mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
    Aerogel->SetMaterialPropertiesTable(mptAerogel);

    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4double rindexWorld[2] = { 1.0, 1.0 };                         //refractive index for air
    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
    worldMat->SetMaterialPropertiesTable(mptWorld);

    //atmosphere setup
    G4double density0 = 1.29 * kg / m3;
    G4double aN = 14.01 * g / mole;
    G4double aO = 16 * g / mole;

    N = new G4Element("Nitrogen", "N", 7, aN);
    O = new G4Element("Oxygen", "O", 8, aO);

    G4double f = 3;
    G4double R = 8.3144626181532;
    G4double g0 = 9.81;
    G4double kappa = (f + 2) / f;
    G4double T = 293.15;
    G4double M = (0.3 * aO + 0.7 * aN) / 1000;

    for (G4int i = 0; i < 10; i++)
    {
        std::stringstream stri;
        stri << i;
        G4double h = 40e3 / 10. * i;
        G4double density = density0 * pow(1 - (1 - kappa) / kappa * M * g0 * h / (R * T), (1 / (kappa - 1)));
        Air[i] = new G4Material("G4_Air" + stri.str(), density, 2);
        Air[i]->AddElement(N, 70 * perCent);
        Air[i]->AddElement(O, 30 * perCent);
    }
}

void MyDetectorConstruction::ConstructAtmosphere()
{
    solidAtmosphere = new G4Box("solidAtmosphere", xWorld, yWorld, zWorld / 10.);
    for (G4int i = 0; i < 10; i++)
    {
        logicAtmosphere[i] = new G4LogicalVolume(solidAtmosphere, Air[i], "logicAtmosphere");
        physAtmosphere[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, zWorld / 10. * 2 * i - zWorld + zWorld / 10.),
                                              logicAtmosphere[i], "physAtmosphere", logicWorld, false, i, true);
    }
}

void MyDetectorConstruction::ConstructCherenkov()
{
    //cherenkov target setup
    solidRadiator = new G4Box("solidRadiator", 0.4 * m, 0.4 * m, 0.01 * m);
    logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicalRadiator");
    fScoringVolume = logicRadiator;
    physRadiator = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.25 * m), logicRadiator, "physRadiator", logicWorld, false, 0, true);

    //setting up the photosensor sensitive detector
    solidDetector = new G4Box("solidDetector", xWorld / nRows, yWorld / nCols, 0.01 * m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

    for (G4int i = 0; i < nRows; i++)
    {
        for (G4int j = 0; j < nCols; j++)
        {
            physDetector = new G4PVPlacement(0, G4ThreeVector(-0.5 * m + (i + 0.5) * m / nRows, -0.5 * m + (j + 0.5) * m / nCols, 0.49 * m),
                                             logicDetector, "physDetector", logicWorld, false, j + i * nCols, true);
        }
    }
}

void MyDetectorConstruction::ConstructScintillator()
{
    solidScintillator = new G4Box("solidScintillator", 5 * cm, 5 * cm, 6 * cm);
    logicScintillator = new  G4LogicalVolume(solidScintillator, NaI, "logicalScintillator");
    G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicWorld, mirrorSurface);
    //coats world in mirror surface, once a photon enters something not the world surface it will not leave
    solidDetector = new G4Box("solidDetector", 1 * cm, 5 * cm, 6 * cm);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

    fScoringVolume = logicScintillator;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            G4Rotate3D rotZ(j * 22.5 * deg, G4ThreeVector(0, 0, 1));

            G4Translate3D transXScint(G4ThreeVector(5. / tan(22.5 / 2 * deg) * cm + 5 * cm, 0 * cm, -40 * cm + i * 15 * cm));
            G4Transform3D transformScint = (rotZ) * (transXScint); //basically stacking rotation&translation into one linear transformation
            physScintillator = new G4PVPlacement(transformScint, logicScintillator, "physScintillator", logicWorld, false, 0, true);

            G4Translate3D transXDet(G4ThreeVector(5. / tan(22.5 / 2 * deg) * cm + 6. * cm + 5. * cm, 0 * cm, -40 * cm + i * 15 * cm));
            G4Transform3D transformDet = (rotZ) * (transXDet); //basically stacking rotation&translation into one linear transformation
            physDetector = new G4PVPlacement(transformDet, logicDetector, "physDetector", logicWorld, false, 0, true);
        }
    }
}

void MyDetectorConstruction::ConstructTOF()
{
    solidDetector = new G4Box("solidDetector", 1 * m, 1 * m, 0.1 * m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
    physDetector = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, -4 * m), logicDetector, "physDetector", logicWorld, false, 0, true);
    physDetector = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m,  3 * m), logicDetector, "physDetector", logicWorld, false, 1, true);
    /*"allocated on heap, overriding them doesn't delete them, well no but it deletes our reference to it :)", the 1 is for disinguishing*/
}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

    if (isCherenkov)
    {
        ConstructCherenkov();
    }
    if (isScintillator)
    {
        ConstructScintillator();
    }
    if (isTOF)
    {
        ConstructTOF();
    }
    if (isAtmosphere)
    {
        ConstructAtmosphere();
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    /*if (logicDetector != NULL)
       {
        //if the detector has been defined in any setup we wish to have a sensitivedetector
        logicDetector->SetSensitiveDetector(sensDet);
       } commented out for the atmospheric case, causes some issues*/
}
