//* CMSC240 Final Project : Vehicle.cpp
//* Authors: Are Oelsner, Maddie Shea, Ryan Jennings
//* Date: 12/18/2017

#include "Vehicle.h"

string intToDir(int dir);

Vehicle::Vehicle(int size, double _rightProb, double _leftProb, Lane* _lane) {

  hasPassedLight = false;

  frontCurrLane = _lane;
  backCurrLane = _lane;

  // Handle the probabilities that the Vehicle will spawn as a Car/SUV/Truck
  // Vehicle::decideType(_carProb, _suvProb, _truckProb);
  this->size = size; // Sets vehicle type

  sections = frontCurrLane->allocSections(size);

  for(int i = 0; (unsigned)i < sections.size(); i++) {
    sections[i]->setOccupied(true);
  }

  // Set front and back sections occupied by the Vehicle
  frontSection = sections[sections.size() - 1];
  backSection = sections[0];

  frontSection->setOccupied(true);
  backSection->setOccupied(true);

  inIntersection = false;    // Vehicles will spawn at the end of the lanes
  nearIntersection = false;  // Minimum lane length prevents spawning near or
  // in the Intersection

  Vehicle::decidePath(_rightProb, _leftProb);

  exited = false;
}

Vehicle::~Vehicle(){ }

//  void Vehicle::decideType(double _carProb, double _suvProb, double _truckProb) {
//
//    // Error catching
//    if ( (_carProb + _suvProb + _truckProb) != 1) {
//      cout << "Invalid probabilities: ( Car: " << _carProb  << ", SUV: " << _suvProb << ", Truck: " << _truckProb << " )" << endl;
//      cout << "Probabilities of Vehicle types must add to 1. (_carProb + _suvProb + _truckProb = 1)" << endl;
//      cout << "Usage: Vehicle(double _carProb, double _suvProb, double _truckProb, double _probRight, double _probLeft, Lane* _lane, vector<Section*> _sections)" << endl;
//      exit(0);
//    }
//
//    // If the probabilities are valid, decide what type the vehicle will be
//    else {
//
//      double carNumLine = _carProb;
//      double suvNumLine = _suvProb + carNumLine;
//      double truckNumLine = _truckProb + suvNumLine;
//
//      size = 2; // delete this later
//
//      /*
//      double val = Random::getRandDouble(0.0,1.0);
//
//      if(val <= carNumLine) {
//        size = 2;
//      }
//
//      else if(val <= suvNumLine) {
//        size = 3;
//      }
//
//      else if(val <= truckNumLine) {
//        size = 4;
//      }
//      */
//    }
//  }

void Vehicle::decidePath(double _rightProb, double _leftProb) {

  double rightNumLine = _rightProb;
  double leftNumLine = _leftProb + rightNumLine;

  double val = Random::randDouble(0.0, 1.0);

  if( val < rightNumLine ) {
    path = 'r';
  }
  else if ( val < leftNumLine ) {
    path = 'l';
  }
  else {
    path = 's';
  }
}

bool Vehicle::canMove(char _direction) {

  // Check if the section in the desired direction is occupied
  bool pathBlocked;

  int frontLaneDir = frontCurrLane->getDirection();

  nearIntersection = frontSection->getNearIntersection();

  if( _direction == 'l' && frontSection->getLeft(frontLaneDir) != NULL) {
    pathBlocked = frontSection->getLeft(frontLaneDir)->getOccupied();
  }
  else if( _direction == 'r' && frontSection->getRight(frontLaneDir) != NULL) {
    pathBlocked = frontSection->getRight(frontLaneDir)->getOccupied();
  }
  else if( _direction == 's' && frontSection->getStraight(frontLaneDir) != NULL) {//TODO here
    pathBlocked = frontSection->getStraight(frontLaneDir)->getOccupied();
  }

  // Get the current state of the traffic light in the Vehicle's lane
  if(!hasPassedLight) {
    TrafficLight::Color lightColor = frontCurrLane->getTrafficLight().getColor();
    bool lightIsGreen = (lightColor == 1);
    //TODO this would set it one section early 
    nearIntersection = frontSection->getNearIntersection();
    if(nearIntersection && !lightIsGreen) {
      return false;
    }
  }

  /* If there is a Vehicle directly in front of it or the Vehicle is near 
   *  the intersection and the light is not green, then the Vehicle cannot
   *  legally move */
  if (pathBlocked) { //TODO problem here
    return false;
  }

  // Otherwise, the Vehicle is permitted to move in the desired direction
  return true;
}


