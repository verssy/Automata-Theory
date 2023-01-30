#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>

class Config{
public:
    static Config& instance() {
        static Config config;
        return config;
    }

    ~Config() {
        serialize();
    }

    bool load_from_file = false;
    std::string input_file_path = "input.txt";
    std::string dka_file_path = "graph.jpg";
    std::string output_file_path = "output.txt";

private:
    Config() {
        deserialize();
    }

    void deserialize() {
        std::ifstream input("cfg.ini");
        if (input.is_open()) {
            input >> load_from_file;
            input.ignore(240, '\n');
            std::getline(input, input_file_path, '\n');
            std::getline(input, dka_file_path, '\n');
            std::getline(input, output_file_path, '\n');
        }
    }

    void serialize() {
        std::ofstream output("cfg.ini");
        if (output.is_open()) {
            output << load_from_file << "\n" << input_file_path << "\n" << dka_file_path << "\n" << output_file_path;
            output.flush();
            output.close();
        }
    }
};

#endif // CONFIG_H
