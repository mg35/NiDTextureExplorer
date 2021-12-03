# NiDTextureExplorer

OPEN FILE: Open a file from Saturn NiGHTS into Dreams. The file must be decompressed (not in PRS format) to view it properly.
<br/>
<br/>
SAVE FILE: Save the currently loaded image as a .bmp file. 
<br/>
<br/>
LOAD IMAGE: Load the file data into the image viewer. Use this after you have entered a custom offset or palette offset.
<br/>
<br/>
Width and Height: Move these sliders to change the width and height of the image currently being viewed. The pixel grid will automatically update as you do this.
<br/>
<br/>
Offset and Palette boxes: Enter a hexadecimal number in these boxes to choose an offset for the pixel data and palette data, respectively.
<br/>
<br/>
Arrows for Offset and Palette: Click these to increment the offset or palette offset. The leftmost arrows move the offset in increments of 1 byte, the middle ones use increments of 4 bytes, and the rightmost ones use increments of 16 bytes.
<br/>
<br/>
Advance Offset/Advance Palette: Use this to automatically move the pixel offset to the end of the currently loaded image or palette, respectively.
<br/>
<br/>
4-bit anf 8-bit: Causes the image viewer to treat the pixel data as being 4-bit or 8-bit, respectively.
<br/>
<br/>
Prev and Next: These move the palette offset to the previous or next possible palette as guessed by an algorithm. This algorithm is not ideal because it skips palettes which have duplicate colors and includes many candidates which are not actually palettes, but it should help narrow down the possibilities.
<br/>
<br/>
Default Palette: Loads a default grayscale palette. This is useful in cases where a texture's actual palette is missing.
