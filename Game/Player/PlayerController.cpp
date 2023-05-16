//
// Created by Owner on 4/28/2023.
//

#include "PlayerController.hpp"


void PlayerController::update(Event& event, Item& heldItem, SDL_RendererFlip flip, v2<int,int>& playerPos) {

    updateAnimator();
    _TempItemOffset = {0,0,0,0};
    _TempItemRotation = 0.0f;

    bool canEnterCombatStance = (heldItem.type() == ItemSpawnList::ItemType::Tool || heldItem.type() == ItemSpawnList::ItemType::Weapon);
    bool canEnterMagicStance = (heldItem.type() == ItemSpawnList::ItemType::MagicalItem);
    bool xKeyDown = event.keyState()[SDL_SCANCODE_X];
    bool yKeyDown = event.keyState()[SDL_SCANCODE_Y];

    // TODO: Seperate Magic And Melee Stance So They Dont Intervein With Eachother
    if (xKeyDown || yKeyDown) {
        if (!canEnterCombatStance)
            printf("Cant Enter %s Stance With A %s\n", (xKeyDown) ? "Combat" : "Magic", (xKeyDown) ? "Non Weapon Or Tool" : "Non Magical Item");
        else {
            if (_PlayerEnterCombatStance.isCompleteReset(300)) {
                if (_PlayerInCombatStance && (_StanceType == StanceType::Melee && xKeyDown || _StanceType == StanceType::Magic && yKeyDown)) {
                    _PlayerInCombatStance = false;
                    printf("Exiting Stance\n");
                    _StanceType = StanceType::None;
                } else {
                    printf("Entering %s Stance\n", (xKeyDown) ? "Combat" : "Magic");
                    if (canEnterCombatStance && xKeyDown) _StanceType = StanceType::Melee;
                    else if (canEnterMagicStance && yKeyDown) _StanceType = StanceType::Magic;
                    _PlayerInCombatStance = true;
                }
            }
        }
    }

    if (canEnterCombatStance) {
        bool shiftHeldDown = event.keyState()[SDL_SCANCODE_LSHIFT];
        bool controlHeldDown = event.keyState()[SDL_SCANCODE_LCTRL];
        if (_PlayerInCombatStance) {
            if (_PlayerAttackTimer.isCompleteReset(_PlayerAttackRecoveryTime)) {
                if (event.mouseDownL()) {
                    if (event.keyState()[SDL_SCANCODE_S]) launch(heldItem, flip);
                    else if (controlHeldDown) lunge(heldItem, flip, playerPos.x);
                    else if (shiftHeldDown) stab(heldItem, flip);
                    else swing(heldItem, flip);
                }
            }
            if (event.mouseDownR()) {
                // TODO: Reimplement Parrying, Make It So That You Cannot Block After A Parry For A Short Time
                /*if (controlHeldDown && _PlayerDefenseTimer.isCompleteReset(_PlayerDefenceRecoveryTime))
                    parry(heldItem, flip);
                else*/
                block(heldItem, flip);
            }
        }
    }
}

void PlayerController::validateCombatStance(Item &heldItem) {
    if ((heldItem.type() != ItemSpawnList::ItemType::Tool || heldItem.type() != ItemSpawnList::ItemType::Weapon) && _PlayerInCombatStance)
        _PlayerInCombatStance = false;
}

void PlayerController::swing(Item &heldItem, SDL_RendererFlip flip) {
    if (_AnimationWaitCount > 0) return;

    _PlayerAttackRecoveryTime = 100;

    AnimationComponent<float> anim{};
    anim.value = &heldItem._rect._rot;
    anim.animationIsCompleteCounter = &_AnimationWaitCount;
    anim.keyframes = {
        AnimationComponent<float>::Keyframe(0, 0, (-45.0f)),
        AnimationComponent<float>::Keyframe(7, (120.0f/4)),
        AnimationComponent<float>::Keyframe(1, (120.0f/4)),
        AnimationComponent<float>::Keyframe(1, (120.0f/4)),
        AnimationComponent<float>::Keyframe(1, (120.0f/4)),
        AnimationComponent<float>::Keyframe(10, 0, 0.0f),
    };

    uploadAnimation(anim);
}

void PlayerController::stab(Item &heldItem, SDL_RendererFlip flip) {
    if (_AnimationWaitCount > 0) return;

    _PlayerAttackRecoveryTime = 150;

    float dirMultiplier = (flip == SDL_FLIP_NONE) ? 1.0f : -1.0f;

    AnimationComponent<float> anim1{};
    anim1.value = &heldItem._rect._rot;
    anim1.animationIsCompleteCounter = &_AnimationWaitCount;
    anim1.keyframes = {
            AnimationComponent<float>::Keyframe(0, 0, 45.0f),
            AnimationComponent<float>::Keyframe(10 + (3 * 4), 0, 0.0f),
    };

    AnimationComponent<int32_t> anim2{};
    anim2.value = &heldItem._rect.pos.x;
    anim2.animationIsCompleteCounter = &_AnimationWaitCount;
    anim2.keyframes = {
            AnimationComponent<int32_t>::Keyframe(10, -10 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 5 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 10 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 20 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 0, 0 * dirMultiplier),
    };

    uploadAnimation(anim1);
    uploadAnimation(anim2);
}

