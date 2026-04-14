#pragma once
#include <vector>
#include <memory>

class GUI;

class MenuManager
{
private:
    std::vector<std::unique_ptr<GUI>> menus;
    std::unique_ptr<GUI> pendingRoot = nullptr;
    bool pendingPop = false;

public:
    void PushMenu(std::unique_ptr<GUI> newMenu){
        menus.push_back(std::move(newMenu));
    };

    void PopMenu(){
        if(!menus.empty()){
            menus.pop_back();
        }
    };

    bool IsEmpty(){
        return menus.empty();
    }

    GUI* GetRootMenu(){
        return menus.front().get();
    };

    GUI* GetMenu(){
        return menus.back().get();
    };

    void RequestRootSwap(std::unique_ptr<GUI> newMenu) {
        pendingRoot = std::move(newMenu);
    }

    void RequestMenuPop(){
        pendingPop = true;
    }

    void applyPendingRequest() {
        if (pendingPop) {
            this->PopMenu();
            pendingPop = false;
        }
        if (!pendingRoot) return;

        menus.clear(); 
        menus.push_back(std::move(pendingRoot));
        
        pendingRoot = nullptr; // Reset the request

    }

    MenuManager() = default;
    ~MenuManager()  = default;
};
