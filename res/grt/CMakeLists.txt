set(DATA_FILES 
    structs.app.xml
    structs.db.mgmt.xml
    structs.db.migration.xml
    structs.db.mssql.xml
    structs.db.mysql.xml
    structs.db.oracle.xml
    structs.db.query.xml
    structs.db.sybase.xml
    structs.db.xml
    structs.eer.xml
    structs.model.xml
    structs.meta.xml
    structs.studio.logical.xml
    structs.studio.model.reporting.xml
    structs.studio.model.xml
    structs.studio.physical.xml
    structs.studio.xml
    structs.xml
    structs.ui.xml
    structs.wrapper.xml
)

install(FILES ${DATA_FILES} DESTINATION ${WB_PACKAGE_SHARED_DIR}/grt)
