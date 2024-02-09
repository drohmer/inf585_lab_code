#include "simulation.hpp"
#include "../../third_party/eigen/Eigen/Core"
#include "../../third_party/eigen/Eigen/SVD"



using namespace cgp;

// Compute the polar decomposition of the matrix M and return the rotation such that
//   M = R * S, where R is a rotation matrix and S is a positive semi-definite matrix
mat3 polar_decomposition(mat3 const& M);

// Compute the collision between the particles and the walls
void collision_with_walls(std::vector<shape_deformable_structure>& deformables);

// Compute the collision between the particles to each other
void collision_between_particles(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param);

// Compute the shape matching on all the deformable shapes
void shape_matching(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param);




// Perform one simulation step (one numerical integration along the time step dt) using PPD + Shape Matching
void simulation_step(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param)
{
    float dt = param.time_step;
    int N_deformable = deformables.size();

    // I. - Apply the external forces to the velocity
    //    - Compute the predicted position from this time integration
    vec3 const gravity = vec3(0.0f, 0.0f, -9.81f);
    for(int kd=0; kd<N_deformable; ++kd) { // For all the deformable shapes
        shape_deformable_structure& deformable = deformables[kd];
        int N_vertex = deformable.position.size();
        for(int k=0; k<N_vertex; ++k) // For all the vertices of each deformable shape
        {
            // Standard integration of external forces
            //   drag + gravity
            deformable.velocity[k] = deformable.velocity[k]*(1-dt*param.friction) + dt*gravity;  
            //   predicted position
            deformable.position_predict[k] = deformable.position[k] + dt*deformable.velocity[k]; 
        }
    }

    // II. Constraints using PPD
    //     - Collision with the walls (particles/walls)
    //     - Collision between particles (particles/particles)
    //     - Apply shape matching (per shape)
    // Note: The parameter collision_steps can be modified by the gui interface
    for(int k_collision_steps=0; k_collision_steps<param.collision_steps; ++k_collision_steps){

        collision_with_walls(deformables);
        collision_between_particles(deformables, param);
        shape_matching(deformables, param);

    }


    // III. Final velocity update
    for(int kd=0; kd<N_deformable; ++kd) 
    {
        shape_deformable_structure& deformable = deformables[kd];

        for(int k=0; k<deformable.size(); ++k)
        {
            // Update velocity
            deformable.velocity[k] = (deformable.position_predict[k]-deformable.position[k])/dt;

            // Update the vertex position
            deformable.position[k] = deformable.position_predict[k];
        }
    }
	
}


// Compute the shape matching on all the deformable shapes
void shape_matching(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param)
{
    // Arguments:
    //   deformables: stores a vector of all the deformable shape
    //   Each deformable shape structure contains 
    //        - the predicted position [deformable].position_predicted
    //        - the center of mass [deformable].com
    //        - the reference position [deformable].position_reference
    //        - the center of mass from the reference position [deformable].com_reference


    // ********************************************** //
    // TO DO: Implement here the Shape Matching
    // ********************************************** //

    // Algorithm:
    //   - For all deformable shapes
    //     - Update the com (center of mass) from the predicted position
    //     - Compute the best rotation R such that p_predicter - com = R (p_reference-com_reference)
    //        - Compute the matrix M = \sum r r_ref^T
    //            with r  = p_predicted - com
    //                 r_ref = p_reference - com_reference
    //        - Compute R as the polar decomposition of M
    //     - Set the new predicted position as p_predicted = R (p_reference-com_reference) + com
    // 
    //
    // Help:
    //   - A function "mat3 polar_decomposition(mat3 const& M)" that computes the polar decomposition using SVD is provided.
    //  - The center of mass of the predicted position can be computed as "average(deformable.position_predicted)"
    //  - A matrix mat3 can be initialized to zeros with the syntax "mat3 M = mat3::build_zero();"
    //  - The product a * b^tr, where (a,b) are vec3 is also called a tensor product. It can be computed using the syntax "mat3 M = tensor_product(a,b)"
    //

}




void collision_between_particles(std::vector<shape_deformable_structure>& deformables, simulation_parameter const& param)
{
    float r = param.collision_radius; // radius of colliding sphere

    // Prepare acceleration structure using axis-aligned bounding boxes.
    std::vector<bounding_box> bbox;
    int N_deformable = deformables.size();
    for(int kd=0; kd<N_deformable; ++kd) {
        bounding_box b;
        b.initialize(deformables[kd].position_predict);
        b.extends(r);
        bbox.push_back(b);
    }
    // Can test if two bounding box (bbox1,bbox2) collide using
    //   bool is_in_collision = bounding_box::collide(bbox1, bbox2);
    // These bounding box are an optional possibility to accelerate the computation to avoid checking pairwise collisions between particles of shapes that are far away.


    // ********************************************** //
    // TO DO: Implement here the collision between particles from different deformable shapes
    // ********************************************** //

    // Algorithm:
    //  - For all the deformable shapes (d_i,d_j)
    //    - For all the vertices(/particles) of the shapes (p_i,p_j)
    //      - If ||p_i-p_j|| < 2 r // collision state
    //           Then modify (p_i,p_j) to remove the collision state

}


// Compute the collision between the particles and the walls
// Note: This function is already pre-coded
void collision_with_walls(std::vector<shape_deformable_structure>& deformables)
{
    int N_deformable = deformables.size();
    for(int kd=0; kd<N_deformable; ++kd) {
        shape_deformable_structure& deformable = deformables[kd];
        int N_vertex = deformable.size();
        for(int k=0; k<N_vertex; ++k)
        {
            vec3& p = deformable.position_predict[k];
            
            // Standard collision with the walls in x and y. 
            //  Modify these values for different parameters
            if(p.x<-1) p.x = -1;
            if(p.x>5)  p.x = 5;
            if(p.y<-1) p.y = -1;
            if(p.y>5)  p.y = 5;

            // Collision with the ground
            if(p.z<0) {
                p.z = 0;
                // model friction with the ground
                p.x = deformable.position[k].x;
                p.y = deformable.position[k].y;
            }

         }
    }
}


// Compute the polar decomposition of the matrix M and return the rotation such that
//   M = R * S, where R is a rotation matrix and S is a positive semi-definite matrix
mat3 polar_decomposition(mat3 const& M) 
{
    // The function uses Eigen to compute the SVD of the matrix M
    //  Give: SVD(M) = U Sigma V^tr
    //  We have: R = U V^tr, and S = V Sigma V^tr
	Eigen::Matrix3f A; A << M(0,0),M(0,1),M(0,2), M(1,0),M(1,1),M(1,2), M(2,0),M(2,1),M(2,2);
	Eigen::JacobiSVD<Eigen::Matrix3f> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::Matrix3f const R = svd.matrixU()* (svd.matrixV().transpose());

	return {R(0,0),R(0,1),R(0,2), R(1,0),R(1,1),R(1,2), R(2,0),R(2,1),R(2,2)};
}