# list headers
SET ( TARGET_HEADER_PATH ${PROJECT_SOURCE_DIR}/slon )

SET ( TARGET_ROOT_HEADERS
    ${TARGET_HEADER_PATH}/Config.h
    ${TARGET_HEADER_PATH}/Engine.h
    ${TARGET_HEADER_PATH}/stdafx.h
)

SET ( TARGET_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Detail/Engine.h
)

SET ( TARGET_ANIMATION_HEADERS
    ${TARGET_HEADER_PATH}/Animation/Animation.h
    ${TARGET_HEADER_PATH}/Animation/AnimationController.h
    ${TARGET_HEADER_PATH}/Animation/AnimationTrack.h
    ${TARGET_HEADER_PATH}/Animation/Forward.h
)

SET ( TARGET_ANIMATION_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Animation/Detail/Animation.h
    ${TARGET_HEADER_PATH}/Animation/Detail/AnimationController.h
    ${TARGET_HEADER_PATH}/Animation/Detail/AnimationTrack.h
)

SET ( TARGET_DATABASE_HEADERS
    ${TARGET_HEADER_PATH}/Database/Archive.h
    ${TARGET_HEADER_PATH}/Database/Cache.h
    ${TARGET_HEADER_PATH}/Database/DatabaseManager.h
    ${TARGET_HEADER_PATH}/Database/Forward.h
    ${TARGET_HEADER_PATH}/Database/Library.h
    ${TARGET_HEADER_PATH}/Database/Loader.h
    ${TARGET_HEADER_PATH}/Database/NVP.h
    ${TARGET_HEADER_PATH}/Database/Saver.h
    ${TARGET_HEADER_PATH}/Database/Serializable.h
    ${TARGET_HEADER_PATH}/Database/Storage.h
)

SET ( TARGET_DATABASE_BULLET_HEADERS
    ${TARGET_HEADER_PATH}/Database/Bullet/Bullet.h
)

SET ( TARGET_DATABASE_PROPRIETARY_HEADERS
    ${TARGET_HEADER_PATH}/Database/Proprietary/SXMLLoader.h
    ${TARGET_HEADER_PATH}/Database/Proprietary/SXMLSaver.h
    ${TARGET_HEADER_PATH}/Database/Proprietary/SXMLArchive.h
)

SET ( TARGET_DATABASE_COLLADA_HEADERS
    ${TARGET_HEADER_PATH}/Database/Collada/Collada.h
    ${TARGET_HEADER_PATH}/Database/Collada/ColladaCommon.h
    ${TARGET_HEADER_PATH}/Database/Collada/ColladaCore.h
    ${TARGET_HEADER_PATH}/Database/Collada/ColladaFX.h
	${TARGET_HEADER_PATH}/Database/Collada/ColladaPhysics.h
)

SET ( TARGET_DATABASE_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Database/Detail/Cache.h
    ${TARGET_HEADER_PATH}/Database/Detail/DatabaseManager.h
    ${TARGET_HEADER_PATH}/Database/Detail/Library.h
    ${TARGET_HEADER_PATH}/Database/Detail/SGLSerialization.h
    ${TARGET_HEADER_PATH}/Database/Detail/UtilitySerialization.h
)

SET ( TARGET_INPUT_HEADERS
    ${TARGET_HEADER_PATH}/Input/InputManager.h
    ${TARGET_HEADER_PATH}/Input/InputHandler.h
    ${TARGET_HEADER_PATH}/Input/KeyboardHandler.h
    ${TARGET_HEADER_PATH}/Input/MouseHandler.h
)

SET ( TARGET_INPUT_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Input/Detail/InputManager.h
)

SET ( TARGET_FILESYSTEM_HEADERS
    ${TARGET_HEADER_PATH}/FileSystem/Directory.h
    ${TARGET_HEADER_PATH}/FileSystem/File.h
    ${TARGET_HEADER_PATH}/FileSystem/Forward.h
    ${TARGET_HEADER_PATH}/FileSystem/Node.h
    ${TARGET_HEADER_PATH}/FileSystem/FileSystemManager.h
)

SET ( TARGET_FILESYSTEM_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/FileSystem/Detail/FileSystemManager.h
)

SET ( TARGET_FILESYSTEM_NATIVE_HEADERS
    ${TARGET_HEADER_PATH}/FileSystem/Native/Directory.h
    ${TARGET_HEADER_PATH}/FileSystem/Native/File.h
    ${TARGET_HEADER_PATH}/FileSystem/Native/Node.h
)

SET ( TARGET_GRAPHICS_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Common.h
    ${TARGET_HEADER_PATH}/Graphics/Effect.h
    ${TARGET_HEADER_PATH}/Graphics/Filter.h   
    ${TARGET_HEADER_PATH}/Graphics/Forward.h  
    ${TARGET_HEADER_PATH}/Graphics/GraphicsManager.h
    ${TARGET_HEADER_PATH}/Graphics/Material.h
    ${TARGET_HEADER_PATH}/Graphics/ParameterBinding.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable.h
    ${TARGET_HEADER_PATH}/Graphics/Renderer.h
)

SET ( TARGET_GRAPHICS_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Detail/AttributeTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/GraphicsManager.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/ParameterTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/StateTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/UniformTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/Utility.h
)

SET ( TARGET_GRAPHICS_EFFECT_HEADERS
    #${TARGET_HEADER_PATH}/Graphics/Effect/ExtrudeEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/DebugEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/DebugTextEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/LightingEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/Pass.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/SkyBoxEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/TransformEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/WaterEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/FrequencySpectrum.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/PhillipsSpectrum.h
)

SET ( TARGET_GRAPHICS_EFFECT_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Effect/Detail/Effect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/Detail/EffectShaderProgram.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/Detail/FFPPass.h
    ${TARGET_HEADER_PATH}/Graphics/Effect/Detail/Pass.h
)

