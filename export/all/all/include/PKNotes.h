//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_NOTES_H
#define PK_NOTES_H

//
// Notes
//

typedef enum
{
    PKNOTATION_LATIN = 0,
    PKNOTATION_US_UK = 1,

    PKNOTATION_NAMES   = 0,
    PKNOTATION_LETTERS = 1,

} PKNotationName;

//
// Notation Latine
//

#define PKNOTE_DO         0
#define PKNOTE_DO_DIESE   1
#define PKNOTE_RE         2
#define PKNOTE_RE_DIESE   3
#define PKNOTE_MI         4
#define PKNOTE_FA         5
#define PKNOTE_FA_DIESE   6
#define PKNOTE_SOL        7
#define PKNOTE_SOL_DIESE  8
#define PKNOTE_LA         9
#define PKNOTE_LA_DIESE  10
#define PKNOTE_SI        11

#define PKNOTE_NUMBER_OF_NOTES 12

// Doublons

#define PKNOTE_RE_BEMOL   1
#define PKNOTE_MI_BEMOL   3
#define PKNOTE_SOL_BEMOL  6
#define PKNOTE_LA_BEMOL   8
#define PKNOTE_SI_BEMOL  10

// Notation Américaine

#define PKNOTE_C         0
#define PKNOTE_C_SHARP   1
#define PKNOTE_D         2
#define PKNOTE_D_SHARP   3
#define PKNOTE_E         4
#define PKNOTE_F         5
#define PKNOTE_F_SHARP   6
#define PKNOTE_G         7
#define PKNOTE_G_SHARP   8
#define PKNOTE_A         9
#define PKNOTE_A_SHARP  10
#define PKNOTE_B        11

// Doublons

#define PKNOTE_D_FLAT   1
#define PKNOTE_E_FLAT   3
#define PKNOTE_G_FLAT   6
#define PKNOTE_A_FLAT   8
#define PKNOTE_B_FLAT  10

//
// Intervalles
//

#define PK_INTERVAL_BASE      0
#define PK_INTERVAL_2ND_MIN   1
#define PK_INTERVAL_2ND_MAJ   2
#define PK_INTERVAL_3RD_MIN   3
#define PK_INTERVAL_3RD_MAJ   4 
#define PK_INTERVAL_4TH       5
#define PK_INTERVAL_5TH_DIM   6
#define PK_INTERVAL_5TH       7
#define PK_INTERVAL_5TH_AUG   8
#define PK_INTERVAL_6TH       9
#define PK_INTERVAL_7TH_MIN  10
#define PK_INTERVAL_7TH_MAJ  11
#define PK_INTERVAL_OCTAVE   12

#define PK_INTERVAL_STOP     255

#define PK_INTERVAL_9TH_DIM  13 // 2ND_MIN
#define PK_INTERVAL_9TH      14 // 2ND_MAJ
#define PK_INTERVAL_9TH_AUG  15 // 3RD_MIN
#define PK_INTERVAL_10TH     16 // 3RD_MAJ
#define PK_INTERVAL_11TH     17 // 4TH
#define PK_INTERVAL_12TH_DIM 18 // 5TH_DIM
#define PK_INTERVAL_12TH     19 // 5TH
#define PK_INTERVAL_12TH_AUG 20 // 5TH_AUG
#define PK_INTERVAL_13TH_DIM 20 // 6TH_DIM
#define PK_INTERVAL_13TH     21 // 6TH
#define PK_INTERVAL_13TH_AUG 22 // 6TH_AUG

// 
// Octaves
//
// La = 440 Hz = La3
//

#define PK_OCTAVE_0  0
#define PK_OCTAVE_1  1
#define PK_OCTAVE_2  2
#define PK_OCTAVE_3  3
#define PK_OCTAVE_4  4
#define PK_OCTAVE_5  5
#define PK_OCTAVE_6  6
#define PK_OCTAVE_7  7
#define PK_OCTAVE_8  8
#define PK_OCTAVE_9  9

#endif // PK_NOTES_H
