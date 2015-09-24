// Copyright NVIDIA Corporation 2010
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

#include "nvsgcommon.h"
#include "nvutil/Observer.h"
#include "nvutil/ReadableObject.h"

namespace nvutil
{
  /** \brief Observer class with a back-link to a HandledObject.
    * \remarks A BackLinkObserver can be used to carry changed state information up a hierarchy of objects.
    * Every nvsg::Object, for example, holds a BackLinkObserver for that reason. */
  class BackLinkObserver : public Observer
  {
    public:
      /*! \brief Static member function to create a BackLinkObserverSharedPtr.
       *  \return A newly created BackLinkObserverSharedPtr. */
      NVSG_API static BackLinkObserverSharedPtr create();

    public:
      /** \brief Needed to be called once for initialization! */
      NVSG_API void setBackLink( const HandledObjectWeakPtr & backLink );

    protected:
      friend struct nvutil::Holder<BackLinkObserver>;

      NVSG_API BackLinkObserver();
      NVSG_API virtual ~BackLinkObserver();

      NVSG_API virtual unsigned int onUpdate( const Subject *subject, unsigned int currentState, unsigned int changeState, const nvutil::Subject::SmartPayload &payload ) const;

    private:
      HandledObjectWeakPtr m_backLink;
  };

  inline BackLinkObserverSharedPtr BackLinkObserver::create()
  {
    return( ObjectTraits<BackLinkObserver>::Handle::create() );
  }

  inline BackLinkObserver::BackLinkObserver()
    : m_backLink(0)
  {
    NVSG_TRACE();
  }

  inline BackLinkObserver::~BackLinkObserver()
  {
    NVSG_TRACE();
  }

  inline unsigned int BackLinkObserver::onUpdate( const Subject *subject, unsigned int currentState, unsigned int updateState, const nvutil::Subject::SmartPayload &payload ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_backLink );

    return HandledObjectReadLock(m_backLink)->onUpdate( subject, currentState, updateState, payload );
  }

  inline void BackLinkObserver::setBackLink( const HandledObjectWeakPtr & backLink )
  {
    NVSG_TRACE();
    m_backLink = backLink;
  }

} // namespace nvutil
