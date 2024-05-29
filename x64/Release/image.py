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
        
        data_array.append([x, y, up, right, down, left])
    
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

def map_string_to_tile_index(pattern):
    """
    Maps a string pattern to a tile index based on the provided mappings.

    Parameters:
    pattern (tuple): The string pattern to map.

    Returns:
    int: The corresponding tile index.
    """
    ##---------
    # circuits
    ##---------
    # mapping = {
    #     ("AAA", "AAA", "AAA", "AAA"): [0],
    #     ("BBB", "BBB", "BBB", "BBB"): [1],
    #     ("BCB", "BBB", "BBB", "BBB"): [2],
    #     ("BBB", "BCB", "BBB", "BBB"): [3],
    #     ("BBB", "BBB", "BCB", "BBB"): [4],
    #     ("BBB", "BBB", "BBB", "BCB"): [5],
    #     ("BDB", "BBB", "BDB", "BBB"): [6],
    #     ("BBB", "BDB", "BBB", "BDB"): [7],
    #     ("BCB", "BBA", "AAA", "ABB"): [8],
    #     ("ABB", "BCB", "BBA", "AAA"): [9],
    #     ("AAA", "ABB", "BCB", "BBA"): [10],
    #     ("BBA", "AAA", "ABB", "BCB"): [11],
    #     ("BBA", "ABB", "BBB", "BBB"): [12],
    #     ("BBB", "BBA", "ABB", "BBB"): [13],
    #     ("BBB", "BBB", "BBA", "ABB"): [14],
    #     ("ABB", "BBB", "BBB", "BBA"): [15],
    #     ("BCB", "BBB", "BCB", "BBB"): [16,34],
    #     ("BBB", "BCB", "BBB", "BCB"): [17,35],
    #     ("BDB", "BCB", "BDB", "BCB"): [18],
    #     ("BCB", "BDB", "BCB", "BDB"): [19],
    #     ("BDB", "BBB", "BCB", "BBB"): [20],
    #     ("BBB", "BDB", "BBB", "BCB"): [21],
    #     ("BCB", "BBB", "BDB", "BBB"): [22],
    #     ("BBB", "BCB", "BBB", "BDB"): [23],
    #     ("BCB", "BCB", "BCB", "BBB"): [24],
    #     ("BBB", "BCB", "BCB", "BCB"): [25],
    #     ("BCB", "BBB", "BCB", "BCB"): [26],
    #     ("BCB", "BCB", "BBB", "BCB"): [27],
    #     ("BCB", "BCB", "BCB", "BCB"): [28, 29],
    #     ("BCB", "BCB", "BBB", "BBB"): [30],
    #     ("BBB", "BCB", "BCB", "BBB"): [31],
    #     ("BBB", "BBB", "BCB", "BCB"): [32],
    #     ("BCB", "BBB", "BBB", "BCB"): [33],
    # }
    ##---------
    # knots
    ##---------
    # mapping = {
    #     ("ABA","ABA","AAA","AAA"): [0],
    #     ("AAA","ABA","ABA","AAA"): [1],
    #     ("AAA","AAA","ABA","ABA"): [2],
    #     ("ABA","AAA","AAA","ABA"): [3],
    #     ("ABA","AAA","ABA","AAA"): [4],
    #     ("AAA","ABA","AAA","ABA"): [5],
    #     ("ABA","ABA","ABA","AAA"): [6],
    #     ("AAA","ABA","ABA","ABA"): [7],
    #     ("ABA","AAA","ABA","ABA"): [8],
    #     ("ABA","ABA","ABA","ABA"): [9],
    #     ("ABA","ABA","ABA","ABA"): [10,11]
    # }
    ##---------
    # circles
    ##---------
    # mapping = {
    #     ("AAA","AAA","AAA","AAA"): [0],

    #     ("AAA","BBB","BBB","BBB"): [1],
    #     ("BBB","AAA","BBB","BBB"): [2],
    #     ("BBB","BBB","AAA","BBB"): [3],
    #     ("BBB","BBB","BBB","AAA"): [4],

    #     ("AAA","AAA","BBB","BBB"): [5],
    #     ("BBB","AAA","AAA","BBB"): [6],
    #     ("BBB","BBB","AAA","AAA"): [7],
    #     ("AAA","BBB","BBB","AAA"): [8],

    #     ("AAA","BBB","AAA","BBB"): [9],
    #     ("BBB","AAA","BBB","AAA"): [10],

    #     ("BBB","AAA","AAA","AAA"): [11],
    #     ("AAA","BBB","AAA","AAA"): [12],
    #     ("AAA","AAA","BBB","AAA"): [13],
    #     ("AAA","AAA","AAA","BBB"): [14],

    #     ("BBB","BBB","AAA","AAA"): [15],
    #     ("AAA","BBB","BBB","AAA"): [16],
    #     ("AAA","AAA","BBB","BBB"): [17],
    #     ("BBB","AAA","AAA","BBB"): [18],

    #     ("BBB","AAA","BBB","AAA"): [19],
    #     ("AAA","BBB","AAA","BBB"): [20],
    # }

    mapping = {
        ("BAA","AAB","BBB","BBB"): [0],
        ("BBB","BAA","AAB","BBB"): [1],
        ("BBB","BBB","BAA","AAB"): [2],
        ("AAB","BBB","BBB","BAA"): [3],
        ("AAA","AAB","BBB","BAA"): [4],
        ("BAA","AAA","AAB","BBB"): [5],
        ("BBB","BAA","AAA","AAB"): [6],
        ("AAB","BBB","BAA","AAA"): [7],
        ("AAA","AAA","AAA","AAA"): [8,15,16],
        ("AAB","BAA","AAA","AAA"): [9],
        ("AAA","AAB","BAA","AAA"): [10],
        ("AAA","AAA","AAB","BAA"): [11],
        ("BAA","AAA","AAA","AAB"): [12],
        ("BBB","BBB","BBB","BBB"): [13,14,24,25],
        ("BAB","BAB","BAB","BAB"): [17],
        ("BBB","BAB","BBB","BBB"): [18],
        ("BBB","BBB","BAB","BBB"): [19],
        ("BBB","BBB","BBB","BAB"): [20],
        ("BAB","BBB","BBB","BBB"): [21],
        ("BBB","BAB","BBB","BAB"): [22],
        ("BAB","BBB","BAB","BBB"): [23],
        ("BAB","BAA","AAA","AAB"): [26],
        ("AAB","BAB","BAA","AAA"): [27],
        ("AAA","AAB","BAB","BAA"): [28],
        ("BAA","AAA","AAB","BAB"): [29],
    }

    return random.choice(mapping.get(pattern, [0]))

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
        x, y, up, right, down, left = int(row[0]), int(row[1]), row[2], row[3], row[4], row[5]
        
        pattern = (up, right, down, left)
        tile_index = map_string_to_tile_index(pattern)
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
    file_path = f"data/Data{suffix}.json"
    tilesheet_path = "input/tile4.png"  # Set the correct path to your tilesheet
    tile_size = (16, 16)  # Set the correct tile size (width, height)
    
    if not os.path.exists(file_path):
        print(f"Error: The file '{file_path}' does not exist.")
        sys.exit(1)
    
    data_array = read_json(file_path)
    tiles = load_tilesheet(tilesheet_path, tile_size)
    create_image(data_array, tiles, suffix, tile_size, 3)