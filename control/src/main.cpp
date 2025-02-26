#include "../headers/camera.hpp"

int main() {
  Camera detector("/dev/video4"); // denne kilde er mit webcam i linux. Ser
                                  // anderledes ud i windows
  detector.run();
  return 0;
}



