# Copyright 2023-2024 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

if(SB_CONFIG_NET_CORE_IMAGE_HCI_IPC)
  set(NET_APP hci_ipc)
  set(NET_APP_SRC_DIR ${ZEPHYR_BASE}/samples/bluetooth/${NET_APP})

  ExternalZephyrProject_Add(
    APPLICATION ${NET_APP}
    SOURCE_DIR  ${NET_APP_SRC_DIR}
    BOARD       ${SB_CONFIG_NET_CORE_BOARD}
  )
endif()
