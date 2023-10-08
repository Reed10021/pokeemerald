#ifndef GUARD_POKEDEX_PLUS_HGSS_H
#define GUARD_POKEDEX_PLUS_HGSS_H

extern u8 gUnusedPokedexU8;
extern void (*gPokedexVBlankCB)(void);

void CB2_OpenPokedexPlusHGSS(void);
u16 NationalPokedexNumToSpeciesHGSS(u16 nationalNum);
u8 DisplayCaughtMonDexPage(u16 species, u32 otId, u32 personality);

#endif // GUARD_POKEDEX_PLUS_HGSS_H
