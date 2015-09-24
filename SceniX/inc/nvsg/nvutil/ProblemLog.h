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

#include "nvsg/nvsgapi.h"  // storage-class defines
#include <string>
#include <list>

namespace nvutil 
{
  //! Class that holds the problem information
  /** This class holds the problem information that is logged in the
    * ProblemLog class.*/
  class ProblemInfo 
  {
  public:
    //! Constructor to construct a valid problem info.
    ProblemInfo( const char * problem  //!< Catchword, like the name of a missing extension
               , bool isCritical       //!< Yes means critical problem
               , const char * missingFeatures //!< Longer description on what feature(s) will be missing
               ) 
      : m_problem(problem), m_isCritical(isCritical), m_missingFeatures(missingFeatures) {}
    ~ProblemInfo() {}

    std::string m_problem;          //!< catchword
    bool        m_isCritical;       //!< severity
    std::string m_missingFeatures;  //!< explanation
  };

  /*! \brief Functor class to order ProblemInfo objects. */
  class LogListCompare
  {
    public:
      /*! \brief Function call operator of this functor
       *  \param infoA Reference to the first ProblemInfo.
       *  \param infoB Reference to the second ProblemInfo.
       *  \return \c true, if \c infoA is 'less' then \c infoB, otherwise \c false.
       *  \remark A ProblemInfo is considered to be less than an other, if the problem string is less. */
      bool operator()( const ProblemInfo & infoA, const ProblemInfo & infoB ) const;
  };

  typedef std::list<ProblemInfo> ProblemLogList;    //!< Structure of the problem list

  /*! \brief Class to log problems */
  class ProblemLog
  {
    public:
      //!< Add a new problem to the problems log
      void addProblem( const ProblemInfo & info  //!< Valid Problem information
                     );

      //!< Get the problem log
      /** Use the returned log to analyse problems and missing features, ...
        * \return Problem log list, if there is no problem the log list does not contain entries.*/
      const ProblemLogList & getLogList();
      
    private:
      ProblemLogList m_log;
  };

  //////////////////////////////////////////////////////////////////////////
  
  inline void ProblemLog::addProblem(const ProblemInfo & info)
  {
    m_log.push_back(info);
  }

  inline const ProblemLogList & ProblemLog::getLogList()
  { 
    //m_log.sort(LogListCompare());
    return m_log; 
  }

  inline bool LogListCompare::operator()( const ProblemInfo & infoA, const ProblemInfo & infoB ) const
  {
      return(infoA.m_problem < infoB.m_problem);
  }

  /*! \brief Helper function to add an error message to a ProblemLog.
   *  \param log The ProblemLog to add the error message.
   *  \param problem The string classifying the error.
   *  \param missingFeature A more detailed description of the error. */
  inline void reportError(ProblemLog& log, const char * problem, const char * missingFeature)
  {
    log.addProblem(ProblemInfo(problem, true, missingFeature));
  }

  /*! \brief Helper function to add a warning message to a ProblemLog.
   *  \param log The ProblemLog to add the warning message.
   *  \param problem The string classifying the warning.
   *  \param missingFeature A more detailed description of the warning. */
  inline void reportWarning(ProblemLog& log, const char * problem, const char * missingFeature)
  {
    log.addProblem(ProblemInfo(problem, false, missingFeature));
  }
}

