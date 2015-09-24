// Copyright NVIDIA Corporation 2002-2005
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

#include "nvsgcommon.h"

#include <vector>
#include <map>
#include "nvsg/CoreTypes.h"
#include "nvutil/HandledObject.h"
#include "nvutil/Singleton.h"
#include "nvutil/SWMRSync.h"
#include "nvutil/WritableObject.h"

namespace nvsg
{
class DALServer;
class DALData;
class DALHost;

#if defined(_WIN32)
typedef DWORD THREADID;
#elif defined(LINUX)
typedef pthread_t THREADID;
#else
#error Unsupported Operating System!
#endif

/*! \brief Handle to a Device Abstraction Link (DAL) */
typedef unsigned long HDAL;
/*! \brief Indicates a invalid handle value */
extern NVSG_API const HDAL HDAL_INVALID; 

inline THREADID getCallingThread()
{
#if defined(_WIN32)
  return GetCurrentThreadId();
#elif defined(LINUX)
  return pthread_self();
#endif
}

class DALHostHolder
{
public:
  NVSG_API DALHostHolder();
  NVSG_API virtual ~DALHostHolder();
  NVSG_API DALHostHolder(const DALHostHolder& rhs);
  NVSG_API DALHostHolder& operator=(const DALHostHolder& rhs);

  NVSG_API DALHostWeakPtr getDALHost() const { return( m_host.get() ); }
  NVSG_API void releaseDALData();

private:
  DALHostSharedPtr m_host;
};

/*! \brief Represents an object capable to host a Device Abstraction Link
  * \par Namespace: nvsg
  * \remarks
  * A DALHost is intended to host a Device Abstraction Link (DAL) to attach arbitrary DALData.
  * A DAL links device or platform dependent DALData created by a DALDataCreator to an independent 
  * DALHost.
  * \n\n
  * An Object, for example, 'is a' DALHost.
  */
class DALHost : public nvutil::HandledObject
{
#if !defined(DOXYGEN_IGNORE)
  friend class DALServer;
#endif

public:
  NVSG_API static DALHostSharedPtr create();

public:
  /*! \brief Stores Device Abstraction Link Data.
    * \param
    * dataID Identifies the kind of data to store. This is so the user can identify the kind
    * of data, especially when stored data will be retrieved later.
    * \param
    * data Pointer to the DALData to store.
    * \return
    * The function returns a handle that identifies the Device Abstraction Link the stored data is
    * associated with. If the function fails, HDAL_INVALID will be returned.
    * \remarks
    * The function attaches the DALData identified by \a dataID, and pointed to by \a data. 
    * Different DALDataCreators can store data with the same \a dataID. In turn, a certain 
    * DALDataCreator can retrieve DALData stored by a different DALDataCreator. That is, two or
    * more DALDataCreators can share DALData if appropriate. For example, a particular OpenGL
    * renderer (nvutil::GLTraverser) might use OpenGL specific texture information that was stored
    * to a certain TextureHost by another OpenGL renderer, provided the two renderers share their
    * render contexts.
    * \n\n
    * Use DALHost::getDeviceAbstractionLinkData to retrieve particular data from the DALHost.
    * \n\n
    * The handle to the Device Abstraction Link (DAL) that will be returned by the function must be 
    * stored by the DALDataCreator. This handle is used, for example, to communicate to the 
    * DALDataCreator that a certain DAL hosted by a DALHost has been destroyed, and hence, stored 
    * DALData has been released as a consequence (DALDataCreator::onReleaseDAL). Or, in turn, if a 
    * DALDataCreator needs to dismiss stored DALData, this handle is used to identify the DAL that
    * is associated to that particular data. 
    * \n\n
    * The behavior is undefined if invalid DALData is passed. The behavior also is undefined if
    * the DALData pointed to by \a data already is hosted by another host!
    * \sa DALHost::getDeviceAbstractionLinkData, DALData, DALDataCreator
    */
  NVSG_API HDAL storeDeviceAbstractionLinkData(unsigned int dataID, DALData * data);
  
