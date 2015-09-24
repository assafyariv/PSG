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

#include <nvsg/PerspectiveCamera.h>
#include <nvsg/FrustumCamera.h>
#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>

#include <nvsg/SpotLight.h>
#include <nvsg/PointLight.h>
#include <nvsg/DirectedLight.h>
#include <nvmath/Vecnt.h>
#include <nvmath/Quatt.h>

#include "CameraAnimator.h"
#include "SceneFunctions.h"

#include <nvutil/DbgNew.h>

using namespace nvsg;
using namespace nvmath;
using namespace nvutil;
using namespace std;

CameraAnimator::CameraAnimator() :
   m_timerID(0)
  , m_cameraOrbit(false)
  , m_cameraOrbitAxis(0)
  , m_cameraMoveTo(false)
  , m_cameraMoveDuration(3000.0)            // two seconds per camera move
  , m_cameraIteration(false)
  , m_cameraIterationIndex(~0)
  , m_cameraIterationPauseDuration(1000.0)  // one seconds pause between camera moves
{
	m_finishCallBack = 0;
}

CameraAnimator::~CameraAnimator()
{ 
}

void
CameraAnimator::setViewState( const ViewStateSharedPtr & vsp )
{
  m_viewState = vsp;
}

void
CameraAnimator::cancel()
{
  // set everything to false
  setCameraOrbit( false );
  setCameraMoveTo( false );
  setCameraIteration( false );
}

// start the timer if needed, kill the timer if allowed
void CameraAnimator::startStopTimer( bool &currentFlag, bool newFlag, bool killIt )
{
  if ( currentFlag != newFlag )
  {
    if ( newFlag != ( m_timerID != 0 ) )   // timer needs to be started or killed
    {
      if ( newFlag )
      {
        if ( ! m_timerID )
        {
			m_timerID = 1;
			m_cameraMoveTimer.start ();
        }
      }
      else if ( killIt )
      {
        m_cameraMoveTimer.stop();
        m_timerID = 0;
      }
    }
    currentFlag = newFlag;
  }
}


double clampDeviation( float start, float end, float part, double minTime )
{
  float f = ( start <= end ) ? end / start : start / end;
  return( clamp( ((1.0 - f) / part), minTime, 1.0 ) );
}

void CameraAnimator::orbitCamera( unsigned int axisID, bool cameraRelative, float radians )
{
  ViewStateReadLock vs(getViewState());
  FrustumCameraSharedPtr fch( sharedPtr_cast<FrustumCamera>(vs->getCamera()) );
  float targetDistance = vs->getTargetDistance();
  Vec3f axis( (axisID & BIT0) ? 1.0f : 0.0f, 
              (axisID & BIT1) ? 1.0f : 0.0f, 
              (axisID & BIT2) ? 1.0f : 0.0f ); 
  normalize( axis );
  FrustumCameraWriteLock(fch)->orbit( axis, targetDistance, radians, cameraRelative );
}

double CameraAnimator::determineDurationFactor()
{
  FrustumCameraReadLock startCam( m_cameraMoveStart );
  FrustumCameraReadLock targetCam( m_cameraMoveTarget );

  // first try: if the start and end orientation differ by more than 90 degree, make the full animation
  Quatf diffQuat = startCam->getOrientation() / targetCam->getOrientation();
  Vec3f axis;
  float angle;
  decompose( diffQuat, axis, angle );
  double durationFactor = clamp( (double)(angle / PI_HALF), 0.0, 1.0 );

  // second try: if position moves about the focus distance (in fact the medium between start and end), make the full
  // animation.
  float mediumFocus = 0.5f * ( startCam->getFocusDistance() + targetCam->getFocusDistance() );
  float posDistance = nvmath::distance( startCam->getPosition(), targetCam->getPosition() );
  durationFactor = clamp( (double)(posDistance / mediumFocus), durationFactor, 1.0 );

  // third try: if near distance changes by more than 25%, make the full animation
  durationFactor = clampDeviation( startCam->getNearDistance(), targetCam->getNearDistance(), 0.25f, durationFactor );

  // fourth try: if focus distance changes by more than 25%, make the full animation
  durationFactor = clampDeviation( startCam->getFocusDistance(), targetCam->getFocusDistance(), 0.25f, durationFactor );

  // fifth try: if far distance changes by more than 25%, make the full animation
  durationFactor = clampDeviation( startCam->getFarDistance(), targetCam->getFarDistance(), 0.25f, durationFactor );

  // sixth try: if window size changes by more than 25%, make the full animation
  durationFactor = clampDeviation( startCam->getWindowSize()[0], targetCam->getWindowSize()[0], 0.25f, durationFactor );
  durationFactor = clampDeviation( startCam->getWindowSize()[1], targetCam->getWindowSize()[1], 0.25f, durationFactor );

  // ignore windowOffset, lowerLeft, upperRight for now! I don't expect them to change much.
  return( durationFactor );
}

