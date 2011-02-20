// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.


#include "RDM/LinearAdv2D.hpp"

/////////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace RDM {

///////////////////////////////////////////////////////////////////////////////////////

LinearAdv2D::LinearAdv2D()
{
}

/////////////////////////////////////////////////////////////////////////////////////

LinearAdv2D::~LinearAdv2D()
{
}

/////////////////////////////////////////////////////////////////////////////////////

Real LinearAdv2D::flux(const RealVector2 & coord, const Real & u, const RealVector2 & gradu)
{
    return 1.0*gradu[XX] + 1.0 * gradu[YY];
}

/////////////////////////////////////////////////////////////////////////////////////

Real LinearAdv2D::Lu(const RealVector2 & coord, const Real & u, const RealVector2 & gradN)
{
    return 1.0*gradN[XX] + 1.0 * gradN[YY];
}

/////////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

/////////////////////////////////////////////////////////////////////////////////////
