// Copyright NVIDIA Corporation 2002-2011
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

#include <typeinfo>
#include "nvsgcommon.h" // commonly used stuff
#include "nvmath/Boxnt.h"
#include "nvmath/Spherent.h"
#include "nvsg/Callback.h"
#include "nvsg/DAL.h" // base class definition
#include "nvutil/HandledObject.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/Incarnation.h"
#include "nvutil/ReadableObject.h"
#include "nvutil/SceniXIterator.h"
#include "nvutil/SWMRSync.h"


namespace nvsg
{
  /*! \brief 64-bit save DataID.
    * \remarks
    * The DataID is used to uniquely identify an Object's embedded data. 
    * \sa Object::getDataID
    */
  typedef unsigned long long DataID;

  /*! \brief Object Codes for SceniX Object Type Identification 
    * \remarks
    * For a custom traverser it is essential to identify all actual scene graph components while 
    * traversing the scene graph, and to call the corresponding handler routine for a particular 
    * object. For this reason, all concrete SceniX object types must be assigned to a unique object 
    * code. Secure object codes for custom defined SceniX objects start at OC_CUSTOMOBJECT.
    * \note Choosing an object code lower than OC_CUSTOMOBJECT for a custom-defined SceniX object 
    * results in undefined runtime behavior!
    * \sa \ref howtoderiveatraverser \ref howtoderiveanvsgobject 
    */
  enum
  {
    OC_INVALID = 0xFFFFFFFF //!< invalid object code
  , OC_OBJECT = 0           //!< Object
  // nodes and node components (0x00-0x1F)
  , OC_NODE = OC_OBJECT     //!< Node = 0x00 
  , OC_GEONODE              //!< GeoNode
  , OC_GROUP                //!< Group
  , OC_FLIPBOOKANIMATION    //!< FlipbookAnimation
  , OC_LOD                  //!< LOD
  , OC_SWITCH               //!< Switch
  , OC_TRANSFORM            //!< Transform
  , OC_ANIMATEDTRANSFORM    //!< AnimatedTransform
  , OC_DIRECTEDLIGHT        //!< DirectedLight
  , OC_POINTLIGHT           //!< PointLight
  , OC_SPOTLIGHT            //!< SpotLight
  , OC_BILLBOARD            //!< Billboard
  , OC_CLIPPLANE            //!< ClipPlane
  // drawables (0x20-0x3F)
  , OC_DRAWABLE = 0x20      //!< Drawable = 0x20
  , OC_VERTEX_ATTRIBUTE_SET //!< VertexAttributeSet
  , OC_ANIMATED_VERTEX_ATTRIBUTE_SET  //!< AnimatedVertexAttributeSet
  , OC_QUADS                //!< DEPRECATED Quads
  , OC_QUADSTRIPS           //!< DEPRECATED QuadStrips
  , OC_TRIANGLES            //!< DEPRECATED Triangles
  , OC_TRISTRIPS            //!< DEPRECATED TriStrips
  , OC_LINES                //!< DEPRECATED Lines
  , OC_LINESTRIPS           //!< DEPRECATED LineStrips
  , OC_TRIFANS              //!< DEPRECATED TriFans
  , OC_POINTS               //!< DEPRECATED Points
  , OC_SHAPE                //!< Shape
  , OC_BITMAP               //!< Bitmap
  , OC_PIXMAP               //!< Pixmap
  , OC_MAPELEMENT           //!< MapElement
  , OC_MAPARRAY             //!< MapArray
  , OC_MAPMATRIX            //!< MapMatrix
  , OC_MAPLIST              //!< MapList
  , OC_QUADMESHES           //!< QuadMeshes
  , OC_SKIN                 //!< Skin
  , OC_PATCHES              //!< DEPRECATED Patches
  , OC_RECT_PATCHES         //!< RectPatches
  , OC_QUAD_PATCHES         //!< QuadPatches
  , OC_QUAD_PATCHES_4X4     //!< QuadPatches4x4
  , OC_TRI_PATCHES          //!< TriPatches
  , OC_TRI_PATCHES_4        //!< TriPatches4
  , OC_PRIMITIVE            //!< Generic Primitive
  // states (0x40-0x5F)
  , OC_STATESET = 0x40      //!< StateSet = 0x40
  , OC_STATEVARIANT         //!< StateVariant
  , OC_STATEPASS            //!< StatePass
  , OC_STATEATTRIBUTE       //!< StateAttribute
  , OC_CGFX                 //!< CgFX
  , OC_RTFX                 //!< RTFx
  , OC_RTFX_PROGRAM         //!< RTFx Program
  , OC_RTFX_SCENE_ATTRIBUTE //!< RTFxSceneAttribute
  , OC_FACEATTRIBUTE        //!< FaceAttribute
  , OC_MATERIAL             //!< Material
  , OC_TEXTUREATTRIBUTE     //!< TextureAttribute
  , OC_TEXTUREATTRIBUTEITEM //!< TextureAttributeItem
  , OC_STATESETEXT          //!< StateSet for default and override state set
  , OC_LINEATTRIBUTE        //!< LineAttribute
  , OC_LIGHTINGATTRIBUTE    //!< LightingAttribute
  , OC_POINTATTRIBUTE       //!< PointAttribute
  , OC_BLENDATTRIBUTE       //!< BlendAttribute
  , OC_ALPHATESTATTRIBUTE   //!< AlphaTestAttribute
  , OC_DEPTHATTRIBUTE       //!< DepthAttribute
  , OC_UNLITCOLORATTRIBUTE  //!< UnlitColorAttribute
  , OC_STENCILATTRIBUTE     //!< StencilAttibute
  , OC_RTBUFFERATTRIBUTE    //!< Ray Tracing Buffer Attribute
  // animations (0x60-0x7F) 
  , OC_ANIMATION = 0x60                       //!< Animation = 0x60
  , OC_INDEXANIMATION                         //!< IndexAnimation
  , OC_TRAFOANIMATION                         //!< TrafoAnimation
  , OC_VERTEXATTRIBUTEANIMATION               //!< VertexAttributeAnimation
  , OC_FRAMEDINDEXANIMATIONDESCRIPTION        //!< FramedIndexAnimationDescription
  , OC_FRAMEDTRAFOANIMATIONDESCRIPTION        //!< FramedTrafoAnimationDescription
  , OC_LINEARINTERPOLATEDTRAFOANIMATIONDESCRIPTION  //!< LinearInterpolatedTrafoAnimationDescription
  , OC_FRAMEDVERTEXATTRIBUTEANIMATIONDESCRIPTION    //!< FramedVertexAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDVERTEXATTRIBUTEANIMATIONDESCRIPTION  //!< LinearInterpolatedVertexAttributeAnimationDescription

