#include "json_parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>

#define START_TIMER(name) auto  start##name = std::chrono::high_resolution_clock::now();
#define END_TIMER(name)     auto stop##name = std::chrono::high_resolution_clock::now();\
    auto duration##name = std::chrono::duration_cast<std::chrono::microseconds>(stop##name - start##name);\
    std::cout << #name << " Duration was " << duration##name.count() << " microseconds." << std::endl;

int main()
{
    START_TIMER(stringstream);
    std::ifstream input("test.json", std::ios::in | std::ios::end);
    std::stringstream ss;
    ss << input.rdbuf();
    END_TIMER(stringstream);

    START_TIMER(parse)
    jfather::JSONObject json2 = jfather::ParseJson(ss.str());
    END_TIMER(parse);
}
