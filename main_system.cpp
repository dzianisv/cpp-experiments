#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sys/signalfd.h>
#include <thread>
#include <unistd.h>

using namespace std;

void signal_handler(int signal_fd) {
  struct signalfd_siginfo si;
  ssize_t ret;

  while ((ret = read(signal_fd, &si, sizeof(si))) == sizeof(si)) {
    cout << "received signal " << si.ssi_signo << endl;
    if (si.ssi_signo == SIGINT || si.ssi_signo == SIGTERM) {
      break;
    }
  }

  close(signal_fd);
  exit(0);
}

int main() {
  sigset_t mask;
  int signal_fd;

  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGHUP);
  sigaddset(&mask, SIGUSR1);

  if ((signal_fd = signalfd(-1, &mask, 0)) == -1) {
    perror("signalfd() failed");
    abort();
  }

  thread signal_thread(signal_handler, signal_fd);
  signal_thread.detach();

  system("./subprocess &");
  cout << "press Return to exit..." << endl;
  char c;
  cin >> c;
  cout << "exiting..." << endl;
  return 0;
}