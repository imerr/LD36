# LD36
Ludum Dare 36

level format:
```json
{
  "texture": "assets/images/pyramid2.png", // texture for everything
  "backgroundTexture": "...", // path to background texture, optional - if not specified the desert one is loaded
  "preview": [LEFT, TOP, WIDTH, HEIGHT], // cutout that will be displayed as preview, optional
  "size": [68, 58], // size of the preview - will be scaled to fit the container
  "_c": "The scale saves us doing even more calculations by hand", // comment
  "scale": 5, // scale, simple multiplier
  "parts": [
    {
      "area": [0, 50, 62, 8],  // left, top, width, height of the part
      "goal": [0, 0], // goal position, top left of the part, relative to the top left corner of the first part
      "attach": [[25, 0], [-25, 0]], // two positions where the "ropes" are attached, relative to the center of the part
      "shapes": [[0, 0, 62, 8]] // all the physics shapes (rectangles) - left, top, width, height, relative to the center of the part (note: 1px is going to be cut off after scaling due to box2d)
    },
  ]
}
```