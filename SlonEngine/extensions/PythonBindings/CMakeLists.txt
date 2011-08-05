# find boost python
FIND_PACKAGE ( Boost 1.42.0 REQUIRED COMPONENTS python)

# global parameters
INCLUDE_DIRECTORIES (
    include
)

# boost python special settings
ADD_DEFINITIONS (-DBOOST_PYTHON_STATIC_LIB)

SET ( PYBIND_SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( PYBIND_SCENE_SOURCES
	Scene/PyGroup.cpp
	Scene/PyMatrixTransform.cpp
	Scene/PyNode.cpp
	Scene/PyTransform.cpp
)

SOURCE_GROUP( "" FILES      ${PYBIND_SOURCES} )
SOURCE_GROUP( scene FILES   ${PYBIND_SCENE_SOURCES} )

ADD_LIBRARY( PythonBindings SHARED
    ${PYBIND_SOURCES}
    ${PYBIND_SCENE_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PythonBindings "stdafx.h" "stdafx.cpp" )

SET_TARGET_PROPERTIES( PythonBindings PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                    ${EXTENSIONS_PROJECT_GROUP}
)

TARGET_LINK_LIBRARIES( PythonBindings
    ${TARGET_UNIX_NAME}
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARY}
)

