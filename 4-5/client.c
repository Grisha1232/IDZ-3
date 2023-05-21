#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

void start_dinner(int philosopher_id, int socket) {
  int count_of_meals = 0;
  // states = 0 or 1 or 2;
  // state 0 is for thinking and taking forks after
  // state 1 is for taking forks and eat;
  // state 2 is for giving back forks
  int current_state = 0;
  while (count_of_meals < 5) {
    // sending numbers 1, 2 -- refers to server that 1 - reserve forks, 2 - unreserve forks
    if (current_state == 0) {
      printf("Phil is thinking\n");
      if (send(socket, &philosopher_id, sizeof philosopher_id, 0) == -1) {
        perror("Error sending reservation");
        exit(1);
      }
      current_state = 1;
      sleep(rand() % 5 + 1);
    } else if (current_state == 1) {
      int result;
      if (read(socket, &result, sizeof result) == -1) {
        perror("Error reading of result taking the forks");
        exit(1);
      }
      // philosopher cant take forks at this moment
      if (result == 0) {
        current_state = 0;
      } else {
        printf("Takes two forks and eating\n");
        current_state = 2;

        sleep(rand() % 5 + 1);
      }
    } else {
      count_of_meals++;
      current_state = 0;
      int negative = -philosopher_id;
      if (send(socket, &negative, sizeof negative, 0) == -1) {
        perror("Error sending back forks");
        exit(1);
      }

      printf("Giving back two forks\n");
      sleep(rand() % 5 + 1);
    }
  }
  int stop = -111;
  if (send(socket, &stop, sizeof stop, 0) == -1){
    perror("Error sending stop message");
    exit(1);
  }
}

int main() {
  srand(time(NULL));
  int client_socket;
  int port;
  struct sockaddr_in server_addr;

  printf("Enter the port: ");
  scanf("%d", &port);

  // Создание клиентского сокета
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("Error creating client socket");
    exit(1);
  }

  // Настройка адреса сервера
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0) {
    perror("Error setting server address");
    exit(1);
  }

  // Подключение к серверу
  if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    perror("Error connecting to server");
    exit(1);
  }

  int id;
  printf("Enter the philosopher id: ");
  scanf("%d", &id);

  if (send(client_socket, &id, sizeof(id), 0) == -1) {
    perror("Error sending philosopher ID");
    exit(1);
  }

  int is_starting_dinner = 0;
  if (read(client_socket, &is_starting_dinner, sizeof(is_starting_dinner)) == -1) {
    perror("Error on reading for start the dinner");
    exit(1);
  }
  start_dinner(id, client_socket);
  // Закрытие клиентского сокета
  close(client_socket);

  return 0;
}
