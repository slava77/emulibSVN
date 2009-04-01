class: ASS_CSC_LV_NODES_CLASS/associated
!panel: CSC_LV_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
        action: OFF	!visible: 1
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
    state: ON	!color: FwStateOKPhysics
        action: STANDBY	!visible: 1
        action: OFF	!visible: 1
    state: STANDBY	!color: FwStateAttention1
        action: ON	!visible: 1
        action: OFF	!visible: 1
