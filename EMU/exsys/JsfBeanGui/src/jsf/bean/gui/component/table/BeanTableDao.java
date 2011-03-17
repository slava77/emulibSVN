package jsf.bean.gui.component.table;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
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

    protected abstract Session getSession();

    /**
     * Method is being called right before executing criteria.
     * Default method adds cache. Override to change!
     * @param table Table used to construct criteria
     * @param c Criteria to be executed
     */
    protected void preExecute(BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }

    /**
     * Method is being called right before executing count criteria.
     * Default method adds cache. Override to change!
     * @param table Table used to construct criteria
     * @param c Criteria to be executed
     */
    protected void preExecuteCount(BeanTable table, Criteria c) {
        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());
    }

    public List<EntityBeanBase> getData(BeanTable table) {

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        Criteria c = getCriteria(session, table);

        for (BeanTableColumn sc: table.getSortingColumns().getTarget()) {
            if (sc.isAscending()) {
                c.addOrder(Order.asc(sc.getName()));
            } else {
                c.addOrder(Order.desc(sc.getName()));
            }
        }

        if (table.getPageSize() > 0) {
            c.setFirstResult((table.getPageIndex() - 1) * table.getPageSize());
            c.setMaxResults(table.getPageSize());
        }

        preExecute(table, c);

        List list = new ArrayList();
        try {

            list = c.list();

        } catch (QueryException ex) {
            logger.error(ex);
            //if (table.isInteractiveMode()) {
                //BeanBase.addErrorMessage("cms.dqm.workflow.getData.ERROR", false);
            //}

        }

        transaction.rollback();

        return list;
    }

    public Long getDataCount(BeanTable table) {

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        Criteria c = getCriteria(session, table).setProjection(Projections.rowCount());

        preExecuteCount(table, c);

        Long count = (Long) c.uniqueResult();
        transaction.rollback();

        return count;
    }

    private Criteria getCriteria(Session session, BeanTable table) {
        return getDetachedCriteria(table).getExecutableCriteria(session);
    }

    private DetachedCriteria getDetachedCriteria(BeanTable table) {

        DetachedCriteria c = DetachedCriteria.forClass(table.getRowClass());

        /*
        if (advancedQuery != null) {
            c.add(SQLParamRestriction.restriction(advancedQuery));
        }
        */

        if (table.getPack().getFilters() != null) {
            for (BeanTablePackFilter pf: table.getPack().getFilters()) {
                if (pf instanceof BeanTableListFilter) {
                    BeanTableListFilter lf = (BeanTableListFilter) pf;

                    Class<? extends EntityBeanBase> parentType = lf.getParentType();
                    String parentPropertyName = lf.getPropertyName();
                    EntityBeanBase parent = lf.getParent();
                    String parentId = EntityBeanBase.getIdPropertyMd(parentType).getName();
                    String myId = EntityBeanBase.getIdPropertyMd(table.getRowClass()).getName();

                    DetachedCriteria subCriteria = DetachedCriteria.forClass(parentType);
                    subCriteria.createAlias(parentPropertyName, "p");
                    subCriteria.setProjection(Projections.property("p.".concat(myId)));
                    subCriteria.add(Restrictions.eq(parentId, parent.getEntityId()));

                    c.add(Subqueries.propertyIn(myId, subCriteria));

                }
            }
        }

        for (BeanTableColumn col: table.getColumns()) {

            if (col.isFilterSet()) {
                BeanTableFilter f = col.getFilter();
                applyColumnFilter(c, col, f);
            }
            
            if (table.getPack().getPropertyFilters().containsKey(col.getName())) {
                applyColumnFilter(c, col, table.getPack().getPropertyFilters().get(col.getName()));
            }
        }

        return c;

    }

    private void applyColumnFilter(DetachedCriteria c, BeanTableColumn col, BeanTableFilter f) {

        String propertyName = col.getName();

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
                                        DetachedCriteria listCriteria = c.createCriteria(propertyName, "c");
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

}
