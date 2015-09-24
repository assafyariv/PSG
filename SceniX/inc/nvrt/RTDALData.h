// Copyright NVIDIA Corporation 2002-2009
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

#include "nvsgcommon.h" // for nvutil
#include "nvsg/DAL.h"
#include "nvrt/RTContext.h"
#include "nvsg/GeoNode.h"
#include "nvsg/Group.h"
#include "nvsg/FlipbookAnimation.h"
#include "nvsg/RTBufferAttribute.h"
#include "nvsg/RTFx.h"
#include "nvsg/Switch.h"
#include "nvsg/Transform.h"
#include "nvsg/VertexAttributeSet.h"

#include <vector>
#include <set>


namespace nvrt
{
  // identifier for device abstraction layer data
  enum
  {
      RT_BUFFER = 1
    , RT_GEOMETRY
    , RT_GEOMETRYGROUP
    , RT_GROUP
    , RT_MATERIAL
    , RT_RTFXSCENEATTRIBUTE
    , RT_SELECTOR
    , RT_TRANSFORM
    , RT_VASBUFFER
    , RT_TEXTURESAMPLER
  };

  class RTDALDataCreator : public nvsg::DALDataCreator
  {
  public:
    NVRT_API ~RTDALDataCreator();
    NVRT_API void onReleaseDAL(nvsg::HDAL); // overrides DALDataCreator::onReleaseDAL

    NVRT_API void addSelector(nvsg::HDAL);
    NVRT_API void addTransform(nvsg::HDAL);
    NVRT_API void addBillboard(nvsg::HDAL);
    NVRT_API void addGeometryGroup(nvsg::HDAL);
    NVRT_API void addMaterial(nvsg::HDAL);
    NVRT_API void addBuffer(nvsg::HDAL);
    NVRT_API void addVASBuffer(nvsg::HDAL);
    NVRT_API void addRTFxSceneAttribute(nvsg::HDAL);
    NVRT_API void addGroup(nvsg::HDAL);
    NVRT_API void addGeometry(nvsg::HDAL);
    NVRT_API void addTextureSampler(nvsg::HDAL);
    
    NVRT_API void releaseSelectors();
    NVRT_API void releaseTransforms();
    NVRT_API void releaseBillboards();
    NVRT_API void releaseGeometryGroups();
    NVRT_API void releaseMaterials();
    NVRT_API void releaseBuffers();
    NVRT_API void releaseVASBuffers();
    NVRT_API void releaseRTFxs();
    NVRT_API void releaseGroups();
    NVRT_API void releaseGeometries();
    NVRT_API void releaseTextureSamplers();

  private:
    // DAL resources
    std::set<nvsg::HDAL> m_selectors;
    std::set<nvsg::HDAL> m_transforms;
    std::set<nvsg::HDAL> m_switches;
    std::set<nvsg::HDAL> m_billboards;
    std::set<nvsg::HDAL> m_geometryGroups;
    std::set<nvsg::HDAL> m_materials;
    std::set<nvsg::HDAL> m_buffers;
    std::set<nvsg::HDAL> m_VASBuffers;
    std::set<nvsg::HDAL> m_rtfxs;
    std::set<nvsg::HDAL> m_groups;
    std::set<nvsg::HDAL> m_geometries;
    std::set<nvsg::HDAL> m_textureSamplers;
  };


