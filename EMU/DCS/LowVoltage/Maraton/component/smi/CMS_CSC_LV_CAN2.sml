class: CSC_ME_M4_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M4_CR1_LV_MRTN is_of_class CSC_ME_M4_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate37 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate37 }
objectset: CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate37 }

class: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate37:Channel0 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate37:Channel1 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate37:Channel3 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate37:Channel4 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate37:Channel5 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate37:Channel7 is_of_class CSC_ME_M4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate37:Channel0,
	Wiener:CAN7:Crate37:Channel1,
	Wiener:CAN7:Crate37:Channel3,
	Wiener:CAN7:Crate37:Channel4,
	Wiener:CAN7:Crate37:Channel5,
	Wiener:CAN7:Crate37:Channel7 }
objectset: CSC_ME_M4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate37:Channel0,
	Wiener:CAN7:Crate37:Channel1,
	Wiener:CAN7:Crate37:Channel3,
	Wiener:CAN7:Crate37:Channel4,
	Wiener:CAN7:Crate37:Channel5,
	Wiener:CAN7:Crate37:Channel7 }

class: CSC_ME_M4_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M4_CR2_LV_MRTN is_of_class CSC_ME_M4_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate38 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate38 }
objectset: CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate38 }

class: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate38:Channel0 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate38:Channel1 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate38:Channel3 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate38:Channel4 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate38:Channel5 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate38:Channel7 is_of_class CSC_ME_M4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate38:Channel0,
	Wiener:CAN6:Crate38:Channel1,
	Wiener:CAN6:Crate38:Channel3,
	Wiener:CAN6:Crate38:Channel4,
	Wiener:CAN6:Crate38:Channel5,
	Wiener:CAN6:Crate38:Channel7 }
objectset: CSC_ME_M4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate38:Channel0,
	Wiener:CAN6:Crate38:Channel1,
	Wiener:CAN6:Crate38:Channel3,
	Wiener:CAN6:Crate38:Channel4,
	Wiener:CAN6:Crate38:Channel5,
	Wiener:CAN6:Crate38:Channel7 }

class: CSC_ME_M4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_M4_LV_MRTN is_of_class CSC_ME_M4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_M4_CR1_LV_MRTN,
	CSC_ME_M4_CR2_LV_MRTN }
objectset: CSC_ME_M4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_M4_CR1_LV_MRTN,
	CSC_ME_M4_CR2_LV_MRTN }

class: CSC_ME_M2_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR1_LV_MRTN is_of_class CSC_ME_M2_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate29 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate29 }
objectset: CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate29 }

class: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate29:Channel0 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate29:Channel1 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate29:Channel3 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate29:Channel4 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate29:Channel5 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate29:Channel7 is_of_class CSC_ME_M2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate29:Channel0,
	Wiener:CAN6:Crate29:Channel1,
	Wiener:CAN6:Crate29:Channel3,
	Wiener:CAN6:Crate29:Channel4,
	Wiener:CAN6:Crate29:Channel5,
	Wiener:CAN6:Crate29:Channel7 }
objectset: CSC_ME_M2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate29:Channel0,
	Wiener:CAN6:Crate29:Channel1,
	Wiener:CAN6:Crate29:Channel3,
	Wiener:CAN6:Crate29:Channel4,
	Wiener:CAN6:Crate29:Channel5,
	Wiener:CAN6:Crate29:Channel7 }

class: CSC_ME_M2_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR2_LV_MRTN is_of_class CSC_ME_M2_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate30 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate30 }
objectset: CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate30 }

class: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate30:Channel0 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate30:Channel1 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate30:Channel3 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate30:Channel4 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate30:Channel5 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate30:Channel7 is_of_class CSC_ME_M2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate30:Channel0,
	Wiener:CAN6:Crate30:Channel1,
	Wiener:CAN6:Crate30:Channel3,
	Wiener:CAN6:Crate30:Channel4,
	Wiener:CAN6:Crate30:Channel5,
	Wiener:CAN6:Crate30:Channel7 }
objectset: CSC_ME_M2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate30:Channel0,
	Wiener:CAN6:Crate30:Channel1,
	Wiener:CAN6:Crate30:Channel3,
	Wiener:CAN6:Crate30:Channel4,
	Wiener:CAN6:Crate30:Channel5,
	Wiener:CAN6:Crate30:Channel7 }

class: CSC_ME_M2_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR3_LV_MRTN is_of_class CSC_ME_M2_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate32 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate32 }
objectset: CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate32 }

class: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate32:Channel0 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate32:Channel1 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate32:Channel3 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate32:Channel4 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate32:Channel5 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate32:Channel7 is_of_class CSC_ME_M2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate32:Channel0,
	Wiener:CAN7:Crate32:Channel1,
	Wiener:CAN7:Crate32:Channel3,
	Wiener:CAN7:Crate32:Channel4,
	Wiener:CAN7:Crate32:Channel5,
	Wiener:CAN7:Crate32:Channel7 }
objectset: CSC_ME_M2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate32:Channel0,
	Wiener:CAN7:Crate32:Channel1,
	Wiener:CAN7:Crate32:Channel3,
	Wiener:CAN7:Crate32:Channel4,
	Wiener:CAN7:Crate32:Channel5,
	Wiener:CAN7:Crate32:Channel7 }

class: CSC_ME_M2_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR4_LV_MRTN is_of_class CSC_ME_M2_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate31 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate31 }
objectset: CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate31 }

class: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate31:Channel0 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate31:Channel1 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate31:Channel3 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate31:Channel4 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate31:Channel5 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate31:Channel7 is_of_class CSC_ME_M2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate31:Channel0,
	Wiener:CAN7:Crate31:Channel1,
	Wiener:CAN7:Crate31:Channel3,
	Wiener:CAN7:Crate31:Channel4,
	Wiener:CAN7:Crate31:Channel5,
	Wiener:CAN7:Crate31:Channel7 }