  , OC_FLOATANIMATION                                         //!< FloatAnimation
  , OC_VEC2FANIMATION                                         //!< Vec2fAnimation
  , OC_VEC3FANIMATION                                         //!< Vec3fAnimation
  , OC_VEC4FANIMATION                                         //!< Vec4fAnimation
  , OC_MAT33FANIMATION                                        //!< Mat33fAnimation
  , OC_MAT44FANIMATION                                        //!< Mat44fAnimation
  , OC_QUATFANIMATION                                         //!< QuatffAnimation
  
  , OC_LINEARINTERPOLATEDFLOATANIMATIONDESCRIPTION           //!< LinearInterpolatedFloatAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDVEC2FANIMATIONDESCRIPTION            //!< LinearInterpolatedVec2fAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDVEC3FANIMATIONDESCRIPTION            //!< LinearInterpolatedVec3fAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDVEC4FANIMATIONDESCRIPTION            //!< LinearInterpolatedVec4fAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDMAT33FANIMATIONDESCRIPTION           //!< LinearInterpolatedMat33fAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDMAT44FANIMATIONDESCRIPTION           //!< LinearInterpolatedMat44fAttributeAnimationDescription
  , OC_LINEARINTERPOLATEDQUATFANIMATIONDESCRIPTION            //!< LinearInterpolatedQuatfAttributeAnimationDescription

  // cameras (0x80-0x9F)  
  , OC_CAMERA = 0x80                          //!< Camera = 0x80              
  , OC_PARALLELCAMERA                         //!< ParallelCamera
  , OC_PERSPECTIVECAMERA                      //!< PerspectiveCamera
  , OC_MATRIXCAMERA                           //!< Matrix Camera
  // custom (0xC0 and higher)
  , OC_CUSTOMOBJECT = 0xC0                    //!< CustomObjects = 0xC0
  };

  /*! \brief Serves as base class for all traversable objects.
    * \par Namespace: nvsg
    * \remarks
    * Besides being reference counted, providing the read/write locking interface, and being host for
    * DAL data, an Object has an object code that identifies its concrete class type. Every concrete
    * class derived from Object has to set an unique object code in its constructors. This object code
    * is used in the Traverser to determine the concrete type of the Object. If there is a handler
    * function registered for that specific object code, this handler is called. Otherwise, a higher
    * level object code is queried, that also has to be provided by the concrete classes derived from
    * Object, until an object code is encountered a handler function is registered for. */
  class Object : public nvutil::HandledObject
  {
    public:

  /*! The Object hints are available in ray tracing programs by using 
   * rtDeclareVariable(int, nvsgHints, ,); inside an RTFx program.
   * The nvsgHints are put at GeometryInstance level and are accessible 
   * like materials.
   * The built-in ray tracing programs evaluate these hints:
   * NVSG_HINT_ALWAYS_INVISIBLE,
   * NVSG_HINT_NO_SHADOW_CASTER, NVSG_HINT_NO_SHADOW_RECEIVER,
   * NVSG_HINT_INVISIBLE_TO_REFLECTIONS, NVSG_HINT_INVISIBLE_TO_REFRACTIONS.
   * To toggle the rendering of any ray traced geometry change the 
   * NVSG_HINT_ALWAYS_INVISIBLE hint at a Group or GeoNode.
   * To render geometry like manipulator gizmos without being seen by any
   * secondary ray, set all these hints: 
   * NVSG_HINT_NO_SHADOW_CASTER | NVSG_HINT_NO_SHADOW_RECEIVER |
   * NVSG_HINT_INVISIBLE_TO_REFLECTIONS | NVSG_HINT_INVISIBLE_TO_REFRACTIONS
   */
    enum
    {
      /*! \brief Flags the Object as always visible
       * \remarks A visible-flagged Object will not be considered by the
       * frameworks cull step. By default, this hint is not set. */
      NVSG_HINT_ALWAYS_VISIBLE                    = BIT0,    // always visible

