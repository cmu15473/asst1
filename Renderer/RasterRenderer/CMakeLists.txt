add_library(RasterRendererLib
   Rasterizer.h
   CommonTraceCollection.h
   Fragment.h
   FrameBuffer.h
   IRasterRenderer.h
   ModelResource.h
   Parallel.h
   PolygonClipper.h
   ProjectedTriangle.h
   RendererImplBase.h
   RenderState.h
   Shader.h
   Statistics.h
   targetver.h
   Tracing.h
   VertexBuffer.h
   FrameBuffer.cpp
   IRasterRenderer.cpp
   ModelResource.cpp
   NontiledForwardRenderer.cpp
   Shader.cpp
   Statistics.cpp
   TiledRenderer.cpp
)

target_link_libraries(RasterRendererLib CoreLib_Basic CoreLib_Imaging CoreLib_Graphics debug ${TBB_DEBUG} optimized ${TBB_RELEASE})
