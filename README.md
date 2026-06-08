# Zoe2Mars_Redcase

Zoe2Mars_Redcase is an attempt to allow researcher to reverse engine the PS2 / PC Mars version while providing an in-game mod panel for the PC version with a bunch of features such as extracting/overriding cygames files, allowing modding to some extent.</br>

Many Assets names were recovered back from their hash due to leftover in the .bin files, especially the short stage name used by the development,Their meaning properly written in the mod panel :)
<img width="1026" height="608" alt="image" src="https://github.com/user-attachments/assets/be70beec-7382-49d9-aa77-2281bb426323" /></br>
<img width="1026" height="608" alt="image" src="https://github.com/user-attachments/assets/e2bb6b23-2b88-481f-af31-64e2838e3683" />

# Known Issues
Note: The PS2 version(s) have no issues, ZOE2 PS2 SpecialEdition and Mars use the same STAGE.DAT.</br>
* Due to Denuvo, one last patch is missing to allow to play with a decrypted STAGE.DAT for the full version.
* Building a new STAGE.DAT will render the game unable to play many Audio/Music since they wrapped the .sdx (Sound Pak) around wWise audio banks.
* Due to how the game work, Sending a `LoadRequest` while a FMV is playing, will freeze the game thread.

### Roadmap
- [x] Extraction/Creation of STAGE.DAT.
- [x] Cygames Packages hook, Extraction/Overrides, Allowing modding.
- [x] In-Game mod panel with a ZOE2 Theme.
- [x] CreateFileA Hook allowing targeted modification for files in the STAGE.DAT while the game run, mostly for analysis purpose.
- [ ] Add Renderdoc injection for the game.
- [ ] Patches to play with a decrypted STAGE.DAT.
- [x] Demo Version Patches to play with a decrypted STAGE.DAT.

# Building the source
Visual Studio 2026 with C++ 20, vcpkg manifest will be used to download the dependencies when you build.</br>
Solution Configuration marked with (Demo) will only build the mod with the "Demo Version" offsets.</br>
Files built will be output in `Zoe2Mars_Redcase\x64\Release or Debug\Bin`.

### Special Thanks
[@Surihix](https://github.com/Surihix) For feedback about the "ZOE2" Theme made for the Mod Panel.</br>
### This project use
[imgui](https://github.com/ocornut/imgui)</br>
[cli11](https://github.com/cliutils/cli11)</br>
[Detours](https://github.com/microsoft/Detours)</br>
[vcpkg](https://github.com/microsoft/vcpkg/)
