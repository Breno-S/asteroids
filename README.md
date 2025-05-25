My attempt to clone the classic arcade game *Asteroids* and learn game programming along the way. Developed entirely in C, using [Raylib](https://github.com/raysan5/raylib).

## 💽 Installation

### Download Release

If you just want to play the game, go to the [releases page](https://github.com/Breno-S/asteroids/releases) and download the appropiate version for your platform.

### Build From Source

Prerequisites: C compiler (MVSC, GCC, Clang, etc.)

#### Using CMake

Prerequisites: CMake 3.28+

1. Clone the repository:
   
   ```
   git clone git@github.com:Breno-S/asteroids.git
   cd asteroids
   ```

2. Inside the project folder, create a `build` directory for CMake:

	```
	mkdir build
	cd build
	```

3. Then run the following commands:

	```sh
	cmake ..
	cmake --build .
	```

    The first command (CMake configuration) may take a while to complete — it will download Raylib, among other things. The second command builds the executable in the parent directory (the project root).

#### Building Manually

Prerequisites: Raylib 5.5

If you're already familiar with Raylib and C, just make sure the library files are available and compile/link with the appropriate flags. If you're using GCC/Clang, run a command similar to this inside the project root:

```sh
# library and headers globally installed
cc src/*.c -Iinclude -lraylib <platform_specific_flags> -o <output_name>

# library and headers installed elsewhere
cc src/*.c -Iinclude -I<include_directory> -L<library_directory> -lraylib <platform_specific_flags> -o <output_name>
```

Replacing `<platform_specific_flags>` as follows:

| Platform | Flags |
|-|-|
| **Windows** | `-lwinmm` `-lgdi32` `-Wl,--subsystem,windows` |
| **Linux**   | `-lm`     |

## 🕹 How to play

Run the *game* executable inside the project root, where the *assets* folder is located.

### Gameplay
Destroy asteroids and flying saucers to score points while avoiding collisions and enemy fire. Every 10000 points earns you an extra spaceship. Use hyperspace as a last resort to escape certain death, but beware: there's a small chance of exploding upon re-entry.

<p align="center"><img src="https://github.com/user-attachments/assets/748c1f02-d09e-4c7f-86ef-3ef87167f891"></img></p>

### Controls

- **Movement** - `W` `A` `D`  
- **Shooting** - `J`  
- **Hyperspace** - `K`  
- **Pause** - `SPACE`

## 📄 License

This project is licensed under the terms of the MIT License.
See the [LICENSE](https://github.com/Breno-S/asteroids/blob/main/LICENSE) file for more details.

All visual assets used in this project were created by me and are also licensed under the MIT license.

The fonts used were downloaded from [*dafont.com*](https://www.dafont.com/hyperspace.font). They are used here for personal and non-commercial purposes, in accordance with their EULA.

Some sound effects used in this project were downloaded from [*Classicgaming.cc*](https://www.classicgaming.cc/classics/asteroids/sounds). These sounds are the property of their respective copyright holders and are used here for educational and demonstration purposes only.

**This project is non-commercial and does not intend to infringe on any copyright.** If you are the copyright holder and wish to have the files removed, please contact me.

---

**Disclaimer**: This is a learning project inspired by Atari's *Asteroids*. It is not affiliated with, sponsored by, or licensed by Atari.
