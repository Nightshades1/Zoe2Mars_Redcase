# Zoe2Mars_Redcase

Zoe2Mars_Redcase is an attempt to allow researcher to reverse engine the PS2 / PC Mars version while providing an in-game mod panel for the PC version with a bunch of features such as extracting/overriding cygames files, allowing modding to some extent.</br>

Many Assets names were recovered back from their hash due to leftover in the .bin files, especially the short stage name used by the development,Their meaning properly written in the mod panel :)</br></br>
Head over the [Redcase Wiki](https://github.com/Nightshades1/Zoe2Mars_Redcase/wiki) for all informations.</br>
<img width="90%" height="90%" alt="image" src="https://github.com/user-attachments/assets/74d203c9-8c24-4d0c-a37b-05b409f49707" />
</br>
<img width="90%" height="90%" alt="image" src="https://github.com/user-attachments/assets/f247c62a-7092-4743-9caa-3252773e3752" />
</br>
<img width="90%" height="90%" alt="image" src="https://github.com/user-attachments/assets/31de4a99-401f-4ba4-aa0e-7696393502a9" />

# Known Issues
Note: The PS2 version(s) have no issues, ZOE2 PS2 SpecialEdition and Mars use the same STAGE.DAT.</br>
* Due to how the game work, Sending a `LoadRequest` while a FMV is playing, will freeze the game thread.

### Roadmap
- [x] Extraction/Creation of STAGE.DAT.
- [x] Cygames Packages hook, Extraction/Overrides, Allowing modding.
- [x] In-Game mod panel with a ZOE2 Theme.
- [x] CreateFileA Hook allowing targeted modification for files in the STAGE.DAT while the game run, mostly for analysis purpose.
- [x] Fix Wwise Audio issue.
- [x] Add Renderdoc injection for the game.
- [x] Patches to play with a decrypted STAGE.DAT.
- [ ] Custom wwise events override (custom bank support)

# Building the source
Visual Studio 2026 with C++ 20 and cmake, vcpkg manifest will be used to download the dependencies when you build.</br>
Simply open the folder, head to the cmake target and right click on the project then "install" once built everything will be inside the `out/install/<x64 Release|x64 Debug>/`

### Special Thanks
[@Surihix](https://github.com/Surihix) For feedback about the "ZOE2" Theme made for the Mod Panel.</br>
### This project use
[imgui](https://github.com/ocornut/imgui)</br>
[cli11](https://github.com/cliutils/cli11)</br>
[Detours](https://github.com/microsoft/Detours)</br>
[vcpkg](https://github.com/microsoft/vcpkg/)</br>
[wwise](https://www.audiokinetic.com/)
