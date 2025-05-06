include_directories(.
    ${PROJECT_SOURCE_DIR}/ext/scintilla/include
    ${PROJECT_SOURCE_DIR}/generated
    ${PROJECT_SOURCE_DIR}/modules/
    ${PROJECT_SOURCE_DIR}/library/grt/src 
    ${PROJECT_SOURCE_DIR}/library/forms
    ${PROJECT_SOURCE_DIR}/library/mysql.canvas/src
    ${PROJECT_SOURCE_DIR}/backend/wbpublic
    ${PROJECT_SOURCE_DIR}/backend/wbprivate
    ${PROJECT_SOURCE_DIR}/library/parsers
    ${PROJECT_SOURCE_DIR}/library/cdbc/src
    ${PROJECT_SOURCE_DIR}/backend/wbpublic/objimpl
    ${ANTLR3C_INCLUDE_DIRS}
    ${SIGC++_INCLUDE_DIRS}
    ${GRT_INCLUDE_DIRS}
    ${MYSQLNG_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${MySQL_INCLUDE_DIRS}
    ${MYSQLCPPCONN_INCLUDE_DIRS}
)

add_library(wb.ng.wbp
 backend/ng_editor_container.cpp
 backend/ng_sheet.cpp
 backend/ng_session_handler.cpp
 backend/ng_sidebar.cpp
 backend/ng_ide.cpp
 backend/ng_recordset_view.cpp
 backend/endless_control.cpp
 editors/code_editor_base.cpp
 editors/mysql_code_editor.cpp
 editors/ecma_code_editor.cpp
)

add_dependencies(wb.ng.wbp
    parsers
)


target_link_libraries(wb.ng.wbp wbscintilla ${MySQL_LIBRARIES} ${MYSQLNG_LIBRARY} ${GRT_LIBRARIES} ${SIGC++_LIBRARIES})

if(COMMAND cotire)
set_target_properties(wb.ng.wbp PROPERTIES
        COTIRE_PREFIX_HEADER_IGNORE_PATH "${PRECOMPILED_HEADERS_EXCLUDE_PATHS}")
cotire(wb.ng.wbp)
endif()

set_target_properties(wb.ng.wbp PROPERTIES
                      PREFIX ""
                      VERSION   ${WB_VERSION}
                      SOVERSION ${WB_VERSION})

install(TARGETS wb.ng.wbp DESTINATION ${WBX_INSTALL_LIB_DIR})
