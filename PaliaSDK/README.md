# OriginPalia requires the Palia SDK to build.
You must dump the SDK yourself using Unreal Engine SDK dumpers such as Dumper-7.
Simply place the SDK folder and files into this folder, build, and enjoy!

Void from the OriginSoftware community has released a Dumper DLL to use for easy dumping!
If you'd like to use this edition, please download Dumper 7 from the link below

🖥️ [Download Dumper 7](https://github.com/Wimberton/OriginPalia/blob/main/PaliaSDK/Dumper7_V3.dll)

## Step 1. Dump Palia
Dump the PaliaClient by injecting this DLL into the game when in the character-selection screen

## Step 2. Move Palia Dump Files
Move the dumped files into our OriginPalia `PaliaSDK`folder.
Dumped files will be default stored in `C:\Dumper-7\`

### Files & Folders To Move
Simply move the entire PaliaSDK folder into the project to use the dumped SDK files. Below is a list of the files and folders for your reference
PaliaSDK\
- SDK\
-  NameCollisions.inl
-  PropertyFixup.hpp
-  SDK.hpp
-  UnrealContainers.hpp


## Step 3. Fixing Variables
If your offsets are stored as constant expressions, browse to the `Basic.hpp` file and change the following variables to the new ones.

`constexpr int32 GObjects          = 0x089FD0C0;` to `inline int32 GObjects          = 0x089FD0C0;`

`constexpr int32 AppendString      = 0x00CFE170;` to `inline int32 AppendString      = 0x00CFE170;`

`constexpr int32 GNames            = 0x08956700;` to `inline int32 GNames            = 0x08956700;`

`constexpr int32 GWorld            = 0x08B6DAF8;` to `inline int32 GWorld            = 0x08B6DAF8;`

`constexpr int32 ProcessEvent      = 0x00ED7AA0;` to `inline int32 ProcessEvent      = 0x00ED7AA0;`

`constexpr int32 ProcessEventIdx   = 0x0000004D;` to `inline int32 ProcessEventIdx   = 0x0000004D;`

If your variable names are incorrect, browse to the `WBP_Cooking_Minigames_PF_parameters.hpp` file and change the following variables to the new ones.

`Temp_real_Variable_1` to `Temp_real_Variable_1_3`

`Temp_real_Variable_2` to `Temp_real_Variable_2_3`

