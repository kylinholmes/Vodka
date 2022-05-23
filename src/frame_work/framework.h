#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "EventPackage.h"
#include "Context.h"
#include "Route.h"
// #include "dllLoader.hpp"


void processEvent(EventPackage* event);

void FrameWorkMain(EventPackage* event);

int endEvent(EventPackage* event);

#endif