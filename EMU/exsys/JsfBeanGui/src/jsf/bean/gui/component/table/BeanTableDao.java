package jsf.bean.gui.component.table;

import jsf.bean.gui.component.table.column.BeanTableColumn;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.column.BeanTableColumnBase;
import jsf.bean.gui.component.table.column.BeanTableColumnEmbedded;
import jsf.bean.gui.component.table.column.BeanTableColumnEntity;
import jsf.bean.gui.component.table.column.BeanTableColumnSortable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.hibernate.Criteria;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Junction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Projections;
import org.hibernate.criterion.Restrictions;
import org.hibernate.criterion.Subqueries;

public abstract class BeanTableDao implements Serializable, BeanTableDaoIf {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDao.class);
    private static final Integer MAX_IN_ELEMENTS = 1000;

    protected abstract Session getSession();

    /**
     * Method is being called right before executing criteria.
     * Default method adds cache. Override to change!
     * @param table Table
     * @param c Criteria to be executed
     */
    protected void preExecute(Session session, Transaction transaction, BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }

    /**
     * Method is being called right before executing count criteria.
     * Default method adds cache. Override to change!
     * @param table Table
     * @param c Criteria to be executed
     */
    protected void preExecuteCount(Session session, Transaction transaction, BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }
    
    /**
     * Check if this table (i.e. rowClass) has a special Id criteria treatment?
     * If this returns true than latter customIdCriteria will be called and 
     * default criterias will not be used.
     * @param table Source table
     * @return true if custom criteria has implementation, false otherwise
     */
    protected boolean hasCustomPageIdCriteria(BeanTable table) {
        return false;
    }

    /**
     * Apply custom criteria
     * @param table Source table
     * @param c Criteria to apply ids to
     * @param pageIds List of page ids
     */
    public void applyCustomPageIdCriteria(BeanTable table, Criteria c, List pageIds) { }
    
    /**
     * Get list of data for the table
     * @param table Source table
     * @return List of data
     */
    public List<EntityBeanBase> getData(BeanTable table) {

        return getData(table,
                table.getPageSize(),
                table.getPageIndex());
    }

    /**
     * Get list of page data for table 
     * @param table Source table
     * @param pageSize Page size
     * @param pageIndex Page index
     * @return List of page data
     */
    @SuppressWarnings("unchecked")
    public List<EntityBeanBase> getData(BeanTable table,
            int pageSize,
            int pageIndex) {

        List<EntityBeanBase> data = new ArrayList<EntityBeanBase>();

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        try {
        
            List pageIds = null;
            
            if (pageSize > 0 && pageSize <= MAX_IN_ELEMENTS) {
                Criteria c = getDetachedCriteria(table)
                                    .getExecutableCriteria(session)
                                    .setProjection(Projections.id())
                                    .setFirstResult((pageIndex - 1) * pageSize)
                                    .setMaxResults(pageSize);
                applyOrder(c, table);
                preExecute(session, transaction, table, c);
                Long sTime = System.nanoTime();
                pageIds = c.list();
                Long eTime = System.nanoTime();
                System.out.println("Elapsed: " + (eTime - sTime) / 1000000 + " ms.");
            }

            if (pageIds == null || !pageIds.isEmpty()) {

                Criteria c;

                if (pageIds != null) {
                    c = session.createCriteria(table.getRowClass());
                    if (hasCustomPageIdCriteria(table)) {
                        applyCustomPageIdCriteria(table, c, pageIds);
                    } else {
                        String itemId = EntityBeanBase.getIdPropertyMd(table.getRowClass()).getName();
                        c.add(Restrictions.in(itemId, pageIds));
                    }
                } else {
                    c = getDetachedCriteria(table).getExecutableCriteria(session);
                }

                applyOrder(c, table);

                preExecute(session, transaction, table, c);
                c.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);
                Long sTime = System.nanoTime();
                data = c.list();
                Long eTime = System.nanoTime();
                System.out.println("Elapsed: " + (eTime - sTime) / 1000000 + " ms.");

            }
            
        } finally {
            transaction.rollback();
        }

        return data;
    }

    /**
     * Get data size on table
     * @param table Source table
     * @return number of rows
     */
    public Long getDataCount(BeanTable table) {
        
        Long count = 0L;
        Session session = getSession();
        Transaction transaction = session.beginTransaction();
        
        try {
            
            Criteria c = getDetachedCriteria(table)
                                .getExecutableCriteria(session)
                                .setProjection(Projections.rowCount());

            preExecuteCount(session, transaction, table, c);
            c.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);

            Long sTime = System.nanoTime();
            count = (Long) c.uniqueResult();
            Long eTime = System.nanoTime();
            System.out.println("Elapsed: " + (eTime - sTime) / 1000000 + " ms.");
            
        } finally {
            transaction.rollback();
        }

        return count;
    }

    /**
     * Apply order instructions on criteria
     * @param c Criteria
     * @param table Source table
     */
    private void applyOrder(Criteria c, BeanTable table) {
        for (BeanTableColumnSortable sc : table.getSortingColumns().getTarget()) {
            if (sc.isAscending()) {
                c.addOrder(Order.asc(sc.getSortName()));
            } else {
                c.addOrder(Order.desc(sc.getSortName()));
            }
        }
    }

    /**
     * Get detached (db session free) criteria
     * @param table Source table
     * @return Criteria
     */
    public DetachedCriteria getDetachedCriteria(BeanTable table) {

        DetachedCriteria c = DetachedCriteria.forClass(table.getRowClass());
        CriteriaConfig config = new CriteriaConfig();

        if (table.isQueryApplied()) {
            c.add(SQLParamRestriction.restriction(table.getAppliedQuery()));
        }

        if (table.getPack().getFilters() != null) {
            for (BeanTablePackFilter pf : table.getPack().getFilters()) {
                if (pf instanceof BeanTableListFilter) {
                    BeanTableListFilter lf = (BeanTableListFilter) pf;

                    Class<? extends EntityBeanBase> parentType = lf.getParentType();
                    String parentPropertyName = lf.getPropertyName();
                    EntityBeanBase parent = lf.getParent();
                    String parentId = EntityBeanBase.getIdPropertyMd(parentType).getName();
                    String myId = EntityBeanBase.getIdPropertyMd(table.getRowClass()).getName();

                    DetachedCriteria subCriteria = DetachedCriteria.forClass(parentType);
                    subCriteria.createAlias(parentPropertyName, config.nextAlias());
                    subCriteria.setProjection(Projections.property(config.sameAlias().concat(".").concat(myId)));
                    subCriteria.add(Restrictions.eq(parentId, parent.getEntityId()));

                    c.add(Subqueries.propertyIn(myId, subCriteria));

                }
            }
        }

        for (String cname: table.getPack().getPropertyFilters().keySet()) {
            BeanTableColumn col = table.getColumn(cname);
            if (col != null) {
                applyColumnFilter(c, col, table.getPack().getPropertyFilters().get(cname), config);
            }
        }

        for (BeanTableColumn col : table.getColumns()) {
            if (col.isFilterSet()) {
                if (col instanceof BeanTableColumnEmbedded) {
                    for (BeanTableColumnBase ecol: ((BeanTableColumnEmbedded) col).getProperties()) {
                        if (ecol.isFilterSet()) {
                            applyColumnFilter(c, ecol, ecol.getFilter(), config);
                        }
                    }
                } else {
                    BeanTableFilter f = col.getFilter();
                    applyColumnFilter(c, col, f, config);
                }
            }
        }

        return c;

    }

    /**
     * Apply single column filter(s) on detached criteria
     * @param c Criteria to apply filters upon
     * @param col Source column 
     * @param f Filter
     * @param config Criteria configuration
     */
    private void applyColumnFilter(DetachedCriteria c, BeanTableColumnBase col, BeanTableFilter f, CriteriaConfig config) {

        String propertyName = col.getFilterName();

        if (f.getItems().size() > 0) {

            Junction disJun = Restrictions.disjunction();
            Junction conJun = Restrictions.conjunction();
            Junction curJun = disJun;

            for (Iterator<BeanTableFilterItem> filterItemItr = f.getItems().iterator(); filterItemItr.hasNext();) {

                BeanTableFilterItem item = filterItemItr.next();

                switch (item.getOperator()) {
                    case AND:
                        curJun = conJun;
                        break;
                    case OR:
                        curJun = disJun;
                        break;
                }

                switch (item.getOperation()) {
                    case EQUAL:
                        curJun.add(Restrictions.eq(propertyName, item.getValue()));
                        break;
                    case MORE:
                        curJun.add(Restrictions.gt(propertyName, item.getValue()));
                        break;
                    case LESS:
                        curJun.add(Restrictions.lt(propertyName, item.getValue()));
                        break;
                    case MORE_EQUAL:
                        curJun.add(Restrictions.ge(propertyName, item.getValue()));
                        break;
                    case LESS_EQUAL:
                        curJun.add(Restrictions.le(propertyName, item.getValue()));
                        break;
                    case NOT_EQUAL:
                        curJun.add(Restrictions.ne(propertyName, item.getValue()));
                        break;
                    case LIKE:
                        curJun.add(Restrictions.like(propertyName, item.getValue()));
                        break;
                    case NOTLIKE:
                        curJun.add(Restrictions.not(Restrictions.like(propertyName, item.getValue())));
                        break;
                    case ISNULL:
                        curJun.add(Restrictions.isNull(propertyName));
                        break;
                    case ISNOTNULL:
                        curJun.add(Restrictions.isNotNull(propertyName));
                        break;
                    default:
                        if (item instanceof BeanTableProjectionFilterItem) {
                            BeanTableProjectionFilterItem pitem = (BeanTableProjectionFilterItem) item;
                            BeanTablePack pack = pitem.getTablePack();
                            BeanTable subQueryTable = pack.getTable();
                            if (subQueryTable != null) {
                                if (subQueryTable.isFilterOn() || !pack.isSingleClass()) {
                                    DetachedCriteria subCriteria = getDetachedCriteria(subQueryTable);
                                    String referencedProperty = ((BeanTableColumnEntity) col).getReferencedProperty();
                                    subCriteria.setProjection(referencedProperty != null ? Projections.property(referencedProperty) : Projections.id());
                                    if (col.isListType()) {
                                        String listItemId = EntityBeanBase.getIdPropertyMd(subQueryTable.getRowClass()).getName();
                                        DetachedCriteria listCriteria = c.createCriteria(propertyName, config.nextAlias());
                                        if (item.getOperation().equals(BeanTableFilter.Operation.IN)) {
                                            listCriteria.add(Subqueries.propertyIn(listItemId, subCriteria));
                                        } else if (item.getOperation().equals(BeanTableFilter.Operation.NOTIN)) {
                                            listCriteria.add(Subqueries.propertyNotIn(listItemId, subCriteria));
                                        }

                                    } else {

                                        if (item.getOperation().equals(BeanTableFilter.Operation.IN)) {
                                            curJun.add(Subqueries.propertyIn(propertyName, subCriteria));
                                        } else if (item.getOperation().equals(BeanTableFilter.Operation.NOTIN)) {
                                            curJun.add(Subqueries.propertyNotIn(propertyName, subCriteria));
                                        }

                                    }
                                }
                            }
                        }
                        break;
                }

            }

            c.add(disJun).add(conJun);

        }

    }

    /**
     * Criteria configuration class which 
     * holds and manages criteria aliases
     */
    private class CriteriaConfig {

        private static final String ALIAS_PREFIX = "a";
        private Integer aliasNo = 0;

        /**
         * Next column alias
         * @return Alias
         */
        public String nextAlias() {
            aliasNo++;
            return sameAlias();
        }

        /**
         * Current alias
         * @return Alias
         */
        public String sameAlias() {
            return ALIAS_PREFIX.concat(String.valueOf(aliasNo));
        }

    }

}
