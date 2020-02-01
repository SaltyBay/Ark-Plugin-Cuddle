cmake_minimum_required (VERSION 3.8)

option(TEST "Test" OFF)

#SET (PATH_ARK_API "C:/Users/Matth/Documents/arkplugins/ark-api")



project (${ProjectName} )

message(${ProjectName})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lglapi")


if(TEST)

enable_testing() 

add_executable(${ProjectName})
target_link_libraries(${ProjectName} gtest_main)

add_test(NAME Database COMMAND Database_CreateTest)

add_subdirectory(test)

else()

add_library(${ProjectName} SHARED )


add_library(ark_api STATIC IMPORTED)
set_target_properties(ark_api PROPERTIES
  IMPORTED_LOCATION ${PATH_ARK_API}/out_lib/ArkApi.lib
  INTERFACE_INCLUDE_DIRECTORIES ${PATH_ARK_API}/version/Core/Public
)


set_target_properties(${ProjectName} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ProjectName} 
PRIVATE
	ark_api)


endif()

set_target_properties( ${ProjectName} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

ADD_DEFINITIONS(-D_UNICODE -DARK_GAME -D_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING)
add_subdirectory(src)





macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()
   

GroupSources(src)