// main.c
// simple webserver
// @author Sérgio Henrique Domingues de Oliveira
// @version 1.0.0
// @date 2024-10-26T14:55:59.0097205-03:00

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define VERSION "1.0.4"
#define PORT 8080
#define ADDR "127.0.0.1"

#define MAX_REQUEST_SIZE 4096

#define GET_REQUEST 0
#define POST_REQUEST 1
#define UNKNOW_REQUEST -1

int getMethod(const char *request)
{
  char method[10];
  sscanf(request, "%s", method);

  if (strcmp(method, "GET") == 0)
  {
    printf("Type GET\n");
    return GET_REQUEST;
  }
  else
  {
    if (strcmp(method, "POST") == 0)
    {
      printf("Type POST\n");
      return POST_REQUEST;
    }
    else
    {
      printf("Type UNKNOW\n");
      return UNKNOW_REQUEST;
    }
  }
}

void handleRequest(const char *request, char *response)
{
  // Variáveis para armazenar o método e a rota
  char method[10];
  char route[100];

  // Analisar a string de solicitação
  sscanf(request, "%s %s", method, route);

  // Comparar a rota e o método HTTP para tomar decisões
  switch (getMethod(method))
  {
  case GET_REQUEST:
    printf("GET Request\n");

    if (strcmp(route, "/api/v1/status") == 0)
    {
      strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nStatus.");
    }
    else
    {
      strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRota Inexistente.");
    }

    break;

  case POST_REQUEST:
    printf("POST Request\n");
    strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nStatus.");
    break;

  case UNKNOW_REQUEST:
    strcpy(response, "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMetodo nao permitido.");
    break;

  default:
    strcpy(response, "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMetodo nao permitido.");
  }
}

int main()
{
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    printf("Falha ao inicializar o Winsock.\n");
    return 1;
  }

  SOCKET serverSocket;
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == INVALID_SOCKET)
  {
    printf("Falha ao criar o socket.\n");
    return 1;
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(ADDR);
  serverAddress.sin_port = htons(PORT);

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
  {
    printf("Falha ao vincular o socket ao endereco IP e a porta.\n");
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  }

  if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
  {
    printf("Falha ao colocar o socket em modo de escuta.\n");
    closesocket(serverSocket);
    WSACleanup();
    return 1;
  }

  SOCKET clientSocket;
  struct sockaddr_in clientAddress;
  int clientAddressLength = sizeof(clientAddress);

  printf("SHDO WEBSERVER\n");
  printf("VERSION: %s\n", VERSION);
  printf("Listening at %s:%d\n", ADDR,PORT);

  while (1)
  {
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket == INVALID_SOCKET)
    {
      printf("Falha ao aceitar a conexao do cliente.\n");
      closesocket(serverSocket);
      WSACleanup();
      return 1;
    }

    char request[MAX_REQUEST_SIZE];
    memset(request, 0, sizeof(request));
    recv(clientSocket, request, sizeof(request) - 1, 0);

    char response[MAX_REQUEST_SIZE];
    memset(response, 0, sizeof(response));
    handleRequest(request, response);

    send(clientSocket, response, strlen(response), 0);

    closesocket(clientSocket);
  }

  closesocket(serverSocket);
  WSACleanup();

  return 0;
}