// TODO: Make This Less Like The Stab Attack
void PlayerController::lunge(Item &heldItem, SDL_RendererFlip flip, int32_t& offset) {
    if (_AnimationWaitCount > 0) return;

    _PlayerAttackRecoveryTime = 200;

    float dirMultiplier = (flip == SDL_FLIP_NONE) ? 1.0f : -1.0f;

    AnimationComponent<float> anim1{};
    anim1.value = &heldItem._rect._rot;
    anim1.animationIsCompleteCounter = &_AnimationWaitCount;
    anim1.keyframes = {
            AnimationComponent<float>::Keyframe(0, 0, 45.0f),
            AnimationComponent<float>::Keyframe(10 + (3*4), 0, 0.0f),
    };

    AnimationComponent<int32_t> anim2{};
    anim2.value = &heldItem._rect.pos.x;
    anim2.animationIsCompleteCounter = &_AnimationWaitCount;
    anim2.keyframes = {
            AnimationComponent<int32_t>::Keyframe(10, -10 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 5 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 10 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 20 * dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 0, 0 * dirMultiplier),
    };
    AnimationComponent<int32_t> anim3{};
    anim3.value = &offset;
    anim3.animationIsCompleteCounter = &_AnimationWaitCount;
    anim3.keyframes = {
            AnimationComponent<int32_t>::Keyframe(10, 10 * -(int)dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 20 * -(int)dirMultiplier),
            AnimationComponent<int32_t>::Keyframe(3, 30 * -(int)dirMultiplier),
    };

    uploadAnimation(anim1);
    uploadAnimation(anim2);
    uploadAnimation(anim3);
}

void PlayerController::launch(Item &heldItem, SDL_RendererFlip flip) {
    if (_AnimationWaitCount > 0) return;

    _PlayerAttackRecoveryTime = 100;


    AnimationComponent<float> anim;
    anim.value = &heldItem._rect._rot;
    anim.animationIsCompleteCounter = &_AnimationWaitCount;
    anim.keyframes = {
            AnimationComponent<float>::Keyframe(2, 0, 120.0),
            AnimationComponent<float>::Keyframe(7, -20),
            AnimationComponent<float>::Keyframe(2, -40),
            AnimationComponent<float>::Keyframe(2, -60),
    };

    uploadAnimation(anim);
}

void PlayerController::block(Item &heldItem, SDL_RendererFlip flip) {
    if (_AnimationWaitCount > 0) return;

    _TempItemRotation = 120;
    _TempItemOffset.y -= 64;
}

void PlayerController::drawWeapon(Renderer& renderer, SDL_Rect pos, SDL_RendererFlip flip, Item &heldItem) {

    int32_t weaponSize = 32;

    Renderer::TextureDrawProperties props;
    props.rotationOrigin = {(flip == SDL_FLIP_NONE) ? 0 : 32 * 2, 32 * 2};
    props.flip = flip;

    bool validStance = (_StanceType == StanceType::Melee &&
                            (heldItem.type() == ItemSpawnList::ItemType::Tool || heldItem.type() == ItemSpawnList::ItemType::Weapon))
                        || (_StanceType == StanceType::Magic && heldItem.type() == ItemSpawnList::ItemType::MagicalItem);

    if (_PlayerInCombatStance && validStance) {
        props.rotation = (heldItem.getRotation() * ((flip == SDL_FLIP_NONE) ? 1.0f : -1.0f)) + _TempItemRotation;

        SDL_Rect weaponPos {
                (((flip == SDL_FLIP_NONE) ? pos.x + pos.w : pos.x - (weaponSize * 2)) + heldItem._rect.pos.x) + _TempItemOffset.x,
                (pos.y - ((weaponSize * 2)/4)) + _TempItemOffset.y,
                weaponSize * 2,
                weaponSize * 2
        };

        heldItem.draw(renderer, weaponPos, props);
    }
    else {
        // Tool/Weapon Needs To Be Sheathed
        props.rotation = (flip == SDL_FLIP_NONE) ? 150.0f : -150.0f;

        SDL_Rect playerBack {
                (flip == SDL_FLIP_NONE) ? pos.x : (pos.x + pos.w) - (weaponSize * 2),
                pos.y - (weaponSize * 2),
                weaponSize * 2,
                weaponSize * 2
        };

        heldItem.draw(renderer, playerBack, props);

    }
}