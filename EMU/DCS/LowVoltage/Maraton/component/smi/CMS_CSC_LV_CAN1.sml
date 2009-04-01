class: CSC_ME_M4_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_M4_LV_CRB is_of_class CSC_ME_M4_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M4_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_M4_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_M4_PC1_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M4_PC2_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M4_PC3_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M4_PC4_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M4_PC5_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M4_PC6_LV_CRB is_of_class CSC_ME_M4_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_M4_PC1_LV_CRB,
	CRB:CSC_ME_M4_PC2_LV_CRB,
	CRB:CSC_ME_M4_PC3_LV_CRB,
	CRB:CSC_ME_M4_PC4_LV_CRB,
	CRB:CSC_ME_M4_PC5_LV_CRB,
	CRB:CSC_ME_M4_PC6_LV_CRB }
objectset: CSC_ME_M4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_M4_PC1_LV_CRB,
	CRB:CSC_ME_M4_PC2_LV_CRB,
	CRB:CSC_ME_M4_PC3_LV_CRB,
	CRB:CSC_ME_M4_PC4_LV_CRB,
	CRB:CSC_ME_M4_PC5_LV_CRB,
	CRB:CSC_ME_M4_PC6_LV_CRB }

class: CSC_ME_M3_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_M3_LV_CRB is_of_class CSC_ME_M3_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M3_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_M3_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_M3_PC1_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M3_PC2_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M3_PC3_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M3_PC4_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M3_PC5_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M3_PC6_LV_CRB is_of_class CSC_ME_M3_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_M3_PC1_LV_CRB,
	CRB:CSC_ME_M3_PC2_LV_CRB,
	CRB:CSC_ME_M3_PC3_LV_CRB,
	CRB:CSC_ME_M3_PC4_LV_CRB,
	CRB:CSC_ME_M3_PC5_LV_CRB,
	CRB:CSC_ME_M3_PC6_LV_CRB }
objectset: CSC_ME_M3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_M3_PC1_LV_CRB,
	CRB:CSC_ME_M3_PC2_LV_CRB,
	CRB:CSC_ME_M3_PC3_LV_CRB,
	CRB:CSC_ME_M3_PC4_LV_CRB,
	CRB:CSC_ME_M3_PC5_LV_CRB,
	CRB:CSC_ME_M3_PC6_LV_CRB }

class: CSC_ME_M2_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_M2_LV_CRB is_of_class CSC_ME_M2_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M2_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_M2_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_M2_PC1_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M2_PC2_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M2_PC3_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M2_PC4_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M2_PC5_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M2_PC6_LV_CRB is_of_class CSC_ME_M2_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_M2_PC1_LV_CRB,
	CRB:CSC_ME_M2_PC2_LV_CRB,
	CRB:CSC_ME_M2_PC3_LV_CRB,
	CRB:CSC_ME_M2_PC4_LV_CRB,
	CRB:CSC_ME_M2_PC5_LV_CRB,
	CRB:CSC_ME_M2_PC6_LV_CRB }
objectset: CSC_ME_M2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_M2_PC1_LV_CRB,
	CRB:CSC_ME_M2_PC2_LV_CRB,
	CRB:CSC_ME_M2_PC3_LV_CRB,
	CRB:CSC_ME_M2_PC4_LV_CRB,
	CRB:CSC_ME_M2_PC5_LV_CRB,
	CRB:CSC_ME_M2_PC6_LV_CRB }

class: CSC_ME_M1_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_M1_LV_CRB is_of_class CSC_ME_M1_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_M1_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_M1_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_M1_PC1A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC1B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC2A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC2B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC3A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC3B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC4A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC4B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC5A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC5B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC6A_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_M1_PC6B_LV_CRB is_of_class CSC_ME_M1_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_M1_PC1A_LV_CRB,
	CRB:CSC_ME_M1_PC1B_LV_CRB,
	CRB:CSC_ME_M1_PC2A_LV_CRB,
	CRB:CSC_ME_M1_PC2B_LV_CRB,
	CRB:CSC_ME_M1_PC3A_LV_CRB,
	CRB:CSC_ME_M1_PC3B_LV_CRB,
	CRB:CSC_ME_M1_PC4A_LV_CRB,
	CRB:CSC_ME_M1_PC4B_LV_CRB,
	CRB:CSC_ME_M1_PC5A_LV_CRB,
	CRB:CSC_ME_M1_PC5B_LV_CRB,
	CRB:CSC_ME_M1_PC6A_LV_CRB,
	CRB:CSC_ME_M1_PC6B_LV_CRB }
