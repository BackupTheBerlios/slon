INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( PHYSICS_HEADERS
	include/PyRigidBody.h
	include/PyCollisionObject.h
	include/PyCollisionShape.h
	include/PyConstraint.h
	include/PyPhysicsTransform.h
)

SET ( PHYSICS_SOURCES
	src/PyRigidBody.cpp
	src/PyCollisionObject.cpp
	src/PyCollisionShape.cpp
	src/PyConstraint.cpp
	src/PyPhysicsTransform.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${PHYSICS_HEADERS} )
SOURCE_GROUP( sources FILES     ${PHYSICS_SOURCES} )

ADD_LIBRARY( PyPhysics SHARED
    ${SOURCES}
    ${PHYSICS_HEADERS} 
    ${PHYSICS_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyPhysics "stdafx.h" "stdafx.cpp" )

SET_TARGET_PROPERTIES( PyPhysics PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "physics"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyPhysics
    ${TARGET_UNIX_NAME}
)

