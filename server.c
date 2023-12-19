#include <stdio.h>
#include <winsock2.h>

#define PORT 9001
#define MAX_BUFFER_SIZE 1024
#define USER_DB_FILE "user_db.txt"

typedef struct
{
    char username[50];
    char password[50];
    char email[100];
} User;

User users[100];
int num_users = 0;

void load_user_db()
{
    FILE *file = fopen(USER_DB_FILE, "r");
    if (file != NULL)
    {
        while (fscanf(file, "%s %s %s", users[num_users].username, users[num_users].password, users[num_users].email) != EOF)
        {
            num_users++;
        }
        fclose(file);
    }
}

void save_user_db()
{
    FILE *file = fopen(USER_DB_FILE, "w");
    if (file != NULL)
    {
        for (int i = 0; i < num_users; i++)
        {
            fprintf(file, "%s %s %s\n", users[i].username, users[i].password, users[i].email);
        }
        fclose(file);
    }
}

int authenticate(char *username, char *password)
{
    for (int i = 0; i < num_users; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            return 1; // 验证成功
        }
    }
    return 0; // 验证失败
}

void register_user(char *username, char *password, char *email)
{
    if (num_users < 100)
    {
        strcpy(users[num_users].username, username);
        strcpy(users[num_users].password, password);
        strcpy(users[num_users].email, email);
        num_users++;
        printf("User registered successfully.\n");
    }
    else
    {
        printf("User database is full.\n");
    }
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, new_socket;
    struct sockaddr_in server, client;
    int c;

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // 创建 socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket.\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // 绑定 socket 到端口
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed.\n");
        closesocket(server_socket);
        return 1;
    }

    // 监听
    listen(server_socket, 3);

    printf("Server listening on port %d...\n", PORT);

    // 等待连接
    c = sizeof(struct sockaddr_in);
    new_socket = accept(server_socket, (struct sockaddr *)&client, &c);
    if (new_socket == INVALID_SOCKET)
    {
        printf("Accept failed.\n");
        closesocket(server_socket);
        return 1;
    }

    // 加载用户信息
    load_user_db();

    // 通信循环
    while (1)
    {
        char buffer[MAX_BUFFER_SIZE] = {0};

        // 接收客户端请求
        recv(new_socket, buffer, MAX_BUFFER_SIZE, 0);
        printf("Received from client: %s\n", buffer);

        // 处理请求
        if (strncmp(buffer, "REGISTER", 8) == 0)
        {
            char username[50], password[50], email[100];
            sscanf(buffer, "REGISTER %s %s %s", username, password, email);
            register_user(username, password, email);
        }
        else if (strncmp(buffer, "LOGIN", 5) == 0)
        {
            char username[50], password[50];
            sscanf(buffer, "LOGIN %s %s", username, password);
            int result = authenticate(username, password);
            if (result == 1)
            {
                send(new_socket, "Login successful", 17, 0);
            }
            else
            {
                send(new_socket, "Login failed", 13, 0);
            }
        }
        else if (strncmp(buffer, "EXIT", 4) == 0)
        {
            printf("Server shutting down.\n");
            break;
        }
        else
        {
            send(new_socket, "Invalid command", 16, 0);
        }

        // 保存用户信息到文件
        save_user_db();
    }

    closesocket(new_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