objectset: CSC_ME_M1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_M1_PC1A_LV_CRB,
	CRB:CSC_ME_M1_PC1B_LV_CRB,
	CRB:CSC_ME_M1_PC2A_LV_CRB,
	CRB:CSC_ME_M1_PC2B_LV_CRB,
	CRB:CSC_ME_M1_PC3A_LV_CRB,
	CRB:CSC_ME_M1_PC3B_LV_CRB,
	CRB:CSC_ME_M1_PC4A_LV_CRB,
	CRB:CSC_ME_M1_PC4B_LV_CRB,
	CRB:CSC_ME_M1_PC5A_LV_CRB,
	CRB:CSC_ME_M1_PC5B_LV_CRB,
	CRB:CSC_ME_M1_PC6A_LV_CRB,
	CRB:CSC_ME_M1_PC6B_LV_CRB }

class: CSC_ME_P1_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_P1_LV_CRB is_of_class CSC_ME_P1_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P1_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_P1_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_P1_PC1A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC1B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC2A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC2B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC3A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC3B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC4A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC4B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC5A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC5B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC6A_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P1_PC6B_LV_CRB is_of_class CSC_ME_P1_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_P1_PC1A_LV_CRB,
	CRB:CSC_ME_P1_PC1B_LV_CRB,
	CRB:CSC_ME_P1_PC2A_LV_CRB,
	CRB:CSC_ME_P1_PC2B_LV_CRB,
	CRB:CSC_ME_P1_PC3A_LV_CRB,
	CRB:CSC_ME_P1_PC3B_LV_CRB,
	CRB:CSC_ME_P1_PC4A_LV_CRB,
	CRB:CSC_ME_P1_PC4B_LV_CRB,
	CRB:CSC_ME_P1_PC5A_LV_CRB,
	CRB:CSC_ME_P1_PC5B_LV_CRB,
	CRB:CSC_ME_P1_PC6A_LV_CRB,
	CRB:CSC_ME_P1_PC6B_LV_CRB }
objectset: CSC_ME_P1_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_P1_PC1A_LV_CRB,
	CRB:CSC_ME_P1_PC1B_LV_CRB,
	CRB:CSC_ME_P1_PC2A_LV_CRB,
	CRB:CSC_ME_P1_PC2B_LV_CRB,
	CRB:CSC_ME_P1_PC3A_LV_CRB,
	CRB:CSC_ME_P1_PC3B_LV_CRB,
	CRB:CSC_ME_P1_PC4A_LV_CRB,
	CRB:CSC_ME_P1_PC4B_LV_CRB,
	CRB:CSC_ME_P1_PC5A_LV_CRB,
	CRB:CSC_ME_P1_PC5B_LV_CRB,
	CRB:CSC_ME_P1_PC6A_LV_CRB,
	CRB:CSC_ME_P1_PC6B_LV_CRB }

class: CSC_ME_P2_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_P2_LV_CRB is_of_class CSC_ME_P2_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P2_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_P2_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_P2_PC1_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P2_PC2_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P2_PC3_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P2_PC4_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P2_PC5_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P2_PC6_LV_CRB is_of_class CSC_ME_P2_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_P2_PC1_LV_CRB,
	CRB:CSC_ME_P2_PC2_LV_CRB,
	CRB:CSC_ME_P2_PC3_LV_CRB,
	CRB:CSC_ME_P2_PC4_LV_CRB,
	CRB:CSC_ME_P2_PC5_LV_CRB,
	CRB:CSC_ME_P2_PC6_LV_CRB }
objectset: CSC_ME_P2_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_P2_PC1_LV_CRB,
	CRB:CSC_ME_P2_PC2_LV_CRB,
	CRB:CSC_ME_P2_PC3_LV_CRB,
	CRB:CSC_ME_P2_PC4_LV_CRB,
	CRB:CSC_ME_P2_PC5_LV_CRB,
	CRB:CSC_ME_P2_PC6_LV_CRB }