  /*! \brief Releases Device Abstraction Link data.
    * \remarks
    * The function releases the host's internal Device Abstraction link with all DALData attached 
    * to it. In turn, the particular DALDataCreator will be informed that the DALData they stored
    * with this host became invalid, so the creator can dismiss the stored handles they received
    * at the time they stored the data using DALHost::storeDeviceAbstractionLinkData.
    * \sa DALHost::storeDeviceAbstractionLinkData
    */
  NVSG_API void releaseDeviceAbstractionLinkData();
  
  /*! \brief Retrieves Devices Abstraction Link data from the DALHost.
    * \param
    * dataID Identifies the kind of data to retrieve.
    * \param
    * cache Reference to a pointer to DALData that receives the pointer to the stored DALData. 
    * \param
    * test A test function of the form 'bool test(const DALDataType*)' to be applied on the 
    * requested DALData.
    * \return
    * The function returns \c true if the DALData could have been retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * DALDataCreators can use this function to retrieve back DALData they previously stored using
    * DALHost::storeDeviceAbstractionLinkData. For this, a creator needs to pass the same \a dataID
    * as used with DALHost::storeDeviceAbstractionLinkData. In addition, the creator must specify a
    * test function of the form 'bool test(const DALDataType *)'. 
    * \n\n
    * The function looks up the DALData by the \a dataID and, if found, passes that data to the 
    * \a test function. If the test function returns \c true, the pointer to the particular DALData 
    * will be assigned to \a cache and the function returns \c true. 
    * \n\n
    * The reason why the test function is needed here is so that different creators can store 
    * different data having the same data ID. The test function verifies that specific data
    * is the right data the caller is looking for. A concrete example would be some texture 
    * information that was created by one renderer and used by some other renderer.
    * \sa DALHost::getLastRecentlyUsedDeviceAbstractionLinkData
    */
  template<typename DALDataType, typename TestFunc>
  bool getDeviceAbstractionLinkData(unsigned int dataID, DALDataType *& cache, TestFunc test) const;

  /*! \brief Returns the Device Abstraction Link data that was queried last.
    * \return
    * The function returns a pointer to the DALData that was successfully queried last.
    * \remarks
    * If particular, DALData could have queried successfully using 
    * DALHost::getDeviceAbstractionLinkData, so the pointer to that data is cached as the most recently
    * used data. For performance reasons, it is recommended to always call this function prior to
    * DALHost::getDeviceAbstractionLinkData with DALData queries.
    * \par Example:
    * The following code example shows how the nvtraverser::GLTraverser queries a given 
    * nvsg::PrimitiveSet for a probably attached display list information structure.
    * \code
    * // isDLInfoValid is used as test function while querying DLInfo data attached to a DALHost,
    * // which in this particular case is a PrimitiveSet
    * bool GLTraverser::isDLInfoValid(const GLTraverser::DLInfo* info)
    * { // a DLInfo is valid if the name of the display list object
    *   // is known by the current GL render context
    *   return glContext==info->context // we are fine if the current context corresponds the info's context
    *         || !!GLIsList(info->listID); // if not, we need to go deeper
    * }
    *
    * // getCachedDLInfo queries a given PrimitiveSet for DLInfo that might be attached to the 
    * // PrimitiveSet earlier
    * GLTraverser::DLInfo * GLTraverser::getCachedDLInfo( const PrimitiveSet *p ) const
    * {
    *   DLInfo * dlInfo = dynamic_cast<DLInfo*>(p->getLastRecentlyUsedDeviceAbstractionLinkData());
    *
    *   if ( !dlInfo || !isDLInfoValid(dlInfo) )
    *   { 
    *     // the last recently queried data is not valid for us, we need to go deeper
    *     p->getDeviceAbstractionLinkData(GL_DL, dlInfo, &GLTraverser::isDLInfoValid);
    *   }
    *   return( dlInfo );
    * }
    * \endcode
    * \sa DALHost::getDeviceAbstractionLinkData
    */
  NVSG_API DALData * getLastRecentlyUsedDeviceAbstractionLinkData() const;

#if !defined(DOXYGEN_IGNORE)
protected:
  friend struct nvutil::Holder<DALHost>;
  DALHost();
  DALHost(const DALHost&);
  ~DALHost();
  DALHost & operator=(const DALHost & rhs) {return *this;}
private:
  HDAL m_hdal;
  mutable std::map<THREADID, DALData*> m_recent; // per-thread recent data
  typedef std::map<THREADID, DALData*>::iterator RecentIter;
  typedef std::map<THREADID, DALData*>::const_iterator ConstRecentIter;
#endif
};

/*! \brief Represents the interface required for a creator of DALData.
  * \par Namespace: nvsg
  * \remarks 
  * Objects that create DALData to attach it to a Device Abstraction Link (DAL) hosted by a DALHost
  * must derive from this base class.
  */
class DALDataCreator
{
public:
#if !defined(DOXYGEN_IGNORE)
  virtual ~DALDataCreator() {}
#endif
  /*! \brief Called from the framework immediately before the Device Abstraction Link (DAL) becomes 
    * invalid.
    * \param
    * hDAL Handle to the Device Abstraction Link that is about to become invalid.
    * \remarks
    * Called from the framework to inform creators that the DAL identified by \a hDAL immediately 
    * becomes invalid. For example, this is the case when the object that hosts the DAL will be 
    * deleted. 
    * \n\n
    * A concrete DALDataCreator usually overrides this function to dismiss all temporary stored 
    * handles to the DALs that the DALDataCreator has attached DALData to. A DAL handle will be returned
    * to a DALDataCreator when it attaches DALdata to the DALHost through
    * DALHost::storeDeviceAbstractionLinkData. 
    * \n\n
    * Note that the creator must not explicitly delete the attached data! The deletion of the 
    * attached data will be performed by the framework, immediately after this function was called.
    */
  virtual void onReleaseDAL(HDAL hDAL) {}

protected:
#if !defined(DOXYGEN_IGNORE)
  // DALDataCreator serves as base class only. 
  DALDataCreator() {}
#endif

