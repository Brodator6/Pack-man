#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "Entities.h"

class Tile
{
protected:
    bool isWalkable;
public:
    virtual void DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);
    Tile(bool isWalkable);
    virtual ~Tile();
    virtual void ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile);
    bool GetIsWalkable() const;
};

class Floor : public Tile{
private:

public:
    void DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);
    void ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile);
    Floor();
    //Floor(bool isWalkable);
    ~Floor() override;

};

class Wall : public Tile{
private:

public:
    void DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);
    void ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile);
    Wall();
    //Wall(bool isWalkable);
    ~Wall() override;

};

class Point : public Tile{
private:
    int score = 10;
public:
    void DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);
    void ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile);
    Point(/* args */);
    ~Point() override;
};