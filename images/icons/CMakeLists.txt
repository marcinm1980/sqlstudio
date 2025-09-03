set(IMAGE_FILES 
    comment.png
    grt.png
    history.png
    layers.png
    mini_edit.png
    mini_ok.png
    mini_error.png
    mini_notice.png
    mini_warning.png
    warning_icon.png
    properties.png
    schemata.png
    user_datatypes.png
    MySQLMySqlStudio-128.png
    MySQLMySqlStudio-16.png
    MySQLMySqlStudio-32.png
    MySQLMySqlStudio-48.png
)

install(FILES ${IMAGE_FILES} DESTINATION ${WB_PACKAGE_SHARED_DIR}/images)

add_subdirectory(linux)