class: CSC_ME_P3_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_P3_LV_CRB is_of_class CSC_ME_P3_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P3_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_P3_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_P3_PC1_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P3_PC2_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P3_PC3_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P3_PC4_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P3_PC5_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P3_PC6_LV_CRB is_of_class CSC_ME_P3_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_P3_PC1_LV_CRB,
	CRB:CSC_ME_P3_PC2_LV_CRB,
	CRB:CSC_ME_P3_PC3_LV_CRB,
	CRB:CSC_ME_P3_PC4_LV_CRB,
	CRB:CSC_ME_P3_PC5_LV_CRB,
	CRB:CSC_ME_P3_PC6_LV_CRB }
objectset: CSC_ME_P3_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_P3_PC1_LV_CRB,
	CRB:CSC_ME_P3_PC2_LV_CRB,
	CRB:CSC_ME_P3_PC3_LV_CRB,
	CRB:CSC_ME_P3_PC4_LV_CRB,
	CRB:CSC_ME_P3_PC5_LV_CRB,
	CRB:CSC_ME_P3_PC6_LV_CRB }

class: CSC_ME_P4_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS

object: CSC_ME_P4_LV_CRB is_of_class CSC_ME_P4_LV_CRBTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_P4_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS
            move_to READY

object: CSC_ME_P4_LV_CRBfwCrb_CSC_LV_FWDM is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_FwDevMode_CLASS


class: CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS/associated
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

object: CRB:CSC_ME_P4_PC1_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P4_PC2_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P4_PC3_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P4_PC4_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P4_PC5_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

object: CRB:CSC_ME_P4_PC6_LV_CRB is_of_class CSC_ME_P4_LV_CRBfwCrb_CSC_LV_CLASS

objectset: CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETSTATES {CRB:CSC_ME_P4_PC1_LV_CRB,
	CRB:CSC_ME_P4_PC2_LV_CRB,
	CRB:CSC_ME_P4_PC3_LV_CRB,
	CRB:CSC_ME_P4_PC4_LV_CRB,
	CRB:CSC_ME_P4_PC5_LV_CRB,
	CRB:CSC_ME_P4_PC6_LV_CRB }
objectset: CSC_ME_P4_LV_CRBFWCRB_CSC_LV_FWSETACTIONS {CRB:CSC_ME_P4_PC1_LV_CRB,
	CRB:CSC_ME_P4_PC2_LV_CRB,
	CRB:CSC_ME_P4_PC3_LV_CRB,
	CRB:CSC_ME_P4_PC4_LV_CRB,
	CRB:CSC_ME_P4_PC5_LV_CRB,
	CRB:CSC_ME_P4_PC6_LV_CRB }

class: CSC_ME_LV_CR1_PSUTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS

object: CSC_ME_LV_CR1_PSU is_of_class CSC_ME_LV_CR1_PSUTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_LV_CR1_PSUFwElmbPSUBranch_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS
            move_to READY

object: CSC_ME_LV_CR1_PSUFwElmbPSUBranch_FWDM is_of_class CSC_ME_LV_CR1_PSUFwElmbPSUBranch_FwDevMode_CLASS


class: CSC_ME_LV_CR1_PSUFwElmbPSUBranch_CLASS/associated
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

object: EPSU:Crate00:Branch00 is_of_class CSC_ME_LV_CR1_PSUFwElmbPSUBranch_CLASS

object: EPSU:Crate00:Branch01 is_of_class CSC_ME_LV_CR1_PSUFwElmbPSUBranch_CLASS

object: EPSU:Crate00:Branch02 is_of_class CSC_ME_LV_CR1_PSUFwElmbPSUBranch_CLASS

object: EPSU:Crate00:Branch03 is_of_class CSC_ME_LV_CR1_PSUFwElmbPSUBranch_CLASS

objectset: CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETSTATES {EPSU:Crate00:Branch00,
	EPSU:Crate00:Branch01,
	EPSU:Crate00:Branch02,
	EPSU:Crate00:Branch03 }
objectset: CSC_ME_LV_CR1_PSUFWELMBPSUBRANCH_FWSETACTIONS {EPSU:Crate00:Branch00,
	EPSU:Crate00:Branch01,
	EPSU:Crate00:Branch02,
	EPSU:Crate00:Branch03 }

