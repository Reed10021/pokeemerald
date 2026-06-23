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
#define ABILITY_SLUSH_RUSH 83 // Gen 7+ implementation
#define ABILITY_SNOW_CLOAK 84 // Gen 4+ implementation
#define ABILITY_DRY_SKIN 85 // Gen 4+ implementation.
#define ABILITY_MOTOR_DRIVE 86 // Gen 4+ implementation.
#define ABILITY_PYRO_REACTOR 87 // Custom ability - Fire-type Motor Drive.
#define ABILITY_SAND_RUSH 88 // Gen 5+ implementation.
#define ABILITY_SAND_FORCE 89 // Custom implementation - Attack rises in sandstorm.
#define ABILITY_ANALYTIC 90 // Gen 5+ implementation.
#define ABILITY_ADAPTABILITY 91 // Gen 4+ implementation.
#define ABILITY_HYDRATION 92 // Gen 4+ implementation.
#define ABILITY_QUICK_FEET 93 // Gen 4+ implementation.
#define ABILITY_LEAF_GUARD 94 // Gen 4+ implementation.
#define ABILITY_MAGIC_BOUNCE 95 // Gen 5+ implementation.
#define ABILITY_ARTIC_FORCE 96 // Custom implementation - Powers up moves, boosts Defense, and blocks hail damage during hail.
#define ABILITY_TRANSISTOR 97 // Gen 8 implementation.
#define ABILITY_DRAGONS_MAW 98 // Gen 8+ implementation.
#define ABILITY_MEGA_SOL 99 // Gen 9+ implementation - User's moves behave as if under harsh sunlight.
#define ABILITY_DRAGONIZE 100 // Gen 9+ implementation - Normal-type moves become Dragon-type and gain 20% power.
#define ABILITY_SHARPNESS 101 // Gen 9+ implementation.
#define ABILITY_SOLID_ROCK 102 // Gen 4+ implementation.
#define ABILITY_FILTER 103 // Gen 4+ implementation.
#define ABILITY_SUPER_LUCK 104 // Gen 4+ implementation.
#define ABILITY_TINTED_LENS 105 // Gen 4+ implementation.
#define ABILITY_OVERCAST 106 // Custom implementation: Castform's weather form and move weather are inverted.
#define ABILITY_STEADFAST 107 // Gen 4+ implementation.
#define ABILITY_JUSTIFIED 108 // Gen 5+ implementation.
#define ABILITY_CURSED_BODY 109 // Gen 5+ implementation.
#define ABILITY_PRANKSTER 110 // Gen 5+ implementation.
#define ABILITY_INFILTRATOR 111 // Gen 5+ implementation.
#define ABILITY_MULTISCALE 112 // Gen 5+ implementation.
#define ABILITY_ICE_SCALES 113 // Gen 8+ implementation.
#define ABILITY_SAP_SIPPER 114 // Gen 5+ implementation.
#define ABILITY_GOOEY 115 // Gen 6+ implementation.
#define ABILITY_SNIPER 116 // Gen 4+ implementation.
#define ABILITY_TANGLED_FEET 117 // Gen 4+ implementation.
#define ABILITY_BIG_PECKS 118 // Gen 5+ implementation.
#define ABILITY_UNNERVE 119 // Gen 5+ implementation.
#define ABILITY_NO_GUARD 120 // Gen 4+ implementation.
#define ABILITY_SHEER_FORCE 121 // Gen 5+ implementation.
#define ABILITY_UNAWARE 122 // Gen 4+ implementation.
#define ABILITY_MAGIC_GUARD 123 // Gen 4+ implementation.
#define ABILITY_ANGER_POINT 124 // Gen 4+ implementation.
#define ABILITY_DEFIANT 125 // Gen 5+ implementation.
#define ABILITY_OVERCOAT 126 // Gen 5+ implementation.
#define ABILITY_COMPETITIVE 127 // Gen 6+ implementation.
#define ABILITY_FRIEND_GUARD 128 // Gen 5+ implementation.
#define ABILITY_FRISK 129 // Gen 4+ implementation.
#define ABILITY_REGENERATOR 130 // Gen 5+ implementation.
#define ABILITY_HEALER 131 // Gen 5+ implementation.
#define ABILITY_WONDER_SKIN 132 // Gen 5+ implementation.
#define ABILITY_NORMALIZE 133 // Gen 4+ implementation.
#define ABILITY_GLUTTONY 134 // Gen 4+ implementation.
#define ABILITY_RATTLED 135 // Gen 5+ implementation.
#define ABILITY_MOXIE 136 // Gen 5+ implementation.
#define ABILITY_SKILL_LINK 137 // Gen 4+ implementation.
#define ABILITY_POISON_TOUCH 138 // Gen 5+ implementation.
#define ABILITY_RECKLESS 139 // Gen 4+ implementation.
#define ABILITY_SCRAPPY 140 // Gen 4+ implementation.
#define ABILITY_DOWNLOAD 141 // Gen 4+ implementation.
#define ABILITY_WEAK_ARMOR 142 // Gen 5+ implementation.
#define ABILITY_BERSERK 143 // Gen 7+ implementation.
#define ABILITY_HARVEST 144 // Gen 5+ implementation.
#define ABILITY_LIGHT_METAL 145 // Gen 5+ implementation.
#define ABILITY_HEAVY_METAL 146 // Gen 5+ implementation.
#define ABILITY_TELEPATHY 147 // Gen 5+ implementation.
#define ABILITY_ICE_BODY 148 // Gen 4+ implementation.
#define ABILITY_STORM_DRAIN 149 // Gen 4+ implementation.
#define ABILITY_MOLD_BREAKER 150 // Gen 4+ implementation.
#define ABILITY_AFTERMATH 151 // Gen 4+ implementation.
#define ABILITY_UNBURDEN 152 // Gen 4+ implementation.
#define ABILITY_TOUGH_CLAWS 153 // Gen 6+ implementation.
#define ABILITY_MOODY 154 // Gen 5+ implementation.
#define ABILITY_CONTRARY 155 // Gen 5+ implementation.
#define ABILITY_POISON_HEAL 156 // Gen 4+ implementation.
#define ABILITY_CUD_CHEW 157 // Gen 9+ implementation.
#define ABILITY_ARMOR_TAIL 158 // Gen 9+ implementation.
#define ABILITY_BULLETPROOF 159 // Gen 6+ implementation.

#define ABILITIES_COUNT 160

#endif  // GUARD_CONSTANTS_ABILITIES_H
