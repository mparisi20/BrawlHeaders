#pragma once

#include <StaticAssert.h>
#include <em/em_create.h>
#include <em/em_extend_param_accesser.h>
#include <em/cymal/em_cymal.h>
#include <em/resource/em_resource_module_impl.h>
#include <types.h>

struct emCymalParam {

};

class emCymalParamAccesser : public emExtendParamAccesser {
public:
    emCymalParamAccesser();
    virtual ~emCymalParamAccesser();
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 p2);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 p2);
    virtual void* getParamIndefinite(soModuleAccesser* moduleAccesser, s32 p2);
};
static_assert(sizeof(emCymalParamAccesser) == 0xC, "Class is the wrong size!");

extern emCymalParamAccesser g_emCymulParamAccesser;
