cmake_minimum_required(VERSION 3.23)

project(MyProjectName)

add_executable(Pac-Man)

target_sources(Pac-Man
    PRIVATE
    Main.cpp
    Tile.cpp
    TileSystem.cpp
    UpdateInput.cpp
    UpdateState.cpp
    GUI/Windows/GUI.h
    GUI/Windows/MainMenuWindow.cpp
    GUI/Windows/GameWindow.cpp
    GUI/Windows/ErrorWindow.cpp
    GUI/Elements/GUIElement.cpp
    Entities/Entity.cpp
    Entities/Player.cpp
    Entities/Enemy.cpp
    Entities/Pathfinding/Pathfinding.cpp
    Entities/AI/BehaviorTree.cpp
)