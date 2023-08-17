//
// Created by HXYP on 2023/8/17.
//

#ifndef ISLAND_BUILDER_H
#define ISLAND_BUILDER_H
#include <string>


class Builder {
private:
    bool built = false;
    const std::string name;
protected:
    explicit Builder(const char* name);
    void setBuilt();

public:
    virtual void build() = 0;
    bool checkBuilt(const char* errmsg) const;
    bool checkNotBuilt(const char* errmsg) const;
};


#endif //ISLAND_BUILDER_H
