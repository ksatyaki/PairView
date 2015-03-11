cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project(PairView)

# For some reason we have undefined symbol errors
# and "DSO missing from command-line errors".
# Not sure if this is the best solution.
find_package (Boost REQUIRED COMPONENTS system date_time)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
 
qt4_wrap_ui(UISrcs pair_view_main_window.ui)
qt4_wrap_cpp(MOCSrcs pair_view_main_window.h)
 
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(PairView main.cpp pair_view_main_window.cpp ${MOCSrcs} ${UISrcs})
target_link_libraries(PairView ${QT_LIBRARIES} srnp_kernel ${Boost_LIBRARIES} srnp_print srnp_pair)