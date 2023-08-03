cmake_minimum_required(VERSION 3.5)
project(mig_server CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(XML)

list(APPEND NEED_SOURCE
    ${CMAKE_SOURCE_DIR}/main.cc 
    ${CMAKE_SOURCE_DIR}/controllers/BT_BTCtrl.cc
    ${CMAKE_SOURCE_DIR}/controllers/container_service_migration.cc
    ${CMAKE_SOURCE_DIR}/sample_nodes/sai_node.cpp
    ${CMAKE_SOURCE_DIR}/XML/create.cpp)

add_executable(${PROJECT_NAME} ${NEED_SOURCE})

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines

find_package(Boost COMPONENTS coroutine QUIET)
find_package(Threads)
find_package(Curses QUIET)
find_package(cpr REQUIRED)


list(APPEND BEHAVIOR_TREE_EXTERNAL_LIBRARIES
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    ${Boost_LIBRARIES} 
    ${CURSES_LIBRARIES} )

find_library(BT_LIB behaviortree_cpp_v3 HINTS ${CMAKE_SOURCE_DIR}/lib)

find_package(Drogon CONFIG REQUIRED)
#huangtao

find_path(HIREDIS_HEADER hiredis)
find_library(HIREDIS_LIBRARY hiredis)

include_directories(${HIREDIS_HEADER})
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon cpr::cpr ${BT_LIB} ${BEHAVIOR_TREE_EXTERNAL_LIBRARIES} XML pthread etcd-cpp-api protobuf grpc++ grpc z cpprest ssl crypto ${HIREDIS_LIBRARY})  

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    # With C++14, use boost to support any, string_view and filesystem
    message(STATUS "use c++14")
    find_package(Boost 1.61.0 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(include INC_SRC)
aux_source_directory(lib LIB_SRC)
aux_source_directory(sample_nodes SAM_SRC)
aux_source_directory(XML XML_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_SOURCE_DIR}/controllers
                                   ${CMAKE_SOURCE_DIR}/include
                                   ${CMAKE_SOURCE_DIR}/sample_nodes
                                   ${CMAKE_SOURCE_DIR}/XML)
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC}
               ${INC_SRC}
               ${LIB_SRC}
               ${SAM_SRC}
               ${XML_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)
