# OriginPalia <a href="https://www.unknowncheats.me/forum/palia/636934-originpalia-feature-packed-multitool-imagine.html"><img align="right" src="https://i.gyazo.com/7e7b0b3f8bd20565233fe2f3fb08d250.png" width="64" height="auto"></a>
<img align="right" src="https://i.gyazo.com/5254871e345d926e03be4757590b6eac.png" width="auto" height="auto">

A multi-purpose tool for Palia, written in C++. Open source for non-commercial usage, looking for contributors.

♥ [Download Latest Injector](https://github.com/Wimberton/OriginPalia/releases/tag/Injector)

♥ [Download Latest DLL](https://github.com/Wimberton/OriginPalia/releases/tag/release)

♥ [Changelogs](https://github.com/Wimberton/OriginPalia/wiki/DLL%E2%80%90Changelogs)

♥ [Wiki](https://github.com/Wimberton/OriginPalia/wiki)

♥ [Discord Server](https://discord.gg/originsoftware)

OriginPalia comes packed with general enhancements like Instant Fishing, ESPs, Silent Aim, Teleport to entities, and more. OriginPalia is still in active development, there may be a few bugs.

Big thanks to @klukule for their [Palia ESP DX11 / DX12 source on GitHub](https://github.com/klukule/PaliaHook). Without that, I wouldn't have taken the time to build something from it.

Thanks to @VoidPollo for the continued work, restructuring, and mainteinace! A real CHAD in the team! Couldn't do this without you!

## Compatibility
- Standalone Client Support
- Steam Palia Support

## Usage
- Toggle the menu using INSERT

<img align="right" src="https://i.gyazo.com/c12d0c130c168678cfe9ab9dbc946c2a.png" width="auto" height="auto">

You will need to dump the SDK of the game yourself in order for this to work. Simply add the latest game SDK files into the `PaliaSDK` folder to reallocate the required SDK files.
More information can be found at: [PaliaSDK README](https://github.com/Wimberton/OriginPalia/tree/main/PaliaSDK)
### Dependencies (included):
- ImGUI
- GLEW
- Detours

<img align="right" src="https://i.gyazo.com/a31227e25a080e65054a4737a4baa6e1.png" width="auto" height="auto">

### ESP & Visuals:
- Enable ESP *(enable or disable drawing ESPs in-game)*
- Limit ESP Distance *(change the distance in which ESPs will show)*
- Show unclassified entities *(shows potentially unnamed or unmapped entities. Good for getting more from updates etc)*

### Player & Entities ESP
Players, NPCs, Fish, Fish Pools, Loot, Quests, Rummage Piles, and Stables.
### Ores ESP
Options to toggle visibility and color customization for various sizes (Small, Medium, Large) of ores like Clay, Stone, Copper, Iron, and Palium.
### Forageables ESP
Toggle settings for various types of forageables, each with normal and starred versions. Coral, Oyster, Shells, various flowers, moss types, mushrooms, spices, and vegetables.
### Trees ESP
Control visibility and customize color settings for various types and sizes of trees, including Bush, Sapwood, Heartwood, and Flow-Infused types.
### Animals ESP
Quality tiers such as Sernuk, Chapaa, and Muujin, including special categories like minigame-specific animals.
### Bugs ESP
Common, uncommon, rare, and epic qualities. Manage settings for bugs like Bees, Beetles, Butterflies, Cicadas, Crabs, Crickets, Dragonflies, Glowbugs, Ladybugs, Mantises, Moths, Pedes, and Snails.

### **Selling & Items:
- Quick Sell Items interface *(sell preset or custom amounts of items from your inventory slots)*

### Aimbots & Fun:
- Silent Aimbot *(within ~25-30m or shots won't teleport and validate)*
- Legacy Aimbot *(work-in-progress / camera boom angle changes when aiming. Working on predictive angle offsetting during this bow transition)*
- Aim Smoothing slider *(makes your aim less sticky. Easier to target other animals)*
- Aim Offset Adjustment draggable grid *(should help adjust the issue with bow aiming camera boom changes for now)*
- Enable FOV Circle *(FOV circle with advanced targeting functionality based on entity scoring)*
- Teleport To Targeted entity *(bottom side mouse button)*
- Anti AFK *(never get kicked for inactivity, helpful during long fishing sessions)*
- Avoid teleporting to players *(safeguard for avoiding teleportation to player entities)*
- Avoid teleporting when players are near *(safeguard for potentially exposing your cheat to others.)*
- Teleport Dropped Loot To Player *(automatically teleport all dropped loot to your player to gather)*

### Movement & Teleportation:
- Enable NoClip *(fly around the map without restrictions. Hooks your WASD keys for movement)*
- Teleport to map waypoint *(Instantly teleport to your map waypoint for faster traversal)*
- Selectable Movement Modes (Walking, Flying, Fly No Collision) *(another way to mess with your movement types)*
- Global Game Speed input *(changes your global game speed [timescale mod])*
- Walk Speed input *(change the speed of your player walking. Has issues above "high")*
- Climbing Speed input *(change the speed of your player climbing. Should work all the way)*
- Gliding Speed input *(change the speed of your player gliding. Haven't noticed any issues)*
- Gliding Fall Speed input *(set this to "none" for 1.0 gliding fall speed. Glide forever and get where you want)*
- Jump Velocity input *(change the jump height of your player)*
- Step Height input *(change the step height of your player. Step over massive walls and such)*

### Fishing Settings:
- Enable Instant Fishing *(force end fishing when your bobber hits the water)*
- Perfect Catch option *(choose whether your catch was perfect or not every time)*
- Enable Fast Fishing *(Automatically re-cast your fishing rod, with the option for left-click requirement or fully autonomous)*
- Instant Sell All Fish *(Automatically sell all fish from every inventory slot)*
- Discard Non-Fish *(Automatically discard non-fish items from your inventory when fishing. Open a shop once for this to work)*
- Open Waterlogged Chests & Store Them *(Automatically opens all waterlogged chests and sends the item to your home storage during fishing)*
- Force fishing pool *(selectable dropdown of available fishing pools to force fish from)*

### Locations & Coordinates:
- Display current coordinates *(use these coordinates to perform certain actions)*
- Teleport to locations on list *(a good list of easily teleportable locations to efficiently perform tasks such as shopping, repairing, etc)*
- Teleport to gatherables on list *(a good list of easily teleportable locations to efficiently collect gatherables around the world)*
- Custom coordinate input *(enter or get custom coordinates to perform certain actions)*
- Buttons for teleport actions *(teleport home, to locations, and other actions)*

### Housing Features:
- Place housing objects anywhere *(decorate your house anyway you'd like)*

<img align="right" src="https://i.gyazo.com/5054c7286fd254ebd39a52f196b05a20.png" width="auto" height="auto">

<p align="left">
  <img src="https://i.gyazo.com/c8ae73a455e9047cf11b14996c345249.jpg" width="350" title="OriginPalia ESP Example">
  <img src="https://i.gyazo.com/6ee8348d80d3d3260de686bef860e4f7.gif" width="350" alt="OriginPalia Menu Example">
</p>
