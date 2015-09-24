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

#ifndef LINUX
  #include <Windows.h>
#else
  #include <sys/time.h>
#endif

#include "nvsg/nvsgapi.h"

namespace nvutil
{
  /*! \brief A simple timer class.
   * This timer class can be used on Windows and Linux systems to
   * measure time intervals in milliseconds. 
   * The timer can be started and stopped several times and accumulates
   * time elapsed between the start() and stop() calls. */
  class Timer
  {
  public:
    //! Default constructor. Constructs a Timer, but does not start it yet. 
    NVSG_API Timer();

    //! Default destructor.
    NVSG_API ~Timer();

    //! Starts the timer.
    NVSG_API void start();

    //! Stops the timer.
    NVSG_API void stop();

    //! Resets the timer.
    NVSG_API void reset();

    //! Resets the timer and starts it.
    NVSG_API void restart();

    //! Returns the current time.
    NVSG_API double getTime() const;

    //! Return whether the timer is still running.
    NVSG_API bool isRunning() const { return m_running; }

  private:
#ifndef LINUX
    typedef LARGE_INTEGER Time;
#else
    typedef timeval Time;
#endif

  private:
    double calcDuration(Time begin, Time end) const;

  private:
#ifndef LINUX
    LARGE_INTEGER m_freq;
#endif
    Time m_begin;
    bool m_running;
    double m_milliSeconds;
  };

}