void CameraAnimator::initCameraMove( const FrustumCameraWeakPtr & targetCam )
{
  NVSG_ASSERT( isPtrTo<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera()) );
  m_cameraMoveStart = sharedPtr_cast<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera())->clone();
  m_cameraMoveTarget = targetCam;
  NVSG_ASSERT( ObjectReadLock(m_cameraMoveStart)->getObjectCode()
            == ObjectReadLock(m_cameraMoveTarget)->getObjectCode() );
  cameraMoveDurationFactor( determineDurationFactor() );
}

void CameraAnimator::initCameraMoveToLight( const LightSourceWeakPtr & targetLight )   // should be a LightSourceWeakPtr
{
  NVSG_ASSERT( isPtrTo<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera()) );
  m_cameraMoveStart = sharedPtr_cast<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera())->clone();

  m_cameraMoveTarget = m_cameraMoveStart->clone();
  FrustumCameraWriteLock targetCam(m_cameraMoveTarget);

  LightSourceWeakPtr lsh = targetLight;
  switch( LightSourceReadLock(lsh)->getObjectCode() )
  {
    case OC_SPOTLIGHT :
      {
        SpotLightReadLock sl( static_cast<SpotLightWeakPtr>(lsh) );
        targetCam->setPosition( sl->getPosition() );
        targetCam->setOrientation( sl->getOrientation() );
      }
      break;

    case OC_POINTLIGHT :
      {
        PointLightReadLock pl( static_cast<PointLightWeakPtr>(lsh) );
        targetCam->setPosition( pl->getPosition() );

        // point us in the direction of the scene center..
        ReadableObject<nvsg::Scene> scene( ViewStateReadLock(m_viewState)->getScene() );
        if ( scene->getRootNode() )
        {
          Vec3f forward = ReadableObject<nvsg::Node>(scene->getRootNode())->getBoundingSphere().getCenter() - pl->getPosition();
          Vec3f worldup( 0.f, 1.f, 0.f ); //pc->getUpVector();

          Vec3f right = forward ^ worldup;
          Vec3f up = right ^ forward;

          normalize( forward );
          normalize( right );
          normalize( up );

          // X east, Y up, -Z north
          Mat33f lookat(   right[0],    right[1],    right[2],
                              up[0],       up[1],       up[2],
                        -forward[0], -forward[1], -forward[2] );

          Quatf ori( lookat );
          targetCam->setOrientation( ori );
        }
      }
      break;

      case OC_DIRECTEDLIGHT :
        {
          DirectedLightReadLock dl( static_cast<DirectedLightWeakPtr>(lsh) );
          targetCam->setOrientation( dl->getOrientation() );
        }
      break;
  }

  cameraMoveDurationFactor( determineDurationFactor() );
}

