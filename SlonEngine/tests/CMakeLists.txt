# boost 
FIND_PACKAGE ( Boost 1.45.0 REQUIRED COMPONENTS unit_test_framework )

# list here all test dirs
ADD_SUBDIRECTORY(Serialization)