objectset: CSC_ME_M2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate31:Channel0,
	Wiener:CAN7:Crate31:Channel1,
	Wiener:CAN7:Crate31:Channel3,
	Wiener:CAN7:Crate31:Channel4,
	Wiener:CAN7:Crate31:Channel5,
	Wiener:CAN7:Crate31:Channel7 }

class: CSC_ME_M2_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR5_LV_MRTN is_of_class CSC_ME_M2_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate34 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate34 }
objectset: CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate34 }

class: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate34:Channel0 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate34:Channel1 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate34:Channel3 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate34:Channel4 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate34:Channel5 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate34:Channel7 is_of_class CSC_ME_M2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate34:Channel0,
	Wiener:CAN7:Crate34:Channel1,
	Wiener:CAN7:Crate34:Channel3,
	Wiener:CAN7:Crate34:Channel4,
	Wiener:CAN7:Crate34:Channel5,
	Wiener:CAN7:Crate34:Channel7 }
objectset: CSC_ME_M2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate34:Channel0,
	Wiener:CAN7:Crate34:Channel1,
	Wiener:CAN7:Crate34:Channel3,
	Wiener:CAN7:Crate34:Channel4,
	Wiener:CAN7:Crate34:Channel5,
	Wiener:CAN7:Crate34:Channel7 }

class: CSC_ME_M2_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR6_LV_MRTN is_of_class CSC_ME_M2_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate33 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate33 }
objectset: CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate33 }

class: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate33:Channel0 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate33:Channel1 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate33:Channel3 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate33:Channel4 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate33:Channel5 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate33:Channel7 is_of_class CSC_ME_M2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate33:Channel0,
	Wiener:CAN7:Crate33:Channel1,
	Wiener:CAN7:Crate33:Channel3,
	Wiener:CAN7:Crate33:Channel4,
	Wiener:CAN7:Crate33:Channel5,
	Wiener:CAN7:Crate33:Channel7 }
objectset: CSC_ME_M2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate33:Channel0,
	Wiener:CAN7:Crate33:Channel1,
	Wiener:CAN7:Crate33:Channel3,
	Wiener:CAN7:Crate33:Channel4,
	Wiener:CAN7:Crate33:Channel5,
	Wiener:CAN7:Crate33:Channel7 }

class: CSC_ME_M2_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR7_LV_MRTN is_of_class CSC_ME_M2_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate35 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate35 }
objectset: CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate35 }

class: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate35:Channel0 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate35:Channel1 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate35:Channel3 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate35:Channel4 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate35:Channel5 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate35:Channel7 is_of_class CSC_ME_M2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate35:Channel0,
	Wiener:CAN6:Crate35:Channel1,
	Wiener:CAN6:Crate35:Channel3,
	Wiener:CAN6:Crate35:Channel4,
	Wiener:CAN6:Crate35:Channel5,
	Wiener:CAN6:Crate35:Channel7 }
objectset: CSC_ME_M2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate35:Channel0,
	Wiener:CAN6:Crate35:Channel1,
	Wiener:CAN6:Crate35:Channel3,
	Wiener:CAN6:Crate35:Channel4,
	Wiener:CAN6:Crate35:Channel5,
	Wiener:CAN6:Crate35:Channel7 }

class: CSC_ME_M2_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M2_CR8_LV_MRTN is_of_class CSC_ME_M2_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate36 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate36 }
objectset: CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate36 }

class: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate36:Channel0 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate36:Channel1 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate36:Channel3 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate36:Channel4 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate36:Channel5 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate36:Channel7 is_of_class CSC_ME_M2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate36:Channel0,
	Wiener:CAN6:Crate36:Channel1,
	Wiener:CAN6:Crate36:Channel3,
	Wiener:CAN6:Crate36:Channel4,
	Wiener:CAN6:Crate36:Channel5,
	Wiener:CAN6:Crate36:Channel7 }
objectset: CSC_ME_M2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate36:Channel0,
	Wiener:CAN6:Crate36:Channel1,
	Wiener:CAN6:Crate36:Channel3,
	Wiener:CAN6:Crate36:Channel4,
	Wiener:CAN6:Crate36:Channel5,
	Wiener:CAN6:Crate36:Channel7 }

class: CSC_ME_M2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_M2_LV_MRTN is_of_class CSC_ME_M2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_M2_CR1_LV_MRTN,
	CSC_ME_M2_CR2_LV_MRTN,
	CSC_ME_M2_CR3_LV_MRTN,
	CSC_ME_M2_CR4_LV_MRTN,
	CSC_ME_M2_CR5_LV_MRTN,
	CSC_ME_M2_CR6_LV_MRTN,
	CSC_ME_M2_CR7_LV_MRTN,
	CSC_ME_M2_CR8_LV_MRTN }
objectset: CSC_ME_M2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_M2_CR1_LV_MRTN,
	CSC_ME_M2_CR2_LV_MRTN,
	CSC_ME_M2_CR3_LV_MRTN,
	CSC_ME_M2_CR4_LV_MRTN,
	CSC_ME_M2_CR5_LV_MRTN,
	CSC_ME_M2_CR6_LV_MRTN,
	CSC_ME_M2_CR7_LV_MRTN,
	CSC_ME_M2_CR8_LV_MRTN }

class: CSC_ME_M1_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR1_LV_MRTN is_of_class CSC_ME_M1_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate21 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate21 }
objectset: CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate21 }

class: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate21:Channel0 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate21:Channel1 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate21:Channel3 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate21:Channel4 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate21:Channel5 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate21:Channel7 is_of_class CSC_ME_M1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate21:Channel0,
	Wiener:CAN6:Crate21:Channel1,
	Wiener:CAN6:Crate21:Channel3,
	Wiener:CAN6:Crate21:Channel4,
	Wiener:CAN6:Crate21:Channel5,
	Wiener:CAN6:Crate21:Channel7 }
objectset: CSC_ME_M1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate21:Channel0,
	Wiener:CAN6:Crate21:Channel1,
	Wiener:CAN6:Crate21:Channel3,
	Wiener:CAN6:Crate21:Channel4,
	Wiener:CAN6:Crate21:Channel5,
	Wiener:CAN6:Crate21:Channel7 }

