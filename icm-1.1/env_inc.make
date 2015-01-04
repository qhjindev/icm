
#check env

set (LR_ROOT $ENV{LR_ROOT})
set (LR_THIRD_ROOT $ENV{LR_THIRD_ROOT})

if("${ON_HOST}" STREQUAL "true")
  if ("$ENV{LR_HOST_ROOT}" STREQUAL "")
    message(FATAL_ERROR " The environment variable LR_HOST_ROOT must be defined")
  endif()
  set (LR_HOST_ROOT $ENV{LR_HOST_ROOT})
  set (LR_HOST_THIRD $ENV{LR_HOST_THIRD})
  #set install prefix
  set(CMAKE_INSTALL_PREFIX $ENV{LR_HOST_ROOT})
  if (IS_DIRECTORY ${LR_HOST_THIRD})
    include_directories(
        ${LR_HOST_ROOT}/include
        ${LR_HOST_THIRD}/include
        ) 
    link_directories(
        ${LR_HOST_ROOT}/lib
        ${LR_HOST_ROOT}/stlib
        ${LR_HOST_THIRD}/lib
        ${LR_HOST_THIRD}/stlib
        )
  else ()
    include_directories(
        ${LR_HOST_ROOT}/include
        )
    link_directories(
        ${LR_HOST_ROOT}/lib
        ${LR_HOST_ROOT}/stlib
        )
  endif()
else()
  if ("${LR_ROOT}" STREQUAL "")
    message(FATAL_ERROR " The environment variable LR_ROOT must be defined")
  endif()

  if ("${LR_THIRD_ROOT}" STREQUAL "")
    message(STATUS " The environment variable LR_THIRD_ROOT not  defined")
  endif()
  #set install prefix
  set(CMAKE_INSTALL_PREFIX $ENV{LR_ROOT})
  #set include and lib directory
  if (IS_DIRECTORY ${LR_THIRD_ROOT})
    include_directories(
        ${LR_ROOT}/include
	${LR_THIRD_ROOT}/include
    ) 
    link_directories(
        ${LR_ROOT}/lib
        ${LR_ROOT}/stlib
	${LR_THIRD_ROOT}/lib
    )
  else ()
    include_directories(
        ${LR_ROOT}/include
    )
    link_directories(
        ${LR_ROOT}/lib
        ${LR_ROOT}/stlib
    )
  endif()
endif()



if( "${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAIX ")
  set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -Wl,-G,-brtl,-bnoipath")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -Wl,-G,-brtl,-bnoipath")
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,-brtl,-bnoipath,-bexpall")  # +s, flag 
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-Wl,-brtl,-bnoipath,-bexpall")  # +s, flag 
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-brtl,-bnoipath,-bexpall")
endif()
