file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH
    SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
# Define the local cache directory for CPM
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm_cache")

# Ensure the cache directory exists
file(MAKE_DIRECTORY ${CPM_SOURCE_CACHE})

# Set the CPM_SOURCE_CACHE environment variable
set(ENV{CPM_SOURCE_CACHE} ${CPM_SOURCE_CACHE})
