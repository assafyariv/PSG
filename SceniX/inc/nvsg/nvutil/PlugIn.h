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

#include <map>

#include "nvsgcommon.h"
#if defined(LINUX)
 #include <dlfcn.h>
#endif
#include "nvutil/PlugInCallback.h"
#include "nvutil/SmartPtr.h"

namespace nvutil {

/*! \def HLIB
 *  Platform independent define for a library handle. On Windows, this resolves to HMODULE, on Linux to void*.*/
/*! \def MapLibrary(name)
 *  Platform independent define to open a library. On Windows, this resolves to LoadLibrary(name), on Linux to
 *  dlopen(name,RTLD_LAZY). */
/*! \def UnMapLibrary
 *  Platform independent define to free a library. On Windows, this resolves to FreeLibrary, on Linux to dlclose. */
/*! \def GetFuncAddress
 *  Platform independet define to get a function from a library. On Windows, this resolves to GetProcAddress, on
 *  Linux to dlsym. */
/*! \def LibExtStr
 *  Platform independent define for the SceniX plugin standard file extension. On both Windows and Linux, this
 *  resolves to "*.nxm". */
#if defined(_WIN32) 
# define HLIB                 HMODULE
# define MapLibrary(name)     LoadLibrary(name)
# define UnMapLibrary         FreeLibrary
# define GetFuncAddress       GetProcAddress  
# define LibExtStr            "*.nxm"
#elif defined(LINUX)
# define HLIB                 void*
# define MapLibrary(name)     dlopen(name, RTLD_LAZY)
# define UnMapLibrary(handle) dlclose(handle)
# define GetFuncAddress       dlsym  
# define LibExtStr            "*.nxm"
#else
# error Undefined Operating System!  
#endif

//! Unique Plug Interface Type ID
/** A helper class for assembling unique IDs that identify a certain plug interface type.
  * A short numeric interface type ID and a 16 byte version string are concatenated to define
  * a unique plug interface type identifier.
  *
  * \note A UPITID must be defined by a client which wants to use a certain plug interface type
  * provided by a plug-in.
  */
class UPITID
{
public:
  //! Default constructor
  /** Default constructs a UPITID object.
    */
  UPITID() {}
  //! Constructor
  /** Constructs a UPITID object from a plug interface type ID and a version string.
    */
  NVSG_API UPITID( unsigned short pit     //!< Identifies a certain plug interface type
                 , const char verStr[16]  //!< A 16 byte version string to maintain version safty
                 );
  //! Copy constructor
  /** Constructs a UPITID object from another UPITID object.
    */
  NVSG_API UPITID(const UPITID& rhs);
  //! "Less than" comparison
  /** Compares two UPITID objects.
    * This operator is required e.g. if it is intended to store UPITID objects in ordered containers.
    * \returns \c true if lhs is less than rhs, false otherwise.
    */
  friend bool operator<( const UPITID& lhs //!< Left operand 
                       , const UPITID& rhs //!< Right operand
                       );
  //! Comparison
  /** Tests two UPITID objects on equality.
    * \returns \c true if lhs is equal to rhs, false otherwise.
    */
  friend bool operator==( const UPITID& lhs //!< Left operand
                        , const UPITID& rhs //!< Right operand 
                        );
  //! Comparison
  /** Tests two UPITID objects on inequality.
    * \returns \c true if lhs is unequal to rhs, false otherwise.
    */
  friend bool operator!=( const UPITID& lhs //!< Left operand
                        , const UPITID& rhs //!< Right operand
                        ); 

private:
  unsigned short data0;      // plug interface type 
  char           data1[16];  // version control
};

//! Unique Plug Interface ID
/** A helper class for assembling unique IDs that identify a certain plug interface.
  * An 8 byte specific identifier string will be concatenated to a UPITID to define a
  * unique plug interface identifier, client code can query for.
  * \note A UPIID will be defined by a plug-in which provides a certain plug interface type.
  */
class UPIID // Unique Plug Interface ID
{
public:
  //! Default constructor
  /** Default constructs a UPIID object.
    */
  UPIID() {};
  //! Constructor
  /** Constructs a UPIID object from a specific identifier string (8 bytes) and a UPITID
    * which identifies the certain type of the interface a plug-in provides.
    */
  NVSG_API UPIID( const char idstr[8]  //!< Specific identifier string
                , UPITID pitid         //!< UPITID the plug-in provides
                ); 
  //! Assignment operator
  /** Assigning a new UPIID.
    * \returns A reference to this UPIID object.
    */
  NVSG_API UPIID& operator=(const UPIID& rhs);

