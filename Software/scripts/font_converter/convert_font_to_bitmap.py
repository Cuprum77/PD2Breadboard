from PIL import Image, ImageFont, ImageDraw
import numpy as np
import string
from textwrap import wrap


# Define font file and color
font_file = "RobotoMono-SemiBold.ttf"
# Standard font sizes
font_size = 24
# Output directory
output_dir = "../../lib/Display/Fonts/"

# Grab the font
font = ImageFont.truetype(font_file, font_size)
# Get the widest character width (usually 'W' or 'M')
char_box = font.getbbox("W")
max_width = char_box[2] - char_box[0]
# Calculate how much we need to crop the bitmap to get the actual character
crop = int((font_size - max_width) / 2)
# Get the tallest character height (usually '(' )
char_box = font.getbbox("(")
height_offset = int((font_size - char_box[3] - char_box[1]) / 2)

# Function that takes the bad string.printables list and returns it in proper Ascii order
def GetAscii():
    Ascii = []

    # Add all the characters in order
    for i in range(32, 127):
        Ascii.append(chr(i))

    return Ascii


# Function that converts a character to a bitmap
def Convert(char, size):
    # Load font and create image object
    image = Image.new('1', (size, size), color=0)
    # Get the font size
    bounding_box = font.getbbox(char)
    print(bounding_box)

    # Calculate offset to center text in bitmap
    x_offset = int((size - bounding_box[2] - bounding_box[0]) / 2)
    # I genuinely cannot be bothered with figuring this out as of now
    # So here we add a simple hack to fix the dumb letters that are too tall such as f and t
    # If its between 97 and 122 its a lowercase letter and unless its a t or f, we need to increase the offset by 1
    local_offset = height_offset
    if 97 <= ord(char) <= 122 and char != 'f' and char != 't':
        local_offset -= 1

    # Create image and draw text
    image = Image.new('1', (size, size), color=0)
    draw = ImageDraw.Draw(image, mode='1')
    draw.text((x_offset, local_offset), char, font=font, fill=1, antialias=True)

    # Convert image to numpy array and flatten
    bitmap = np.array(image)
    bitmap = bitmap[:, crop:-crop]
    return bitmap.flatten(), int(size - crop * 2)


# Function to compress the data
def Compress(data):
    # We will compress it by only yielding the number of 0s and 1s after each other
    blacks = 0
    whites = 0

    for i, pixel in enumerate(data):
        if pixel == 1:
            whites += 1
        else:
            blacks += 1

        if i == 0 or data[i - 1] == pixel:
            continue

        if pixel == 1:
            yield blacks
            blacks = 0
        else:
            yield whites
            whites = 0

    # Handle the last pixel
    if whites != 0:
        yield whites
    elif blacks != 0:
        yield blacks


# Write out the character as a comment if it is not a regular character
def CharComment(char):
    if char == '\\':
        return "Character Backslash"
    elif char == '\n':
        return "Character Newline"
    elif char == '\r':
        return "Character Carriage Return"
    elif char == '\t':
        return "Character Tab"
    else:
        return "Character {}".format(char)


# Pad zeroes to the list to make them all equal length
def PadZeroes(list):
    desired_length = 0

    for sub_list in list:
        length = len(sub_list)
        if length > desired_length:
            desired_length = length

    for sub_list in list:
        length = len(sub_list)
        if length < desired_length:
            num_zeroes_to_add = desired_length - length
            sub_list.extend([0] * num_zeroes_to_add)

    return list


# Draw the symbols as a sanity check
def Draw_Symbol(data, width):
    # Keep track of the row
    out = ""

    # Get each index of the list
    for i in range(len(data)):
        dataWidth = data[i]
        # If the index is odd, its colored
        if i % 2 == 0:
            # Print out the black characters as #
            out = out + "#" * dataWidth
        # If the index is even, its white
        else:
            # Print out the white characters as spaces
            out = out + "." * dataWidth

    # Add a newline after every width characters
    out = wrap(out, width)

    # Print the output
    for row in out:
        print(row)


def Generate_File(size):
    output_file = f"Font"
    new_width = size

    # Get all Ascii characters
    AsciiList = GetAscii()

    # Generate all the ascii characters
    Ascii = []
    for char in AsciiList:
        # Ignore Ascii values below 32
        if ord(char) < 32:
            continue

        # Convert character to bitmap
        bitmap, new_width = Convert(char, size)

        bitmap = list(Compress(bitmap))
        Ascii.append(bitmap)

    # Pad all the lists with zeroes
    Ascii = PadZeroes(Ascii)

    # Output bitmap as C header file
    with open(output_dir + output_file + ".h", "w") as f:
        f.write("#pragma once\n\n")
        memory_usage = len(AsciiList) * len(Ascii[0] * 4)   # We need to remember that a word is 4 bytes
        f.write("// Estimated memory usage: {:,} bytes\n\n".format(memory_usage).replace(",", " "))
        f.write("// Number of characters\n")
        f.write(f"#define FONT_CHAR\t\t\t{len(AsciiList)}\n")
        f.write("// Number of bytes in a character\n")
        f.write(f"#define FONT_DATA\t\t\t{len(Ascii[0])}\n")
        f.write(f"// Font bitmap width\n")
        f.write(f"#define FONT_WIDTH\t\t\t{new_width}\n")
        f.write(f"// Font bitmap height\n")
        f.write(f"#define FONT_HEIGHT\t\t\t{size}\n")
        f.write(f"// Font bitmap shortcut\n")
        f.write(f"#define FONT(character)\t\tfont_bitmap(character)\n\n")
        # Create the C constant
        f.write(f"static const unsigned int font_bitmaps[][FONT_DATA] = {{\n")

        # Convert each letter to a bitmap
        index = 0
        for char in AsciiList:
            # Ignore Ascii values below 32
            if ord(char) < 32:
                continue

            # Write the constant name
            f.write("\t{")

            # Loop through each pixel and write to file
            for i in range(len(Ascii[index])):
                # Convert to Octal
                f.write("0x{:02x}".format(Ascii[index][i]))
                # Only append a comma if its not the last pixel
                if i != len(Ascii[index]) - 1:
                    f.write(",")

            # Write the comment describing the character
            f.write(f"}},\t//{CharComment(char)} ({ord(char)})\n")

            # Sanity check
            print(f"{char}:")
            Draw_Symbol(Ascii[index], new_width)
            index += 1

        f.write("};\n\n")
        
        # Output character lookup table
        f.write("static const unsigned int* font_bitmap(const char c) {\n")
        f.write("\tif (c < 0x20 || c > 0x7e) return nullptr;\n")
        f.write("\t\treturn font_bitmaps[c - 0x20];\n")
        f.write("}\n")


# Loop through each font size and generate a file
Generate_File(font_size)
