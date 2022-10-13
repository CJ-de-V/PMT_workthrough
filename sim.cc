#include <iostream>

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "QGSP_BERT.hh"
//standard physics list for hadron processes
#include "G4DecayPhysics.hh" //why can i leave this out again?

#include "construction.hh"
#include "physics.hh"
#include "action.hh"

int main(int argc, char **argv)
{
    #ifdef G4MULTITHREADED
    G4MTRunManager *runManager = new G4MTRunManager();
    //initialize g4kernel, just how it's done
    runManager->SetNumberOfThreads(8);
    #else
    //kind of a stub since multithreading is the default
    G4RunManager *runManager = new G4RunManager();
    #endif

    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new MyActionInitialization());

    //atmospheric processes
    G4VModularPhysicsList *physics = new QGSP_BERT();
    physics->RegisterPhysics(new G4DecayPhysics());
    runManager->SetUserInitialization(physics);

    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();                            // into run.mac for multithreading reasons
    G4UImanager *UImanager = G4UImanager::GetUIpointer();


    if (argc == 1)
    {
        //Visual mode
        //if we're interested in visuals, no run file given
        G4UIExecutive *ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }
    else
    {
        //Batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    //runManager->Initialize();

    return 0;
}