class: CSC_ME_M1_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR2_LV_MRTN is_of_class CSC_ME_M1_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate22 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate22 }
objectset: CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate22 }

class: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate22:Channel0 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate22:Channel1 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate22:Channel3 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate22:Channel4 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate22:Channel5 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate22:Channel7 is_of_class CSC_ME_M1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate22:Channel0,
	Wiener:CAN6:Crate22:Channel1,
	Wiener:CAN6:Crate22:Channel3,
	Wiener:CAN6:Crate22:Channel4,
	Wiener:CAN6:Crate22:Channel5,
	Wiener:CAN6:Crate22:Channel7 }
objectset: CSC_ME_M1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate22:Channel0,
	Wiener:CAN6:Crate22:Channel1,
	Wiener:CAN6:Crate22:Channel3,
	Wiener:CAN6:Crate22:Channel4,
	Wiener:CAN6:Crate22:Channel5,
	Wiener:CAN6:Crate22:Channel7 }

class: CSC_ME_M1_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR3_LV_MRTN is_of_class CSC_ME_M1_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate24 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate24 }
objectset: CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate24 }

class: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate24:Channel0 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate24:Channel1 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate24:Channel3 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate24:Channel4 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate24:Channel5 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate24:Channel7 is_of_class CSC_ME_M1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate24:Channel0,
	Wiener:CAN7:Crate24:Channel1,
	Wiener:CAN7:Crate24:Channel3,
	Wiener:CAN7:Crate24:Channel4,
	Wiener:CAN7:Crate24:Channel5,
	Wiener:CAN7:Crate24:Channel7 }
objectset: CSC_ME_M1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate24:Channel0,
	Wiener:CAN7:Crate24:Channel1,
	Wiener:CAN7:Crate24:Channel3,
	Wiener:CAN7:Crate24:Channel4,
	Wiener:CAN7:Crate24:Channel5,
	Wiener:CAN7:Crate24:Channel7 }

class: CSC_ME_M1_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR4_LV_MRTN is_of_class CSC_ME_M1_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate23 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate23 }
objectset: CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate23 }

class: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate23:Channel0 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate23:Channel1 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate23:Channel3 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate23:Channel4 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate23:Channel5 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate23:Channel7 is_of_class CSC_ME_M1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate23:Channel0,
	Wiener:CAN7:Crate23:Channel1,
	Wiener:CAN7:Crate23:Channel3,
	Wiener:CAN7:Crate23:Channel4,
	Wiener:CAN7:Crate23:Channel5,
	Wiener:CAN7:Crate23:Channel7 }
objectset: CSC_ME_M1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate23:Channel0,
	Wiener:CAN7:Crate23:Channel1,
	Wiener:CAN7:Crate23:Channel3,
	Wiener:CAN7:Crate23:Channel4,
	Wiener:CAN7:Crate23:Channel5,
	Wiener:CAN7:Crate23:Channel7 }

class: CSC_ME_M1_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR5_LV_MRTN is_of_class CSC_ME_M1_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate26 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate26 }
objectset: CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate26 }

class: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate26:Channel0 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate26:Channel1 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate26:Channel3 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate26:Channel4 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate26:Channel5 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate26:Channel7 is_of_class CSC_ME_M1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate26:Channel0,
	Wiener:CAN7:Crate26:Channel1,
	Wiener:CAN7:Crate26:Channel3,
	Wiener:CAN7:Crate26:Channel4,
	Wiener:CAN7:Crate26:Channel5,
	Wiener:CAN7:Crate26:Channel7 }
objectset: CSC_ME_M1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate26:Channel0,
	Wiener:CAN7:Crate26:Channel1,
	Wiener:CAN7:Crate26:Channel3,
	Wiener:CAN7:Crate26:Channel4,
	Wiener:CAN7:Crate26:Channel5,
	Wiener:CAN7:Crate26:Channel7 }

class: CSC_ME_M1_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR6_LV_MRTN is_of_class CSC_ME_M1_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN7:Crate25 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN7:Crate25 }
objectset: CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN7:Crate25 }

class: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN7:Crate25:Channel0 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate25:Channel1 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate25:Channel3 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate25:Channel4 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate25:Channel5 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN7:Crate25:Channel7 is_of_class CSC_ME_M1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN7:Crate25:Channel0,
	Wiener:CAN7:Crate25:Channel1,
	Wiener:CAN7:Crate25:Channel3,
	Wiener:CAN7:Crate25:Channel4,
	Wiener:CAN7:Crate25:Channel5,
	Wiener:CAN7:Crate25:Channel7 }
objectset: CSC_ME_M1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN7:Crate25:Channel0,
	Wiener:CAN7:Crate25:Channel1,
	Wiener:CAN7:Crate25:Channel3,
	Wiener:CAN7:Crate25:Channel4,
	Wiener:CAN7:Crate25:Channel5,
	Wiener:CAN7:Crate25:Channel7 }

class: CSC_ME_M1_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR7_LV_MRTN is_of_class CSC_ME_M1_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate27 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate27 }
objectset: CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate27 }

class: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate27:Channel0 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate27:Channel1 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate27:Channel3 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate27:Channel4 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate27:Channel5 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate27:Channel7 is_of_class CSC_ME_M1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate27:Channel0,
	Wiener:CAN6:Crate27:Channel1,
	Wiener:CAN6:Crate27:Channel3,
	Wiener:CAN6:Crate27:Channel4,
	Wiener:CAN6:Crate27:Channel5,
	Wiener:CAN6:Crate27:Channel7 }
objectset: CSC_ME_M1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate27:Channel0,
	Wiener:CAN6:Crate27:Channel1,
	Wiener:CAN6:Crate27:Channel3,
	Wiener:CAN6:Crate27:Channel4,
	Wiener:CAN6:Crate27:Channel5,
	Wiener:CAN6:Crate27:Channel7 }

class: CSC_ME_M1_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_M1_CR8_LV_MRTN is_of_class CSC_ME_M1_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN6:Crate28 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN6:Crate28 }
objectset: CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN6:Crate28 }

