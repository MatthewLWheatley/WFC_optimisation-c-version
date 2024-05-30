import json
from PIL import Image

def parse_json_and_create_image(file_path, scale_factor):
    with open(file_path, 'r') as file:
        data = json.load(file)

    # Parse EntropyList and prepare colors
    entropy_list = data['EntropyList']
    entropy_dict = {int(key): value for key, value in entropy_list.items()}

    # Define colors for each character
    color_map = {
        'A': (0, 0, 0),
        'B': (0, 255, 0),
        'C': (0, 0, 255),
        'D': (255, 0, 0),
        'E': (125, 0, 0),
        'F': (0, 125, 0),
        'G': (0, 0, 125),
    }
    neutral_color = (0, 0, 0)  # Black for any unexpected character and background

    # Determine the dimensions of the image
    max_x = (max(item['X'] for item in data['Grid']) + 1) * 3 * scale_factor
    max_y = (max(item['Y'] for item in data['Grid']) + 1) * 3 * scale_factor

    # Create an image with a black background
    img = Image.new('RGB', (max_x, max_y), (0, 0, 0))
    pixels = img.load()

    # Process each item in the Grid
    for grid_item in data['Grid']:
        if len(grid_item['Entropy']) == 1:
            entropy_id = grid_item['Entropy'][0]
            entropy_data = entropy_dict.get(entropy_id, None)
            if entropy_data:
                base_x = grid_item['X'] * 3 * scale_factor
                base_y = grid_item['Y'] * 3 * scale_factor
                directions = {
                    'Up': list(entropy_data['Up']),
                    'Down': list(entropy_data['Down']),
                    'Left': list(entropy_data['Left']),
                    'Right': list(entropy_data['Right'])
                }

                # Draw the detailed 3x3 block based on the entropy rules
                side_colors = []
                for i in range(3):
                    for j in range(3):
                        if i == 0:  # Top row - Up
                            char_index = j
                            direction = 'Up'
                        elif i == 2:  # Bottom row - Down
                            char_index = j
                            direction = 'Down'
                        elif j == 0:  # Left column - Left
                            char_index = i
                            direction = 'Left'
                        elif j == 2:  # Right column - Right
                            char_index = i
                            direction = 'Right'
                        else:
                            continue  # Skip center for now

                        color = color_map.get(directions[direction][char_index], neutral_color)
                        side_colors.append(color)
                        
                        # Apply the scale factor to each sub-block
                        for x in range(scale_factor):
                            for y in range(scale_factor):
                                pixels[base_x + j * scale_factor + x, base_y + i * scale_factor + y] = color

                # Calculate the average color for the center
                if side_colors:
                    avg_r = sum(color[0] for color in side_colors) // len(side_colors)
                    avg_g = sum(color[1] for color in side_colors) // len(side_colors)
                    avg_b = sum(color[2] for color in side_colors) // len(side_colors)
                    center_color = (avg_r, avg_g, avg_b)

                    # Apply the average color to the center
                    center_x = base_x + scale_factor  # Center position x
                    center_y = base_y + scale_factor  # Center position y
                    for x in range(scale_factor):
                        for y in range(scale_factor):
                            pixels[center_x + x, center_y + y] = center_color

    # Save or show the image
    img.save('output.png')
    img.show()

# Example usage
# Replace 'file.json' with the actual path to your JSON file
parse_json_and_create_image('file.json', 2)  # Adjust the scale factor as needed
