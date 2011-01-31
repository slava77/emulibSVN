package org.cern.cms.csc.dw.dao.table;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.util.SQLParamRestriction;
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

public abstract class BeanTableDao implements BeanTableDaoIf {

    private static final Logger logger = Logger.getLogger(BeanTableDao.class.getName());

    protected abstract Session getSession();

    @Override
    public List<EntityBase> getData(BeanTableIf table) {

        Session session = getSession();
        Transaction transaction = session.beginTransaction();

        Criteria c = getCriteria(session, table);

        for (BeanTableColumnIf sc: table.getSortingColumns().getTarget()) {
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
            logger.log(Level.SEVERE, ex.getMessage(), ex);
            //if (table.isInteractiveMode()) {
                //BeanBase.addErrorMessage("cms.dqm.workflow.getData.ERROR", false);
            //}

        }

        transaction.rollback();

        return list;
    }

    @Override
    public Long getDataCount(BeanTableIf table) {

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

    private Criteria getCriteria(Session session, BeanTableIf table) {
        return getDetachedCriteria(table).getExecutableCriteria(session);
    }

    private Criteria getCriteria(Session session, Class rowClass, List<BeanTableColumnIf> columns) {
        return getDetachedCriteria(rowClass, columns, null).getExecutableCriteria(session);
    }

    private DetachedCriteria getDetachedCriteria(BeanTableIf table) {
        return getDetachedCriteria(table.getRowClass(), table.getColumns(), null); //table.getAdvancedQuery());
    }

    private DetachedCriteria getDetachedCriteria(Class rowClass, List<BeanTableColumnIf> columns, String advancedQuery) {

        DetachedCriteria c = DetachedCriteria.forClass(rowClass);

        if (advancedQuery != null) {
            c.add(SQLParamRestriction.restriction(advancedQuery));
        }

        for (BeanTableColumnIf col: columns) {

            if (col.isFilterSet()) {

                BeanTableFilterIf f = col.getFilter();
                String propertyName = col.getName();

                if (f.getItems().size() > 0) {

                    Junction disJun = Restrictions.disjunction();
                    Junction conJun = Restrictions.conjunction();
                    Junction curJun = disJun;

                    for (Iterator<BeanTableFilterItemIf> filterItemItr = f.getItems().iterator(); filterItemItr.hasNext();) {

                        BeanTableFilterItemIf item = filterItemItr.next();

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
                            case IN:
                                if (item instanceof BeanTableProjectionFilterItemIf) {
                                    BeanTableProjectionFilterItemIf pitem = (BeanTableProjectionFilterItemIf) item;
                                    BeanTableIf subQueryTable = pitem.getTablePack().getTable();
                                    if (subQueryTable != null) {
                                        if (subQueryTable.isFilterOn() || subQueryTable.isAdvancedQuerySet()) {
                                            DetachedCriteria subCriteria = getDetachedCriteria(subQueryTable);
                                            curJun.add(Subqueries.propertyIn(propertyName, subCriteria));
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

    /*
    public String getCriteriaFilterWhereClause(BeanTableIf table) throws NoSuchFieldException {

        StringBuilder sql = new StringBuilder();
        Class rowClass = table.getRowClass();

        for (Iterator<String> filterItr = table.getFilter().keySet().iterator(); filterItr.hasNext();) {

            String k = filterItr.next();
            BeanTableFilterIf f = table.getFilter().get(k);

            if (f.getItems().size() > 0) {

                filterKeyMatcher.reset(k);
                k = filterKeyMatcher.replaceAll("");

                StringBuffer subsql = new StringBuffer();

                for (Iterator<BeanTableFilterItemIf> filterItemItr = f.getItems().iterator(); filterItemItr.hasNext();) {
                    BeanTableFilterItemIf item = filterItemItr.next();
                    switch (item.getOperator()) {
                        case AND:
                            if (subsql.length() > 0) {
                                subsql.append(" AND ");
                            }
                            break;
                        case OR:
                            if (subsql.length() > 0) {
                                subsql.append(" OR ");
                            }
                            break;
                    }

                    Field field = rowClass.getDeclaredField(k);
                    Column fieldColumn = field.getAnnotation(Column.class);
                    subsql.append(fieldColumn.name());

                    boolean skipValue = false;

                    switch (item.getOperation()) {
                        case NOTLIKE:
                            subsql.append(" NOT LIKE ");
                            break;
                        case ISNULL:
                            subsql.append(" IS NULL ");
                            skipValue = true;
                            break;
                        case ISNOTNULL:
                            subsql.append(" IS NOT NULL ");
                            skipValue = true;
                            break;
                        default:
                            subsql.append(" ")
                                  .append(item.getOperation().getValue())
                                  .append(" ");
                    }

                    if (!skipValue) {

                        Class fieldType = field.getType();

                        if (fieldType.equals(String.class)) {
                            String v = (String) item.getValue();
                            subsql.append("'");
                            subsql.append(v.replaceAll("'", "''"));
                            subsql.append("'");
                        } else {
                            if (fieldType.equals(Boolean.class)) {
                                Boolean v = (Boolean) item.getValue();
                                subsql.append(v ? 1 : 0);
                            } else {
                                subsql.append(item.getValue());
                            }
                        }
                    }

                }

                if (sql.length() > 0) {
                    sql.append(" AND ");
                }
                sql.append("(");
                sql.append(subsql);
                sql.append(")");

            }

        }

        return sql.toString();
    }
    */

}