class: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN6:Crate28:Channel0 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate28:Channel1 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate28:Channel3 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate28:Channel4 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate28:Channel5 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN6:Crate28:Channel7 is_of_class CSC_ME_M1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN6:Crate28:Channel0,
	Wiener:CAN6:Crate28:Channel1,
	Wiener:CAN6:Crate28:Channel3,
	Wiener:CAN6:Crate28:Channel4,
	Wiener:CAN6:Crate28:Channel5,
	Wiener:CAN6:Crate28:Channel7 }
objectset: CSC_ME_M1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN6:Crate28:Channel0,
	Wiener:CAN6:Crate28:Channel1,
	Wiener:CAN6:Crate28:Channel3,
	Wiener:CAN6:Crate28:Channel4,
	Wiener:CAN6:Crate28:Channel5,
	Wiener:CAN6:Crate28:Channel7 }

class: CSC_ME_M1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_M1_LV_MRTN is_of_class CSC_ME_M1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_M1_CR1_LV_MRTN,
	CSC_ME_M1_CR2_LV_MRTN,
	CSC_ME_M1_CR3_LV_MRTN,
	CSC_ME_M1_CR4_LV_MRTN,
	CSC_ME_M1_CR5_LV_MRTN,
	CSC_ME_M1_CR6_LV_MRTN,
	CSC_ME_M1_CR7_LV_MRTN,
	CSC_ME_M1_CR8_LV_MRTN }
objectset: CSC_ME_M1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_M1_CR1_LV_MRTN,
	CSC_ME_M1_CR2_LV_MRTN,
	CSC_ME_M1_CR3_LV_MRTN,
	CSC_ME_M1_CR4_LV_MRTN,
	CSC_ME_M1_CR5_LV_MRTN,
	CSC_ME_M1_CR6_LV_MRTN,
	CSC_ME_M1_CR7_LV_MRTN,
	CSC_ME_M1_CR8_LV_MRTN }

class: CSC_ME_P1_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR1_LV_MRTN is_of_class CSC_ME_P1_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate1 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate1 }
objectset: CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate1 }

class: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate1:Channel0 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate1:Channel1 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate1:Channel3 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate1:Channel4 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate1:Channel5 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate1:Channel7 is_of_class CSC_ME_P1_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate1:Channel0,
	Wiener:CAN4:Crate1:Channel1,
	Wiener:CAN4:Crate1:Channel3,
	Wiener:CAN4:Crate1:Channel4,
	Wiener:CAN4:Crate1:Channel5,
	Wiener:CAN4:Crate1:Channel7 }
objectset: CSC_ME_P1_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate1:Channel0,
	Wiener:CAN4:Crate1:Channel1,
	Wiener:CAN4:Crate1:Channel3,
	Wiener:CAN4:Crate1:Channel4,
	Wiener:CAN4:Crate1:Channel5,
	Wiener:CAN4:Crate1:Channel7 }

class: CSC_ME_P1_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR2_LV_MRTN is_of_class CSC_ME_P1_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate2 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate2 }
objectset: CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate2 }

class: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate2:Channel0 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate2:Channel1 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate2:Channel3 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate2:Channel4 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate2:Channel5 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate2:Channel7 is_of_class CSC_ME_P1_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate2:Channel0,
	Wiener:CAN4:Crate2:Channel1,
	Wiener:CAN4:Crate2:Channel3,
	Wiener:CAN4:Crate2:Channel4,
	Wiener:CAN4:Crate2:Channel5,
	Wiener:CAN4:Crate2:Channel7 }
objectset: CSC_ME_P1_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate2:Channel0,
	Wiener:CAN4:Crate2:Channel1,
	Wiener:CAN4:Crate2:Channel3,
	Wiener:CAN4:Crate2:Channel4,
	Wiener:CAN4:Crate2:Channel5,
	Wiener:CAN4:Crate2:Channel7 }

class: CSC_ME_P1_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR3_LV_MRTN is_of_class CSC_ME_P1_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate4 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate4 }
objectset: CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate4 }

class: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate4:Channel0 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate4:Channel1 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate4:Channel3 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate4:Channel4 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate4:Channel5 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate4:Channel7 is_of_class CSC_ME_P1_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate4:Channel0,
	Wiener:CAN5:Crate4:Channel1,
	Wiener:CAN5:Crate4:Channel3,
	Wiener:CAN5:Crate4:Channel4,
	Wiener:CAN5:Crate4:Channel5,
	Wiener:CAN5:Crate4:Channel7 }
objectset: CSC_ME_P1_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate4:Channel0,
	Wiener:CAN5:Crate4:Channel1,
	Wiener:CAN5:Crate4:Channel3,
	Wiener:CAN5:Crate4:Channel4,
	Wiener:CAN5:Crate4:Channel5,
	Wiener:CAN5:Crate4:Channel7 }

class: CSC_ME_P1_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR4_LV_MRTN is_of_class CSC_ME_P1_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate3 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate3 }
objectset: CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate3 }

class: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate3:Channel0 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate3:Channel1 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate3:Channel3 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate3:Channel4 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate3:Channel5 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate3:Channel7 is_of_class CSC_ME_P1_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate3:Channel0,
	Wiener:CAN5:Crate3:Channel1,
	Wiener:CAN5:Crate3:Channel3,
	Wiener:CAN5:Crate3:Channel4,
	Wiener:CAN5:Crate3:Channel5,
	Wiener:CAN5:Crate3:Channel7 }
objectset: CSC_ME_P1_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate3:Channel0,
	Wiener:CAN5:Crate3:Channel1,
	Wiener:CAN5:Crate3:Channel3,
	Wiener:CAN5:Crate3:Channel4,
	Wiener:CAN5:Crate3:Channel5,
	Wiener:CAN5:Crate3:Channel7 }

class: CSC_ME_P1_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR5_LV_MRTN is_of_class CSC_ME_P1_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate6 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate6 }
objectset: CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate6 }

