import json
import sys
from PIL import Image
import os
import random

def read_json(file_path):
    """
    Reads a JSON file and returns the data in the required format.

    Parameters:
    file_path (str): The path to the JSON file.

    Returns:
    list: A list of rows with the image data in the required format.
    """
    with open(file_path, 'r') as file:
        data = json.load(file)
    
    entropy_list = data["EntropyList"]
    grid = data["Grid"]
    
    data_array = []
    for grid_item in grid:
        x = grid_item["X"]
        y = grid_item["Y"]
        entropy_index = grid_item["Entropy"][0]
        entropy_data = entropy_list[str(entropy_index)]
        up = entropy_data["Up"]
        right = entropy_data["Right"]
        down = entropy_data["Down"]
        left = entropy_data["Left"]
        spriteIndex = entropy_data["spritePos"]
        
        data_array.append([x, y, up, right, down, left,spriteIndex])
    
    return data_array

def load_tilesheet(tilesheet_path, tile_size):
    """
    Loads a tilesheet image and extracts individual tiles.

    Parameters:
    tilesheet_path (str): The path to the tilesheet image.
    tile_size (tuple): The size of each tile (width, height).

    Returns:
    list: A list of tile images.
    """
    tilesheet = Image.open(tilesheet_path)
    tilesheet_width, tilesheet_height = tilesheet.size
    tiles = []
    num_tiles_x = tilesheet_width // tile_size[0]
    num_tiles_y = tilesheet_height // tile_size[1]
    
    for y in range(num_tiles_y):
        for x in range(num_tiles_x):
            tile = tilesheet.crop((x * tile_size[0], y * tile_size[1],
                                   (x + 1) * tile_size[0], (y + 1) * tile_size[1]))
            tiles.append(tile)
    
    return tiles

def create_image(data_array, tiles, suffix, tile_size, scale_factor=2):
    """
    Creates an image based on the data array using tiles and saves it with the given suffix.

    Parameters:
    data_array (list): A list of rows containing the image data.
    tiles (list): A list of tile images.
    suffix (str): A suffix for the output file name.
    tile_size (tuple): The size of each tile (width, height).
    scale_factor (int): Factor by which the image will be scaled.
    """
    max_x = max(int(row[0]) for row in data_array)
    max_y = max(int(row[1]) for row in data_array)
    width = (max_x + 1) * tile_size[0]
    height = (max_y + 1) * tile_size[1]
    
    output_image = Image.new("RGB", (width, height), "white")

    for row in data_array:
        x, y, up, right, down, left, spriteIndex = int(row[0]), int(row[1]), row[2], row[3], row[4], row[5], row[6]
        
        pattern = (up, right, down, left)
        tile_index = random.choice(spriteIndex)
        tile = tiles[tile_index]
        
        output_image.paste(tile, (x * tile_size[0], y * tile_size[1]))

    # Scale the image using nearest neighbor interpolation
    scaled_width = width * scale_factor
    scaled_height = height * scale_factor
    scaled_image = output_image.resize((scaled_width, scaled_height), Image.NEAREST)

    if not os.path.exists('output'):
        os.makedirs('output')

    output_filename = f"output/image{suffix}.png"
    scaled_image.save(output_filename)
    #scaled_image.show()
    
if __name__ == "__main__":
    suffix = sys.argv[1] if len(sys.argv) > 1 else ""
    tilesheet_path = sys.argv[2] if len(sys.argv) > 2 else "input/tile1.png"  # Default to "input/tile4.png" if not provided
    file_path = f"data/Data{suffix}.json"

    tilesheet = Image.open(tilesheet_path)
    tilesheet_width, tilesheet_height = tilesheet.size
    tile_size = (tilesheet_height, tilesheet_height)  # Set the correct tile size (width, height)
    
    if not os.path.exists(file_path):
        print(f"Error: The file '{file_path}' does not exist.")
        sys.exit(1)
    
    data_array = read_json(file_path)
    tiles = load_tilesheet(tilesheet_path, tile_size)
    create_image(data_array, tiles, suffix, tile_size, 3)