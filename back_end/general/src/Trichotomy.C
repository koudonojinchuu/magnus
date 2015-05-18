// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Member initializations for class Trichotomy
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//

#include "Trichotomy.h"


//---------------- global member initializations -----------------//


// YES, NO and DONTKNOW are provided for backward compatibility
// dontknow is a new (recommended) synonym for DONTKNOW, same for yes and no

const Trichotomy YES = Trichotomy(true);

const Trichotomy yes = YES;

const Trichotomy NO = Trichotomy(false);

const Trichotomy no = NO;

const Trichotomy DONTKNOW = Trichotomy(Trichotomy::dontknow());

const Trichotomy dontknow = DONTKNOW;
