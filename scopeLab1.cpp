#define HARD_CODED_INPUT_PATH "D:\\SCOPE_LAB\\Code\\test4.off"

#include <fstream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <nlohmann/json.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/detect_features.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/alpha_wrap_3.h>

using json = nlohmann::json;
namespace PMP = CGAL::Polygon_mesh_processing;

typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3>                   Mesh;
typedef Kernel::Point_3                                       Point_3;
typedef Kernel::Vector_3                                      Vector_3;
typedef Kernel::Plane_3                                       Plane_3;

Mesh mesh;

// Check if a file exists at the given path
static bool check_path_exists(const std::string& path) {
	DWORD ftyp = GetFileAttributesA(path.c_str());

	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;
	}

	return true;
}

// Load mesh from file
static void load_mesh(std::string path, Mesh& mesh) {
	if (!check_path_exists(path)) {
		std::cerr << "ERROR: Path: \"" << path << "\" not exists." << std::endl;
		throw std::exception();
	}

	if (!PMP::IO::read_polygon_mesh(path, mesh) || CGAL::is_empty(mesh) || !is_triangle_mesh(mesh)) {
		std::cerr << "ERROR: Invalid input:" << path << std::endl;
		throw std::exception();
	}

	std::cout << "INFO: Mesh loaded with: " << num_vertices(mesh) << " vertices, " << num_edges(mesh) << " edges, " << num_faces(mesh) << " faces." << std::endl;
}

// Extract directory path from full path
static std::string get_path(const std::string& fullPath) {
	size_t lastSlash = fullPath.find_last_of("/\\");

	if (lastSlash == std::string::npos) {
		return "";
	}

	return fullPath.substr(0, lastSlash);
}

// Extract filename without extension from path
static std::string get_basename(const std::string& path) {
	size_t lastSlash = path.find_last_of("/\\");

	std::string filename;

	if (lastSlash == std::string::npos) {
		filename = path;
	}
	else {
		filename = path.substr(lastSlash + 1);
	}

	size_t lastDot = filename.find_last_of(".");

	if (lastDot != std::string::npos) {
		return filename.substr(0, lastDot);
	}

	return filename;
}

// Detect and analyze mesh edges
static void detect_edges(Mesh& mesh) {
	try {
		// Create JSON object to store edge data
		json edge_data;
		edge_data["feature_edges"] = json::array();
		edge_data["border_edges"] = json::array();

		// Set angle threshold for feature detection (in degrees)
		double angle_threshold = 60.0;

		// Add property maps for edge features
		auto edge_is_feature = mesh.add_property_map<Mesh::Edge_index, bool>("e:is_feature", false).first;

		// Detect sharp edges and set the property map
		PMP::detect_sharp_edges(mesh, angle_threshold, edge_is_feature);

		// Iterate over edges and add feature edges to JSON if marked
		for (const auto& e : mesh.edges()) {
			if (edge_is_feature[e]) {
				Mesh::Halfedge_index h = mesh.halfedge(e);
				Mesh::Vertex_index v1 = mesh.source(h);
				Mesh::Vertex_index v2 = mesh.target(h);
				Point_3 p1 = mesh.point(v1);
				Point_3 p2 = mesh.point(v2);

				edge_data["feature_edges"].push_back({
					{"start", {p1.x(), p1.y(), p1.z()}},
					{"end", {p2.x(), p2.y(), p2.z()}}
					});
			}
		}

	
		 // Process border edges
		auto edge_is_border = mesh.add_property_map<Mesh::Edge_index, bool>("e:is_border", false).first;

		for (const Mesh::Edge_index& e : mesh.edges()) {
			Mesh::Halfedge_index h = mesh.halfedge(e);
			if (mesh.is_border(h) || mesh.is_border(mesh.opposite(h))) {
				edge_is_border[e] = true;

				Mesh::Vertex_index v1 = mesh.source(h);
				Mesh::Vertex_index v2 = mesh.target(h);
				Point_3 p1 = mesh.point(v1);
				Point_3 p2 = mesh.point(v2);

				edge_data["border_edges"].push_back({
					{"start", {p1.x(), p1.y(), p1.z()}},
					{"end", {p2.x(), p2.y(), p2.z()}}
					});
			}
		}
		// Save edge data to JSON file
		std::string json_path = get_path(HARD_CODED_INPUT_PATH) + "/" +
			get_basename(HARD_CODED_INPUT_PATH) + "_edges.json";
		std::ofstream json_file(json_path);
		json_file << std::setw(4) << edge_data << std::endl;
		std::cout << "Edge data saved to: " << json_path << std::endl;

	}
	catch (const std::exception& e) {
		std::cerr << "Error in edge detection: " << e.what() << std::endl;
		throw;
	}
}

static void output_mesh(std::string path, Mesh& mesh) {
	std::cout << "INFO: Writing alpha wrapped model to \"" << path << "\"" << std::endl;

	CGAL::IO::write_polygon_mesh(path, mesh, CGAL::parameters::stream_precision(17));
}


int main(int argc, char* argv[]) {
	// std::string input_pth = std::string(argv[1]);

	std::string input_pth = HARD_CODED_INPUT_PATH;

	//mesh_preprocess(input_pth);
	load_mesh(input_pth, mesh);
	detect_edges(mesh);

	return EXIT_SUCCESS;
}