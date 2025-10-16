#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include <cstdint>

#include <numeric>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <string>
#include <random>
#include <ctime>
#include <map>
#include <iostream>


typedef unsigned int length;

class UniformRandomGenerator {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distr;
public:
    UniformRandomGenerator(int min, int max)
        : rd(), gen(rd())
        , distr(min, max){};
    ~UniformRandomGenerator();
    int generate();
};
class NormalRandomGenerator {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> distr;
public:
    NormalRandomGenerator(double _mean, double _sigma)
        : rd(), gen(rd())
        , distr(_mean, _sigma){};
    ~NormalRandomGenerator();
    double generate();
};

typedef std::map<std::pair<int,int>     ,UniformRandomGenerator*>    UniformGeneratorDict;
typedef std::map<std::pair<float,float> ,NormalRandomGenerator*>     NormalGeneratorDict;


class RandomServer : public Singleton<RandomServer>{
private:
    static RandomServer* singleton;

    UniformGeneratorDict ug_dict = UniformGeneratorDict();
    NormalGeneratorDict  ng_dict = NormalGeneratorDict();

public:
    RandomServer();
    ~RandomServer();

    // advanced
    double get_uniform_distr_random_float(double _min, double _max);
    int get_uniform_distr_random_int(int _min, int _max);
    int get_normal_distr_random_int(double _mean, double _sigma);
    double get_normal_distr_random_float(double _mean, double _sigma);
    char get_uniform_distr_random_char();

    struct RandomRange{
        int _min;
        int _max;
    };
    int get_uniform_distr_random_int(std::vector<RandomRange> _ranges);

    // simple
    char new_random_char();
    int new_random_int(int _min, int _max);


    std::string get_uniform_distr_random_code(const length& _l = 10);
};

