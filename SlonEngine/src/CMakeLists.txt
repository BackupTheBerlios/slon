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
    ${TARGET_HEADER_PATH}/FileSystem/Detail/NativeDirectory.h
    ${TARGET_HEADER_PATH}/FileSystem/Detail/NativeFile.h
    ${TARGET_HEADER_PATH}/FileSystem/Detail/NativeNode.h
)

SET ( TARGET_GRAPHICS_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Common.h
    ${TARGET_HEADER_PATH}/Graphics/DebugDrawCommon.h
    ${TARGET_HEADER_PATH}/Graphics/DebugDrawSkinned.h
    ${TARGET_HEADER_PATH}/Graphics/DebugEffect.h
    ${TARGET_HEADER_PATH}/Graphics/DebugMesh.h
    ${TARGET_HEADER_PATH}/Graphics/DebugTextEffect.h
    ${TARGET_HEADER_PATH}/Graphics/Effect.h
    ${TARGET_HEADER_PATH}/Graphics/FFTFilter.h  
    ${TARGET_HEADER_PATH}/Graphics/FixedPipelineRenderer.h
    ${TARGET_HEADER_PATH}/Graphics/FogFilter.h  
    ${TARGET_HEADER_PATH}/Graphics/Forward.h  
    ${TARGET_HEADER_PATH}/Graphics/ForwardRenderer.h
    ${TARGET_HEADER_PATH}/Graphics/FrequencySpectrum.h
    ${TARGET_HEADER_PATH}/Graphics/GraphicsManager.h
    ${TARGET_HEADER_PATH}/Graphics/LightingEffect.h
    ${TARGET_HEADER_PATH}/Graphics/LightingMaterial.h
    ${TARGET_HEADER_PATH}/Graphics/Material.h
    ${TARGET_HEADER_PATH}/Graphics/Mesh.h
    ${TARGET_HEADER_PATH}/Graphics/ParameterBinding.h
    ${TARGET_HEADER_PATH}/Graphics/Pass.h
    ${TARGET_HEADER_PATH}/Graphics/PhillipsSpectrum.h
    ${TARGET_HEADER_PATH}/Graphics/PostProcessFilter.h 
	${TARGET_HEADER_PATH}/Graphics/PostProcessUnit.h
    ${TARGET_HEADER_PATH}/Graphics/ProjectedGrid.h
    ${TARGET_HEADER_PATH}/Graphics/Renderable.h
    ${TARGET_HEADER_PATH}/Graphics/Renderer.h
    ${TARGET_HEADER_PATH}/Graphics/SkinnedMesh.h
    ${TARGET_HEADER_PATH}/Graphics/SkyBox.h
	${TARGET_HEADER_PATH}/Graphics/SkyBoxEffect.h
    ${TARGET_HEADER_PATH}/Graphics/StaticMesh.h
    ${TARGET_HEADER_PATH}/Graphics/TransformEffect.h
    ${TARGET_HEADER_PATH}/Graphics/WaterEffect.h
    ${TARGET_HEADER_PATH}/Graphics/WaterMaterial.h
)

SET ( TARGET_GRAPHICS_DETAIL_HEADERS
    ${TARGET_HEADER_PATH}/Graphics/Detail/AttributeTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/Effect.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/EffectShaderProgram.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/FFPPass.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/GraphicsManager.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/ParameterTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/Pass.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/StateTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/UniformTable.h
    ${TARGET_HEADER_PATH}/Graphics/Detail/Utility.h
)

IF (SLON_ENGINE_USE_PHYSICS)
    LIST ( APPEND TARGET_GRAPHICS_HEADERS
    	${TARGET_HEADER_PATH}/Graphics/DebugDrawPhysics.h
    )
ENDIF (SLON_ENGINE_USE_PHYSICS)

SET ( TARGET_PHYSICS_HEADERS
    ${TARGET_HEADER_PATH}/Physics/Constraint.h
    ${TARGET_HEADER_PATH}/Physics/CollisionObject.h
    ${TARGET_HEADER_PATH}/Physics/CollisionShape.h
    ${TARGET_HEADER_PATH}/Physics/ConstraintNode.h
    ${TARGET_HEADER_PATH}/Physics/DynamicsWorld.h
    ${TARGET_HEADER_PATH}/Physics/Forward.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsManager.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsMaterial.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsModel.h
    ${TARGET_HEADER_PATH}/Physics/Motor.h
    ${TARGET_HEADER_PATH}/Physics/RigidBody.h
    ${TARGET_HEADER_PATH}/Physics/PhysicsTransform.h
    ${TARGET_HEADER_PATH}/Physics/ServoMotor.h
    ${TARGET_HEADER_PATH}/Physics/SpringMotor.h
    ${TARGET_HEADER_PATH}/Physics/VelocityMotor.h
)

