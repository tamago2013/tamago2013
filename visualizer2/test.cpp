#include "visualizer/tkg-config.hpp"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    tkg::ConfigFile config;
    cout << config.load("visualizer.conf") << endl;

	cout << (string)config["LaserStream"]["ssm-name"] << endl;
}