  struct RTDALData : public nvsg::DALData
  {
    NVRT_API RTDALData(nvrt::RTDALDataCreator *creator, const nvsg::Object *p);
    NVRT_API virtual ~RTDALData();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTDALDATA) || DALData::isTypeOf(typeId);
    }

    nvrt::RTDALDataCreator *m_creator;
    // These three incarnation are shared by all caches below.
    // Switch nodes need one more.
    nvutil::Incarnation m_incarnation;
    nvutil::Incarnation m_treeIncarnation;
    nvutil::Incarnation m_boundingVolumeIncarnation;
  };

  // Used by Switch and FlipbookAnimation nodes.
  struct RTSelectorCache : public nvrt::RTDALData
  {
    NVRT_API RTSelectorCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTSelectorWeakPtr & selector );
    NVRT_API ~RTSelectorCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTSELECTORCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTSelectorSharedPtr m_selector;
    nvutil::Incarnation       m_switchIncarnation; // Only used by Switch nodes.
  };

  struct RTTransformCache : public nvrt::RTDALData
  {
    NVRT_API RTTransformCache( nvrt::RTDALDataCreator *creator, nvsg::Group *p, const nvrt::RTTransformWeakPtr & transform );
    NVRT_API ~RTTransformCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTTRANSFORMCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTTransformSharedPtr  m_transform;
    // One entry per encountered Billboard.
    // If one Billboard is encountered multiple times, there are as many entries.
    std::map<std::vector<void *>, nvrt::RTTransformSharedPtr> m_billboardPathMap;
  };

  struct RTGeometryGroupCache : public nvrt::RTDALData
  {
    NVRT_API RTGeometryGroupCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTGeometryGroupWeakPtr & geometryGroup );
    NVRT_API ~RTGeometryGroupCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTGEOMETRYGROUPCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTGeometryGroupSharedPtr  m_geometryGroup;
    unsigned int                    m_hints;
  };

  struct RTMaterialCache : public nvrt::RTDALData
  {
    NVRT_API RTMaterialCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTMaterialWeakPtr & material );
    NVRT_API RTMaterialCache(nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTMaterialSharedPtr & material );
    NVRT_API ~RTMaterialCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTMATERIALCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTMaterialSharedPtr m_material;
    nvutil::Incarnation       m_nvsgMaterialIncarnation; // the NVSG material incarnation 
    nvutil::Incarnation       m_rtfxProgramIncarnation;  // the incarnation for the attached rtfxProgram
  };

  struct RTBufferCache : public nvrt::RTDALData
  {
    NVRT_API RTBufferCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTBufferWeakPtr & buffer );
    NVRT_API ~RTBufferCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTBUFFERCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTBufferSharedPtr m_buffer;
  };

  struct RTTextureSamplerCache : public nvrt::RTDALData
  {
    NVRT_API RTTextureSamplerCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTTextureSamplerWeakPtr & tsh );
    NVRT_API ~RTTextureSamplerCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTTEXTURESAMPLERCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTTextureSamplerSharedPtr m_textureSampler;
  };

  struct RTVASBufferCache : public nvrt::RTDALData
  {
    NVRT_API RTVASBufferCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTBufferWeakPtr & attributeBuffer, const nvrt::RTBufferWeakPtr & vertexBuffer );
    NVRT_API ~RTVASBufferCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTVASBUFFERCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTBufferSharedPtr m_attributeBuffer;
    nvrt::RTBufferSharedPtr m_vertexBuffer;
  };

  struct RTFxSceneAttributeCache : public nvrt::RTDALData
  {
    NVRT_API RTFxSceneAttributeCache(nvrt::RTDALDataCreator *creator, nvsg::Object *p );
    NVRT_API ~RTFxSceneAttributeCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTFXSCENEATTRIBUTECACHE) || RTDALData::isTypeOf(typeId);
    }

    std::vector<nvrt::RTProgramSharedPtr>   m_rtPrograms;
    nvutil::Incarnation                     m_rtfxProgramIncarnation;
  };

  struct RTGroupCache : public nvrt::RTDALData
  {
    NVRT_API RTGroupCache( nvrt::RTDALDataCreator *creator, const nvsg::Object *p, const nvrt::RTGroupWeakPtr & group );
    NVRT_API ~RTGroupCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTGROUPCACHE) || RTDALData::isTypeOf(typeId);
    }

    nvrt::RTGroupSharedPtr  m_group;
  };

  struct RTGeometryCache : public nvrt::RTDALData
  {
    NVRT_API RTGeometryCache( nvrt::RTDALDataCreator *creator, nvsg::Object *p, const nvrt::RTGeometryWeakPtr & geometry );
    NVRT_API ~RTGeometryCache();

    virtual bool isTypeOf(int typeId) const 
    { 
      return (typeId == DT_RTGEOMETRYCACHE) || RTDALData::isTypeOf(typeId);
    }
    
    nvrt::RTGeometrySharedPtr m_geometry;
  };

}

