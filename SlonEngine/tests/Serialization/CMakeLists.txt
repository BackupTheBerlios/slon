SET (TEST_NAME "Serialization")
    
ADD_EXECUTABLE( ${TEST_NAME} main.cpp )
TARGET_LINK_LIBRARIES( ${TEST_NAME}
    ${TARGET_UNIX_NAME}
	${Boost_LIBRARIES}
)

SET_TARGET_PROPERTIES( ${TEST_NAME} PROPERTIES
                       RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
                       FOLDER                   "Test"
)