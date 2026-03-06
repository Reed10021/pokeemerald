#ifndef GUARD_BRAILLE_PUZZLES_H
#define GUARD_BRAILLE_PUZZLES_H

bool8 ShouldDoBrailleRegigigasEffect(void);
bool8 ShouldDoBrailleRegisteelEffect(void);
bool8 ShouldDoBrailleRegirockEffect(void);
bool8 ShouldDoBrailleDigEffect(void);
void DoBrailleDigEffect(void);
void SetUpPuzzleEffectRegisteel(void);
void SetUpPuzzleEffectRegirock(void);
void SetUpPuzzleEffectRegigigas(void);

void UseRegirockHm_Callback(void);
void UseRegisteelHm_Callback(void);
void UseRegigigasHm_Callback(void);

void SealedChamberShakingEffect(u8);
void DoBrailleRegirockEffect(void);
void DoBrailleRegisteelEffect(void);
void DoBrailleRegigigasEffect(void);


#endif // GUARD_BRAILLE_PUZZLES_H
