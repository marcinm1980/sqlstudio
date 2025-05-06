if (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-rdynamic)
endif()

install(FILES ${studio_DATA_FILES} DESTINATION ${WB_PACKAGE_SHARED_DIR})

set(libdir          ${LIB_INSTALL_DIR})
set(utilities_cmd   "")
configure_file(studio.x.in studio.x)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/studio.x DESTINATION ${WBX_INSTALL_BIN_DIR})

include_directories(.
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/library/base
    ${PROJECT_SOURCE_DIR}/library/grt/src
    ${PROJECT_SOURCE_DIR}/backend
    ${PROJECT_SOURCE_DIR}/backend/wbpublic
    ${PROJECT_SOURCE_DIR}/frontend/linux/linux_utilities
    ${PROJECT_SOURCE_DIR}/library/forms
    ${PROJECT_SOURCE_DIR}/library/cdbc/src
    ${PROJECT_SOURCE_DIR}/library/forms/gtk
    ${PROJECT_SOURCE_DIR}/library/forms/gtk/src
    ${PROJECT_SOURCE_DIR}/ext
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/ext/scintilla/include
    ${PROJECT_SOURCE_DIR}/library/forms/mforms
    ${PROJECT_SOURCE_DIR}/library/mysql.canvas/src
    ${PROJECT_SOURCE_DIR}/library/parsers
    ${PROJECT_SOURCE_DIR}/MySqlStudio.X/ide/studio.ide
    ${PROJECT_SOURCE_DIR}/MySqlStudio.X
    ${PROJECT_SOURCE_DIR}/ext/antlr-runtime/include
    ${PROJECT_SOURCE_DIR}/ext/antlr-runtime
    ${PROJECT_SOURCE_DIR}/generated
    ${GTK3_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${LIBZIP_INCLUDE_DIRS}
    ${MYSQLCPPCONN_INCLUDE_DIRS}
    ${GNOME_KEYRING_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${MYSQLNG_INCLUDE_DIR}
    ${GRT_INCLUDE_DIRS}
)

SET(CMAKE_INSTALL_RPATH "${WB_INSTALL_LIB_DIR}")
add_executable(studio.x-bin
    main.cpp
    MainForm.cpp
    ApplicationController.cpp
    ../../backend/ApplicationBackend.cpp
    ../../backend/metaclasses.cpp
)



if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set_target_properties(studio.x-bin PROPERTIES COMPILE_FLAGS "-fPIE")
else()
  set_target_properties(studio.x-bin PROPERTIES COMPILE_FLAGS "-fPIE -pie")
endif()

if (KEYRING_DEFINITION)
	set_target_properties(studio.x-bin PROPERTIES
		COMPILE_DEFINITIONS "${KEYRING_DEFINITION}"
	)
endif()

target_link_libraries(studio.x-bin
    mforms
    wbbase
    wbscintilla
    mdcanvasgtk
    mdcanvas
    parsers
    wbpublic
    grt
    linux_utilities
    wb.ng.wbp
    sqlide
    ${X11_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${SIGC++_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${GNOME_KEYRING_LIBRARIES}
    ${GRT_LIBRARIES}
)

if(COMMAND cotire)
  set_target_properties(studio.x-bin PROPERTIES
    COTIRE_PREFIX_HEADER_IGNORE_PATH "${PRECOMPILED_HEADERS_EXCLUDE_PATHS}")
  cotire(mysql-studio-bin)
endif()

install(TARGETS studio.x-bin DESTINATION ${WBX_INSTALL_DIR_EXECUTABLE})