  //! Receive specific identifier string
  /** Get hold of the identifier that is specific for a plug interface provided by a certain plug-in.
    * Client code can take this additional information to decide which plug-in best matches the client's
    * requirements concerning a certain plug interface type. The kind of information stored in the 
    * specific identifier string and how a certain client can use it, must be documented in detail by the 
    * plug-in writer. 
    * \par
    * For example, plug-ins that provide the \c nvsg::PITID_SCENE_LOADER or \c nvsg::PITID_SCENE_SAVER
    * interface type for either loading or saving a SceniX scene, store the specific file extension within 
    * the specific identifier string of the UPIID object the plug-in returns, if client code queries for
    * these interface types using the nvutil:queryInterfaceType interface.
    * \returns A constant string holding the specific identifier string. 
    */
  const char * getPlugSpecificIDString() const;
  //! Receive the unique interface type
  /** This is for a client to verify if the plug interface type stored in this interface ID is the same,
    * it was querying for.
    * \returns The UPITID stored in the UPIID.
    */
  const UPITID& getPlugInterfaceType()  const; 

  //! "Less than" comparison
  /** Compares two UPIID objects.
    * This operator is required e.g. if it is intended to store UPIID objects in ordered containers.
    * \returns \c true if lhs is less than rhs, false otherwise.
    */
  friend bool operator<( const UPIID& lhs //!< Left operand
                       , const UPIID& rhs //!< Right operand
                       );
  //! Comparison
  /** Tests two UPIID objects on equality.
    * \returns \c true if lhs is equal to rhs, false otherwise.
    */
  friend bool operator==( const UPIID& lhs //!< Left operand
                        , const UPIID& rhs //!< Right operand
                        ); 
  //! Comparison
  /** Tests two UPIID objects on inequality.
    * \returns \c true if lhs is unequal to rhs, false otherwise.
    */
  friend bool operator!=( const UPIID& lhs //!< Left operand
                        , const UPIID& rhs //!< Right operand
                        );

private:
  char           data0[8];   // plug specific identifier string 
  UPITID         data1;      // plug interface type
};

inline const char * UPIID::getPlugSpecificIDString() const
{
  return data0;
}
  
inline const UPITID& UPIID::getPlugInterfaceType() const
{
  return data1;
}

// this 'less' operator is required when storing UPITIDs in STL maps
inline bool operator<(const UPITID& lhs, const UPITID& rhs)
{
  return (lhs.data0==rhs.data0) ? _stricmp(lhs.data1, rhs.data1)<0 : lhs.data0<rhs.data0;
}

// test if two UPITIDs are equal
inline bool operator==(const UPITID& lhs, const UPITID& rhs) 
{ 
  return lhs.data0==rhs.data0 && !_stricmp(lhs.data1, rhs.data1); 
}

// test if two UPITIDs are unequal
inline bool operator!=(const UPITID& lhs, const UPITID& rhs) 
{ 
  return !(lhs==rhs); 
}

// this 'less' operator is required when storing UPIIDs in STL maps
inline bool operator<(const UPIID& lhs, const UPIID& rhs)
{ 
  return (lhs.data1==rhs.data1) ? _stricmp(lhs.data0, rhs.data0)<0 : lhs.data1<rhs.data1;
}

// test if two UPIIDs are equal
inline bool operator==(const UPIID& lhs, const UPIID& rhs) 
{ 
  return !_stricmp(lhs.data0, rhs.data0) && lhs.data1==rhs.data1; 
}

// test if two UPIIDs are unequal
inline bool operator!=(const UPIID& lhs, const UPIID& rhs) 
{ 
  return !(lhs==rhs); 
}

// required forward declarations
class PlugIn;

//! Function pointer type for \c getPlugInterface export function required by a plug-in  
typedef bool (*PFNGETPLUGINTERFACE)(const UPIID& piid, PlugIn *& plugIn);
//! Function pointer type for \c queryPlugInterfaceType export function required by a plug-in  
typedef bool (*PFNQUERYPLUGINTERFACETYPE)(const UPITID& pitid, std::vector<UPIID>& piids);

//! Plug-in base class
/** Serves as base class for all user defined plug-ins.
  */
class PlugIn : public RCObject
{
  //! Plug-in managment
  /** A \c PlugInServer is also responsible for releasing plug-in objects after usage
    * and therefore needs access to the private member function \c deleteThis.
    */
  friend class PlugInServer;

public:
  PlugIn();

