| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

DIY Tracpod
==============================================
Phase 1:
ESP32 exhibits coexistence of gattc and gatts.
gattc -> GATT Client, will get data from TPMS tags 
gatts -> Will connect with mobile app and synchronize the Data (TPMS Data + GPS Data)

Phase 2:
Configure the tracpod to work with the CAN bus of vehicle, get that data and send to cloud.

Migrated the development platform to VSCODE and its extension
IDE -> VSCODE
Toolchain -> ESP IDF Vscode extension

Tutorial link: https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/toc.md