void CameraAnimator::initCameraZoomAll()
{
  ViewStateWriteLock vs( getViewState() );
  SceneReadLock scene( vs->getScene() );
  m_cameraMoveStart = sharedPtr_cast<PerspectiveCamera>(vs->getCamera())->clone();
  m_cameraMoveTarget = m_cameraMoveStart->clone();

  if ( scene->getRootNode() )
  {
    FrustumCameraWriteLock camera(m_cameraMoveTarget);
    const Sphere3f & bs = ReadableObject<nvsg::Node>(scene->getRootNode())->getBoundingSphere();
    camera->zoom( bs, float( AVOCADO_DEFAULT_FOV)/*float(nvmath::PI_QUARTER) */);

    vs->setTargetDistance( std::max<float>( nvmath::distance( camera->getPosition(), bs.getCenter() ), 0.1f ) );
  }

  cameraMoveDurationFactor( determineDurationFactor() );
}
void CameraAnimator::initCameraZoomToWithDirection(nvmath::Sphere3f target,nvmath::Quatf dir)
{
  ViewStateWriteLock vs( getViewState() );
 // SceneReadLock scene( vs->getScene() );
  m_cameraMoveStart = sharedPtr_cast<PerspectiveCamera>(vs->getCamera())->clone();
  m_cameraMoveTarget = m_cameraMoveStart->clone();

 // if ( scene->getRootNode() )
  {
    FrustumCameraWriteLock camera(m_cameraMoveTarget);
  //  const Sphere3f & bs = ReadableObject<nvsg::Node>(scene->getRootNode())->getBoundingSphere();
//camera->setOrientation (nvmath::Vec3f (dir[0],dir[1],dir[2]),nvmath::Vec3f (1.0f,0.0f,0.0f));
	camera->setOrientation (nvmath::Quatf (dir[0],dir[1],dir[2],dir[3]));
    camera->zoom( target,float( AVOCADO_DEFAULT_FOV)/*float(nvmath::PI_QUARTER)*/ );

    vs->setTargetDistance( std::max<float>( nvmath::distance( camera->getPosition(), target.getCenter() ), 0.1f ) );
	//camera->setDirection (nvmath::Vec3f (dir[0],dir[1],dir[2]));
  }

  cameraMoveDurationFactor( determineDurationFactor() );
}

void CameraAnimator::initCameraZoomTo(nvmath::Sphere3f target)
{
  ViewStateWriteLock vs( getViewState() );
 // SceneReadLock scene( vs->getScene() );
  m_cameraMoveStart = sharedPtr_cast<PerspectiveCamera>(vs->getCamera())->clone();
  m_cameraMoveTarget = m_cameraMoveStart->clone();

 // if ( scene->getRootNode() )
  {
    FrustumCameraWriteLock camera(m_cameraMoveTarget);
  //  const Sphere3f & bs = ReadableObject<nvsg::Node>(scene->getRootNode())->getBoundingSphere();
  
	camera->zoom( target, AVOCADO_DEFAULT_FOV /*float(nvmath::PI_QUARTER)*/ );

    vs->setTargetDistance( std::max<float>( nvmath::distance( camera->getPosition(), target.getCenter() ), 0.1f ) );
  }

  cameraMoveDurationFactor( determineDurationFactor() );
}
void CameraAnimator::moveCamera( double t )
{
  Quatf orientation;
  Vec3f position;
  float nearDistance, focusDistance, farDistance;
  Vec2f windowOffset, windowSize, lowerLeft, upperRight;
  {
    FrustumCameraReadLock startCam( m_cameraMoveStart );
    FrustumCameraReadLock targetCam( m_cameraMoveTarget );
	float ft = (float) t;
    lerp( ft, startCam->getOrientation(), targetCam->getOrientation(), orientation );
    lerp( ft, startCam->getPosition(), targetCam->getPosition(), position );
    lerp( ft, startCam->getNearDistance(), targetCam->getNearDistance(), nearDistance );
    lerp( ft, startCam->getFocusDistance(), targetCam->getFocusDistance(), focusDistance );
    lerp( ft, startCam->getFarDistance(), targetCam->getFarDistance(), farDistance );
    lerp( ft, startCam->getWindowOffset(), targetCam->getWindowOffset(), windowOffset );
    lerp( ft, startCam->getWindowSize(), targetCam->getWindowSize(), windowSize );
    const Box2f & windowRegionStart = startCam->getWindowRegion();
    const Box2f & windowRegionTarget = targetCam->getWindowRegion();
    lerp( ft, windowRegionStart.getLower(), windowRegionTarget.getLower(), lowerLeft );
    lerp( ft, windowRegionStart.getUpper(), windowRegionTarget.getUpper(), upperRight );
  }

  ViewStateReadLock viewState(getViewState());
  NVSG_ASSERT( isPtrTo<FrustumCamera>(viewState->getCamera()) );
  FrustumCameraWriteLock fch( sharedPtr_cast<FrustumCamera>(viewState->getCamera()) );
  fch->setOrientation( orientation );
  fch->setPosition( position );
  fch->setNearDistance( nearDistance );
  fch->setFocusDistance( focusDistance );
  fch->setFarDistance( farDistance );
  fch->setWindowOffset( windowOffset );
  fch->setWindowSize( windowSize );
  fch->setWindowRegion( Box2f( lowerLeft, upperRight ) );
}

