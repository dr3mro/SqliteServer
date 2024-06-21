file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.0/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH
    SHA256=7b354f3a5976c4626c876850c93944e52c83ec59a159ae5de5be7983f0e17a2a)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
# Define the local cache directory for CPM
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm_cache")

# Ensure the cache directory exists
file(MAKE_DIRECTORY ${CPM_SOURCE_CACHE})

# Set the CPM_SOURCE_CACHE environment variable
set(ENV{CPM_SOURCE_CACHE} ${CPM_SOURCE_CACHE})
