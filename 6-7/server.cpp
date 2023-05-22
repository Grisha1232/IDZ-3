#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 12345
#define MAX_CLIENTS 5

int philosophers[MAX_CLIENTS];
int forks[MAX_CLIENTS];
pthread_mutex_t mutex;

struct ARGS {
  int id;
  int socket_id;
  int viewer_id;
};

void *handlePhilosopher(void *args) {
  int count = 0;
  ARGS *arg = (ARGS *) args;
  int socket_id = arg->socket_id;
  int id = arg->id - 1;
  int viewer = arg->viewer_id;
  int start = 1;
  char buff[1024];
  if (send(socket_id, &start, sizeof(start), 0) == -1) {
    perror("Error sending start of dinner to client");
    exit(1);
  }
  while (count != 6) {
    int message;
    if (read(socket_id, &message, sizeof message) == -1) {
      perror("Error reading message from client");
      exit(1);
    }
    // philosopher end taking meals
    if (message == -111) {
      count = 6;
    }
    // phil number id(message) trying to take two forks
    if (message > 0) {
      printf("Philosopher %d trying to take two forks\n", id + 1);
      sprintf(buff, "Server: Philosopher %d trying to take two forks\n", id + 1);

      if (send(viewer, buff, sizeof buff, 0) == -1) {
        perror("Error sending message to viewer");
        exit(1);
      }
      // locking for sharing memory
      pthread_mutex_lock(&mutex);
      if (forks[id] == 0 && forks[(id + 1) % 5] == 0) {
        printf("Philosopher %d taking two forks\n", id + 1);
        sprintf(buff, "Server: Philosopher %d taking two forks\n", id + 1);

        if (send(viewer, buff, sizeof buff, 0) == -1) {
          perror("Error sending message to viewer");
          exit(1);
        }
        forks[id] = message;
        forks[(id + 1) % 5] = message;
        int res = 1;
        if (send(socket_id, &res, sizeof res, 0) == -1) {
          perror("Error sending about taking two forks");
          exit(1);
        }
      } else {
        printf("Philosopher %d must wait for forks\n", id + 1);
        sprintf(buff, "Server: Philosopher %d must wait for forks\n", id + 1);

        if (send(viewer, buff, sizeof buff, 0) == -1) {
          perror("Error sending message to viewer");
          exit(1);
        }
        int res = 0;
        if (send(socket_id, &res, sizeof res, 0) == -1) {
          perror("Error sending about not taking two forks");
          exit(1);
        }
      }
      pthread_mutex_unlock(&mutex);
    } else {
      printf("Philosopher %d giving back two forks\n", id + 1);
      sprintf(buff, "Server: Philosopher %d giving back two forks\n", id + 1);

      if (send(viewer, buff, sizeof buff, 0) == -1) {
        perror("Error sending message to viewer");
        exit(1);
      }
      pthread_mutex_lock(&mutex);
      forks[id] = 0;
      forks[(id + 1) % 5] = 0;
      pthread_mutex_unlock(&mutex);
    }
  }
  sprintf(buff, "stop");
  if (send(viewer, buff, sizeof buff, 0) == -1) {
    perror("Error sending message to viewer");
    exit(1);
  }
  return NULL;
}

int main() {
  int server_socket, viewer_socket;
  int client_sockets[MAX_CLIENTS];
  int port;
  struct sockaddr_in server_addr, client_addr, viewer_addr;
  socklen_t sockaddr_in_size;

  printf("Enter the port (dont use service ports): ");
  scanf("%d", &port);

  // Создание сокета сервера
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Error creating server socket");
    exit(1);
  }

  // Настройка адреса сервера
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Привязка сокета к адресу сервера
  if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    perror("Error binding server socket");
    exit(1);
  }

  // Ожидание подключения клиентов
  if (listen(server_socket, MAX_CLIENTS) == -1) {
    perror("Error listening for clients");
    exit(1);
  }

  viewer_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (viewer_socket == -1) {
    perror("Error creating viewer socket");
    exit(1);
  }
  viewer_addr.sin_family = AF_INET;
  viewer_addr.sin_port = htons(8080);
  viewer_addr.sin_addr.s_addr = INADDR_ANY;
  if (connect(viewer_socket, (struct sockaddr*) &viewer_addr, sizeof viewer_addr) == -1) {
    perror("Error connecting viewer socket");
    exit(1);
  }


  pthread_mutex_init(&mutex, NULL);

  char buff[1024] = "Server is running waiting for clients\n";
  printf("%s", buff);

  int connected_clients = 0;

  while (connected_clients < MAX_CLIENTS) {
    // Принятие подключения от клиента
    sockaddr_in_size = sizeof(client_addr);
    client_sockets[connected_clients] = accept(server_socket, (struct sockaddr *) &client_addr, &sockaddr_in_size);
    if (client_sockets[connected_clients] == -1) {
      perror("Error accepting client connection");
      exit(1);
    }

    int id;
    if (read(client_sockets[connected_clients], &id, sizeof id) == -1) {
      perror("Error reading id client");
      exit(1);
    }

    sprintf(buff, "Server: Client connected. Client ID: %d\n", id);

    printf("Client connected. Client ID: %d\n", id);
    if (send(viewer_socket, buff, sizeof buff, 0) == -1) {
      perror("Error sending message to viewer");
      exit(1);
    }

    // Добавление нового клиента в массив философов
    philosophers[connected_clients] = id;
    forks[connected_clients] = 0;

    connected_clients++;
  }
  sprintf(buff, "Server: all clients are connected (5 philosophers) starting the dinner\n");

  printf("all clients are connected (5 philosophers) starting the dinner\n");
  if (send(viewer_socket, buff, sizeof buff, 0) == -1) {
    perror("Error sending message to viewer");
    exit(1);
  }
  pthread_t threads[MAX_CLIENTS];
  for (int i = 0; i < MAX_CLIENTS; i++) {
    ARGS *args;
    args = (ARGS *) (malloc(sizeof(struct ARGS) * 1));
    args->id = philosophers[i];
    args->socket_id = client_sockets[i];
    args->viewer_id = viewer_socket;
    pthread_create(&threads[i], NULL, &handlePhilosopher, (void *) args);
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    pthread_join(threads[i], NULL);
  }

  // Закрытие сокета сервера
  pthread_mutex_destroy(&mutex);
  close(server_socket);

  return 0;
}
