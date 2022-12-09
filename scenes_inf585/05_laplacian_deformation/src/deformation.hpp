#pragma once

#include "cgp/cgp.hpp"
#include <set>


// Include Eigen
#define EIGEN_NO_DEBUG
#define EIGEN_INITIALIZE_MATRICES_BY_ZERO
#include "../third_party/eigen/Eigen/Sparse"
#include "../third_party/eigen/Eigen/SVD"


// 2 types of constraints:
//  - Fixed points (associated to weight_target)
//  - Target points that can be displaced (associated to weight_target)
struct constraint_structure {

	// Store constraints as (index in mesh, 3D target position)
	std::map<int, cgp::vec3> fixed;
	std::map<int, cgp::vec3> target;

	// Associated weights for the least square solution
	float weight_fixed = 2.0f; // fixed points are almost hard constraints
	float weight_target = 0.1f; // smaller weights for target points
};

// Structure containing the element of the system related to the quadratic energy
//   E = || M q = rhs ||^2
struct linear_system_structure
{
    Eigen::SparseMatrix<float> M; // The coefficients are stored in a Sparse Matrix
    Eigen::LeastSquaresConjugateGradient< Eigen::SparseMatrix<float> > solver;  // Conjugate gradient solver

    Eigen::VectorXf rhs_x, rhs_y, rhs_z;       // System Right-hand-Side
    Eigen::VectorXf guess_x, guess_y, guess_z; // Initial coordinates used as initial guess solution

};


void build_matrix(linear_system_structure& linear_system, constraint_structure const& constraints, cgp::mesh const& shape, cgp::numarray<cgp::vec3> const& initial_position, cgp::numarray<cgp::numarray<int> > const& one_ring);

void update_deformation(linear_system_structure& linear_system, constraint_structure const& constraints, cgp::mesh& shape, cgp::mesh_drawable& visual, cgp::numarray<cgp::vec3> const& initial_position, cgp::numarray<cgp::numarray<int> > const& one_ring);
