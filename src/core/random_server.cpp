#include "core/random_server.h"
#include <algorithm>
#include <sys/stat.h>
#include "DecToolsBox/debug/messenger.h"

int UniformRandomGenerator::generate(){
    return distr(gen);
}
double NormalRandomGenerator::generate(){
    return distr(gen);
}


RandomServer* RandomServer::singleton = nullptr;

RandomServer::RandomServer(){}
RandomServer::~RandomServer(){}

double RandomServer::get_uniform_distr_random_float(double _min, double _max){
    double max = std::max(_max, _min);
    double min = std::min(_max, _min);

    int int_ret = get_uniform_distr_random_int(0, 100000);
    double percentage = (double)int_ret/(double)100000;
    
    double diff = max - min;
    return min + (diff * percentage);
}

int RandomServer::get_uniform_distr_random_int(int _min, int _max){
    int max = std::max(_max, _min);
    int min = std::min(_max, _min);


    std::pair range = std::make_pair(min,max);
    if(ug_dict.contains(range)){
        UniformRandomGenerator& _rg = *ug_dict[range];
        return _rg.generate();
    }else{
        UniformRandomGenerator* new_generator = new UniformRandomGenerator(min,max);
        ug_dict.insert_or_assign(range,new_generator);
        return new_generator->generate();
    }
    return 0;
}
int RandomServer::get_uniform_distr_random_int(std::vector<RandomRange> _ranges){
    std::vector<int> rets;
    for(RandomRange& range : _ranges){
        int max = std::max(range._max, range._min);
        int min = std::min(range._max, range._min);

        rets.push_back(get_uniform_distr_random_int(min, max));
    }

    int ret_index = get_uniform_distr_random_int(0, rets.size() - 1);
    return rets[ret_index];
}
int RandomServer::get_normal_distr_random_int(double _mean, double _sigma){
    std::pair range = std::make_pair(_mean,_sigma);
    if(ng_dict.contains(range)){
        NormalRandomGenerator& _rg = *ng_dict[range];
        return std::round(_rg.generate());
    }else{
        NormalRandomGenerator* new_generator = new NormalRandomGenerator(_mean,_sigma);
        ng_dict.insert_or_assign(range,new_generator);
        return std::round(new_generator->generate());
    }
    return 0;
}
double RandomServer::get_normal_distr_random_float(double _mean, double _sigma){
    std::pair range = std::make_pair(_mean,_sigma);
    if(ng_dict.contains(range)){
        NormalRandomGenerator& _rg = *ng_dict[range];
        return _rg.generate();
    }else{
        NormalRandomGenerator* new_generator = new NormalRandomGenerator(_mean,_sigma);
        ng_dict.insert_or_assign(range,new_generator);
        return new_generator->generate();
    }
    return 0;
}
char RandomServer::get_uniform_distr_random_char(){
    int ret_r = this->get_uniform_distr_random_int(0,35);
    char ret;
    if (ret_r < 10){
        ret =  '0' + ret_r;
    }else{
        ret = 'A' + ret_r - 10;
    }
    return ret;
}
std::string RandomServer::get_uniform_distr_random_code(const length& _l){
    std::string ret = "";
    for(size_t i = 0; i < _l; i++){
        ret += get_uniform_distr_random_char();
    }
    return  ret;
}


char RandomServer::new_random_char(){
    int ret_r = this->get_uniform_distr_random_int(0,35);
    char ret;
    if (ret_r < 10){
        ret =  '0' + ret_r;
    }else{
        ret = 'A' + ret_r - 10;
    }
    return ret;
}

int RandomServer::new_random_int(int _min, int _max){
    return get_uniform_distr_random_int(_min,_max);
}

