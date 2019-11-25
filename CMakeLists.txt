cmake_minimum_required(VERSION 2.8)
project( CCDDrone)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
find_package( CFITSIO )

#find_package(sqlite3 REQUIRED)
#MESSAGE( STATUS "Found : ${SQLITE3_LIBRARIES}" )

SET(CMAKE_CXX_FLAGS "-std=gnu++1y -O2 -march=native -m64")


set(SOURCE
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachController.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachControllerExpose.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachControllerClkBiasMethods.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachControllerConfigHandler.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachControllerMiscHardwareProcedures.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachControllerTimingProcedures.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/FitsOps.cpp)
set(HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/picosha2.h
   ${CMAKE_CURRENT_SOURCE_DIR}/LeachController.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/CCDControlDataTypes.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/INIReader.h
   ${CMAKE_CURRENT_SOURCE_DIR}/UtilityFunctions.hpp)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/ArcAPI/CArcDevice/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ArcAPI/CArcDeinterlace/src
    ${CFITSIO_INCLUDE_DIR}
    )

if(UNIX AND NOT APPLE)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ArcAPI/Release/x64)
endif(UNIX AND NOT APPLE)

if(APPLE)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ArcAPI/Release/apple_x64)
SET(CMAKE_EXE_LINKER_FLAGS "-framework IOKit")
endif(APPLE)

add_library(LeachController SHARED ${SOURCE} ${HEADERS})
set_target_properties(LeachController PROPERTIES
        PUBLIC_HEADER LeachController.hpp)

#-lcurl seems to be required by fitsio!
add_executable( CCDDExpose CCDDExpose.cpp )
target_link_libraries( CCDDExpose -lCArcDeinterlace -lCArcDevice LeachController -lcurl ${CFITSIO_LIBRARIES})

add_executable( CCDDStartupAndErase CCDDStartupAndErase.cpp)
target_link_libraries( CCDDStartupAndErase -lCArcDeinterlace -lCArcDevice LeachController -lcurl ${CFITSIO_LIBRARIES})

add_executable( CCDDPerformEraseProcedure CCDDPerformEraseProcedure.cpp)
target_link_libraries( CCDDPerformEraseProcedure -lCArcDeinterlace -lCArcDevice LeachController -lcurl ${CFITSIO_LIBRARIES})

add_executable( CCDDToggleBias CCDDToggleBias.cpp)
target_link_libraries( CCDDToggleBias -lCArcDeinterlace -lCArcDevice LeachController -lcurl ${CFITSIO_LIBRARIES})

add_executable( CCDDApplyNewSettings CCDDApplyNewSettings.cpp)
target_link_libraries( CCDDApplyNewSettings -lCArcDeinterlace -lCArcDevice LeachController -lcurl ${CFITSIO_LIBRARIES} )

#add_executable( CCDDUnitTests CCDDUnitTests.cpp ${SOURCE} ${HEADERS})
#target_link_libraries( CCDDUnitTests -lCArcDeinterlace -lCArcDevice ${CFITSIO_LIBRARIES})
