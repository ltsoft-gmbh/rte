/* -*-plt-c++-*- */
#ifndef PLT_KEY_INCLUDED
#define PLT_KEY_INCLUDED
/*
 * Copyright (c) 1996, 1997, 1998, 1999
 * Lehrstuhl fuer Prozessleittechnik, RWTH Aachen
 * D-52064 Aachen, Germany.
 * All rights reserved.
 *
 * This file is part of the ACPLT/KS Package which is licensed as open
 * source under the Artistic License; you can use, redistribute and/or
 * modify it under the terms of that license.
 *
 * You should have received a copy of the Artistic License along with
 * this Package; see the file ARTISTIC-LICENSE. If not, write to the
 * Copyright Holder.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/* Author: Martin Kneissl <martin@plt.rwth-aachen.de> */

#include "plt/rtti.h"

//////////////////////////////////////////////////////////////////////

class PltKey 
{
public:
    virtual unsigned long hash() const = 0;
    virtual bool operator == (const PltKey &) const = 0;
    PLT_DECL_RTTI;
};

//////////////////////////////////////////////////////////////////////

#endif // PLT_KEY_INCLUDED
