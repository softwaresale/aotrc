//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_UTIL_H
#define AOTRC_UTIL_H

#include <string>

namespace aotrc::compiler {
    /**
     * Make the label for a basic block for the given state
     * @param stateId State id
     * @return label used for the corresponding label
     */
    std::string getStateBlockLabel(unsigned int stateId);
}

#endif //AOTRC_UTIL_H
