#ifndef _Entities_H_
#define _Entities_H_

#include <string>
#include <map>
#include <SDL3/SDL.h>

class Entity
{
private:
    SDL_FRect rect;
    int xPosition;
    int yPosition;
public:
    int GetPositionX() const;
    int GetPositionY() const;
    void SetPosition(int X, int Y);
    
    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);
    
    //void DoMovement(int level[10][10]);
    
    Entity(/* args */);
    Entity(int x, int y);
    ~Entity();
};

class Player: public Entity
{
private:
    std::string name;
    int score = 0;

public:
    std::map<SDL_Scancode, bool> movement;

    int GetScore() const;
    void AddScore(int extraPoints);
    std::string GetName() const;
    void UpdateState(int level[10][10]);

    Player(int x, int Y, std::string name = "player", std::map<SDL_Scancode, bool> movement = {{SDL_SCANCODE_UP, false}, {SDL_SCANCODE_RIGHT, false}, {SDL_SCANCODE_DOWN, false}, {SDL_SCANCODE_LEFT, false}});
    ~Player();

};


#endif