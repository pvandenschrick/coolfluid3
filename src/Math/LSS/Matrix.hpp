// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Math_LSS_Matrix_hpp
#define CF_Math_LSS_Matrix_hpp

////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/utility.hpp>

#include "Math/LibMath.hpp"
#include "Common/MPI/CommPattern.hpp"
#include "Math/LSS/BlockAccumulator.hpp"
#include "Math/LSS/Vector.hpp"

////////////////////////////////////////////////////////////////////////////////////////////

/**
  @file Matrix.hpp implementation of LSS::Matrix
  @author Tamas Banyai
**/

////////////////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Math {
namespace LSS {

////////////////////////////////////////////////////////////////////////////////////////////

class Math_API Matrix : public CF::Common::Component {
public:

  /// @name CREATION, DESTRUCTION AND COMPONENT SYSTEM
  //@{

  /// pointer to this type
  typedef boost::shared_ptr<Matrix> Ptr;

  /// const pointer to this type
  typedef boost::shared_ptr<Matrix const> ConstPtr;

  /// name of the type
  static std::string type_name () { return "Matrix"; }

  /// Default constructor
  Matrix(const std::string& name) : Component(name) { }

  /// Setup sparsity structure
  /// should only work with local numbering (parallel computations, plus rcm could be a totally internal matter of the matrix)
  /// internal mapping should be invisible to outside (needs to reorganize to push ghost nodes)
  /// maybe 2 ctable csr style
  /// local numbering
  /// needs global numbering for communication - ??? commpattern ???
  virtual void create(CF::Common::Comm::CommPattern& cp, Uint neq, std::vector<Uint>& node_connectivity, std::vector<Uint>& starting_indices, LSS::Vector::Ptr solution, LSS::Vector::Ptr rhs) = 0;

  /// Deallocate underlying data
  virtual void destroy() = 0;

  //@} END CREATION, DESTRUCTION AND COMPONENT SYSTEM

  /// @name INDIVIDUAL ACCESS
  //@{

  /// Set value at given location in the matrix
  virtual void set_value(const Uint icol, const Uint irow, const Real value) = 0;

  /// Add value at given location in the matrix
  virtual void add_value(const Uint icol, const Uint irow, const Real value) = 0;

  /// Get value at given location in the matrix
  virtual void get_value(const Uint icol, const Uint irow, Real& value) = 0;

  //@} END INDIVIDUAL ACCESS

  /// @name SOLVE THE SYSTEM
  //@{

  /// The holy solve, for solving the m_mat*m_sol=m_rhs problem.
  /// We bow on our knees before your greatness.
  virtual void solve(LSS::Vector::Ptr solution, LSS::Vector::Ptr rhs) = 0;

  //@} END SOLVE THE SYSTEM

  /// @name EFFICCIENT ACCESS
  //@{

  /// Set a list of values
  virtual void set_values(const BlockAccumulator& values) = 0;

  /// Add a list of values
  /// local ibdices
  /// eigen, templatization on top level
  virtual void add_values(const BlockAccumulator& values) = 0;

  /// Add a list of values
  virtual void get_values(BlockAccumulator& values) = 0;

  /// Set a row, diagonal and off-diagonals values separately (dirichlet-type boundaries)
  virtual void set_row(const Uint iblockrow, const Uint ieq, Real diagval, Real offdiagval) = 0;

  /// Get a column and replace it to zero (dirichlet-type boundaries, when trying to preserve symmetry)
  /// Note that sparsity info is lost, values will contain zeros where no matrix entry is present
  virtual void get_column_and_replace_to_zero(const Uint iblockcol, Uint ieq, std::vector<Real>& values) = 0;

  /// Add one line to another and tie to it via dirichlet-style (applying periodicity)
  virtual void tie_blockrow_pairs (const Uint iblockrow_to, const Uint iblockrow_from) = 0;

  /// Set the diagonal
  virtual void set_diagonal(const std::vector<Real>& diag) = 0;

  /// Add to the diagonal
  virtual void add_diagonal(const std::vector<Real>& diag) = 0;

  /// Get the diagonal
  virtual void get_diagonal(std::vector<Real>& diag) = 0;

  /// Reset Matrix
  virtual void reset(Real reset_to=0.) = 0;

  //@} END EFFICCIENT ACCESS

  /// @name MISCELLANEOUS
  //@{

  /// Print to wherever
  virtual void print(std::ostream& stream) = 0;

  /// Print to file given by filename
  virtual void print(const std::string& filename) = 0;

  /// Accessor to the state of create
  virtual const bool is_created() = 0;

  /// Accessor to the number of equations
  virtual const Uint neq() = 0;

  /// Accessor to the number of block rows
  virtual const Uint blockrow_size() = 0;

  /// Accessor to the number of block columns
  virtual const Uint blockcol_size() = 0;

  /// Accessor to solver type
  virtual const std::string solvertype() = 0;

  //@} END MISCELLANEOUS

}; // end of class Matrix

////////////////////////////////////////////////////////////////////////////////////////////

} // namespace LSS
} // namespace Math
} // namespace CF

#endif // CF_Math_LSS_Matrix_hpp