class: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate6:Channel0 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate6:Channel1 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate6:Channel3 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate6:Channel4 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate6:Channel5 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate6:Channel7 is_of_class CSC_ME_P1_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate6:Channel0,
	Wiener:CAN5:Crate6:Channel1,
	Wiener:CAN5:Crate6:Channel3,
	Wiener:CAN5:Crate6:Channel4,
	Wiener:CAN5:Crate6:Channel5,
	Wiener:CAN5:Crate6:Channel7 }
objectset: CSC_ME_P1_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate6:Channel0,
	Wiener:CAN5:Crate6:Channel1,
	Wiener:CAN5:Crate6:Channel3,
	Wiener:CAN5:Crate6:Channel4,
	Wiener:CAN5:Crate6:Channel5,
	Wiener:CAN5:Crate6:Channel7 }

class: CSC_ME_P1_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR6_LV_MRTN is_of_class CSC_ME_P1_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate5 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate5 }
objectset: CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate5 }

class: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate5:Channel0 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate5:Channel1 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate5:Channel3 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate5:Channel4 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate5:Channel5 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate5:Channel7 is_of_class CSC_ME_P1_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate5:Channel0,
	Wiener:CAN5:Crate5:Channel1,
	Wiener:CAN5:Crate5:Channel3,
	Wiener:CAN5:Crate5:Channel4,
	Wiener:CAN5:Crate5:Channel5,
	Wiener:CAN5:Crate5:Channel7 }
objectset: CSC_ME_P1_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate5:Channel0,
	Wiener:CAN5:Crate5:Channel1,
	Wiener:CAN5:Crate5:Channel3,
	Wiener:CAN5:Crate5:Channel4,
	Wiener:CAN5:Crate5:Channel5,
	Wiener:CAN5:Crate5:Channel7 }

class: CSC_ME_P1_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR7_LV_MRTN is_of_class CSC_ME_P1_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate7 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate7 }
objectset: CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate7 }

class: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate7:Channel0 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate7:Channel1 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate7:Channel3 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate7:Channel4 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate7:Channel5 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate7:Channel7 is_of_class CSC_ME_P1_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate7:Channel0,
	Wiener:CAN4:Crate7:Channel1,
	Wiener:CAN4:Crate7:Channel3,
	Wiener:CAN4:Crate7:Channel4,
	Wiener:CAN4:Crate7:Channel5,
	Wiener:CAN4:Crate7:Channel7 }
objectset: CSC_ME_P1_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate7:Channel0,
	Wiener:CAN4:Crate7:Channel1,
	Wiener:CAN4:Crate7:Channel3,
	Wiener:CAN4:Crate7:Channel4,
	Wiener:CAN4:Crate7:Channel5,
	Wiener:CAN4:Crate7:Channel7 }

class: CSC_ME_P1_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P1_CR8_LV_MRTN is_of_class CSC_ME_P1_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate8 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate8 }
objectset: CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate8 }

class: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate8:Channel0 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate8:Channel1 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate8:Channel3 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate8:Channel4 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate8:Channel5 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate8:Channel7 is_of_class CSC_ME_P1_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate8:Channel0,
	Wiener:CAN4:Crate8:Channel1,
	Wiener:CAN4:Crate8:Channel3,
	Wiener:CAN4:Crate8:Channel4,
	Wiener:CAN4:Crate8:Channel5,
	Wiener:CAN4:Crate8:Channel7 }
objectset: CSC_ME_P1_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate8:Channel0,
	Wiener:CAN4:Crate8:Channel1,
	Wiener:CAN4:Crate8:Channel3,
	Wiener:CAN4:Crate8:Channel4,
	Wiener:CAN4:Crate8:Channel5,
	Wiener:CAN4:Crate8:Channel7 }

class: CSC_ME_P1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_P1_LV_MRTN is_of_class CSC_ME_P1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_P1_CR1_LV_MRTN,
	CSC_ME_P1_CR2_LV_MRTN,
	CSC_ME_P1_CR3_LV_MRTN,
	CSC_ME_P1_CR4_LV_MRTN,
	CSC_ME_P1_CR5_LV_MRTN,
	CSC_ME_P1_CR6_LV_MRTN,
	CSC_ME_P1_CR7_LV_MRTN,
	CSC_ME_P1_CR8_LV_MRTN }
objectset: CSC_ME_P1_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_P1_CR1_LV_MRTN,
	CSC_ME_P1_CR2_LV_MRTN,
	CSC_ME_P1_CR3_LV_MRTN,
	CSC_ME_P1_CR4_LV_MRTN,
	CSC_ME_P1_CR5_LV_MRTN,
	CSC_ME_P1_CR6_LV_MRTN,
	CSC_ME_P1_CR7_LV_MRTN,
	CSC_ME_P1_CR8_LV_MRTN }

class: CSC_ME_P2_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR1_LV_MRTN is_of_class CSC_ME_P2_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate9 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate9 }
objectset: CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate9 }

class: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate9:Channel0 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate9:Channel1 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate9:Channel3 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate9:Channel4 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate9:Channel5 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate9:Channel7 is_of_class CSC_ME_P2_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate9:Channel0,
	Wiener:CAN4:Crate9:Channel1,
	Wiener:CAN4:Crate9:Channel3,
	Wiener:CAN4:Crate9:Channel4,
	Wiener:CAN4:Crate9:Channel5,
	Wiener:CAN4:Crate9:Channel7 }
objectset: CSC_ME_P2_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate9:Channel0,
	Wiener:CAN4:Crate9:Channel1,
	Wiener:CAN4:Crate9:Channel3,
	Wiener:CAN4:Crate9:Channel4,
	Wiener:CAN4:Crate9:Channel5,
	Wiener:CAN4:Crate9:Channel7 }

class: CSC_ME_P2_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR2_LV_MRTN is_of_class CSC_ME_P2_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate10 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate10 }
objectset: CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate10 }

