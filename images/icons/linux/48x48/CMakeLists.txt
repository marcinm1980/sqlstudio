set(THEME_DIR ${WB_INSTALL_SHARED_DIR}/icons/hicolor)
set(SIZE 48x48)

set(MIME_ICONS 
    mimetypes/application-vnd.mysql-studio-model.png
    mimetypes/application-vnd.mysql-studio-plugin.png
)

install(FILES ${MIME_ICONS} DESTINATION ${THEME_DIR}/${SIZE}/mimetypes)

set(APP_ICONS 
    apps/mysql-studio.png
)

install(FILES ${APP_ICONS} DESTINATION ${THEME_DIR}/${SIZE}/apps)
