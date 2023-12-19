#include <stdio.h>
#include <winsock2.h>

#define PORT 9001
#define MAX_BUFFER_SIZE 1024

int main()
{
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // 创建 socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket.\n");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connection failed.\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // 通信循环
    while (1)
    {
        char buffer[MAX_BUFFER_SIZE] = {0};
        char command[MAX_BUFFER_SIZE];

        // 从用户输入读取命令
        printf("Enter command: ");
        fgets(command, MAX_BUFFER_SIZE, stdin);

        // 发送命令给服务器
        send(client_socket, command, strlen(command), 0);

        // 接收服务器响应
        recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        printf("Server response: %s\n", buffer);

        // 处理退出命令
        if (strncmp(command, "EXIT", 4) == 0)
        {
            break;
        }
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
