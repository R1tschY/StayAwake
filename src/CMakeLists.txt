
# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS filesystem locale system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})
message(STATUS "Boost include at ${Boost_INCLUDE_DIRS}")

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

# Generate pot-file from sources
# add_gettext_sources(domain_name
#                     OUTPUT file.pot
#                     SOURCES source1.c source2.cpp ...)
function(add_gettext_sources _domain_name)
    set(oneValueArgs OUTPUT)
    set(multiValueArgs SOURCES)
    set(prefix _add_gettext_sources)
    cmake_parse_arguments(${prefix} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(_pot_file ${${prefix}_OUTPUT} ABSOLUTE)

    set(_po_rela_SOURCES)
    foreach (_po_SOURCE ${${prefix}_SOURCES})
      file(RELATIVE_PATH _po_rela_SOURCE ${CMAKE_CURRENT_SOURCE_DIR} ${_po_SOURCE})
      set(_po_rela_SOURCES ${_po_rela_SOURCES} ${_po_rela_SOURCE})
    endforeach()

    add_custom_command(OUTPUT ${_pot_file}
        COMMAND ${XGETTEXT_EXE} -o ${_pot_file} ${_po_rela_SOURCES}
                  -c++
                  --keyword=gettext:1 --keyword=pgettext:1c,2
                  --keyword=ngettext:1,2 --keyword=npgettext:1c,2,3
                  --keyword=_:1 --keyword=P_:1c,2
                  --keyword=N_:1,2 --keyword=NP_:1c,2,3
                  --package-name=${PROJECT_NAME} --package-name=${PROJECT_VERSION}
        DEPENDS ${${prefix}_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Extract translatable messages to ${_pot_file}"
    )
    add_custom_target(${_domain_name}_pot ALL DEPENDS ${_pot_file})
endfunction()

# Generate mo-files from (updated) po-files.
# add_gettext_translations(domain_name
#                          [EXCLUDE_FROM_ALL]
#                          [UPDATE_FROM file.pot]
#                          OUTPUTDIR local_dir
#                          SOURCES lang1.po lang2.po ...)
function(add_gettext_translations _domain_name)
    # options
    set(_options EXCLUDE_FROM_ALL)
    set(oneValueArgs OUTPUTDIR UPDATE_FROM)
    set(multiValueArgs SOURCES)
    set(prefix _add_gettext_translations)
    cmake_parse_arguments(${prefix} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # iterate over po files
    foreach(_current_PO_FILE ${${prefix}_SOURCES})
        # get lang from po-file basename
        get_filename_component(_lang ${_current_PO_FILE} NAME_WE)

        if (${prefix}_UPDATE_FROM)
            # update po-file from pot-file and compile it to a mo-file
            add_custom_command(
                OUTPUT ${_current_PO_FILE}
                COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=t ${_current_PO_FILE} ${${prefix}_UPDATE_FROM}
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                DEPENDS ${${prefix}_UPDATE_FROM}
                COMMENT "Updating ${_current_PO_FILE}"
            )
        endif()

        # build destination folder
        set(_mo_file "${${prefix}_OUTPUTDIR}/${_lang}/LC_MESSAGES/${_domain_name}.mo")

        # compile po-file to a mo-file
        add_custom_command(
            OUTPUT ${_mo_file}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${${prefix}_OUTPUTDIR}/${_lang}/LC_MESSAGES"
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_mo_file} ${_current_PO_FILE}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS ${_current_PO_FILE}
        )

        # list all mo-files
        list(APPEND _mo_files ${_mo_file})
    endforeach()

    # create target
    if(${prefix}_EXCLUDE_FROM_ALL)
       add_custom_target(${_domain_name}_mo DEPENDS ${_mo_files})
    else()
       add_custom_target(${_domain_name}_mo ALL DEPENDS ${_mo_files})
    endif()
endfunction()




add_gettext_sources(
    ${PROJECT_NAME}
    SOURCES ${stayawake_SOURCES}
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/../po/${PROJECT_NAME}.pot"
)

add_gettext_translations(
    ${PROJECT_NAME}
    UPDATE_FROM "${CMAKE_CURRENT_SOURCE_DIR}/../po/${PROJECT_NAME}.pot"
    SOURCES  ${stayawake_PO_FILES}
    OUTPUTDIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/locale"
)


# install
install(TARGETS stayawake DESTINATION .)
install(DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/locale" DESTINATION .)

# installer
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/setup ${CMAKE_MODULE_PATH})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_EXECUTABLES "stayawake;StayAwake")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../GPL-3.0.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_MONOLITHIC_INSTALL ON)

set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/img/appicon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "stayawake.exe")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/R1tschY/StayAwake")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "stayawake.exe")

include(CPack)