SET ( TARGET_GRAPHICS_MATERIAL_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Material/LightingMaterial.h
    ${TARGET_HEADER_PATH}/Graphics/Material/WaterMaterial.h
)

SET ( TARGET_GRAPHICS_GUI_HEADERS
    #${TARGET_HEADER_PATH}/Graphics/GUI/ProgressBar.h
)

SET ( TARGET_GRAPHICS_PASS_HEADERS
    #${TARGET_HEADER_PATH}/Graphics/Pass/AllRenderPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/AmbientPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/DebugPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/DepthPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/MultiLightPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/LightingPass.h
    #${TARGET_HEADER_PATH}/Graphics/Pass/ShadowVolumePass.h
)

SET ( TARGET_GRAPHICS_POST_PROCESS_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/PostProcess/Common.h
    ${TARGET_HEADER_PATH}/Graphics/PostProcess/FFTFilter.h
    ${TARGET_HEADER_PATH}/Graphics/PostProcess/FogFilter.h  
    ${TARGET_HEADER_PATH}/Graphics/PostProcess/Unit.h
)

SET ( TARGET_GRAPHICS_RENDERABLE_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Renderable/DebugMesh.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/Forward.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/Mesh.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/ProjectedGrid.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/SkyBox.h
    #${TARGET_HEADER_PATH}/Graphics/Renderable/ShadowVolumeMesh.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/SkinnedMesh.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/StaticMesh.h
)

SET ( TARGET_GRAPHICS_RENDERABLE_DEBUG_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Renderable/Debug/DebugDrawCommon.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable/Debug/DebugDrawSkinned.h
)

IF (SLON_ENGINE_USE_PHYSICS)
    LIST ( APPEND TARGET_GRAPHICS_RENDERABLE_DEBUG_HEADERS
    	${TARGET_HEADER_PATH}/Graphics/Renderable/Debug/DebugDrawPhysics.h
    )
ENDIF (SLON_ENGINE_USE_PHYSICS)

SET ( TARGET_GRAPHICS_RENDERER_HEADERS
    #${TARGET_HEADER_PATH}/Graphics/Renderer/DefferedRenderer.h
    ${TARGET_HEADER_PATH}/Graphics/Renderer/FixedPipelineRenderer.h
    ${TARGET_HEADER_PATH}/Graphics/Renderer/ForwardRenderer.h
)

SET ( TARGET_PHYSICS_HEADERS
    ${TARGET_HEADER_PATH}/Physics/Constraint.h
    ${TARGET_HEADER_PATH}/Physics/CollisionObject.h
    ${TARGET_HEADER_PATH}/Physics/CollisionShape.h
    ${TARGET_HEADER_PATH}/Physics/DynamicsWorld.h
    ${TARGET_HEADER_PATH}/Physics/Forward.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsManager.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsMaterial.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsModel.h
    ${TARGET_HEADER_PATH}/Physics/Motor.h
    ${TARGET_HEADER_PATH}/Physics/RigidBody.h
    ${TARGET_HEADER_PATH}/Physics/RigidBodyTransform.h
    ${TARGET_HEADER_PATH}/Physics/ServoMotor.h
    ${TARGET_HEADER_PATH}/Physics/SpringMotor.h
    ${TARGET_HEADER_PATH}/Physics/VelocityMotor.h
)

SET ( TARGET_PHYSICS_BULLET_HEADERS
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletCommon.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletConstraint.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletCollisionObject.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletDynamicsWorld.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletMotionState.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletPhysicsMaterial.h
#    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletPhysicsModel.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRigidBody.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalServoMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalSpringMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalVelocityMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletSolver.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletSolverCollector.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletTranslationalMotor.h
)

SET ( TARGET_PHYSICS_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Physics/Detail/PhysicsManager.h
)

SET ( TARGET_LOG_HEADERS
    ${TARGET_HEADER_PATH}/Log/Formatters.h
    ${TARGET_HEADER_PATH}/Log/Forward.h
    ${TARGET_HEADER_PATH}/Log/Logger.h
    ${TARGET_HEADER_PATH}/Log/LogManager.h
    ${TARGET_HEADER_PATH}/Log/LogVisitor.h
)

SET ( TARGET_LOG_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Log/Detail/Logger.h
    ${TARGET_HEADER_PATH}/Log/Detail/LogManager.h
    ${TARGET_HEADER_PATH}/Log/Detail/Stream.h
)

SET ( TARGET_REALM_HEADERS
    ${TARGET_HEADER_PATH}/Realm/BVHLocation.h
    ${TARGET_HEADER_PATH}/Realm/BVHLocationNode.h
    ${TARGET_HEADER_PATH}/Realm/EventVisitor.h 
    ${TARGET_HEADER_PATH}/Realm/DefaultWorld.h
    ${TARGET_HEADER_PATH}/Realm/Forward.h
    ${TARGET_HEADER_PATH}/Realm/Location.h
    ${TARGET_HEADER_PATH}/Realm/World.h
)

SET ( TARGET_REALM_DETAIL_HEADERS
)

SET ( TARGET_SCENE_HEADERS
    ${TARGET_HEADER_PATH}/Scene/AcceptVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Camera.h
    ${TARGET_HEADER_PATH}/Scene/Entity.h
    ${TARGET_HEADER_PATH}/Scene/Forward.h
    ${TARGET_HEADER_PATH}/Scene/Geode.h
    ${TARGET_HEADER_PATH}/Scene/Light.h
    ${TARGET_HEADER_PATH}/Scene/MatrixTransform.h
    ${TARGET_HEADER_PATH}/Scene/Node.h
    ${TARGET_HEADER_PATH}/Scene/Group.h
    ${TARGET_HEADER_PATH}/Scene/Joint.h
    ${TARGET_HEADER_PATH}/Scene/Skeleton.h
    #${TARGET_HEADER_PATH}/Scene/Switch.h
    ${TARGET_HEADER_PATH}/Scene/Transform.h
    ${TARGET_HEADER_PATH}/Scene/Visitable.h
)

