class: ASS_FwElmbPSUBranch_CLASS/associated
!panel: FwElmbPSUBranch.pnl
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
        action: STANDBY	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
        action: OFF	!visible: 1
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