      /*! \brief Flags the Object as always invisible
       * \remarks An invisible-flagged Object will not be processed by the
       * render framework. The built-in ray tracing programs will ignore 
       * intersections with geometry with this flag set.
       * By default, this hint is not set. */
      NVSG_HINT_ALWAYS_INVISIBLE                  = BIT1,    // never visible
      NVSG_HINT_VISIBILITY_MASK = NVSG_HINT_ALWAYS_VISIBLE | NVSG_HINT_ALWAYS_INVISIBLE,

      /*! \brief Flags the Object not to cast shadows
       * \remarks Shadow algorithms can consider this hint for optimized 
       * strategy.
       * The built-in ray tracing programs will ignore intersections 
       * with geometry with this hint set in the any_hit_shadow program. */
      NVSG_HINT_NO_SHADOW_CASTER                  = BIT2,
 
      /*! \brief Flags the Object not to receive shadows
       * \remarks Shadow algorithms can consider this hint for optimized 
       * strategy. The built-in ray tracing programs will not shoot
       * shadow rays from geometry with this hint set.
       * By default, this hint is not set. */
      NVSG_HINT_NO_SHADOW_RECEIVER                = BIT3,

      /*! \brief Flags the Object as overlay Object
       * \remarks An overlay Object will always be rendered on top of other
       * non-overlay Objects independent of the Objects z-depth. By default,
       * this hint is not set. */
      NVSG_HINT_OVERLAY                           = BIT4,

      /*! \brief Flags the Object as frequently altering
       * \remarks The processing framework considers dynamic-flagged objects
       * different from ordinary non-dynamic Objects to yield optimized render 
       * performance. By default, an Object is not flagged dynamic. */
      NVSG_HINT_DYNAMIC                           = BIT5,

      /*! \brief Flags the Object as not being considered for clipping
       * \remarks During rendering, active clip planes have no effect on Objects 
       * that have the NVSG_HINT_DONT_CLIP bit set. This hint only applies to 
       * Node-derived Objects. */
      NVSG_HINT_DONT_CLIP                         = BIT6,

      /*!  \brief Advises the ray tracing framework to use high quality AS builder
       *  \remarks The flag advises the ray tracing engine to use high quality AS builder for
       *  this particular object. Higher quality acceleration structures potentially take longer
       *  to be built and probably require more memory, but often support considerably faster ray
       *  tracing than lower quality acceleration structures. */
      NVSG_HINT_HIGH_QUALITY_ACCELERATION_BUILDER = BIT7,

      /*! \brief Flags the Object to be invisble in ray traced reflections
       * \remarks The built-in ray tracing programs will ignore geometry
       * with this hint set in reflection rays.
       * By default, this hint is not set. */
      NVSG_HINT_INVISIBLE_TO_REFLECTIONS          = BIT8,
 
      /*! \brief Flags the Object to be invisible in ray traced refractions
       * \remarks The built-in ray tracing programs will ignore geometry
       * with this hint set in transparency/refraction rays.
       * By default, this hint is not set. */
      NVSG_HINT_INVISIBLE_TO_REFRACTIONS          = BIT9,

      NVSG_HINT_LAST_HINT                         = NVSG_HINT_INVISIBLE_TO_REFRACTIONS,

      /*! \brief All possible hints */
      NVSG_HINT_ALL_HINTS                         = (NVSG_HINT_LAST_HINT << 1) - 1
    };

    enum
    {
      // incarnation state
      NVSG_INCARNATION                        = NVSG_HINT_LAST_HINT               << 1,
      NVSG_TREE_INCARNATION                   = NVSG_INCARNATION                  << 1,
      NVSG_SWITCH_INCARNATION                 = NVSG_TREE_INCARNATION             << 1,
      NVSG_MATERIAL_INCARNATION               = NVSG_SWITCH_INCARNATION           << 1,
      NVSG_BOUNDING_VOLUME_INCARNATION        = NVSG_MATERIAL_INCARNATION         << 1,
      NVSG_VAS_INCARNATION                    = NVSG_BOUNDING_VOLUME_INCARNATION  << 1,
      NVSG_INDEXSET_INCARNATION               = NVSG_VAS_INCARNATION              << 1,
      NVSG_PRIMITIVE_INCARNATION              = NVSG_INDEXSET_INCARNATION         << 1,

      // bounding volume state
      NVSG_BOUNDING_BOX                       = NVSG_PRIMITIVE_INCARNATION        << 1,
      NVSG_BOUNDING_SPHERE                    = NVSG_BOUNDING_BOX                 << 1,
      
      NVSG_BOUNDING_VOLUMES                   = NVSG_BOUNDING_BOX | 
                                                NVSG_BOUNDING_SPHERE |
                                                NVSG_BOUNDING_VOLUME_INCARNATION,
      // containment state
      NVSG_CONTAINS_SHADER                    = NVSG_BOUNDING_SPHERE                    << 1,
      NVSG_CONTAINS_TRANSPARENCY              = NVSG_CONTAINS_SHADER                    << 1,
      NVSG_CONTAINS_LIGHT                     = NVSG_CONTAINS_TRANSPARENCY              << 1,
      NVSG_CONTAINS_LOD                       = NVSG_CONTAINS_LIGHT                     << 1,
      NVSG_CONTAINS_ANIMATION                 = NVSG_CONTAINS_LOD                       << 1,

      NVSG_CONTAINS_MASK                      = NVSG_CONTAINS_SHADER | 
                                                NVSG_CONTAINS_TRANSPARENCY | 
                                                NVSG_CONTAINS_LIGHT | 
                                                NVSG_CONTAINS_LOD | 
                                                NVSG_CONTAINS_ANIMATION,
      // hash key state
      NVSG_HASH_KEY                           = NVSG_CONTAINS_ANIMATION                 << 1
    };

