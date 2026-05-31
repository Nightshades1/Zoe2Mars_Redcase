// pch.h : Il s'agit d'un fichier d'en-tête précompilé.
// Les fichiers listés ci-dessous sont compilés une seule fois, ce qui améliore les performances de génération des futures builds.
// Cela affecte également les performances d'IntelliSense, notamment la complétion du code et de nombreuses fonctionnalités de navigation du code.
// Toutefois, les fichiers listés ici sont TOUS recompilés si l'un d'entre eux est mis à jour entre les builds.
// N'ajoutez pas de fichiers fréquemment mis à jour ici, car cela annule les gains de performance.

#ifndef PCH_H
#define PCH_H
// ajouter les en-têtes à précompiler ici
#pragma comment(lib, "d3d11")
#pragma comment(lib, "detours")
#include <windows.h>
#include <detours/detours.h>
#include <iostream>
#include <thread>
#include <string>
#include <stdint.h>
#include <zlib.h>
#include <sstream>
#include <algorithm>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <fstream>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#endif //PCH_H
