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
    fMessenger->DeclareProperty("mountain", isMountain, "Toggle Mountain setup");
    //fMessenger->DeclareProperty("ScaleWorld", boxsize, "Toggle Size of setup");

    nCols = 10;
    nRows = 10;

    DefineMaterials();

    isCherenkov = false;
    isScintillator = false;
    isTOF = false;
    isAtmosphere = false;
    isMountain = true;//mountanous setup
    isIronSlab = false; //QA
}

MyDetectorConstruction::~MyDetectorConstruction()
{
}

void MyDetectorConstruction::DefineMaterials()
{
    G4NistManager *nist = G4NistManager::Instance();

    G4double energy[2] = { 1.239841939 * eV / 0.9, 1.239841939 * eV / 0.2 };//energies of blue and violet light


    //?????????????????????TWODEFINITIONS OF SIO2, HERE AND BOTTOM DELETE ONE
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


    /*Densities from tunnel geology, composition from engineering doc.*/

    /*Granite and setup for main sim.*/
    Granite = new G4Material("Granite", 2648 * kg / m3, 11);
    Granite->AddElement(nist->FindOrBuildElement("O"), 0.48764);
    Granite->AddElement(nist->FindOrBuildElement("Na"), 0.03831);
    Granite->AddElement(nist->FindOrBuildElement("Mg"), 0.000019);
    Granite->AddElement(nist->FindOrBuildElement("Al"), 0.05495);
    Granite->AddElement(nist->FindOrBuildElement("Si"), 0.35835);
    Granite->AddElement(nist->FindOrBuildElement("P"), 0.00025);
    Granite->AddElement(nist->FindOrBuildElement("K"), 0.04102);
    Granite->AddElement(nist->FindOrBuildElement("Ca"), 0.00497);
    Granite->AddElement(nist->FindOrBuildElement("Ti"), 0.00103);
    Granite->AddElement(nist->FindOrBuildElement("Mn"), 0.00024);
    Granite->AddElement(nist->FindOrBuildElement("Fe"), 0.01322);

    /*Sandstone setup for main sim*/
    Sandstone = new G4Material("Sandstone", 2500 * kg / m3, 11);
    Sandstone->AddElement(nist->FindOrBuildElement("O"), 0.52667);
    Sandstone->AddElement(nist->FindOrBuildElement("Na"), 0.00011);
    Sandstone->AddElement(nist->FindOrBuildElement("Mg"), 0.000021);
    Sandstone->AddElement(nist->FindOrBuildElement("Al"), 0.02813);
    Sandstone->AddElement(nist->FindOrBuildElement("Si"), 0.43703);
    Sandstone->AddElement(nist->FindOrBuildElement("P"), 0.00017);
    Sandstone->AddElement(nist->FindOrBuildElement("K"), 0.000043);
    Sandstone->AddElement(nist->FindOrBuildElement("Ca"), 0.00218);
    Sandstone->AddElement(nist->FindOrBuildElement("Ti"), 0.0007);
    Sandstone->AddElement(nist->FindOrBuildElement("Mn"), 0.000064);
    Sandstone->AddElement(nist->FindOrBuildElement("Fe"), 0.00488);

    /*Iron setup for QA*/
    Fe = nist->FindOrBuildMaterial("G4_Fe");/*new G4Material("Fe", 8.874 * g / cm3, 1);
                                               Fe->AddElement(nist->FindOrBuildElement("Fe"), 1);*/
}

void MyDetectorConstruction::DefineWorldVolume()
{
    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
}