  public:
    /*! \brief The container type of the Callbacks */
    typedef std::vector<nvutil::SmartPtr<Callback> >                          CallbackContainer;

    /*! \brief The iterator over the CallbackContainer */
    typedef nvutil::SceniXIterator<Object,CallbackContainer::iterator>        CallbackIterator;

    /*! \brief The const iterator over the CallbackContainer */
    typedef nvutil::SceniXIterator<Object,CallbackContainer::const_iterator>  CallbackConstIterator;

   public:
    /*! \brief Constructs an Object.
      */
    NVSG_API Object();

    /*! \brief Constructs an Object as a copy of another Object.
      */
    NVSG_API Object( const Object& rhs );

    /*! \brief Destructs an Object. 
      */
    NVSG_API virtual ~Object();

    /*! \brief Returns the object code, which is unique per object type.
      * \return
      * The function returns the object code enum for this object.
      * \remarks
      * The object code, which is unique per object type, is used for fast object type 
      * identification at runtime. A nvtraverser::Traverser, for example, uses the object code to 
      * fast lookup the handler function for a particular object type. 
      * \n\n
      * Object-derived classes must override the protected member 
      * \link nvsg::Object::m_objectCode m_objectCode \endlink, with the correct value for the
      * respective type. It is recommended to do this at instantiation time of the customized object.
      * \sa \ref howtoderiveanvsgobject, Object::getHigherLevelObjectCode
      */
    unsigned int getObjectCode() const;

    /*! \brief Returns the higher-level object code for a given object code.
      * \param
      * oc %Object code for which to retrieve the associated higher-level object code.
      * \return
      * The default implementation returns OC_INVALID, which causes the traverser framework to 
      * immediately proceed without handling the object when it comes across an unknown object code.
      * \remarks
      * This function will be called from the object traverser framework if the object code \a oc 
      * is unknown. This would be the case if for a custom object a certain traverser did not 
      * register an appropriate handler function.
      * \n\n
      * The framework expects this function to return a higher-level object code in terms of a 
      * custom object hierarchy. The function will be repeatedly called with the returned 
      * higher-level object code, until either a known object code or OC_INVALID will be returned. 
      * That is - by repeatedly calling this function, the traverser framework moves up the custom 
      * object hierarchy to find an object for which a handler function was registered. If the 
      * traverser framework recognizes a known object code, it calls the appropriate handler 
      * function and proceeds. If OC_INVALID was returned, the traverser framework proceeds without 
      * handling the object.
      * \n\n
      * The runtime behavior is undefined if the function returns an object code of an object that 
      * is not part of the object hierarchy of the primary object!
      * \n\n
      * The framework might end up in a deadlock if the function never returns either OC_INVALID or 
      * an object code of a concrete object known by the traverser!
      * \sa Object::getObjectCode
      */
    NVSG_API virtual unsigned int getHigherLevelObjectCode(unsigned int oc) const; 

    /*! \brief Returns whether this object shares its embedded data with other objects.
      * \return
      * The function should return \c true if the object shares its embedded data with other 
      * objects. Otherwise, the function should return \c false.
      * \n\n
      * The default implementation always returns \c false.
      * \remarks
      * For Object-derived classes that are capable of sharing their embedded data with other 
      * objects, it is recommended to override this function. For all other objects, the default
      * implementation will be sufficient.
      * \n\n
      * An object, capable of data sharing, shares its data with another object if it was either 
      * instantiated as a copy of the other object, or if its content was re-assigned to by the 
      * content of the other object using the appropriate assignment operator.
      * \n\n
      * Only a few classes of the SceniX core implement data sharing. These mainly are classes where
      * data sharing probably could save a lot of memory because of the large amount of data these
      * classes might contain. VertexAttributeSet, IndependentPrimitiveSet, and StrippedPrimitiveSet
      * are examples of \ref coreobjects that implement data sharing.
      * \sa Object::getDataID
      */
    NVSG_API virtual bool isDataShared() const;

    /*! \brief Returns the unique data identifier for this object.
      * \return
      * The function returns a 64-bit value which uniquely identifies the embedded data.
      * \remarks
      * This function in particular is useful to identify different objects that share their embedded
      * data. Two objects that share the same data always have the same DataID.
      * \n\n
      * Identifying objects that share their data with other objects is useful for saving objects
      * to a file and later reloading it without losing the data sharing. 
      * \sa Object::isDataShared
      */
    NVSG_API virtual DataID getDataID() const;

    NVSG_API virtual void notifyChange( const Subject * originator, unsigned int state = 0 ) const;

    /*! \brief Specifies the name for an object.
      * \param
      * name Reference to a STL sting holding the name to specify. 
      * \remarks
      * A previously specified name gets lost when specifying a new name using this function.
      * \n\n
      * Use Object::getName to retrieve the actual name of an Object.  
      */
    NVSG_API void setName(const std::string& name);

    /*! \brief Returns the name of the object.
      * \return
      * A reference to a STL string holding the name of the object. The function returns a reference
      * to an empty string if no name was previously specified for the object. 
      * \remarks
      * The function retrieves the object's name previously specified through Object::setName. 
      */
    NVSG_API const std::string& getName() const;

