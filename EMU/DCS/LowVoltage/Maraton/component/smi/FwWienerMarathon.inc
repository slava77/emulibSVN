class: ASS_FwWienerMarathon_CLASS/associated
!panel: FwWienerMarathon.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
        action: OFF	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1
        action: ON	!visible: 1