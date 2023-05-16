//
// Created by Owner on 4/28/2023.
//

#ifndef SDL_PLAYERCONTROLLER_HPP
#define SDL_PLAYERCONTROLLER_HPP

#include <stdint.h>
#include <variant>

#include "../../Engine/Renderer/Renderer.hpp"
#include "../../Engine/Renderer/Event.hpp"
#include "../../Engine/Renderer/Animation/Animate.hpp"
#include "../../Engine/Utility/Types/Types.hpp"
#include "../../World/World.hpp"

class PlayerController {
private:
    bool _PlayerInCombatStance = false;
    enum class StanceType {
        None,
        Melee, // X Button To Enter This Stance
        Magic // Y Button To Enter This Stance
    } _StanceType;
    Timer _PlayerEnterCombatStance{};
    Timer _PlayerAttackTimer{};
    int32_t _PlayerDefenceRecoveryTime = 0;
    Timer _PlayerDefenseTimer{};
    int32_t _PlayerAttackRecoveryTime = 0;

    int32_t _AnimationWaitCount = 0;
    bool _AnimationIsComplete = true;
    bool _AnimationIsComplete2 = true;

    SDL_Rect _TempItemOffset = {};
    float _TempItemRotation = 0.0f;

// Combat
    // Blocks Incoming Attacks
    void block(Item& heldItem, SDL_RendererFlip flip);
    // Parries Incoming Attacks
    inline void parry(Item& heldItem, SDL_RendererFlip flip) {
        printf("Block: Parry\n");
        _PlayerDefenceRecoveryTime = 200;
    }
    // Generic Slash Downwards
    void swing(Item& heldItem, SDL_RendererFlip flip);
    // Thrust Attach
    void stab(Item& heldItem, SDL_RendererFlip flip);
    // Stab But Launches You Forward
    // The `posX` Is To Offset The World
    void lunge(Item& heldItem, SDL_RendererFlip flip, int32_t& posX);
    // Swings Upwards, Potentially Knocking Enemies Upwards
    inline void launch(Item& heldItem, SDL_RendererFlip flip);

public:
    PlayerController() = default;

    void update(Event& event, Item& heldItem, SDL_RendererFlip flip, v2<int,int>& worldOffset);

    inline bool playerInCombatStance() { return _PlayerInCombatStance; }
    inline bool playerInMeleeStance() { return _PlayerInCombatStance && _StanceType == StanceType::Melee; }
    inline bool playerInMagicStance() { return _PlayerInCombatStance && _StanceType == StanceType::Magic; }


    void validateCombatStance(Item& heldItem);
    inline bool playerInAttack() { return !_PlayerAttackTimer.isComplete(_PlayerAttackRecoveryTime); }
    void drawWeapon(Renderer& renderer, SDL_Rect pos, SDL_RendererFlip flip, Item& heldItem);

};


#endif //SDL_PLAYERCONTROLLER_HPP
