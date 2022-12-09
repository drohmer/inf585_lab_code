#include "gui.hpp"



bool display_interface(gui_parameters& gui)
{
	ImGui::Checkbox("Display frame", &gui.display_frame);

	ImGui::Checkbox("Surface", &gui.surface); ImGui::SameLine();
	ImGui::Checkbox("Wireframe", &gui.wireframe);

	// Create interface
	ImGui::Text("Surface type:"); ImGui::SameLine();  // Select surface to be deformed
	int* ptr_surface_type  = reinterpret_cast<int*>(&gui.surface_type); 
	bool new_surface = false;
    new_surface |= ImGui::RadioButton("Cylinder",ptr_surface_type, surface_cylinder); ImGui::SameLine();
    new_surface |= ImGui::RadioButton("Sphere",ptr_surface_type, surface_sphere); ImGui::SameLine();
    new_surface |= ImGui::RadioButton("Cube",ptr_surface_type, surface_cube);  ImGui::SameLine();
    new_surface |= ImGui::RadioButton("Mesh",ptr_surface_type, surface_mesh);
	
	ImGui::Text("Grid: "); ImGui::SameLine();
	ImGui::Checkbox("point", &gui.display_grid_sphere); ImGui::SameLine();
	ImGui::Checkbox("edge", &gui.display_grid_edge);

	return new_surface;
}