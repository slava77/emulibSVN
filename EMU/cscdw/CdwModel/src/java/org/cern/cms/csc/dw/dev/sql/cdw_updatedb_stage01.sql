select CDW_UTILITY.PARTITION_TABLE_DDL(table_name, 'FCT_TIME', 6) "--partition tables" from CDW_FACT_MD join USER_TABLES on (table_name = fmd_table_name and partitioned = 'NO')
/

