//
// Created by ilya on 25.11.2019.
//
#include "manager.h"

int main() {
  Database db;
  DatabaseManager dm(db);
  std::cout.precision(6);
  auto result = dm.ProcessAllJSONRequests();
  std::cout << result << std::endl;
  return 0;
}