    /*! \brief Lets you append an annotation to this Object.
     *  \param anno Annotation to append to this Object.
     *  \remarks
     *  The function lets you append an optional annotation. 
     *  A previously appended annotation gets lost.
     *  \sa Object::getAnnotation
     */
    NVSG_API void setAnnotation(const std::string& anno);

    /*! \brief Retrieves the Object's annotation. 
     *  \return
     *  The function returns the annotation as last specified through setAnnotation. 
     *  If no annotation was specified before, the function returns an empty string.
     *  \sa Object::setAnnotation
     */
    NVSG_API const std::string& getAnnotation() const;

    /*! \brief Tests whether this object is equivalent to another object.  
      * \param
      * object Pointer to the object to test for equivalence with this object.
      * \param
      * ignoreNames Object names will be ignored while testing if this is \c true.
      * \param
      * deepCompare The function performs a deep-compare if this is \c true.
      * Otherwise the function only performs a shallow compare.
      * \return
      * The function returns \c true if the object pointed to by \a object is detected to be 
      * equivalent to this object. Otherwise the function returns \c false.
      * \remarks
      * The test will be performed considering the optional control parameters ignoreNames and deepCompare. 
      * If you omit these two, the function ignores object names
      * and performs a shallow compare only.
      */
    NVSG_API virtual bool isEquivalent( const Object * object
                                      , bool ignoreNames = true
                                      , bool deepCompare = false) const;

    /*! \brief Attaches arbitrary user data to the object.
     * \param 
     * userData Specifies the address of the arbitrary user data.
     * \remarks
     * Use setUserData to store the address to arbitrary user data with the object.
     * The object does not interpret the data, nor does it take over responsibility 
     * for managing the memory occupied by the data behind \a userData. It just keeps 
     * the address to the user data until the address will be overwritten by a 
     * subsequent call to setUserData.
     * \n\n
     * User data will not be considered for storing to a file! 
     * \n\n
     * Use getUserData to get back the address to the user data that was last specified 
     * through setUserData.
     * \sa getUserData
     */
    NVSG_API void setUserData(const void* userData);

    /*! \brief Returns a pointer to the attached arbitrary user data.
     * \return
     * The function returns the pointer to the arbitrary user data that was last 
     * specified by a call to setUserData.
     * \sa setUserData
     */
    NVSG_API const void* getUserData() const; 
    
    NVSG_API unsigned int getHints() const;
    NVSG_API unsigned int getHints( unsigned int mask ) const;
    NVSG_API void setHints( unsigned int hints );
    NVSG_API void addHints( unsigned int hints );
    NVSG_API void removeHints( unsigned int hints );
    NVSG_API const nvmath::Box3f & getBoundingBox() const;
    NVSG_API const nvmath::Sphere3f & getBoundingSphere() const;
    NVSG_API const nvutil::Incarnation& getIncarnation() const;
    NVSG_API const nvutil::Incarnation& getTreeIncarnation() const;
    NVSG_API const nvutil::Incarnation& getBoundingVolumeIncarnation() const;
    NVSG_API const nvutil::Incarnation& getSwitchIncarnation() const;
    NVSG_API const nvutil::Incarnation& getMaterialIncarnation() const;
    NVSG_API const nvutil::Incarnation& getVertexAttributeSetIncarnation() const;
    NVSG_API const nvutil::Incarnation& getIndexSetIncarnation() const;
    NVSG_API const nvutil::Incarnation& getPrimitiveIncarnation() const;
    NVSG_API const nvutil::Incarnation& getHintsIncarnation() const;

    /*! \brief Get the hash key of this Object.
     *  \return The hash key of this Object.
     *  \remarks If the hash key is not valid, the virtual function feedHashGenerator() is called to
     *  recursively determine a hash string, using a HashGenerator, which then is converted into
     *  a HashKey.
     *  \sa feedHashGenerator */
    NVSG_API HashKey getHashKey() const;

    NVSG_API unsigned int getContainedHints(unsigned int which) const;
    NVSG_API bool containsHints(unsigned int which) const;
    NVSG_API bool containsShader() const;
    NVSG_API bool containsTransparency() const;
    NVSG_API bool containsAnimation() const;
    NVSG_API bool containsLight() const;
    NVSG_API bool containsLOD() const;

