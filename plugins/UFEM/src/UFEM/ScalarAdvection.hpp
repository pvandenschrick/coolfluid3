//#ifndef SCALARADVECTION_H
//#define SCALARADVECTION_H

//#endif // SCALARADVECTION_H


// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_UFEM_ScalarAdvection_hpp
#define cf3_UFEM_ScalarAdvection_hpp

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>

#define BOOST_PROTO_MAX_ARITY 10
#ifdef BOOST_MPL_LIMIT_METAFUNCTION_ARITY
  #undef BOOST_MPL_LIMIT_METAFUNCTION_ARITY
#endif
#define BOOST_MPL_LIMIT_METAFUNCTION_ARITY 10

#include <boost/scoped_ptr.hpp>

#include "LibUFEM.hpp"
#include "LSSActionUnsteady.hpp"

#include "SUPG.hpp"

namespace cf3 {

namespace UFEM {

struct CrosswindDiffusion
{
  typedef Real result_type;
  
  CrosswindDiffusion() : d0(1.)
  {
  }
  
  template<typename UT, typename CT>
  Real operator()(const UT& u, const CT& c)
  {
    typedef typename UT::EtypeT ElementT;
    static const Uint dim = ElementT::dimension;
    typedef mesh::Integrators::GaussMappedCoords<1, ElementT::shape> GaussT;
    typedef Eigen::Matrix<Real, dim, 1> ColVecT;
    
    ColVecT g = c.nabla() * c.value();
    const Real grad_norm = g.norm();
    const Real u_norm = u.eval().norm();
    if(grad_norm < 1e-10 || u_norm < 1e-10)
    {
      return 0.;
    }
    g /= grad_norm;
    const Real hg = 2./(g.transpose()*c.nabla()).cwiseAbs().sum();
    return d0*hg*u_norm;
  }
  
  Real d0;
};
  
/// solver for scalar transport
class UFEM_API ScalarAdvection : public LSSActionUnsteady
{
public: // functions

  /// Contructor
  /// @param name of the component
  ScalarAdvection ( const std::string& name );

  /// Get the class name
  static std::string type_name () { return "ScalarAdvection"; }

private:

  /// Called when the internal name to use for the scalar variable is changed
  void trigger_scalar_name();

  /// Scalar diffusivity
  Real m_alpha;

  /// Parameter for the theta scheme
  Real m_theta;

  /// Stabilization coefficient
  Real tau_su;

  ComputeTau compute_tau;
  
  solver::actions::Proto::MakeSFOp<CrosswindDiffusion>::stored_type m_diff_data;
  solver::actions::Proto::MakeSFOp<CrosswindDiffusion>::reference_type diffusion_coeff;
};

} // UFEM
} // cf3


#endif // cf3_UFEM_NavierStokes_hpp
