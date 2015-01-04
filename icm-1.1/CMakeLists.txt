cmake_minimum_required(VERSION 2.6.2)
include(env_inc.make)

add_subdirectory(icc)
add_subdirectory(icm)
add_subdirectory(msg)

install(DIRECTORY icc DESTINATION include/icm
        FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE)
install(DIRECTORY icc DESTINATION include/icm
        FILES_MATCHING PATTERN "*.cpp" PATTERN ".svn" EXCLUDE)

install(DIRECTORY icm DESTINATION include/icm
        FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install(DIRECTORY msg DESTINATION include/icm
        FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install(DIRECTORY os DESTINATION include/icm
        FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install(DIRECTORY os DESTINATION include/icm
        FILES_MATCHING PATTERN "*.inl" PATTERN ".svn" EXCLUDE)