    /*  \brief Get the number of AppTraverserCallbacks in this object.
     *  \return The number of AppTraverserCallbacks in this object.
     *  \remarks AppTraverserCallbacks are invoked from AppTraverser::postTraverseObject. That is, whenever
     *  an object has been traversed by the AppTraverser, its app traverser callbacks are called.
     *  \sa Callback, AppTraverser, beginAppTraverserCallbacks, endAppTraverserCallbacks, addAppTraverserCallback,
     *  removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API unsigned int getNumberOfAppTraverserCallbacks() const;

    /*! \brief Get a const iterator to the first AppTraverserCallback in this Object.
     *  \return A const iterator to the first AppTraverserCallback in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, endAppTraverserCallbacks,
     *  addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackConstIterator beginAppTraverserCallbacks() const;

    /*! \brief Get an iterator to the first AppTraverserCallback in this Object.
     *  \return An iterator to the first AppTraverserCallback in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, endAppTraverserCallbacks,
     *  addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackIterator beginAppTraverserCallbacks();

    /*! \brief Get a const iterator that points just beyond the end of the AppTraverserCallbacks in this Object.
     *  \return A const iterator that points just beyond the end of the AppTraverserCallbacks in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackConstIterator endAppTraverserCallbacks() const;

    /*! \brief Get an iterator that points just beyond the end of the AppTraverserCallbacks in this Object.
     *  \return An iterator that points just beyond the end of the AppTraverserCallbacks in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackIterator endAppTraverserCallbacks();

    /*! \brief Add an AppTraverserCallback to this Object.
     *  \param cb The Callback to add.
     *  \return An iterator that points to the position where \a cb was added.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, removeAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackIterator addAppTraverserCallback( Callback * cb );

    /*! \brief Remove an AppTraverserCallback from this Object.
     *  \param cb The Callback to remove from this Object.
     *  \return \c true, if the AppTraverserCallback has been removed from this Object, otherwise \c false.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, addAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API bool removeAppTraverserCallback( const Callback * cb );

    /*! \brief Remove an AppTraverserCallback from this Object.
     *  \param ci An iterator to the Callback to remove from this Object.
     *  \return An iterator pointing to the new location of the AppTraverserCallback that followed the one removed by
     *  this function call, which is endAppTraverserCallbacks() if the operation removed the last Callback.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, addAppTraverserCallback, clearAppTraverserCallbacks, findAppTraverserCallback */
    NVSG_API CallbackIterator removeAppTraverserCallback( const CallbackIterator & ci );

    /*! \brief Remove all AppTraverserCallbacks from this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, addAppTraverserCallback, removeAppTraverserCallback, findAppTraverserCallback */
    NVSG_API void clearAppTraverserCallbacks();

    /*  \brief Find a specified AppTraverserCallback in this Object.
     *  \param cb The Callback to find.
     *  \return A const iteator to the found AppTraverserCallback in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks */
    NVSG_API CallbackConstIterator findAppTraverserCallback( const Callback * cb ) const;

    /*  \brief Find a specified AppTraverserCallback in this Object.
     *  \param cb The Callback to find.
     *  \return An iteator to the found AppTraverserCallback in this Object.
     *  \sa Callback, AppTraverser, getNumberOfAppTraverserCallbacks, beginAppTraverserCallbacks,
     *  endAppTraverserCallbacks, addAppTraverserCallback, removeAppTraverserCallback, clearAppTraverserCallbacks */
    NVSG_API CallbackIterator findAppTraverserCallback( const Callback * cb );

    /*! \brief Set a TraversalMask to be used with this Object.
     *  \param mask The mask to be used.
     *  \remarks The traversal mask is used in conjuction with Traverser-derived and Renderer-derived objects to
     *  determine whether this node, and therefore any of this node's children, are traversed, and/or rendered.
     *  See Traverser::setTraversalMask and SceneRenderer::setTraversalMask for more info.
     *  \note The default traversal mask is ~0
     *  \sa getTraversalMask, ViewState::setTraversalMask, Traverser::setTraversalMask, Traverser::setTraversalMaskOverride,
     *  \sa SceneRenderer::setTraversalMask, SceneRenderer::setTraversalMaskOverride */
    NVSG_API void setTraversalMask( unsigned int mask );

    /*  \brief Get the Traversal Mask
     *  \return The current traversal mask.
     *  \sa setTraversalMask, nvsg::ViewState::getTraversalMask */
    NVSG_API unsigned int getTraversalMask() const;

    NVSG_API const DALHostSharedPtr & getDALHost() const;

    NVSG_API void releaseDALData();

  // exposed properties for reflection
  public:
    REFLECTION_INFO_API( NVSG_API, Object );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Name );
          NVSG_API DECLARE_STATIC_PROPERTY( Annotation );
          NVSG_API DECLARE_STATIC_PROPERTY( Hints );
          NVSG_API DECLARE_STATIC_PROPERTY( TraversalMask );
      END_DECLARE_STATIC_PROPERTIES

  protected: // accessible for derived classes
    /*! \brief Assigns new content from another Object. 
      * \param
      * rhs Reference to an Object from which to assign the new content.
      * \return
      * A reference to this object.
      * \remarks
      * The assignment operator unreferences the old content before assigning the new content. The new
      * content will be a deep-copy of the content of right-hand-sided object. 
      */
    NVSG_API Object & operator=(const Object & rhs);

    NVSG_API virtual unsigned int determineHintsContainment(unsigned int which) const;
    NVSG_API virtual bool determineShaderContainment() const;
    NVSG_API virtual bool determineTransparencyContainment() const;
    NVSG_API virtual bool determineAnimationContainment() const;
    NVSG_API virtual bool determineLightContainment() const;
    NVSG_API virtual bool determineLODContainment() const;
    NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;
    NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;
    NVSG_API virtual const nvmath::Box3f & getBoundingBox( bool recalculate ) const;
    NVSG_API virtual const nvmath::Sphere3f & getBoundingSphere( bool recalculate ) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
    NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    typedef const nvutil::Incarnation& (Object::*PFNGETINCARNATION)() const;
    NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

    NVSG_API virtual void incrementHintsIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryHintsIncarnation() const;
    NVSG_API virtual void incrementIndexSetIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryIndexSetIncarnation() const;
    NVSG_API virtual void incrementMaterialIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryMaterialIncarnation() const;
    NVSG_API virtual void incrementPrimitiveIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryPrimitiveIncarnation() const;
    NVSG_API virtual void incrementSwitchIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & querySwitchIncarnation() const;
    NVSG_API virtual void incrementVertexAttributeSetIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryVertexAttributeSetIncarnation() const;

