#include "vesTestBase.h"

#include <vesActor.h>
#include <vesCamera.h>
#include <vesGeometryData.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesRenderer.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

//----------------------------------------------------------------------------
namespace {

class vesTestDrawPlane : public vesTestBase
{
public:

  vesTestDrawPlane() : vesTestBase(),
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_material(vesMaterial::Ptr(new vesMaterial())),
    m_mapper(vesMapper::Ptr(new vesMapper())),
    m_actor(vesActor::Ptr(new vesActor()))
  {
  }


  ~vesTestDrawPlane()
  {
  }

  bool initScene()
  {
    this->m_material->addAttribute(this->m_positionVertexAttribute);
    this->m_material->addAttribute(this->m_normalVertexAttribute);
    this->m_material->addAttribute(this->m_colorVertexAttribute);

    this->m_mapper->setGeometryData(this->createPlane());
    this->m_actor->setMapper(this->m_mapper);
    this->m_actor->setMaterial(this->m_material);
    this->m_renderer->addActor(this->m_actor);

    this->m_renderer->camera()->setParallelProjection(false);
    this->m_renderer->setBackgroundColor(0.0, 0.4, 0.6);

    return true;
  }


  vesSharedPtr<vesGeometryData> createPlane()
  {
    vesGeometryData::Ptr geometryData (new vesGeometryData());
    vesSourceDataP4N4C4f::Ptr sourceData(new vesSourceDataP4N4C4f());

    vesVector4f topLeftColor = vesVector4f(0.0f, 1.0f, 0.0f, 1.0f);
    vesVector4f bottomRightColor = vesVector4f(0.0f, 0.0f, 1.0f, 1.0f);
    vesVector4f color = vesVector4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Points.
    vesVertexDataP4N4C4f v1;
    v1.m_position = vesVector4f(-1.0f, -1.0f, 0.0f, 1.0f);
    v1.m_normal = vesVector4f(0.0f, 0.0f, 1.0f, 0.0f);
    v1.m_color = vesVector4f(color[0], color[1], color[2], color[3]);

    vesVertexDataP4N4C4f v2;
    v2.m_position = vesVector4f(1.0f, -1.0f, 0.0f, 1.0f);
    v2.m_normal = vesVector4f(0.0f, 0.0f, 1.0f, 0.0f);
    v2.m_color = vesVector4f(bottomRightColor[0], bottomRightColor[1],
      bottomRightColor[2], bottomRightColor[3]);

    vesVertexDataP4N4C4f v3;
    v3.m_position = vesVector4f(1.0f, 1.0f, 0.0f,1.0f);
    v3.m_normal = vesVector4f(0.0f, 0.0f, 1.0f, 0.0f);
    v3.m_color = vesVector4f(color[0], color[1], color[2], color[3]);

    vesVertexDataP4N4C4f v4;
    v4.m_position = vesVector4f(-1.0f, 1.0f, 0.0f, 1.0f);
    v4.m_normal = vesVector4f(0.0f, 0.0f, 1.0f, 0.0f);
    v4.m_color = vesVector4f(topLeftColor[0], topLeftColor[1],
        topLeftColor[2], topLeftColor[3]);

    sourceData->pushBack(v1);
    sourceData->pushBack(v2);
    sourceData->pushBack(v3);
    sourceData->pushBack(v4);

    // Triangle cells.
    vesPrimitive::Ptr triangles (new vesPrimitive());
    vesSharedPtr< vesIndices<unsigned short> > indices (new vesIndices<unsigned short>());
    indices->pushBackIndices(0, 3, 2);
    indices->pushBackIndices(1, 0, 2);
    triangles->setVesIndices(indices);
    triangles->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    triangles->setIndexCount(3);
    triangles->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedShort);

    geometryData->setName("PlaneGeometryData");
    geometryData->addSource(sourceData);
    geometryData->addPrimitive(triangles);

    return geometryData;
  }

  void toggleVisibility()
  {
    this->m_actor->setVisible(!this->m_actor->isVisible());
    this->render();
  }


  void toggleColorVisibility()
  {
    //this->m_material->enableVertexColor(!this->m_material->isEnabledVertexColor());
    this->render();
  }


private:
  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesGeometryData> m_backgroundPlaneData;

  vesMaterial::Ptr m_material;
  vesMapper::Ptr m_mapper;
  vesActor::Ptr m_actor;

};

}; // end namespace

int main(int argc, char *argv[])
{
  vesTestDrawPlane testDrawPlane;

#ifdef VES_QNX
  vesQNXTestDriver<vesTestDrawPlane> testDriver(&testDrawPlane);
#elif defined (VES_HOST)
  vesX11TestDriver<vesTestDrawPlane> testDriver(&testDrawPlane);
#endif

  if (!testDrawPlane.init(argc, argv))
  {
    return -1;
    fprintf(stderr, "Looping\n");
  }

  testDriver.init();

  // render once
  testDrawPlane.resizeView(testDriver.width, testDriver.height);
  testDrawPlane.resetView();

  testDriver.render();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawPlane.isTesting()) {
    testDriver.start();
  }
  else {
    testPassed = testDrawPlane.doTesting();
  }

  testDrawPlane.finalizeTest();
  testDriver.finalize();

  return testPassed ? 0 : 1;
}