void CameraAnimator::setCameraOrbit( bool co )
{
  if ( co )
  {
    // stop move-to and iteration, when orbit starts
    setCameraMoveTo( false );
    setCameraIteration( false );
  }
  startStopTimer( m_cameraOrbit, co, true );
}

void CameraAnimator::setCameraMoveTo( bool cmt )
{
  if ( cmt )
  {
    // stop orbit when move-to starts; iteration is kept unchanged
    setCameraOrbit( false );
  }
  startStopTimer( m_cameraMoveTo, cmt, ! ( m_cameraIteration ) );
  if ( cmt )
  {
    m_cameraMoveTimer.restart();
  }
  else
  {

    m_cameraMoveTimer.stop();
	if (m_finishCallBack != 0)
		m_finishCallBack->call ();

  }
}

// this slot is signaled by the VRW when it determines the distance between current cam and next cam
void CameraAnimator::cameraMoveDurationFactor( double df )
{
  if ( m_cameraMoveDurationFactor < df )
  {
    m_cameraMoveDurationFactor = df;
  }
}

void CameraAnimator::cameraOrbitX( bool tf )
{
  if( tf )
  {
    m_cameraOrbitAxis |= BIT0;
  }
  else
  {
    m_cameraOrbitAxis &= ~BIT0;
  }

  setCameraOrbit( (m_cameraOrbitAxis > 0) );
}

void CameraAnimator::cameraOrbitY( bool tf )
{
  if( tf )
  {
    m_cameraOrbitAxis |= BIT1;
  }
  else
  {
    m_cameraOrbitAxis &= ~BIT1;
  }
  setCameraOrbit( (m_cameraOrbitAxis > 0) );
}

void CameraAnimator::cameraOrbitZ( bool tf )
{
  if( tf )
  {
    m_cameraOrbitAxis |= BIT2;
  }
  else
  {
    m_cameraOrbitAxis &= ~BIT2;
  }
  setCameraOrbit( (m_cameraOrbitAxis > 0) );
}

void CameraAnimator::moveToCamera( const FrustumCameraWeakPtr & cam )
{
  m_cameraMoveDurationFactor = 0.0;
  initCameraMove( cam );
  setCameraMoveTo( true );
}

void CameraAnimator::moveToLight( const LightSourceWeakPtr & light )
{
  m_cameraMoveDurationFactor = 0.0;
  initCameraMoveToLight( light );
  setCameraMoveTo( true );
}
void CameraAnimator::zoomTo(float c1,float c2,float c3, float radius)
{
	nvmath::Sphere3f targetSphere ;
	targetSphere.setCenter(nvmath::Vec3f (c1,c2,c3));
	targetSphere.setRadius (radius);
  m_cameraMoveDurationFactor = 0.0;
  initCameraZoomTo(targetSphere);
  setCameraMoveTo( true );
}

void CameraAnimator::zoomToWithDirection(float c1,float c2,float c3, float radius,float q1, float q2,float q3 ,float q4)
{
	nvmath::Sphere3f targetSphere ;
	targetSphere.setCenter(nvmath::Vec3f (c1,c2,c3));
	targetSphere.setRadius (radius);
  m_cameraMoveDurationFactor = 0.0;
  initCameraZoomToWithDirection(targetSphere,nvmath::Quatf (q1,q2,q3,q3));
  setCameraMoveTo( true );
}
void CameraAnimator::zoomAll()
{
  m_cameraMoveDurationFactor = 0.0;
  initCameraZoomAll();
  setCameraMoveTo( true );
}

