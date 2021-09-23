#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

void signal_handler(int signo) {
  cout << "subprocess received signal " << signo << endl;
  exit(0);
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  cout << "subprocess started..." << endl;
  std::this_thread::sleep_for(300s);
  cout << "subprocess stopped..." << endl;
  return 0;
}