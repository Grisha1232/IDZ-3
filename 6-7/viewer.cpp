#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int sockets[6];
pthread_mutex_t mutex;

void* handle(void* arg) {
  int socket = *((int *) arg);
  char buffer[1024];

  while (strcmp(buffer, "stop") != 0) {
    if (read(socket, &buffer, 1024) == -1) {
      perror("Error reading message from server");
      exit(1);
    }
    pthread_mutex_lock(&mutex);
    printf("%s", buffer);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main() {
  int sock = 0;
  struct sockaddr_in server_addr, clients_addr;
  socklen_t sockaddr_in_size;
  char buffer[1024];

  pthread_mutex_init(&mutex, NULL);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error creating socket");
    exit(1);
  }

  memset(&server_addr, 0, sizeof server_addr);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(8080);

  if (bind(sock, (struct sockaddr *) &server_addr, sizeof server_addr) == -1) {
    perror("bind failed");
    exit(1);
  }

  // Ожидание подключения клиентов
  if (listen(sock, 6) == -1) {
    perror("Error listening for clients");
    exit(1);
  }

  printf("Viewer active waiting for connections from all\n");
  for (int i = 0; i < 6; i++) {
    sockaddr_in_size = sizeof(clients_addr);
    sockets[i] = accept(sock, (struct sockaddr*) &clients_addr, &sockaddr_in_size);

    if (sockets[i] == -1) {
      perror("Error accepting client connection");
      exit(1);
    }
  }
  pthread_t threads[6];
  for (int i = 0; i < 6; i++) {
    pthread_create(&threads[i], NULL, handle, (void *) &sockets[i]);
  }

  for (int i = 0; i < 6; i++) {
    pthread_join(threads[i], NULL);
  }

  close(sock);
  return 0;
}