void Vehicle::move() {

  if(!exited) {

    int frontLaneDir = frontCurrLane->getDirection();
    int backLaneDir = backCurrLane->getDirection();

    // TODO make sure these null comparisons work
    bool frontCanTurnRight = frontSection->getRight(frontLaneDir) != NULL;
    bool frontCanTurnLeft = frontSection->getLeft(frontLaneDir) != NULL;
    bool backCanTurnRight = backSection->getRight(backLaneDir) != NULL;
    bool backCanTurnLeft = backSection->getLeft(backLaneDir) != NULL;

    // Check if any section of the vehicle is in the intersection
    if(frontSection->getInIntersection() || backSection->getInIntersection()) { 
      inIntersection = true;
    }

    char direction = 's';
    // If the Vehicle is attempting to move while in the intersection, check
    // if it should turn or continue straight
    if(inIntersection) {
      // If the Vehicle's path indicates that it will turn left or right,
      // check if a turn needs to occur
      if( (path == 'l' && frontCanTurnLeft) || (path == 'r' && frontCanTurnRight) ) {
        direction = path;
      }
    }

    bool moved = false;
    if(frontSection->getNearEdge()) {
      // If the front and back sections are the same, move the vehicle
      // out of view
      while (!backSection->getNearEdge()) {
        backSection->setOccupied(false);
        backSection = backSection->getStraight(backLaneDir);
      }
      backSection->setOccupied(false);
      frontSection->setOccupied(false); 
      exited = true;
    }
    else if(this->canMove(direction)) {

      // HANDLE FRONT SECTION
      // Set the front section of this vehicle one section forward

      if(direction == 's') {
        frontSection = frontSection->getStraight(frontLaneDir);
        moved = true;
      }
      else if (direction == 'l' && frontCanTurnLeft) {
        frontSection = frontSection->getLeft(frontLaneDir);
        hasPassedLight = true;
        frontCurrLane = frontCurrLane->getLeft();
        moved = true;
      }
      else if (direction == 'r' && frontCanTurnRight) {
        frontSection = frontSection->getRight(frontLaneDir);
        hasPassedLight = true;
        frontCurrLane = frontCurrLane->getRight();
        moved = true;
      }
      // Set new front section to be occupied
      frontSection->setOccupied(true);
    }

    //Dealing with Back section
    if(moved) {
      bool pathBlocked = false;
      if( direction == 'l' && backSection->getLeft(backLaneDir) != NULL) { //backLaneDir??
        pathBlocked = backSection->getLeft(backLaneDir)->getOccupied();
      }
      else if( direction == 'r' && backSection->getRight(backLaneDir) != NULL) {
        pathBlocked = backSection->getRight(backLaneDir)->getOccupied();
      }
      if(!pathBlocked) {    //If true can't go

        // Set original back section to be unoccupied
        backSection->setOccupied(false);
        // Set the back section of this vehicle one section forward

        if(direction == 's') {
          backSection = backSection->getStraight(backLaneDir);
        }
        else if (direction == 'l' && backCanTurnLeft) {
          backSection = backSection->getLeft(backLaneDir);
          backCurrLane = backCurrLane->getLeft();
        }
        else if (direction == 'r' && backCanTurnRight) {
          backSection = backSection->getRight(backLaneDir);
          backCurrLane = backCurrLane->getRight();
        }

      }
    }
  }
}

void
Vehicle::
printVehicle() {
  cout << "front Lane: \t\t" << intToDir(frontCurrLane->getDirection()) << endl;
  cout << "back Lane: \t\t" << intToDir(backCurrLane->getDirection()) << endl;
  cout << "in Intersection: \t" << inIntersection << endl;
  cout << "near Intersection: \t" << nearIntersection << endl;
  cout << "has Passed Light: \t" << hasPassedLight << endl;
  cout << "# sections: \t\t" << sections.size() << endl;
  cout << "size: \t\t\t" << size << endl;
  cout << "path: \t\t\t" << path << endl;
  cout << "exited: \t\t" << exited << endl;
}

string intToDir(int dir) {
  switch(dir) {
    case 1: return "north";
    case 2: return "east";
    case 3: return "south";
    case 4: return "west";
  }
  return "error";
}
