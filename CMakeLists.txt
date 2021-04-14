#project
PROJECT(fsm)

#Add CFlag and LDFLAG
set(CMAKE_CXX_FLAGS " -lrt -std=c++11 -Wall -Werror -Wno-unused-variable -WNo-unused-function ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS}")

#include directories
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

#collect the app src file
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
message("list DIR_SRCS")
message("${DIR_SRCS}")

ADD_DEFINITIONS(-D_LINUX)

#Generate static
ADD_LIBRARY(fsm STATIC ${DIR_SRCS})
target_include_directories(fsm PUBLIC ${CMAKE_SOURCE_DIR})

#Generate dynamic
#ADD_LIBRARY(fsm SHARED ${DIR_SRCS})
#target_include_directories(fsm PUBLIC ${CMAKE_SOURCE_DIR})
#set_target_properties(fsm PROPERTIES PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/fsm.h)

message("CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")

install(TARGETS fsm DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/fsm.h DESTINATION include)

#install(TARGETS fsm
#        EXPORT FsmTargets
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        RUNTIME DESTINATION bin
#        PUBLIC_HEADER DESTINATION include)