  /*! \brief Releases Device Abstraction Link Data.
    * \param
    * hDAL Handle to the Device Abstraction Link the data to release is attached to.
    * \param
    * dataID Identifies the kind of data to release.
    * \remarks
    * Releases the data identified by \a dataID that was attached to the 
    * Device Abstraction Link identified by \a hDAL.
    * \n\n
    * This function should be called by the creator immediately before this creator becomes invalid, 
    * to free all the data of kind \a dataID that was created by this DALDataCreator and attached 
    * to the DAL identified by \a hDAL. The function deletes the data and removes the according 
    * entry from the DAL. 
    */
  NVSG_API void releaseDeviceAbstractionLinkData(HDAL hDAL, unsigned int dataID);
};

/*! \brief Device Abstraction Link Data.
  * \par Namespace: nvsg
  * \remarks
  * DALData can be used to abstract device dependent data that will be linked to device independent
  * data via a Device Abstraction Link (DAL). 
  * \n\n
  * Note that only a DALDataCreator is permitted to create DALData. DALData can be attached to a DAL
  * using the interface of a DALHost.
  * \sa DALHost, DALDataCreator 
  */
class DALData
{
#if !defined(DOXYGEN_IGNORE)
  friend class DALServer;
  friend class DALDataCreator;
  friend class DALHost;
protected:
  NVSG_API virtual ~DALData();
#endif

public:

  /*! \brief Codes for different DALData types
   *
   *  */
  enum
  {
    DT_INVALID = ~0               //!< invalid DALData type
  , DT_DALDATA = 0                //!< DALData
  , DT_GLDALDATA                  //!< GLDALData
  , DT_GLARRAYBUFFERCACHE         //!< GLArrayBufferCache
  , DT_GLDISPLAYLISTCACHE         //!< GLDisplayListCache
  , DT_GLTEXTURECACHE             //!< GLTextureCache
  , DT_GLELEMENTARRAYBUFFERCACHE  //!< GLElementArrayBufferCache
  , DT_GLPIXELBUFFERCACHE         //!< GLPixelBufferCache
  
  , DT_RTDALDATA                  //!< Ray Tracing DAL data handling
  , DT_RTBUFFERCACHE
  , DT_RTVASBUFFERCACHE
  , DT_RTSELECTORCACHE
  , DT_RTTRANSFORMCACHE
  , DT_RTGEOMETRYGROUPCACHE
  , DT_RTMATERIALCACHE
  , DT_RTFXSCENEATTRIBUTECACHE
  , DT_RTGROUPCACHE
  , DT_RTGEOMETRYCACHE
  , DT_RTTEXTURESAMPLERCACHE
  
