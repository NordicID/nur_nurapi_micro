cmake_minimum_required(VERSION 3.20)
project(uAPITest C)

set(SOURCES
    ../source/NurMicroApi.c
    uAPITest/SerialTransport.c
    uAPITest/uAPITest.c
)
set(HEADERS
    ../source/NurApiConfig.h
    ../source/NurMicroApi.h
    ../source/NurProtocol.h
    uAPITest/targetver.h
)
add_executable(uAPITest ${SOURCES} ${HEADERS})
target_include_directories(uAPITest PRIVATE ../source) # relative includes
if(WIN32)
    target_link_libraries(uAPITest ws2_32)
endif()

