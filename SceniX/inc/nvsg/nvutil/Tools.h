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
#include "nvsg/CoreTypes.h"
#include <vector>
#include <string>

#include <stdio.h>

namespace nvutil
{ 

  //! Find files matching a pattern
  /** Tries to find files matching \a mask and output results (full path)
    * to \a results
    * \par
    * The function tries to lookup the file as follows:
    * -# If mask contains a drive or path, that path will be looked for first
    * -# If it does not contain a path and searchPaths is empty we look inside
    *    current dir for a match
    * -# Look inside the search paths stored in \a searchPaths
    * -# Dive into all the subdirs if \a recursive is set true 
    * 
    * Note: Use FindFileFirst, if you are only interested in the first occurence
    *       of a file.
    *         
    * \returns true if successfull, false otherwise.
    */
  extern NVSG_API bool FindFile( const std::string &mask                       //!< the path/file mask to match
                               , const std::vector<std::string> &searchPaths   //!< the dirs to search additionally
                               , std::vector<std::string> &results             //!< store results in here
                               , bool recursive=false                          //!< search all subdirs also
                               );           
    
  //! Check for the first occurence of a file    
  /** Works as FindFile, but return the first match only to \a nameOut
    */
  extern NVSG_API bool FindFileFirst( const std::string &mask                     //!< Name of the file to search for.
                                    , const std::vector<std::string> &searchPaths //!< Collection of search paths to lookup the file.
                                    , std::string &nameOut                        //!< Holds the full path and filename, if successful.
                                    , bool recursive=false                        //!< search all subdirs also
                                    );

  //! Check if a given directory exists
  extern NVSG_API bool DirExists( const std::string &dirpath  //<! Input (path)
                                 );

  //! Create a directory
  extern NVSG_API bool CreateDir( const std::string &dirpath );

  //! Check if a given file exists
  extern NVSG_API bool FileExists( const std::string &filepath  //<! Input (path+)filename.
                                 );

  //! Return filesize in bytes of an existing file. Supports filesizes up to 2GB only.
  extern NVSG_API bool FileSize( const std::string &filepath,  //<! Input (path+)filename
                                 long &size                    //<! Filesize if file exists (function return true)
                               );

  //! Delete a file
  extern NVSG_API bool FileDelete( const std::string &filepath );

  //! Extract filename
  /** Extracts the filename with extension from \a path and returns it in \a fname.
    */
  extern NVSG_API void GetFileNameFromPath( const std::string& path //!< Input path and filename.
                                          , std::string& fname      //!< Extracted filename with extension.
                                          ); 
  //! Extract file extension
  /** Extracts the file extension, including leading period from \a path and returns it in \a fext.
    */
  extern NVSG_API void GetFileExtFromPath( const std::string& path //!< Input path and filename.
                                         , std::string& fext       //!< Extracted file extension, including leading period.
                                         );
  //! Extract directory path
  /** Extracts the directory path, including trailing slash from \a path and returns it in \a dir.
    */
  extern NVSG_API void GetDirFromPath( const std::string& path //!< Input path and filename.
                                     , std::string& dir        //!< Extracted directory path, including trailing slash.
                                     );
  //! Extract drive letter
  /** Extracts the optional drive letter, followed by a colon from \a path and returns it in \a drv.
    */
  extern NVSG_API void GetDrvFromPath( const std::string& path //!< Input path and filename.
                                     , std::string& drv        //!< Extracted drive letter, followed by a colon.
                                     );
  //! Extract full path
  /** Extracts the drive and directory from \a path and returns it in \a dir.
    */
  extern NVSG_API void GetDrvAndDirFromPath(const std::string& path //!< Input path and filename.
                                           , std::string& dir             //!< Extracted path, including trailing slash.
                                           );

