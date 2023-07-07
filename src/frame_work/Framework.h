#pragma once

#include "EventPackage.h"
#include "Context.h"
#include "Route.h"
#include "error.h"
#include "log.h"
#include "dllLoader.hpp"

void ProcessEvent(EventPackage* event);

int EndEvent(EventPackage* event);

