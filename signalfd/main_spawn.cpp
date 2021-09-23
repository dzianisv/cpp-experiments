#include "spawn.h"
#include "wait.h"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sys/signalfd.h>
#include <thread>
#include <vector>
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

int spawn(const std::vector<std::string> cmd, bool wait = true) {
  pid_t child_pid;
  sigset_t mask;
  int err;

  posix_spawnattr_t attr;
  posix_spawn_file_actions_t file_actions;

  if ((err = posix_spawn_file_actions_init(&file_actions))) {
    std::cerr << "posix_spawn_file_actions_init() failed" << std::endl;
    return err;
  }

  if ((err = posix_spawnattr_init(&attr))) {
    std::cerr << "posix_spawnattr_init() failed" << std::endl;
    return err;
  }

  if ((err = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK))) {
    std::cerr << "posix_spawnattr_setflags() failed" << std::endl;
    return err;
  }

  sigemptyset(&mask);

  if ((err = posix_spawnattr_setsigmask(&attr, &mask))) {
    std::cerr << "posix_spawnattr_setsigmask() failed" << std::endl;
    return err;
  }

  const char *argv[cmd.size() + 1] = {0};
  for (size_t i = 0; i < cmd.size(); i++) {
    argv[i] = cmd[i].c_str();
  }

  if ((err = posix_spawn(&child_pid, argv[0], &file_actions, &attr,
                         (char *const *)argv, environ))) {
    std::cerr << "posix_spawn() failed" << std::endl;
    return err;
  }

  if (wait) {
    waitpid(child_pid, &err, __WALL);
  }
  return err;
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

  spawn({"./subprocess"}, false);
  cout << "press Return to exit..." << endl;
  char c;
  cin >> c;
  cout << "exiting..." << endl;
  return 0;
}