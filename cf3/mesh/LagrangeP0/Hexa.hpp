// Copyright (C) 2010-2013 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_mesh_LagrangeP0_Hexa_hpp
#define cf3_mesh_LagrangeP0_Hexa_hpp

#include "mesh/ShapeFunctionBase.hpp"
#include "mesh/LagrangeP0/API.hpp"

namespace cf3 {
namespace mesh {
namespace LagrangeP0 {

////////////////////////////////////////////////////////////////////////////////

struct Mesh_LagrangeP0_API Hexa_traits
{
  enum { nb_nodes       = 1              };
  enum { nb_faces       = 6              };
  enum { dimensionality = 3              };
  enum { order          = 0              };
  enum { shape          = GeoShape::HEXA };
};

////////////////////////////////////////////////////////////////////////////////

/// @class Hexa
/// @verbatim
/// Local connectivity:
///                -------------
///               /|          /|
///              / |         / |
///             |-----------|  |
///             |  |        |  |
///             |  |   0    |  |
///             |  |--------|--|
///             | /         | /
///     ETA     |/          |/
///      |      -------------
///      |
///      o--- KSI
///     /
///    /
///  ZTA
/// Reference domain: <-1,1> x <-1,1> x <-1,1>
/// @endverbatim
/// @see ShapeFunction for documentation on undocumented static functions
struct Mesh_LagrangeP0_API Hexa : public ShapeFunctionBase<Hexa,Hexa_traits>
{
  static const RealMatrix& local_coordinates();
  static void compute_value(const MappedCoordsT& mapped_coord, ValueT& result);
  static void compute_gradient(const MappedCoordsT& mapped_coord, GradientT& result);
  static const RealMatrix& mononomial_coefficients();
  static const RealMatrix& mononomial_exponents();
};

////////////////////////////////////////////////////////////////////////////////

} // LagrangeP0
} // mesh
} // cf3

#endif // cf3_mesh_LagrangeP0_Hexa_hpp
