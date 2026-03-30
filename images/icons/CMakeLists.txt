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
    MySQLstudio-128.png
    MySQLstudio-16.png
    MySQLstudio-32.png
    MySQLstudio-48.png
)

install(FILES ${IMAGE_FILES} DESTINATION ${WB_PACKAGE_SHARED_DIR}/images)

add_subdirectory(linux)
