#ifndef __DM9000A_INICHE_H__
#define __DM9000A_INICHE_H__

#include "alt_iniche_dev.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct dm9000a_if_struct
{
  alt_iniche_dev              dev;
} dm9000a_if;

#define ALTERA_AVALON_DM9000A_INSTANCE(name, dev)                         \
    dm9000a_if dev =                                                      \
    {                                                                     \
      {                                                                   \
        ALT_LLIST_ENTRY,                                                  \
        name##_NAME,                                                      \
        dm9000a_init                                                      \
      }                                                                   \
    }

#define ALTERA_AVALON_LANDM9000A_INIT(name, dev_inst)                     \
    alt_iniche_dev_reg(&(dev_inst.dev))

error_t dm9000a_init(alt_iniche_dev *p_dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*  __ALTERA_AVALON_LANDM9000A_INICHE_H__ */
