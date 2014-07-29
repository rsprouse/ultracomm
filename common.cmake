
INCLUDE(TestCXXAcceptsFlag)

IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
    CMAKE_POLICY(SET CMP0003 NEW)
    CMAKE_POLICY(SET CMP0005 OLD)
    CMAKE_POLICY(SET CMP0011 OLD)
ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)

GET_FILENAME_COMPONENT(CURRENT_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)

SET(UCM_CMAKE_INCLUDE_PATH ${CURRENT_PATH})
LIST(APPEND CMAKE_MODULE_PATH ${UCM_CMAKE_INCLUDE_PATH})

SET(UNICODE_SUPPORT ON CACHE BOOL "Unicode Build")
SET(UMC_ENABLE_VERA OFF CACHE BOOL "Enable Project level Vera++ commands.")
SET(UMC_BUILD_SHARED_LIBS ON CACHE BOOL "Build Shared or Static libraries.")
SET(UMC_INHERIT_DEPENDENCIES ON CACHE BOOL "Enable Project Dependency Inheritance (reduces building time).")
SET(UMC_ENABLE_VISUALLEAKDETECTOR OFF CACHE BOOL "Enable the use of Visual Leak Detector (Hunting Memory Leaks?).")
SET(DEFAULT_SOURCE_MASKS *.cpp *.c *.def *.h *.rc *.rc2 *.asm *.nsi ${DEFAULT_RESOURCE_MASKS})
SET(BUILD_UNITTESTS ON CACHE BOOL "Build Unit Tests")
SET(BUILD_FACTORY_RELEASE OFF CACHE BOOL "Build Factory Release (BUILD_ULTRASONIX_RELEASE)")

SET(UMC_64BIT_WINDRIVER OFF CACHE BOOL "Building a 32bit executable on a 64bit system?")
SET(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "Don't build CMake files within project")

# Set Warning Level 4 (CMake default = /W3)
#STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#SET(CMAKE_CXX_WARNING_LEVEL 4)
# Set the 'Warnings as Errors' flag
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
# Enable Run-Time Type Information (RTTI) (CMake default = /GR)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR")

