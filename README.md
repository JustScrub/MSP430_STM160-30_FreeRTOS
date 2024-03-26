# MSP430FR5994 FreeRTOS measuring temperature using STM160-30
This project aims to do what's in the title. The repo contains sources for the task, inits of different clocks and IO port, and Code Composer Studio project configuration. See below for more info. If you prefer coding in VS Code, see more below.

## Code Composer Studio project for Windows    
You need sources of FreeRTOS port to MSP430, ideally in the directory `../FreeRTOS_MSP430`, relative to the root directory of this project. If on any other path, this must be adjusted in CCS > Project > Show Build Settings... > Include Options (under MSP430 Compiler settings).

## VS Code project
You need all the MSP430 files included in CCS, so the best shot is to install CCS with MSP430 support.    
To code in VS Code, create `.vscode` directory and the file `.vscode/c_cpp_properties.json` with the following:

```
{
    "configurations": [
        {
            "name": "MSP430",
            "defines": ["__MSP430FR5994__", "_MPU_ENABLE"],
            "includePath": [
                "${workspaceFolder}/**",
                "/path/to/FreeRTOS_MSP430/include",
                "/path/to/FreeRTOS_MSP430/portable/CCS/MSP430X",
                "/CCS/install_dir/code_composer_studio/installation/ccs/ccs_base/msp430/include",
                "/CCS/install_dir/code_composer_studio/installation/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include"
            ],
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}
```

where `/path/to/FreeRTOS_MSP430/` is the path to your FreeRTOS port and `/CCS/install_dir` is the directory where your CCS is installed. 

Uploading to device and compilation is easiest done in CCS. Use VS Code to write code and CCS to build/upload...