SET ( TARGET_SCENE_CAMERA_HEADERS
    ${TARGET_HEADER_PATH}/Scene/Camera/CommonCamera.h
    ${TARGET_HEADER_PATH}/Scene/Camera/CopyCamera.h
    ${TARGET_HEADER_PATH}/Scene/Camera/LookAtCamera.h
    ${TARGET_HEADER_PATH}/Scene/Camera/ReflectCamera.h
    ${TARGET_HEADER_PATH}/Scene/Camera/RenderTargetCamera.h
    ${TARGET_HEADER_PATH}/Scene/Camera/SlaveCamera.h
)

SET ( TARGET_SCENE_LIGHT_HEADERS
    ${TARGET_HEADER_PATH}/Scene/Light/AmbientLight.h
    ${TARGET_HEADER_PATH}/Scene/Light/DirectionalLight.h
    ${TARGET_HEADER_PATH}/Scene/Light/PointLight.h
)

SET ( TARGET_SCENE_VISITORS_HEADERS
    ${TARGET_HEADER_PATH}/Scene/Visitors/BFSNodeVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/CullVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/DFSNodeVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/FilterVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/NodeVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/TransformVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Visitors/Traverse.hpp
)

SET ( TARGET_THREAD_HEADERS
    ${TARGET_HEADER_PATH}/Thread/Lock.h
    ${TARGET_HEADER_PATH}/Thread/StartStopTimer.h
    ${TARGET_HEADER_PATH}/Thread/ThreadManager.h
    ${TARGET_HEADER_PATH}/Thread/Timer.h
)

SET ( TARGET_THREAD_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Thread/Detail/ThreadManager.h
)

SET ( TARGET_UTILITY_HEADERS
    ${TARGET_HEADER_PATH}/Utility/base64.hpp
    ${TARGET_HEADER_PATH}/Utility/cached_value.hpp
    ${TARGET_HEADER_PATH}/Utility/callback.hpp
    ${TARGET_HEADER_PATH}/Utility/error.hpp
    ${TARGET_HEADER_PATH}/Utility/if_then_else.hpp
    ${TARGET_HEADER_PATH}/Utility/math.hpp
    ${TARGET_HEADER_PATH}/Utility/referenced.hpp
    ${TARGET_HEADER_PATH}/Utility/singleton.hpp
    ${TARGET_HEADER_PATH}/Utility/tmp_file.hpp
    ${TARGET_HEADER_PATH}/Utility/handle.hpp
    ${TARGET_HEADER_PATH}/Utility/hash_string.hpp
)

SET ( TARGET_UTILITY_ALGORITHM_HEADERS
    ${TARGET_HEADER_PATH}/Utility/Algorithm/aabb_tree.hpp
    ${TARGET_HEADER_PATH}/Utility/Algorithm/algorithm.hpp
    ${TARGET_HEADER_PATH}/Utility/Algorithm/prefix_tree.hpp
    ${TARGET_HEADER_PATH}/Utility/Algorithm/spatial_node.hpp
)

SET ( TARGET_UTILITY_MEMORY_HEADERS
    ${TARGET_HEADER_PATH}/Utility/Memory/aligned.hpp
    ${TARGET_HEADER_PATH}/Utility/Memory/aligned_allocator.hpp
    ${TARGET_HEADER_PATH}/Utility/Memory/block_allocator.hpp
    ${TARGET_HEADER_PATH}/Utility/Memory/debug_new.h
    ${TARGET_HEADER_PATH}/Utility/Memory/fast_mutex.h
    ${TARGET_HEADER_PATH}/Utility/Memory/small_object_allocator.hpp
    ${TARGET_HEADER_PATH}/Utility/Memory/static_assert.h
    ${TARGET_HEADER_PATH}/Utility/Memory/object_in_pool.hpp
    ${TARGET_HEADER_PATH}/Utility/Memory/object_pool.hpp
)

SET ( TARGET_UTILITY_URI_HEADERS
    ${TARGET_HEADER_PATH}/Utility/URI/uri.hpp
    ${TARGET_HEADER_PATH}/Utility/URI/file_uri.hpp
)

SET ( TARGET_UTILITY_PLOT_HEADERS
    ${TARGET_HEADER_PATH}/Utility/Plot/gnuplot.h
)

