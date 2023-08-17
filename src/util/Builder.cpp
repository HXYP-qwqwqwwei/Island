//
// Created by HXYP on 2023/8/17.
//

#include "util/Builder.h"
#include <iostream>

Builder::Builder(const char *name): name(name) { }

void Builder::setBuilt() {
    this->built = true;
}

bool Builder::checkBuilt(const char *errmsg) const {
    if (this->built) {
        std::cout << "WARN::" << this->name << "::" << errmsg << std::endl;
    }
    return this->built;
}

bool Builder::checkNotBuilt(const char *errmsg) const {
    if (!this->built) {
        std::cout << "WARN::" << this->name << "::" << errmsg << std::endl;
    }
    return !this->built;
}