class: CSC_ME_LV_PSUTOP_CSC_LV_TREE_NODES_CLASS
!panel: CSC_LV_TREE_NODES.pnl
    state: OFF	!color: FwStateOKNotPhysics
        when ( ( any_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
                when ( ( all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
        action: ON	!visible: 1
            do ON all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
    state: ERROR	!color: FwStateAttention3
                when ( ( all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state ON ) )  move_to ON
                when ( ( all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state {ON,OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
        action: ON	!visible: 1
            do ON all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
    state: ON	!color: FwStateOKPhysics
        when ( ( any_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state {ERROR,DEAD} ) ) move_to ERROR
           when ( ( any_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES in_state {OFF,RAMP,STANDBY} ) )  move_to OFF
        action: OFF	!visible: 1
            do OFF all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
        action: STANDBY	!visible: 1
            do ON all_in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS

object: CSC_ME_LV_PSU is_of_class CSC_ME_LV_PSUTOP_CSC_LV_TREE_NODES_CLASS

class: CSC_ME_LV_PSUCSC_LV_TREE_NODES_FwDevMode_CLASS
    state: READY
        action: Disable(Device)
            remove &VAL_OF_Device from CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES
            remove &VAL_OF_Device from CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY
        action: Enable(Device)
            insert &VAL_OF_Device in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES
            insert &VAL_OF_Device in CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS
            move_to READY

object: CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWDM is_of_class CSC_ME_LV_PSUCSC_LV_TREE_NODES_FwDevMode_CLASS


objectset: CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_LV_CR1_PSU }
objectset: CSC_ME_LV_PSUCSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_LV_CR1_PSU }

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

object: CMS_CSC_LV_CAN1 is_of_class TOP_CSC_LV_NODES_CLASS

class: FwChildrenMode_CLASS
!panel: FwChildrenMode.pnl
    state: Complete	!color: _3DFace
        when ( ( any_in FWDEVMODE_FWSETSTATES in_state DISABLED ) )  move_to IncompleteDev
    state: Incomplete	!color: FwStateAttention2
    state: IncompleteDev	!color: FwStateAttention1
        when (  ( ( all_in FWDEVMODE_FWSETSTATES not_in_state DISABLED ) )  ) move_to Complete

object: CMS_CSC_LV_CAN1_FWCNM is_of_class FwChildrenMode_CLASS

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

object: CMS_CSC_LV_CAN1_FWM is_of_class FwMode_CLASS

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


objectset: CSC_LV_TREE_NODES_FWSETSTATES {CSC_ME_LV_PSU,
	CSC_ME_M1_LV_CRB,
	CSC_ME_M2_LV_CRB,
	CSC_ME_M3_LV_CRB,
	CSC_ME_M4_LV_CRB,
	CSC_ME_P1_LV_CRB,
	CSC_ME_P2_LV_CRB,
	CSC_ME_P3_LV_CRB,
	CSC_ME_P4_LV_CRB }
objectset: CSC_LV_TREE_NODES_FWSETACTIONS {CSC_ME_LV_PSU,
	CSC_ME_M1_LV_CRB,
	CSC_ME_M2_LV_CRB,
	CSC_ME_M3_LV_CRB,
	CSC_ME_M4_LV_CRB,
	CSC_ME_P1_LV_CRB,
	CSC_ME_P2_LV_CRB,
	CSC_ME_P3_LV_CRB,
	CSC_ME_P4_LV_CRB }

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

object: CMS_CSC_LV_CAN1:CSC_ME_LV_CR1_PSU_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_LV_PSU_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_M1_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_M2_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_M3_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_M4_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_P1_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_P2_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_P3_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1:CSC_ME_P4_LV_CRB_FWDM is_of_class FwDevMode_CLASS

object: CMS_CSC_LV_CAN1_FWDM is_of_class FwDevMode_CLASS

objectset: FWDEVMODE_FWSETSTATES {CMS_CSC_LV_CAN1:CSC_ME_LV_CR1_PSU_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_LV_PSU_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M1_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M2_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M3_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M4_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P1_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P2_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P3_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P4_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1_FWDM }
objectset: FWDEVMODE_FWSETACTIONS {CMS_CSC_LV_CAN1:CSC_ME_LV_CR1_PSU_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_LV_PSU_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M1_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M2_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M3_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_M4_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P1_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P2_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P3_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1:CSC_ME_P4_LV_CRB_FWDM,
	CMS_CSC_LV_CAN1_FWDM }