  protected:
    /*! \brief Per-object type identifier.
      * \remarks
      * Concrete derived objects must initialize this object code with an identifier,
      * which must be a unique per-object type. It is recommended to do this at object 
      * instantiation.
      * \n\n
      * SceniX uses this object code for fast object type identification at runtime.
      * \n\n
      * <B>Codes from 0x00 to 0xBF are reserved for SceniX core objects,
      * and may not be used for custom object types!</B>
      * \n\n
      * Codes for custom objects should start at OC_CUSTOMOBJECT:
      * \n\n
      * \code
      * // example for custom object code initialization
      *
      * enum
      * {
      *   OC_MYFIRSTOBJECT = nvsg::OC_CUSTOMOBJECT,
      *   OC_MYSECONDOBJECT, // = OC_MYFIRSTOBJECT + 1
      *   OC_MYTHIRDOBJECT.  // = OC_MYFIRSTOBJECT + 2
      * // ... further codes
      * };
      * \endcode
      */
    unsigned int m_objectCode;

    mutable unsigned int     m_flags; // containment, contained hints
    mutable HashKey          m_hashKey;

  private:
    std::string     * m_name; // optional name
    std::string     * m_annotation; // optional annotation specified for the object

    const void      * m_userData; // dumb pointer to arbitrary user data
    unsigned int      m_hints;  // object exclusive hints
    unsigned int      m_traversalMask; // object traversal mask

    CallbackContainer * m_appTraverserCallbacks;  // callbacks called on AppTraverser traversal

    mutable nvutil::Incarnation  m_incarnation;     // tracks net changes 
    mutable nvutil::Incarnation  m_treeIncarnation; // tracks tree changes
    mutable nvutil::Incarnation  m_bvIncarnation; // tracks BV changes

    mutable unsigned int      m_dirtyState;
    mutable DALHostSharedPtr  m_dalHost;

    static nvmath::Box3f        m_defaultBoundingBox;
    static nvmath::Sphere3f     m_defaultBoundingSphere;
    static CallbackContainer    m_dummyCallbackContainer;
    static nvutil::Incarnation  m_dummyIncarnation;
  };
  
  NVSG_API void copy( const ObjectReadLock & src, const ObjectWriteLock & dst );

#if !defined(NDEBUG)
  template <typename T>
  inline void NVSG_ASSERT_OBJECT_WRITE_LOCKED(const T* obj)
  {
    nvutil::getWeakPtr<nvutil::HandledObject>(obj)->assertWriteLocked();
  }
  template <typename T>
  inline void NVSG_ASSERT_OBJECT_READ_LOCKED(const T* obj)
  {
    nvutil::getWeakPtr<nvutil::HandledObject>(obj)->assertReadLocked();
  }
