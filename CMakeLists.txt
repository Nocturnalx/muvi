cmake_minimum_required(VERSION 3.0.0)
project(muvi VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set( project_sources
   src/main.cpp
   src/Visualiser.cpp
   src/AudioManager.cpp
   src/AudioProcessor.cpp
   src/FFTProcessor.cpp
   kissfft/kiss_fft.c
   kissfft/kiss_fftr.c
)

add_executable(${PROJECT_NAME} ${project_sources})

find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)

# Setup CMake, tell the compiler where to look for headers
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(EPOXY REQUIRED epoxy)

# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS} ${EPOXY_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${EPOXY_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER} ${EPOXY_CFLAGS_OTHER})

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${EPOXY_LIBRARIES} pulse Threads::Threads)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
