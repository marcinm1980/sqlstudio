# WiX 7 build via the pinned SDK and NuGet extension packages.
# Called by MakeSetup.bat with a staged BIN_DIR distribution.
# AcceptEula is optional here: WiX also supports per-user EULA acceptance.
WIX_EULA_ARG=
!IFDEF WIX_ACCEPT_EULA
WIX_EULA_ARG=-p:AcceptEula=$(WIX_ACCEPT_EULA)
!ENDIF

all: sql_studio.msi

sql_studio.msi: SqlStudio.wixproj source\sql_studio.xml source\sql_studio_fragment.xml source\mysql_common_ui.xml MySQLCA\MySQLCA.csproj MySQLCA\CustomAction.cs MySQLCA\CustomAction.config
	dotnet build SqlStudio.wixproj -c Release -p:Edition=$(SETUP_TYPE) -p:ProductVersion=$(VERSION_DETAIL) -p:InstallerPlatform=$(ARCHITECTURE) -p:Platform=$(ARCHITECTURE) "-p:PayloadDirectory=$(BIN_DIR)" $(WIX_EULA_ARG)
	copy /y "bin\$(ARCHITECTURE)\$(SETUP_TYPE)\sql_studio.msi" sql_studio.msi > nul

clean:
	del sql_studio.msi 1> nul 2> nul
	del sql_studio*.xml 1> nul 2> nul