#else
# define NVSG_ASSERT_OBJECT_WRITE_LOCKED(x) static_cast<void*>(0);
# define NVSG_ASSERT_OBJECT_READ_LOCKED(x) static_cast<void*>(0);
#endif

  inline unsigned int Object::getObjectCode() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_objectCode;
  }

  inline void Object::setUserData(const void * userData)
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_userData != userData )
    {
      m_userData = userData; // no sanity check!
      notifyChange( this );
    }
  }

  inline const void * Object::getUserData() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_userData;
  }

  inline unsigned int Object::getHints() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( getHints(~0) );
  }

  inline unsigned int Object::getHints( unsigned int mask ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_hints & mask );
  }

  inline void Object::setHints( unsigned int hints )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_hints != hints )
    {
      unsigned int changedHints = ( hints & ~m_hints ) | ( ~hints & m_hints );
      m_hints = hints;
      notifyChange( this, changedHints );
      notify( PID_Hints );
    }
  }

  inline void Object::addHints( unsigned int hints )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    unsigned int changedHints = hints & ~m_hints;
    if ( changedHints )
    {
      m_hints |= hints;
      notifyChange( this, changedHints );
      notify( PID_Hints );
    }
  }

  inline void Object::removeHints( unsigned int hints )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    unsigned int changedHints = hints & m_hints;
    if ( changedHints )
    {
      m_hints &= ~hints;
      notifyChange( this, changedHints );
      notify( PID_Hints );
    }
  }

  inline unsigned int Object::getNumberOfAppTraverserCallbacks() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_appTraverserCallbacks ? checked_cast<unsigned int>(m_appTraverserCallbacks->size()) : 0 );
  }

  inline Object::CallbackConstIterator Object::beginAppTraverserCallbacks() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CallbackConstIterator( m_appTraverserCallbacks ? m_appTraverserCallbacks->begin() : m_dummyCallbackContainer.begin() ) );
  }

  inline Object::CallbackIterator Object::beginAppTraverserCallbacks()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CallbackIterator( m_appTraverserCallbacks ? m_appTraverserCallbacks->begin() : m_dummyCallbackContainer.begin() ) );
  }

  inline Object::CallbackConstIterator Object::endAppTraverserCallbacks() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CallbackConstIterator( m_appTraverserCallbacks ? m_appTraverserCallbacks->end() : m_dummyCallbackContainer.end() ) );
  }

  inline Object::CallbackIterator Object::endAppTraverserCallbacks()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CallbackIterator( m_appTraverserCallbacks ? m_appTraverserCallbacks->end() : m_dummyCallbackContainer.end() ) );
  }

  inline Object::CallbackIterator Object::removeAppTraverserCallback( const CallbackIterator & ci )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CallbackIterator( m_appTraverserCallbacks ? m_appTraverserCallbacks->erase( ci.m_iter ) : m_dummyCallbackContainer.end() ) );
  }

  inline void Object::clearAppTraverserCallbacks()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_appTraverserCallbacks )
    {
      m_appTraverserCallbacks->clear();
    }
  }

  inline Object::CallbackConstIterator Object::findAppTraverserCallback( const Callback * cb ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CallbackConstIterator( m_appTraverserCallbacks ? std::find( m_appTraverserCallbacks->begin(), m_appTraverserCallbacks->end(), cb ) : m_dummyCallbackContainer.end() ) );
  }

  inline Object::CallbackIterator Object::findAppTraverserCallback( const Callback * cb )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CallbackIterator( m_appTraverserCallbacks ? std::find( m_appTraverserCallbacks->begin(), m_appTraverserCallbacks->end(), cb ) : m_dummyCallbackContainer.end() ) );
  }

  inline unsigned int Object::getTraversalMask() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_traversalMask );
  }

  inline void Object::setTraversalMask( unsigned int mask )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_traversalMask != mask )
    {
      m_traversalMask = mask;
      notifyChange( this );
      notify( PID_TraversalMask );
    }
  }

  inline void Object::incrementHintsIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::queryHintsIncarnation() const
  {
    return( Object::m_dummyIncarnation );
  }

  inline void Object::incrementIndexSetIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::queryIndexSetIncarnation() const
  {
    return( m_dummyIncarnation );
  }

  inline void Object::incrementMaterialIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::queryMaterialIncarnation() const
  {
    return( m_dummyIncarnation );
  }

  inline void Object::incrementPrimitiveIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::queryPrimitiveIncarnation() const
  {
    return( m_dummyIncarnation );
  }

  inline void Object::incrementSwitchIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::querySwitchIncarnation() const
  {
    return( m_dummyIncarnation );
  }

  inline void Object::incrementVertexAttributeSetIncarnation() const
  {
  }

  inline const nvutil::Incarnation & Object::queryVertexAttributeSetIncarnation() const
  {
    return( m_dummyIncarnation );
  }

  //! Detects if the WeakPtr \a p is a WeakPtr to a specified (templated) type.
  /** \returns \c true if \a p is a WeakPtr to the specified type, \c false otherwise. */
  template<typename T>
  inline bool isPtrTo( const nvutil::HandledObjectWeakPtr & p )
  {
    return( dynamic_cast<const typename nvutil::ObjectTraits<T>::Handle *>( p ) != NULL );
  }

  //! Detects if the SharedPtr \a p is a SharedPtr to a specified (templated) type.
  /** \returns \c true if \a p is a SharedPtr to the specified type, \c false otherwise. */
  template<typename T>
  inline bool isPtrTo( const nvutil::HandledObjectSharedPtr & p )
  {
    return( isPtrTo<T>( p.get() ) );
  }

  //! Detects if the two objects \a p0 and \a p1 are of the same type.
  /** \return \c true if \a p0 and \a p1 are of the same type, \c false otherwise. */
  inline bool areOfSameType( const ObjectSharedPtr & p0, const ObjectSharedPtr & p1 )
  { // found not initialized object codes if one of the following fires
    ObjectReadLock obj0(p0);
    ObjectReadLock obj1(p1);
    NVSG_ASSERT(obj0->getObjectCode()!=OC_INVALID);
    NVSG_ASSERT(obj1->getObjectCode()!=OC_INVALID);
    return( obj0->getObjectCode()==obj1->getObjectCode() );
  }

#if !defined(DOXYGEN_IGNORE)
  template <typename T, typename U>
  inline T object_cast_impl(const nvutil::ReadableObject<U>& obj)
  {
    NVSG_CTASSERT(!!(nvutil::Conversion<T,const U*>::exists));
    return dynamic_cast<T>(obj.operator->());
  }

  template <typename T, typename U>
  inline T object_cast_impl(const nvutil::WritableObject<U>&  obj)
  {
    NVSG_CTASSERT(!!(nvutil::Conversion<T,U*>::exists));
    return dynamic_cast<T>(obj.operator->());
  }
#endif

#define object_cast object_cast_impl
// for downward compatibility
#define node_cast object_cast_impl
#define drawable_cast object_cast_impl
#define light_cast object_cast_impl
#define camera_cast object_cast_impl


/*! \def object_cast 
 *  \brief Downcasts a ReadableObject or a WritableObject
 *  
 * The expression <b> object_cast<type-id>(obj) </b> downcasts the operand \a obj
 * to an object of type \a type-id. The operand \a obj must be of type ReadableObject<T>
 * or WritableObject<T>, and \a type-id must be a pointer or a pointer to const of a type
 * dynamic-convertible to type \b T. The expression evaluates to NULL if \a obj is not
 * convertible to \a type-id.
 *
 * <b> Usage </b>
 * \code
 * void foo(const Group* p)
 * {
 *   unsigned int numChildren = p->getNumberOfChildren();
 *   for ( unsigned int i=0 ; i<numChildren ; i++ )
 *   {
 *     NodeReadLock node(p->getChild(i));
 *     const Group * group = object_cast<const Group *>(node); 
 *     if ( group )
 *     {
 *       // ... do something Group-specific
 *     }
 *   }
 * }
 * \endcode
 * \sa nvutil::ReadableObject, nvutil::WritableObject
 */
   
} //  namespace nvsg