class: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate10:Channel0 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate10:Channel1 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate10:Channel3 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate10:Channel4 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate10:Channel5 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate10:Channel7 is_of_class CSC_ME_P2_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate10:Channel0,
	Wiener:CAN4:Crate10:Channel1,
	Wiener:CAN4:Crate10:Channel3,
	Wiener:CAN4:Crate10:Channel4,
	Wiener:CAN4:Crate10:Channel5,
	Wiener:CAN4:Crate10:Channel7 }
objectset: CSC_ME_P2_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate10:Channel0,
	Wiener:CAN4:Crate10:Channel1,
	Wiener:CAN4:Crate10:Channel3,
	Wiener:CAN4:Crate10:Channel4,
	Wiener:CAN4:Crate10:Channel5,
	Wiener:CAN4:Crate10:Channel7 }

class: CSC_ME_P2_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR3_LV_MRTN is_of_class CSC_ME_P2_CR3_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate12 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate12 }
objectset: CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate12 }

class: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate12:Channel0 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate12:Channel1 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate12:Channel3 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate12:Channel4 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate12:Channel5 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate12:Channel7 is_of_class CSC_ME_P2_CR3_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate12:Channel0,
	Wiener:CAN5:Crate12:Channel1,
	Wiener:CAN5:Crate12:Channel3,
	Wiener:CAN5:Crate12:Channel4,
	Wiener:CAN5:Crate12:Channel5,
	Wiener:CAN5:Crate12:Channel7 }
objectset: CSC_ME_P2_CR3_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate12:Channel0,
	Wiener:CAN5:Crate12:Channel1,
	Wiener:CAN5:Crate12:Channel3,
	Wiener:CAN5:Crate12:Channel4,
	Wiener:CAN5:Crate12:Channel5,
	Wiener:CAN5:Crate12:Channel7 }

class: CSC_ME_P2_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR4_LV_MRTN is_of_class CSC_ME_P2_CR4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate11 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate11 }
objectset: CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate11 }

class: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate11:Channel0 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate11:Channel1 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate11:Channel3 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate11:Channel4 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate11:Channel5 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate11:Channel7 is_of_class CSC_ME_P2_CR4_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate11:Channel0,
	Wiener:CAN5:Crate11:Channel1,
	Wiener:CAN5:Crate11:Channel3,
	Wiener:CAN5:Crate11:Channel4,
	Wiener:CAN5:Crate11:Channel5,
	Wiener:CAN5:Crate11:Channel7 }
objectset: CSC_ME_P2_CR4_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate11:Channel0,
	Wiener:CAN5:Crate11:Channel1,
	Wiener:CAN5:Crate11:Channel3,
	Wiener:CAN5:Crate11:Channel4,
	Wiener:CAN5:Crate11:Channel5,
	Wiener:CAN5:Crate11:Channel7 }

class: CSC_ME_P2_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR5_LV_MRTN is_of_class CSC_ME_P2_CR5_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate14 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate14 }
objectset: CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate14 }

class: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate14:Channel0 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate14:Channel1 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate14:Channel3 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate14:Channel4 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate14:Channel5 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate14:Channel7 is_of_class CSC_ME_P2_CR5_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate14:Channel0,
	Wiener:CAN5:Crate14:Channel1,
	Wiener:CAN5:Crate14:Channel3,
	Wiener:CAN5:Crate14:Channel4,
	Wiener:CAN5:Crate14:Channel5,
	Wiener:CAN5:Crate14:Channel7 }
objectset: CSC_ME_P2_CR5_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate14:Channel0,
	Wiener:CAN5:Crate14:Channel1,
	Wiener:CAN5:Crate14:Channel3,
	Wiener:CAN5:Crate14:Channel4,
	Wiener:CAN5:Crate14:Channel5,
	Wiener:CAN5:Crate14:Channel7 }

class: CSC_ME_P2_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR6_LV_MRTN is_of_class CSC_ME_P2_CR6_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate13 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate13 }
objectset: CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate13 }

class: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate13:Channel0 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate13:Channel1 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate13:Channel3 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate13:Channel4 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate13:Channel5 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate13:Channel7 is_of_class CSC_ME_P2_CR6_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate13:Channel0,
	Wiener:CAN5:Crate13:Channel1,
	Wiener:CAN5:Crate13:Channel3,
	Wiener:CAN5:Crate13:Channel4,
	Wiener:CAN5:Crate13:Channel5,
	Wiener:CAN5:Crate13:Channel7 }
objectset: CSC_ME_P2_CR6_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate13:Channel0,
	Wiener:CAN5:Crate13:Channel1,
	Wiener:CAN5:Crate13:Channel3,
	Wiener:CAN5:Crate13:Channel4,
	Wiener:CAN5:Crate13:Channel5,
	Wiener:CAN5:Crate13:Channel7 }

class: CSC_ME_P2_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR7_LV_MRTN is_of_class CSC_ME_P2_CR7_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate15 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate15 }
objectset: CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate15 }

class: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate15:Channel0 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate15:Channel1 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate15:Channel3 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate15:Channel4 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate15:Channel5 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate15:Channel7 is_of_class CSC_ME_P2_CR7_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate15:Channel0,
	Wiener:CAN4:Crate15:Channel1,
	Wiener:CAN4:Crate15:Channel3,
	Wiener:CAN4:Crate15:Channel4,
	Wiener:CAN4:Crate15:Channel5,
	Wiener:CAN4:Crate15:Channel7 }
objectset: CSC_ME_P2_CR7_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate15:Channel0,
	Wiener:CAN4:Crate15:Channel1,
	Wiener:CAN4:Crate15:Channel3,
	Wiener:CAN4:Crate15:Channel4,
	Wiener:CAN4:Crate15:Channel5,
	Wiener:CAN4:Crate15:Channel7 }

class: CSC_ME_P2_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P2_CR8_LV_MRTN is_of_class CSC_ME_P2_CR8_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate16 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate16 }
objectset: CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate16 }

