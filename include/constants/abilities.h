#ifndef GUARD_CONSTANTS_ABILITIES_H
#define GUARD_CONSTANTS_ABILITIES_H

// Unless otherwise specified, abilities are Gen 3 implementation.
#define ABILITY_NONE 0
#define ABILITY_STENCH 1
#define ABILITY_DRIZZLE 2 // Still Gen 3-5 implementation - lasts whole battle unless changed.
#define ABILITY_SPEED_BOOST 3
#define ABILITY_BATTLE_ARMOR 4
#define ABILITY_STURDY 5 // Gen 5+ implementation - cannot be KO'd by a single hit as long as full HP, immune to OHKO moves.
#define ABILITY_DAMP 6
#define ABILITY_LIMBER 7
#define ABILITY_SAND_VEIL 8
#define ABILITY_STATIC 9
#define ABILITY_VOLT_ABSORB 10 // Gen 4+ implementation - now activates if hit by electric status moves.
#define ABILITY_WATER_ABSORB 11 // Gen 4+ implementation - now activates if hit by water status moves.
#define ABILITY_OBLIVIOUS 12 // Gen 8+ implementation - also not afflicted by Taunt or affected by Intimidate.
#define ABILITY_CLOUD_NINE 13
#define ABILITY_COMPOUND_EYES 14
#define ABILITY_INSOMNIA 15
#define ABILITY_COLOR_CHANGE 16
#define ABILITY_IMMUNITY 17
#define ABILITY_FLASH_FIRE 18 // Works even if user is frozen - does not thaw user out. Otherwise Gen 3 implementation.
#define ABILITY_SHIELD_DUST 19
#define ABILITY_OWN_TEMPO 20 // Gen 8+ implementation - also not affected by Intimidate.
#define ABILITY_SUCTION_CUPS 21
#define ABILITY_INTIMIDATE 22
#define ABILITY_SHADOW_TAG 23
#define ABILITY_ROUGH_SKIN 24
#define ABILITY_WONDER_GUARD 25
#define ABILITY_LEVITATE 26
#define ABILITY_EFFECT_SPORE 27 // Gen 6+ implementation - Grass-types are immune, Punching Gloves don't make contact, 9% chance of poison, 10% chance of paralysis, 11% chance of sleep.
#define ABILITY_SYNCHRONIZE 28 // Gen 5+ implementation.
#define ABILITY_CLEAR_BODY 29
#define ABILITY_NATURAL_CURE 30
#define ABILITY_LIGHTNING_ROD 31 // Gen 5+ implementation.
#define ABILITY_SERENE_GRACE 32 // Gen 5+ implementation.
#define ABILITY_SWIFT_SWIM 33
#define ABILITY_CHLOROPHYLL 34
#define ABILITY_ILLUMINATE 35
#define ABILITY_TRACE 36
#define ABILITY_HUGE_POWER 37
#define ABILITY_POISON_POINT 38
#define ABILITY_INNER_FOCUS 39 // Gen 8+ implementation - also not affected by Intimidate.
#define ABILITY_MAGMA_ARMOR 40
#define ABILITY_WATER_VEIL 41
#define ABILITY_MAGNET_PULL 42
#define ABILITY_SOUNDPROOF 43 // Cannot fall asleep during its own Uproar now.
#define ABILITY_RAIN_DISH 44
#define ABILITY_SAND_STREAM 45 // Still Gen 3-5 implementation - lasts whole battle unless changed.
#define ABILITY_PRESSURE 46
#define ABILITY_THICK_FAT 47
#define ABILITY_EARLY_BIRD 48
#define ABILITY_FLAME_BODY 49
#define ABILITY_RUN_AWAY 50
#define ABILITY_KEEN_EYE 51 // Gen 6+ implementation - also ignores target's evasion stat increases.
#define ABILITY_HYPER_CUTTER 52
#define ABILITY_PICKUP 53
#define ABILITY_TRUANT 54
#define ABILITY_HUSTLE 55
#define ABILITY_CUTE_CHARM 56
#define ABILITY_PLUS 57 // Gen 5+ implementation - Plus or Minus triggers stat boost.
#define ABILITY_MINUS 58 // Gen 5+ implementation - Plus or Minus triggers stat boost.
#define ABILITY_FORECAST 59 // Now works with Sandstorm -> Rock type transformation.
#define ABILITY_STICKY_HOLD 60
#define ABILITY_SHED_SKIN 61
#define ABILITY_GUTS 62
#define ABILITY_MARVEL_SCALE 63
#define ABILITY_LIQUID_OOZE 64
#define ABILITY_OVERGROW 65
#define ABILITY_BLAZE 66
#define ABILITY_TORRENT 67
#define ABILITY_SWARM 68
#define ABILITY_ROCK_HEAD 69
#define ABILITY_DROUGHT 70 // Still Gen 3-5 implementation - lasts whole battle unless changed.
#define ABILITY_ARENA_TRAP 71
#define ABILITY_VITAL_SPIRIT 72
#define ABILITY_WHITE_SMOKE 73
#define ABILITY_PURE_POWER 74
#define ABILITY_SHELL_ARMOR 75
#define ABILITY_CACOPHONY 76
#define ABILITY_AIR_LOCK 77

#define ABILITY_TITANIC 78 // Custom ability - Reduces Speed stat by 20%, but cannot be forced out, intimidated, or flinched.
#define ABILITY_IRON_FIST 79 // Gen 4+ implementation.
#define ABILITY_TECHNICIAN 80 // Gen 5+ implementation with a slight change - now powers up moves with <= 65 base power.
#define ABILITY_SNOW_WARNING 81 // Gen 4-5 implementation - lasts whole battle unless changed.
#define ABILITY_SOLAR_POWER 82 // Gen 4+ implementation.

#define ABILITIES_COUNT 83

#endif  // GUARD_CONSTANTS_ABILITIES_H
