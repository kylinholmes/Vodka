#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "EventPackage.h"
#include "Context.h"
#include "Route.h"
#include "error.h"
#include "log.h"
// #include "dllLoader.hpp"


void ProcessEvent(EventPackage* event);

void FrameWorkMain(EventPackage* event);

int endEvent(EventPackage* event);

#endif