class: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate16:Channel0 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate16:Channel1 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate16:Channel3 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate16:Channel4 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate16:Channel5 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate16:Channel7 is_of_class CSC_ME_P2_CR8_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate16:Channel0,
	Wiener:CAN4:Crate16:Channel1,
	Wiener:CAN4:Crate16:Channel3,
	Wiener:CAN4:Crate16:Channel4,
	Wiener:CAN4:Crate16:Channel5,
	Wiener:CAN4:Crate16:Channel7 }
objectset: CSC_ME_P2_CR8_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate16:Channel0,
	Wiener:CAN4:Crate16:Channel1,
	Wiener:CAN4:Crate16:Channel3,
	Wiener:CAN4:Crate16:Channel4,
	Wiener:CAN4:Crate16:Channel5,
	Wiener:CAN4:Crate16:Channel7 }

class: CSC_ME_P2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_P2_LV_MRTN is_of_class CSC_ME_P2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_P2_CR1_LV_MRTN,
	CSC_ME_P2_CR2_LV_MRTN,
	CSC_ME_P2_CR3_LV_MRTN,
	CSC_ME_P2_CR4_LV_MRTN,
	CSC_ME_P2_CR5_LV_MRTN,
	CSC_ME_P2_CR6_LV_MRTN,
	CSC_ME_P2_CR7_LV_MRTN,
	CSC_ME_P2_CR8_LV_MRTN }
objectset: CSC_ME_P2_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_P2_CR1_LV_MRTN,
	CSC_ME_P2_CR2_LV_MRTN,
	CSC_ME_P2_CR3_LV_MRTN,
	CSC_ME_P2_CR4_LV_MRTN,
	CSC_ME_P2_CR5_LV_MRTN,
	CSC_ME_P2_CR6_LV_MRTN,
	CSC_ME_P2_CR7_LV_MRTN,
	CSC_ME_P2_CR8_LV_MRTN }

class: CSC_ME_P4_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P4_CR1_LV_MRTN is_of_class CSC_ME_P4_CR1_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN5:Crate17 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN5:Crate17 }
objectset: CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN5:Crate17 }

class: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN5:Crate17:Channel0 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate17:Channel1 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate17:Channel3 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate17:Channel4 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate17:Channel5 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN5:Crate17:Channel7 is_of_class CSC_ME_P4_CR1_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN5:Crate17:Channel0,
	Wiener:CAN5:Crate17:Channel1,
	Wiener:CAN5:Crate17:Channel3,
	Wiener:CAN5:Crate17:Channel4,
	Wiener:CAN5:Crate17:Channel5,
	Wiener:CAN5:Crate17:Channel7 }
objectset: CSC_ME_P4_CR1_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN5:Crate17:Channel0,
	Wiener:CAN5:Crate17:Channel1,
	Wiener:CAN5:Crate17:Channel3,
	Wiener:CAN5:Crate17:Channel4,
	Wiener:CAN5:Crate17:Channel5,
	Wiener:CAN5:Crate17:Channel7 }

class: CSC_ME_P4_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state ON ) and
          ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) and
          ( all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {ERROR,DEAD} ) or
          ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) or
          ( any_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do OFF all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            do ON all_in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS

object: CSC_ME_P4_CR2_LV_MRTN is_of_class CSC_ME_P4_CR2_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathon_FWDM is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathon_FwDevMode_CLASS


class: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathon_CLASS/associated
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

object: Wiener:CAN4:Crate18 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathon_CLASS

objectset: CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETSTATES {Wiener:CAN4:Crate18 }
objectset: CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHON_FWSETACTIONS {Wiener:CAN4:Crate18 }

class: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_FWDM is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_FwDevMode_CLASS


class: CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS/associated
!panel: FwWienerMarathonChannel.pnl
    parameters: int estimatedTime = 0
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
    state: RECOVERING	!color: FwStateAttention2
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1

object: Wiener:CAN4:Crate18:Channel0 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate18:Channel1 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate18:Channel3 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate18:Channel4 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate18:Channel5 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

object: Wiener:CAN4:Crate18:Channel7 is_of_class CSC_ME_P4_CR2_LV_MRTNFwWienerMarathonChannel_CLASS

objectset: CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETSTATES {Wiener:CAN4:Crate18:Channel0,
	Wiener:CAN4:Crate18:Channel1,
	Wiener:CAN4:Crate18:Channel3,
	Wiener:CAN4:Crate18:Channel4,
	Wiener:CAN4:Crate18:Channel5,
	Wiener:CAN4:Crate18:Channel7 }
objectset: CSC_ME_P4_CR2_LV_MRTNFWWIENERMARATHONCHANNEL_FWSETACTIONS {Wiener:CAN4:Crate18:Channel0,
	Wiener:CAN4:Crate18:Channel1,
	Wiener:CAN4:Crate18:Channel3,
	Wiener:CAN4:Crate18:Channel4,
	Wiener:CAN4:Crate18:Channel5,
	Wiener:CAN4:Crate18:Channel7 }

class: CSC_ME_P4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_P4_LV_MRTN is_of_class CSC_ME_P4_LV_MRTNTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_P4_CR1_LV_MRTN,
	CSC_ME_P4_CR2_LV_MRTN }
objectset: CSC_ME_P4_LV_MRTNCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_P4_CR1_LV_MRTN,
	CSC_ME_P4_CR2_LV_MRTN }

