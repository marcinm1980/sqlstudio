set(WORKBENCH_DATA_FILES
    res/wb.glade
    res/model_view.glade
    res/diagram_view.glade
    res/find.glade
    res/diagram_size_form.glade
    res/input_dialog.glade
    studio.css
)
install(FILES ${WORKBENCH_DATA_FILES} DESTINATION ${WB_PACKAGE_SHARED_DIR})

set(libdir          ${LIB_INSTALL_DIR})
set(utilities_cmd   "")
configure_file(mysql-studio.in mysql-studio @ONLY)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/mysql-studio DESTINATION ${WB_INSTALL_BIN_DIR})

add_executable(mysql-studio-bin
    main.cpp
    program.cpp
    main_form.cpp
    overview_panel.cpp
    model_panel.cpp
    model_diagram_panel.cpp
    properties_tree.cpp
    navigator_box.cpp
    documentation_box.cpp
    diagram_size_form.cpp
    sqlide_form.cpp
    sqlide_output_view.cpp
    sqlide_main.cpp
)

target_include_directories(mysql-studio-bin
  PRIVATE
    ${PROJECT_SOURCE_DIR}/generated
    ${PROJECT_SOURCE_DIR}/modules
)

target_include_directories(mysql-studio-bin
 SYSTEM
  PRIVATE
    ${GTKMM_INCLUDE_DIRS}
)


target_compile_options(linux_utilities PRIVATE $<$<CXX_COMPILER_ID:GNU>:-rdynamic>)
target_compile_options(mysql-studio-bin PRIVATE ${WB_CXXFLAGS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(mysql-studio-bin PUBLIC -fPIE)
else()
  target_compile_options(mysql-studio-bin PUBLIC -fPIE -pie)
endif()

if (KEYRING_DEFINITION)
  target_compile_definitions(mysql-studio-bin PRIVATE HAVE_LIBSECRET_KEYRING)
endif()

target_link_libraries(mysql-studio-bin
  PRIVATE
    mforms
    linux_utilities
    sqlide
    wbprivate
    wbpublic
    cdbc
    mdcanvasgtk
    mdcanvas
    parsers
    ${X11_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${GTKMM_LIBRARIES}
    ${GMODULE_LIBRARIES}
    ${PCRE_LIBRARIES}
    ${LIBZIP_LIBRARIES}
    ${MySQL_LIBRARIES}
)

if(BUILD_FOR_GCOV)
  target_link_libraries(mysql-studio-bin PRIVATE gcov)
endif()

if(COMMAND cotire)
  set_target_properties(mysql-studio-bin PROPERTIES
    COTIRE_PREFIX_HEADER_IGNORE_PATH "${PRECOMPILED_HEADERS_EXCLUDE_PATHS}")
  cotire(mysql-studio-bin)
endif()

install(TARGETS mysql-studio-bin DESTINATION ${WB_INSTALL_DIR_EXECUTABLE})
