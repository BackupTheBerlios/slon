# find packages
SET (Boost_USE_MULTITHREADED  ON)
SET (Boost_USE_STATIC_RUNTIME OFF)
IF (WIN32)
    SET (Boost_USE_STATIC_LIBS ON)
    FIND_PACKAGE ( Boost 1.42.0 	REQUIRED	COMPONENTS signals thread system filesystem iostreams unit_test_framework )
    IF (SLON_ENGINE_USE_PYTHON)
        SET (Boost_USE_STATIC_LIBS OFF)
        FIND_PACKAGE ( Boost 1.42.0 REQUIRED	COMPONENTS python )
    ENDIF (SLON_ENGINE_USE_PYTHON)
ELSE (WIN32)
    SET (LIBS signals thread system filesystem iostreams unit_test_framework)
    IF (SLON_ENGINE_USE_PYTHON)
        LIST (APPEND LIBS python)
    ENDIF (SLON_ENGINE_USE_PYTHON)
    SET (Boost_USE_STATIC_LIBS OFF)
    FIND_PACKAGE ( Boost 1.42.0 REQUIRED COMPONENTS ${LIBS})
ENDIF (WIN32)

FIND_PACKAGE ( SDL              REQUIRED )
FIND_PACKAGE ( Doxygen )
FIND_PACKAGE ( Bullet  )
FIND_PACKAGE ( PythonInterp  )
FIND_PACKAGE ( PythonLibs  )
FIND_PACKAGE ( GCCXML  )

IF (NOT XMLPP_CONFIGURE_INTRUSIVE)
	FIND_PACKAGE ( XML++        REQUIRED )
ENDIF (NOT XMLPP_CONFIGURE_INTRUSIVE)

IF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)
	FIND_PACKAGE ( SimpleGL     REQUIRED )
ENDIF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)

# Pretty plots
FIND_PACKAGE ( Gnuplot )
