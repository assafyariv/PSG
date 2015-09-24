// Copyright NVIDIA Corporation 2009-2010
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

#include <vector>


#include <nvsg/CoreTypes.h>
#include <nvutil/Timer.h>
class  CameraAnimatorFinishCallBack
{
public:
	virtual void call() = 0;
};
class CameraAnimator 
{

public:
  CameraAnimator();
  ~CameraAnimator();

  void setViewState( const nvsg::ViewStateSharedPtr & vsp );
  void moveToCamera( const nvsg::FrustumCameraWeakPtr & cam );
  void moveToLight( const nvsg::LightSourceWeakPtr & light );
  void zoomAll();
  void zoomTo(float c1,float c2,float c3, float radius);
  void zoomToWithDirection(float c1,float c2,float c3, float radius,float q1, float q2,float q3 ,float q4);
  void cameraOrbitX( bool );
  void cameraOrbitY( bool );
  void cameraOrbitZ( bool );
  void cameraCycle( bool );
  bool isCameraOrbitX();
  bool isCameraOrbitY();
  bool isCameraOrbitZ();
  bool isCameraCycle();

public:
  void cancel();
  void moveToCameraIndex( unsigned int index );
  CameraAnimatorFinishCallBack* m_finishCallBack;
//signals:
  //void update();

protected:
  nvsg::FrustumCameraWeakPtr findNextIterationCamera();
  void setCameraOrbit( bool cr );
  void setCameraMoveTo( bool cr );
  void setCameraIteration( bool cr );
  void orbitCamera( unsigned int axisID, bool cameraRelative, float radians );
  void initCameraMove( const nvsg::FrustumCameraWeakPtr & targetCam );
  void initCameraMoveToLight( const nvsg::LightSourceWeakPtr & targetLight );
  void initCameraZoomAll();
  void initCameraZoomToWithDirection(nvmath::Sphere3f target,nvmath::Quatf dir);
  void initCameraZoomTo(nvmath::Sphere3f target);
  void moveCamera( double t );

  void cameraMoveDurationFactor(double);
  void startStopTimer( bool &currentOpt, bool newOpt, bool dontStop );
  double determineDurationFactor();

  const nvsg::ViewStateSharedPtr & getViewState();
public:
 // virtual bool eventFilter( QObject * obj, QEvent * event );
  virtual bool timerEvent();

protected:
  nvsg::FrustumCameraSharedPtr  m_cameraMoveStart;
  nvsg::FrustumCameraSharedPtr  m_cameraMoveTarget;
  nvsg::ViewStateSharedPtr      m_viewState;

  int           m_timerID;

  bool          m_cameraOrbit;
  unsigned int  m_cameraOrbitAxis;
  bool          m_cameraMoveTo;
  nvutil::Timer m_cameraMoveTimer;
  double        m_cameraMoveDuration;
  double        m_cameraMoveDurationFactor;
  bool          m_cameraIteration;
  unsigned int  m_cameraIterationIndex;
  nvutil::Timer m_cameraIterationTimer;
  double        m_cameraIterationPauseDuration;
};

inline const nvsg::ViewStateSharedPtr & 
CameraAnimator::getViewState()
{
  return m_viewState;
}

inline bool 
CameraAnimator::isCameraOrbitX()
{
  return m_cameraOrbit && (m_cameraOrbitAxis & BIT0);
}

inline bool 
CameraAnimator::isCameraOrbitY()
{
  return m_cameraOrbit && (m_cameraOrbitAxis & BIT1);
}

inline bool 
CameraAnimator::isCameraOrbitZ()
{
  return m_cameraOrbit && (m_cameraOrbitAxis & BIT2);
}

inline bool 
CameraAnimator::isCameraCycle()
{
  return m_cameraIteration;
}