FrustumCameraWeakPtr CameraAnimator::findNextIterationCamera()
{
  SceneReadLock scene( ViewStateReadLock(getViewState())->getScene() );
  unsigned int noc = scene->getNumberOfCameras();

  Scene::CameraConstIterator scci = scene->beginCameras();
  unsigned int first = m_cameraIterationIndex++;
  if( m_cameraIterationIndex >= noc )
  {
    m_cameraIterationIndex = 0;
    if ( first >= noc )   // can happen, if the latest camera in iteration has been removed while animating!
    {
      first = noc - 1;    // just take the last camera in the scene as the search end condition
    }
  }

  std::advance( scci, m_cameraIterationIndex );

  while( m_cameraIterationIndex != first )
  {
    // check if its a frustum camera, and is not one of the ones being used in any of the viewports (userdata)
    if ( isPtrTo<FrustumCamera>( *scci ) && CameraReadLock( *scci )->getUserData() == nullptr ) 
    {
      return( sharedPtr_cast<FrustumCamera>( *scci ).get() );
    }

    ++scci;
    ++m_cameraIterationIndex;

    // start over if we have hit the end
    if( scci == scene->endCameras() )
    {
      scci = scene->beginCameras();
      m_cameraIterationIndex = 0;
    }
  }

  return nullptr;
}

void CameraAnimator::setCameraIteration( bool ci )
{
  if ( m_cameraIteration != ci )
  {
    if ( ci )
    {
      // re-use to see if we found an appropriate camera
      ci = false;

      // assure, there are at least two FrustumCameras in the scene
      m_cameraIterationIndex = 0;
      FrustumCameraWeakPtr fsp = findNextIterationCamera();

      if( fsp )
      {
        moveToCamera( fsp );
        m_cameraIteration = true;
        ci = true;
      }
    }

    // if we are turning off camera iteration, or we didn't find a camera, then turn it off
    if( !ci )
    {
      m_cameraIteration = false;
      setCameraMoveTo( false );   // also stop any running move-to
    }
  }
}

void CameraAnimator::cameraCycle( bool enable )
{
  m_cameraMoveDurationFactor = 0.0;
  setCameraIteration( enable );
}

void CameraAnimator::moveToCameraIndex( unsigned int index )
{
  SceneReadLock scene( ViewStateReadLock(getViewState())->getScene() );
  if( index < scene->getNumberOfCameras() )
  {
    Scene::CameraConstIterator scci = scene->beginCameras();
    std::advance( scci, index );
    NVSG_ASSERT( isPtrTo<FrustumCamera>( *scci ) );
    moveToCamera( sharedPtr_cast<FrustumCamera>( *scci ).get() );
  }
}

 bool CameraAnimator::timerEvent(  )
{
	 bool needRepaint = false;
  if ( m_cameraIteration && ! m_cameraMoveTo )
  {
    if ( ! m_cameraIterationTimer.isRunning() )
    {
      m_cameraIterationTimer.restart();
    }
    else
    {
      double currentTime = m_cameraIterationTimer.getTime();
      if ( m_cameraIterationPauseDuration < currentTime )
      {
        m_cameraIterationTimer.stop();

        // move-to next camera
        FrustumCameraWeakPtr fsp = findNextIterationCamera();
        if( fsp )
        {
          moveToCamera( fsp );
        }
      }
    }
  }
  else if ( m_cameraMoveTo )
  {
    NVSG_ASSERT( m_cameraMoveTimer.isRunning() );
    double currentTime = m_cameraMoveTimer.getTime();
    double t;
    double realDuration = m_cameraMoveDuration * m_cameraMoveDurationFactor;
    if ( realDuration < currentTime )
    {
      setCameraMoveTo( false );
      t = 1.0;
    }
    else
    {
      t = currentTime / realDuration;
      t = 0.5 * ( sin( -nvmath::PI_HALF + t * nvmath::PI ) + 1.0 );   // smooth-in and -out using simple sine curve
    }

    moveCamera( t );
 //   emit 
//	update();
	needRepaint = true;
  }
  else if ( m_cameraOrbit )
  {
    orbitCamera( m_cameraOrbitAxis, true, nvmath::degToRad(1.0f) );
    //emit 
//	update();
	needRepaint = true;
  }
  return needRepaint;
}

