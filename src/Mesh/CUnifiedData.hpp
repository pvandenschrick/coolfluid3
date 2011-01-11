// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Mesh_CUnifiedData_hpp
#define CF_Mesh_CUnifiedData_hpp

#include <boost/tuple/tuple.hpp>

#include "Common/Foreach.hpp"
#include "Common/Component.hpp"
#include "Common/CLink.hpp"
#include "Common/CGroup.hpp"
#include "Common/String/Conversion.hpp"

#include "Mesh/CList.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {

////////////////////////////////////////////////////////////////////////////////

/// This class allows to access data spread over multiple components
/// with a continuous index
/// @pre the data components must be of the same type and must have
///      a member function "Uint size() const" defined.
template <typename DATA>
class Mesh_API CUnifiedData : public Common::Component
{
public:
    
  typedef boost::shared_ptr<CUnifiedData> Ptr;
  typedef boost::shared_ptr<CUnifiedData const> ConstPtr;
  
  typedef DATA data_type;
  
  typedef boost::tuple<boost::shared_ptr<data_type const>, Uint> const_data_location_type;
  typedef boost::tuple<boost::shared_ptr<data_type>, Uint>       data_location_type;
  
  /// Contructor
  /// @param name of the component
  CUnifiedData ( const std::string& name );

  /// Virtual destructor
  virtual ~CUnifiedData() {}

  /// Get the class name
  static std::string type_name () { return "CUnifiedData<"+data_type::type_name()+">"; }
  
  /// set the data given a range of components obtained for instance
  /// using the find_components_recursively<data_type>() function
  /// @param [in] range  The range of data components to be unified
  template <typename DataRangeT>
    void set_data(DataRangeT range );

  /// Get the component and local index in the component
  /// given a continuous index spanning multiple components
  /// @param [in] data_glb_idx continuous index covering multiple components
  /// @return boost::tuple<data_type::Ptr component, Uint idx_in_component> 
  data_location_type data_location(const Uint data_glb_idx);

  /// Get the const component and local index in the component
  /// given a continuous index spanning multiple components
  /// @param [in] data_glb_idx continuous index covering multiple components
  /// @return boost::tuple<data_type::ConstPtr component, Uint idx_in_component>   
  const_data_location_type data_location(const Uint data_glb_idx) const;
  
  /// Get the total number of data spanning multiple components
  /// @return the size
  Uint size() const;
  
  /// non-const access to the unified data components
  /// @return vector of data components
  std::vector<boost::shared_ptr<data_type> >& data_components();
  
  /// const access to the unified data components
  /// @return vector of data components
  const std::vector<boost::shared_ptr<data_type> >& data_components() const;
  
  Common::CGroup& data_links() { return *m_data_links; }
  
private: // data

  /// vector of components to view as continuous
  std::vector<boost::shared_ptr<data_type> > m_data_vector;

  /// start index for each component in the continous view
  boost::shared_ptr<CList<Uint> > m_data_indices;

  /// group with links to data components (links to elements of m_data_vector)
  Common::CGroup::Ptr m_data_links;
  
  /// total number of indices spanning all components
  Uint m_size;
  
}; // CUnifiedData

////////////////////////////////////////////////////////////////////////////////

template <typename DATA>
inline CUnifiedData<DATA>::CUnifiedData ( const std::string& name ) : Component(name)
{
  m_data_indices = create_static_component<CList<Uint> >  ("data_indices");
  m_data_links   = create_static_component<Common::CGroup>("data_links");
}

////////////////////////////////////////////////////////////////////////////////

/// set the data given a range of components obtained for instance
/// using the find_components_recursively<data_type>() function
/// @param [in] range  The range of data components to be unified
template <typename DATA>
template <typename DataRangeT>
inline void CUnifiedData<DATA>::set_data(DataRangeT range)
{
  m_data_vector.clear();
  CList<Uint>::Buffer data_start_indices = m_data_indices->create_buffer();
  Uint sum = 0;
  data_start_indices.add_row(sum);
  index_foreach(i, Component& data_val, range)
  {
    Component::Ptr linked = data_val.self();
    while (linked->is_link())
      linked = linked->get();
    m_data_vector.push_back(linked->as_type<typename CUnifiedData<DATA>::data_type>());
    m_data_links->create_component<Common::CLink>("data_component_"+Common::String::to_str(i))->link_to(linked);
    sum += linked->as_type<typename CUnifiedData<DATA>::data_type>()->template size(); 
    data_start_indices.add_row(sum);
  }
  m_size = sum;
}

////////////////////////////////////////////////////////////////////////////////

/// Get the component and local index in the component
/// given a continuous index spanning multiple components
/// @param [in] data_glb_idx continuous index covering multiple components
/// @return boost::tuple<data_type::Ptr component, Uint idx_in_component> 
template <typename DATA>
inline typename CUnifiedData<DATA>::data_location_type CUnifiedData<DATA>::data_location(const Uint data_glb_idx)
{
  const Uint data_vector_idx = std::upper_bound(m_data_indices->array().begin(), m_data_indices->array().end(), data_glb_idx) - 1 - m_data_indices->array().begin();
  return boost::make_tuple(m_data_vector[data_vector_idx], data_glb_idx - m_data_indices->array()[data_vector_idx]);
}

////////////////////////////////////////////////////////////////////////////////

/// Get the const component and local index in the component
/// given a continuous index spanning multiple components
/// @param [in] data_glb_idx continuous index covering multiple components
/// @return boost::tuple<data_type::ConstPtr component, Uint idx_in_component>
template <typename DATA>
inline typename CUnifiedData<DATA>::const_data_location_type CUnifiedData<DATA>::data_location(const Uint data_glb_idx) const
{
  const Uint data_vector_idx = std::upper_bound(m_data_indices->array().begin(), m_data_indices->array().end(), data_glb_idx) - 1 - m_data_indices->array().begin();
  return boost::make_tuple(m_data_vector[data_vector_idx].as_const().as_type<CUnifiedData<DATA>::data_type>(), data_glb_idx - m_data_indices->array()[data_vector_idx]);
}

////////////////////////////////////////////////////////////////////////////////

/// Get the total number of data spanning multiple components
/// @return the size
template <typename DATA>
inline Uint CUnifiedData<DATA>::size() const
{
  return m_size;
}

////////////////////////////////////////////////////////////////////////////////

/// non-const access to the unified data components
/// @return vector of data components
template <typename DATA>
inline std::vector<boost::shared_ptr<typename CUnifiedData<DATA>::data_type> >& CUnifiedData<DATA>::data_components()
{
  return m_data_vector;
}

////////////////////////////////////////////////////////////////////////////////

/// const access to the unified data components
/// @return vector of data components
template <typename DATA>
inline const std::vector<boost::shared_ptr<typename CUnifiedData<DATA>::data_type> >& CUnifiedData<DATA>::data_components() const
{
  return m_data_vector;
}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_CUnifiedData_hpp