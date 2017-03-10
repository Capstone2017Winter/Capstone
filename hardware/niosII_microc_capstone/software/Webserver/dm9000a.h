#ifndef __DM9000A_INICHE_H__
#define __DM9000A_INICHE_H__

#include "alt_iniche_dev.h"

#define DM9000A_INST_BASE DM9000A_IF_0_BASE
#define DM9000A_INST_IRQ DM9000A_IF_0_IRQ


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct dm9000a_if_struct
{
  alt_iniche_dev              dev;
} dm9000a_if;

#define DM9000A_INSTANCE(name, dev)                                       \
    dm9000a_if dev =                                                      \
    {                                                                     \
      {                                                                   \
        ALT_LLIST_ENTRY,                                                  \
        name##_NAME,                                                      \
        dm9000a_init                                                      \
      }                                                                   \
    }

#define DM9000A_INIT(name, dev_inst)                                      \
    alt_iniche_dev_reg(&(dev_inst.dev))

error_t dm9000a_init(alt_iniche_dev *p_dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*  __DM9000A_INICHE_H__ */
