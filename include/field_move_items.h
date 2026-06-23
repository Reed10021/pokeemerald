u32 CanUseCut(s16, s16);
u32 UseCut(u32);

void ReturnToFieldFromFlyToolMapSelect();
bool32 IsFlyToolUsed();
void ReturnToFieldOrBagFromFlyTool();
void ResetFlyTool();

u32 CanUseSurf(s16, s16);
u32 CanUseSurfFromInteractedWater();
u32 UseSurf(u32);
void RemoveRelevantSurfFieldEffect();
void Task_SurfToolFieldEffect(u8 taskId);

u32 CanUseStrength();
u32 UseStrength(u32, u8, u8, u8);

void FldEff_UseFlashTool();
u32 CanUseFlash();
void InitAutoFlash();
void QueueAutoFlash();
bool32 TryStartQueuedAutoFlash();
void TryUseFlash();

u32 CanUseRockSmash(s16 x, s16 y);
u32 UseRockSmash(u32 fieldMoveStatus);

u32 CanUseWaterfall();
bool32 CanUseWaterfallTool();
u32 UseWaterfall(struct PlayerAvatar, u32);
void CreateUseWaterfallTask();
u32 CanUseWaterfallFromInteractedWater();
void RemoveRelevantWaterfallFieldEffect();

bool8 FldEff_UseDiveTool();
void RemoveRelevantDiveFieldEffect();
u32 CanUseDiveDown();
u32 CanUseDiveEmerge();

bool8 FldEff_UseTeleportTool();

bool8 FldEff_SweetScentTool();

void ClearFieldMoveFlags();

enum FieldMoveActionSource
{
    FIELD_MOVE_FAIL,
    FIELD_MOVE_POKEMON,
    FIELD_MOVE_TOOL
};

enum FlyToolSource
{
    FLY_SOURCE_MOVE,
    FLY_SOURCE_FIELD,
    FLY_SOURCE_BAG
};
