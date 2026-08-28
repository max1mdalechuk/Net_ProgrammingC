project(Lab4)



add_executable(${PROJECT_NAME}_http_server practical/http_server.cpp)
target_link_libraries(${PROJECT_NAME}_http_server ${COMMON_DEPENDENCIES})


add_executable(${PROJECT_NAME}_ftp_tcp_server practical/ftp_tcp_server.cpp)
target_link_libraries(${PROJECT_NAME}_ftp_tcp_server ${COMMON_DEPENDENCIES})

add_executable(${PROJECT_NAME}_ftp_udp_server practical/ftp_udp_server.cpp)
target_link_libraries(${PROJECT_NAME}_ftp_udp_server ${COMMON_DEPENDENCIES})

add_executable(${PROJECT_NAME}_ftp_udp_client practical/ftp_udp_client.cpp)
target_link_libraries(${PROJECT_NAME}_ftp_udp_client ${COMMON_DEPENDENCIES})



