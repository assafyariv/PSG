// Copyright NVIDIA Corporation 2002-2008
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once
/** \file */

#include "optix.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Templated base class to manage a set of children in a grouping node.
   *  \par Namespace: nvrt
   *  \remarks An RTChildrenManager provides the functionality to manage the handling of children of a
   *  specified type. RTGeometryGroup, RTGroup, and RTSelector derive from RTChildrenManager to have a
   *  unique interface on that task. Those classes need to provide the pure virtual functions that finally
   *  perform the required operations.
   *  \note While RTGroup and RTSelector can hold any RTNode as children, an RTGeometryGroup can hold only
   *  RTGeometryInstances.
   *  \sa RTGeometryGroup, RTGroup, RTSelector */
  template<typename T>
  class RTChildrenManager
  {
    public:
      /*! \brief Add a child to this grouping node.
       *  \param child A pointer to the templated handle to add as a child.
       *  \returns The index of \a child in the set of children.
       *  \sa clearChildren, getChild, getNumberOfChildren, removeChild, replaceChild */
      unsigned int addChild( const typename nvutil::ObjectTraits<T>::WeakPtr & child );

      /*! \brief Add a child to this grouping node.
       *  \param child A pointer to the templated handle to add as a child.
       *  \returns The index of \a child in the set of children.
       *  \sa clearChildren, getChild, getNumberOfChildren, removeChild, replaceChild */
      unsigned int addChild( const typename nvutil::ObjectTraits<T>::SharedPtr & child );

      /*! \brief Clear the set of children.
       *  \remarks Removes all children from this grouping node.
       *  \sa addChild, getChild, getNumberOfChildren, removeChild, replaceChild */
      void clearChildren();

      /*! \brief Get the child at the specified index from this grouping node.
       *  \param index The index of the child to get.
       *  \returns A pointer to the templated handle of the specified child.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of children of
       *  this grouping node.
       *  \sa addChild, clearChildren, getNumberOfChildren, removeChild, replaceChild */
      typename nvutil::ObjectTraits<T>::WeakPtr getChild( unsigned int index ) const;

      /*! \brief Get the number of children of this grouping node.
       *  \returns The number of children of this grouping node.
       *  \sa addChild, clearChildren, getChild, removeChild, replaceChild */
      unsigned int getNumberOfChildren() const;

      /*! \brief Remove the child at the specified index from this grouping node.
       *  \param index The index of the child to remove.
       *  \remarks The index of children following the child to remove will be one less after this
       *  operation.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of children of
       *  this grouping node.
       *  \sa addChild, clearChildren, getChild, getNumberOfChildren, replaceChild */
      void removeChild( unsigned int index );

      /*! \brief Replace the child at the specified index of this grouping node.
       *  \param index The index of the child to replace.
       *  \param child A pointer to the templated handle to replace a current child.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of children of
       *  this grouping node.
       *  \sa addChild, clearChildren, getChild, getNumberOfChildren, removeChild */
      bool replaceChild( unsigned int index, const typename nvutil::ObjectTraits<T>::WeakPtr & child );

    protected:
      /*! \brief Default-constructor of RTChildrenManager */
      RTChildrenManager();

      /*! \brief Destructor of RTChildrenManager */
      virtual ~RTChildrenManager();

      /*! \brief Interface to get the RTNode of this object.
       *  \returns The RTNode of this object.
       *  \remarks RTChildrenManager is designed to be inherited by classes that also derive from RTNode.
       *  This pure virtual function should return that RTNode. */
      virtual RTNodeWeakPtr doGetNode() const = 0;

      /*! \brief Interface to get the RTobject resource of the specified child.
       *  \param index The index of the child to get the resource from.
       *  \return The RTobject resource managed of the specified child.
       *  \remarks This class manages a templated set of handles, which in turn wraps RTobject resources.
       *  This function is used to get the resource of one of the children.
       *  \sa doGetNumberOfChildren, doSetNumberOfChildren, doSetChildResource */
      virtual RTobject doGetChildResource( unsigned int index ) const = 0;

      /*! \brief Interface to get the number of children of this grouping object.
       *  \returns The number of children of this grouping object.
       *  \remarks This class manages a templated set of handles, which in turn wraps RTobject resources.
       *  This function is used to get the number of children to store.
       *  \sa doGetChildResource, doSetNumberOfChildren, doSetChildResource */
      virtual unsigned int doGetNumberOfChildren() const = 0;

      /*! \brief Interface to set the number of children of this grouping object.
       *  \param numberOfChildren The number of children to reserve for this grouping object.
       *  \remarks This class manages a templated set of handles, which in turn wraps RTobject resources.
       *  This function is used to set the number of children to store.
       *  \sa doGetChildResource, doGetNumberOfChildren, doSetChildResource */
      virtual void doSetNumberOfChildren( unsigned int numberOfChildren ) = 0;

      /*! \brief Interface to set the RTobject resource for the specified index.
       *  \param index The index of the child to set.
       *  \param child The RTobject resource to set as a child.
       *  \remarks This class manages a templated set of handles, which in turn wraps RTobject resources.
       *  This function is used to set the RTobject resource of the specified child.
       *  \sa doGetChildResource, doGetNumberOfChildren, doSetNumberOfChildren */
      virtual void doSetChildResource( unsigned int index, RTobject child ) = 0;

    private:
      std::vector<typename nvutil::ObjectTraits<T>::SharedPtr>  m_children;
  };

  template<typename T>
  inline RTChildrenManager<T>::RTChildrenManager()
  {
  }

  template<typename T>
  inline RTChildrenManager<T>::~RTChildrenManager()
  {
    NVSG_ASSERT( m_children.empty() );
  }

  template<typename T>
  inline unsigned int RTChildrenManager<T>::addChild( const typename nvutil::ObjectTraits<T>::WeakPtr & child )
  {
    NVSG_ASSERT( child );
    NVSG_ASSERT( m_children.size() == doGetNumberOfChildren() );

    m_children.push_back( child );
    nvutil::WritableObject<T>(child)->addOwner( doGetNode() );

    unsigned int numberOfChildren = checked_cast<unsigned int>(m_children.size());
    doSetNumberOfChildren( numberOfChildren );

    unsigned int childIndex = numberOfChildren - 1;
    doSetChildResource( childIndex, nvutil::ReadableObject<T>(child)->getResource() );

    return( childIndex );
  }

  template<typename T>
  inline unsigned int RTChildrenManager<T>::addChild( const typename nvutil::ObjectTraits<T>::SharedPtr & child )
  {
    return( addChild( child.get() ) );
  }

  template<typename T>
  inline void RTChildrenManager<T>::clearChildren()
  {
    doSetNumberOfChildren( 0 );
    for ( size_t i=0 ; i<m_children.size() ; i++ )
    {
      nvutil::WritableObject<T>(m_children[i])->removeOwner( doGetNode() );
    }
    m_children.clear();
  }

  template<typename T>
  inline typename nvutil::ObjectTraits<T>::WeakPtr RTChildrenManager<T>::getChild( unsigned int index ) const
  {
    NVSG_ASSERT( index < m_children.size() );
    NVSG_ASSERT( nvutil::ReadableObject<T>(m_children[index])->getResource() == doGetChildResource( index ) );
    return( m_children[index].get() );
  }

  template<typename T>
  inline unsigned int RTChildrenManager<T>::getNumberOfChildren() const
  {
    NVSG_ASSERT( m_children.size() == doGetNumberOfChildren() );
    return( checked_cast<unsigned int>(m_children.size()) );
  }

  template<typename T>
  inline void RTChildrenManager<T>::removeChild( unsigned int index )
  {
    NVSG_ASSERT( index < m_children.size() );
    NVSG_ASSERT( m_children.size() == doGetNumberOfChildren() );

    if ( index < m_children.size() - 1 )
    {
      doSetChildResource( index, nvutil::ReadableObject<T>(m_children.back())->getResource() );
    }
    doSetNumberOfChildren( checked_cast<unsigned int>(m_children.size() - 1) );

    nvutil::WritableObject<T>(m_children[index])->removeOwner( doGetNode() );
    m_children[index] = m_children.back();
    m_children.pop_back();
  }

  template<typename T>
  inline bool RTChildrenManager<T>::replaceChild( unsigned int index, const typename nvutil::ObjectTraits<T>::WeakPtr & child )
  {
    NVSG_ASSERT( (index < m_children.size() ) && child );
    NVSG_ASSERT( m_children.size() == doGetNumberOfChildren() );

    bool changed = ( m_children[index] != child );
    NVSG_ASSERT( changed == (  nvutil::ReadableObject<T>(child)->getResource()
                            != nvutil::ReadableObject<T>(m_children[index])->getResource() ) );
    if ( changed )
    {
      RTobject gi = nvutil::ReadableObject<T>(child)->getResource();
      doSetChildResource( index, gi );

      nvutil::WritableObject<T>(m_children[index])->removeOwner( doGetNode() );
      m_children[index] = child;
      nvutil::WritableObject<T>(child)->addOwner( doGetNode() );
    }
    return( changed );
  }

}