# Enable Edit and Continue for Debug builds
STRING(REGEX REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
# Enable Minimal Rebuilds
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm")

# Remove the explicit /STACK flag (CMake default = /stack:10000000)
STRING(REGEX REPLACE "/(stack|STACK):[0-9]* " "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
STRING(REGEX REPLACE "/(stack|STACK):[0-9]* " "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
STRING(REGEX REPLACE "/(stack|STACK):[0-9]* " "" CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LARGEADDRESSAWARE")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")

MARK_AS_ADVANCED(BUILD_FACTORY_RELEASE UNICODE_SUPPORT SUBVERSION_PATH NASM_ASSEMBLER CMAKE_INSTALL_PREFIX CMAKE_BACKWARDS_COMPATIBILITY CMAKE_SUPPRESS_REGENERATION UMC_BUILD_SHARED_LIBS UMC_INHERIT_DEPENDENCIES NSIS_EXECUTABLE)

IF(UMC_BUILD_SHARED_LIBS)
    SET(LIBS_BUILD_TYPE SHARED)
    ADD_DEFINITIONS("-DUMC_PLATFORM_EXPORT=__declspec(dllexport)")
    ADD_DEFINITIONS("-DUMC_PLATFORM_IMPORT=__declspec(dllimport)")
ELSE(UMC_BUILD_SHARED_LIBS)
    SET(LIBS_BUILD_TYPE STATIC)
    ADD_DEFINITIONS(-DUMC_PLATFORM_EXPORT=)
    ADD_DEFINITIONS(-DUMC_PLATFORM_IMPORT=)
ENDIF(UMC_BUILD_SHARED_LIBS)

IF(CMAKE_GENERATOR MATCHES "NMake")
    SET(DEFAULT_RESOURCE_MASKS)
ELSE(CMAKE_GENERATOR MATCHES "NMake")
    SET(DEFAULT_RESOURCE_MASKS res/*.rc2 res/*.bmp res/*.ico res/*.xml res/*.cur res/*.ani res/*.png res/*.wav)
ENDIF(CMAKE_GENERATOR MATCHES "NMake")

IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
    SET(UMC_PART_NUMBER "00.036.042" CACHE STRING
        "What is the Part Number of the primary software?\n    Sinoways: 00.036.180\n    SIUI (Apogee): 00.036.168\n    Ultrasonix (SONIX): 00.036.042")
ELSE(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
    # 2.4 doesn't support \n in description strings
    SET(UMC_PART_NUMBER "00.036.042" CACHE STRING
        "What is the Part Number of the primary software?    Sinoways: 00.036.180    SIUI (Apogee): 00.036.168    Ultrasonix (SONIX): 00.036.042")
ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)

IF(UMC_PART_NUMBER)
    STRING(REPLACE "." "_" UMC_PART_NUMBER_WITH_UNDERSCORES "${UMC_PART_NUMBER}")
    ADD_DEFINITIONS(-DBUILD_PARTNUMBER_${UMC_PART_NUMBER_WITH_UNDERSCORES})
ENDIF(UMC_PART_NUMBER)

IF(UMC_ENABLE_VISUALLEAKDETECTOR)
ADD_DEFINITIONS(-DUMC_ENABLE_VISUALLEAKDETECTOR)
ENDIF(UMC_ENABLE_VISUALLEAKDETECTOR)

ADD_DEFINITIONS(-DSTRICT)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_NON_CONFORMING_SWPRINTFS)
#ADD_DEFINITIONS(-DKERNEL_64BIT)
ADD_DEFINITIONS("-DPSAPI_VERSION=1")

# Add support for UNICODE
IF(UNICODE_SUPPORT)
    ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
ENDIF(UNICODE_SUPPORT)

# Build a Factory Release
IF(BUILD_FACTORY_RELEASE)
    ADD_DEFINITIONS(-DBUILD_ULTRASONIX_RELEASE)
ENDIF(BUILD_FACTORY_RELEASE)

IF(UMC_64BIT_WINDRIVER)
    ADD_DEFINITIONS(-DKERNEL_64BIT)
ENDIF(UMC_64BIT_WINDRIVER)

SET(ENVPOSTFIX_OPT_U "_u_vc10")
SET(ENVPOSTFIX_DEB_U "_ud_vc10")
SET(ENVPOSTFIX_OPT_A "_vc10")
SET(ENVPOSTFIX_DEB_A "_d_vc10")

IF(UNICODE_SUPPORT)
    SET(ENVPOSTFIX_OPT ${ENVPOSTFIX_OPT_U})
    SET(ENVPOSTFIX_DEB ${ENVPOSTFIX_DEB_U})
ELSE(UNICODE_SUPPORT)
    SET(ENVPOSTFIX_OPT ${ENVPOSTFIX_OPT_A})
    SET(ENVPOSTFIX_DEB ${ENVPOSTFIX_DEB_A})
ENDIF(UNICODE_SUPPORT)


# ============= MACRO =================
MACRO(INIT)
    CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
    INCLUDE_DIRECTORIES("../../inc" "../../..")    

    IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
        CMAKE_POLICY(SET CMP0003 NEW)
        CMAKE_POLICY(SET CMP0005 OLD)
        CMAKE_POLICY(SET CMP0011 OLD)
    ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)

    PROJECT(${ARGV0})

    REMOVE_MFC_SUPPORT() # don't propogate this setting to any SUBDIRS

    IF(BUILD_UNITTESTS AND IS_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
        SUBDIRS(tests)
    ENDIF(BUILD_UNITTESTS AND IS_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

    IF(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/sdk)
        SUBDIRS(sdk)
    ENDIF(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/sdk)
   
    SOURCE_GROUP("Assembly Files" ".asm$")
    SOURCE_GROUP("Resource Files" ".(wav|rc2|bmp|ico|xml|cur|ani|png)$")
    SOURCE_GROUP("Installer Files" ".nsi$")

	SET(QT_FILES)
    SET(USE_PCH_SUPPORT)
	
    FILE(GLOB SRCS ${DEFAULT_SOURCE_MASKS})
ENDMACRO(INIT)

MACRO(CREATE_LIBRARY)
    CREATE_PROJECT(1 0 ${ARGV0} ${ARGV1})
ENDMACRO(CREATE_LIBRARY)

MACRO(CREATE_CONSOLE_EXECUTABLE)
    CREATE_PROJECT(0 0 ${ARGV0} ${ARGV1})
ENDMACRO(CREATE_CONSOLE_EXECUTABLE)

MACRO(CREATE_EXECUTABLE)
    CREATE_PROJECT(0 1 ${ARGV0} ${ARGV1})
ENDMACRO(CREATE_EXECUTABLE)

MACRO(CREATE_QT_EXECUTABLE)
    CREATE_PROJECT()
ENDMACRO(CREATE_QT_EXECUTABLE)

MACRO(CREATE_PROJECT)
    IF(${ARGV2})
        SET(PROJECT_LIBS ${${ARGV2}})
    ELSE(${ARGV2})
        SET(PROJECT_LIBS ${LIBS})
    ENDIF(${ARGV2})

    SET(PROJECT_SRCS ${SRCS})
    
    SET(PROJECT_SRCS ${SRCS} ${QT_FILES})
    SET_SOURCE_FILES_PROPERTIES(${QT_QRC_SRCS} PROPERTIES COMPILE_FLAGS "/Y-")
    
    # every project should have an .rc file (for versioning); if a project doesn't then insert a basic .rc
    SET(PROJECT_HAS_RC FALSE)
    FOREACH(item ${PROJECT_SRCS})
        GET_FILENAME_COMPONENT(src_extension ${item} EXT)
        IF(${src_extension} STREQUAL ".rc")
            SET(PROJECT_HAS_RC TRUE)
        ENDIF(${src_extension} STREQUAL ".rc")
    ENDFOREACH(item ${PROJECT_SRCS})

    IF(NOT PROJECT_HAS_RC)
        LIST(APPEND PROJECT_SRCS ${UCM_CMAKE_INCLUDE_PATH}/default.rc)
    ENDIF(NOT PROJECT_HAS_RC)
    
    IF(UMC_ENABLE_VERA)
        FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/checkstyle.cmd "cd \"${PROJECT_SOURCE_DIR}\"\n")
        FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/checkstyle.cmd "\"${TOOLS_PATH}/vera++/vera++.bat\"")
        ADD_CUSTOM_COMMAND(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/checkstyle_output
            COMMAND ${TOOLS_PATH}/vera++/vera++.bat
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/checkstyle.cmd)

        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/checkstyle_output PROPERTIES SYMBOLIC TRUE)

        LIST(APPEND PROJECT_SRCS
            ${CMAKE_CURRENT_BINARY_DIR}/checkstyle.cmd
        )
    ENDIF(UMC_ENABLE_VERA)

    IF(PROJECTNAME_UPPERCASE)
        # remove the export definition of the parent project
        REMOVE_DEFINITIONS("-DEXPORT_${PROJECTNAME_UPPERCASE}_DLL=UMC_PLATFORM_EXPORT")
    ENDIF(PROJECTNAME_UPPERCASE)

    STRING(TOUPPER ${PROJECT_NAME} PROJECTNAME_UPPERCASE)
    
    IF(${ARGV0})
        ADD_DEFINITIONS("-DEXPORT_${PROJECTNAME_UPPERCASE}_DLL=UMC_PLATFORM_EXPORT")
        ADD_LIBRARY(${PROJECT_NAME} ${LIBS_BUILD_TYPE} ${PROJECT_SRCS})
    ELSE(${ARGV0})
        IF(${ARGV1})
            ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS})
        ELSE(${ARGV1})
            STRING(REGEX REPLACE "/entry:WinMainCRTStartup" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
            STRING(REGEX REPLACE "/entry:wWinMainCRTStartup" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")            
            ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
        ENDIF(${ARGV1})
    ENDIF(${ARGV0})

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
    
    GET_TARGET_PROPERTY(TARGET_LOCATION ${PROJECT_NAME} LOCATION)

    # temporarily replace the compiler with one that does support intrinsics
    GET_FILENAME_COMPONENT(INTRINSICS_COMPILER_PATH "${CMAKE_CXX_COMPILER}" PATH)
            
    SET(INTRINSICS_COMPILER ${INTRINSICS_COMPILER_PATH}/c2.dll)
    FILE(TO_NATIVE_PATH "${INTRINSICS_COMPILER}" INTRINSICS_COMPILER)

    SET(INTRINSICS_COMPILER_NEW ${UCM_CMAKE_INCLUDE_PATH}/../../c2.dll.13.0.9044.0)
    SET(INTRINSICS_COMPILER_OLD ${UCM_CMAKE_INCLUDE_PATH}/../../c2.dll.12.0.9782.0)

    FILE(TO_NATIVE_PATH "${INTRINSICS_COMPILER_NEW}" INTRINSICS_COMPILER_NEW)
    FILE(TO_NATIVE_PATH "${INTRINSICS_COMPILER_OLD}" INTRINSICS_COMPILER_OLD)

    SET(USER_FILE "${PROJECT_NAME}.vcxproj.user")
    SET(OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE}")	
    SET(ENV_TEMPLATE 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
    <PropertyGroup>
        <LocalDebuggerEnvironment>PATH=${QT_DOC_DIR}/../bin;${UCM_CMAKE_INCLUDE_PATH}/bin;%PATH%</LocalDebuggerEnvironment>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>")
    FILE(WRITE ${OUTPUT_PATH} "${ENV_TEMPLATE}")
ENDMACRO(CREATE_PROJECT)

# remove stuff to use MFC in this executable
MACRO(REMOVE_MFC_SUPPORT)
    REMOVE_DEFINITIONS(-D_AFXDLL)
    SET(CMAKE_MFC_FLAG 0)

    STRING(REGEX REPLACE "/entry:WinMainCRTStartup" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    STRING(REGEX REPLACE "/entry:wWinMainCRTStartup" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
ENDMACRO(REMOVE_MFC_SUPPORT)

MACRO(ASSEMBLE)
    SET(ASM_OBJECTS)
    FOREACH(file ${${ARGV0}})
        IF(${file} MATCHES ".*asm$")
            GET_FILENAME_COMPONENT(OUTPUT_FILENAME "${file}" NAME_WE)
            GET_FILENAME_COMPONENT(OUTPUT_PATH "${file}" PATH)

            SET(ASM_SOURCE ${file})
            SET(ASM_OBJECT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILENAME}.obj)
            SET(ASM_OBJECTS ${ASM_OBJECTS} ${ASM_OBJECT})

            IF(CMAKE_GENERATOR MATCHES "NMake")
                ADD_CUSTOM_COMMAND(
                    OUTPUT      ${ASM_OBJECT}
                    COMMAND     ${NASM_ASSEMBLER}
                    ARGS        -f win32 -DPREFIX -I${ARGV1} -Xvc -o "${ASM_OBJECT}" ${ASM_SOURCE}
                    DEPENDS     ${ASM_SOURCE}
                    COMMENT     "Assembling ${OUTPUT_FILENAME}.asm"
                )
                LIST(APPEND ${ARGV0} ${ASM_OBJECT})
            ELSE(CMAKE_GENERATOR MATCHES "NMake")
                ADD_CUSTOM_COMMAND(
                    OUTPUT      ${ASM_OBJECT}
                    COMMAND     ${NASM_ASSEMBLER}
                    ARGS        -f win32 -DPREFIX -I${ARGV1} -Xvc -o "${ASM_OBJECT}" ${ASM_SOURCE}
                    MAIN_DEPENDENCY ${ASM_SOURCE}
                    COMMENT     "Assembling ${OUTPUT_FILENAME}.asm"
                )
            ENDIF(CMAKE_GENERATOR MATCHES "NMake")
        ENDIF(${file} MATCHES ".*asm$")
    ENDFOREACH(file ${${ARGV0}})
ENDMACRO(ASSEMBLE)

# Configure the Precompiled Header
# ARGV0 = Files that shouldn't have PCH support (default = *.c)
MACRO(ADD_PCH_SUPPORT)
    IF(MSVC)
        SET(USE_PCH_SUPPORT 1)
        
        # Figure out which files should have PCH support and which files shouldn't
        FILE(GLOB_RECURSE NON_PCH_FILES *.c)
        FILE(GLOB_RECURSE PCH_FILES *.cpp)
        FILE(GLOB_RECURSE PCH_SOURCE stdafx.cpp)
        
        IF(${ARGC} GREATER 0)
            LIST(APPEND NON_PCH_FILES ${ARGN})
        ENDIF(${ARGC} GREATER 0)
        LIST(REMOVE_ITEM NON_PCH_FILES ${PCH_SOURCE})
        LIST(REMOVE_ITEM PCH_FILES ${PCH_SOURCE})
        
        IF(NOT MSVC_IDE)
            SET_SOURCE_FILES_PROPERTIES(${PCH_FILES} PROPERTIES COMPILE_FLAGS "/Fp\"$(INTDIR)/${PROJECT_NAME}.pch\" /Yu\"stdafx.h\"")           
            SET_SOURCE_FILES_PROPERTIES(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Fp\"$(INTDIR)/${PROJECT_NAME}.pch\" /Yc\"stdafx.h\"")
        ELSEIF(MSVC60)
            SET_SOURCE_FILES_PROPERTIES(${PCH_FILES} PROPERTIES COMPILE_FLAGS "/Yu\"stdafx.h\"")
            SET_SOURCE_FILES_PROPERTIES(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc\"stdafx.h\"")
        ELSE(NOT MSVC_IDE)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yu")
            SET_SOURCE_FILES_PROPERTIES(${NON_PCH_FILES} PROPERTIES COMPILE_FLAGS "/Y-")
            SET_SOURCE_FILES_PROPERTIES(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc")
        ENDIF(NOT MSVC_IDE)
    ENDIF(MSVC)
ENDMACRO(ADD_PCH_SUPPORT)

MACRO(ADD_QT_SUPPORT)
    SET(QT_MOC_HEADERS)
    SET(QT_SRC_UI)
    SET(QT_SRC_QRC)
    SET(QT_UIS_H)
    SET(QT_MOC_SRCS)
    SET(QT_QRC_SRCS)

    INCLUDE(${QT_USE_FILE})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR})
    ADD_DEFINITIONS(${QT_DEFINITIONS})
    LIST(APPEND LIBS ${QT_LIBRARIES})

    FILE(GLOB QT_SRC_UI *.ui)
    FILE(GLOB QT_SRC_QRC *.qrc)
   
    FILE(GLOB ALL_MOC_HEADERS *.h)
    FOREACH(hfile ${ALL_MOC_HEADERS})
        GET_FILENAME_COMPONENT(_abs_FILE ${hfile} ABSOLUTE)
        FILE(READ ${_abs_FILE} _contents)
        STRING(REGEX MATCHALL "Q_OBJECT" _match "${_contents}")
        IF(_match)
            LIST(APPEND QT_MOC_HEADERS ${hfile})
        ENDIF(_match)
    ENDFOREACH(hfile)
    IF(USE_PCH_SUPPORT)
        HACK_QT4_WRAP_CPP(QT_MOC_SRCS ${QT_MOC_HEADERS})
    ELSE(USE_PCH_SUPPORT)
        QT4_WRAP_CPP(QT_MOC_SRCS ${QT_MOC_HEADERS})
    ENDIF(USE_PCH_SUPPORT)

    QT4_WRAP_UI(QT_UIS_H ${QT_SRC_UI})                  
    QT4_ADD_RESOURCES(QT_QRC_SRCS ${QT_SRC_QRC})            
    SET(QT_FILES ${QT_UIS_H} ${QT_MOC_SRCS} ${QT_QRC_SRCS}) 
ENDMACRO(ADD_QT_SUPPORT)

MACRO (HACK_QT4_WRAP_CPP outfiles)
    QT4_GET_MOC_FLAGS(moc_flags)
    if(CMAKE_VERSION VERSION_LESS 2.8.12)
        QT4_EXTRACT_OPTIONS(moc_files moc_options ${ARGN})
    else()
        QT4_EXTRACT_OPTIONS(moc_files moc_options moc_target ${ARGN})
    endif()
    FOREACH (it ${moc_files})
        GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
        SET(MOC_FLAGS_APPEND "-fStdAfx.h" "-f${it}")
        QT4_MAKE_OUTPUT_FILE(${it} moc_ cxx outfile)
        if(CMAKE_VERSION VERSION_LESS 2.8.12)
            QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags};${MOC_FLAGS_APPEND}" "${moc_options}")
        else()
            QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags};${MOC_FLAGS_APPEND}" "${moc_options}" "")
        endif()
        SET(${outfiles} ${${outfiles}} ${outfile})
    ENDFOREACH(it)
ENDMACRO (HACK_QT4_WRAP_CPP)