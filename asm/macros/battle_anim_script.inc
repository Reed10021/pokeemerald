@ commands

	.macro loadspritegfx tag:req
	.byte 0x0
	.2byte \tag
	.endm

	.macro unloadspritegfx tag:req
	.byte 0x1
	.2byte \tag
	.endm

.macro createsprite template:req, anim_battler:req, subpriority_offset:req, argv:vararg
	.byte 0x02
	.4byte \template
	.if \anim_battler == ANIM_TARGET
	.byte 0x80 | (\subpriority_offset & 0x7F)
	.else
	.byte (\subpriority_offset & 0x7F)
	.endif
	.byte (.Lsprite_\@_2 - .Lsprite_\@_1) / 2
.Lsprite_\@_1:
	.2byte \argv
.Lsprite_\@_2:
	.endm

	.macro createvisualtask addr:req, priority:req, argv:vararg
	.byte 0x03
	.4byte \addr
	.byte \priority
	.byte (.Lcreatetask_\@_2 - .Lcreatetask_\@_1) / 2
.Lcreatetask_\@_1:
	.2byte \argv
.Lcreatetask_\@_2:
	.endm

	.macro delay param0:req
	.byte 0x4
	.byte \param0
	.endm

	.macro waitforvisualfinish
	.byte 0x5
	.endm

	.macro hang1
	.byte 0x6
	.endm

	.macro hang2
	.byte 0x7
	.endm

	.macro end
	.byte 0x8
	.endm

	.macro playse se:req
	.byte 0x9
	.2byte \se
	.endm

	.macro monbg battler:req
	.byte 0xa
	.byte \battler
	.endm

	.macro clearmonbg battler:req
	.byte 0xb
	.byte \battler
	.endm

	.macro setalpha eva:req, evb:req
	.byte 0x0C
	.2byte ((\evb) << 8) | (\eva)
	.endm

	.macro blendoff
	.byte 0xd
	.endm

	.macro call param0:req
	.byte 0xe
	.4byte \param0
	.endm

	.macro return
	.byte 0xf
	.endm

	.macro setarg param0:req, param1:req
	.byte 0x10
	.byte \param0
	.2byte \param1
	.endm

	.macro choosetwoturnanim param0:req, param1:req
	.byte 0x11
	.4byte \param0
	.4byte \param1
	.endm

	.macro jumpifmoveturn param0:req, ptr:req
	.byte 0x12
	.byte \param0
	.4byte \ptr
	.endm

	.macro goto ptr:req
	.byte 0x13
	.4byte \ptr
	.endm

	.macro fadetobg bg:req
	.byte 0x14
	.byte \bg
	.endm

	.macro restorebg
	.byte 0x15
	.endm

	.macro waitbgfadeout
	.byte 0x16
	.endm

	.macro waitbgfadein
	.byte 0x17
	.endm

	.macro changebg bg:req
	.byte 0x18
	.byte \bg
	.endm

	.macro playsewithpan se:req, pan:req
	.byte 0x19
	.2byte \se
	.byte \pan
	.endm

	.macro setpan pan:req
	.byte 0x1a
	.byte \pan
	.endm

	.macro panse_1B se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x1b
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro loopsewithpan se:req, pan:req, wait:req, times:req
	.byte 0x1c
	.2byte \se
	.byte \pan
	.byte \wait
	.byte \times
	.endm

	.macro waitplaysewithpan se:req, pan:req, wait:req
	.byte 0x1d
	.2byte \se
	.byte \pan
	.byte \wait
	.endm

	.macro setbldcnt param0:req
	.byte 0x1e
	.2byte \param0
	.endm

	.macro createsoundtask addr:req, argv:vararg
	.byte 0x1F
	.4byte \addr
	.byte (.Lcreatetask_1F_\@_2 - .Lcreatetask_1F_\@_1) / 2
.Lcreatetask_1F_\@_1:
	.2byte \argv
