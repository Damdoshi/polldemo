
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(void)
{
  struct sockaddr_in sockaddr;
  int type = 1;
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    perror("socket");
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &type, sizeof(type)) == -1)
    perror("setsockopt");
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sockaddr.sin_port = htons(2000);
  if (bind(fd, (struct sockaddr*)&sockaddr, (socklen_t)sizeof(sockaddr)) == -1)
    perror("bind");
  if (listen(fd, SOMAXCONN) == -1)
    perror("listen");
  struct pollfd monitors[2];
  int nbr_co = 1;
  int connec;

  monitors[0].fd = fd;
  monitors[0].events = POLLIN;
  monitors[0].revents = 0;

  while (1) // La classe
    {
      puts("Je vis!");

      if (poll(monitors, nbr_co, 10) == -1)
	perror("poll");

      if (monitors[0].revents & POLLIN)
	{
	  puts("Je vais accepter quelque chose");
	  type = sizeof(sockaddr);
	  if ((connec = accept(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&type)) == -1)
	    perror("accept");
	  nbr_co += 1;
	  monitors[1].fd = connec;
	  monitors[1].events = POLLIN | POLLOUT;
	}
      if (nbr_co > 1 && monitors[1].revents & POLLIN)
	{
	  puts("Je vais lire quelque chose");
	  char buffer[4096];
	  int l  ;

	  l = read(monitors[1].fd, &buffer[0], sizeof(buffer));
	  write(1, &buffer[0], l);
	}
      usleep(1e6);
    }

  close(fd);
  return (0);
}
