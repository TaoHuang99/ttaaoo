# aux_source_directory(. XML_DIR)

add_library(XML create.cpp tinyxml2.cpp)

# target_include_directories(XML 
#         INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
#         )