# group headers
SOURCE_GROUP( headers FILES                                ${TARGET_ROOT_HEADERS} )
SOURCE_GROUP( headers\\Animation FILES                     ${TARGET_ANIMATION_HEADERS} )
SOURCE_GROUP( headers\\Animation\\Detail FILES			   ${TARGET_ANIMATION_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Detail FILES                        ${TARGET_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Database FILES                      ${TARGET_DATABASE_HEADERS} )
SOURCE_GROUP( headers\\Database\\Bullet FILES              ${TARGET_DATABASE_BULLET_HEADERS} )
SOURCE_GROUP( headers\\Database\\Collada FILES             ${TARGET_DATABASE_COLLADA_HEADERS} )
SOURCE_GROUP( headers\\Database\\Detail FILES              ${TARGET_DATABASE_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Database\\Proprietary FILES         ${TARGET_DATABASE_PROPRIETARY_HEADERS} )
SOURCE_GROUP( headers\\FileSystem FILES                    ${TARGET_FILESYSTEM_HEADERS} )
SOURCE_GROUP( headers\\FileSystem\\Native FILES            ${TARGET_FILESYSTEM_NATIVE_HEADERS} )
SOURCE_GROUP( headers\\FileSystem\\Detail FILES            ${TARGET_FILESYSTEM_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Graphics FILES                      ${TARGET_GRAPHICS_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Detail FILES              ${TARGET_GRAPHICS_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Effect FILES              ${TARGET_GRAPHICS_EFFECT_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Effect\\Detail FILES	   ${TARGET_GRAPHICS_EFFECT_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\GUI FILES                 ${TARGET_GRAPHICS_GUI_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Material FILES            ${TARGET_GRAPHICS_MATERIAL_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Pass FILES                ${TARGET_GRAPHICS_PASS_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\PostProcess FILES         ${TARGET_GRAPHICS_POST_PROCESS_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Renderable FILES          ${TARGET_GRAPHICS_RENDERABLE_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Renderable\\Debug FILES   ${TARGET_GRAPHICS_RENDERABLE_DEBUG_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Renderer FILES            ${TARGET_GRAPHICS_RENDERER_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Log FILES                 ${TARGET_GRAPHICS_LOG_HEADERS} )
SOURCE_GROUP( headers\\Input FILES                         ${TARGET_INPUT_HEADERS} )
SOURCE_GROUP( headers\\Input\\Detail FILES                 ${TARGET_INPUT_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Log FILES                           ${TARGET_LOG_HEADERS} )
SOURCE_GROUP( headers\\Log\\Detail FILES                   ${TARGET_LOG_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Physics FILES                       ${TARGET_PHYSICS_HEADERS} )
SOURCE_GROUP( headers\\Physics\\Bullet FILES               ${TARGET_PHYSICS_BULLET_HEADERS} )
SOURCE_GROUP( headers\\Physics\\Detail FILES               ${TARGET_PHYSICS_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Realm FILES                         ${TARGET_REALM_HEADERS} )
SOURCE_GROUP( headers\\Realm\\Detail FILES                 ${TARGET_REALM_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Scene FILES                         ${TARGET_SCENE_HEADERS} )
SOURCE_GROUP( headers\\Scene\\Camera FILES                 ${TARGET_SCENE_CAMERA_HEADERS} )
SOURCE_GROUP( headers\\Scene\\Light FILES                  ${TARGET_SCENE_LIGHT_HEADERS} )
SOURCE_GROUP( headers\\Scene\\Visitors FILES			   ${TARGET_SCENE_VISITORS_HEADERS} )
SOURCE_GROUP( headers\\Thread FILES                        ${TARGET_THREAD_HEADERS} )
SOURCE_GROUP( headers\\Thread\\Detail FILES                ${TARGET_THREAD_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Utility FILES                       ${TARGET_UTILITY_HEADERS} )
SOURCE_GROUP( headers\\Utility\\Algorithm FILES            ${TARGET_UTILITY_ALGORITHM_HEADERS} )
SOURCE_GROUP( headers\\Utility\\Memory FILES               ${TARGET_UTILITY_MEMORY_HEADERS} )
SOURCE_GROUP( headers\\Utility\\Plot FILES                 ${TARGET_UTILITY_PLOT_HEADERS} )
SOURCE_GROUP( headers\\Utility\\URI FILES                  ${TARGET_UTILITY_URI_HEADERS} )

SET ( TARGET_HEADERS
    ${TARGET_ROOT_HEADERS}
    ${TARGET_ANIMATION_HEADERS}
    ${TARGET_ANIMATION_DETAIL_HEADERS}
    ${TARGET_DATABASE_HEADERS}
    ${TARGET_DATABASE_COLLADA_HEADERS}
    ${TARGET_DATABASE_DETAIL_HEADERS}
    ${TARGET_DATABASE_PROPRIETARY_HEADERS}
    ${TARGET_DETAIL_HEADERS}
    ${TARGET_FILESYSTEM_HEADERS}
    ${TARGET_FILESYSTEM_DETAIL_HEADERS}
    ${TARGET_FILESYSTEM_NATIVE_HEADERS}
    ${TARGET_GRAPHICS_HEADERS}
    ${TARGET_SCENE_CAMERA_HEADERS}
    ${TARGET_GRAPHICS_DETAIL_HEADERS}
    ${TARGET_GRAPHICS_EFFECT_HEADERS}
    ${TARGET_GRAPHICS_EFFECT_DETAIL_HEADERS}
    ${TARGET_GRAPHICS_MATERIAL_HEADERS}
    ${TARGET_GRAPHICS_GUI_HEADERS}
    ${TARGET_GRAPHICS_PASS_HEADERS}
    ${TARGET_GRAPHICS_POST_PROCESS_HEADERS}    
    ${TARGET_GRAPHICS_RENDERABLE_HEADERS}
    ${TARGET_GRAPHICS_RENDERABLE_DEBUG_HEADERS}
    ${TARGET_GRAPHICS_RENDERER_HEADERS}
    ${TARGET_GRAPHICS_WATER_HEADERS}
    ${TARGET_INPUT_HEADERS}
    ${TARGET_INPUT_DETAIL_HEADERS}
    ${TARGET_LOG_HEADERS}
    ${TARGET_LOG_DETAIL_HEADERS}
    ${TARGET_REALM_HEADERS}
    ${TARGET_REALM_DETAIL_HEADERS}
    ${TARGET_SCENE_LIGHT_HEADERS}
    ${TARGET_SCENE_HEADERS}
    ${TARGET_SCENE_VISITORS_HEADERS}
    ${TARGET_THREAD_HEADERS}
    ${TARGET_THREAD_DETAIL_HEADERS}
    ${TARGET_UTILITY_HEADERS}
    ${TARGET_UTILITY_ALGORITHM_HEADERS}
    ${TARGET_UTILITY_MEMORY_HEADERS}
    ${TARGET_UTILITY_URI_HEADERS}
)

IF (SLON_ENGINE_USE_BULLET)
    LIST ( APPEND TARGET_HEADERS
        ${TARGET_DATABASE_BULLET_HEADERS}
        ${TARGET_PHYSICS_HEADERS}
        ${TARGET_PHYSICS_BULLET_HEADERS}
        ${TARGET_PHYSICS_DETAIL_HEADERS}
    )
ENDIF (SLON_ENGINE_USE_BULLET)

IF (SLON_ENGINE_USE_GNUPLOT)
    LIST ( APPEND TARGET_HEADERS
        ${TARGET_UTILITY_PLOT_HEADERS} 
    )
ENDIF (SLON_ENGINE_USE_GNUPLOT)

# list sources
SET ( TARGET_SOURCES
    stdafx.cpp
)

SET ( TARGET_ANIMATION_SOURCES
)

SET ( TARGET_ANIMATION_DETAIL_SOURCES
    Animation/Detail/Animation.cpp
    Animation/Detail/AnimationController.cpp
    Animation/Detail/AnimationTrack.cpp
)

SET ( TARGET_DETAIL_SOURCES
    Detail/Engine.cpp
)

SET ( TARGET_DATABASE_SOURCES
)

SET ( TARGET_DATABASE_BULLET_SOURCES
    Database/Bullet/Bullet.cpp
)

SET ( TARGET_DATABASE_PROPRIETARY_SOURCES
    Database/Proprietary/SXMLArchive.cpp
    Database/Proprietary/SXMLLoader.cpp
    Database/Proprietary/SXMLSaver.cpp
)

SET ( TARGET_DATABASE_COLLADA_SOURCES
    Database/Collada/Collada.cpp
    Database/Collada/ColladaCore.cpp
    Database/Collada/ColladaFX.cpp
	Database/Collada/ColladaPhysics.cpp
)

SET ( TARGET_DATABASE_DETAIL_SOURCES
    Database/Detail/Cache.cpp
    Database/Detail/DatabaseManager.cpp
    Database/Detail/Library.cpp
    Database/Detail/SGLSerialization.cpp
    Database/Detail/UtilitySerialization.cpp
)

SET ( TARGET_FILESYSTEM_SOURCES
)

SET ( TARGET_FILESYSTEM_DETAIL_SOURCES
    FileSystem/Detail/FileSystemManager.cpp
)

SET ( TARGET_FILESYSTEM_NATIVE_SOURCES
    FileSystem/Native/Directory.cpp
    FileSystem/Native/File.cpp
    FileSystem/Native/Node.cpp
)

SET ( TARGET_INPUT_SOURCES
    Input/KeyboardHandler.cpp
    Input/MouseHandler.cpp
)

SET ( TARGET_INPUT_DETAIL_SOURCES
    Input/Detail/InputManager.cpp
)

SET ( TARGET_GRAPHICS_SOURCES
    Graphics/Common.cpp
)

SET ( TARGET_GRAPHICS_DETAIL_SOURCES
    Graphics/Detail/AttributeTable.cpp
    Graphics/Detail/ParameterTable.cpp
    Graphics/Detail/GraphicsManager.cpp
    Graphics/Detail/StateTable.cpp
    Graphics/Detail/UniformTable.cpp
    Graphics/Detail/Utility.cpp
)

SET ( TARGET_GRAPHICS_EFFECT_SOURCES
    #Graphics/Effect/ExtrudeEffect.cpp
    Graphics/Effect/DebugEffect.cpp
    Graphics/Effect/DebugTextEffect.cpp
    Graphics/Effect/LightingEffect.cpp
    Graphics/Effect/PhillipsSpectrum.cpp
    Graphics/Effect/SkyBoxEffect.cpp
    Graphics/Effect/TransformEffect.cpp
    Graphics/Effect/WaterEffect.cpp
)

SET ( TARGET_GRAPHICS_EFFECT_DETAIL_SOURCES
    Graphics/Effect/Detail/Effect.cpp
    Graphics/Effect/Detail/EffectShaderProgram.cpp
    Graphics/Effect/Detail/FFPPass.cpp
    Graphics/Effect/Detail/Pass.cpp
)

SET ( TARGET_GRAPHICS_MATERIAL_SOURCES
    Graphics/Material/LightingMaterial.cpp
    Graphics/Material/WaterMaterial.cpp
)

SET ( TARGET_GRAPHICS_GUI_SOURCES
    #Graphics/GUI/ProgressBar.cpp
)

SET ( TARGET_GRAPHICS_PASS_SOURCES
    #Graphics/Pass/AllRenderPass.cpp
    #Graphics/Pass/AmbientPass.cpp
    #Graphics/Pass/DebugPass.cpp
    #Graphics/Pass/DepthPass.cpp
    #Graphics/Pass/MultiLightPass.cpp
    #Graphics/Pass/LightingPass.cpp
    #Graphics/Pass/ShadowVolumePass.cpp
)

SET ( TARGET_GRAPHICS_POST_PROCESS_SOURCES
    Graphics/PostProcess/Common.cpp
    Graphics/PostProcess/FFTFilter.cpp
    Graphics/PostProcess/FogFilter.cpp
)

SET ( TARGET_GRAPHICS_RENDERABLE_SOURCES
    Graphics/Renderable/DebugMesh.cpp
	Graphics/Renderable/Mesh.cpp
    Graphics/Renderable/ProjectedGrid.cpp
    Graphics/Renderable/SkyBox.cpp
    #Graphics/Renderable/ShadowVolumeMesh.cpp
    Graphics/Renderable/StaticMesh.cpp
    Graphics/Renderable/SkinnedMesh.cpp
)

SET ( TARGET_GRAPHICS_RENDERABLE_DEBUG_SOURCES
    Graphics/Renderable/Debug/DebugDrawCommon.cpp
    Graphics/Renderable/Debug/DebugDrawSkinned.cpp
)

IF (SLON_ENGINE_USE_PHYSICS)
    LIST ( APPEND TARGET_GRAPHICS_RENDERABLE_DEBUG_SOURCES
        Graphics/Renderable/Debug/DebugDrawPhysics.cpp
    )
ENDIF (SLON_ENGINE_USE_PHYSICS)

SET ( TARGET_GRAPHICS_RENDERER_SOURCES
    #Graphics/Renderer/DefferedRenderer.cpp
    Graphics/Renderer/FixedPipelineRenderer.cpp
    Graphics/Renderer/ForwardRenderer.cpp
)

SET ( TARGET_LOG_SOURCES
    Log/LogVisitor.cpp
)

SET ( TARGET_LOG_DETAIL_SOURCES
    Log/Detail/Logger.cpp
    Log/Detail/LogManager.cpp
    Log/Detail/Stream.cpp
)

SET ( TARGET_PHYSICS_SOURCES
    Physics/CollisionShape.cpp
#    Physics/DynamicsWorld.cpp
#    Physics/PhysicsMaterial.cpp
    Physics/PhysicsModel.cpp
    Physics/RigidBodyTransform.cpp
#    Physics/RigidBody.cpp
)

SET ( TARGET_PHYSICS_BULLET_SOURCES
    Physics/Bullet/BulletConstraint.cpp
    Physics/Bullet/BulletCollisionObject.cpp
    Physics/Bullet/BulletDynamicsWorld.cpp
    Physics/Bullet/BulletMotionState.cpp
    Physics/Bullet/BulletPhysicsMaterial.cpp
#    Physics/Bullet/BulletPhysicsModel.cpp
    Physics/Bullet/BulletRigidBody.cpp
    Physics/Bullet/BulletRotationalMotor.cpp
    Physics/Bullet/BulletRotationalServoMotor.cpp
    Physics/Bullet/BulletRotationalSpringMotor.cpp
    Physics/Bullet/BulletRotationalVelocityMotor.cpp  
    Physics/Bullet/BulletSolverCollector.cpp
    Physics/Bullet/BulletTranslationalMotor.cpp
)

SET ( TARGET_PHYSICS_DETAIL_SOURCES
    Physics/Detail/PhysicsManager.cpp
)

SET ( TARGET_REALM_SOURCES
    Realm/BVHLocation.cpp
    Realm/BVHLocationNode.cpp
    Realm/DefaultWorld.cpp
	Realm/EventVisitor.cpp
)

SET ( TARGET_REALM_DETAIL_SOURCES
)

SET ( TARGET_SCENE_SOURCES
    Scene/Entity.cpp
    Scene/Group.cpp
    Scene/Joint.cpp
    Scene/MatrixTransform.cpp
    Scene/Node.cpp
    Scene/Skeleton.cpp
    #Scene/Switch.cpp
    Scene/Transform.cpp
)

SET ( TARGET_SCENE_CAMERA_SOURCES
    Scene/Camera/CommonCamera.cpp
    Scene/Camera/CopyCamera.cpp
    Scene/Camera/LookAtCamera.cpp
    Scene/Camera/ReflectCamera.cpp
    Scene/Camera/RenderTargetCamera.cpp
    Scene/Camera/SlaveCamera.cpp
)

SET ( TARGET_SCENE_LIGHT_SOURCES
    Scene/Light/AmbientLight.cpp
    Scene/Light/DirectionalLight.cpp
    Scene/Light/PointLight.cpp
)

SET ( TARGET_SCENE_VISITORS_SOURCES
    Scene/Visitors/BFSNodeVisitor.cpp
    Scene/Visitors/CullVisitor.cpp
    Scene/Visitors/DFSNodeVisitor.cpp
    #Scene/Visitors/NodeVisitor.cpp
    Scene/Visitors/TransformVisitor.cpp
)

SET ( TARGET_THREAD_SOURCES
    Thread/StartStopTimer.cpp
    Thread/Utility.cpp
)

SET ( TARGET_THREAD_DETAIL_SOURCES
    Thread/Detail/ThreadManager.cpp
)

SET ( TARGET_UTILITY_SOURCES
	Utility/error.cpp
)

SET ( TARGET_UTILITY_MEMORY_SOURCES
	Utility/Memory/debug_new.cpp
)

SET ( TARGET_UTILITY_PLOT_SOURCES
	Utility/Plot/gnuplot.cpp
)

# group sources
SOURCE_GROUP( sources FILES                                ${TARGET_SOURCES} )
SOURCE_GROUP( sources\\Animation FILES                     ${TARGET_ANIMATION_SOURCES} )
SOURCE_GROUP( sources\\Animation\\Detail FILES             ${TARGET_ANIMATION_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Detail FILES                        ${TARGET_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Database FILES                      ${TARGET_DATABASE_SOURCES} )
SOURCE_GROUP( sources\\Database\\Bullet FILES              ${TARGET_DATABASE_BULLET_SOURCES} )
SOURCE_GROUP( sources\\Database\\Collada FILES             ${TARGET_DATABASE_COLLADA_SOURCES} )
SOURCE_GROUP( sources\\Database\\Detail FILES              ${TARGET_DATABASE_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Database\\Proprietary FILES         ${TARGET_DATABASE_PROPRIETARY_SOURCES} )
SOURCE_GROUP( sources\\FileSystem FILES                    ${TARGET_FILESYSTEM_SOURCES} )
SOURCE_GROUP( sources\\FileSystem\\Detail FILES            ${TARGET_FILESYSTEM_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\FileSystem\\Native FILES            ${TARGET_FILESYSTEM_NATIVE_SOURCES} )
SOURCE_GROUP( sources\\Input FILES                         ${TARGET_INPUT_SOURCES} )
SOURCE_GROUP( sources\\Input\\Detail FILES                 ${TARGET_INPUT_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Graphics FILES                      ${TARGET_GRAPHICS_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Detail FILES              ${TARGET_GRAPHICS_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Effect FILES              ${TARGET_GRAPHICS_EFFECT_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Effect\\Detail FILES	   ${TARGET_GRAPHICS_EFFECT_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\GUI FILES                 ${TARGET_GRAPHICS_GUI_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Material FILES            ${TARGET_GRAPHICS_MATERIAL_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Pass FILES                ${TARGET_GRAPHICS_PASS_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\PostProcess FILES         ${TARGET_GRAPHICS_POST_PROCESS_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Renderable FILES          ${TARGET_GRAPHICS_RENDERABLE_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Renderable\\Debug FILES   ${TARGET_GRAPHICS_RENDERABLE_DEBUG_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Renderer FILES            ${TARGET_GRAPHICS_RENDERER_SOURCES} )
SOURCE_GROUP( sources\\Log FILES                           ${TARGET_LOG_SOURCES} )
SOURCE_GROUP( sources\\Log\\Detail FILES                   ${TARGET_LOG_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Physics FILES                       ${TARGET_PHYSICS_SOURCES} )
SOURCE_GROUP( sources\\Physics\\Bullet FILES               ${TARGET_PHYSICS_BULLET_SOURCES} )
SOURCE_GROUP( sources\\Physics\\Detail FILES               ${TARGET_PHYSICS_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Realm FILES                         ${TARGET_REALM_SOURCES} )
SOURCE_GROUP( sources\\Realm\\Detail FILES                 ${TARGET_REALM_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Scene FILES                         ${TARGET_SCENE_SOURCES} )
SOURCE_GROUP( sources\\Scene\\Camera FILES                 ${TARGET_SCENE_CAMERA_SOURCES} )
SOURCE_GROUP( sources\\Scene\\Light FILES                  ${TARGET_SCENE_LIGHT_SOURCES} )
SOURCE_GROUP( sources\\Scene\\Visitors FILES               ${TARGET_SCENE_VISITORS_SOURCES} )
SOURCE_GROUP( sources\\Thread FILES                        ${TARGET_THREAD_SOURCES} )
SOURCE_GROUP( sources\\Thread\\Detail FILES                ${TARGET_THREAD_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Utility FILES                       ${TARGET_UTILITY_SOURCES} )
SOURCE_GROUP( sources\\Utility\\Memory FILES               ${TARGET_UTILITY_MEMORY_SOURCES} )
SOURCE_GROUP( sources\\Utility\\Plot FILES                 ${TARGET_UTILITY_PLOT_SOURCES} )

SET ( TARGET_SOURCES
    ${TARGET_SOURCES}
    ${TARGET_ANIMATION_SOURCES}
    ${TARGET_ANIMATION_DETAIL_SOURCES}
    ${TARGET_DETAIL_SOURCES}
    ${TARGET_DATABASE_SOURCES}
    ${TARGET_DATABASE_COLLADA_SOURCES}
	${TARGET_DATABASE_DETAIL_SOURCES}
    ${TARGET_DATABASE_PROPRIETARY_SOURCES}
    ${TARGET_FILESYSTEM_SOURCES}
    ${TARGET_FILESYSTEM_DETAIL_SOURCES}
    ${TARGET_FILESYSTEM_NATIVE_SOURCES}
    ${TARGET_INPUT_SOURCES}
    ${TARGET_INPUT_DETAIL_SOURCES} 
    ${TARGET_GRAPHICS_SOURCES}
    ${TARGET_GRAPHICS_DETAIL_SOURCES}
    ${TARGET_GRAPHICS_EFFECT_SOURCES}
    ${TARGET_GRAPHICS_EFFECT_DETAIL_SOURCES}
    ${TARGET_GRAPHICS_GUI_SOURCES}
    ${TARGET_GRAPHICS_MATERIAL_SOURCES}
    ${TARGET_GRAPHICS_PASS_SOURCES}
    ${TARGET_GRAPHICS_POST_PROCESS_SOURCES}
    ${TARGET_GRAPHICS_RENDERABLE_SOURCES}
    ${TARGET_GRAPHICS_RENDERABLE_DEBUG_SOURCES}
    ${TARGET_GRAPHICS_RENDERER_SOURCES}
    ${TARGET_GRAPHICS_WATER_SOURCES}
    ${TARGET_LOG_SOURCES}
    ${TARGET_LOG_DETAIL_SOURCES}
    ${TARGET_REALM_SOURCES}
    ${TARGET_REALM_DETAIL_SOURCES}
    ${TARGET_SCENE_SOURCES}
    ${TARGET_SCENE_CAMERA_SOURCES}
    ${TARGET_SCENE_LIGHT_SOURCES}
    ${TARGET_SCENE_VISITORS_SOURCES}
    ${TARGET_THREAD_SOURCES}
    ${TARGET_THREAD_DETAIL_SOURCES}
    ${TARGET_UTILITY_SOURCES}
	${TARGET_UTILITY_MEMORY_SOURCES}
)

IF (SLON_ENGINE_USE_BULLET)
    LIST ( APPEND TARGET_SOURCES
		${TARGET_DATABASE_BULLET_SOURCES}
        ${TARGET_PHYSICS_SOURCES}
        ${TARGET_PHYSICS_BULLET_SOURCES}
        ${TARGET_PHYSICS_DETAIL_SOURCES}
    )
ENDIF (SLON_ENGINE_USE_BULLET)

IF (SLON_ENGINE_USE_GNUPLOT)
    LIST ( APPEND TARGET_SOURCES
        ${TARGET_UTILITY_PLOT_SOURCES}
    )
ENDIF (SLON_ENGINE_USE_GNUPLOT)

# library target
ADD_LIBRARY ( ${TARGET_UNIX_NAME} STATIC ${TARGET_HEADERS} ${TARGET_SOURCES} ${TARGET_OPTIONS} )

SET_TARGET_PROPERTIES( ${TARGET_UNIX_NAME} PROPERTIES      
    FOLDER "${SLON_ENGINE_PROJECT_GROUP}"
)

# add precompiled headers
ADD_PRECOMPILED_HEADER( ${TARGET_UNIX_NAME} "stdafx.h" "stdafx.cpp" )

# link libraries
TARGET_LINK_LIBRARIES ( ${TARGET_UNIX_NAME}
    ${SDL_LIBRARY}
    ${OPENGL_LIBRARY}
    ${Boost_LIBRARIES}
)

IF (SLON_ENGINE_3RD_PARTY_FOUND)
    ADD_DEPENDENCIES( ${TARGET_UNIX_NAME} ${3RD_PARTY_TARGET} )
ENDIF (SLON_ENGINE_3RD_PARTY_FOUND)

IF (XMLPP_CONFIGURE_INTRUSIVE)
	ADD_DEPENDENCIES(${TARGET_UNIX_NAME} XML++)
	TARGET_LINK_LIBRARIES(${TARGET_UNIX_NAME} XML++)
ELSE (XMLPP_CONFIGURE_INTRUSIVE)
	TARGET_LINK_LIBRARIES(${TARGET_UNIX_NAME} ${XML++_LIBRARIES})
ENDIF (XMLPP_CONFIGURE_INTRUSIVE)

IF (SIMPLE_GL_CONFIGURE_INTRUSIVE)
	ADD_DEPENDENCIES(${TARGET_UNIX_NAME} SimpleGL)
	TARGET_LINK_LIBRARIES(${TARGET_UNIX_NAME} SimpleGL)
ELSE (SIMPLE_GL_CONFIGURE_INTRUSIVE)
	TARGET_LINK_LIBRARIES(${TARGET_UNIX_NAME} ${SimpleGL_LIBRARIES})
ENDIF (SIMPLE_GL_CONFIGURE_INTRUSIVE)

IF (SLON_ENGINE_USE_BULLET)
    TARGET_LINK_LIBRARIES ( ${TARGET_UNIX_NAME}
        ${Bullet_LIBRARIES}
    )
ENDIF (SLON_ENGINE_USE_BULLET)

SET( TARGET_PUBLIC_HEADERS ${TARGET_HEADERS} )

IF (NOT SLON_ENGINE_CONFIGURE_INTRUSIVE)
    SET_TARGET_PROPERTIES( ${TARGET_UNIX_NAME} PROPERTIES      
        FRAMEWORK       TRUE
        PUBLIC_HEADER   "${TARGET_ROOT_HEADERS}"
    )
ENDIF (NOT SLON_ENGINE_CONFIGURE_INTRUSIVE)

# install
IF (NOT SLON_ENGINE_CONFIGURE_INTRUSIVE)
	INSTALL (
		TARGETS         ${TARGET_UNIX_NAME}
		RUNTIME         DESTINATION ${INSTALL_BINDIR}
		LIBRARY         DESTINATION ${INSTALL_LIBDIR}
		ARCHIVE         DESTINATION ${INSTALL_ARCHIVEDIR}
		PUBLIC_HEADER   DESTINATION ${INSTALL_INCDIR}/slon
	)

	INSTALL (
		FILES       ${TARGET_ANIMATION_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Animation
	)

	INSTALL (
		FILES       ${TARGET_DATABASE_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Database
	)

	INSTALL (
		FILES       ${TARGET_DATABASE_COLLADA_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Database/Collada
	)

	INSTALL (
		FILES       ${TARGET_INPUT_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Input
	)

	INSTALL (
		FILES       ${TARGET_FILESYSTEM_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/FileSystem
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_DETAIL_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Detail
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_EFFECT_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Effect
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_EFFECT_DETAIL_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Effect/Detail
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_GUI_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/GUI
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_MATERIAL_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Material
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_PASS_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Pass
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_POST_PROCESS_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/PostProcess
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_RENDERABLE_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Renderable
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_RENDERABLE_DEBUG_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Renderable/Debug
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_RENDERER_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Renderer
	)

	INSTALL (
		FILES       ${TARGET_GRAPHICS_WATER_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Graphics/Water
	)

	INSTALL (
		FILES       ${TARGET_LOG_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Log
	)

	INSTALL (
		FILES       ${TARGET_REALM_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Realm
	)

	INSTALL (
		FILES       ${TARGET_REALM_LOCATION_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Realm/Location
	)

	INSTALL (
		FILES       ${TARGET_REALM_WORLD_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Realm/World
	)

	INSTALL (
		FILES       ${TARGET_SCENE_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Scene
	)

	INSTALL (
		FILES       ${TARGET_SCENE_CAMERA_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Scene/Camera
	)

	INSTALL (
		FILES       ${TARGET_SCENE_LIGHT_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Scene/Light
	)

	INSTALL (
		FILES       ${TARGET_SCENE_VISITORS_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Scene/Visitors
	)

	INSTALL (
		FILES       ${TARGET_THREAD_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Thread
	)

	INSTALL (
		FILES       ${TARGET_UTILITY_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Utility
	)

	INSTALL (
		FILES       ${TARGET_UTILITY_ALGORITHM_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Utility/Algorithm
	)

	INSTALL (
		FILES       ${TARGET_UTILITY_MEMORY_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Utility/Memory
	)
    
	INSTALL (
		FILES       ${TARGET_UTILITY_URI_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/slon/Utility/URI
	)
    
    IF (SLON_ENGINE_USE_GNUPLOT)
        INSTALL (
            FILES       ${TARGET_UTILITY_PLOT_HEADERS}
            DESTINATION ${INSTALL_INCDIR}/slon/Utility/Plot
        )
    ENDIF (SLON_ENGINE_USE_GNUPLOT)

	IF (SLON_ENGINE_USE_PHYSICS)
		INSTALL (
			FILES       ${TARGET_PHYSICS_HEADERS}
			DESTINATION ${INSTALL_INCDIR}/slon/Physics
		)

        IF (SLON_ENGINE_USE_BULLET)
            INSTALL (
                FILES       ${TARGET_PHYSICS_BULLET_HEADERS}
                DESTINATION ${INSTALL_INCDIR}/slon/Physics/Bullet
            )
        ENDIF (SLON_ENGINE_USE_BULLET)
        
		INSTALL (
			FILES       ${TARGET_SCENE_PHYSICS_HEADERS}
			DESTINATION ${INSTALL_INCDIR}/slon/Scene/Physics
		)

		INSTALL (
			FILES       ${TARGET_SCENE_PHYSICS_HEADERS}
			DESTINATION ${INSTALL_INCDIR}/slon/Scene/Visitors/Physics
		)
	ENDIF (SLON_ENGINE_USE_PHYSICS)
ENDIF (NOT SLON_ENGINE_CONFIGURE_INTRUSIVE)
