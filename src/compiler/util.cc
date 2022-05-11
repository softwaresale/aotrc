//
// Created by charlie on 5/11/22.
//

#include "util.h"

std::string aotrc::compiler::getStateBlockLabel(unsigned int stateId) {
    return "STATE_" + std::to_string(stateId);
}
