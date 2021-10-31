#include <iostream>
#include <bitset>
#include <vector>
#include <thread>
#include <fstream>
#include <random>
#include <filesystem>
namespace fs = std::filesystem;

#define WIDTH 1001

std::string get_full_path(std::string filename)
{
    return fs::current_path() /= fs::path("data") /= fs::path(filename);
}

std::vector<std::vector<bool> *> *iterate_rule(std::bitset<8> rule, bool seed)
{
    // initialize first line
    auto first = new std::vector<bool>(WIDTH, false);
    first->at(WIDTH / 2 + 1) = seed;
    std::vector<std::vector<bool> *> *result = new std::vector<std::vector<bool> *>{first};

    for (int i = 1; i < (WIDTH - 1) / 2; i++)
    {
        auto row = new std::vector<bool>{};
        std::vector<bool> *prev = result->at(i - 1);
        row->push_back(false);
        for (unsigned int j = 1; j < prev->size() - 1; j++)
        {
            // reverse order for correct binary number
            auto val = std::bitset<3>().set(0, prev->at(j + 1)).set(1, prev->at(j)).set(2, prev->at(j - 1));
            row->push_back((bool)rule[val.to_ulong()]);
        }
        row->push_back(false);
        result->push_back(row);
    }
    return result;
}

void write_to_file(std::vector<std::vector<std::vector<bool> *> *> map, std::string filename, int scale = 1)
{
    int height = (WIDTH - 1) / 2;
    filename = get_full_path(filename);
    // open file and add pixelmap header
    std::ofstream myfile;
    myfile.open(filename);
    myfile << "P3" << std::endl;
    myfile << WIDTH * scale << " " << height * scale << " 1" << std::endl;

    // iterate data structure and add padding for pretty pyramid
    for (int i = 0; i < height; i++)
    {
        for (int l = 0; l < scale; l++)
        {
            for (unsigned int j = 0; j < WIDTH; j++)
            {
                for (int k = 0; k < scale; k++)
                {
                    for (auto result : map)
                    {
                        myfile << result->at(i)->at(j) << " ";
                    }
                    myfile << "   ";
                }
            }
            myfile << std::endl;
        }
    }
    std::string new_filename = filename.substr(0, filename.length() - (filename.length() - filename.find_last_of(".")));
    std::ostringstream command;
    command << "convert " << filename << " " << new_filename << ".png";

    std::system(command.str().c_str());
    fs::remove(fs::path(filename));
}

void full_build(std::bitset<8> rule, bool seed, std::string filename)
{
    write_to_file(std::vector<std::vector<std::vector<bool> *> *>{iterate_rule(rule, seed)}, get_full_path(filename));
}

void create_all()
{
    for (int i = 0; i < 256; i++)
    {
        std::bitset<8> rule(i);
        std::cout << "Building rule: " << rule.to_ullong() << std::endl;
        std::ostringstream filename;
        filename << "rule_" << rule.to_ulong() << ".ppm";
        full_build(rule, true, filename.str());
    }
}
void test()
{
    // test with rule 30
    bool seed = true;
    std::bitset<8> rule("00011110");
    std::string filename = get_full_path("test.ppm");
    write_to_file(std::vector<std::vector<std::vector<bool> *> *>{iterate_rule(rule, seed)}, filename);
}

void create_colored(std::bitset<8> rule1, std::bitset<8> rule2, std::bitset<8> rule3, int scale = 1)
{
    auto map_1 = iterate_rule(rule1, true);
    auto map_2 = iterate_rule(rule2, true);
    auto map_3 = iterate_rule(rule3, true);
    std::ostringstream filename;
    filename << "rule_" << rule1.to_ulong() << "_" << rule2.to_ullong() << "_" << rule3.to_ullong() << ".ppm";
    write_to_file(std::vector<std::vector<std::vector<bool> *> *>{map_1, map_2, map_3}, filename.str(), scale);
}
void random_colored(int n)
{
    std::random_device rd;                         // obtain a random number from hardware
    std::mt19937 gen(rd());                        // seed the generator
    std::uniform_int_distribution<> distr(0, 255); // define the range

    for (int i = 0; i < n; ++i)
    {
        create_colored(std::bitset<8>(distr(gen)), std::bitset<8>(distr(gen)), std::bitset<8>(distr(gen)));
    }
}
void cli_builder_colored()
{
    int rule1;
    int rule2;
    int rule3;
    int scale;
    std::cout << "Enter rule for red channel: ";
    std::cin >> rule1;
    std::cout << std::endl;
    std::cout << "Enter rule for green channel: ";
    std::cin >> rule2;
    std::cout << std::endl;
    std::cout << "Enter rule for blue channel: ";
    std::cin >> rule3;
    std::cout << std::endl;
    std::cout << "Enter scale: ";
    std::cin >> scale;
    std::cout << std::endl;
    create_colored(std::bitset<8>(rule1), std::bitset<8>(rule2), std::bitset<8>(rule3), scale);
}

void file_builder_colored(std::string filename)
{
    std::ifstream infile(filename);
    int rule1, rule2, rule3, scale;
    while (infile >> rule1 >> rule2 >> rule3 >> scale)
    {
        create_colored(std::bitset<8>(rule1), std::bitset<8>(rule2), std::bitset<8>(rule3), scale);
    }
}
int main()
{
    // test();
    // create_all();
    // random_colored(2000);
    // cli_builder_colored();
    file_builder_colored("colors.in");
    return 0;
}