SET ( TARGET_PHYSICS_BULLET_HEADERS
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletCommon.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletConstraint.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletCollisionObject.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletDynamicsWorld.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletPhysicsMaterial.h
#    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletPhysicsModel.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRigidBody.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalServoMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalSpringMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletRotationalVelocityMotor.h
    ${TARGET_HEADER_PATH}/Physics/Bullet/BulletSolver.h
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
    ${TARGET_HEADER_PATH}/Scene/AcceptVisitor.hpp
    ${TARGET_HEADER_PATH}/Scene/AmbientLight.h
    ${TARGET_HEADER_PATH}/Scene/Camera.h
    ${TARGET_HEADER_PATH}/Scene/CommonCamera.h
    ${TARGET_HEADER_PATH}/Scene/CullVisitor.h
    ${TARGET_HEADER_PATH}/Scene/DirectionalLight.h
    ${TARGET_HEADER_PATH}/Scene/Entity.h
    ${TARGET_HEADER_PATH}/Scene/Forward.h
    ${TARGET_HEADER_PATH}/Scene/FilterVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Light.h
    ${TARGET_HEADER_PATH}/Scene/LookAtCamera.h
    ${TARGET_HEADER_PATH}/Scene/MatrixTransform.h
    ${TARGET_HEADER_PATH}/Scene/Node.h
    ${TARGET_HEADER_PATH}/Scene/PointLight.h
    ${TARGET_HEADER_PATH}/Scene/Group.h
    ${TARGET_HEADER_PATH}/Scene/Joint.h
    ${TARGET_HEADER_PATH}/Scene/ReflectCamera.h
    ${TARGET_HEADER_PATH}/Scene/Skeleton.h
    ${TARGET_HEADER_PATH}/Scene/SlaveCamera.h
    ${TARGET_HEADER_PATH}/Scene/Transform.h
    ${TARGET_HEADER_PATH}/Scene/TransformVisitor.h
    ${TARGET_HEADER_PATH}/Scene/Traverse.hpp
    ${TARGET_HEADER_PATH}/Scene/Visitor.h
    ${TARGET_HEADER_PATH}/Scene/VisitorImpl.hpp
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
    ${TARGET_HEADER_PATH}/Utility/connection.hpp
    ${TARGET_HEADER_PATH}/Utility/error.hpp
    ${TARGET_HEADER_PATH}/Utility/if_then_else.hpp
    ${TARGET_HEADER_PATH}/Utility/math.hpp
    ${TARGET_HEADER_PATH}/Utility/function.hpp
    ${TARGET_HEADER_PATH}/Utility/functor_slot.hpp
    ${TARGET_HEADER_PATH}/Utility/referenced.hpp
    ${TARGET_HEADER_PATH}/Utility/signal.hpp
    ${TARGET_HEADER_PATH}/Utility/singleton.hpp
    ${TARGET_HEADER_PATH}/Utility/slot.hpp
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
SOURCE_GROUP( headers\\FileSystem\\Detail FILES            ${TARGET_FILESYSTEM_DETAIL_HEADERS} )
SOURCE_GROUP( headers\\Graphics FILES                      ${TARGET_GRAPHICS_HEADERS} )
SOURCE_GROUP( headers\\Graphics\\Detail FILES              ${TARGET_GRAPHICS_DETAIL_HEADERS} )
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
    ${TARGET_GRAPHICS_HEADERS}
    ${TARGET_GRAPHICS_DETAIL_HEADERS}
    ${TARGET_INPUT_HEADERS}
    ${TARGET_INPUT_DETAIL_HEADERS}
    ${TARGET_LOG_HEADERS}
    ${TARGET_LOG_DETAIL_HEADERS}
    ${TARGET_REALM_HEADERS}
    ${TARGET_REALM_DETAIL_HEADERS}
    ${TARGET_SCENE_HEADERS}
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
    FileSystem/Detail/NativeDirectory.cpp
    FileSystem/Detail/NativeFile.cpp
    FileSystem/Detail/NativeNode.cpp
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
    Graphics/DebugDrawCommon.cpp
    Graphics/DebugDrawSkinned.cpp
    Graphics/DebugEffect.cpp
    Graphics/DebugMesh.cpp
    Graphics/DebugTextEffect.cpp
    Graphics/FFTFilter.cpp
    Graphics/FixedPipelineRenderer.cpp
    Graphics/FogFilter.cpp
    Graphics/ForwardRenderer.cpp
    Graphics/LightingEffect.cpp
    Graphics/LightingMaterial.cpp
	Graphics/Mesh.cpp
    Graphics/PhillipsSpectrum.cpp
    Graphics/PostProcessCommon.cpp
    Graphics/ProjectedGrid.cpp
    Graphics/SkinnedMesh.cpp
    Graphics/SkyBox.cpp
    Graphics/SkyBoxEffect.cpp
    Graphics/StaticMesh.cpp
    Graphics/TransformEffect.cpp
    Graphics/WaterEffect.cpp
    Graphics/WaterMaterial.cpp
)