  //! Set the callback object to use on this PlugIn.
  /** The PlugInCallback object is used to report errors and warnings that happen while using this PlugIn.  */
  void setCallback( PlugInCallback *cb                  //!<  callback object to use
                  , bool throwExceptionOnError = true   //!<  true if an exception is thrown on error
                  );

protected:
  virtual ~PlugIn();
  //! Plug-in deletion
  /** Every plug-in must provide this function. It assures that, when deleting the 
    * plug-in object, always the right heap manager is called to free the associated 
    * memory.
    * \note Ordinary client code is not allowed to call \c deleteThis on a plug-in
    * object. Client code needs to utilize the \c PlugInServer interface for plug-in
    * managment for this purposes.
    */
  virtual void deleteThis(void) = 0;

  /*! \brief Get the current callback object as a const pointer.
   *  \returns The current callback object. */
  const PlugInCallback * callback( void ) const;

  /*! \brief Get the current callback object
   *  \returns The current callback object. */
  PlugInCallback * callback( void );

private:
  nvutil::SmartPtr<PlugInCallback> m_cb;
};

inline  PlugIn::PlugIn()
: m_cb(NULL)
{
}

inline  void  PlugIn::setCallback( PlugInCallback *cb, bool throwExceptionOnError )
{
  m_cb = cb;
  if ( cb )
  {
    m_cb->setThrowExceptionOnError( throwExceptionOnError );
  }
}

inline  const PlugInCallback * PlugIn::callback( void ) const
{
  return( m_cb.get() );
}

inline  PlugInCallback * PlugIn::callback( void )
{
  return( m_cb.get() );
}

inline  PlugIn::~PlugIn()
{
}

  //! Get hold of a certain interface object
  /** Searches the \a searchPath in addition to plug-in search-paths for a plug-in that provides an 
    * interface specified by \a piid. If successfull, \a plugIn holds the requested interface object.
    *
    * While searching for appropriate plug-in modules, this function uses either the PlugInServer's
    * default file name filter or a custom file name filter set by a call to nvutil::setPlugInFileFilter.
    * The default file name filter for plug-in modules is "*.nxm" on Microsoft(R) Windows platforms as
    * well as on Linux platforms.
    *
    * The function always returns the first matching interface found in the specified search path in 
    * consideration of the used file name filter.   
    *
    * If client code asks for a certain interface a second time, without releasing the interface object
    * got with the first call to getInterface, this function will not search through the search path a 
    * second time but returning the interface object registered with the first call. This will also work
    * this way, independent of whether the file name filter was changed in between.
    *
    * \remarks 
    * If client code has no concrete interface ID available to query for, it should first query for
    * a superior interface type using \c nvutil::queryInterfaceType to get hold of a list of
    * interfaces all providing the superior interface type.
    * \par
    * In order to utilize the concrete interface queried for, client code must 'reinterpret_cast'
    * \a plugIn to a concrete interface pointer and use this pointer to access the interface.
    * \par
    * To avoid resource leaks, client code should release the interface through a call to 
    * \c nvutil::releaseInterface, after usage.
    *
    * \returns \c true if successfull, false otherwise.
    */
  NVSG_API bool getInterface( 
    const std::vector<std::string>& searchPath //!< The optional path to search for the plug-ins.
  , const UPIID& piid                //!< Identifies the interface to search for.
  , PlugIn *& plugIn                 //!< Holds the interface object (plug-in), if successfull.
  );

