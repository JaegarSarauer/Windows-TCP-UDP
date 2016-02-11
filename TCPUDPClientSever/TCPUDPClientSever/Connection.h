#ifndef CONNECTION_H
#define CONNECTION_H

#include "Shared.h"
#include <Commctrl.h>

void SetupAsClient (bool isTCP);

void SetupAsServer (bool isTCP);

void SetupShared (bool isClient, bool isTCP);



#endif