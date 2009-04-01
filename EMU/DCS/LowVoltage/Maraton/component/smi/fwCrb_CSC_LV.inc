class: ASS_fwCrb_CSC_LV_CLASS/associated
!panel: CMS_CSCfw_LV_CRB/emuDevfwCrb_CSC_LVOperation.pnl
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
        action: STANDBY	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
        action: OFF	!visible: 1
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1
        action: STANDBY	!visible: 1