SET ( TARGET_GRAPHICS_DETAIL_SOURCES
    Graphics/Detail/AttributeTable.cpp
    Graphics/Detail/Effect.cpp
    Graphics/Detail/EffectShaderProgram.cpp
    Graphics/Detail/FFPPass.cpp
    Graphics/Detail/GraphicsManager.cpp
    Graphics/Detail/ParameterTable.cpp
    Graphics/Detail/StateTable.cpp
    Graphics/Detail/UniformTable.cpp
    Graphics/Detail/Utility.cpp
    Graphics/Detail/Pass.cpp
)

IF (SLON_ENGINE_USE_PHYSICS)
    LIST ( APPEND TARGET_GRAPHICS_SOURCES
        Graphics/DebugDrawPhysics.cpp
    )
ENDIF (SLON_ENGINE_USE_PHYSICS)

SET ( TARGET_LOG_SOURCES
    Log/LogVisitor.cpp
)

SET ( TARGET_LOG_DETAIL_SOURCES
    Log/Detail/Logger.cpp
    Log/Detail/LogManager.cpp
    Log/Detail/Stream.cpp
)

SET ( TARGET_PHYSICS_SOURCES
    Physics/CollisionObject.cpp
    Physics/CollisionShape.cpp
    Physics/Constraint.cpp
    Physics/ConstraintNode.cpp
    Physics/DynamicsWorld.cpp
    Physics/PhysicsMaterial.cpp
    Physics/PhysicsModel.cpp
    Physics/PhysicsTransform.cpp
    Physics/RigidBody.cpp
    Physics/ServoMotor.cpp
    Physics/SpringMotor.cpp
    Physics/VelocityMotor.cpp  
)

SET ( TARGET_PHYSICS_BULLET_SOURCES
    Physics/Bullet/BulletConstraint.cpp
    Physics/Bullet/BulletCollisionObject.cpp
    Physics/Bullet/BulletDynamicsWorld.cpp
    Physics/Bullet/BulletPhysicsMaterial.cpp
#    Physics/Bullet/BulletPhysicsModel.cpp
    Physics/Bullet/BulletRigidBody.cpp
    Physics/Bullet/BulletRotationalMotor.cpp
    Physics/Bullet/BulletRotationalServoMotor.cpp
    Physics/Bullet/BulletRotationalSpringMotor.cpp
    Physics/Bullet/BulletRotationalVelocityMotor.cpp  
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
	Scene/AmbientLight.cpp
    Scene/CullVisitor.cpp
	Scene/DirectionalLight.cpp
    Scene/Entity.cpp
    Scene/CommonCamera.cpp
    Scene/Group.cpp
    Scene/Joint.cpp
    Scene/LookAtCamera.cpp
    Scene/MatrixTransform.cpp
    Scene/Node.cpp
	Scene/PointLight.cpp
    Scene/ReflectCamera.cpp
    Scene/SlaveCamera.cpp
    Scene/Skeleton.cpp
    Scene/Transform.cpp
	Scene/TransformVisitor.cpp
	Scene/Visitor.cpp
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
SOURCE_GROUP( sources\\Input FILES                         ${TARGET_INPUT_SOURCES} )
SOURCE_GROUP( sources\\Input\\Detail FILES                 ${TARGET_INPUT_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Graphics FILES                      ${TARGET_GRAPHICS_SOURCES} )
SOURCE_GROUP( sources\\Graphics\\Detail FILES              ${TARGET_GRAPHICS_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Log FILES                           ${TARGET_LOG_SOURCES} )
SOURCE_GROUP( sources\\Log\\Detail FILES                   ${TARGET_LOG_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Physics FILES                       ${TARGET_PHYSICS_SOURCES} )
SOURCE_GROUP( sources\\Physics\\Bullet FILES               ${TARGET_PHYSICS_BULLET_SOURCES} )
SOURCE_GROUP( sources\\Physics\\Detail FILES               ${TARGET_PHYSICS_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Realm FILES                         ${TARGET_REALM_SOURCES} )
SOURCE_GROUP( sources\\Realm\\Detail FILES                 ${TARGET_REALM_DETAIL_SOURCES} )
SOURCE_GROUP( sources\\Scene FILES                         ${TARGET_SCENE_SOURCES} )
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
    ${TARGET_INPUT_SOURCES}
    ${TARGET_INPUT_DETAIL_SOURCES} 
    ${TARGET_GRAPHICS_SOURCES}
    ${TARGET_GRAPHICS_DETAIL_SOURCES}
    ${TARGET_LOG_SOURCES}
    ${TARGET_LOG_DETAIL_SOURCES}
    ${TARGET_REALM_SOURCES}
    ${TARGET_REALM_DETAIL_SOURCES}
    ${TARGET_SCENE_SOURCES}
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
