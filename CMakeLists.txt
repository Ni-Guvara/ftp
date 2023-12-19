cmake_minimum_required(VERSION 3.10)
project(ClientServerExample C)

set(CMAKE_C_STANDARD 11)

# 添加服务器端可执行文件
add_executable(server server.c)

# 添加客户端可执行文件
add_executable(client client.c)

# 链接 Winsock 库（仅在 Windows 系统上需要）
if (WIN32)
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(client wsock32 ws2_32)
endif()
