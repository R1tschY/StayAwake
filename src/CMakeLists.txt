
# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS filesystem locale system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# config.h
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${PROJECT_BINARY_DIR}")

# sources
file(GLOB stayawake_SOURCES "*.cpp")
file(GLOB stayawake_RESOURCES "*.rc")
file(GLOB stayawake_PO_FILES "../po/*.po")

# exe
add_executable(stayawake WIN32 ${stayawake_SOURCES} ${stayawake_RESOURCES})
target_link_libraries(stayawake PRIVATE 
	cpp-utils 
	lightports-base lightports-extra lightports-controls 
        user32 comctl32
)
set_target_properties(stayawake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)

# gettext
find_package(Gettext REQUIRED)
find_program(XGETTEXT_EXE xgettext)

set(PROJECT_POT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pot")
message(STATUS "Pot file: ${PROJECT_POT_FILE}")
add_custom_command(OUTPUT ${PROJECT_POT_FILE}
    COMMAND ${XGETTEXT_EXE} -o ${PROJECT_POT_FILE} ${stayawake_SOURCES}
              -c++
              --keyword=gettext:1 --keyword=pgettext:1c,2
              --keyword=ngettext:1,2 --keyword=npgettext:1c,2,3
              --keyword=_:1 --keyword=P_:1c,2
              --keyword=N_:1,2 --keyword=NP_:1c,2,3
              --package-name=${PROJECT_NAME} --package-name=${PROJECT_VERSION}
    DEPENDS ${stayawake_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Extract translatable messages to ${PROJECT_POT_FILE}"
)
add_custom_target(pot_file DEPENDS ${PROJECT_POT_FILE})

foreach(_current_PO_FILE ${stayawake_PO_FILES})
   get_filename_component(_name ${_current_PO_FILE} NAME)
   string(REGEX REPLACE "^(.+)(\\.[^.]+)$" "\\1" _lang ${_name})
   set(_mo_file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/locale/${_lang}/LC_MESSAGES/${PROJECT_NAME}.mo)
   add_custom_command(OUTPUT ${_mo_file}
       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/locale/${_lang}/LC_MESSAGES
       COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_mo_file} ${_current_PO_FILE}
       WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
       DEPENDS ${_current_PO_FILE}
   )
   list(APPEND _mo_files ${_mo_file})
endforeach()
add_custom_target(pofiles ALL DEPENDS ${_mo_files})
