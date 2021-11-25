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
Advance Offset: Use this to automatically move the pixel offset to the end of the currently loaded image.
<br/>
<br/>
4-bit anf 8-bit: Causes the image viewer to treat the pixel data as being 4-bit or 8-bit, respectively. These have not been fully tested yet, so they may not work properly.
<br/>
<br/>
Prev and Next: These move the palette offset to the previous or next possible palette as guessed by an algorithm. This algorithm is not ideal because it skips palettes which have duplicate colors and includes many candidates which are not actually palettes, but it should help narrow down the possibilities.
