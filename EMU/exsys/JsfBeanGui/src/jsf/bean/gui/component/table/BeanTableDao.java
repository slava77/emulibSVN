package jsf.bean.gui.component.table;

import jsf.bean.gui.component.table.column.BeanTableColumn;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.column.BeanTableColumnBase;
import jsf.bean.gui.component.table.column.BeanTableColumnEmbedded;
import jsf.bean.gui.component.table.column.BeanTableColumnSortable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.hibernate.Criteria;
import org.hibernate.QueryException;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Junction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Projections;
import org.hibernate.criterion.Restrictions;
import org.hibernate.criterion.Subqueries;

public abstract class BeanTableDao implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDao.class);
    private static final Integer MAX_IN_ELEMENTS = 1000;
    private static final String ALIAS_PREFIX = "a";

    protected abstract Session getSession();

    /**
     * Method is being called right before executing criteria.
     * Default method adds cache. Override to change!
     * @param table Table
     * @param c Criteria to be executed
     */
    protected void preExecute(BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }

    /**
     * Method is being called right before executing count criteria.
     * Default method adds cache. Override to change!
     * @param table Table
     * @param c Criteria to be executed
     */
    protected void preExecuteCount(BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }

    public List<EntityBeanBase> getData(BeanTable table) {

        return getData(table,
                table.getPageSize(),
                table.getPageIndex());
    }

    public List<EntityBeanBase> getData(BeanTable table,
            int pageSize,
            int pageIndex) {

        List data = new ArrayList();
        List pageIds = null;

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        if (pageSize > 0 && pageSize <= MAX_IN_ELEMENTS) {
            Criteria c = getDetachedCriteria(table)
                                .getExecutableCriteria(session)
                                .setProjection(Projections.id())
                                .setFirstResult((pageIndex - 1) * pageSize)
                                .setMaxResults(pageSize);
            applyOrder(c, table);
            pageIds = c.list();
        }

        if (pageIds == null || !pageIds.isEmpty()) {

            Criteria c;

            if (pageIds != null) {
                c = session.createCriteria(table.getRowClass());
                String itemId = EntityBeanBase.getIdPropertyMd(table.getRowClass()).getName();
                c.add(Restrictions.in(itemId, pageIds));
            } else {
                c = getDetachedCriteria(table).getExecutableCriteria(session);
            }

            applyOrder(c, table);

            try {
                preExecute(table, c);
                c.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);
                data = c.list();
            } catch (QueryException ex) {
                logger.error(ex);
                //if (table.isInteractiveMode()) {
                //BeanBase.addErrorMessage("cms.dqm.workflow.getData.ERROR", false);
                //}

            }
        }

        transaction.rollback();

        return data;
    }

    public Long getDataCount(BeanTable table) {

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        Criteria c = getDetachedCriteria(table)
                            .getExecutableCriteria(session)
                            .setProjection(Projections.rowCount());

        preExecuteCount(table, c);
        c.setResultTransformer(Criteria.DISTINCT_ROOT_ENTITY);

        Long count = (Long) c.uniqueResult();
        transaction.rollback();

        return count;
    }

    private void applyOrder(Criteria c, BeanTable table) {
        for (BeanTableColumnSortable sc : table.getSortingColumns().getTarget()) {
            if (sc.isAscending()) {
                c.addOrder(Order.asc(sc.getSortName()));
            } else {
                c.addOrder(Order.desc(sc.getSortName()));
            }
        }
    }

    private DetachedCriteria getDetachedCriteria(BeanTable table) {

        DetachedCriteria c = DetachedCriteria.forClass(table.getRowClass());
        Integer aliasNr = 0;

        /*
        if (advancedQuery != null) {
        c.add(SQLParamRestriction.restriction(advancedQuery));
        }
         */

        if (table.getPack().getFilters() != null) {
            for (BeanTablePackFilter pf : table.getPack().getFilters()) {
                if (pf instanceof BeanTableListFilter) {
                    BeanTableListFilter lf = (BeanTableListFilter) pf;

                    Class<? extends EntityBeanBase> parentType = lf.getParentType();
                    String parentPropertyName = lf.getPropertyName();
                    EntityBeanBase parent = lf.getParent();
                    String parentId = EntityBeanBase.getIdPropertyMd(parentType).getName();
                    String myId = EntityBeanBase.getIdPropertyMd(table.getRowClass()).getName();

                    String alias = ALIAS_PREFIX.concat(String.valueOf(aliasNr++));
                    DetachedCriteria subCriteria = DetachedCriteria.forClass(parentType);
                    subCriteria.createAlias(parentPropertyName, alias);
                    subCriteria.setProjection(Projections.property(alias.concat(myId)));
                    subCriteria.add(Restrictions.eq(parentId, parent.getEntityId()));

                    c.add(Subqueries.propertyIn(myId, subCriteria));

                }
            }
        }

        for (String cname: table.getPack().getPropertyFilters().keySet()) {
            BeanTableColumn col = table.getColumn(cname);
            if (col != null) {
                aliasNr = applyColumnFilter(c, col, table.getPack().getPropertyFilters().get(cname), aliasNr);
            }
        }

        for (BeanTableColumn col : table.getColumns()) {
            if (col.isFilterSet()) {
                if (col instanceof BeanTableColumnEmbedded) {
                    for (BeanTableColumnBase ecol: ((BeanTableColumnEmbedded) col).getProperties()) {
                        if (ecol.isFilterSet()) {
                            aliasNr = applyColumnFilter(c, ecol, ecol.getFilter(), aliasNr);
                        }
                    }
                } else {
                    BeanTableFilter f = col.getFilter();
                    aliasNr = applyColumnFilter(c, col, f, aliasNr);
                }
            }
        }

        return c;

    }

    private Integer applyColumnFilter(DetachedCriteria c, BeanTableColumnBase col, BeanTableFilter f, Integer aliasNr) {

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
                                    subCriteria.setProjection(Projections.id());
                                    if (col.isListType()) {
                                        String listItemId = EntityBeanBase.getIdPropertyMd(subQueryTable.getRowClass()).getName();
                                        DetachedCriteria listCriteria = c.createCriteria(propertyName, ALIAS_PREFIX.concat(String.valueOf(aliasNr++)));
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

        return aliasNr;

    }

}
