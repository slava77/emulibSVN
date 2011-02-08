package jsf.bean.gui.component.table;

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

public abstract class BeanTableDao {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDao.class);

    protected abstract Session getSession();

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

        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());

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

        Long count = (Long) getCriteria(session, table)
                .setProjection(Projections.rowCount())
                .setCacheable(true)
                .setCacheRegion(table.getRowClass()
                .getCanonicalName())
                .uniqueResult();

        transaction.rollback();

        return count;
    }

    private Criteria getCriteria(Session session, BeanTable table) {
        return getDetachedCriteria(table).getExecutableCriteria(session);
    }

    private Criteria getCriteria(Session session, Class rowClass, List<BeanTableColumn> columns) {
        return getDetachedCriteria(rowClass, columns).getExecutableCriteria(session);
    }

    private DetachedCriteria getDetachedCriteria(BeanTable table) {
        return getDetachedCriteria(table.getRowClass(), table.getColumns()); //table.getAdvancedQuery());
    }

    private DetachedCriteria getDetachedCriteria(Class rowClass, List<BeanTableColumn> columns) {

        DetachedCriteria c = DetachedCriteria.forClass(rowClass);

        /*
        if (advancedQuery != null) {
            c.add(SQLParamRestriction.restriction(advancedQuery));
        }
        */

        for (BeanTableColumn col: columns) {

            if (col.isFilterSet()) {

                BeanTableFilter f = col.getFilter();
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
                                            if (item.getOperation().equals(BeanTableFilter.Operation.IN)) {
                                                curJun.add(Subqueries.propertyIn(propertyName, subCriteria));
                                            } else if (item.getOperation().equals(BeanTableFilter.Operation.NOTIN)) {
                                                curJun.add(Subqueries.propertyNotIn(propertyName, subCriteria));
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

        return c;

    }

}
