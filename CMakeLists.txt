cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project(pair_view)

include(GNUInstallDirs)
# For some reason we have undefined symbol errors
# and "DSO missing from command-line errors".
# Not sure if this is the best solution.
find_package (Boost REQUIRED COMPONENTS system date_time)
find_package (srnp REQUIRED)

if(SRNP_FOUND)
  MESSAGE("Found srnp ${SRNP_VERSION} at ${SRNP_INCLUDE_DIRS}")
endif(SRNP_FOUND)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
 
qt4_wrap_ui(UISrcs pair_view_main_window.ui)
qt4_wrap_cpp(MOCSrcs pair_view_main_window.h)
 
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${SRNP_INCLUDE_DIRS})

add_executable(pair_view main.cpp pair_view_main_window.cpp ${MOCSrcs} ${UISrcs})
target_link_libraries(pair_view ${QT_LIBRARIES} ${SRNP_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS pair_view DESTINATION ${CMAKE_INSTALL_BINDIR})