  , DT_CUSTOMDALDATA = 128        //!< Custom DALData types
  };

  /*! \brief Get the user ID of this DALData
   *  \return The current user ID of this DALData.
   *  \remarks A DALData can hold an userID. The default value is 0. This ID can be used to identify
   *  special informations for this DALData.\n\n
   *  For example, the CPUSkinningProcessor sets this ID to 'CPUS' in the array buffer caches, that
   *  hold the data for the skinned VertexAttributeSets. The GPUSkinningProcessor, in turn, tests if
   *  any array buffer cache of a VertexAttributeSet already available. If it is not zero, it is
   *  refilled with the (unskinned) data of that VertexAttributeSet.
   *  \sa setUserID */
  NVSG_API unsigned int getUserID() const;

  /*! \brief Set the user ID of this DALData.
   *  \param userID The user ID to use.
   *  \remarks A DALData can hold an userID. The default value is 0. This ID can be used to identify
   *  special informations for this DALData.\n\n
   *  \sa getUserID */
  NVSG_API void setUserID( unsigned int userID );

  /*! \brief Verifies if this DALData was created by the indicated creator. 
   *  \param creator Specifies the creator to test.
   *  \return The function returns \c true if this DALData was created by \a creator. 
   *  Otherwise, it returns \c false. 
   *  Use this function to check whether a DALData was created by a specific creator. 
   *  This in particular is useful to verify if a DALData can be safely used in a given context.
   */
  NVSG_API bool verifyCreator(const DALDataCreator * creator) const; 

  /* \brief Determine whether the object is of type typeId.
   * \remarks A class that derives from DALData should override isTypeOf() 
   * and extend the DALData type enum. The isTypeOf() function should look 
   * somewhat like this, to enable recursive type 
   * class DALDataPrime : public DALData
   * {
   *   public:
   *   virtual bool isTypeOf( int typeId ) const 
   *   { 
   *     return typeId == DT_DALDATAPRIME || DALData::isTypeOf( typeId );
   *   }
   * };
   */
  NVSG_API virtual bool isTypeOf( int typeId ) const { return typeId == DT_DALDATA; };

protected:
  /*! \brief Constructs a DALData object.
    * \param
    * creator Pointer to the DALDataCreator that creates, end hence, owns the data.
    * \remarks
    * After DALData has been created it can be attached to Device Abstraction Link hosted by a 
    * DALHost using DALHost::storeDeviceAbstractionLink.
    */
  NVSG_API DALData(DALDataCreator * creator);

  /*! \brief Induces the deletion of this DALData object.
    * \remarks
    * This function will be called from the DALServer framework when this DALData object is going 
    * to be released from a Device Abstraction Link hosted by a DALHost.
    * \n\n
    * The default implementation calls delete on this pointer. If a custom memory manager is 
    * used, in particular a memory manager that does not issue new and delete for creating and 
    * deleting a DALData object, it is recommended to override this function so that the deletion of 
    * the DALData object will be performed correctly. 
    */
  NVSG_API virtual void deleteThis() { delete this; }
  
#if !defined(DOXYGEN_IGNORE)
private:
  DALDataCreator  * m_creator;
  DALHostWeakPtr m_host;
  unsigned int      m_userID;
#endif
};

inline unsigned int DALData::getUserID() const
{
  return( m_userID );
}

inline void DALData::setUserID( unsigned int userID )
{
  m_userID = userID;
}

inline bool DALData::verifyCreator(const DALDataCreator * creator) const
{
  return creator == m_creator;
}

#if !defined(DOXYGEN_IGNORE)
// Device Abstraction Link Server - implementation details
class DALServer
{
  friend class DALDataCreator;
  friend class DALHost;

public:
  // Initializes the DALServer at instantiation
  NVSG_API DALServer();
  // Cleanup after deletion
  NVSG_API ~DALServer();
private:
  // Attaches the data identified by \a dataID and pointed to by \a data 
  // to the Device Abstraction Link (DAL) identified by \a hDAL.
  // If HDAL_INVALID is passed for \a hDAL, a new DAL will be created first.
  // \returns A handle to the Device Abstraction Link or DAL_INVALID if the function fails.
  NVSG_API HDAL storeDALData(HDAL hDAL, unsigned int dataID, DALData* data);
  // Releases the data identified by \a dataID that was attached to the DAL identified by \a hDAL 
  // by the creator pointed to by \a creator.
  // If 0xFFFFFFFF is passed for \a dataID, all data that was attached to the DAL identified by \a hDAL
  // by the creator pointed to by \a creator will be released.
  // If NULL is passed for \a creator, all data attached to the DAL identified by \a hDAL will be released
  // and the DAL will be deleted afterwards.
  NVSG_API void releaseDALData(HDAL hDAL, DALDataCreator * creator=NULL, unsigned int dataID=0xFFFFFFFF);
  // Provides all addresses of the data objects identified by \a dataID that are currently stored 
  // with the DAL identified by \a hDAL. 
  // The addresses of the data objects will be copied to the vector referenced by \a data.
  NVSG_API bool getDALData(HDAL hDAL, unsigned int dataID, std::vector<DALData*>& data) const;

