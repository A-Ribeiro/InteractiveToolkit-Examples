> For this is how God loved the world:  
he gave his only Son, so that everyone  
who believes in him may not perish  
but may have eternal life.  
  \
John 3:16

Use examples of the [https://github.com/A-Ribeiro/InteractiveToolkit](https://github.com/A-Ribeiro/InteractiveToolkit) library.

## How to Clone?

This library uses git submodules.

You need to fetch the repo and the submodules also.

__HTTPS__

```bash
git clone --recurse-submodules https://github.com/A-Ribeiro/InteractiveToolkit-Examples.git
```

__SSH__

```bash
git clone --recurse-submodules git@github.com:A-Ribeiro/InteractiveToolkit-Examples.git
```

## How to build?

```bash
cd InteractiveToolkit-Examples/
mkdir build
cd build
cmake ..
make -j
```

All binaries will be generated at ```InteractiveToolkit-Examples/build/bin``` folder.

## Example Projects

 - **src/astar-on-roguelike-matrix**: Roguelike map generation with A* path computing over it.

 - **src/callback-handling**: std::bind and std::function alternative with easier assignment and comparison for functions, methods and lambdas.

 - **src/ease-core**: Several damping animation equations.

 - **src/event-handling**: Event treatment with support to subscribe several listeners to one event controller(dispatcher). Can be used with functions, methods and lambdas.

 - **src/math-core**: Example of drawing a rotating cube in the terminal using the lib matrices and lib vectors.

 - **src/on-abort-handling**: Handling abort event in runtime.

 - **src/pattern-match**: Pattern match examples. Exact matching and approximated matching.

 - **src/property**: Example of a property. When the internal value change, this class dispatch an event notifying all listeners its modification.

 - **src/random**: Several random generation examples.

 - **src/sorting**: O(n) Sort algorithm example (RadixSort).

## Authors

***Alessandro Ribeiro da Silva*** obtained his Bachelor's degree in Computer Science from Pontifical Catholic 
University of Minas Gerais and a Master's degree in Computer Science from the Federal University of Minas Gerais, 
in 2005 and 2008 respectively. He taught at PUC and UFMG as a substitute/assistant professor in the courses 
of Digital Arts, Computer Science, Computer Engineering and Digital Games. He have work experience with interactive
software. He worked with OpenGL, post-processing, out-of-core rendering, Unity3D and game consoles. Today 
he work with freelance projects related to Computer Graphics, Virtual Reality, Augmented Reality, WebGL, web server 
and mobile apps (andoid/iOS).

More information on: https://alessandroribeiro.thegeneralsolution.com/en/
