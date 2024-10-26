import json
import numpy as np
import pyvista as pv
import trimesh
import os

# Visualize and Save Function with adjusted camera settings
def visualize_and_save(json_path, obj_path, output_png=None):
    """
    Visualize edges with the original mesh and optionally save as PNG
    
    Parameters:
    - json_path: path to the edge data JSON file
    - obj_path: path to the original mesh file (.obj or other formats)
    - output_png: path to save the PNG file (optional)
    """
    # Create PyVista plotter with off_screen mode for headless environments
    pv_plotter = pv.Plotter(window_size=[1024, 768], off_screen=True)
    
    # Load edge data
    with open(json_path, 'r') as f:
        edge_data = json.load(f)
    
    # Load and add the original mesh
    mesh = pv.read(obj_path)
    pv_plotter.add_mesh(mesh, style='wireframe', color='gray', opacity=0.3, lighting=False)
    
    # Add feature edges (red)
    for edge in edge_data['feature_edges']:
        start = edge['start']
        end = edge['end']
        line = pv.Line(start, end)
        pv_plotter.add_mesh(line, color='red', line_width=4)
    
    # Add border edges (blue)
    for edge in edge_data['border_edges']:
        start = edge['start']
        end = edge['end']
        line = pv.Line(start, end)
        pv_plotter.add_mesh(line, color='blue', line_width=4)
    
    # Add legend
    pv_plotter.add_legend([
        ('Original Mesh', 'gray'),
        ('Feature Edges', 'red'),
        ('Border Edges', 'blue')
    ])
    
    # Set the camera position to isometric and reset to ensure full view
    pv_plotter.view_isometric()  # Set to an isometric view
    pv_plotter.reset_camera()    # Auto zoom to fit the entire object
    
    # Optional zoom adjustment if needed
    pv_plotter.camera.zoom(1.2)  # Increase or decrease the value as needed
    
    # Capture the screenshot as an image
    image = pv_plotter.screenshot(output_png)
    
    # Save to PNG if specified
    if output_png:
        print(f"Screenshot saved to: {output_png}")
    
    # Return image for any additional processing
    # return image

def save_multiple_views(json_path, obj_path, output_dir):
    """
    Save multiple views of the mesh and edges as PNG files
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Create plotter without showing
    plotter = pv.Plotter(window_size=[1024, 768], off_screen=True)
    
    # Load edge data
    with open(json_path, 'r') as f:
        edge_data = json.load(f)
    
    # Load and add the original mesh
    mesh = pv.read(obj_path)
    plotter.add_mesh(mesh, style='wireframe', color='gray', opacity=0.3)
    
    # Add feature edges in red
    for edge in edge_data['feature_edges']:
        start, end = edge['start'], edge['end']
        line = pv.Line(start, end)
        plotter.add_mesh(line, color='red', line_width=4)
    
    # Add border edges in blue
    for edge in edge_data['border_edges']:
        start, end = edge['start'], edge['end']
        line = pv.Line(start, end)
        plotter.add_mesh(line, color='blue', line_width=4)
    
    # Add legend
    plotter.add_legend([
        ('Original Mesh', 'gray'),
        ('Feature Edges', 'red'),
        ('Border Edges', 'blue')
    ])
    
    # Define camera views with direction vectors
    views = {
        'front': (1, 0, 0),
        'back': (-1, 0, 0),
        'top': (0, 1, 0),
        'bottom': (0, -1, 0),
        'left': (0, 0, 1),
        'right': (0, 0, -1),
        'isometric': 'iso'
    }
    
    # Save each view
    for view_name, direction in views.items():
        if view_name == 'isometric':
            plotter.view_isometric()  # Isometric view
        else:
            plotter.camera_position = direction  # Set camera direction
        
        plotter.reset_camera()  # Ensure the view fits in frame
        plotter.camera.zoom(1.5)  # Adjust zoom as needed
        
        # Define output path and take a screenshot
        output_path = os.path.join(output_dir, f'view_{view_name}.png')
        plotter.screenshot(output_path)
        print(f"Saved {view_name} view to: {output_path}")
    
    # Close the plotter after saving all views
    plotter.close()

if __name__ == "__main__":
    
    # Define File Paths and Call Functions
    json_path = "test6_edges.json"
    obj_path = "test6.off"  # can be .obj, .off, or other formats
    output_dir = "test6_output_views"

    # visualize_and_save(
    #     json_path=json_path,
    #     obj_path=obj_path,
    #     output_png="output_view.png"
    # )

    save_multiple_views(
        json_path=json_path,
        obj_path=obj_path,
        output_dir=output_dir
    )