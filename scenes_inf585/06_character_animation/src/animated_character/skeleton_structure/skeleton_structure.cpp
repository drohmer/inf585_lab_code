#include "skeleton_structure.hpp"

int skeleton_structure::size() const{
    return joint_name.size();   
}

void skeleton_structure::update_joint_matrix_local_to_global() {
    int const N = size();
    if(N==0){ return; }
    if(joint_matrix_global.size()!=N) {
        joint_matrix_global.resize(N);
    }
    assert_cgp(parent_index.size()==size(), "Incoherent size of skeleton structure");
    assert_cgp(joint_matrix_local.size()==size(), "Incoherent size of skeleton structure");

    joint_matrix_global[0] = joint_matrix_local[0];
    for (int k = 1; k < N; ++k)
        joint_matrix_global[k] = joint_matrix_global[parent_index[k]] * joint_matrix_local[k];
}
void skeleton_structure::update_joint_matrix_global_to_local() {
    int const N = size();
    if(N==0){ return; }
    if(joint_matrix_local.size()!=N) {
        joint_matrix_local.resize(N);
    }
    assert_cgp(parent_index.size()==size(), "Incoherent size of skeleton structure");
    assert_cgp(joint_matrix_global.size()==size(), "Incoherent size of skeleton structure");

    joint_matrix_local[0] = joint_matrix_global[0];
    for (int k = 1; k < N; ++k)
        joint_matrix_local[k] = joint_matrix_global[parent_index[k]].inverse_assuming_rigid_transform() * joint_matrix_global[k];
}
std::string skeleton_structure::parent_name(int joint_index) const 
{
    int parent = parent_index[joint_index];
    if(parent==-1) {
        return "GlobalFrame";
    }
    return joint_name[parent];
}
std::string skeleton_structure::parent_name(std::string const& name) const
{
    int N = size();
    for(int k=0; k<N; ++k) {
        if(name==joint_name[k]) {
            return parent_name(k);
        }
    }
    return "None";
}
cgp::numarray<int> skeleton_structure::child(int joint_index) const
{
    cgp::numarray<int> children_index;
    int N = size();
    for(int k=0; k<N; ++k) {
        if(parent_index[k]==joint_index) {
            children_index.push_back(k);
        }
    }
    return children_index;
}