#include <iostream>

#include "cxx-langstat/Stats.h"

using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
ordered_json add(ordered_json&& lhs, const ordered_json& rhs){
    if(lhs.empty())
        return rhs;
    for(auto [keyr, valr] : rhs.items()){
        // std::cout << keyr << ", " << valr << std::endl;
        bool matched = false;
        for(auto [keyl, vall] : lhs.items()){
            // std::cout << keyl << ", " << vall << std::endl;
            if(keyr == keyl){
                matched = true;
                if(valr.is_object() && vall.is_object()){
                    lhs[keyl] = add(std::move(vall), valr);
                } else if(valr.is_number() && vall.is_number()){
                    lhs[keyl] = valr.get<int>() + vall.get<int>();
                } else {
                    assert(false && "Either lhs or rhs contained illegal data type or inconsistent data");
                }
            }
        }
        if(!matched) {
            lhs[keyr] = valr;
            // std::cout << "else: " << keyr << ", " << valr << std::endl;
        }
        // std::cout << "--" << std::endl;
    }
    return std::move(lhs);
}

//-----------------------------------------------------------------------------
