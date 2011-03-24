CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (SlonEngine)
SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

IF (WIN32)
    # setup 3rd party directory as search path
    SET ( SLON_ENGINE_3RD_PARTY_FOUND FALSE )
    IF ( MSVC AND MSVC_VERSION EQUAL 1600 )
        SET ( CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/3rdParty/msvs_2010;${CMAKE_PREFIX_PATH}" )
        SET ( 3RD_PARTY_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdParty/msvs_2010 )
        SET ( SLON_ENGINE_3RD_PARTY_FOUND TRUE )
    ENDIF ( MSVC AND MSVC_VERSION EQUAL 1600 )
    
    IF (SLON_ENGINE_3RD_PARTY_FOUND)
        LIST ( APPEND CMAKE_INCLUDE_PATH 
            ${3RD_PARTY_DIRECTORY}/boost/ 
            ${3RD_PARTY_DIRECTORY}/BULLET_PHYSICS/include
            ${3RD_PARTY_DIRECTORY}/DevIL/include
            ${3RD_PARTY_DIRECTORY}/GLEW/include
            ${3RD_PARTY_DIRECTORY}/SDL/include
        )

        LIST ( APPEND CMAKE_LIBRARY_PATH 
            ${3RD_PARTY_DIRECTORY}/boost/lib
            ${3RD_PARTY_DIRECTORY}/BULLET_PHYSICS/lib
            ${3RD_PARTY_DIRECTORY}/DevIL/lib
            ${3RD_PARTY_DIRECTORY}/GLEW/lib
            ${3RD_PARTY_DIRECTORY}/SDL/lib
        )

        LIST ( APPEND CMAKE_PROGRAM_PATH
            ${3RD_PARTY_DIRECTORY}/gnuplot
        )

        # copy dlls into target binary dir
        ADD_CUSTOM_TARGET (CopyDLL)

        MACRO (COPY_DLLS SOURCE DESTINATION COPY_FILES)
            FOREACH(FILENAME ${${COPY_FILES}})
                ADD_CUSTOM_COMMAND( TARGET CopyDLL
                                    COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE}/${FILENAME}" "${DESTINATION}/${FILENAME}")
            ENDFOREACH(FILENAME)
        ENDMACRO (COPY_DLLS)

        SET (DEVIL_DLLS
            DevIL.dll
            ILU.dll
            ILUT.dll
            jpeg.dll
            libpng12-0.dll
            libtiff-3.dll
            zlib1.dll
        )

        SET (GLEW_DLLS
            glew32.dll
            glew32mx.dll
        )

        SET (SDL_DLLS
            SDL.dll
            SDL_image.dll
        )

        SET (RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        SET (3RD_PARTY_TARGET_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
        COPY_DLLS(${3RD_PARTY_DIRECTORY}/DevIL/lib  ${3RD_PARTY_TARGET_DIRECTORY}   DEVIL_DLLS)
        COPY_DLLS(${3RD_PARTY_DIRECTORY}/GLEW/bin   ${3RD_PARTY_TARGET_DIRECTORY}   GLEW_DLLS)
        COPY_DLLS(${3RD_PARTY_DIRECTORY}/SDL/bin    ${3RD_PARTY_TARGET_DIRECTORY}   SDL_DLLS)
            
        SET (3RD_PARTY_TARGET CopyDLL)
    ENDIF (SLON_ENGINE_3RD_PARTY_FOUND)
ENDIF (WIN32)

SET (SIMPLE_GL_CONFIGURE_INTRUSIVE 	ON)	
SET (SIMPLE_GL_PATH					${PROJECT_SOURCE_DIR}/SimpleGL)
SET (SIMPLE_GL_PROJECT_GROUP        Lib)
SET (XMLPP_CONFIGURE_INTRUSIVE 		ON)
SET (XMLPP_PATH						${PROJECT_SOURCE_DIR}/XML++)
SET (XMLPP_PROJECT_GORUP            Lib)

ADD_SUBDIRECTORY(SimpleGL)
ADD_SUBDIRECTORY(XML++)
ADD_SUBDIRECTORY(SlonEngine)
      