void MyDetectorConstruction::ConstructAtmosphere()
{
    solidAtmosphere = new G4Box("solidAtmosphere", xWorld, yWorld, zWorld / 11.);
    for (G4int i = 0; i < 10; i++)
    {
        std::stringstream stri;
        stri << i;
        logicAtmosphere[i] = new G4LogicalVolume(solidAtmosphere, Air[i], "logicAtmosphere" + stri.str());
        physAtmosphere[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, zWorld / 10. * 2 * i - zWorld + zWorld / 10.),
                                              logicAtmosphere[i], "physAtmosphere" + stri.str(), logicWorld, false, i, true);
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

void MyDetectorConstruction::ConstructIronSlab()
{
    xWorld = 1 * m, yWorld = 1 * m, zWorld = 2 * m;
    DefineWorldVolume();

    //Base setup
    solidIronSlab = new G4Box("solidIronSlab", xWorld, yWorld, 1.5 * m);
    logicIronSlab = new G4LogicalVolume(solidIronSlab, Fe, "logicIronSlab");
    physIronSlab = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicIronSlab, "physIronSlab", logicWorld, false, 0, true);

    //detector setup
    solidDetector = new G4Box("solidDetector", xWorld, yWorld, 0.01 * m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
    physDetector = new G4PVPlacement(0, G4ThreeVector(0., 0., 1.51 * m),
                                     logicDetector, "physDetector", logicWorld, 0, true);
}

void :: MyDetectorConstruction::ConstructMountain()
{
    xWorld = 1.5 * km, yWorld = 0.1 * km, zWorld = 0.6 * km;
    DefineWorldVolume();

    //Base setup
    solidMBase = new G4Box("solidMBase", xWorld, yWorld, 150 * m);
    logicMBase = new G4LogicalVolume(solidMBase, Granite, "logicalMBase");
    physMBase = new G4PVPlacement(0, G4ThreeVector(0., 0., (450 - 10.) * m), logicMBase, "physMBase", logicWorld, false, 0, true);

    //TiltedBase setup
    /*  const G4ThreeVector pt[8] = { G4ThreeVector(0, 0, 1), G4ThreeVector(1, 1, 1),//Edge with smaller Y of the base at -z
                                    G4ThreeVector(11, 10, 12), G4ThreeVector(10, 10, 10),//Edge with bigger Y of the base at -z
                                    G4ThreeVector(10, 111, 10), G4ThreeVector(1101, 101, 101), //Edge with smaller Y of the base at +z
                                    G4ThreeVector(1011, 1011, 11011), G4ThreeVector(1011, 101, 11) //Edge with bigger Y of the base at +z
       };
       solidMTB = new G4Trap("solidMTB", pt);
       logicMTB = new G4LogicalVolume(solidMTB, Granite, "logicalMTB");
       physMCap = new G4PVPlacement(0, G4ThreeVector(0., 0., 0. - 0.01 * zWorld), logicMTB, "physMTB", logicWorld, false, 0, true);
       //temporarily scrapped since this is a real piece of work
     */

    //building up side setup
    /*  solidMTB = new G4Trap("solidMTB", 2 * zWorld, 500. * m, 300. * m, 200. * m);
       logicMTB = new G4LogicalVolume(solidMTB, Granite, "logicalMTB");
       G4Transform3D transformDet = (new G4Rotate3D(90 * deg, G4ThreeVector(0, 0, 1))) * (new G4Translate3D(G4ThreeVector(0., 0., 0.)));
       physMCap = new G4PVPlacement(0, G4ThreeVector(0., 0., 0. - 0.01 * zWorld), logicMTB, "physMTB", logicWorld, false, 0, true);
       //more tempscrapped
     */


    //cap setup
    solidMCap = new G4Trd("solidMCap", 50. * m, 600. * m, yWorld, yWorld, 250. * m);
    logicMCap = new G4LogicalVolume(solidMCap, Sandstone, "logicalMCap");
    physMCap = new G4PVPlacement(0, G4ThreeVector(0., 0., 40. * m), logicMCap, "physMCap", logicWorld, false, 0, true);

    //detector setup
    solidDetector = new G4Box("solidDetector", xWorld, yWorld, 0.5 * m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
    physDetector = new G4PVPlacement(0, G4ThreeVector(0., 0., zWorld - 0.5 * m),
                                     logicDetector, "physDetector", logicWorld, 0, true);
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
    /*solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
       logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
       physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);*/
    //Moved to each of the setups for their different world sizes. Not done for base cases from PMT

    if (isCherenkov)
    {
        ConstructCherenkov();
    }
    else if (isMountain)
    {
        ConstructMountain();
    }
    else if (isIronSlab)
    {
        ConstructIronSlab();
    }
    else if (isScintillator)
    {
        ConstructScintillator();
    }
    else if (isTOF)
    {
        ConstructTOF();
    }
    else if (isAtmosphere)
    {
        ConstructAtmosphere();
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    if (/*logicDetector != NULL*/ !isAtmosphere)
    {
        //if the detector has been defined in any setup we wish to have a sensitivedetector
        logicDetector->SetSensitiveDetector(sensDet);
    }
    /*essentially logicdetector is not NULL if we leave it untouched.
       meaning if our setup doesn't havea logicdetector this if statement runs and runmanager is handed garbage
       this garbage causes crashes
       commented out for the atmospheric case, causes some issues*/
}
