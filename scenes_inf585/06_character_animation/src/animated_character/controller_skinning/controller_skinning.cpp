#include "controller_skinning.hpp"

std::string type_str(skinning_weight_info const& )
{
    return "skinning_weight_info";
}
std::ostream& operator<<(std::ostream& s, skinning_weight_info const& weight_info)
{
    s<<weight_info.joint_index<<" "<<weight_info.weight;
    return s;
}