#pragma once

#include <StaticAssert.h>
#include <it/it_archive.h>
#include <it/it_customizer.h>
#include <it/it_gen_archive.h>
#include <it/item.h>
#include <so/so_dispose_instance_manager.h>
#include <types.h>
#include <bitset>

class itManager : public gfTask, public soDisposeInstanceEventObserver {
public:
    struct ItemSwitch {
        bitset<NUM_IT_KINDS> m_itemSwitch;

        ItemSwitch() : m_itemSwitch(0) {

        }
        ItemSwitch(bool val) : m_itemSwitch(val) {

        }
    };

    char _76[116];
    u32 m_currentGameFrameCount;
    int m_numItems;
    soArrayList<itArchive*, 128> m_itArchiveArrayList;
    bool m_drawDone;
    char _1749[3];
    soArrayList<BaseItem*, 64> m_itemArrayList;
    soArrayList<BaseItem*, 64> m_itemArrayList2;
    soArrayList<BaseItem*, 64> m_itemArrayList3;
    char _4080[25];
    u8 m_itemNums[NUM_IT_KINDS];
    char _4283[1];
    itKind m_nextAssistKind;
    int m_numAssists;
    int m_preloadAssistTimer;
    soArrayList<itKind, 12> m_pokemonKindArrayList;
    u32 m_nextPokemonIndex;
    int m_numPokemon;
    int m_numPokeballs;
    int m_preloadPokemonTimer;
    soArrayList<itGenArchive*, 3> m_itGenArchiveArrayList;
    ItemSwitch m_itemSwitch;
    float m_itemLotRates[NUM_IT_KINDS];
    int m_crateVariation;
    int m_smashBallDropTimer;
    char _5252[8];
    int m_smashBallDropFrame1;
    int m_smashBallDropFrame2;
    char _5268[28];
    int m_itemGenLevel;
    bool m_needsToProcess;
    char _5301[3];
    void* m_figureManager;
    char _5308[4];

    virtual void processDefault();
    virtual void processBegin();
    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void processDebug();
    virtual void renderDebug();
    virtual ~itManager();

    virtual void notifyDisposeInstance(bool, int, int taskId);
    virtual void notifyDrawDone();

    bool isCompItemKindArchive(itKind kind, u32 variation, bool);
    itArchive* preloadItemKindArchive(itKind kind, u32 variation, itArchive::Type archiveType, bool);
    ItemKind getLotOneItemKind(itGenSheetKind* sheetKind, itGenKind genKind, ItemSwitch*, bool);
    BaseItem* createItem(itKind kind, u32 variation, int creatorTaskId, int, u8, int, int, int);
    BaseItem* createItemInstance(itCreate* create);
    u32 getItemNum(itKind kind);
    u32 getItemNum(itKind kind, int variation, int taskId, int);
    void removeItem(BaseItem*);
    static itManager* getInstance();
};
static_assert(sizeof(itManager) == 0x14c0, "Class is wrong size!");
