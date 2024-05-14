# OriginPalia <img align="right" src="https://i.gyazo.com/7e7b0b3f8bd20565233fe2f3fb08d250.png" width="64" height="auto">
<p align="left">
  <img src="https://i.gyazo.com/c8ae73a455e9047cf11b14996c345249.jpg" width="350" title="OriginPalia ESP Example">
  <img src="https://i.gyazo.com/6ee8348d80d3d3260de686bef860e4f7.gif" width="350" alt="OriginPalia Menu Example">
</p>

A multi-purpose tool for Palia, written in C++ and C#. Open source for non-commercial usage, looking for contributors.

<p>OriginPalia comes packed with general enhancements like Instant Fishing, ESPs, Silent Aim, Teleport to entities, and more. OriginPalia is still in active development, there may be a few bugs.</p>
<p>I will continue to maintain and update OriginPalia for as long as I can, permitted I have the time.</p>
<p>I want to give a big thanks to @klukule for their Palia ESP DX11 / DX12 source on GitHub. Without that, I wouldn't have taken the time to build something from it.</p>

## Compatibility
- Standalone Client Support
- Steam Palia Support

## Usage
- Toggle the menu using INSERT

## Features
### ESP & Visuals:
- Enable ESP *(enable or disable drawing ESPs in-game)*
- Limit ESP Distance *(change the distance in which ESPs will show)*
- Show unclassified entities *(shows potentially unnamed or unmapped entities. Good for getting more from updates etc)*

**Player & Entities ESP** - 
Players, NPCs, Fish, Fish Pools, Loot, Quests, Rummage Piles, and Stables.

**Ores ESP** - 
Options to toggle visibility and color customization for various sizes *(Small, Medium, Large)* of ores like Clay, Stone, Copper, Iron, and Palium.

**Forageables ESP** - 
Toggle settings for various types of forageables, each with normal and starred versions. Coral, Oyster, Shells, various flowers, moss types, mushrooms, spices, and vegetables.

**Trees ESP** - 
Control visibility and customize color settings for various types and sizes of trees, including Bush, Sapwood, Heartwood, and Flow-Infused types.

**Animals ESP** - 
Quality tiers such as Sernuk, Chapaa, and Muujin, including special categories like minigame-specific animals.

**Bugs ESP** - 
Common, uncommon, rare, and epic qualities. Manage settings for bugs like Bees, Beetles, Butterflies, Cicadas, Crabs, Crickets, Dragonflies, Glowbugs, Ladybugs, Mantises, Moths, Pedes, and Snails.

## Aimbots & Fun:
- Enable Silent Aimbot *(within ~25-30m or shots won't teleport and validate)*
- Enable Legacy Aimbot *(work-in-progress / camera boom angle changes when aiming. Working on predictive angle offsetting during this bow transition)*
- Aim Smoothing slider *(makes your aim less sticky. Easier to target other animals)*
- Aim Offset Adjustment draggable grid *(should help adjust the issue with bow aiming camera boom changes for now)*
- Enable InteliAim Circle *(FOV circle with advanced targeting functionality based on entity scoring)*
- Teleport To Targeted entity *(top mouse button)*
- Teleport Target entity to you *(bottom mouse button)*
- Teleport Dropped Loot To Player *(automatically teleport all dropped loot to your player to gather)*
- Send Animals To Orbit Around Player *(fun mod - work-in-progress / animals may invalidate animals. Working on disabling the AI brain component)*

## Movement & Teleportation:
- Enable NoClip *(fly around the map without restrictions. Hooks your WASD keys for movement)*
- NoClip fly speed slider *(change the speed of your NoClip flying)*
- Selectable Movement Modes *(Walking, Flying, Fly No Collision) (another way to mess with your movement types)*
- Global Game Speed slider *(changes your global game speed [timescale mod])*
- Movement Velocity slider *(work-in-progress / should help calculate your current player velocity and smoothly increase it as you move. Might end up bypassing movement validations.)*
- Walk Speed slider *(change the speed of your player walking. Has issues above "high")*
- Climbing Speed slider *(change the speed of your player climbing. Should work all the way)*
- Gliding Speed slider *(change the speed of your player gliding. Haven't noticed any issues)*
- Gliding Fall Speed slider *(set this to "none" for 1.0 gliding fall speed. Glide forever and get where you want)*
- Jump Velocity slider *(change the jump height of your player)*
- Step Height slider *(change the step height of your player. Step over massive walls and such)*
- Sprint Speed Multiplier *(work-in-progress / modify your sprint multiplier)*

## Selling & Items:
- You must open a shop in order for selling to work. Getting a pointer to the shop happens when you instantiate the component. I'm looking into a better way to instantiate it when loading into a main map world
- Quick Sell Items interface
- Select bag, slot, and quantity
- Custom quantity input

## Fishing Settings:
- Enable Instant Fishing *(force end fishing when your bobber hits the water)*
- Start and End Rod Health sliders *(customize the total health of your fishing rod)*
- Start and End Fish Health sliders *(customize the total health of the fish when caught)*
- Capture and override fishing spot options *(capture and fish from any previous fishing pool)*
- Perfect Catch option *(choose whether your catch was perfect or not every time)*
- Instant Sell (Slot 1) *(automatically sell fish from your bag 1 slot 1. Open a shop once for this to work)*

## Locations & Coordinates:
- Display current coordinates *(use these coordinates to perform certain actions)*
- List of teleport locations *(a good list of easily teleportable locations to efficiently perform tasks such as shopping, repairing, etc)*
- Custom coordinate input *(enter or get custom coordinates to perform certain actions)*
- Buttons for teleport actions *(teleport home, to locations, and other actions)*

## Building
You will need to dump the SDK of the game yourself in order for this to work. Simply add the latest game SDK files into the `PaliaSDK` folder to reallocate the required SDK files.

### Dependencies (included):
- ImGUI
- GLEW
- Detours