class: TOP_CSC_LV_NODES_CLASS
!panel: CSC_LV_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_LV_TREE_NODES_FWSETSTATES in_state ERROR ) ) move_to ERROR
                when ( ( all_in CSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
           when ( ( any_in CSC_LV_TREE_NODES_FWSETSTATES in_state ERROR ) )  move_to ERROR
           when ( ( any_in CSC_LV_TREE_NODES_FWSETSTATES in_state OFF ) )  move_to OFF
        action: STANDBY	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_LV_TREE_NODES_FWSETACTIONS
    state: STANDBY	!color: FwStateAttention1
        when ( ( any_in CSC_LV_TREE_NODES_FWSETSTATES in_state ERROR ) ) move_to ERROR
          when ( ( all_in CSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
         when ( ( any_in CSC_LV_TREE_NODES_FWSETSTATES in_state OFF ) )  move_to OFF
        action: ON	!visible: 1
            do ON all_in CSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_LV_TREE_NODES_FWSETACTIONS

object: CMS_CSC_LV_CAN2 is_of_class TOP_CSC_LV_NODES_CLASS

class: FwChildrenMode_CLASS
!panel: FwChildrenMode.pnl
    state: Complete	!color: _3DFace
        when ( ( any_in FWDEVMODE_FWSETSTATES in_state DISABLED ) )  move_to IncompleteDev
    state: Incomplete	!color: FwStateAttention2
    state: IncompleteDev	!color: FwStateAttention1
        when (  ( ( all_in FWDEVMODE_FWSETSTATES not_in_state DISABLED ) )  ) move_to Complete

object: CMS_CSC_LV_CAN2_FWCNM is_of_class FwChildrenMode_CLASS

class: FwMode_CLASS
!panel: FwMode.pnl
    state: Excluded	!color: FwStateOKNotPhysics
        action: Take(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 1
            move_to InLocal
        action: Include(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
            move_to Included
        action: Manual	!visible: 0
            move_to Manual
        action: Ignore	!visible: 0
            move_to Ignored
    state: Included	!color: FwStateOKPhysics
        action: Exclude(string OWNER = "")	!visible: 0
            move_to Excluded
        action: Manual(string OWNER = "")	!visible: 0
            move_to Manual
        action: Ignore(string OWNER = "")	!visible: 0
            move_to Ignored
        action: ExcludeAll(string OWNER = "")	!visible: 0
            move_to Excluded
        action: Include(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
            move_to Included
        action: Free(string OWNER = "")	!visible: 0
            move_to Included
        action: SetMode(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
    state: InLocal	!color: FwStateOKNotPhysics
        action: Release(string OWNER = "")	!visible: 1
            move_to Excluded
        action: ReleaseAll(string OWNER = "")	!visible: 1
            move_to Excluded
        action: SetMode(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
        action: Take(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 1
            move_to InLocal
    state: Manual	!color: FwStateOKNotPhysics
        action: Include(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
            move_to Included
        action: Take(string OWNER = "")	!visible: 1
            move_to InManual
        action: Exclude(string OWNER = "")	!visible: 0
            move_to Excluded
        action: Ignore	!visible: 0
            move_to Ignored
        action: Free(string OWNER = "")	!visible: 0
            move_to Excluded
        action: ExcludeAll(string OWNER = "")	!visible: 0
            move_to Excluded
    state: InManual	!color: FwStateOKNotPhysics
        action: Release(string OWNER = "")	!visible: 1
            move_to Manual
        action: SetMode(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
        action: ReleaseAll(string OWNER = "")	!visible: 0
            move_to Excluded
        action: SetInLocal	!visible: 0
            move_to InLocal
    state: Ignored	!color: FwStateOKNotPhysics
        action: Include	!visible: 0
            move_to Included
        action: Exclude(string OWNER = "")	!visible: 0
            move_to Excluded
        action: Manual	!visible: 0
            move_to Manual
        action: SetMode(string OWNER = "",string EXCLUSIVE = "YES")	!visible: 0
        action: Free(string OWNER = "")	!visible: 0
            move_to Included
        action: ExcludeAll(string OWNER = "")	!visible: 0
            move_to Excluded

object: CMS_CSC_LV_CAN2_FWM is_of_class FwMode_CLASS

class: CSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_LV_TREE_NODES_FWDM is_of_class CSC_LV_TREE_NODES_FwDevMode_CLASS


class: CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
        action: OFF	!visible: 1
    state: ERROR	!color: FwStateAttention3
        action: OFF	!visible: 1
        action: ON	!visible: 1
        action: STANDBY	!visible: 1
    state: ON	!color: FwStateOKPhysics
        action: OFF	!visible: 1
        action: STANDBY	!visible: 1

object: CSC_ME_M3_LV_MRTN is_of_class CSC_LV_TREE_NODES_CLASS

object: CSC_ME_P3_LV_MRTN is_of_class CSC_LV_TREE_NODES_CLASS

objectset: CSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_M1_LV_MRTN,
	CSC_ME_M2_LV_MRTN,
	CSC_ME_M3_LV_MRTN,
	CSC_ME_M4_LV_MRTN,
	CSC_ME_P1_LV_MRTN,
	CSC_ME_P2_LV_MRTN,
	CSC_ME_P3_LV_MRTN,
	CSC_ME_P4_LV_MRTN }
objectset: CSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_M1_LV_MRTN,
	CSC_ME_M2_LV_MRTN,
	CSC_ME_M3_LV_MRTN,
	CSC_ME_M4_LV_MRTN,
	CSC_ME_P1_LV_MRTN,
	CSC_ME_P2_LV_MRTN,
	CSC_ME_P3_LV_MRTN,
	CSC_ME_P4_LV_MRTN }

class: FwDevMode_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from FWDEVMODE_FWSETSTATES
            remove &VAL_OF_Device from FWDEVMODE_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in FWDEVMODE_FWSETSTATES
            insert &VAL_OF_Device in FWDEVMODE_FWSETACTIONS
            move_to READY

object: FwDevMode_FWDM is_of_class FwDevMode_FwDevMode_CLASS


class: FwDevMode_CLASS/associated
!panel: FwDevMode.pnl
    state: ENABLED	!color: FwStateOKPhysics
    state: DISABLED	!color: FwStateAttention1

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR3_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR5_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR6_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR7_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_CR8_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR3_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR5_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR6_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR7_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_CR8_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M4_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M4_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_M4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR3_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR5_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR6_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR7_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_CR8_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR3_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR5_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR6_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR7_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_CR8_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P4_CR1_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P4_CR2_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2:CSC_ME_P4_LV_MRTN_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN2_FWDM is_of_class FwDevMode_CLASS

objectset: FWDEVMODE_FWSETSTATES {CMS_CSC_LV_CAN2:CSC_ME_M1_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2_FWDM }
objectset: FWDEVMODE_FWSETACTIONS {CMS_CSC_LV_CAN2:CSC_ME_M1_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_M4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR3_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR5_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR6_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR7_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_CR8_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_CR1_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_CR2_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2:CSC_ME_P4_LV_MRTN_FWDM,
	CMS_CSC_LV_CAN2_FWDM }