  /*! \brief Break a path name into components.
   *  \param path The full path.
   *  \param drv String to get the drive letter, followed by a colon (:).
   *  \param dir String to get the directory path.
   *  \param fname String to get the base filename without extension.
   *  \param fext String to get the filename extension, including leading period (.). */
  extern NVSG_API void SplitPath( const std::string& path
                                , std::string& drv
                                , std::string& dir
                                , std::string& fname
                                , std::string& fext );

#if defined(_WIN32)
  //! Determine the module path **Windows only**
  /** Determines the full path to the module, given the module's name in \a module and returns it in \a path.
    */
  extern NVSG_API void GetModulePath( const std::string& module //!< Module name.
                                    , std::string& path         //!< Holds the full path to the module if the module was found.
                                    );
  //! Determine the module path **Windows only**
  /** Determines the full path of the module, given the module handle in \a hModule and returns it in \a path.
    */
  extern NVSG_API void GetModulePath( const HMODULE hModule //!< Module handle.
                                    , std::string& path          //!< Holds the full path to the module.
                                    );
#endif 

  //! Determine the executable module path **Cross Platform**
  /** Determines the full path to the current executable and returns it in \a path.
    */
  extern NVSG_API void GetModulePath( std::string& path );   //!< Holds the full executable path.

  //! Get the current directory for the current process
  /** \a path will hold the current directory if the function succeeds
    * \returns \c true if successful, false otherwise.
    */
  extern NVSG_API bool GetCurrentDir( std::string& path //!< Holds the current directory if the function succeeds.
                                    );

  //! Set the current directory for the current process
  /** Sets the current directory for the current process to \a path and returns the former current directory.
    * \returns A string holding the former current directory.
    */
  extern NVSG_API std::string SetCurrentDir( const std::string& dir //!< Path to be set as current directory.
                                      );
  //! Separate a string into tokens
  /** Consecutive calls to getNextToken() return the tokens of \a inputString
      that are separated by one or multiple occurences of characters from \a delimString.
      If no more tokens are left, an empty string will be returned and hasMoreTokens() will return false.
    */
  class StrTokenizer
  {
    public:
      /*! \brief Create a StrTokenizer with a specified delimiter string.
       *  \param delimString String holding the delimiter characters. */
      explicit NVSG_API StrTokenizer( const std::string& delimString );

      /*! \brief Destructor of a StrTokenizer. */
      NVSG_API ~StrTokenizer();

      /*! \brief Set the string to tokenize
       *  \param inputString The string to tokenize. */
      NVSG_API void setInput( const std::string& inputString );

      /*! \brief Get the next token out of the string to tokenize.
       *  \return A string with the next token in the input string. */
      NVSG_API const std::string& getNextToken();

      /*! \brief Check if the input string is completely tokenized.
       *  \return \c true, if there are more tokens in the input string, otherwise \c false. */
      NVSG_API bool hasMoreTokens() const;

      /*! \brief finds the next delimiter from startIndex (outside of quotes)
       *  \return the position of the next quoted delimiter. */
      NVSG_API size_t findDelimiter( const std::string &delim, size_t startIndex = 0 ) const;

    private:
      unsigned int m_currentPos;
      std::string m_inputString;
      std::string m_token;
      std::string m_delim;
  };

  //! Sets a temporary locale for the lifetime of the object
  class TempLocale
  {
  public:
    /*! \brief Create a scoped locale information.
     *  \param tempLocale String containing the name of a C locale.
     *  \remarks The available locales are system specific, but at least the two locales "C" and "" must exist. */
    explicit NVSG_API TempLocale( std::string tempLocale );

    /*! \brief Destructor of a TempLocale. */
    NVSG_API ~TempLocale();

  private:
    std::string oldLocale;
  };

  //! Sets or clears a collection of object hints recursively.
  /** Sets the object hints \a setHints and clears the hints \a clearHints recursively, from \a root downward.
    */
  extern NVSG_API void ApplyNodeHints( const nvsg::NodeSharedPtr & root, unsigned int setHints, unsigned int clearHints = 0 );


} // namespace nvutil


