INIT(ultracomm)

ADD_PCH_SUPPORT()
#LIST(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/ulterius_old.lib)
LIST(APPEND LIBS ${ULTRASONIX_SDK_PATH}/ulterius/lib/${ULTERIUS_LIB_NAME})
ADD_DEFINITIONS(-DIMPORT_ULTERIUS)

# The /EHa flag is used to capture access violation errors that occur when
# closing ultracomm and appear to occur within the ulterius library,
# which we don't control.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")

# The /MD compiler flag might reduce access violation errors, but this is
# not certain.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")

CREATE_CONSOLE_EXECUTABLE()
