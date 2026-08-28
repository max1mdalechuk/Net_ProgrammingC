project(Lab1)


#--------------------------------------------------------------------------------#
#INDIVIDUAL
#target for server
include_directories(individual/include)
add_executable(${PROJECT_NAME}_individual_server individual/src/server.cpp)
target_link_libraries(${PROJECT_NAME}_individual_server ${COMMON_DEPENDENCIES})
# target for client
add_executable(${PROJECT_NAME}_individual_client individual/src/client.cpp)
target_link_libraries(${PROJECT_NAME}_individual_client ${COMMON_DEPENDENCIES})
#--------------------------------------------------------------------------------#



#--------------------------------------------------------------------------------#
#PRACTICAL
include_directories(practical/include)
add_executable(${PROJECT_NAME}_practical_nslookup practical/src/ns_lookup.cpp)
target_link_libraries(${PROJECT_NAME}_practical_nslookup ${COMMON_DEPENDENCIES})

include_directories(practical/include)
add_executable(${PROJECT_NAME}_practical_udp_server practical/src/udp_server.cpp)
target_link_libraries(${PROJECT_NAME}_practical_udp_server ${COMMON_DEPENDENCIES})

include_directories(practical/include)
add_executable(${PROJECT_NAME}_practical_udp_client practical/src/udp_client.cpp)
target_link_libraries(${PROJECT_NAME}_practical_udp_client ${COMMON_DEPENDENCIES})
# add_executable(${PROJECT_NAME}_practical_client practical/src/client.cpp)
# target_link_libraries(${PROJECT_NAME}_practical_client ${COMMON_DEPENDENCIES})
#--------------------------------------------------------------------------------#
