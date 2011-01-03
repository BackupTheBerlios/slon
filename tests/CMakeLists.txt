# vars
SET (TEST_DIR ${PROJECT_SOURCE_DIR}/tests)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

# list here all test dirs
ADD_SUBDIRECTORY(PrefixTree)