  //! Query for a certain interface type
  /** Searches the \a searchPath in addition to plug-in search paths for all plug-ins that provide a certain
    * interface type specified by \a pitid. If successful, client code will receive the concrete interface
    * IDs in \a piids. Each of these IDs can than be used along with \a nvutil::getInterface, to get an 
    * interface object.
    *
    * While searching for appropriate plug-in modules, this function uses either the PlugInServer's
    * default file name filter or a custom file name filter set by a call to nvutil::setPlugInFileFilter.
    * The default file name filter for plug-in modules is "*.nxm" on Microsoft(R) Windows platforms as
    * well as on Linux platforms.
    *
    * \returns true if successful, false otherwise.
    */
  NVSG_API bool queryInterfaceType( 
    const std::vector<std::string>& searchPath //!< The optional path to search for plug-ins.
  , const UPITID& pitid              //!< Identifies the interface type to query for.
  , std::vector<UPIID>& piids             //!< Holds all interface IDs found, if successfull.
  );

  //! Release a certain interface
  /** Frees all resources acquired by the interface object identified by \a piid. 
    */
  NVSG_API void releaseInterface( 
    const UPIID& piid //!< Identifies the interface to be released.
  );

  //! Specify a file name filter to be used while searching for appropriate plug-in modules.
  /** This function overrides the PlugInServer's default file name filter used to search for appropriate
    * plug-in modules. The function lets you specify different file name patterns. The patterns must be
    * separated by a semicolon. */
  NVSG_API void setPlugInFileFilter(
    const std::string& filter //!< String of file name filter patterns all delimited by a semicolon. 
  );

  //! Add a path to the list of directories used to search for PlugIn DLLs
  /** This function adds a directory to the internal search list.
    * The list is used internally when SceniX looks for Texture Loaders and it can be queried
    * by an application through getPlugInSearchPath() to be used as a search path
    * when querying SceneLoader PlugIns.
      It will always contain the current instance's path (module path), which is added
      during initialization. */  
  NVSG_API void addPlugInSearchPath(
    const std::string& path //!< A directory to be added to the search path.
    );

  //! Return the list of directories used to search for PlugIn DLLs
  /** See comments for addPlugInSearchPath() */
  NVSG_API const std::vector<std::string>& getPlugInSearchPath();

//! Plug-in management
/** The \c PlugInServer class provides functionality for convenient plug-in managment.
  * \par
  * The \c PlugInServer is a central management tool to handle plug-ins and therefore 
  * it is implemented as a singleton object. The interface of the \c PlugInServer 
  * can only be accesses through an external set of wrapper functions with special
  * access rights.
  */
class PlugInServer
{
public:
  friend NVSG_API bool getInterface( const std::vector<std::string> & searchPath, const UPIID & piid, PlugIn *& plugIn );
  friend NVSG_API bool queryInterfaceType( const std::vector<std::string> & searchPath, const UPITID & pitid, std::vector<UPIID> & piids );
  friend NVSG_API void releaseInterface( const UPIID & piid );
  friend NVSG_API void setPlugInFileFilter( const std::string & filter );
  friend NVSG_API void addPlugInSearchPath( const std::string & path );
  friend NVSG_API const std::vector<std::string>& getPlugInSearchPath();

  PlugInServer();
  ~PlugInServer();

private:
  // hidden interface
  bool getInterfaceImpl(const std::vector<std::string>& searchPath, const UPIID& piid, PlugIn *& plugIn);
  bool queryInterfaceTypeImpl(const std::vector<std::string>& searchPath, const UPITID& pitid, std::vector<UPIID>& piids);
  void releaseInterfaceImpl(const UPIID& piid);
  void setFileFilterImpl(const std::string& filter);

private:
  bool findPlugIns(const std::vector<std::string>& searchPath, std::vector<std::string>& plugIns);
  bool registerPlugIn(const std::string& fileName, const UPIID& piid, PlugIn *& plugIn);
  typedef std::map< UPIID, SmartPtr<PlugIn> > PlugInMap;
  PlugInMap m_plugIns;
  std::string m_filter;
  std::vector<std::string> m_searchPath;  // the search path for PlugIn DLLs
};

//! Type define for the application unique PlugInServer.
typedef Singleton<PlugInServer> PIS;

} // namespace nvutil
