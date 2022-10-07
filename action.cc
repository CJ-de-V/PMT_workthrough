#include "action.hh"

MyActionInitialization::MyActionInitialization(){

}

MyActionInitialization::~MyActionInitialization(){

}

void MyActionInitialization::BuildForMaster() const{

          MyRunAction *runAction = new MyRunAction();
          SetUserAction(runAction);

}

void MyActionInitialization::Build () const
{
        MyPrimaryGenerator *generator = new MyPrimaryGenerator();
        SetUserAction(generator);

        MyRunAction *runAction = new MyRunAction();
        SetUserAction(runAction);

        MyEventAction *eventaction = new MyEventAction(runAction);
        SetUserAction(eventaction);

        MySteppingAction *steppingaction = new MySteppingAction(eventaction);
        SetUserAction(steppingaction);
}
