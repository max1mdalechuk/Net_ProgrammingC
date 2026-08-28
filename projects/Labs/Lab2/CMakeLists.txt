project(Lab2)


#--------------------------------------------------------------------------------#
#INDIVIDUAL
#target for server
include_directories(include)
add_executable(${PROJECT_NAME}_parallel_server src/parallel_server.cpp)
target_link_libraries(${PROJECT_NAME}_parallel_server ${COMMON_DEPENDENCIES})
# target for client
add_executable(${PROJECT_NAME}_client src/client.cpp)
target_link_libraries(${PROJECT_NAME}_client ${COMMON_DEPENDENCIES})
#--------------------------------------------------------------------------------#



