//#include <vld.h>		// For debugging 
// put this here to avoid linking error LNK2005 and lnk2009
#include "stdafx.h"
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "TStrackerMFC.h"

SystemPtr spinSystem;
TStrackerMain theMain;
HANDLE SpinSysMTX;
