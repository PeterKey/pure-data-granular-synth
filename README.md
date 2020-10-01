# grainmaker~
A Pure Data external providing granular synthesis possibilities.

## Functionality
The grainmaker~ external is capable of spawning grains of a provided sample around a certain position. You can dynamically specify a center position and an offset, and within this range grains will constantly be spawned. The length and number of grains can be changed at runtime.

## Installation
Either copy the build files into your pd/externals folder or add this repository to your pure data search path.

## Build
You can either build through Xcode by opening the project file or by running `make` in the `Grainmaker` directory.

## Future improvements
- [ ] Improve grain blending function
- [ ] Normalise volume when using a lot of grains
- [ ] Provide more grain playback possibilities (speed, direction)
- [ ] Implement support to input lists for multiple value ranges
- [ ] Windows support


