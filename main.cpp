#include "src/FileSystem.cpp"
#include <string>
// empty main function
int main(int argc, char *argv[])
{
  std::string path = argv[1];
  FileSystem fs(path);
  fs.printBootSector();
  // std::cout << std::endl;
  // fs.printBootSectorAll();
  std::cout << std::endl;
  std::cout << "Files in root directory:" << std::endl;
  std::cout << std::endl;
  fs.printRootSector();
  return 0;
}