.Lcreatetask_1F_\@_2:
	.endm

	.macro waitsound
	.byte 0x20
	.endm

	.macro jumpargeq param0:req, param1:req, ptr:req
	.byte 0x21
	.byte \param0
	.2byte \param1
	.4byte \ptr
	.endm

	.macro monbg_22 battler:req
	.byte 0x22
	.byte \battler
	.endm

	.macro clearmonbg_23 battler:req
	.byte 0x23
	.byte \battler
	.endm

	.macro jumpifcontest ptr:req
	.byte 0x24
	.4byte \ptr
	.endm

	.macro fadetobgfromset param0:req, param1:req, param2:req
	.byte 0x25
	.byte \param0
	.byte \param1
	.byte \param2
	.endm

	.macro panse_26 se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x26
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro panse_27 se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x27
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro monbgprio_28 battler:req
	.byte 0x28
	.byte \battler
	.endm

	.macro monbgprio_29
	.byte 0x29
	.endm

	.macro monbgprio_2A battler:req
	.byte 0x2a
	.byte \battler
	.endm

	.macro invisible battler:req
	.byte 0x2b
	.byte \battler
	.endm

	.macro visible battler:req
	.byte 0x2c
	.byte \battler
	.endm

	.macro doublebattle_2D battler:req
	.byte 0x2d
	.byte \battler
	.endm

	.macro doublebattle_2E battler:req
	.byte 0x2e
	.byte \battler
	.endm

	.macro stopsound
	.byte 0x2f
	.endm
	
@ useful macros
	.macro jumpreteq value:req, ptr:req
	jumpargeq ARG_RET_ID, \value, \ptr
	.endm
	
	.macro jumprettrue ptr:req
	jumpreteq TRUE, \ptr
	.endm
	
	.macro jumpretfalse ptr:req
	jumpreteq FALSE, \ptr
	.endm

	.macro simple_palette_blend unused_anim_battler=ANIM_ATTACKER, unused_subpriority_offset=2, selector:req, delay:req, initial_blend_y:req, target_blend_y:req, color:req
	createsprite gSimplePaletteBlendSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \selector, \delay, \initial_blend_y, \target_blend_y, \color
	.endm

	.macro complex_palette_blend unused_anim_battler=ANIM_ATTACKER, unused_subpriority_offset=2, selector:req, delay:req, num_blends:req, color1:req, blend_y1:req, color2:req, blend_y2:req
	createsprite gComplexPaletteBlendSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \selector, \delay, \num_blends, \color1, \blend_y1, \color2, \blend_y2
	.endm

	.macro shake_mon_or_platform unused_anim_battler=ANIM_ATTACKER, unused_subpriority_offset=2, velocity:req, shake_timer:req, shake_duration:req, type:req, battler_selector
	.if \type == SHAKE_MON_X || \type == SHAKE_MON_Y
	.ifb \battler_selector
	.error "battler_selector required for SHAKE_MON_X or SHAKE_MON_Y"
	.endif
	.else
	.ifnb \battler_selector
	.warning "unused battler_selector in shake_mon_or_platform"
	.endif
	.endif
	.ifb \battler_selector
	createsprite gShakeMonOrTerrainSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \velocity, \shake_timer, \shake_duration, \type
	.else
	createsprite gShakeMonOrTerrainSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \velocity, \shake_timer, \shake_duration, \type, \battler_selector
	.endif
	.endm

	.macro create_claw_slash_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, animation:req
	createsprite gClawSlashSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \animation
	.endm

	.macro create_basic_hitsplat_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, relative_to:req, animation:req
	createsprite gBasicHitSplatSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \relative_to, \animation
	.endm

	.macro create_cross_impact_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, relative_to:req, duration:req
	createsprite gCrossImpactSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \relative_to, \duration
	.endm

	.macro attacker_fade_to_invisible priority=2, step_delay:req
	createvisualtask AnimTask_AttackerFadeToInvisible, \priority, \step_delay
	.endm

	.macro attacker_fade_from_invisible priority=2, step_delay:req
	createvisualtask AnimTask_AttackerFadeFromInvisible, \priority, \step_delay
	.endm

	.macro shrink_target_copy priority=5, unk0:req, unk1:req
	createvisualtask AnimTask_ShrinkTargetCopy, \priority, \unk0, \unk1
	.endm
