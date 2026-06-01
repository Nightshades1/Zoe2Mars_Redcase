# Zoe2Mars_Redcase

Zoe2Mars_Redcase is an attempt to allow researcher to reverse engine the PS2 / PC Mars version while providing an in-game mod panel for the PC version with a bunch of features such as extracting/overriding cygames files, allowing modding to some extent.</br>
<img width="1026" height="608" alt="image" src="https://github.com/user-attachments/assets/be70beec-7382-49d9-aa77-2281bb426323" />

# Known Issues
Note: The PS2 version(s) have no issues, ZOE2 PS2 SpecialEdition and Mars use the same STAGE.DAT.</br>
* Due to Denuvo, one last patch is missing to allow to play with a decrypted STAGE.DAT for the full version.
* Building a new STAGE.DAT will render the game unable to play many Audio/Music since they wrapped the .sdx (Sound Pak) around wWise audio banks.
* Due to how the game work, Sending a `LoadRequest` while a FMV is playing, will freeze the game thread.

### Roadmap
- [x] Build an app to allow the extraction/creation of the STAGE.DAT.
- [x] Cygames Packages hook, Extraction/Overrides, Allowing modding.
- [x] In-Game mod panel with a ZOE2 Theme.
- [ ] Add Renderdoc injection for the game.
- [ ] Patches to play with a decrypted STAGE.DAT.
- [x] Demo Version Patches to play with a decrypted STAGE.DAT.

# Setting up Zoe2 Mars
For now only the Demo is supported, the full version will be supported later however to debug the game, the Demo version is better, it doesn't have Denuvo and it is in fact the full game with some engine features removed, as long as you have all the assets from the full version.
However the entire game can be played in addition to the vs mode and the extra missions that can be opened via the in-game mod panel.

1. Copy your installation from the full version over your demo installation at `Steam\steamapps\common\ZONE OF THE ENDERS THE 2nd RUNNER MARS ANUBIS ZONE OF THE ENDERS MARS ORANGE CASE`.

2. Download the files at [Assets/Steam/Demo Version](Assets/Steam/Demo%20Version) and copy `Zoe2Mars.exe`,`steam_appid.txt` to your game directory, The .exe has been patched to disable all STAGE.DAT decryption, allowing you to start the game directly with IDA.

3. (Skip if not the demo version) Extract `cygames pac.zip` in your game directory.

4. it is recommended to make a `Backup` folder and to copy/paste the original STAGE.DAT before using `StageDec`, See [StageDec - STAGE.DAT Extraction/Build Tool](https://github.com/Nightshades1/Zoe2Mars_Redcase/blob/master/README.md#stagedec---stagedat-extractionbuild-tool).

5. Once you have extracted all files from the original `STAGE.DAT` by using StageDec, you will have the decrypted one output where StageDec is located, you can either copy/paste it to your game/ZOE2 folder and start `Zoe2Mars_Redcase_Launcher.exe` or mess around and build a new STAGE.DAT for research/testing purpose.

### Using the Mod Panel
Most features have a tooltip about what it does, just hover the (?).
* Ins/Insert key to open/hide the panel.
* If you want to intercept the cygames files, such as extracting only the files of your language as they are readt by the game, you can do so by enabling the option in the mod panel -> Settings.</br>

Alternatively you can extract all cygames pack by using https://github.com/Surihix/CyArchiveTool then move all the cygames .pac elsewhere, the game will then load the files from the game directory instead of the .pac, you can also see the change at runtime if you reload the stage/menu in question.

# Setting up Zoe2 (Demo) and Special Edition (PS2)
Games data aren't provided, only the patched .elf and empty folders are, you will need to download the ps2 demo of the 2nd runner, or the full game of the 2nd runner (SpecialEdition).</br>
</br>
The demo version is recommended since it has debug features quite helping.

1. I recommend to open `PCSX2` -> `Tool` -> `Open Data Directory` and extract your game `.iso` in the PCSX2 `games` folder, you can delete the original `.elf` and `system.cnf`.

2. Download the [ps2_patches.zip](Assets/PS2/ps2_patches.zip) then Extract the `patches` folder inside your PCSX2 `Data Directory`.

3. Download StageDec (See Bellow), Decrypt the `STAGE.DAT` (See Bellow on how to use) and copy it to your ZOE2 directory.</br>

### Configuring the game / Testing your changes
Note: To enable the debug menu, set the `DebugFlag` to 1 inside the `Debugger`->`Globals`, open the pause menu in-game and press L1.</br>
1. The .elf of both games have been patched so that it load the files from the game directory,</br>
be sure that in `PCSX2`->`Settings` the `Enable Host Filesystem` is ticked.

2. This must be done on both games, Right click -> `Properties` -> `Patches` and tick `Disable Decryption` that will allow you to play with a fully decrypted STAGE.DAT.

3. Still in the properties, go to `Debug` -> in the `Import Symbols` add the one found in the game folder named `globals.sym`.

If everything is fine, you booted the game with a fully decrypted `STAGE.DAT`, Now you can open the PCSX2 Debugger, </br>
when you click on the `Globals` tab, the `globals.sym` will reveal some Address/Varibles that i added.</br>
</br>
You can replace the `STAGE.DAT` at runtime but `be sure that you aren't adding or removing bytes or the game will crash`, In the `Globals` window, simply set the value of `LoadRequest` to `4113` the game will reload the stage and re-initialize the data.</br>
</br>
Do not reload the stage during a FMV or the game thread will be locked.

# StageDec - STAGE.DAT Extraction/Build Tool
Can be downloaded from the latest mod [releases](https://github.com/Nightshades1/Zoe2Mars_Redcase/releases).
You may have a look at the [Samples batch files](Samples) on how to use StageDec, in addition the CLI have his own example/help, single Backward slashes also work.
### USAGE EXAMPLE
```
stage:
  extract -> StageDec.exe stage -e -p "C:/games/zoe2/ZOE2/backup/STAGE.DAT"
  build   -> StageDec.exe stage -b

hash:
  hash a string          -> StageDec.exe hash -s jeh_def.mdz
  hash a list of strings -> StageDec.exe hash -l <path>/MyList.txt
```

# Building the source
Visual Studio 2026 with C++ 20, vcpkg manifest will be used to download the dependencies when you build.</br>
Solution Configuration marked with (Demo) will only build the mod with the "Demo Version" offsets.</br>
Files built will be output in `Zoe2Mars_Redcase\x64\Release or Debug\Bin`.

# Special Thanks
[imgui](https://github.com/ocornut/imgui)</br>
[cli11](https://github.com/cliutils/cli11)</br>
[Detours](https://github.com/microsoft/Detours)</br>
[vcpkg](https://github.com/microsoft/vcpkg/)</br>
[@Surihix](https://github.com/Surihix) For feedback about the "ZOE2" Theme made for the Mod Panel.
