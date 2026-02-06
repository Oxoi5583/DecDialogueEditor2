#pragma once

#include "system/obj/fstream/base.h"

typedef std::string FString;

class FStreamFile : public FStreamBase{
private:

public:
    FStreamFile();
    ~FStreamFile();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    FSizeUnit get_size() override;

    void truncate();
    void append(FString p_text);

    void remove() override;
};