  // Calls delete this on the data pointer passed 
  NVSG_API void deleteDALData(DALData * data);

private:
  std::map<HDAL, std::multimap<unsigned int, DALData*> > m_DALMap; // map of DALs
  std::vector<HDAL> m_freeHDALs; // free DAL handles can be re-used
  HDAL m_hDALNext; // use this if no handle is available for re-use

  nvutil::SWMRSync m_lock; // secure access to the singleton server in a multi-threaded environment
};

typedef nvutil::Singleton<DALServer> DALS;

#endif // DOXYGEN_IGNORE

inline void DALHostHolder::releaseDALData()
{
  NVSG_TRACE();
  nvsg::DALHostWriteLock host(m_host);
  host->releaseDeviceAbstractionLinkData();
}

inline HDAL DALHost::storeDeviceAbstractionLinkData(unsigned int dataID, DALData * data)
{
  NVSG_TRACE();
  NVSG_ASSERT(data); // do not like bad data
  NVSG_ASSERT(data->m_host==NULL); // data must not be hosted by anyone else
 
  m_hdal = DALS::instance()->storeDALData(m_hdal, dataID, data);
  if ( HDAL_INVALID != m_hdal )
  {
    data->m_host = nvutil::getWeakPtr<DALHost>(this);  
    m_recent[getCallingThread()] = data;
  }
  return m_hdal;
}

inline void DALHost::releaseDeviceAbstractionLinkData()
{
  NVSG_TRACE();

  // HDAL_INVALID means no data hosted
  if ( HDAL_INVALID != m_hdal ) 
  {
    // release all data hosted
    //
    DALS::instance()->releaseDALData(m_hdal);
    m_recent.clear();
    m_hdal = HDAL_INVALID;
  }
}

template <typename DALDataType, typename TestFunc> 
inline bool DALHost::getDeviceAbstractionLinkData(unsigned int dataID, DALDataType *& data, TestFunc test) const
{
  NVSG_TRACE();
  
  data = NULL;
  std::vector<DALData*> caches;

  if ( DALS::instance()->getDALData(m_hdal, dataID, caches) )
  {
    for ( std::vector<DALData*>::iterator i = caches.begin(); i!=caches.end(); ++i )
    {
      DALDataType * theData = dynamic_cast<DALDataType*>(*i);
      if ( theData && test(theData) )
      {
        m_recent[getCallingThread()] = theData;
        data = theData;
        return true;
      }
    }
  }
  return false;
}

inline DALData * DALHost::getLastRecentlyUsedDeviceAbstractionLinkData() const
{
  NVSG_TRACE();
  ConstRecentIter ri = m_recent.find(getCallingThread());
  if ( m_recent.end() == ri )
  {
    return NULL;
  }
  return ri->second;
}

inline void DALDataCreator::releaseDeviceAbstractionLinkData(HDAL hDAL, unsigned int dataID)
{
  NVSG_TRACE();
  NVSG_ASSERT(hDAL != HDAL_INVALID);
  DALS::instance()->releaseDALData(hDAL, this, dataID);
}

} // namespace nvsg
