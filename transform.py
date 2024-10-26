import trimesh

if __name__ == "__main__":

    for i in range(1, 7):
        # Load the .obj file with the correct format
        mesh = trimesh.load(f'part ({i}).obj')

        # Export the file to .off format
        mesh.export(f'test{i}.off')