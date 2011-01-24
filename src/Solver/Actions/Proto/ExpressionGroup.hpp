// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Solver_Actions_Proto_ExpressionGroup_hpp
#define CF_Solver_Actions_Proto_ExpressionGroup_hpp

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <boost/proto/proto.hpp>

/// @file
/// Grammar and transform to make grouping expressions possible

namespace CF {
namespace Solver {
namespace Actions {
namespace Proto {

/// Primitive transform to evaluate a group of expressions
template<typename GrammarT>
struct ExpressionGroup :
  boost::proto::transform< ExpressionGroup<GrammarT> >
{
  template<typename ExprT, typename StateT, typename DataT>
  struct impl : boost::proto::transform_impl<ExprT, StateT, DataT>
  { 
    
    typedef void result_type;
  
    /// Fusion functor to evaluate each child expression using the GrammarT supplied in the template argument
    struct evaluate_expr
    {
      evaluate_expr(typename impl::state_param state, typename impl::data_param data) :
        m_state(state),
        m_data(data)
      {
      }
      
      template<typename ChildExprT>
      void operator()(ChildExprT& expr) const
      {
        GrammarT()(expr, m_state, m_data);
      }

    private:
      typename impl::state_param  m_state;
      typename impl::data_param m_data;
    };
    
    void operator ()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
    ) const
    {
      // Use the group(expr1, expr2, ... exprN) proto expression as a fusion sequence. pop_front removes the group node itself from the list of expressions,
      // so this calls evaluate_expr on expr1, expr2, ... exprN
      boost::fusion::for_each( boost::fusion::pop_front(expr), evaluate_expr(state, data) );
    }
  };
};

/// Tags a terminal that indicates that each of its function arguments should be treated as an expression to be evaluated,
/// i.e. allow specifying a group of expressions
struct ExpressionGroupTag {};

/// Use group(expr1, expr2, ..., exprN) to evaluate a group of expressions
static boost::proto::terminal< ExpressionGroupTag >::type group = {};

/// Matches and evaluates groups of expressions matching GrammarT
template<typename GrammarT>
struct GroupGrammar :
  boost::proto::when
  <
    boost::proto::function< boost::proto::terminal<ExpressionGroupTag>, boost::proto::vararg<GrammarT> >,
    boost::proto::call< ExpressionGroup<GrammarT> >
  >
{
};
  
  
} // Proto
} // Actions
} // Solver
} // CF

#endif // CF_Solver_Actions_Proto_ExpressionGroup_hpp
